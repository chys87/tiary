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

#ifndef TIARY_MAIN_DIALOG_VIEW_EDIT_H
#define TIARY_MAIN_DIALOG_VIEW_EDIT_H

#include <vector>

namespace tiary {


struct DiaryEntry;

/**
 * @brief	Edit the entry in an external editor
 * @param	editor	External editor used to edit the text.
 * "$param" is supported (no braces)
 *
 * "$EDITOR|rvim|vim|emacs -nw"
 * @result	Whether the entry is modified
 */
bool edit_entry (DiaryEntry &, const char *editor);
/**
 * @brief	View the content in a dialog
 */
void view_entry (DiaryEntry &);

/**
 * @brief	View many items at one time
 */
void view_all_entries (const std::vector <DiaryEntry *> &);



} // namespace tiary


#endif // include guard
