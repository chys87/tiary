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


#ifndef TIARY_UI_CHECKBOX_LABEL_H
#define TIARY_UI_CHECKBOX_LABEL_H

#include "ui/checkbox.h"
#include "ui/label.h"

namespace tiary {
namespace ui {


class CheckBoxLabel : public MovableObject
{
public:
	CheckBox checkbox;
	Label label;

	CheckBoxLabel(Window &, std::wstring_view, bool initial_status = false);
	CheckBoxLabel(Window &, std::wstring &&, bool initial_status = false);
	CheckBoxLabel(Window &win, const wchar_t *text,bool initial_status = false) : CheckBoxLabel(win, std::wstring_view(text), initial_status) {}
	~CheckBoxLabel ();

	void move_resize (Size, Size);

	void set_status (bool status, bool emit_signal = true) { checkbox.set_status (status, emit_signal); }
	bool get_status () const { return checkbox.get_status (); }
};


} // namespace tiary::ui
} // namespace tiary

#endif
