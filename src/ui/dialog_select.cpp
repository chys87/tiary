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
 * @file	ui/dialog_select.cpp
 * @author	chys <admin@chys.info>
 * @brief	Implements tiary::ui::dialog_select, displaying a list from which one can be selected
 */


#include "ui/dialog_select.h"
#include "ui/fixed_window.h"
#include "ui/listbox.h"
#include "ui/button.h"
#include "ui/button_default.h"
#include "ui/layout.h"
#include "ui/chain.h"
#include "common/algorithm.h"
#include "common/string.h"
#include "common/unicode.h"

namespace tiary {
namespace ui {

namespace {

class DialogSelect final : public FixedWindow, private ButtonDefault {
	ListBox lst_items;
	Button btn_ok;
	Button btn_cancel;

	Layout layout_main;
	Layout layout_buttons;

	unsigned max_text_width; ///< Maximum screen width of the text
	size_t result;

public:
	DialogSelect(std::wstring_view, const std::vector<std::wstring> &, size_t);
	~DialogSelect ();

	void redraw ();
	void slot_ok ();

	size_t get_result () const { return result; }
};

DialogSelect::DialogSelect(std::wstring_view title, const std::vector <std::wstring> &items,
		size_t pre_select)
	: Window (0, title)
	, FixedWindow ()
	, ButtonDefault ()
	, lst_items (*this)
	, btn_ok(*this, L"&OK"sv)
	, btn_cancel(*this, L"&Cancel"sv)
	, layout_main (VERTICAL)
	, layout_buttons (HORIZONTAL)
	, max_text_width (0)
	, result (size_t (-1))
{
	lst_items.set_items (items, pre_select, false);

	for (std::vector <std::wstring>::const_iterator it = items.begin (); it != items.end (); ++it) {
		max_text_width = maxU (max_text_width, ucs_width (*it));
	}

	layout_buttons.add({
			{btn_ok, 10, 10},
			{1, 1},
			{btn_cancel, 10, 10},
		});
	layout_main.add({
			{lst_items, 1, Layout::UNLIMITED},
			{1, 1},
			{layout_buttons, 3, 3},
		});

	ChainControlsHorizontal{&btn_ok, &btn_cancel};
	ChainControlsVertical{&lst_items, &btn_ok};
	btn_cancel.ctrl_up = btn_cancel.ctrl_down = &lst_items;

	lst_items.sig_double_clicked.connect (this, &DialogSelect::slot_ok);
	btn_ok.sig_clicked.connect (this, &DialogSelect::slot_ok);
	btn_cancel.sig_clicked.connect (this, &Window::request_close);

	set_default_button (btn_ok);
	register_hotkey (ESCAPE, btn_cancel.sig_clicked);
}

DialogSelect::~DialogSelect ()
{
}

void DialogSelect::redraw ()
{
	unsigned items = lst_items.get_items ().size ();

	Size winsize = (Size{max_text_width + 10, items + 6} | Size{30, 12})
		& get_screen_size ();

	resize(winsize);

	layout_main.move_resize({2, 1}, winsize - Size{4, 2});

	Window::redraw ();
}

void DialogSelect::slot_ok ()
{
	result = lst_items.get_select ();
	request_close();
}

} // anonymous namespace


size_t dialog_select(std::wstring_view title,
		const std::vector <std::wstring> &selections,
		size_t pre_select)
{
	if (selections.empty ()) {
		return size_t (-1);
	}
	DialogSelect win (title, selections, pre_select);
	win.event_loop ();
	return win.get_result ();
}

} // namespace tiary::ui
} // namespace tiary
