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

	void on_focus_changed() override;

	/**
	 * @brief	Set default button rules
	 */
	void set_default_button(Button *default_button) { default_default_ = default_button; }
	void set_default_button(Button &default_button) { set_default_button(&default_button); }

	void set_default_button(Button *default_button, std::map<Control *, Button *> &&focus_defaults) {
		default_default_ = default_button;
		focus_default_map_ = std::move(focus_defaults);
	}

	Button *get_current_default_button () const { return current_default_; }

private:
	Button *current_default_ = nullptr; // Updated by on_focus_changed

	Button *default_default_ = nullptr;
	std::map<Control *, Button *> focus_default_map_;

	void slot_default_button ();
	bool cond_default_button () const; // Whether the default button is usable
	void redraw_all_buttons ();
};


} // namespace tiary::ui
} // namespace tiary


#endif // include guard
