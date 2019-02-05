// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2010, 2018, 2019, chys <admin@CHYS.INFO>
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
#include "ui/richtextlist.h"
#include "common/string.h"
#include "common/datetime.h"
#include "common/dir.h"
#include "common/misc.h"
#include "ui/paletteid.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <functional>
#include <errno.h>

namespace tiary {

namespace {

using namespace ui;

const unsigned edit_line_width = 78;
const unsigned view_line_width = 78;

void write_for_view(MultiLineRichText *mrt,
		const DiaryEntry &ent, const std::wstring &longtime_format) {
	mrt->append(PALETTE_ID_SHOW_BOLD, view_line_width, L'=');
	mrt->append(PALETTE_ID_SHOW_BOLD, ent.title);
	mrt->append(PALETTE_ID_SHOW_BOLD, view_line_width, L'=');
	mrt->append(PALETTE_ID_SHOW_NORMAL, ent.local_time.format (longtime_format));
	if (!ent.labels.empty ()) {
		mrt->append(PALETTE_ID_SHOW_NORMAL,
				L"Labels: "sv, join(ent.labels.begin(), ent.labels.end(), L", "sv));
	}
	mrt->append(PALETTE_ID_SHOW_NORMAL);

	// Text
	size_t base_offset = mrt->text.length ();
	mrt->text += ent.text;
	PaletteID palette = PALETTE_ID_SHOW_NORMAL;
	for (const auto &item: split_line (edit_line_width, ent.text)) {
		size_t begin = base_offset + item.begin;
		// If a line begins with a space, it's considered the first line of a qutoed paragraph
		if (item.len && (mrt->text[begin] == L' ')) {
			palette = PALETTE_ID_SHOW_QUOTE;
		}
		// An empty line ends a quoted paragraph
		if (item.len == 0) {
			palette = PALETTE_ID_SHOW_NORMAL;
		}
		mrt->lines.push_back({begin, item.len, palette, item.wid});
	}
}

// Write the title and text to a file which will be passed to an editor
bool write_for_edit (int fd, const std::wstring &title, const std::wstring &text)
{
	std::string mbs = wstring_to_mbs (title);
	mbs.reserve(mbs.length() + text.length() * 2);
	mbs.append (2, '\n');
	for (const auto &item: split_line(edit_line_width, text)) {
		mbs += wstring_to_mbs(text.data() + item.begin, item.len);
		mbs += '\n';
	}
	return (size_t)write (fd, mbs.data (), mbs.length ()) == mbs.length ();
}

// The string view argument references the edited file.
// Read its contents, removing unnecessary newline characters
// (add spaces when necessary)
// The first line is converted to the title
void reformat_content(std::wstring *title, std::wstring *text, std::string_view s) {
	// Be careful!
	// Some implementations may use COW in std::basic_string
	// or not always keeping a null character at the end
	// (The Standard does not specify whether *end() returns L'\0' or
	// triggers an exception. Avoid that.)
	// Make sure this function works with those implementations too.

	// First line = title
	size_t first_new_line = s.find('\n');
	*title = mbs_to_wstring(s.substr(0, first_new_line));

	if (first_new_line == s.npos) {
		// Only one line??
		text->clear();
		return;
	}
	s.remove_prefix(first_new_line + 1);

	*text = mbs_to_wstring(s);
	std::wstring::iterator iw = text->begin ();
	std::wstring::const_iterator ir = iw;
	std::wstring::const_iterator end = text->end();
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
	text->erase(iw, text->end());
}

bool error_false(std::wstring_view info) {
	ui::dialog_message (info);
	return false;
}

} // anonymous namespace

bool edit_entry (DiaryEntry &ent, const char *editor)
{
	// Write the contents to a temp file
	// Then invoke an editor
	std::string temp_file;
	/*
	 * The reason why I implement my own my_mkstemp is that
	 * I want to add a suffix (.txt) to the filename, in order
	 * to take advantage of the filetype recognition feature of
	 * some editors (For example, VIM can behave slightly differently
	 * between filenames "/tmp/tiary.temp.ABcdEF" and 
	 * "/tmp/tiary.temp.ABcdEF.txt"
	 */
	int fd = my_mkstemp(&temp_file, "/tmp/tiary.temp.|.txt");
	if (fd < 0) {
		return error_false(L"Failed to create a temporary file in /tmp."sv);
	}

	// There is no universal method to notify the editor of the encoding;
	// So the best way is to use LC_CTYPE
	if (!write_for_edit (fd, ent.title, ent.text)) {
		close (fd);
		unlink (temp_file.c_str ());
		return error_false(L"Failed to write to temporary file :( Why?"sv);
	}

	struct stat stbuf;
	if (fstat (fd, &stbuf) != 0) {
		close (fd);
		unlink (temp_file.c_str ());
		return error_false(L"stat on temporary file failed :( Why?"sv);
	}
	time_t oldmtime = stbuf.st_mtime;

	// Call and wait for the editor
	ui::Window::suspend ();
	int editor_status = call_external_program_system (editor, temp_file.c_str ());
	ui::Window::resume ();

	unlink (temp_file.c_str ());

	if (editor_status != 0) { // Editor failed.
		close (fd);
		return error_false(L"Editor not found or failed for some reason :("sv);
	}

	// No change?
	if (fstat (fd, &stbuf) != 0 || oldmtime == stbuf.st_mtime) {
		close (fd);
		return false;
	}

	// Read contents. For convenience, use a memory mapping
	if (lseek(fd, 0, SEEK_SET) != 0) {
		close(fd);
		return error_false(L"Strange IO error with temporary file :( Why?"sv);
	}

	std::string raw;
	bool read_ret = read_whole_file(fd, &raw);
	close(fd);
	if (!read_ret) {
		return error_false(L"Failed to read temporary file :( Why?"sv);
	}

	reformat_content(&ent.title, &ent.text, raw);
	return true;
}

void view_entry (DiaryEntry &ent, const std::wstring &longtime_format)
{
	MultiLineRichText mrt;
	mrt.text.reserve(ent.text.length() + 512);
	mrt.lines.reserve(ent.text.length() / 32);
	write_for_view(&mrt, ent, longtime_format);
	ui::dialog_richtext (
			ent.title,
			std::move(mrt),
			Size{view_line_width + 3, 0});
}

void view_all_entries (const DiaryEntryList &entries, const std::wstring &longtime_format)
{
	if (entries.empty ()) {
		return;
	}
	MultiLineRichText mrt;
	mrt.text.reserve(512 * entries.size()); // This is arbitrary. Actual size is difficult to guess.
	mrt.lines.reserve(32 * entries.size()); // This is arbitrary. Actual size is difficult to guess.
	DiaryEntryList::const_iterator it = entries.begin ();
	for (;;) {
		write_for_view(&mrt, **it, longtime_format);
		if (++it == entries.end ()) {
			break;
		}
		ui::RichTextLine tmp_line = {mrt.text.length(), 0, ui::PALETTE_ID_SHOW_NORMAL, 0};
		mrt.lines.insert(mrt.lines.end(), 4, tmp_line);
	}
	ui::dialog_richtext (
			L"View all entries"sv,
			std::move(mrt),
			Size{view_line_width + 3, 0});
}

} // namespace tairy
