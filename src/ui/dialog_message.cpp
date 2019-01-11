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


#include "ui/dialog_message.h"
#include "ui/window.h"
#include "ui/label.h"
#include "ui/button.h"
#include "common/algorithm.h"
#include "ui/chain.h"

namespace tiary {
namespace ui {

namespace {

class WindowMessage : public Window
{

	Label lbl_text;

	WindowMessageButton button_mask;
	WindowMessageButton result;
	unsigned n_buttons;
	Button *p_buttons[4];

public:
	WindowMessage(std::wstring_view text, std::wstring_view title,
			WindowMessageButton buttons);
	~WindowMessage ();

	WindowMessageButton get_result () const { return result; }

	void slot_click (WindowMessageButton);

	void redraw ();
};

WindowMessage::WindowMessage(std::wstring_view text, std::wstring_view title,
		WindowMessageButton mask)
	: Window (0, title)
	, lbl_text (*this, text)
	, button_mask (mask)
	, result (0)
	, n_buttons (0)
{
	if (!(mask & MESSAGE_ALL)) {
		button_mask = mask = MESSAGE_OK;
	}
	Button *btn_ok = 0;
	Button *btn_yes = 0;
	Button *btn_no = 0;
	Button *btn_cancel = 0;
	Button **p = p_buttons;
	if (mask & MESSAGE_OK) {
		btn_ok = new Button (*this, L"&OK");
		btn_ok->sig_clicked.connect (this, &WindowMessage::slot_click, MESSAGE_OK);
		*p++ = btn_ok;
	}
	if (mask & MESSAGE_YES) {
		btn_yes = new Button (*this, L"&Yes");
		btn_yes->sig_clicked.connect (this, &WindowMessage::slot_click, MESSAGE_YES);
		*p++ = btn_yes;
	}
	if (mask & MESSAGE_NO) {
		btn_no = new Button (*this, L"&No");
		btn_no->sig_clicked.connect (this, &WindowMessage::slot_click, MESSAGE_NO);
		*p++ = btn_no;
	}
	if (mask & MESSAGE_CANCEL) {
		btn_cancel = new Button (*this, L"&Cancel");
		btn_cancel->sig_clicked.connect (this, &WindowMessage::slot_click, MESSAGE_CANCEL);
		*p++ = btn_cancel;
	}
	n_buttons = p - p_buttons;

	if (n_buttons >= 2) {
		ChainControlsHorizontal (p_buttons, n_buttons);
	}

	// Map Esc
	Button *btn_escape;

	if (!(btn_escape = btn_cancel)) {
		if (!(btn_escape = btn_no)) {
			if (!(btn_escape = btn_yes)) {
				btn_escape = btn_ok;
			}
		}
	}
	register_hotkey (ESCAPE, btn_escape->sig_clicked);

	// Focus the default button
	if ((mask & (MESSAGE_OK|MESSAGE_DEFAULT_OK)) == (MESSAGE_OK|MESSAGE_DEFAULT_OK)) {
		set_focus_ptr (btn_ok);
	}
	else if ((mask & (MESSAGE_YES|MESSAGE_DEFAULT_YES)) == (MESSAGE_YES|MESSAGE_DEFAULT_YES)) {
		set_focus_ptr (btn_yes);
	}
	else if ((mask & (MESSAGE_NO|MESSAGE_DEFAULT_NO)) == (MESSAGE_NO|MESSAGE_DEFAULT_NO)) {
		set_focus_ptr (btn_no);
	}
	else if ((mask & (MESSAGE_CANCEL|MESSAGE_DEFAULT_CANCEL)) == (MESSAGE_CANCEL|MESSAGE_DEFAULT_CANCEL)) {
		set_focus_ptr (btn_cancel);
	}

	WindowMessage::redraw ();
}

WindowMessage::~WindowMessage ()
{
	for (unsigned i=0; i<n_buttons; ++i) {
		delete p_buttons[i];
	}
}

void WindowMessage::slot_click (WindowMessageButton button)
{
	result = button;
	request_close ();
}

void WindowMessage::redraw ()
{
	Size scr_size = get_screen_size ();
	// Estimate the width: Every button is 10-char wide, plus 2 spaces
	// Two spaces are left on each side of the window
	unsigned width;

	const unsigned ideal_width = maxU (54, scr_size.x*2/3);
	if (scr_size.x>ideal_width && lbl_text.get_max_text_width()>ideal_width &&
		// First try not too wide.
		// So that the box will seem better (if we only have a long single line)
			lbl_text.split_line(ideal_width-4).size() + 6 <= scr_size.y / 2) {
		width = ideal_width;
	}
	else {
		width = lbl_text.get_max_text_width () + 4;
	}
	width = minU (maxU (n_buttons * 12, width), scr_size.x);
	unsigned height = minU (lbl_text.split_line (width - 4).size () + 6, scr_size.y);

	Size win_size{width, height};
	move_resize ((scr_size - win_size) / 2, win_size);
	lbl_text.move_resize({2, 1}, win_size - Size{4, 6});

	const Size button_size{10, 3};
	Size button_pos{width / 2 - n_buttons * 6 + 1, height - 4};

	for (unsigned i=0; i<n_buttons; ++i) {
		p_buttons[i]->move_resize (button_pos, button_size);
		button_pos.x += 12;
	}

	Window::redraw ();
}

} // anonymous namespace

WindowMessageButton dialog_message(std::wstring_view text, WindowMessageButton buttons) {
	return dialog_message(text, {}, buttons);
}

WindowMessageButton dialog_message(std::wstring_view text, std::wstring_view title,
		WindowMessageButton buttons)
{
	WindowMessage win (text, title, buttons);
	win.event_loop ();
	return win.get_result ();
}

} // namespace tiary::ui
} // namespace tiary
