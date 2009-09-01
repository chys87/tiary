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


#ifndef TIARY_UI_TEXTBOX_H
#define TIARY_UI_TEXTBOX_H

#include "ui/control.h"
#include "common/signal.h"
#include "ui/scroll.h"
#include <string>

namespace tiary {
namespace ui {

/*
 * Single-line text editor.
 */
class TextBox : public Control, private Scroll
{
public:

	explicit TextBox (Window &, unsigned attr = 0);
	~TextBox ();

	bool on_key (wchar_t);
	bool on_mouse (MouseEvent);
	void move_resize (Size, Size);
	void redraw ();
	unsigned get_item_screen_size (unsigned) const; // Overload an interface in Scroll

	const std::wstring & get_text () const { return text; }

	void set_text (const std::wstring &, bool emit_sig_changed = true, unsigned new_focus = unsigned(-1));
#ifdef TIARY_HAVE_RVALUE_REFERENCES
	void set_text (std::wstring &&, bool emit_sig_changed = true, unsigned new_focus = unsigned(-1));
#endif

	Signal sig_changed;

	static const unsigned PASSWORD_BOX = 1;

private:
	std::wstring text;
	unsigned attributes;
};


} // namespace tiary::ui
} // namespace tiary

#endif // include guard
