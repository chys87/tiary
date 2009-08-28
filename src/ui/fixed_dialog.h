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


#ifndef TIARY_UI_FIXED_DIALOG_H
#define TIARY_UI_FIXED_DIALOG_H


#include "ui/dialog.h"
#include <string>

namespace tiary {
namespace ui {

/*
 * Fixed-size dialog.
 * Automatically positioned in the middle of the screen
 */

class FixedDialog : public Dialog
{
public:
	explicit FixedDialog (unsigned options = 0, const std::wstring & = std::wstring ());
	explicit FixedDialog (Size size, unsigned options = 0, const std::wstring & = std::wstring ());
	virtual ~FixedDialog ();

	void resize (Size);
	void on_winch ();
};

} // namespace tiary::ui
} // namespace tiary

#endif // include guard
