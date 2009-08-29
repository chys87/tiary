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


#include "ui/dialog_search.h"
#include "ui/fixed_dialog.h"
#include "ui/textbox.h"
#include "ui/button.h"
#include "ui/button_default.h"
#include "ui/chain.h"
#include "ui/checkbox.h"
#include "ui/label.h"
#include "common/container_of.h"


namespace tiary {
namespace ui {

namespace {

class WindowSearch : public FixedWindow, private ButtonDefault
{

	ui::TextBox box_input;
	ui::CheckBox chk_backward;
	ui::Label lbl_backward;
#ifdef TIARY_USE_PCRE
	ui::CheckBox chk_regex;
	ui::Label lbl_regex;
#endif
	ui::Button btn_ok;

	std::wstring &o_text;
	bool &o_bkwd;
	bool &o_regex;

public:

	WindowSearch (std::wstring &o_text, bool &o_bkwd, bool &o_regex, 
		const std::wstring &text, bool bkwd, bool regex);
	~WindowSearch ();

	//void redraw (); // The default one is okay

	void slot_ok (); ///< Pressed Enter or clicked OK
};

WindowSearch::WindowSearch (std::wstring &o_text_, bool &o_bkwd_, bool &o_regex_, 
		const std::wstring &text, bool bkwd, bool regex)
	: Window (0, L"Search")
	, FixedWindow ()
	, box_input (*this, 0)
	, chk_backward (*this, bkwd)
	, lbl_backward (*this, L"&Backward")
#ifdef TIARY_USE_PCRE
	, chk_regex (*this, regex)
	, lbl_regex (*this, L"&Regular expression")
#endif
	, btn_ok (*this, L"&Go!")
	, o_text (o_text_)
	, o_bkwd (o_bkwd_)
	, o_regex (o_regex_)
{
	box_input.set_text (text, false, text.size ());
	chk_backward.set_status (bkwd, false);
#ifdef TIARY_USE_PCRE
	chk_regex.set_status (regex, false);
#endif

	FixedWindow::resize (make_size (40, 7));
	box_input.move_resize (make_size (2, 1), make_size (36, 1));
	chk_backward.move_resize (make_size (2, 3), make_size (3, 1));
	lbl_backward.move_resize (make_size (6, 3), make_size (10, 1));
#ifdef TIARY_USE_PCRE
	chk_regex.move_resize (make_size (2, 4), make_size (3, 1));
	lbl_regex.move_resize (make_size (6, 4), make_size (20, 1));
#endif
	btn_ok.move_resize (make_size (28, 3), make_size (10, 3));

	ChainControlsVertical () (box_input) (chk_backward)
#ifdef TIARY_USE_PCRE
		(chk_regex)
#endif
		;
	ChainControlsHorizontal () (chk_backward) (btn_ok);
#ifdef TIARY_USE_PCRE
	chk_regex.ctrl_left = chk_regex.ctrl_right = &btn_ok;
#endif
	btn_ok.ctrl_up = btn_ok.ctrl_down = &box_input;

	set_default_button (btn_ok);

	lbl_backward.sig_hotkey.connect (
			TIARY_LIST_OF (Signal)
				Signal (chk_backward, &CheckBox::toggle, false),
				Signal (this, &Window::set_focus_ptr, &chk_backward, 0)
			TIARY_LIST_OF_END
			);
	lbl_backward.sig_clicked.connect (lbl_backward.sig_hotkey);
#ifdef TIARY_USE_PCRE
	lbl_regex.sig_hotkey.connect (
			TIARY_LIST_OF (Signal)
				Signal (chk_regex, &CheckBox::toggle, false),
				Signal (this, &Window::set_focus_ptr, &chk_regex, 0)
			TIARY_LIST_OF_END
			);
	lbl_regex.sig_clicked.connect (lbl_regex.sig_hotkey);
#endif
	btn_ok.sig_clicked.connect (this, &WindowSearch::slot_ok);
	register_hotkey (ESCAPE, Signal (this, &Window::request_close));

	WindowSearch::redraw ();
}

WindowSearch::~WindowSearch ()
{
}

void WindowSearch::slot_ok ()
{
	o_text = box_input.get_text ();
	o_bkwd = chk_backward.get_status ();
#ifdef TIARY_USE_PCRE
	o_regex = chk_regex.get_status ();
#else
	o_regex = false;
#endif
	request_close ();
}

} // anonymous namespace



void dialog_search (std::wstring &o_text, bool &o_bkwd, bool &o_regex, 
		const std::wstring &text, bool bkwd, bool regex)
{
	WindowSearch (o_text, o_bkwd, o_regex, text, bkwd, regex).event_loop ();
}

} // namespace tiary::ui
} // namespace tiary
