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


#include "ui/menubar.h"
#include "ui/menu.h"
#include "ui/paletteid.h"
#include "ui/window.h"
#include "ui/mouse.h"
#include "common/algorithm.h"
#include <stack>
#include <assert.h>

namespace tiary {
namespace ui {






MenuBar::MenuBar (Window &win)
	: Control(win, kUnfocusable)
	, item_list ()
	, text ()
{
}

MenuBar::~MenuBar ()
{
}

Menu &MenuBar::add(std::wstring_view text) {
	item_list.emplace_back();
	Item &item = item_list.back();
	item.text.set_text (text);
	size_t n = item_list.size ();
	if (n == 1) {
		item.w = 0;
	}
	else {
		ItemList::const_reverse_iterator it = item_list.rbegin ();
		++it;
		item.w = it->w + it->text.get_width () + 2 /* Space */;
	}
	if (wchar_t c = item.text.get_hotkey ()) {
		window().register_hotkey(c, Signal(this, &MenuBar::slot_clicked, n - 1),
				Hotkeys::CASE_INSENSITIVE | Hotkeys::ALLOW_ALT);
	}

	return *item.menu;
}

bool MenuBar::on_mouse (MouseEvent mouse_event)
{
	if (mouse_event.m & MOUSE_ALL_BUTTON) {
		ItemList::const_iterator it = item_list.begin ();
		ItemList::const_iterator end = item_list.end ();
		unsigned k = 0;
		while (it != end) {
			if ((mouse_event.p.x - it->w) < it->text.get_width ()) {
				slot_clicked (k);
				return true;
			}
			++k;
			++it;
		}
	}
	return false;
}

void MenuBar::redraw ()
{
	choose_palette (PALETTE_ID_MENUBAR);
	clear ();
	for (ItemList::iterator it=item_list.begin(); it!=item_list.end(); ++it) {
		it->text.output(*this, {it->w, 0}, it->text.get_width ());
	}
	unsigned x = 0;
	if (!item_list.empty ()) {
		x = item_list.back ().w + item_list.back ().text.get_width () + 2;
	}
	if (x < get_size ().x) {
		// Align to the right
		unsigned w = minU (get_size ().x - x, text.get_width ());
		x = get_size ().x - w;
		text.output(*this, {x, 0}, w);
	}
}

void MenuBar::slot_clicked (size_t k)
{
	for (;;) {
		assert (k < item_list.size ());

		// Get the item
		Item &item = item_list[k];

		// Highlight the selected item
		choose_palette (PALETTE_ID_MENUBAR_SELECT);
		item.text.output(*this, {item.w, 0}, item.text.get_width());

		Size left = window().get_pos() + get_pos() + Size{item.w, 1};
		Size right{get_screen_width(), left.y};

		// Pop out sub menu
		MenuItem *chosen_item = item.menu->show (left, right);

		// Restore display
		choose_palette (PALETTE_ID_MENUBAR);
		item.text.output(*this, {item.w, 0}, item.text.get_width());

		// Deal with the result
		if (chosen_item) {
			chosen_item->action.emit ();
			break;
		}

		// LEFT/RIGHT key?
		MouseEvent me_buf;
		wchar_t c = window().get_noblock(&me_buf);
		if (c == LEFT) {
			k = (k + item_list.size () - 1) % item_list.size ();
		}
		else if (c == RIGHT) {
			k = (k + 1) % item_list.size ();
		}
		else {
			if (c) {
				if (c == MOUSE) {
					window().unget(me_buf);
				}
				else {
					window().unget(c);
				}
			}
			break;
		}
	}
}

void MenuBar::set_text (const std::wstring &s)
{
	if (text.get_text () != s) {
		text.set_text (s, UIStringOne::NO_HOTKEY);
		MenuBar::redraw ();
	}
}

} // namespace tiary::ui
} // namespace tiary
