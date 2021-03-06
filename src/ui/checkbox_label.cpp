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


#include "ui/checkbox_label.h"
#include "ui/window.h"


namespace tiary {
namespace ui {


CheckBoxLabel::CheckBoxLabel(Window &win, std::wstring_view text, bool initial_status)
	: checkbox (win, initial_status)
	, label(win, text) {
	common_initialize();
}

CheckBoxLabel::CheckBoxLabel(Window &win, std::wstring &&text, bool initial_status)
	: checkbox(win, initial_status)
	, label(win, std::move(text)) {
	common_initialize();
}

void CheckBoxLabel::common_initialize() {
	label.sig_clicked.connect([this] {
			checkbox.window().set_focus_ptr(&checkbox, 0);
			checkbox.toggle(true);
		});
	label.sig_hotkey = label.sig_clicked.signal();
}

CheckBoxLabel::~CheckBoxLabel ()
{
}

void CheckBoxLabel::move_resize (Size new_pos, Size new_size)
{
	set_pos(new_pos);
	set_size(new_size);
	if (get_size() >= Size{4, 1}) {
		checkbox.move_resize(new_pos, {3, 1});
		label.move_resize(new_pos + Size{4, 0}, get_size() - Size{4, 0});
	}
}

} // namespace tiary::ui
} // namespace tiary
