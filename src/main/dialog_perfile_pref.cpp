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


/**
 * @file	main/dialog_perfile_pref.cpp
 * @author	chys <admin@chys.info>
 * @brief	Implements a dialog to edit file preferences
 */

#include "main/dialog_perfile_pref.h"
#include "ui/fixed_window.h"
#include "ui/button_default.h"
#include "ui/layout.h"
#include "ui/chain.h"
#include "ui/button.h"
#include "ui/checkbox_label.h"
#include "ui/dialog_message.h"
#include "common/algorithm.h"
#include "common/dir.h"
#include "common/string.h"
#include "common/unicode.h"



namespace tiary {

namespace {

using namespace ui;

class WindowPerFileOptions : public FixedWindow, private ButtonDefault
{
	PerFileOptionGroup &options;

	CheckBoxLabel chk_modtime;

	Button btn_ok;
	Button btn_cancel;
	Button btn_reset;
	Button btn_help;

	Layout layout_buttons;
	Layout layout_main;

	void slot_ok ();
	void slot_reset ();
	void slot_help ();

public:
	WindowPerFileOptions (PerFileOptionGroup &options_);
	~WindowPerFileOptions ();
};

WindowPerFileOptions::WindowPerFileOptions (PerFileOptionGroup &options_)
	: Window(0, L"File Preferences"sv)
	, FixedWindow ()
	, ButtonDefault ()
	, options (options_)
	, chk_modtime(*this, L"Use &modification time"sv, options_.get_bool (PERFILE_OPTION_MODTIME))
	, btn_ok(*this, L"&OK"sv)
	, btn_cancel(*this, L"&Cancel"sv)
	, btn_reset(*this, L"&Reset"sv)
	, btn_help(*this, L"&Help"sv)
	, layout_buttons (HORIZONTAL)
	, layout_main (VERTICAL)
{
	FixedWindow::resize (get_screen_size () & Size{80, 10});

	// Set up layouts
	layout_buttons.add({
			{btn_ok, 10, 10},
			{2, 2},
			{btn_cancel, 10, 10},
			{2, 2},
			{btn_reset, 10, 10},
			{2, 2},
			{btn_help, 10, 10},
		});

	layout_main.add({
			{chk_modtime, 1, 1},
			{1, 1},
			{layout_buttons, 3, 3},
		});

	layout_main.move_resize({2, 1}, get_size () - Size{4, 2});

	// Set up chains
	ChainControlsVerticalNC{&chk_modtime.checkbox, &btn_ok};
	ChainControlsHorizontalO{&btn_ok, &btn_cancel, &btn_reset, &btn_help};

	// Set up signals
	btn_ok.sig_clicked.connect (this, &WindowPerFileOptions::slot_ok);
	btn_cancel.sig_clicked.connect (this, &Window::request_close);
	btn_reset.sig_clicked.connect (this, &WindowPerFileOptions::slot_reset);
	btn_help.sig_clicked.connect (this, &WindowPerFileOptions::slot_help);

	// Other initializations
	set_default_button (btn_ok);
	register_hotkey (ESCAPE, btn_cancel.sig_clicked);
	register_hotkey (F1, btn_help.sig_clicked);
}

WindowPerFileOptions::~WindowPerFileOptions ()
{
}

void WindowPerFileOptions::slot_ok ()
{
	options.set (PERFILE_OPTION_MODTIME, chk_modtime.get_status ());
	Window::request_close ();
}

void WindowPerFileOptions::slot_reset ()
{
	if (ui::dialog_message(L"Do you really want to reset all settings to default?"sv,
				MESSAGE_YES|MESSAGE_NO|MESSAGE_DEFAULT_NO) == MESSAGE_YES) {
		// Use a new GlobalOptionGroup object
		// Do not clobber options in this function
		PerFileOptionGroup default_options;
		chk_modtime.set_status (default_options.get_bool (PERFILE_OPTION_MODTIME));
	}
}

constexpr std::wstring_view help_info = L"\
Use modification time:\n\
    If enabled, tiary assumes the date/time of each entry to be the\n\
    recent modification time, and is reset each time the entry is modified.\n\
    By default (disabled), the date/time is assumed to be the creation time.\n\
"sv;

void WindowPerFileOptions::slot_help ()
{
	dialog_message (help_info);
}

} // anonymous namespace


bool edit_perfile_options (PerFileOptionGroup &options)
{
	PerFileOptionGroup old_options (options);
	WindowPerFileOptions (options).event_loop ();
	return (options != old_options);
}


} //namespace tiary
