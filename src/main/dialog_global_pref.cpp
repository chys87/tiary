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


/**
 * @file	main/dialog_global_pref.cpp
 * @author	chys <admin@chys.info>
 * @brief	Implements a dialog to edit preferences
 */

#include "main/dialog_global_pref.h"
#include "ui/fixed_window.h"
#include "ui/layout.h"
#include "ui/chain.h"
#include "ui/label.h"
#include "ui/droplist.h"
#include "ui/button.h"
#include "ui/button_default.h"
#include "ui/textbox.h"
#include "ui/dialog_message.h"
#include "ui/dialog_select_file.h"
#include "common/algorithm.h"
#include "common/unicode.h"
#include "common/dir.h"
#include "common/container_of.h"



namespace tiary {

namespace {

using namespace ui;

class WindowGlobalOptions : public FixedWindow, private ButtonDefault
{

	GlobalOptionGroup &options;
	const std::wstring &current_filename;

	// GLOBAL_OPTION_DEFAULT_FILE
	Label lbl_default_file;
	Label lbl_default_file_name;
	Layout layout_default_file;
	Button btn_default_file;
	Button btn_default_file_current;
	Layout layout_default_file_buttons;

	// GLOBAL_OPTION_EXPAND_LINES
	Label lbl_expand_lines;
	DropList drp_expand_lines;
	Layout layout_expand_lines;

	// GLOBAL_OPTION_EDITOR
	Label lbl_editor;
	TextBox txt_editor;
	Layout layout_editor;

	// GLOBAL_OPTION_DATETIME_FORMAT
	Label lbl_datetime_format;
	TextBox txt_datetime_format;
	Layout layout_datetime_format;

	// GLOBAL_OPTION_LONGTIME_FORMAT
	Label lbl_longtime_format;
	TextBox txt_longtime_format;
	Layout layout_longtime_format;

	// Buttons
	Button btn_ok;
	Button btn_cancel;
	Button btn_reset;
	Button btn_help;
	Layout layout_buttons;

	Layout layout_main;

	void copy_options_to_controls (const GlobalOptionGroup &);

