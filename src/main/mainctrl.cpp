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
 * @file	main/mainctrl.cpp
 * @author	chys <admin@chys.info>
 * @brief	Implements tiary::MainCtrl
 */

#include "main/mainctrl.h"
#include "main/mainwin.h"
#include "ui/paletteid.h"
#include "ui/mouse.h"
#include "common/algorithm.h"
#include "common/format.h"
#include "common/split_line.h"
#include "common/string.h"
#include "common/unicode.h"
#include <algorithm>
#include <limits>

namespace tiary {

MainCtrl::MainCtrl (MainWin &win)
	: ui::Control (win)
	, ui::Scroll (1 /* To be set later */, false)
{
	set_cursor_visibility (false);
}

MainCtrl::~MainCtrl ()
{
}

bool MainCtrl::on_mouse (ui::MouseEvent mouse_event)
{
	if (!(mouse_event.m & (ui::LEFT_CLICK | ui::LEFT_DCLICK | ui::RIGHT_CLICK))) {
		return false;
	}
	ui::Scroll::Info info = ui::Scroll::get_info ();
	unsigned k = mouse_event.p.y + info.first;
	if (k >= info.focus) {
		unsigned expand_lines = w().global_options.get_num (GLOBAL_OPTION_EXPAND_LINES);
		if (k < info.focus + expand_lines) {
			k = info.focus;
		}
		else {
			k -= expand_lines - 1;
		}
	}
	if (k >= w().get_current_list ().size ()) {
		return false;
	}
	set_focus (k);
	if (mouse_event.m & ui::LEFT_DCLICK) {
		w().view_current ();
	}
	else if (mouse_event.m & ui::RIGHT_CLICK) {
		w().context_menu->show (mouse_event.p + get_pos (), true);
	}
	return true;
}


void MainCtrl::redraw ()
{
	choose_palette (ui::PALETTE_ID_ENTRY);
	clear ();

	if (w().entries.empty ()) {
		put(ui::Size{}, L"No entry yet.");
		put(ui::Size{0, 1}, L"Press \"a\" to create one; press Esc for the menu.");
#ifdef TIARY_USE_MOUSE
		put(ui::Size{0, 2}, L"You can also use your mouse.");
#endif
		return;
	}

	if (w().filtered_entries_ && w().filtered_entries_->empty ()) {
		put(ui::Size{}, L"This is in filtered mode.");
		put(ui::Size{0, 1}, L"But no entry satisfies your requirement.");
		put(ui::Size{0, 2}, L"Press Ctrl-G to modify your filter; or LEFT to see all entries.");
		return;
	}

	unsigned expand_lines = w().global_options.get_num (GLOBAL_OPTION_EXPAND_LINES);
	ui::Scroll::modify_height (get_size ().y - expand_lines + 1);

	Scroll::Info info = Scroll::get_info ();

	ui::Size pos{};

	std::wstring date_format = w().global_options.get_wstring (GLOBAL_OPTION_DATETIME_FORMAT);

	// Build a map to get entry ID from pointer
	std::map <const DiaryEntry *, unsigned> id_map;
	for (size_t i=0; i<w().entries.size (); ++i) {
		id_map.insert (std::make_pair (w().entries[i], i+1));
	}
	// Is there a filter?
	const DiaryEntryList &ent_lst = w().get_current_list ();

	wchar_t *disp_buffer = new wchar_t [get_size ().x];

	for (unsigned i=0; i<info.len; ++i) {

		choose_palette (i == info.focus_pos ? ui::PALETTE_ID_ENTRY_SELECT : ui::PALETTE_ID_ENTRY);

		if (i == info.focus_pos) {
			move_cursor (pos);
			clear(pos, ui::Size{get_size().x, expand_lines});
		}
		const DiaryEntry &entry = *ent_lst[i+info.first];

		// Entry ID
		pos = put(pos, format(L"%04a  "sv, id_map[&entry]));

		// Date
		choose_palette (i == info.focus_pos ? ui::PALETTE_ID_ENTRY_DATE_SELECT : ui::PALETTE_ID_ENTRY_DATE);
		pos = put (pos, entry.local_time.format (date_format.c_str ()));
		pos.x++;

		// Title
		SplitStringLine split_info;
		const std::wstring &title = entry.title;
		choose_palette (i == info.focus_pos ? ui::PALETTE_ID_ENTRY_TITLE_SELECT : ui::PALETTE_ID_ENTRY_TITLE);
		split_line (split_info, maxS (0, get_size().x-pos.x), title, 0, SPLIT_NEWLINE_AS_SPACE|SPLIT_CUT_WORD);
		pos = put (pos, disp_buffer,
				std::replace_copy_if (
						&title[split_info.begin], &title[split_info.begin+split_info.len],
						disp_buffer, [](auto x) { return !iswprint(x); }, L' ') - disp_buffer);
		pos.x++;

		// Labels
		const DiaryEntry::LabelList &labels = entry.labels;
		choose_palette (i == info.focus_pos ? ui::PALETTE_ID_ENTRY_LABELS_SELECT : ui::PALETTE_ID_ENTRY_LABELS);
		int left_wid = get_size().x - pos.x;
		for (DiaryEntry::LabelList::const_iterator it=labels.begin(); it!=labels.end(); ) {
			if (left_wid < 3) {
				break;
			}
			unsigned labelwid = ucs_width (*it);
			if (labelwid + 2 > unsigned (left_wid)) {
				pos = put (pos, L"...", 3);
				break;
			}
			pos = put (pos, *it);
			if (++it != labels.end ()) {
				pos = put (pos, L',');
			}
		}
		pos.x++;

		choose_palette (i == info.focus_pos ? ui::PALETTE_ID_ENTRY_TEXT_SELECT : ui::PALETTE_ID_ENTRY_TEXT);
		const std::wstring &text = entry.text;
		size_t offset = 0;
		if (i == info.focus_pos && expand_lines >= 2) {
			// Current entry
			// [Date] [Title] [Labels]
			// [...]
			for (unsigned j=1; j<expand_lines; ++j) {
				pos = ui::Size{0, pos.y + 1};
				offset = split_line (split_info, get_size().x, text, offset,
						SPLIT_NEWLINE_AS_SPACE);
				wchar_t *bufend = std::replace_copy_if (
						&text[split_info.begin], &text[split_info.begin+split_info.len],
						disp_buffer, [](auto x) { return !iswprint(x); }, L' ');
				pos = put (pos, disp_buffer, bufend-disp_buffer);
			}
		}
		else {
			// Other entry
			// [Date] [Title] [Labels] [...]
			offset = split_line (split_info, maxS (0, get_size().x - pos.x), text, offset,
					SPLIT_NEWLINE_AS_SPACE|SPLIT_CUT_WORD);
			wchar_t *bufend = std::replace_copy_if (
					&text[split_info.begin], &text[split_info.begin+split_info.len],
					disp_buffer, [](auto x) { return !iswprint(x); }, L' ');
			pos = put (pos, disp_buffer, bufend-disp_buffer);
		}
		pos = ui::Size{0, pos.y + 1};
	}
	delete [] disp_buffer;
}

void MainCtrl::set_focus (unsigned k)
{
	unsigned num_ent = w().get_current_list ().size ();
	if (num_ent == 0) {
		return;
	}
	if (int (k) < 0) {
		k = 0;
	}
	if (k >= num_ent) {
		k = num_ent - 1;
	}
	ui::Scroll::modify_focus (k);
	MainCtrl::redraw ();
}

void MainCtrl::set_focus_up ()
{
	set_focus (get_current_focus () - 1);
}

void MainCtrl::set_focus_down ()
{
	set_focus (get_current_focus () + 1);
}

void MainCtrl::set_focus_pageup ()
{
	set_focus (get_current_focus () - ui::Scroll::get_height () + 1);
}

void MainCtrl::set_focus_pagedown ()
{
	set_focus (get_current_focus () + ui::Scroll::get_height () - 1);
}

void MainCtrl::touch ()
{
	ui::Scroll::modify_number (w ().get_current_list ().size ());
	w().saved = false;
	MainCtrl::redraw ();
}


} // namespace tiary
