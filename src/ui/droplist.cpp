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


#include "ui/droplist.h"
#include "ui/paletteid.h"
#include "ui/dialog_select.h"
#include "ui/mouse.h"
#include "common/algorithm.h"
#include <algorithm>
#include <wctype.h>


namespace tiary {
namespace ui {


DropList::DropList (Window &win, const ItemList &items, size_t default_select)
	: Control(win, kRedrawOnFocusChange)
	, items_(items.empty() ? ItemList(1) : items)
	, select_(default_select < items_.size () ? default_select : 0)
{
}

DropList::DropList(Window &win, ItemList &&items, size_t default_select)
	: Control(win, kRedrawOnFocusChange)
	, items_(items.empty() ? ItemList(1) : std::move(items))
	, select_(default_select < items_.size () ? default_select : 0)
{
}

DropList::~DropList ()
{
}

void DropList::set_select (size_t new_select, bool emit_signal)
{
	if (new_select != select_ && new_select < items_.size()) {
		select_ = new_select;
		if (emit_signal) {
			sig_select_changed.emit ();
		}
		DropList::redraw ();
	}
}

namespace {

void dialog_select_set (DropList *drp)
{
	size_t sel = dialog_select (L"Select", drp->get_items (), drp->get_select ());
	if (sel < drp->get_items ().size ()) {
		drp->set_select (sel);
	}
}

} // anonymous namespace

bool DropList::on_key (wchar_t key)
{
	switch (key) {
		case UP:
			set_select(select_ - 1);
			return true;

		case DOWN:
			set_select(select_ + 1);
			return true;

		case HOME:
			set_select (0);
			return true;

		case END:
			set_select (items_.size () - 1);
			return true;

		case L' ':
			dialog_select_set (this);
			return true;

		default:
			key = towlower (key);
			// Convert items_[k] to const to that it is guaranteed
			// that items_[k][0] is meaningful.
			// (In case of an empty string; Per C++ Standard)
			// Convert key to wint_t to suppress a warning
			for (size_t k = select_ + 1; k < items_.size(); ++k) {
				if (towlower(items_[k][0]) == wint_t(key)) {
					set_select (k);
					return true;
				}
			}
			for (size_t k = 0; k <= select_; ++k) {
				if (towlower(items_[k][0]) == wint_t(key)) {
					set_select (k);
					return true;
				}
			}
			return false;
	}
}

bool DropList::on_mouse (MouseEvent mouse_event)
{
	if (mouse_event.m & MOUSE_ALL_BUTTON) {
		dialog_select_set (this);
		return true;
	}
	return false;
}

void DropList::redraw ()
{
	choose_palette (is_focus () ? PALETTE_ID_DROPLIST_FOCUS : PALETTE_ID_DROPLIST);
	clear ();
	put({}, items_[select_]);
}

} // namespace tiary::ui
} // namespace tiary
