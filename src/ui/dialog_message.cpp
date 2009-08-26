// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the so-called 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#include "ui/dialog_message.h"
#include "ui/dialog.h"
#include "ui/label.h"
#include "ui/button.h"
#include "common/algorithm.h"
#include "ui/chain.h"

namespace tiary {
namespace ui {

namespace {

class DialogMessage : public Dialog
{

	Label lbl_text;

	DialogMessageButton button_mask;
	DialogMessageButton result;
	unsigned n_buttons;
	Button *p_buttons[4];

public:
	DialogMessage (const std::wstring &text, const std::wstring &title,
			DialogMessageButton buttons);
	~DialogMessage ();

	DialogMessageButton get_result () const { return result; }

	void slot_click (DialogMessageButton);

	void on_winch ();
	void redraw ();
};

DialogMessage::DialogMessage (const std::wstring &text, const std::wstring &title,
		DialogMessageButton mask)
	: Dialog (0, title)
	, lbl_text (*this, text)
	, button_mask (mask)
	, result (0)
	, n_buttons (0)
{
	if (!(mask & MESSAGE_ALL))
		button_mask = mask = MESSAGE_OK;
	Button *btn_ok = 0;
	Button *btn_yes = 0;
	Button *btn_no = 0;
	Button *btn_cancel = 0;
	Button **p = p_buttons;
	if (mask & MESSAGE_OK) {
		btn_ok = new Button (*this, L"&OK");
		btn_ok->sig_clicked.connect (this, &DialogMessage::slot_click, MESSAGE_OK);
		*p++ = btn_ok;
	}
	if (mask & MESSAGE_YES) {
		btn_yes = new Button (*this, L"&Yes");
		btn_yes->sig_clicked.connect (this, &DialogMessage::slot_click, MESSAGE_YES);
		*p++ = btn_yes;
	}
	if (mask & MESSAGE_NO) {
		btn_no = new Button (*this, L"&No");
		btn_no->sig_clicked.connect (this, &DialogMessage::slot_click, MESSAGE_NO);
		*p++ = btn_no;
	}
	if (mask & MESSAGE_CANCEL) {
		btn_cancel = new Button (*this, L"&Cancel");
		btn_cancel->sig_clicked.connect (this, &DialogMessage::slot_click, MESSAGE_CANCEL);
		*p++ = btn_cancel;
	}
	n_buttons = p - p_buttons;

	if (n_buttons >= 2)
		ChainControlsHorizontal (p_buttons, n_buttons);

	// Map Enter and Esc
	Button *btn_enter;
	Button *btn_escape;
	if (!(btn_enter = btn_ok))
		if (!(btn_enter = btn_yes))
			if (!(btn_enter = btn_no))
				btn_enter = btn_cancel;
	if (!(btn_escape = btn_cancel))
		if (!(btn_escape = btn_no))
			if (!(btn_escape = btn_yes))
				btn_escape = btn_ok;
	if (btn_enter == btn_escape)
		btn_enter->set_attribute (Button::DEFAULT_BUTTON | Button::ESCAPE_BUTTON);
	else {
		btn_enter->set_attribute (Button::DEFAULT_BUTTON);
		btn_escape->set_attribute (Button::ESCAPE_BUTTON);
	}
	DialogMessage::redraw ();
}

DialogMessage::~DialogMessage ()
{
	for (unsigned i=0; i<n_buttons; ++i)
		delete p_buttons[i];
}

void DialogMessage::slot_click (DialogMessageButton button)
{
	result = button;
	request (REQUEST_CLOSE);
}

void DialogMessage::on_winch ()
{
	DialogMessage::redraw ();
}

void DialogMessage::redraw ()
{
	Size scr_size = get_screen_size ();
	// Estimate the width: Every button is 10-char wide, plus 2 spaces
	// Two spaces are left on each side of the window
	unsigned width;

	const unsigned ideal_width = 54;
	if (scr_size.x>ideal_width && lbl_text.get_max_text_width()>ideal_width &&
		// First try not too wide.
		// So that the box will seem better (if we only have a long single line)
			lbl_text.split_line(ideal_width-4).size() + 6 <= scr_size.y / 2)
		width = ideal_width;
	else
		width = lbl_text.get_max_text_width () + 4;
	width = minU (maxU (n_buttons * 12, width), scr_size.x);
	unsigned height = minU (lbl_text.split_line (width - 4).size () + 6, scr_size.y);

	Size win_size = make_size (width, height);
	move_resize ((scr_size - win_size) / 2, win_size);
	lbl_text.move_resize (make_size (2, 1), win_size - make_size (4, 6));

	const Size button_size = make_size (10, 3);
	Size button_pos = make_size (width / 2 - n_buttons * 6 + 1, height - 4);

	for (unsigned i=0; i<n_buttons; ++i) {
		p_buttons[i]->move_resize (button_pos, button_size);
		button_pos.x += 12;
	}

	Dialog::redraw ();
}

} // anonymous namespace

DialogMessageButton dialog_message (const std::wstring &text, DialogMessageButton buttons)
{
	return dialog_message (text, std::wstring (), buttons);
}

DialogMessageButton dialog_message (const wchar_t *text, DialogMessageButton buttons)
{
	return dialog_message (std::wstring (text), buttons);
}

DialogMessageButton dialog_message (const std::wstring &text, const std::wstring &title,
		DialogMessageButton buttons)
{
	DialogMessage win (text, title, buttons);
	win.event_loop ();
	return win.get_result ();
}

DialogMessageButton dialog_message (const wchar_t *text, const wchar_t *title,
		DialogMessageButton buttons)
{
	return dialog_message (std::wstring (text), std::wstring (title), buttons);
}

} // namespace tiary::ui
} // namespace tiary
