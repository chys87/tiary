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


/**
 * @file	ui/dialog_search.cpp
 * @author	chys <admin@chys.info>
 * @brief	Implements tiary::ui::search, displaying a typical "Search" window
 */

#include "ui/dialog_search.h"
#include "ui/fixed_window.h"
#include "ui/textbox.h"
#include "ui/button.h"
#include "ui/button_default.h"
#include "ui/chain.h"
#include "ui/checkbox_label.h"
#include "ui/label.h"


namespace tiary {
namespace ui {

namespace {

class WindowSearch : public FixedWindow, private ButtonDefault
{
public:
	ui::TextBox box_input;
	ui::CheckBoxLabel chk_backward;
#ifdef TIARY_USE_RE2
	ui::CheckBoxLabel chk_regex;
#endif
	ui::Button btn_ok;

	SearchDesc *output_;

public:

	WindowSearch(SearchDesc *output, const SearchDesc &default_search);
	~WindowSearch ();

	//void redraw (); // The default one is okay

private:
	void slot_ok (); ///< Pressed Enter or clicked OK
	bool is_text_nonempty () const;
};

WindowSearch::WindowSearch(SearchDesc *output, const SearchDesc &default_search)
	: Window (0, L"Search")
	, FixedWindow ()
	, box_input (*this, 0)
	, chk_backward(*this, L"&Backward", default_search.backward)
#ifdef TIARY_USE_RE2
	, chk_regex(*this, L"&Regular expression", default_search.regex)
#endif
	, btn_ok (*this, L"&Go!")
	, output_(output)
{
	box_input.set_text(default_search.text, false, default_search.text.size());
	chk_backward.checkbox.set_status(default_search.backward, false);
#ifdef TIARY_USE_RE2
	chk_regex.checkbox.set_status(default_search.regex, false);
#endif

	FixedWindow::resize({40, 7});
	box_input.move_resize({2, 1}, {36, 1});
	chk_backward.move_resize({2, 3}, {15, 1});
#ifdef TIARY_USE_RE2
	chk_regex.move_resize({2, 4}, {25, 1});
#endif
	btn_ok.move_resize({28, 3}, {10, 3});

	ChainControlsVertical{
		&box_input,
		&chk_backward.checkbox,
#ifdef TIARY_USE_RE2
		&chk_regex.checkbox,
#endif
	};
	ChainControlsHorizontal{&chk_backward.checkbox, &btn_ok};
#ifdef TIARY_USE_RE2
	chk_regex.checkbox.ctrl_left = chk_regex.checkbox.ctrl_right = &btn_ok;
#endif
	btn_ok.ctrl_up = btn_ok.ctrl_down = &box_input;

	set_default_button (btn_ok);

	box_input.sig_changed.connect (btn_ok, &Button::redraw);
	btn_ok.sig_clicked = Condition (this, &WindowSearch::is_text_nonempty);
	btn_ok.sig_clicked.connect (this, &WindowSearch::slot_ok);
	register_hotkey (ESCAPE, Signal (this, &Window::request_close));

	WindowSearch::redraw ();
}

WindowSearch::~WindowSearch ()
{
}

void WindowSearch::slot_ok ()
{
	output_->text = box_input.get_text();
	output_->backward = chk_backward.get_status();
#ifdef TIARY_USE_RE2
	output_->regex = chk_regex.get_status();
#else
	output_->regex = false;
#endif
	request_close ();
}

bool WindowSearch::is_text_nonempty () const
{
	return !box_input.get_text ().empty ();
}

} // anonymous namespace



void dialog_search(SearchDesc *output, const SearchDesc &default_search) {
	WindowSearch(output, default_search).event_loop ();
}

} // namespace tiary::ui
} // namespace tiary
