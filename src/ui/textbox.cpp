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
	: Control(win, kRedrawOnFocusChange)
	, scroll_(0, true, (attr & PASSWORD_BOX) ? nullptr : std::function<unsigned(unsigned)>([this](unsigned idx) { return get_item_screen_size(idx); }))
	, text_()
	, attributes_(attr)
{
}

TextBox::~TextBox ()
{
}

bool TextBox::on_key (wchar_t key)
{
	unsigned scroll_focus = scroll_.get_focus();
	bool processed = false;
	switch (key) {
		case LEFT:
			if (scroll_focus) {
				scroll_.modify_focus(scroll_focus - 1);
				processed = true;
			}
			break;
		case RIGHT:
			if (scroll_focus < text_.length()) {
				scroll_.modify_focus(scroll_focus + 1);
				processed = true;
			}
			break;
		case HOME:
			scroll_.modify_focus(0);
			break;
		case END:
			scroll_.modify_focus(text_.length());
			break;
		case DELETE:
			if (scroll_focus < text_.length()) {
				text_.erase(scroll_focus, 1);
				scroll_.modify_number_delete();
				sig_changed.emit ();
				processed = true;
			}
			break;
		case BACKSPACE1:
		case BACKSPACE2:
			if (scroll_focus) {
				text_.erase(scroll_focus - 1, 1);
				scroll_.modify_number_backspace();
				sig_changed.emit ();
				processed = true;
			}
			break;
		case CTRL_K:
			if (scroll_focus < text_.length()) {
				text_.erase(scroll_focus);
				scroll_.modify_number(scroll_focus);
				sig_changed.emit();
				processed = true;
			}
			break;
		default:
			if (iswprint (key)) {
				text_.insert(scroll_focus, 1, key);
				scroll_.modify_number_insert ();
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
	scroll_.modify_focus_pos(mouse_event.p.x);
	move_cursor({scroll_.get_focus_pos(), 0});
	return true;
}

void TextBox::move_resize (Size new_pos, Size new_size)
{
	unsigned old_width = get_size ().x;
	Control::move_resize (new_pos, new_size);
	if (old_width != new_size.x) {
		scroll_.modify_width(new_size.x);
	}
}

void TextBox::redraw ()
{
	choose_palette (is_focus () ? PALETTE_ID_TEXTBOX_FOCUS : PALETTE_ID_TEXTBOX);
	clear ();
	Scroll::Info scroll_info = scroll_.get_info();
	if (attributes_ & PASSWORD_BOX) {
		fill(Size{}, Size{scroll_info.len, 1}, L'*');
	} else {
		put(Size{}, text_.data() + scroll_info.first, scroll_info.len);
	}
	move_cursor({scroll_info.focus_pos, 0});
}

unsigned TextBox::get_item_screen_size(unsigned j) const {
	// Only for non-password text boxes is this function invoked.
	assert(j < text_.length());
	return ucs_width(text_[j]);
}

void TextBox::set_text(std::wstring_view s, bool emit_sig_changed) {
	set_text (s, emit_sig_changed, scroll_.get_focus());
}

void TextBox::set_text(std::wstring_view s, bool emit_sig_changed, unsigned new_cursor_pos) {
	if (text_ == s) {
		return;
	}
	text_ = s;
	scroll_.modify_number(s.length());
	new_cursor_pos = minU (new_cursor_pos, s.length ());
	if (new_cursor_pos != scroll_.get_focus()) {
		scroll_.modify_focus(new_cursor_pos);
	}
	if (emit_sig_changed) {
		sig_changed.emit ();
	}
	TextBox::redraw ();
}


} // namespace tiary::ui
} // namespace tiary
