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


#ifndef TIARY_MAIN_DIALOG_EDIT_TIME_H
#define TIARY_MAIN_DIALOG_EDIT_TIME_H

#include "diary/diary.h"

namespace tiary {

struct DiaryEntry;

/**
 * @brief	Edit the time for an entry
 * @result	Whether there is any modification
 */
bool edit_entry_time (DiaryEntry &);


} // namespace tiary

#endif // include guard
