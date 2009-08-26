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


#include "ui/dialog_input.h"
#include "ui/label.h"
#include "ui/textbox.h"
#include "ui/button.h"
#include "ui/fixed_dialog.h"
#include "ui/chain.h"
#include "common/algorithm.h"

namespace tiary {
namespace ui {

namespace {

class DialogInput : public FixedDialog
{

	Label lbl_hint;
	TextBox box_input;
	Button btn_ok;

	std::wstring &result;

public:
	DialogInput (const std::wstring &title,
			const std::wstring &hint, const std::wstring &default_text,
			unsigned text_box_width,
			DialogInputAttribute attr,
			std::wstring &result_);
	~DialogInput ();

	//void redraw (); // The default one is okay

	void slot_ok ();     // Pressed Enter or clicked OK Button
};

DialogInput::DialogInput (const std::wstring &title,
		const std::wstring &hint, const std::wstring &default_text,
		unsigned text_box_width,
		DialogInputAttribute attr,
		std::wstring &result_)
	: FixedDialog (0, title)
	, lbl_hint (*this, hint)
	, box_input (*this, (attr & INPUT_PASSWORD) ? TextBox::PASSWORD_BOX : 0)
	, btn_ok (*this, L"&OK")
	, result (result_)
{
	unsigned hint_height = lbl_hint.split_line (text_box_width).size ();
	FixedDialog::resize (make_size (text_box_width + 4, hint_height + 8));
	lbl_hint.move_resize (make_size (2, 1), make_size (text_box_width, hint_height));
	box_input.move_resize (make_size (2, hint_height+2), make_size (text_box_width, 1));
	btn_ok.move_resize (make_size (maxS (0, (text_box_width+4-10)/2), hint_height+4), make_size (10, 3));

	ChainControlsVertical () (box_input) (btn_ok);

	box_input.set_text (default_text, false, default_text.length ());
	register_hotkey (ESCAPE, Signal (this, &Window::request_close));
	btn_ok.set_attribute (Button::DEFAULT_BUTTON);
	btn_ok.sig_clicked.connect (this, &DialogInput::slot_ok);
	DialogInput::redraw ();
}

DialogInput::~DialogInput ()
{
}

void DialogInput::slot_ok ()
{
	result = box_input.get_text ();
	request_close ();
}

} // anonymous namespace

std::wstring dialog_input (const std::wstring &hint, const std::wstring &default_text,
		unsigned text_box_width, DialogInputAttribute attr,
		const std::wstring &return_on_error)
{
	return dialog_input2 (std::wstring (), hint, default_text, text_box_width,
			attr, return_on_error);
}

std::wstring dialog_input2 (const std::wstring &title, const std::wstring &hint,
		const std::wstring &default_text, unsigned text_box_width,
		DialogInputAttribute attr, const std::wstring &return_on_error)
{
	std::wstring ret = return_on_error;
	DialogInput (title, hint, default_text, text_box_width, attr, ret).event_loop ();
	return ret;
}

} // namespace tiary::ui
} // namespace tiary

