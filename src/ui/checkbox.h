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


#ifndef TIARY_UI_CHECKBOX_H
#define TIARY_UI_CHECKBOX_H

#include "ui/control.h"

namespace tiary {
namespace ui {

class CheckBox : public virtual Control, public FocusColorControl
{
public:
	explicit CheckBox (Window &, bool = false);
	~CheckBox ();

	void set_status (bool, bool emit_signal = true);
	void toggle (bool emit_signal = true);
	bool get_status () const { return status; }

	bool on_key (wchar_t);
	bool on_mouse (MouseEvent);
	void redraw ();

	Signal sig_toggled;

private:
	bool status;
};


} // namespace tiary::ui
} // namespace tiary


#endif // Include guard
