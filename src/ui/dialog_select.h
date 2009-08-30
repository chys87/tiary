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

#ifndef TIARY_UI_DIALOG_SELECT_H
#define TIARY_UI_DIALOG_SELECT_H

#include <string>
#include <vector>

namespace tiary {
namespace ui {


/**
 * @brief	Displays a menu to allow the user to choose an item from the many
 * @result	The subscript of the selected item; or size_t(-1) on error
 */
size_t dialog_select (const std::wstring &title,
		const std::vector <std::wstring> &selections);


} // namespace tiary::ui
} // namespace tiary


#endif // include guard
