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


#include "ui/textbox.h"
#include "ui/ui.h"
#include "ui/paletteid.h"
#include "ui/mouse.h"
#include "common/unicode.h"
#include "common/algorithm.h"
#include <wctype.h>
#include <assert.h>

namespace tiary {
namespace ui {

TextBox::TextBox (Window &win, unsigned attr)
	: Control (win)
	, FocusColorControl (win)
	, Scroll (0, true)
	, text ()
	, attributes (attr)
{
}

TextBox::~TextBox ()
{
}

bool TextBox::on_key (wchar_t key)
{
	Scroll::Info scroll_info = Scroll::get_info ();
	bool processed = false;
	switch (key) {
		case LEFT:
			if (scroll_info.focus) {
				Scroll::modify_focus (scroll_info.focus - 1);
				processed = true;
			}
			break;
		case RIGHT:
			if (scroll_info.focus < text.length ()) {
				Scroll::modify_focus (scroll_info.focus + 1);
				processed = true;
			}
			break;
		case HOME:
			Scroll::modify_focus (0);
			break;
		case END:
			Scroll::modify_focus (text.length ());
			break;
		case DELETE:
			if (scroll_info.focus < text.length ()) {
				text.erase (scroll_info.focus, 1);
				Scroll::modify_number_delete ();
				sig_changed.emit ();
				processed = true;
			}
			break;
		case BACKSPACE1:
		case BACKSPACE2:
			if (scroll_info.focus) {
				text.erase (scroll_info.focus-1, 1);
				Scroll::modify_number_backspace ();
				sig_changed.emit ();
				processed = true;
			}
			break;
		default:
			if (iswprint (key)) {
				text.insert (scroll_info.focus, 1, key);
				Scroll::modify_number_insert ();
				sig_changed.emit ();
				processed = true;
			}
			break;
	}
	TextBox::redraw ();
	return processed;
}

bool TextBox::on_mouse (MouseEvent mouse_event)
{
	if ((mouse_event.m & (LEFT_CLICK | LEFT_PRESS)) == 0) {
		return false;
	}
	Scroll::modify_focus_pos (mouse_event.p.x);
	move_cursor({Scroll::get_info().focus_pos, 0});
	return true;
}

void TextBox::move_resize (Size new_pos, Size new_size)
{
	unsigned old_width = get_size ().x;
	Control::move_resize (new_pos, new_size);
	if (old_width != new_size.x) {
		Scroll::modify_width (new_size.x);
	}
}

void TextBox::redraw ()
{
	choose_palette (is_focus () ? PALETTE_ID_TEXTBOX_FOCUS : PALETTE_ID_TEXTBOX);
	clear ();
	Scroll::Info scroll_info = Scroll::get_info ();
	if (attributes & PASSWORD_BOX) {
		fill(Size{}, Size{scroll_info.len, 1}, L'*');
	} else {
		put(Size{}, text.data() + scroll_info.first, scroll_info.len);
	}
	move_cursor({scroll_info.focus_pos, 0});
}

unsigned TextBox::get_item_screen_size (unsigned j) const
{
	assert (j < text.length ());
	if (attributes & PASSWORD_BOX) {
		return 1;
	}
	else {
		return ucs_width (text[j]);
	}
}

void TextBox::set_text (const std::wstring &s, bool emit_sig_changed)
{
	set_text (s, emit_sig_changed, Scroll::get_focus ());
}

void TextBox::set_text (const std::wstring &s, bool emit_sig_changed, unsigned new_cursor_pos)
{
	if (text == s) {
		return;
	}
	text = s;
	Scroll::modify_number (s.length ());
	new_cursor_pos = minU (new_cursor_pos, s.length ());
	if (new_cursor_pos != Scroll::get_focus ()) {
		Scroll::modify_focus (new_cursor_pos);
	}
	if (emit_sig_changed) {
		sig_changed.emit ();
	}
	TextBox::redraw ();
}


} // namespace tiary::ui
} // namespace tiary
