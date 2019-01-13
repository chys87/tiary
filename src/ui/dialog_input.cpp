// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2010, 2018, 2019, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/

/**
 * @file	ui/dialog_input.cpp
 * @author	chys <admin@chys.info>
 * @brief	Implements tiary::ui::dialog_input and tiary::ui::dialog_input2
 */

#include "ui/dialog_input.h"
#include "ui/label.h"
#include "ui/textbox.h"
#include "ui/button.h"
#include "ui/button_default.h"
#include "ui/fixed_window.h"
#include "ui/chain.h"
#include "common/algorithm.h"
#include "common/string.h"

namespace tiary {
namespace ui {

namespace {

class WindowInput : public FixedWindow, private ButtonDefault
{

	Label lbl_hint;
	TextBox box_input;
	Button btn_ok;

	std::wstring &result;

public:
	WindowInput(std::wstring_view title,
			std::wstring_view hint, std::wstring_view default_text,
			unsigned text_box_width,
			WindowInputAttribute attr,
			std::wstring &result_);
	~WindowInput ();

	//void redraw (); // The default one is okay

	void slot_ok ();     // Pressed Enter or clicked OK Button
};

WindowInput::WindowInput(std::wstring_view title,
		std::wstring_view hint, std::wstring_view default_text,
		unsigned text_box_width,
		WindowInputAttribute attr,
		std::wstring &result_)
	: Window (0, title)
	, FixedWindow ()
	, ButtonDefault ()
	, lbl_hint(*this, hint)
	, box_input (*this, (attr & INPUT_PASSWORD) ? TextBox::PASSWORD_BOX : 0)
	, btn_ok(*this, L"&OK"sv)
	, result (result_)
{
	unsigned hint_height = lbl_hint.split_line (text_box_width).size ();
	FixedWindow::resize(Size(text_box_width + 4, hint_height + 8));
	lbl_hint.move_resize({2, 1}, Size(text_box_width, hint_height));
	box_input.move_resize(Size(2, hint_height + 2), Size(text_box_width, 1));
	btn_ok.move_resize(Size(maxS (0, (text_box_width + 4 - 10) / 2), hint_height + 4), {10, 3});

	ChainControlsVertical{&box_input, &btn_ok};

	box_input.set_text (default_text, false, default_text.length ());
	register_hotkey (ESCAPE, Signal (this, &Window::request_close));
	set_default_button (btn_ok);
	btn_ok.sig_clicked.connect (this, &WindowInput::slot_ok);
	WindowInput::redraw ();
}

WindowInput::~WindowInput ()
{
}

void WindowInput::slot_ok ()
{
	result = box_input.get_text ();
	request_close ();
}

} // anonymous namespace

std::wstring dialog_input(std::wstring_view hint, std::wstring_view default_text,
		unsigned text_box_width, WindowInputAttribute attributes,
		std::wstring_view return_on_error) {
	return dialog_input2({}, hint, default_text, text_box_width,
			attributes, return_on_error);
}

std::wstring dialog_input2(std::wstring_view title, std::wstring_view hint,
		std::wstring_view default_text, unsigned text_box_width,
		WindowInputAttribute attributes, std::wstring_view return_on_error)
{
	std::wstring ret(return_on_error);
	WindowInput (title, hint, default_text, text_box_width, attributes, ret).event_loop ();
	return ret;
}

} // namespace tiary::ui
} // namespace tiary

