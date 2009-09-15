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


#ifndef TIARY_MAIN_DIALOG_GLOBAL_PREF_H
#define TIARY_MAIN_DIALOG_GLOBAL_PREF_H

#include "diary/config.h"
#include <string>

namespace tiary {

void edit_global_options (GlobalOptionGroup &, const std::wstring &current_filename);

} //namespace tiary

#endif // include guard
