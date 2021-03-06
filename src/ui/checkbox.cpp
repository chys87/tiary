// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2018, 2019, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#include "ui/checkbox.h"
#include "ui/mouse.h"
#include "ui/paletteid.h"
#include "ui/terminal_emulator.h"

namespace tiary {
namespace ui {

CheckBox::CheckBox (Window &win, bool initial_status)
	: Control(win, kRedrawOnFocusChange)
	, status (initial_status)
{
}

CheckBox::~CheckBox ()
{
}

void CheckBox::set_status (bool new_status, bool emit_signal)
{
	if (new_status != status) {
		status = new_status;
		if (emit_signal) {
			sig_toggled.emit ();
		}
		CheckBox::redraw ();
	}
}

void CheckBox::toggle (bool emit_signal)
{
	status = !status;
	if (emit_signal) {
		sig_toggled.emit ();
	}
	CheckBox::redraw ();
}

bool CheckBox::on_key (wchar_t c)
{
	if (c == L' ') {
		toggle (true);
		return true;
	}
	return false;
}

bool CheckBox::on_mouse (MouseEvent event)
{
	if (event.m & (LEFT_PRESS|LEFT_CLICK)) {
		toggle (true);
		return true;
	}
	return false;
}

void CheckBox::redraw ()
{
	choose_palette (is_focus () ? PALETTE_ID_CHECKBOX_FOCUS : PALETTE_ID_CHECKBOX);
	clear ();
	unsigned wid = get_size().x;
	Size pos{};
	if (wid >= 3) {
		pos = put (pos, L'[');
	}
	move_cursor (pos);
	// U+00D7 is Multipilcation sign
	pos = put (pos, status ? (
				terminal_emulator_correct_wcwidth () ? L'\u00d7' : L'x'
				) : L' ');
	if (wid >= 3) {
		put (pos, L']');
	}
}

} // namespace tiary::ui
} // namespace tiary

