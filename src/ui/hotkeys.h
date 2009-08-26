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


#ifndef TIARY_UI_HOTKEYS_H
#define TIARY_UI_HOTKEYS_H

#include "common/signal.h"
#include "common/containers.h"

namespace tiary {
namespace ui {

/**
 * @brief	A class holding a hotkey list
 */
class Hotkeys
{
public:
	Hotkeys ();
	~Hotkeys ();
	Hotkeys (const Hotkeys &);
	Hotkeys &operator = (const Hotkeys &);
#ifdef TIARY_HAVE_RVALUE_REFERENCES
	Hotkeys (Hotkeys &&);
	Hotkeys &operator = (Hotkeys &&);
#endif

	static const int CASE_SENSITIVE = 1;
	/**
	 * If the key is a letter, don't allow Alt + Key
	 */
	static const int DISALLOW_ALT   = 2;
	/**
	 * @brief	Register a hotkey
	 *
	 * Tab and arrow keys are handled internally by tiary::ui::Dialog::on_key.
	 *
	 * Note that in dialogs, the currently focused control has the highest
	 * priority for handling inputs. The hotkey signal is emitted
	 * only if the focused control fails to handle it.
	 */
	void register_hotkey (wchar_t, const Signal &, int options = 0 /* Case insensitive; Allow Alt */);
#ifdef TIARY_HAVE_RVALUE_REFERENCES
	void register_hotkey (wchar_t, Signal &&, int options = 0);
#endif

	bool emit_hotkey (wchar_t);

private:
	typedef unordered_map<wchar_t, Signal> HotkeyList;

	HotkeyList hotkey_list;
};

} // namespace tiary::ui
} // namespace tiary


#endif // include guard
