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


#include "ui/label.h"
#include "ui/window.h"
#include "ui/paletteid.h"
#include "common/signal.h"

namespace tiary {
namespace ui {


Label::Label (Window &win, std::wstring_view str, unsigned options)
	: Control(win, kUnfocusable)
	, text (str, options)
{
	// Register hotkey
	if (wchar_t c = text.get_hotkey ()) {
		win.register_hotkey (c, Signal (sig_hotkey, 0),
				Hotkeys::CASE_INSENSITIVE | Hotkeys::ALLOW_ALT);
		sig_hotkey.connect (win, &Window::set_focus_ptr, this, 1);
	}
}

Label::Label(Window &win, std::wstring &&str, unsigned options)
	: Control(win, kUnfocusable)
	, text(std::move(str), options) {
	// Register hotkey
	if (wchar_t c = text.get_hotkey ()) {
		win.register_hotkey (c, Signal (sig_hotkey, 0),
				Hotkeys::CASE_INSENSITIVE | Hotkeys::ALLOW_ALT);
		sig_hotkey.connect (win, &Window::set_focus_ptr, this, 1);
	}
}

Label::~Label ()
{
}

void Label::redraw ()
{
	choose_palette (PALETTE_ID_LABEL);
	clear ();
	text.output(*this, Size{}, get_size());
}

void Label::set_text (const std::wstring &str, unsigned options)
{
	text.set_text (str, options);
	Label::redraw ();
}

void Label::set_text(std::wstring &&str, unsigned options) {
	text.set_text(std::move(str), options);
	Label::redraw();
}


} // namespace tiary::ui
} // namespace tiary
