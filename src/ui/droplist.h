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


#ifndef TIARY_UI_DROPLIST_H
#define TIARY_UI_DROPLIST_H

#include "ui/control.h"
#include <vector>

namespace tiary {
namespace ui {

class DropList : public Control
{
public:

	typedef std::vector<std::wstring> ItemList;

	DropList (Window &, const ItemList &, size_t default_select);
#ifdef TIARY_HAVE_RVALUE_REFERENCES
	DropList (Window &, ItemList &&, size_t default_select);
#endif
	~DropList ();

	size_t get_select () const { return select; }
	const ItemList &get_items () const { return items; }

	void set_select (size_t, bool emit_signal = true);

	// Implement virtual functions
	bool on_key (wchar_t);
	bool on_mouse (MouseEvent);
	bool on_focus ();
	void on_defocus ();
//	void on_move_resize (Size, Size);
	void redraw ();

	Signal sig_select_changed;

private:
	const ItemList items;
	size_t select;
};

} // namespace tiary::ui
} // namespace tiary

#endif // include guard
