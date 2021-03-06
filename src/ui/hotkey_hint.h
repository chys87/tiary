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


#ifndef TIARY_UI_HOTKEY_HINT_H
#define TIARY_UI_HOTKEY_HINT_H

#include "ui/control.h"
#include "common/action.h"
#include <vector>
#include <list>

namespace tiary {
namespace ui {

class HotkeyHint final : public Control {
public:
	HotkeyHint (Window &);
	~HotkeyHint ();

	bool on_mouse(MouseEvent) override;
	bool on_focus() override;
	void redraw() override;

	// Does not automatically call redraw
	HotkeyHint &operator () (unsigned weight, std::wstring_view key_name, std::wstring_view fun_name, const Action &act);
	HotkeyHint &operator () (unsigned weight, std::wstring_view key_name, std::wstring_view fun_name, const Signal &sig);
	HotkeyHint &operator () (unsigned weight, std::wstring_view key_name, std::wstring_view fun_name, Action &&act);
	HotkeyHint &operator () (unsigned weight, std::wstring_view key_name, std::wstring_view fun_name, Signal &&sig);

private:
	template <typename... Args>
	HotkeyHint &add(unsigned weight, std::wstring_view key_name, std::wstring_view fun_name, Args&&...args);

private:
	struct HotkeyItem
	{
		std::wstring_view key_name; ///< Name of the key, expected to be a string literal
		std::wstring_view fun_name; ///< Function of the key, expected to be a string literal
		Action action;
		unsigned wid; ///< ucs_width (key_name) + ucs_width (fun_name)
		unsigned weight; ///< The larger, the more important
		unsigned x; ///< Actually position displayed at
	};
	// Holds all HotkeyItem objects
	std::list <HotkeyItem> key_list_;
	// Sorted by weight, from largest to smallest
	std::vector <HotkeyItem *> sorted_list_;

	void construct_sorted_list ();
};

} // namespace tiary::ui
} // namespace tiary

#endif // include guard
