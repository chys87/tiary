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


#ifndef TIARY_UI_DROPLIST_H
#define TIARY_UI_DROPLIST_H

#include "ui/control.h"
#include <vector>

namespace tiary {
namespace ui {

class DropList final : public virtual Control, public FocusColorControl {
public:

	typedef std::vector<std::wstring> ItemList;

	DropList (Window &, const ItemList &, size_t default_select);
	DropList (Window &, ItemList &&, size_t default_select);
	~DropList ();

	size_t get_select() const { return select_; }
	const ItemList &get_items() const { return items_; }

	void set_select (size_t, bool emit_signal = true);

	// Implement virtual functions
	bool on_key (wchar_t);
	bool on_mouse (MouseEvent);
	void redraw ();

	Signal sig_select_changed;

private:
	const ItemList items_;
	size_t select_;
};

} // namespace tiary::ui
} // namespace tiary

#endif // include guard
