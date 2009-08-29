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


#ifndef TIARY_UI_MENUBAR_H
#define TIARY_UI_MENUBAR_H

#include "ui/control.h"
#include "ui/uistring.h"
#include "ui/menu.h"
#include <string>
#include <list>

namespace tiary {
namespace ui {

/**
 * @brief	Control for a menu bar
 *
 * tiary::ui::MenuBar is supposed to provide a more sophisticated than
 * tiary::ui::show_menu, so we do not use tiary::ui::LLMenuItem, which
 * is considered a "low level" interface.
 */
class MenuBar : public UnfocusableControl
{
public:

	explicit MenuBar (Window &);
	~MenuBar ();

	Menu &add (const std::wstring &); ///< Add an item to menu
	Menu &add (const wchar_t *); ///< Add an item to menu

	bool on_mouse (MouseEvent);
	void redraw ();

	void slot_clicked (size_t k);

private:
	struct Item
	{
		UIStringOne text;
		unsigned w; // X coordinate relative to containing MenuBar
		Menu menu;
	};
	/*
	 * IMPORTANT: Cannot use other containers
	 * It is important for our program that adding new items to the
	 * list does not invalidate existing references to other items.
	 */
	typedef std::list<Item> ItemList;
	ItemList item_list;
};


} // namespace tiary::ui
} // namespace tiary


#endif
