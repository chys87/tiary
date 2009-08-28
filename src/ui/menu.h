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


#ifndef TIARY_UI_MENU_H
#define TIARY_UI_MENU_H

#include "common/signal.h"
#include <list>
#include <string>

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

struct MenuItem
{
	std::wstring text; ///< Empty string indicates a separator
	Signal sig; ///< A Signal associated with this item
	bool hidden; ///< Don't display this item. Can be useful in some contexts, i.e. a Recent File list
	Menu *submenu; ///< Submenu

	MenuItem (); ///< Initialize to a separator

#ifdef TIARY_HAVE_RVALUE_REFERENCES
	MenuItem (const std::wstring &, const Signal &);
	explicit MenuItem (const std::wstring &, Signal && = Signal ());
#else
	explicit MenuItem (const std::wstring &, const Signal & = Signal ());
#endif

	MenuItem (const MenuItem &); ///< Deep copy...
	MenuItem &operator = (const MenuItem &); ///< Deep copy...
#ifdef TIARY_HAVE_RVALUE_REFERENCES
	MenuItem (MenuItem &&);
	MenuItem &operator = (MenuItem &&);
#endif

	~MenuItem ();

	Menu &get_submenu (); ///< Returns a reference to the submenu (create if necessary)
};

struct Menu
{
	/*
	 * IMPORTANT: Cannot use other containers
	 * It is important for our program that adding new items to the
	 * list does not invalidate existing references to other items.
	 */
	typedef std::list<MenuItem> MenuItemList;
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
#ifdef TIARY_HAVE_RVALUE_REFERENCES
	MenuItem &add (const std::wstring &text, const Signal &sig);
	MenuItem &add (const std::wstring &text, Signal &&sig = Signal ());
	MenuItem &add (const wchar_t *text, const Signal &sig);
	MenuItem &add (const wchar_t *text, Signal &&sig = Signal ());
#else
	MenuItem &add (const std::wstring &text, const Signal &sig = Signal ());
	MenuItem &add (const wchar_t *text, const Signal &sig = Signal ());
#endif

	// operator () is identical to add (), except that it returns *this
	Menu &operator () ()
	{
		add ();
		return *this;
	}
#ifdef TIARY_HAVE_RVALUE_REFERENCES
	Menu &operator () (const std::wstring &text, Signal &&sig = Signal ())
	{
		add (text, std::forward<Signal> (sig));
		return *this;
	}
	Menu &operator () (const wchar_t *text, Signal &&sig = Signal ())
	{
		add (text, std::forward<Signal> (sig));
		return *this;
	}
#endif

#ifdef TIARY_HAVE_RVALUE_REFERENCES
	Menu &operator () (const std::wstring &text, const Signal &sig)
#else
	Menu &operator () (const std::wstring &text, const Signal &sig = Signal ())
#endif
	{
		add (text, sig);
		return *this;
	}
#ifdef TIARY_HAVE_RVALUE_REFERENCES
	Menu &operator () (const wchar_t *text, const Signal &sig)
#else
	Menu &operator () (const wchar_t *text, const Signal &sig = Signal ())
#endif
	{
		add (text, sig);
		return *this;
	}

	Menu &add_submenu (const std::wstring &text);

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
