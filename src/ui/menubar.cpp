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


#include "ui/menubar.h"
#include "ui/menu.h"
#include "ui/paletteid.h"
#include "ui/window.h"
#include <utility> // std::forward
#include <stack>
#include <iterator> // std::advance
#include <assert.h>

namespace tiary {
namespace ui {






MenuBar::MenuBar (Window &win)
	: Control (win)
	, UnfocusableControl (win)
{
}

MenuBar::~MenuBar ()
{
}

Menu &MenuBar::add (const std::wstring &text)
{
	item_list.push_back (Item ());
	Item &item = item_list.back ();
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
		win.register_hotkey (c, Signal (this, &MenuBar::slot_clicked, n-1));
	}

	return item.menu;
}

Menu &MenuBar::add (const wchar_t *text)
{
	return add (std::wstring (text));
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
		it->text.output (*this, make_size (it->w, 0), it->text.get_width ());
	}
}

void MenuBar::slot_clicked (size_t k)
{
	for (;;) {
		assert (k < item_list.size ());

		// Get the item
		ItemList::iterator it = item_list.begin ();
		std::advance (it, k);
		Item &item = *it;

		// Highlight the selected item
		choose_palette (PALETTE_ID_MENUBAR_SELECT);
		item.text.output (*this, make_size (item.w, 0), item.text.get_width ());

		Size left = win.get_pos () + get_pos () + make_size (item.w, 1);
		Size right = make_size (get_screen_size ().x, left.y);

		// Pop out sub menu
		MenuItem *chosen_item = item.menu.show (left, right);

		// Restore display
		choose_palette (PALETTE_ID_MENUBAR);
		item.text.output (*this, make_size (item.w, 0), item.text.get_width ());

		// Deal with the result
		if (chosen_item) {
			chosen_item->sig.emit ();
			break;
		}

		// LEFT/RIGHT key?
		MouseEvent me_buf;
		wchar_t c = win.get_noblock (&me_buf);
		if (c == LEFT) {
			k = (k + item_list.size () - 1) % item_list.size ();
		}
		else if (c == RIGHT) {
			k = (k + 1) % item_list.size ();
		}
		else {
			if (c) {
				if (c == MOUSE) {
					win.unget (me_buf);
				}
				else {
					win.unget (c);
				}
			}
			break;
		}
	}
}

} // namespace tiary::ui
} // namespace tiary
