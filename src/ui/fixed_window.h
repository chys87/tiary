// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2018, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#ifndef TIARY_UI_FIXED_DIALOG_H
#define TIARY_UI_FIXED_DIALOG_H

/**
 * @file	ui/fixed_window.h
 * @author	chys <admin@chys.info>
 * @brief	Declares class tiary::ui::FixedWindow
 */

#include "ui/window.h"
#include <string>

namespace tiary {
namespace ui {

/*
 * Fixed-size window.
 * Automatically positioned in the middle of the screen
 */

class FixedWindow : public virtual Window
{
public:

	void resize (Size);
	void on_winch() override final;
};

} // namespace tiary::ui
} // namespace tiary

#endif // include guard
