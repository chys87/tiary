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

#include "ui/dialog.h"
#include "common/containers.h"

namespace tiary {
namespace ui {

class Control;
class Button;

/**
 * @brief	A class that helps deciding the default button of a Dialog
 *
 * A "default button" is the one triggered when Enter is pressed, regardless
 * of focus.
 *
 * The default button may change depending on the current focus control
 */

class ButtonDefault : public virtual Dialog
{
public:
	ButtonDefault ();

	/**
	 * @brief	Set the default "default button"
	 */
	void set_default_button (Button * = 0);
	void set_default_button (Button &btn) { set_default_button (&btn); }
	/**
	 * @brief	Set a pair of focus-default controls
	 */
	void set_special_default_button (Control *, Button *);
	void set_special_default_button (Control *ctrl, Button &btn) { set_special_default_button (ctrl, &btn); }
	void set_special_default_button (Control &ctrl, Button *btn) { set_special_default_button (&ctrl, btn); }
	void set_special_default_button (Control &ctrl, Button &btn) { set_special_default_button (&ctrl, &btn); }

	Button *get_current_default_button () const;

private:
	Button *default_default;

	typedef /* No std:: here */unordered_map <Control *, Button *> SpecialMap;

	SpecialMap special_map;


	void slot_default_button ();
};


} // namespace tiary::ui
} // namespace tiary


#endif // include guard
