// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the so-called 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#ifndef TIARY_MAIN_MAINUI_H
#define TIARY_MAIN_MAINUI_H

#include <string>

namespace tiary {


/**
 * @brief	The kernel of Tiary
 *
 * Before calling this function,
 *  - Initialize the UI system;
 */
int main_body (const std::wstring &initial_filename);


} // namespace tiary

#endif // Include guard
