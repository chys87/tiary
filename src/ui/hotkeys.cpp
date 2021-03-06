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


#include "ui/hotkeys.h"
#include "ui/ui.h"
#include "common/unicode.h"
#include <utility> // std::move


namespace tiary {
namespace ui {

void Hotkeys::register_hotkey (wchar_t c, const Action &sig)
{
	hotkey_list[c] = sig;
}

void Hotkeys::register_hotkey (wchar_t c, const Action &sig, int options)
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
			if (d != c) {
				register_hotkey (ALT_BASE + d, sig);
			}
		}
	}
	register_hotkey (c, sig);
}

void Hotkeys::register_hotkey (wchar_t c, Action &&sig)
{
	hotkey_list[c] = std::move (sig);
}

void Hotkeys::register_hotkey (wchar_t c, Action &&sig, int options)
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
			if (d != c) {
				register_hotkey (ALT_BASE + d, sig);
			}
		}
	}
	register_hotkey (c, std::move (sig));
}

bool Hotkeys::emit(wchar_t c) {
	HotkeyList::iterator it = hotkey_list.find (c);
	if (it != hotkey_list.end ()) {
		Action &act = it->second;
		if (act.is_really_connected() && act.call_condition()) {
			act.emit ();
			return true;
		}
	}
	return false;
}

} // namespace tiary::ui
} // namespace tiary
