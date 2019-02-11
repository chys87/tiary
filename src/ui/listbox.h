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


#ifndef TIARY_UI_LISTBOX_H
#define TIARY_UI_LISTBOX_H

#include "ui/control.h"
#include "ui/scroll.h"
#include <vector>

namespace tiary {
namespace ui {

class ListBox final : public Control {
public:

	typedef std::vector<std::wstring> ItemList;

	explicit ListBox (Window &);
	~ListBox ();

	size_t get_select () const; // -1 if none
	const ItemList &get_items () const { return items; }
	bool is_valid_select () const { return select_any; }

	void set_items (const ItemList &, size_t new_select = size_t(-1), bool emit_signal = true);
	void set_items (ItemList &&, size_t = size_t(-1), bool emit_signal = true);

	void set_select (size_t /* -1 = none */, bool emit_signal = true, bool scroll_to_top = false);

	// Interfaces for ancesters
	bool on_key (wchar_t);
	bool on_mouse (MouseEvent);
	bool on_focus ();
	void on_defocus ();
	void move_resize (Size, Size);
	void redraw ();

	Signal sig_select_changed;
	Signal sig_double_clicked; // If double clicked, first emit sig_select_changed, then sig_double_click

private:
	Scroll scroll_;
	ItemList items;
	bool select_any;
};

} // namespace tiary::ui
} // namespace tiary


#endif // Include guard
