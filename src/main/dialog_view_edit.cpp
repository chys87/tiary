// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#include "main/dialog_view_edit.h"
#include "diary/diary.h"
#include "common/algorithm.h"
#include "common/unicode.h"
#include "common/split_line.h"
#include "ui/window.h"
#include "ui/dialog_message.h"
#include "common/external.h"
#include "ui/dialog_input.h"
#include "ui/dialog_richtext.h"
#include "common/string.h"
#include "common/datetime.h"
#include "ui/paletteid.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <functional>
#include <errno.h>
#include <sys/wait.h>

namespace tiary {

namespace {

using namespace ui;

const unsigned edit_line_width = 78;
const unsigned view_line_width = 78;

void write_for_view (RichTextList &lst, const DiaryEntry &ent, const std::wstring &longtime_format)
{
	lst.push_back (RichTextLine (PALETTE_ID_SHOW_BOLD, std::wstring (view_line_width, L'=')));
	lst.push_back (RichTextLine (PALETTE_ID_SHOW_BOLD, ent.title));
	lst.push_back (RichTextLine (PALETTE_ID_SHOW_BOLD, std::wstring (view_line_width, L'=')));
	lst.push_back (RichTextLine (PALETTE_ID_SHOW_NORMAL, ent.local_time.format (longtime_format)));
	if (!ent.labels.empty ()) {
		std::wstring labelstr = L"Labels: " + join (ent.labels.begin (), ent.labels.end (), L", ");
		lst.push_back (RichTextLine (PALETTE_ID_SHOW_NORMAL, labelstr));
	}
	lst.push_back (RichTextLine (PALETTE_ID_SHOW_NORMAL, std::wstring ()));

	// Text
	SplitStringLineList split_list = split_line (edit_line_width, ent.text);
	for (SplitStringLineList::const_iterator it = split_list.begin ();
			it != split_list.end (); ++it) {
		lst.push_back (RichTextLine (PALETTE_ID_SHOW_NORMAL,
					ent.text.substr (it->begin, it->len)));
	}
}

// Write the title and text to a file which will be passed to an editor
bool write_for_edit (int fd, const std::wstring &title, const std::wstring &text)
{
	std::string mbs = wstring_to_mbs (title);
	mbs.reserve (text.length () * 2);
	mbs.append (2, '\n');
	SplitStringLineList split_list = split_line (edit_line_width, text);
	for (SplitStringLineList::const_iterator it = split_list.begin ();
			it != split_list.end (); ++it) {
		mbs += wstring_to_mbs (text.data() + it->begin, it->len);
		mbs += '\n';
	}
	return (size_t)write (fd, mbs.data (), mbs.length ()) == mbs.length ();
}

// The argument points to a memory-mapped area that maps the edited file.
// Read its contents, removing unnecessary newline characters
// (add spaces when necessary)
// The first line is converted to the title
void reformat_content (std::wstring &title, std::wstring &text, const char *s)
{
	// Be careful!
	// Some implementations may use COW in std::basic_string
	// or not always keeping a null character at the end
	// (The Standard does not specify whether *end() returns L'\0' or
	// triggers an exception. Avoid that.)
	// Make sure this function works with those implementations too.

	// First line = title
	const char *firstnewline = strchrnul (s, '\n');
	title = mbs_to_wstring (std::string (s, firstnewline));

	if (*firstnewline != '\n') {
		// Only one line??
		text.clear ();
		return;
	}
	s = firstnewline;
	if (*s == '\n') {
		++s;
	}

	text = mbs_to_wstring (s);
	std::wstring::iterator iw = text.begin ();
	std::wstring::const_iterator ir = iw;
	std::wstring::const_iterator end = c(text).end ();
	wchar_t last = L'\0', cur;
	while (ir != end) {
		cur = *ir++;
		if (cur == L'\n' && (ir == end || (*ir != L' ' && *ir != L'\t'))) {
			if (ir == end) { // Drop newlines at the end
				break;
			}
			if (*ir == L'\n') {
				// More than one contiguous newlines
				int n = 1;
				do {
					++n;
				} while (++ir!=end && *ir == L'\n');
				if (ir == end) { // Drop newlines at the end
					break;
				}
				std::fill_n (iw, n, L'\n');
				iw += n;
			}
			else {
				// A single newline character.
				// Drop it or replace it with a space character
				if (iswgraph (last) && !ucs_iscjk (last) && iswgraph (*ir) && !ucs_iscjk (*ir)) {
					*iw++ = L' ';
				}
			}
		}
		else {
			*iw++ = cur;
		}
		last = cur;
	}
	text.erase (iw, text.end ());
}

bool error_false (const wchar_t *info)
{
	ui::dialog_message (info);
	return false;
}

} // anonymous namespace

bool edit_entry (DiaryEntry &ent, const char *editor)
{
	// Write the contents to a temp file
	// Then invoke an editor
	char temp_file[] = "/tmp/tiary.temp.XXXXXXXXX";
	int fd = mkstemp (temp_file);
	if (fd < 0) {
		return error_false (L"Failed to create a temporary file in /tmp.");
	}
	fchmod (fd, S_IRUSR|S_IWUSR);
#if defined F_GETFD && defined F_SETFD && defined FD_CLOEXEC
	int fdflag = fcntl (fd, F_GETFD);
	if (fdflag >= 0) {
		fcntl (fd, F_SETFD, fdflag|FD_CLOEXEC);
	}
#endif

	// There is no universal method to notify the editor of the encoding;
	// So the best way is to use LC_CTYPE
	if (!write_for_edit (fd, ent.title, ent.text)) {
		close (fd);
		unlink (temp_file);
		return error_false (L"Failed to write to temporary file :( Why?");
	}

	time_t oldmtime;
	struct stat stbuf;
	if (fstat (fd, &stbuf) != 0) {
		close (fd);
		unlink (temp_file);
		return error_false (L"stat on temporary file failed :( Why?");
	}
	oldmtime = stbuf.st_mtime;

	// Call and wait for the editor
	ui::Window::suspend ();
	int editor_status = call_external_program_system (editor, temp_file);
	ui::Window::resume ();

	unlink (temp_file);

	if (editor_status != 0) { // Editor failed.
		close (fd);
		return error_false (L"Editor not found or failed for some reason :(");
	}

	// No change?
	if (fstat (fd, &stbuf) != 0 || oldmtime == stbuf.st_mtime) {
		close (fd);
		return false;
	}

	// Read contents. For convenience, use a memory mapping
	size_t fsize = lseek (fd, 0, SEEK_END);
	if (ssize_t (fsize) < 0 || write (fd, "", 1) != 1) {
		close (fd);
		return error_false (L"Failed to append one byte to temporary file :( Why?");
	}
	++fsize;
	char *raw = (char *) mmap (NULL, fsize, PROT_READ, MAP_PRIVATE
#ifdef MAP_POPULATE
			|MAP_POPULATE
#endif
			, fd, 0);
	close (fd);
	if (raw == MAP_FAILED) {
		return error_false (L"Failed to read new entry :(");
	}
#ifdef MADV_SEQUENTIAL
	madvise (raw, fsize, MADV_SEQUENTIAL);
#endif
	reformat_content (ent.title, ent.text, raw);
	munmap (raw, fsize);
	return true;
}

void view_entry (DiaryEntry &ent, const std::wstring &longtime_format)
{
	RichTextList text_list;
	write_for_view (text_list, ent, longtime_format);
	ui::dialog_richtext (
			ent.title,
			text_list,
			make_size (view_line_width + 3, 0));
}

void view_all_entries (const DiaryEntryList &entries, const std::wstring &longtime_format)
{
	if (entries.empty ()) {
		return;
	}
	ui::RichTextList text_list;
	DiaryEntryList::const_iterator it = entries.begin ();
	for (;;) {
		write_for_view (text_list, **it, longtime_format);
		if (++it == entries.end ()) {
			break;
		}
		text_list.insert (text_list.end (),
				4, ui::RichTextLine (ui::PALETTE_ID_SHOW_NORMAL, std::wstring ()));
	}
	ui::dialog_richtext (
			L"View all entries",
			text_list,
			make_size (view_line_width + 3, 0));
}

} // namespace tairy
