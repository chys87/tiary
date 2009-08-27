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


#ifndef TIARY_MAIN_PREF_EDIT_H
#define TIARY_MAIN_PREF_EDIT_H

#include "diary/config.h"

namespace tiary {

void edit_options (GlobalOptionGroup &);
void edit_options (PerFileOptionGroup &);

} //namespace tiary

#endif // include guard
