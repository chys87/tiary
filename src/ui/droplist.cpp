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


#include "ui/droplist.h"
#include "ui/uistring.h"
#include "ui/menu.h"
#include "ui/paletteid.h"
#include <algorithm>
#include <wctype.h>


namespace tiary {
namespace ui {


DropList::DropList (Window &dlg, const ItemList &items_, size_t default_select)
	: Control (dlg)
	, items (items_.empty () ? ItemList (1) : items_)
	, select (default_select < items_.size () ? default_select : 0)
{
}

#ifdef TIARY_HAVE_RVALUE_REFERENCES
DropList::DropList (Window &dlg, ItemList &&items_, size_t default_select)
	: Control (dlg)
	, items (items_.empty () ? ItemList (1) : std::forward<ItemList> (items_))
	, select (default_select < items.size () ? default_select : 0)
{
}
#endif

DropList::~DropList ()
{
}

void DropList::set_select (size_t new_select, bool emit_signal)
{
	if (new_select != select && new_select < items.size ()) {
		select = new_select;
		if (emit_signal)
			sig_select_changed.emit ();
		DropList::redraw ();
	}
}

bool DropList::on_key (wchar_t key)
{
	switch (key) {
		case UP:
			set_select (select - 1);
			return true;

		case DOWN:
			set_select (select + 1);
			return true;

		case HOME:
			set_select (0);
			return true;

		case END:
			set_select (items.size () - 1);
			return true;

		default:
			key = towlower (key);
			// Convert items[k] to const to that it is guaranteed
			// that items[k][0] is meaningful.
			// (In case of an empty string; Per C++ Standard)
			// Convert key to wint_t to suppress a warning
			for (size_t k = select+1; k<items.size(); ++k)
				if (towlower (c(items[k])[0]) == wint_t(key)) {
					set_select (k);
					return true;
				}
			for (size_t k = 0; k <= select; ++k)
				if (towlower (c(items[k])[0]) == wint_t(key)) {
					set_select (k);
					return true;
				}
			return false;
	}
}

bool DropList::on_mouse (MouseEvent mouse_event)
{
	// TODO
	return false;
}

bool DropList::on_focus ()
{
//	DropList::redraw ();
	return true;
}

void DropList::on_defocus ()
{
//	DropList::redraw ();
}

void DropList::redraw ()
{
	choose_palette (PALETTE_ID_DROPLIST);
	clear ();
	UIStringOne (items[select]).output (*this, make_size (), get_size ().x);
}

} // namespace tiary::ui
} // namespace tiary
