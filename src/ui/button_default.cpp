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


#include "ui/button_default.h"
#include "ui/button.h"
#include "common/algorithm.h"


namespace tiary {
namespace ui {





ButtonDefault::ButtonDefault ()
	: Window ()
	, default_default (0)
	, current_default (0)
{
	Signal sig_tmp (this, &ButtonDefault::slot_default_button);
	Window::register_hotkey (RETURN, sig_tmp, Hotkeys::CASE_SENSITIVE|Hotkeys::DISALLOW_ALT);
	Window::register_hotkey (NEWLINE, TIARY_STD_MOVE (sig_tmp),
			Hotkeys::CASE_SENSITIVE|Hotkeys::DISALLOW_ALT);
}

ButtonDefault::~ButtonDefault ()
{
}

void ButtonDefault::on_focus_changed ()
{
	Button *default_button = dynamic_cast <Button *> (Window::get_focus ());
	if (default_button == 0) {
		default_button = default_default;
	}

	if (default_button != current_default) {
		current_default = default_button;
		redraw_all_buttons ();
	}
}

void ButtonDefault::redraw_all_buttons ()
{
	const ControlList &control_list = get_control_list ();
	for (ControlList::const_iterator it = control_list.begin ();
			it != control_list.end (); ++it) {
		if (Button *btn = dynamic_cast <Button *> (*it)) {
			btn->Button::redraw ();
		}
	}
}

void ButtonDefault::set_default_button (Button *btn)
{
	default_default = btn;
}

void ButtonDefault::slot_default_button ()
{
	if (Button *btn = get_current_default_button ()) {
		btn->sig_clicked.emit ();
	}
}



ButtonDefaultExtended::ButtonDefaultExtended ()
	: ButtonDefault ()
	, special_map ()
{
}

ButtonDefaultExtended::~ButtonDefaultExtended ()
{
}

void ButtonDefaultExtended::set_special_default_button (Control *focus, Button *btn)
{
	special_map.insert (std::make_pair (focus, btn));
}

void ButtonDefaultExtended::on_focus_changed ()
{
	Button *default_button = dynamic_cast <Button *> (Window::get_focus ());
	if (default_button == 0) {
		SpecialMap::const_iterator it = special_map.find (Window::get_focus ());
		if (it == special_map.end ()) {
			default_button = default_default;
		}
		else {
			default_button = it->second;
		}
	}

	if (default_button != current_default) {
		current_default = default_button;
		redraw_all_buttons ();
	}
}

} // namespace tiary::ui
} // namespace tiary
