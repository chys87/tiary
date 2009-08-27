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


#include "ui/button.h"
#include "ui/paletteid.h"
#include "common/algorithm.h"
#include "ui/dialog.h"

namespace tiary {
namespace ui {

Button::Button (Dialog &dlg, const std::wstring &str)
	: Control (dlg)
	, text (str)
	, attributes (0)
{
	// Register hotkey
	if (wchar_t c = text.get_hotkey ())
		dlg.register_hotkey (c, Signal (sig_clicked, 0)); // Connecting to, not copying from sig_clicked
}

Button::~Button ()
{
}

bool Button::on_key (wchar_t c)
{
	if (c==L'\r' || c==L' ') {
		sig_clicked.emit ();
		return true;
	}
	return false;
}

void Button::on_focus_changed ()
{
	Button::redraw ();
}

void Button::redraw ()
{
	Control *focus_ctrl = dlg.get_focus ();
	choose_palette (focus_ctrl == this ? PALETTE_ID_BUTTON_FOCUS : PALETTE_ID_BUTTON_UNFOCUS);
	unsigned y = (get_size().y - 1) / 2;
	unsigned w = minU (get_size().x, text.get_width () + 4);
	unsigned x = (get_size().x - w) / 2;

	bool display_as_if_focus; // Whether display the "><" characters
	if (focus_ctrl == this)
		display_as_if_focus = true;
	else if (!(attributes & DEFAULT_BUTTON))
		display_as_if_focus = false;
	else if (!dynamic_cast<Button *>(focus_ctrl)) // Currently focused on another button. Do not display
		display_as_if_focus = true;
	else
		display_as_if_focus = false;

	Size pos = make_size (x,y);
	clear ();
	move_cursor (pos);
	pos = put (pos, display_as_if_focus ? L"> " : L"  ");
	pos = text.output (*this, pos, w-4);
	pos = make_size (x+w-2, y);
	pos = put (pos, display_as_if_focus ? L" <" : L"  ");
}

void Button::set_attribute (unsigned attr)
{
	attributes = attr;
	if (attr & DEFAULT_BUTTON) {
		Signal sig (sig_clicked, 0); // Connecting to, not copying from, sig_clicked
		dlg.register_hotkey (RETURN, sig, Hotkeys::CASE_SENSITIVE | Hotkeys::DISALLOW_ALT);
		dlg.register_hotkey (NEWLINE, TIARY_STD_MOVE (sig), Hotkeys::CASE_SENSITIVE | Hotkeys::DISALLOW_ALT);
	}
	if (attr & ESCAPE_BUTTON)
		dlg.register_hotkey (ESCAPE, Signal (sig_clicked, 0), Hotkeys::CASE_SENSITIVE | Hotkeys::DISALLOW_ALT);
	redraw ();
}

} // namespace tiary::ui
} // namespace tiary
