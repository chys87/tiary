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


#ifndef TIARY_MAIN_DIALOG_LABELS_H
#define TIARY_MAIN_DIALOG_LABELS_H

#include "common/containers.h"
#include <vector>

namespace tiary {


struct DiaryEntry;

/**
 * @brief	Display a window to allow the user to edit labels
 * @param	labels	The set of labels to edit
 * @param	entries	The list of all entries
 * @result	If anything is changed
 */
bool edit_labels (WStringLocaleOrderedSet &labels, const std::vector<DiaryEntry *> &entries);

} // namespace tiary

#endif // include guard
