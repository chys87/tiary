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
#include "ui/uistring_one.h"
#include "ui/menu.h"
#include <string>
#include <vector>

namespace tiary {
namespace ui {

/**
 * @brief	Control for a menu bar
 *
 * tiary::ui::MenuBar is supposed to provide a more sophisticated than
 * tiary::ui::show_menu.
 *
 * We also allows displaying some texts on the right side of the menubar.
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

	void set_text (const std::wstring &);

private:
	struct Item
	{
		UIStringOne text;
		unsigned w; ///< X coordinate relative to containing MenuBar
		std::unique_ptr<Menu> menu{new Menu}; ///< Menu object. Use unique_ptr to avoid pointer invalidation.
	};
	typedef std::vector<Item> ItemList;
	ItemList item_list;

	UIStringOne text;
};


} // namespace tiary::ui
} // namespace tiary


#endif
