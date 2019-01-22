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


#include "ui/button.h"
#include "ui/paletteid.h"
#include "common/algorithm.h"
#include "ui/window.h"
#include "ui/button_default.h"

namespace tiary {
namespace ui {

Button::Button(Window &win, std::wstring_view str)
	: Control(win, kRedrawOnFocusChange)
	, text (str)
{
	// Register hotkey
	if (wchar_t c = text.get_hotkey ()) {
		win.register_hotkey (c, Signal (this, &Button::slot_clicked),
				Hotkeys::CASE_INSENSITIVE|Hotkeys::ALLOW_ALT);
	}
}

Button::Button (Window &win, std::wstring &&str)
	: Control(win, kRedrawOnFocusChange)
	, text(std::move(str))
{
	// Register hotkey
	if (wchar_t c = text.get_hotkey()) {
		win.register_hotkey(c, Signal(this, &Button::slot_clicked),
				Hotkeys::CASE_INSENSITIVE | Hotkeys::ALLOW_ALT);
	}
}

Button::~Button ()
{
}

bool Button::on_key (wchar_t c)
{
	if (c==RETURN || c==NEWLINE || c==L' ') {
		if (sig_clicked.is_really_connected() && sig_clicked.call_condition()) {
			sig_clicked.emit ();
			return true;
		}
	}
	return false;
}

void Button::slot_clicked ()
{
	if (sig_clicked.call_condition()) {
		sig_clicked.emit ();
	}
}

void Button::redraw ()
{
	unsigned y = (get_size().y - 1) / 2;
	unsigned w = minU (get_size().x, text.get_width () + 4);
	unsigned x = (get_size().x - w) / 2;

	PaletteID id;
	if (!sig_clicked.call_condition()) {
		if (is_focus ()) {
			id = PALETTE_ID_BUTTON_FOCUS_INVALID;
		}
		else {
			id = PALETTE_ID_BUTTON_INVALID;
		}
	}
	else if (is_focus ()) {
		id = PALETTE_ID_BUTTON_FOCUS;
	}
	else if (ButtonDefault *def_chooser = dynamic_cast<ButtonDefault *>(&window())) {
		if (def_chooser->get_current_default_button () == this) {
			id = PALETTE_ID_BUTTON_DEFAULT;
		}
		else {
			id = PALETTE_ID_BUTTON_NORMAL;
		}
	}
	else {
		id = PALETTE_ID_BUTTON_NORMAL;
	}

	choose_palette (id);
	Size pos{x, y};
	clear ();
	move_cursor (pos);
	pos = put (pos, (id != PALETTE_ID_BUTTON_NORMAL && id != PALETTE_ID_BUTTON_INVALID) ? L"> " : L"  ");
	pos = text.output (*this, pos, w-4);
	pos = {x + w - 2, y};
	pos = put (pos, (id != PALETTE_ID_BUTTON_NORMAL && id != PALETTE_ID_BUTTON_INVALID) ? L" <" : L"  ");
}

} // namespace tiary::ui
} // namespace tiary
