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


#ifndef TIARY_MAIN_MAINUI_H
#define TIARY_MAIN_MAINUI_H

#include <string_view>

namespace tiary {


/**
 * @brief	The kernel of Tiary
 *
 * Before calling this function,
 *  - Initialize the UI system;
 */
int main_body(std::wstring_view initial_filename);


} // namespace tiary

#endif // Include guard
