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


#include "ui/checkbox_label.h"
#include "ui/window.h"
#include "common/container_of.h"


namespace tiary {
namespace ui {


CheckBoxLabel::CheckBoxLabel (Window &win, const std::wstring &text, bool initial_status)
	: checkbox (win, initial_status)
	, label (win, text)
{
	label.sig_clicked.connect (
			TIARY_LIST_OF(Signal)
				Signal (win, &Window::set_focus_ptr, &checkbox, 0),
				Signal (checkbox, &CheckBox::toggle, true)
			TIARY_LIST_OF_END
		);
	label.sig_hotkey = label.sig_clicked.signal;
}

CheckBoxLabel::~CheckBoxLabel ()
{
}

void CheckBoxLabel::move_resize (Size new_pos, Size new_size)
{
	pos = new_pos;
	size = new_size;
	if (both (size >= make_size (4, 1))) {
		checkbox.move_resize (new_pos, make_size (3,1));
		label.move_resize (new_pos + make_size (4, 0), size - make_size (4, 0));
	}
}

} // namespace tiary::ui
} // namespace tiary
