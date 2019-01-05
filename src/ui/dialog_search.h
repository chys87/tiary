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


#ifndef TIARY_UI_DIALOG_SEARCH_H
#define TIARY_UI_DIALOG_SEARCH_H

#include <string>

namespace tiary {
namespace ui {

struct SearchDesc {
	std::wstring text;
	bool backward = false; ///< Direction. true = backward
	bool regex = false; ///< Use regular expression or not
};


void dialog_search(SearchDesc *output, const SearchDesc &default_search);



} // namespace tiary::ui
} // namespace tiary

#endif // include gaurd
