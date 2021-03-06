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


#include "ui/listbox.h"
#include "ui/paletteid.h"
#include "ui/mouse.h"
#include "ui/scrollbar.h"
#include "common/algorithm.h"
#include "common/unicode.h"
#include <algorithm>
#include <assert.h>


namespace tiary {
namespace ui {

ListBox::ListBox (Window &win)
	: Control (win)
	, scroll_(0, false)
	, items ()
	, select_any (false)
{
}

ListBox::~ListBox ()
{
}

size_t ListBox::get_select () const
{
	if (select_any) {
		return scroll_.get_focus();
	} else {
		return size_t (-1);
	}
}

void ListBox::set_items (const ItemList &new_items, size_t new_select, bool emit_signal)
{
	items = new_items;
	scroll_.modify_number(items.size());
	set_select (new_select, emit_signal);
}

void ListBox::set_items (ItemList &&new_items, size_t new_select, bool emit_signal)
{
	items = std::forward<ItemList> (new_items);
	scroll_.modify_number(items.size());
	set_select (new_select, emit_signal);
}

void ListBox::set_select (size_t new_select, bool emit_signal, bool scroll_to_top)
{
	if (new_select < items.size ()) {
		select_any = true;
		scroll_.modify_focus(new_select);
		if (scroll_to_top) {
			scroll_.scroll_focus_to_first();
		}
	}
	else {
		select_any = false;
	}
	if (emit_signal) {
		sig_select_changed.emit ();
	}
	ListBox::redraw ();
}

bool ListBox::on_key (wchar_t key)
{
	Scroll::Info scroll_info = scroll_.get_info();
	switch (key) {
		case UP:
		case 'k':
			if (!select_any) {
				if (items.empty ()) {
					return false;
				}
				set_select (scroll_info.focus);
				return true;
			}
			else {
				if (items.size () == 0) {
					return false;
				}
				if (scroll_info.focus == 0) {
					return false;
				}
				set_select (scroll_info.focus - 1);
				return true;
			}

		case DOWN:
		case 'j':
			if (!select_any) {
				if (items.empty ()) {
					return false;
				}
				set_select (scroll_info.focus);
				return true;
			}
			else {
				if (items.size () == 0) {
					return false;
				}
				if (scroll_info.focus + 1 >= items.size ()) {
					return false;
				}
				set_select (scroll_info.focus + 1);
				return true;
			}

		case PAGEUP:
			if (items.size () == 0) {
				return false;
			}
			if (scroll_info.focus == 0) {
				return false;
			}
			set_select (maxS (0, scroll_info.focus - get_size().y + 1));
			return true;

		case PAGEDOWN:
			if (items.size () == 0) {
				return false;
			}
			if (scroll_info.focus + 1 >= items.size ()) {
				return false;
			}
			set_select (minS (scroll_info.focus + get_size().y - 1, items.size () - 1));
			return true;

		case HOME:
		case L'g':
		case L'0':
		case L'^':
			if (items.size () == 0) {
				return false;
			}
			set_select (0);
			return true;

		case END:
		case L'G':
		case L'$':
			if (items.size () == 0) {
				return false;
			}
			set_select (items.size () - 1);
			return true;

		case L' ':
			if (select_any) {
				return false;
			}
			else {
				set_select (scroll_info.focus);
				return true;
			}

		default:
			return false;
	}
	assert (false);
}

bool ListBox::on_mouse (MouseEvent mouse_event)
{
	size_t new_focus;
	bool focus_to_first = false;
	// Right click: Deselect all
	if (mouse_event.m & (RIGHT_CLICK|RIGHT_PRESS)) {
		new_focus = size_t(-1);
	}
	else if (mouse_event.m & (LEFT_CLICK|LEFT_PRESS|LEFT_DCLICK)) {
		if (mouse_event.p.x + 1 >= get_size().x) {
			// Clicked on the scroll bar
			new_focus = scrollbar_click (get_size().y, items.size(), mouse_event.p.y);
			focus_to_first = true;
		}
		else {
			if (mouse_event.p.y >= scroll_.get_len()) {
				new_focus = size_t(-1);
			} else {
				new_focus = mouse_event.p.y + scroll_.get_first();
			}
		}
	}
	else {
		return false;
	}
	set_select (new_focus, true, focus_to_first);
	if (new_focus < items.size () && (mouse_event.m & LEFT_DCLICK) && mouse_event.p.x+1 < get_size().x) {
		sig_double_clicked.emit ();
	}
	return true;
}

bool ListBox::on_focus ()
{
	ListBox::redraw ();
	return true;
}

void ListBox::on_defocus ()
{
	ListBox::redraw ();
}

void ListBox::move_resize (Size new_pos, Size new_size)
{
	unsigned oldheight = get_size ().y;
	Control::move_resize (new_pos, new_size);
	if (oldheight != new_size.y) {
		scroll_.modify_height(new_size.y);
	}
}

void ListBox::redraw ()
{
	unsigned disp_wid = get_size().x - 1; // Reserve one for scrollbar
	unsigned disp_hgt = get_size().y;
	if (disp_wid==0 || disp_hgt==0) {
		return;
	}
	choose_palette (PALETTE_ID_LISTBOX);
	clear ();
	const Scroll::Info info = scroll_.get_info();
	for (unsigned i=0; i<info.len; ++i) {
		const std::wstring &str = items[info.first+i];
		choose_palette ((select_any && i==info.focus_pos) ? PALETTE_ID_LISTBOX_SELECT : PALETTE_ID_LISTBOX);
		Size pos{0, i};
		unsigned strwid = ucs_width (str);
		// Are we going to overflow?
		if (strwid <= disp_wid) {
			//NO:
			pos = put (pos, str);
			if (select_any && i==info.focus_pos) // Focus? Highlight the whole line
				clear(pos, {disp_wid - pos.x, 1});
		}
		else {
			// YES: Cannot display all
			size_t display_wchars = max_chars_in_width (str, maxS (disp_wid - 4, 0));
			put (pos, str.c_str (), display_wchars);
			choose_palette (PALETTE_ID_LISTBOX);
			pos = put(Size(maxS(disp_wid - 4, 0), i), L' ');
			attribute_toggle (REVERSE);
			pos = put (pos, L"...");
		}
	}
	// Display the scroll bar
	choose_palette (PALETTE_ID_LISTBOX);
	clear({disp_wid, 0}, {1, disp_hgt});
	attribute_toggle (REVERSE);
	ScrollBarInfo scrollbar = scrollbar_info (disp_hgt, items.size(), info.first);
	clear({disp_wid, scrollbar.pos}, {1, scrollbar.size});

	move_cursor({0, info.focus_pos});
}

} // namespace tiary::ui
} // namespace tiary
