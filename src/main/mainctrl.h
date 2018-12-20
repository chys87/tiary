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

/**
 * @file	main/mainctrl.h
 * @author	chys <admin@chys.info>
 * @brief	Header for the "Main Control" class
 */

#ifndef TIARY_MAIN_MAINCTRL_H
#define TIARY_MAIN_MAINCTRL_H

#include "ui/control.h"
#include "ui/scroll.h"

namespace tiary {

class MainWin;

/**
 * @brief	The "main control"
 *
 * The so-called main control displays the list of diary entries, as used by
 * tiary::MainWin
 *
 * The menu is not part of this control.
 */
class MainCtrl final : public ui::Control, private ui::Scroll {
private:
	// Everything is private.
	// Only friend class MainWin can instantiate MainCtrl

	// Constructor & destructor
	explicit MainCtrl (MainWin &);
	~MainCtrl ();

	// Overload virtual functions
	bool on_mouse (ui::MouseEvent);
	void redraw ();

	MainWin &w ();
	const MainWin &w () const;

	friend class MainWin;

	inline unsigned get_current_focus () const { return ui::Scroll::get_focus (); }

	void set_focus (unsigned); ///< Change focus
	void set_focus_up (); ///< As if UP is pressed
	void set_focus_down (); ///< As if DOWN is pressed
	void set_focus_pageup (); ///< As if PAGEUP is pressed
	void set_focus_pagedown (); ///< As if PAGEDOWN is pressed
	void touch (); ///< Call this when any entry is modified
};


} // namespace tiary

#endif // include guard
