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


#include "diary/diary.h"
#include "common/unicode.h"
#include "ui/window.h"
#include "ui/dialog_message.h"
#include "common/external.h"
#include "ui/dialog_input.h"
#include "common/string.h"
#include "common/datetime.h"
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

// Format text to a human-friendly format
// If tags!=0, also displays the tags
std::string split_text_for_display (const DiaryEntry &ent, bool extra)
{
	const unsigned line_width = 78;
	// Title
	std::string mbs (line_width, '=');
	mbs += '\n';
	mbs += wstring_to_mbs (ent.title);
	mbs += '\n';
	mbs.append (78, '=');

	// Time and tags
	if (extra) {
		mbs += ent.local_time.format ("\n%W %B %d, %Y  %h:%M:%S %P");
		if (!ent.tags.empty ()) {
			mbs += "\nTags: ";
			DiaryEntry::TagList::const_iterator it = ent.tags.begin (), e = ent.tags.end ();
			do {
				mbs += wstring_to_mbs (*it);
				if (++it != e)
					mbs += ", ";
			} while (it != e);
		}
	}

	mbs.append (2, '\n');

	// Text
	const std::wstring &s = ent.text;
	size_t offset = 0;
	SplitStringLine split_info;
	while (offset < s.length ()) {
		offset = split_line (split_info, line_width, s, offset);
		mbs += wstring_to_mbs (s.data () + split_info.begin, split_info.len);
		mbs += '\n';
	}
	return mbs;
}

bool split_write (int fd, const DiaryEntry &ent, bool extra)
{
	std::string mbs = split_text_for_display (ent, extra);
	return (size_t)write (fd, mbs.data (), mbs.length ()) == mbs.length ();
}

bool split_write (FILE *fp, const DiaryEntry &ent, bool extra)
{
	std::string mbs = split_text_for_display (ent, extra);
	return fwrite_unlocked (mbs.data (), 1, mbs.length (), fp) == mbs.length ();
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
	if (*s == '\n')
		++s;

	text = mbs_to_wstring (s);
	std::wstring::iterator iw = text.begin ();
	std::wstring::const_iterator ir = iw;
	std::wstring::const_iterator end = c(text).end ();
	wchar_t last = L'\0', cur;
	while (ir != end) {
		cur = *ir++;
		if (cur == L'\n' && (ir == end || (*ir != L' ' && *ir != L'\t'))) {
			if (ir == end) // Drop newlines at the end
				break;
			if (*ir == L'\n') {
				// More than one contiguous newlines
				int n = 1;
				do
					++n;
				while (++ir!=end && *ir == L'\n');
				if (ir == end) // Drop newlines at the end
					break;
				std::fill_n (iw, n, L'\n');
				iw += n;
			} else {
				// A single newline character.
				// Drop it or replace it with a space character
				if (ucs_isalpha (last) && ucs_isalpha (*ir))
					*iw++ = L' ';
			}
		} else
			*iw++ = cur;
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

bool DiaryEntry::edit (const char *editor)
{
	// Write the contents to a temp file
	// Then invoke an editor
	char temp_file[] = "/tmp/tiary.temp.XXXXXXXXX";
	int fd = mkstemp (temp_file);
	if (fd < 0)
		return error_false (L"Failed to create a temporary file in /tmp.");
	fchmod (fd, S_IRUSR|S_IWUSR);
#if defined F_GETFD && defined F_SETFD && defined FD_CLOEXEC
	int fdflag = fcntl (fd, F_GETFD);
	if (fdflag >= 0)
		fcntl (fd, F_SETFD, fdflag|FD_CLOEXEC);
#endif

	// There is no universal method to notify the editor of the encoding;
	// So the best way is to use LC_CTYPE
	if (!split_write (fd, *this, false)) {
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
	reformat_content (title, text, raw);
	munmap (raw, fsize);
	return true;
}

bool DiaryEntry::edit_tags ()
{
	std::wstring tagstr = join (tags.begin (), tags.end (), L", ");
	std::wstring newtagstr = ui::dialog_input2 (L"Tags", std::wstring (), tagstr, 30, 0, tagstr);
	if (newtagstr == tagstr)
		return false;
	std::list<std::wstring> tags_list = split_string (newtagstr, L',');
	std::for_each (tags_list.begin (), tags_list.end (), (void (*)(std::wstring &))strip);
	// Here we may have empty strings which should be eliminated
	TagList newtags (tags_list.begin (), tags_list.end ());
	newtags.erase (std::wstring ());
	tags.swap (newtags);
	return (tags != newtags);
}

void DiaryEntry::view (const char *pager)
{
	ui::Window::suspend ();
	FILE *fp = call_external_program_popen (pager, "", "w");
	// SIGPIPE is ignored (signal called by main)
	split_write (fp, *this, true);
	pclose (fp);
	ui::Window::resume ();
}

void DiaryEntry::view_all (const char *pager, const DiaryEntryList &entries)
{
	ui::Window::suspend ();
	FILE *fp = call_external_program_popen (pager, "", "w");
	// SIGPIPE is ignored (signal called by main)
	for (DiaryEntryList::const_iterator it = entries.begin (); it != entries.end (); ++it) {
		split_write (fp, **it, true);
		fwrite_unlocked ("\n\n\n\n", 1, 4, fp);
	}
	pclose (fp);
	ui::Window::resume ();
}

} // namespace tairy
