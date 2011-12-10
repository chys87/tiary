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

#ifndef TIARY_UI_BUTTON_DEFAULT_H
#define TIARY_UI_BUTTON_DEFAULT_H

#include "ui/window.h"
#include <map>

namespace tiary {
namespace ui {

class Control;
class Button;

/**
 * @brief	A class that helps deciding the default button of a Window
 *
 * A "default button" is the one triggered when Enter is pressed, regardless
 * of focus.
 *
 * A focused button is always the current default button.
 */

class ButtonDefault : public virtual Window
{
public:
	ButtonDefault ();
	~ButtonDefault ();

	void on_focus_changed ();

	/**
	 * @brief	Set the default "default button"
	 */
	void set_default_button (Button * = 0);
	void set_default_button (Button &btn) { set_default_button (&btn); }

	Button *get_current_default_button () const { return current_default; }

private:
	Button *default_default;

	Button *current_default; // Updated by on_focus_changed

	void slot_default_button ();
	bool cond_default_button () const; // Whether the default button is usable
	void redraw_all_buttons ();

	friend class ButtonDefaultExtended;
};


/**
 * @brief	An "extended" version of tiary::ui::ButtonDefault
 *
 * This class allows the default button to change depending on the current focus control
 */
class ButtonDefaultExtended : public ButtonDefault
{
public:
	ButtonDefaultExtended ();
	~ButtonDefaultExtended ();

	/**
	 * @brief	Set a pair of focus-default controls
	 */
	void set_special_default_button (Control *, Button *);
	void set_special_default_button (Control *ctrl, Button &btn) { set_special_default_button (ctrl, &btn); }
	void set_special_default_button (Control &ctrl, Button *btn) { set_special_default_button (&ctrl, btn); }
	void set_special_default_button (Control &ctrl, Button &btn) { set_special_default_button (&ctrl, &btn); }

	void on_focus_changed ();

private:

	typedef std::map <Control *, Button *> SpecialMap;

	SpecialMap special_map;
};


} // namespace tiary::ui
} // namespace tiary


#endif // include guard
