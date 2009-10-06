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


#ifndef TIARY_UI_HOTKEYS_H
#define TIARY_UI_HOTKEYS_H

#include "common/action.h"
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

	static const int CASE_INSENSITIVE = 1;
	/**
	 * If the key is a letter, also registers Alt + Key
	 */
	static const int ALLOW_ALT   = 2;
	/**
	 * @brief	Register a hotkey
	 *
	 * Tab and arrow keys are handled internally by tiary::ui::Window::on_key.
	 *
	 * Note that in dialogs, the currently focused control has the highest
	 * priority for handling inputs. The hotkey signal is emitted
	 * only if the focused control fails to handle it.
	 */
	void register_hotkey (wchar_t, const Action &);
	void register_hotkey (wchar_t, const Action &, int options);
#ifdef TIARY_HAVE_RVALUE_REFERENCES
	void register_hotkey (wchar_t, Action &&);
	void register_hotkey (wchar_t, Action &&, int options);
#endif

	bool emit_hotkey (wchar_t);

private:
	typedef unordered_map<wchar_t, Action> HotkeyList;

	HotkeyList hotkey_list;
};

} // namespace tiary::ui
} // namespace tiary


#endif // include guard
