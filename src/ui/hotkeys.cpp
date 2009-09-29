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


#include "ui/hotkeys.h"
#include "ui/ui.h"
#include "common/unicode.h"
#include <utility> // std::forward


namespace tiary {
namespace ui {

Hotkeys::Hotkeys ()
	: hotkey_list ()
{
}

Hotkeys::~Hotkeys ()
{
}

void Hotkeys::register_hotkey (wchar_t c, const Signal &sig)
{
	hotkey_list[c] = sig;
}

void Hotkeys::register_hotkey (wchar_t c, const Signal &sig, int options)
{
	wchar_t d = c;
	if (options & CASE_INSENSITIVE) {
		d = ucs_reverse_case (c);
		if (d != c) {
			register_hotkey (d, sig);
		}
	}
	if (options & ALLOW_ALT) {
		if (unsigned (c - L'A') < 26 || unsigned (c - L'a') < 26) {
			register_hotkey (ALT_BASE + c, sig);
			hotkey_list[ALT_BASE + c] = sig;
			if (d != c) {
				register_hotkey (ALT_BASE + d, sig);
			}
		}
	}
	register_hotkey (c, sig);
}

#ifdef TIARY_HAVE_RVALUE_REFERENCES
void Hotkeys::register_hotkey (wchar_t c, Signal &&sig)
{
	hotkey_list[c] = std::forward <Signal> (sig);
}

void Hotkeys::register_hotkey (wchar_t c, Signal &&sig, int options)
{
	wchar_t d = c;
	if (options & CASE_INSENSITIVE) {
		d = ucs_reverse_case (c);
		if (d != c) {
			register_hotkey (d, sig);
		}
	}
	if (options & ALLOW_ALT) {
		if (unsigned (c - L'A') < 26 || unsigned (c - L'a') < 26) {
			register_hotkey (ALT_BASE + c, sig);
			hotkey_list[ALT_BASE + c] = sig;
			if (d != c) {
				register_hotkey (ALT_BASE + d, sig);
			}
		}
	}
	register_hotkey (c, std::forward <Signal> (sig));
}
#endif

bool Hotkeys::emit_hotkey (wchar_t c)
{
	HotkeyList::iterator it = hotkey_list.find (c);
	if (it != hotkey_list.end ()) {
		it->second.emit ();
		return true;
	}
	else {
		return false;
	}
}

} // namespace tiary::ui
} // namespace tiary
