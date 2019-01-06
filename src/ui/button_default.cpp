// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2016, 2018, 2019, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/

/**
 * @file	ui/button_default.cpp
 * @author	chys <admin@chys.info>
 * @brief	Implements class tiary::ui::ButtonDefault
 */


#include "ui/button_default.h"
#include "ui/button.h"
#include "common/algorithm.h"


namespace tiary {
namespace ui {





ButtonDefault::ButtonDefault ()
	: Window ()
{
	Action sig_tmp(std::piecewise_construct,
			std::make_tuple(this, &ButtonDefault::slot_default_button),
			std::make_tuple(this, &ButtonDefault::cond_default_button));
	Window::register_hotkey (RETURN, sig_tmp);
	Window::register_hotkey (NEWLINE, std::move (sig_tmp));
}

void ButtonDefault::on_focus_changed ()
{
	Control *focus = get_focus();
	// If the focus is a button, it's the default
	Button *default_button = dynamic_cast<Button *>(focus);
	// Otherwise, follow the preset rules
	if (default_button == 0) {
		auto it = focus_default_map_.find(focus);
		if (it == focus_default_map_.end()) {
			default_button = default_default_;
		} else {
			default_button = it->second;
		}
	}

	if (default_button != current_default_) {
		current_default_ = default_button;
		redraw_all_buttons ();
	}
}

void ButtonDefault::redraw_all_buttons ()
{
	for (Control *ctrl = get_dummy_ctrl ()->get_next ();
			ctrl != get_dummy_ctrl (); ctrl = ctrl->get_next ()) {
		if (Button *btn = dynamic_cast <Button *> (ctrl)) {
			btn->redraw();
		}
	}
}

void ButtonDefault::slot_default_button ()
{
	if (Button *btn = get_current_default_button ()) {
		btn->sig_clicked.emit ();
	}
}

bool ButtonDefault::cond_default_button () const
{
	if (Button *btn = get_current_default_button ()) {
		if (btn->sig_clicked.call_condition (true)) {
			return true;
		}
	}
	return false;
}

} // namespace tiary::ui
} // namespace tiary
