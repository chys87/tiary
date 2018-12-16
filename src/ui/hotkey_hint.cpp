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
	, UnfocusableControl (win)
	, key_list ()
	, sorted_list ()
{
}

HotkeyHint::~HotkeyHint ()
{
}

namespace {

// This template is a trick:
// HotkeyHint::HotkeyItem is inaccessible here
struct LargerWeight {
	template <typename HotkeyItem>
	bool operator () (const HotkeyItem *a, const HotkeyItem *b) const
	{
		return (a->weight > b->weight);
	}
};


} // anonymous namespace

void HotkeyHint::construct_sorted_list ()
{
	sorted_list.resize (key_list.size ());
	SortedList::iterator iw = sorted_list.begin ();
	for (HotkeyList::iterator it=key_list.begin(), e=key_list.end();
			it != e; ++it) {
		*iw++ = &*it;
	}
	std::sort(sorted_list.begin(), iw, LargerWeight());
}

void HotkeyHint::redraw ()
{
	// First check if sorted_list has been properly constructed
	if (sorted_list.size () != key_list.size ()) {
		construct_sorted_list ();
	}

	unsigned left_width = get_size ().x;

	// Mark items to be displayed at position 0, and those not to be displayed
	// at position -1
	for (SortedList::iterator it = sorted_list.begin(), e=sorted_list.end();
			it != e; ++it) {
		unsigned required_width = (*it)->wid + 1;
		if ((left_width >= required_width) && (*it)->action.call_condition (true)) {
			left_width -= required_width;
			if (left_width) {
				--left_width;
			}
			(*it)->x = 0;
		}
		else {
			(*it)->x = unsigned (-1);
		}
	}

	// Now actually display items

	choose_palette (PALETTE_ID_HOTKEY_HINT);
	clear ();

	unsigned x = 0;
	for (HotkeyList::iterator it = key_list.begin (); it != key_list.end (); ++it) {
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
	for (HotkeyList::iterator it = key_list.begin (); it != key_list.end (); ++it) {
		if ((it->x != unsigned (-1)) && (x - it->x <= it->wid)) {
			it->action.emit ();
			return true;
		}
	}
	return false;
}

#define APPEND_ITEM(action__)	\
	do {\
		HotkeyList::iterator it = key_list.insert (key_list.end (), HotkeyItem ());\
		it->key_name = key_name;\
		it->fun_name = fun_name;\
		it->wid = ucs_width (key_name) + ucs_width (fun_name);\
		it->action = action__;\
		it->weight = weight;\
		it->x = unsigned (-1);\
	} while (0)

HotkeyHint &HotkeyHint::operator () (unsigned weight, const wchar_t *key_name, const wchar_t *fun_name, const Action &action)
{
	APPEND_ITEM (action);
	return *this;
}

HotkeyHint &HotkeyHint::operator () (unsigned weight, const wchar_t *key_name, const wchar_t *fun_name, const Signal &signal)
{
	APPEND_ITEM (signal);
	return *this;
}

HotkeyHint &HotkeyHint::operator () (unsigned weight, const wchar_t *key_name, const wchar_t *fun_name, Action &&action)
{
	APPEND_ITEM (std::move (action));
	return *this;
}

HotkeyHint &HotkeyHint::operator () (unsigned weight, const wchar_t *key_name, const wchar_t *fun_name, Signal &&signal)
{
	APPEND_ITEM (std::move (signal));
	return *this;
}


} // namespace tiary::ui
} // namespace tiary
