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


#include "ui/textbox.h"
#include "ui/ui.h"
#include "ui/chain.h"
#include "ui/paletteid.h"
#include "common/unicode.h"
#include <wctype.h>
#include <assert.h>

namespace tiary {
namespace ui {

TextBox::TextBox (Window &dlg, unsigned attr)
	: Control (dlg)
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
	switch (key) {
		case LEFT:
			if (scroll_info.focus)
				Scroll::modify_focus (scroll_info.focus - 1);
			break;
		case RIGHT:
			if (scroll_info.focus < text.length ())
				Scroll::modify_focus (scroll_info.focus + 1);
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
			}
			break;
		case BACKSPACE1:
		case BACKSPACE2:
			if (scroll_info.focus) {
				text.erase (scroll_info.focus-1, 1);
				Scroll::modify_number_backspace ();
				sig_changed.emit ();
			}
			break;
		default:
			if (!iswprint (key))
				return false;
			text.insert (scroll_info.focus, 1, key);
			Scroll::modify_number_insert ();
			sig_changed.emit ();
			break;
	}
	TextBox::redraw ();
	return true;
}

bool TextBox::on_mouse (MouseEvent mouse_event)
{
	if ((mouse_event.m & (LEFT_CLICK | LEFT_PRESS)) == 0)
		return false;
	Scroll::modify_focus_pos (mouse_event.p.x);
	move_cursor (make_size (Scroll::get_info ().focus_pos, 0));
	return true;
}

void TextBox::on_move_resize (Size, Size oldsize)
{
	unsigned newwid = get_size ().x;
	if (newwid != oldsize.x)
		Scroll::modify_width (newwid);
}

void TextBox::redraw ()
{
	choose_palette (PALETTE_ID_TEXTBOX);
	clear ();
	Scroll::Info scroll_info = Scroll::get_info ();
	if (attributes & PASSWORD_BOX)
		fill (make_size (), make_size(scroll_info.len,1), L'*');
	else
		put (make_size (), text.data() + scroll_info.first, scroll_info.len);
	move_cursor (make_size (scroll_info.focus_pos, 0));
}

unsigned TextBox::get_item_screen_size (unsigned j) const
{
	assert (j < text.length ());
	if (attributes & PASSWORD_BOX)
		return 1;
	else
		return ucs_width (text[j]);
}

void TextBox::set_text (const std::wstring &s, bool emit_sig_changed, unsigned new_focus)
{
	if (text == s)
		return;
	text = s;
	Scroll::modify_number (s.length ());
	if (new_focus <= s.length ())
		Scroll::modify_focus (new_focus);
	if (emit_sig_changed)
		sig_changed.emit ();
	TextBox::redraw ();
}

#ifdef TIARY_HAVE_RVALUE_REFERENCES
void TextBox::set_text (std::wstring &&s, bool emit_sig_changed, unsigned new_focus)
{
	if (text == s)
		return;
	text.swap (s); // operator = (std::string &&) not available (GCC)
	size_t len = text.length ();
	Scroll::modify_number (len);
	if (new_focus <= len)
		Scroll::modify_focus (new_focus);
	if (emit_sig_changed)
		sig_changed.emit ();
	TextBox::redraw ();
}
#endif


} // namespace tiary::ui
} // namespace tiary
