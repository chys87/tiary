// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2018, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#ifndef TIARY_UI_MENU_H
#define TIARY_UI_MENU_H

#include "common/action.h"
#include <string>
#include <vector>

/*
 * Because we want to put tiary::ui::MenuItem and tiary::ui::Menu in containers,
 * we must make sure they can be safely copied, which unfortunately might be
 * expensive.
 */

namespace tiary {
namespace ui {

struct Size;

struct MenuItem;
struct Menu;

struct MenuItem {
	std::wstring text; ///< Empty string indicates a separator
	Action action; ///< An action associated with this item
	bool hidden = false; ///< Don't display this item. Can be useful in some contexts, i.e. a Recent File list
	std::unique_ptr<Menu> submenu; ///< Submenu

	Menu &get_submenu (); ///< Returns a reference to the submenu (create if necessary)
};

struct Menu
{
	/*
	 * Use unique_ptr to avoid pointer invalidation.
	 */
	typedef std::vector<std::unique_ptr<MenuItem>> MenuItemList;
	MenuItemList item_list;

	// Add some functions to make it behave like a container
	typedef MenuItemList::const_iterator const_iterator;
	typedef MenuItemList::iterator iterator;
	const_iterator begin () const { return item_list.begin (); }
	const_iterator end () const { return item_list.end (); }
	iterator begin () { return item_list.begin (); }
	iterator end () { return item_list.end (); }
	size_t size () const { return item_list.size (); }

	MenuItem &add (); ///< Add a separator
	MenuItem &add(std::wstring_view text, const Signal &sig);
	MenuItem &add(std::wstring_view text, Action &&act);
	MenuItem &add(std::wstring_view text, Signal &&sig = Signal());
	MenuItem &add(std::wstring_view text, const Action &act);

	// operator () is identical to add (), except that it returns *this
	Menu &operator () ()
	{
		add ();
		return *this;
	}
	Menu &operator () (std::wstring_view text, Signal &&sig = Signal()) {
		add (text, std::move (sig));
		return *this;
	}
	Menu &operator () (std::wstring_view text, Action &&act) {
		add (text, std::move (act));
		return *this;
	}

	Menu &operator () (std::wstring_view text, const Signal &sig) {
		add (text, sig);
		return *this;
	}
	
	Menu &operator () (std::wstring_view text, const Action &act) {
		add (text, act);
		return *this;
	}

	Menu &add_submenu(std::wstring_view text);

	/**
	 * @param	left	The first-choice place to display the menu.
	 *					(Menu is displayed on the right-hand side of left)
	 * @param	right	The second-choice place to display the menu.
	 *					(Menu is displayed on the left-hand side of right)
	 * @param	emit_signal	Whether or not emit the associated signal automatically
	 */
	MenuItem *show (Size left, Size right, bool emit_signal = false);
	MenuItem *show (Size, bool emit_signal = false);
};

} // namespace tiary::ui
} // namespace tiary

#endif // include guard
