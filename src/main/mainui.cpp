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


#include "main/mainui.h"
#include "main/mainwin.h"

namespace tiary {


int main_body(std::wstring_view initial_filename) {
	MainWin (initial_filename).event_loop ();
	return 0;
}



} // namespace tiary
