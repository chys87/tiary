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

#include "ui/hotkey_hint.h"
#include "ui/mouse.h"
#include "ui/paletteid.h"
#include "ui/output.h"
#include "common/algorithm.h"
#include "common/unicode.h"
#include <functional>

namespace tiary {
namespace ui {

HotkeyHint::HotkeyHint (Window &win)
	: Control (win)
	, key_list_()
	, sorted_list_()
{
}

HotkeyHint::~HotkeyHint ()
{
}

void HotkeyHint::construct_sorted_list ()
{
	sorted_list_.resize(key_list_.size());
	auto iw = sorted_list_.begin();
	for (auto it = key_list_.begin(), e = key_list_.end(); it != e; ++it) {
		*iw++ = &*it;
	}
	std::sort(sorted_list_.begin(), iw, [](const auto *a, const auto *b) { return a->weight > b->weight; });
}

bool HotkeyHint::on_focus() {
	return false;
}

void HotkeyHint::redraw ()
{
	// First check if sorted_list has been properly constructed
	if (sorted_list_.size() != key_list_.size()) {
		construct_sorted_list ();
	}

	unsigned left_width = get_size ().x;

	// Mark items to be displayed at position 0, and those not to be displayed
	// at position -1
	for (HotkeyItem *item: sorted_list_) {
		unsigned required_width = item->wid + 1;
		if ((left_width >= required_width) && item->action.call_condition (true)) {
			left_width -= required_width;
			if (left_width) {
				--left_width;
			}
			item->x = 0;
		} else {
			item->x = unsigned (-1);
		}
	}

	// Now actually display items

	choose_palette (PALETTE_ID_HOTKEY_HINT);
	clear ();

	unsigned x = 0;
	for (auto it = key_list_.begin(); it != key_list_.end(); ++it) {
		if (it->x) {
			continue;
		}
		// Remember its position
		it->x = x;
		// Display this item
		Size pos = this
			<< Size{x, 0}
			<< PALETTE_ID_HOTKEY_HINT_KEY
			<< it->key_name
			<< PALETTE_ID_HOTKEY_HINT
			<< L'-'
			<< it->fun_name
			<< L' ';
		x = pos.x;
	}
}

bool HotkeyHint::on_mouse (MouseEvent mouse_event)
{
	if (!(mouse_event.m & LEFT_CLICK)) {
		return false;
	}
	if (mouse_event.p.y != 0) {
		return false;
	}
	unsigned x = mouse_event.p.x;
	for (auto it = key_list_.begin(); it != key_list_.end(); ++it) {
		if ((it->x != unsigned (-1)) && (x - it->x <= it->wid)) {
			it->action.emit ();
			return true;
		}
	}
	return false;
}

template <typename... Args>
HotkeyHint &HotkeyHint::add(unsigned weight, std::wstring_view key_name, std::wstring_view fun_name, Args&&...args) {
	key_list_.push_back({
			key_name, fun_name, Action(std::forward<Args>(args)...),
			unsigned(ucs_width(key_name) + ucs_width(fun_name)), weight, unsigned(-1),
		});
	return *this;
}

HotkeyHint &HotkeyHint::operator () (unsigned weight, std::wstring_view key_name, std::wstring_view fun_name, const Action &action)
{
	return add(weight, key_name, fun_name, action);
}

HotkeyHint &HotkeyHint::operator () (unsigned weight, std::wstring_view key_name, std::wstring_view fun_name, const Signal &signal)
{
	return add(weight, key_name, fun_name, signal);
}

HotkeyHint &HotkeyHint::operator () (unsigned weight, std::wstring_view key_name, std::wstring_view fun_name, Action &&action)
{
	return add(weight, key_name, fun_name, std::move(action));
}

HotkeyHint &HotkeyHint::operator () (unsigned weight, std::wstring_view key_name, std::wstring_view fun_name, Signal &&signal)
{
	return add(weight, key_name, fun_name, std::move(signal));
}


} // namespace tiary::ui
} // namespace tiary