	void slot_default_file ();
	void slot_default_file_current ();
	void slot_ok ();
	void slot_reset ();
	void slot_help ();

public:
	WindowGlobalOptions (GlobalOptionGroup &options_, const std::wstring &current_filename_);
	~WindowGlobalOptions ();
};

const wchar_t expand_lines_array[][2] = { L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8" };
WindowGlobalOptions::WindowGlobalOptions (GlobalOptionGroup &options_, const std::wstring &current_filename_)
	: Window (0, L"Preferences")
	, FixedWindow ()
	, ButtonDefault ()
	, options (options_)
	, current_filename (current_filename_)
	, lbl_default_file (*this, L"Default &file:")
	, lbl_default_file_name (*this, std::wstring ())
	, layout_default_file (HORIZONTAL)
	, btn_default_file (*this, L"Choose...")
	, btn_default_file_current (*this, L"Use current file")
	, layout_default_file_buttons (HORIZONTAL)
	, lbl_expand_lines (*this, L"Expand &lines:")
	, drp_expand_lines (*this, std::vector<std::wstring>(expand_lines_array, array_end (expand_lines_array)),
			options_.get_num(GLOBAL_OPTION_EXPAND_LINES)-1)
	, layout_expand_lines (HORIZONTAL)
	, lbl_editor (*this, L"&Editor:")
	, txt_editor (*this)
	, layout_editor (HORIZONTAL)
	, lbl_datetime_format (*this, L"&Time format:")
	, txt_datetime_format (*this)
	, layout_datetime_format (HORIZONTAL)
	, lbl_longtime_format (*this, L"Lo&ng time format:")
	, txt_longtime_format (*this)
	, layout_longtime_format (HORIZONTAL)
	, btn_ok (*this, L"&OK")
	, btn_cancel (*this, L"&Cancel")
	, btn_reset (*this, L"&Reset")
	, btn_help (*this, L"&Help")
	, layout_buttons (HORIZONTAL)
	, layout_main (VERTICAL)
{
	FixedWindow::resize (get_screen_size () & make_size (80, 18));

	// Set up layouts

	layout_default_file.add
		(lbl_default_file, 20, 20)
		(1, 1)
		(lbl_default_file_name, 2, Layout::UNLIMITED)
		;

	layout_default_file_buttons.add
		(21, 21)
		(btn_default_file, 10, 10)
		(1, 1)
		(btn_default_file_current, 20, 20)
		(0, Layout::UNLIMITED)
		;

	layout_expand_lines.add
		(lbl_expand_lines, 20, 20)
		(1, 1)
		(drp_expand_lines, 1, 1)
		(0, Layout::UNLIMITED)
		;

	layout_editor.add
		(lbl_editor, 20, 20)
		(1, 1)
		(txt_editor, 2, Layout::UNLIMITED)
		;

	layout_datetime_format.add
		(lbl_datetime_format, 20, 20)
		(1, 1)
		(txt_datetime_format, 2, Layout::UNLIMITED)
		;

	layout_longtime_format.add
		(lbl_longtime_format, 20, 20)
		(1, 1)
		(txt_longtime_format, 2, Layout::UNLIMITED)
		;

	layout_buttons.add
		(btn_ok, 10, 10)
		(2, 2)
		(btn_cancel, 10, 10)
		(2, 2)
		(btn_reset, 10, 10)
		(2, 2)
		(btn_help, 10, 10)
		;

	layout_main.add
		(layout_default_file, 1, 1)
		(layout_default_file_buttons, 1, 1)
		(1, 1)
		(layout_expand_lines, 1, 1)
		(1, 1)
		(layout_editor, 1, 1)
		(1, 1)
		(layout_datetime_format, 1, 1)
		(1, 1)
		(layout_longtime_format, 1, 1)
		(1, 1)
		(layout_buttons, 3, 3);

	layout_main.move_resize (make_size (2, 1), get_size () - make_size (4, 2));


	// Chain

	ChainControlsHorizontal () (btn_default_file) (btn_default_file_current);
	ChainControlsHorizontal () (btn_ok) (btn_cancel) (btn_reset) (btn_help);
	ChainControlsVerticalNC ()
		(btn_default_file)
		(drp_expand_lines)
		(txt_editor)
		(txt_datetime_format)
		(txt_longtime_format)
		(btn_ok);
	btn_default_file_current.ctrl_down = btn_default_file.ctrl_down;
	btn_reset.ctrl_up = btn_cancel.ctrl_up = btn_help.ctrl_up = btn_ok.ctrl_up;


	// Set up signals

	lbl_default_file.sig_hotkey.connect (
			TIARY_LIST_OF(Signal)
				Signal (this, &Window::set_focus_ptr, &btn_default_file, 0),
				Signal (btn_default_file, &Button::slot_clicked)
			TIARY_LIST_OF_END
			);
	btn_default_file.sig_clicked.connect (this, &WindowGlobalOptions::slot_default_file);
	btn_default_file_current.sig_clicked.connect (this, &WindowGlobalOptions::slot_default_file_current);
	btn_ok.sig_clicked.connect (this, &WindowGlobalOptions::slot_ok);
	btn_cancel.sig_clicked.connect (this, &Window::request_close);
	btn_reset.sig_clicked.connect (this, &WindowGlobalOptions::slot_reset);
	btn_help.sig_clicked.connect (this, &WindowGlobalOptions::slot_help);

	set_default_button (btn_ok);
	register_hotkey (ESCAPE, btn_cancel.sig_clicked);
	register_hotkey (F1, btn_help.sig_clicked);

	// Copy existing option values to controls
	copy_options_to_controls (options);

	WindowGlobalOptions::redraw ();
}

WindowGlobalOptions::~WindowGlobalOptions ()
{
}

void WindowGlobalOptions::copy_options_to_controls (const GlobalOptionGroup &grp)
{
	lbl_default_file_name.set_text (grp.get_wstring (GLOBAL_OPTION_DEFAULT_FILE), UIString::NO_HOTKEY);
	drp_expand_lines.set_select (grp.get_num (GLOBAL_OPTION_EXPAND_LINES) - 1, false);
	txt_editor.set_text (grp.get_wstring (GLOBAL_OPTION_EDITOR), false);
	txt_datetime_format.set_text (grp.get_wstring (GLOBAL_OPTION_DATETIME_FORMAT), false);
	txt_longtime_format.set_text (grp.get_wstring (GLOBAL_OPTION_LONGTIME_FORMAT), false);
}

void WindowGlobalOptions::slot_default_file ()
{
	lbl_default_file_name.set_text(
			get_full_pathname (
				dialog_select_file (L"Default file",
					lbl_default_file_name.get_text (),
					SELECT_FILE_READ)),
			UIString::NO_HOTKEY);
}

void WindowGlobalOptions::slot_default_file_current ()
{
	lbl_default_file_name.set_text (
			current_filename.empty () ? std::wstring () :
			get_full_pathname (current_filename),
			UIString::NO_HOTKEY);
}

void WindowGlobalOptions::slot_ok ()
{
	options.set (GLOBAL_OPTION_DEFAULT_FILE, lbl_default_file_name.get_text ());
	options.set (GLOBAL_OPTION_EXPAND_LINES, unsigned (drp_expand_lines.get_select ())+1);
	options.set (GLOBAL_OPTION_EDITOR, txt_editor.get_text ());
	options.set (GLOBAL_OPTION_DATETIME_FORMAT, txt_datetime_format.get_text ());
	options.set (GLOBAL_OPTION_LONGTIME_FORMAT, txt_longtime_format.get_text ());
	Window::request_close ();
}

void WindowGlobalOptions::slot_reset ()
{
	if (ui::dialog_message (L"Do you really want to reset all settings to default?",
				MESSAGE_YES|MESSAGE_NO|MESSAGE_DEFAULT_NO) == MESSAGE_YES) {
		// Use a new GlobalOptionGroup object
		// Do not clobber options in this function
		copy_options_to_controls (GlobalOptionGroup ());
	}
}

const wchar_t help_info[] = L"\
Default file: This file is automatically loaded, unless you explicitly open another one.\n\
\n\
Expand lines: The number of lines the selected diary entry should use on screen.\n\
\n\
Editor: The editor used to edit diary entries.\n\
\n\
    You can specify multiple editors, delimited by pipe signs(|).\n\
    You can also refer to environment variables, e.g. \"$EDITOR|vi\"\n\
\n\
Time format: This specifies how to display date/time in the main window.\n\
Long time format: This specifies how to display date/time when viewing entries.\n\
    %Y: 4-digit year (1989)        %y: 2-digit year (1989)\n\
    %m: 2-digit month (06)         %d: 2-digit day of month (04)\n\
    %b: 3-character month (Jun)    %w: 3-character day of week (Sun)\n\
    %B: Month in full (June)       %W: Day of week in full (Sunday)\n\
    %H: 2-digit hour (00-23)       %h: 2-digit hour (01-12)\n\
    %M: 2-digit minute (00-59)     %S: 2-digit second (00-59)\n\
    %P: Upper-case AM/PM           %p: Lower-case am/pm\n\
    Example: %b %d, %Y\n\
";

void WindowGlobalOptions::slot_help ()
{
	dialog_message (help_info);
}

} // anonymous namespace



void edit_global_options (GlobalOptionGroup &options, const std::wstring &current_filename)
{
	WindowGlobalOptions (options, current_filename).event_loop ();
}


} //namespace tiary
