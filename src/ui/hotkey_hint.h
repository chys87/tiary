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


#ifndef TIARY_UI_HOTKEY_HINT_H
#define TIARY_UI_HOTKEY_HINT_H

#include "ui/control.h"
#include "common/action.h"
#include <utility>

namespace tiary {
namespace ui {

class HotkeyHint : public UnfocusableControl
{
public:
	HotkeyHint (Window &);
	~HotkeyHint ();

	bool on_mouse (MouseEvent);
	void redraw ();

	// Does not automatically call redraw
	HotkeyHint &operator () (const wchar_t *key_name, const wchar_t *fun_name, const Action &act);
	HotkeyHint &operator () (const wchar_t *key_name, const wchar_t *fun_name, const Signal &sig);
#ifdef TIARY_HAVE_RVALUE_REFERENCES
	HotkeyHint &operator () (const wchar_t *key_name, const wchar_t *fun_name, Action &&act);
	HotkeyHint &operator () (const wchar_t *key_name, const wchar_t *fun_name, Signal &&sig);
#endif // Rvalue references

private:
	struct HotkeyItem
	{
		const wchar_t *key_name; ///< Name of the key, expected to be a string literal
		const wchar_t *fun_name; ///< Function of the key, expected to be a string literal
		unsigned w_key; ///< ucs_width (key_name)
		unsigned w_fun; ///< ucs_width (fun_name)
		Action action;
		unsigned x, y; ///< Actually position displayed
	};
	// The key is the order 
	typedef std::list <HotkeyItem> HotkeyList;
	HotkeyList key_list;
};

} // namespace tiary::ui
} // namespace tiary

#endif // include guard
