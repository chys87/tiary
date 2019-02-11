// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2019, chys <admin@CHYS.INFO>
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
class TextBox final : public Control {
public:

	explicit TextBox (Window &, unsigned attr = 0);
	~TextBox ();

	bool on_key (wchar_t);
	bool on_mouse (MouseEvent);
	void move_resize (Size, Size);
	void redraw ();

	const std::wstring & get_text() const { return text_; }

	// Does not change cursor_position
	void set_text(std::wstring_view, bool emit_sig_changed = true);
	void set_text(std::wstring_view, bool emit_sig_changed, unsigned new_cursor_pos);

	Signal sig_changed;

	static const unsigned PASSWORD_BOX = 1;

private:
	unsigned get_item_screen_size(unsigned) const; /// Provides callback for Scroll

private:
	Scroll scroll_;
	std::wstring text_;
	unsigned attributes_;
};


} // namespace tiary::ui
} // namespace tiary

#endif // include guard
