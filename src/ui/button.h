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


#ifndef TIARY_UI_BUTTON_H
#define TIARY_UI_BUTTON_H

#include "ui/control.h"
#include "ui/uistring_one.h"
#include <string>

namespace tiary {
namespace ui {


class Button final : public Control {
public:
	// The text must be a single line
	// Otherwise, the behavior is undefined
	Button(Window &, std::wstring_view);
	Button(Window &, std::wstring &&);
	Button(Window &win, const wchar_t *text) : Button(win, std::wstring_view(text)) {}
	~Button ();

	bool on_key (wchar_t);
//	bool on_mouse (MouseEvent); // Handled by Window
	void redraw ();

	void slot_clicked (); ///< Intentionally public

private:
	void common_initialize();

private:
	UIStringOne text_;
};




} // namespace tiary::ui
} // namespace tiary

#endif // include guard
