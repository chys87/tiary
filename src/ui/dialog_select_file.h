// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2019, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#ifndef TIARY_UI_DIALOG_SELECT_FILE_H
#define TIARY_UI_DIALOG_SELECT_FILE_H

#include <string>

namespace tiary {
namespace ui {

const unsigned SELECT_FILE_READ           = 0;
const unsigned SELECT_FILE_WRITE          = 1;
const unsigned SELECT_FILE_WARN_OVERWRITE = 2; // Ignored if SELECT_FILE_WRITE not set

std::wstring dialog_select_file (
		std::wstring_view hint, ///< Info displayed in the top of the dialog
		std::wstring_view default_file, ///< Default file name
		unsigned options
		);


} // namespace tiary::ui
} // namespace tiary


#endif // Include guard
