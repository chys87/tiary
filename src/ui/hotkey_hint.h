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


#ifndef TIARY_UI_HOTKEY_HINT_H
#define TIARY_UI_HOTKEY_HINT_H

#include "ui/control.h"
#include "common/action.h"
#include <vector>
#include <list>

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
	HotkeyHint &operator () (unsigned weight, const wchar_t *key_name, const wchar_t *fun_name, const Action &act);
	HotkeyHint &operator () (unsigned weight, const wchar_t *key_name, const wchar_t *fun_name, const Signal &sig);
	HotkeyHint &operator () (unsigned weight, const wchar_t *key_name, const wchar_t *fun_name, Action &&act);
	HotkeyHint &operator () (unsigned weight, const wchar_t *key_name, const wchar_t *fun_name, Signal &&sig);

private:
	template <typename... Args>
	HotkeyHint &add(unsigned weight, const wchar_t *key_name, const wchar_t *fun_name, Args&&...args);

private:
	struct HotkeyItem
	{
		const wchar_t *key_name; ///< Name of the key, expected to be a string literal
		const wchar_t *fun_name; ///< Function of the key, expected to be a string literal
		Action action;
		unsigned wid; ///< ucs_width (key_name) + ucs_width (fun_name)
		unsigned weight; ///< The larger, the more important
		unsigned x; ///< Actually position displayed at
	};
	// Holds all HotkeyItem objects
	typedef std::list <HotkeyItem> HotkeyList;
	HotkeyList key_list;
	// Sorted by weight, from largest to smallest
	typedef std::vector <HotkeyItem *> SortedList;
	SortedList sorted_list;

	void construct_sorted_list ();
};

} // namespace tiary::ui
} // namespace tiary

#endif // include guard
