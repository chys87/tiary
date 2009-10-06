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

#ifndef TIARY_MAIN_DIALOG_OPEN_RECENT_H
#define TIARY_MAIN_DIALOG_OPEN_RECENT_H

#include "diary/diary.h"

namespace tiary {


/**
 * @brief	Displays the "Open recent file" dialog
 * @param	lst	[IN/OUT] The list of recent files
 * @param	mod	[OUT] Returns true iff lst was modified
 * @result	The selected file or empty if none were selected
 *
 * Note that the parameter may be modified
 */
std::wstring dialog_open_recent_file (RecentFileList &lst, bool &mod);

} // namespace tiary

#endif // include guard
