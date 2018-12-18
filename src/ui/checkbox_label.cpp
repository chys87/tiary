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


#include "ui/checkbox_label.h"
#include "ui/window.h"


namespace tiary {
namespace ui {


CheckBoxLabel::CheckBoxLabel (Window &win, const std::wstring &text, bool initial_status)
	: checkbox (win, initial_status)
	, label (win, text)
{
	label.sig_clicked.connect(std::vector<Signal>{
			Signal(win, &Window::set_focus_ptr, &checkbox, 0),
			Signal(checkbox, &CheckBox::toggle, true)
		});
	label.sig_hotkey = label.sig_clicked.signal();
}

CheckBoxLabel::CheckBoxLabel(Window &win, std::wstring &&text, bool initial_status)
	: checkbox(win, initial_status)
	, label(win, std::move(text)) {
	label.sig_clicked.connect(std::vector<Signal>{
			Signal(win, &Window::set_focus_ptr, &checkbox, 0),
			Signal(checkbox, &CheckBox::toggle, true)
		});
	label.sig_hotkey = label.sig_clicked.signal();
}

CheckBoxLabel::~CheckBoxLabel ()
{
}

void CheckBoxLabel::move_resize (Size new_pos, Size new_size)
{
	pos = new_pos;
	size = new_size;
	if (both(size >= Size{4, 1})) {
		checkbox.move_resize(new_pos, {3, 1});
		label.move_resize(new_pos + Size{4, 0}, size - Size{4, 0});
	}
}

} // namespace tiary::ui
} // namespace tiary
