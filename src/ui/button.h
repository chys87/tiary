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


#ifndef TIARY_UI_BUTTON_H
#define TIARY_UI_BUTTON_H

#include "ui/control.h"
#include "ui/uistring.h"
#include "common/signal.h"
#include <string>

namespace tiary {
namespace ui {


class Button : public Control
{
public:
	// The text must be a single line
	// Otherwise, the behavior is undefined
	Button (Dialog &, const std::wstring &);
	~Button ();

	bool on_key (wchar_t);
//	bool on_mouse (MouseEvent); // Handled by Dialog
	void on_focus_changed ();
	void redraw ();

	static const unsigned DEFAULT_BUTTON = 1;
	static const unsigned ESCAPE_BUTTON  = 2;

	void set_attribute (unsigned);

private:
	UIStringOne text;
	unsigned attributes;
};




} // namespace tiary::ui
} // namespace tiary

#endif // include guard
