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

	CheckBoxLabel (Window &, const std::wstring &, bool initial_status = false);
	~CheckBoxLabel ();

	void move_resize (Size, Size);

	void set_status (bool status, bool emit_signal = true) { checkbox.set_status (status, emit_signal); }
	bool get_status () const { return checkbox.get_status (); }
};


} // namespace tiary::ui
} // namespace tiary

#endif
