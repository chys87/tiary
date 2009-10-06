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

#include "ui/hotkey_hint.h"
#include "ui/mouse.h"
#include "ui/paletteid.h"
#include "common/algorithm.h"
#include "common/unicode.h"

namespace tiary {
namespace ui {

HotkeyHint::HotkeyHint (Window &win)
	: Control (win)
	, UnfocusableControl (win)
	, key_list ()
{
}

HotkeyHint::~HotkeyHint ()
{
}

void HotkeyHint::redraw ()
{
	choose_palette (PALETTE_ID_HOTKEY_HINT);
	clear ();

	unsigned width = get_size ().x;
	unsigned height = get_size ().y;
	Size pos = make_size ();
	for (HotkeyList::iterator it = key_list.begin (); it != key_list.end (); ++it) {
		if (!it->action.call_condition (true)) {
			it->y = unsigned (-1);
			continue;
		}
		unsigned required_width = it->w_key + it->w_fun + 1;
		if (int (width - pos.x) < int (required_width)) {
			if (++pos.y >= height) {
				do {
					it->y = unsigned (-1);
				} while (++it != key_list.end ());
				break;
			}
			pos.x = 0;
		}
		// Remember its position
		it->x = pos.x;
		it->y = pos.y;
		// Display this item
		choose_palette (PALETTE_ID_HOTKEY_HINT_KEY);
		pos = put (pos, it->key_name);
		choose_palette (PALETTE_ID_HOTKEY_HINT);
		pos = put (pos, L'-');
		pos = put (pos, it->fun_name);
		pos = put (pos, L"  ");
	}
}

bool HotkeyHint::on_mouse (MouseEvent mouse_event)
{
	if (!(mouse_event.m & LEFT_CLICK)) {
		return false;
	}
	unsigned y = mouse_event.p.y;
	unsigned x = mouse_event.p.x;
	for (HotkeyList::iterator it = key_list.begin (); it != key_list.end (); ++it) {
		if (it->y==y && (x - it->x < it->w_key + it->w_fun + 1)) {
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
		it->w_key = ucs_width (key_name);\
		it->fun_name = fun_name;\
		it->w_fun = ucs_width (fun_name);\
		it->action = action__;\
		it->y = unsigned (-1);\
	} while (0)

HotkeyHint &HotkeyHint::operator () (const wchar_t *key_name, const wchar_t *fun_name, const Action &action)
{
	APPEND_ITEM (action);
	return *this;
}

HotkeyHint &HotkeyHint::operator () (const wchar_t *key_name, const wchar_t *fun_name, const Signal &signal)
{
	APPEND_ITEM (signal);
	return *this;
}

#ifdef TIARY_HAVE_RVALUE_REFERENCES
HotkeyHint &HotkeyHint::operator () (const wchar_t *key_name, const wchar_t *fun_name, Action &&action)
{
	APPEND_ITEM (std::move (action));
	return *this;
}

HotkeyHint &HotkeyHint::operator () (const wchar_t *key_name, const wchar_t *fun_name, Signal &&signal)
{
	APPEND_ITEM (std::move (signal));
	return *this;
}
#endif


} // namespace tiary::ui
} // namespace tiary
