// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the so-called 3-clause BSD license.
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

Hotkeys::Hotkeys (const Hotkeys &other)
	: hotkey_list (other.hotkey_list)
{
}

Hotkeys &Hotkeys::operator = (const Hotkeys &other)
{
	hotkey_list = other.hotkey_list;
	return *this;
}

#ifdef TIARY_HAVE_RVALUE_REFERENCES
Hotkeys::Hotkeys (Hotkeys &&other)
	: hotkey_list (std::forward<HotkeyList> (other.hotkey_list))
{
}

Hotkeys &Hotkeys::operator = (Hotkeys &&other)
{
	hotkey_list = std::forward<HotkeyList> (other.hotkey_list);
	return *this;
}
#endif

void Hotkeys::register_hotkey (wchar_t c, const Signal &sig, int options)
{
	wchar_t d = c;
	if (!(options & CASE_SENSITIVE)) {
		d = ucs_reverse_case (c);
		if (d != c)
			hotkey_list[d] = sig;
	}
	if (!(options & DISALLOW_ALT)) {
		if (unsigned (c - L'A') < 26 || unsigned (c - L'a') < 26) {
			hotkey_list[ALT_BASE + c] = sig;
			if (d != c)
				hotkey_list[ALT_BASE + d] = sig;
		}
	}
	hotkey_list[c] = sig;
}

#ifdef TIARY_HAVE_RVALUE_REFERENCES
void Hotkeys::register_hotkey (wchar_t c, Signal &&sig, int options)
{
	wchar_t d = c;
	if (!(options & CASE_SENSITIVE)) {
		d = ucs_reverse_case (c);
		if (d != c)
			hotkey_list[d] = sig; // Copy semantics
	}
	if (!(options & DISALLOW_ALT)) {
		if (unsigned (c - L'A') < 26 || unsigned (c - L'a') < 26) {
			hotkey_list[ALT_BASE + c] = sig; // Copy semantics
			if (d != c)
				hotkey_list[ALT_BASE + d] = sig; // Copy semantics
		}
	}
	hotkey_list[c] = std::forward<Signal> (sig); // Move semantics
}
#endif

bool Hotkeys::emit_hotkey (wchar_t c)
{
	HotkeyList::iterator it = hotkey_list.find (c);
	if (it != hotkey_list.end ()) {
		it->second.emit ();
		return true;
	} else
		return false;
}

} // namespace tiary::ui
} // namespace tiary
