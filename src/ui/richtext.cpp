// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2018, 2019, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/

/**
 * @file	ui/richtext.cpp
 * @author	chys <admin@chys.info>
 * @brief	Implement class tiary::ui::RichText
 */

#include "ui/richtext.h"
#include "ui/paletteid.h"
#include "ui/dialog_message.h"
#include "ui/mouse.h"
#include "ui/scrollbar.h"
#include "common/algorithm.h"
#include "common/format.h"
#include "common/string.h"
#include <utility>

namespace tiary {
namespace ui {

RichText::RichText(Window &win, const MultiLineRichText &mrt)
	: Control (win)
	, mrt_(mrt)
	, top_line (0)
	, highlight_list ()
	, search_info ()
{
	set_cursor_visibility (false);
}

RichText::RichText(Window &win, MultiLineRichText &&mrt)
	: Control (win)
	, mrt_(std::move(mrt))
	, top_line (0)
	, highlight_list ()
	, search_info ()
{
	set_cursor_visibility (false);
}

RichText::~RichText ()
{
}

void RichText::redraw ()
{
	unsigned wid = get_size ().x - 1;
	unsigned hgt = get_size ().y - 1;
	if (int (wid) < 0 || int (hgt) < 0) {
		return;
	}
	unsigned show_lines = minU (hgt, mrt_.lines.size() - top_line);

	for (unsigned i=0; i<show_lines; ++i) {
		Size pos{0, i};
		choose_palette(mrt_.lines[top_line + i].id);
		clear(pos, {wid, 1});
		size_t offset = mrt_.lines[top_line + i].offset;
		size_t end_offset = mrt_.lines[top_line + i].len + offset;
		HighlightList::const_iterator lower = highlight_list.lower_bound (offset);
		HighlightList::const_iterator upper = highlight_list.lower_bound (end_offset);
		// We may have a match whose starting point is on the previous line
		// Check for that!
		if (lower != highlight_list.begin ()) {
			HighlightList::const_iterator last = lower;
			--last;
			if (offset < last->first + last->second) {
				lower = last;
			}
		}
		for (HighlightList::const_iterator it = lower; it != upper; ++it) {
			if (offset < it->first) {
				pos = put(pos, mrt_.text.data() + offset, it->first-offset);
				offset = it->first;
			}
			attribute_toggle (REVERSE);
			size_t highlight_end = minSize (it->first+it->second, end_offset);
			pos = put(pos, mrt_.text.data() + offset, highlight_end - offset);
			attribute_toggle (REVERSE);
			offset = highlight_end;
		}
		pos = put(pos, mrt_.text.data() + offset, end_offset - offset);
	}
	if (show_lines < hgt) {
		choose_palette (PALETTE_ID_RICHTEXT);
		clear({0, show_lines}, {wid, hgt - show_lines});
	}
	// Status bar
	choose_palette (PALETTE_ID_BACKGROUND);
	clear({0, hgt}, {wid, 1});
	put({0, hgt},
			format(L"Lines %a-%b/%c"sv, top_line + 1, top_line + show_lines,
				unsigned(mrt_.lines.size())));
	// Scroll bar
	clear({wid, 0}, {1, hgt + 1});
	attribute_toggle (REVERSE);
	ScrollBarInfo scrollbar = scrollbar_info(hgt, mrt_.lines.size(), top_line);
	clear({wid, scrollbar.pos}, {1, scrollbar.size});
}

bool RichText::on_mouse (MouseEvent mouse_event)
{
	if (!(mouse_event.m & (LEFT_CLICK|LEFT_PRESS|LEFT_DCLICK)) ||
			(mouse_event.p.x+1 < get_size().x)) {
		return false;
	}
	top_line = scrollbar_click(get_size().y - 1, mrt_.lines.size(), mouse_event.p.y);
	RichText::redraw ();
	return true;
}

bool RichText::on_key (wchar_t key)
{
	switch (key) {
		case L'k':
		case UP:
			if (top_line) {
				--top_line;
				RichText::redraw ();
				return true;
			}
			break;
		case L'j':
		case DOWN:
			if (top_line + 1 < mrt_.lines.size()) {
				++top_line;
				RichText::redraw ();
				return true;
			}
			break;
		case HOME:
		case L'^':
		case L'g':
		case L'<':
			top_line = 0;
			RichText::redraw ();
			return true;
		case END:
		case L'$':
		case L'>':
		case L'G':
			if (mrt_.lines.size()) {
				top_line = mrt_.lines.size() - 1;
				RichText::redraw ();
				return true;
			}
			break;
		case PAGEUP:
		case L'b':
			if (top_line) {
				top_line = maxS (0, top_line - get_size ().y + 2);
				RichText::redraw ();
			}
			break;

		case PAGEDOWN:
		case L'f':
		case L' ':
			if (top_line + 1 < mrt_.lines.size()) {
				top_line = minU(mrt_.lines.size() - 1, top_line + get_size().y - 2);
				RichText::redraw ();
				return true;
			}
			break;

		case CTRL_F:
		case L'/':
			slot_search (false);
			return true;

		case L'?':
			slot_search (true);
			return true;

		case F3:
		case L'n':
			slot_search_continue (false);
			return true;

		case L'N':
			slot_search_continue (true);
			return false;
	}
	return false;
}

void RichText::slot_search (bool bkwd)
{
	if (search_info.dialog (bkwd)) {
		std::vector<std::pair<size_t, size_t>> result = search_info.match(mrt_.text);
		highlight_list.clear ();
		highlight_list.insert(result.begin(), result.end());
		do_search (false, true);
	}
}

void RichText::slot_search_continue (bool previous)
{
	if (!search_info) {
		slot_search (previous);
	}
	else {
		do_search (previous, false);
	}
}

void RichText::do_search (bool previous, bool include_current)
{
	unsigned k = top_line;
	unsigned num_ents = mrt_.lines.size();
	int inc = (!previous == !search_info.get_backward ()) ? 1 : -1;
	if (!include_current) {
		k += inc;
	}
	for (; k < num_ents; k += inc) {
		// Is there any match on the k-th line?
		if (highlight_list.lower_bound(mrt_.lines[k].offset)
				!= highlight_list.lower_bound(mrt_.lines[k].offset + mrt_.lines[k].len)) {
			top_line = k;
			RichText::redraw ();
			return;
		}
	}
	dialog_message(L"Not found"sv, L"Error"sv);
}

} // namespace tiary::ui
} // namespace tiary
