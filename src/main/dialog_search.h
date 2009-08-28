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


#ifndef TIARY_MAIN_DIALOG_SEARCH_H
#define TIARY_MAIN_DIALOG_SEARCH_H

#include <string>

namespace tiary {


void dialog_search (
		std::wstring &o_text, ///< Output search text
		bool &o_bkwd, ///< Output direction. false = backward
		bool &o_regex, ///< Output regex status
		const std::wstring &text, ///< Default search text
		bool bkwd, ///< Default direction
		bool regex ///< Default regex status
		);




} // namespace tiary

#endif // include gaurd
