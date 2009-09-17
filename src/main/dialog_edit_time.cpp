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
 * @file	main/dialog_edit_time.cpp
 * @author	chys <admin@chys.info>
 * @brief	Implements a window to allow the user to edit date & time
 */


#include "main/dialog_edit_time.h"
#include "diary/diary.h"
#include "ui/fixed_window.h"
#include "ui/droplist.h"
#include "ui/label.h"
#include "ui/button.h"
#include "ui/button_default.h"
#include "ui/chain.h"
#include "ui/layout.h"
#include "ui/date_select.h"
#include "common/datetime.h"

namespace tiary {

namespace {

using namespace ui;

class WindowTime : public FixedWindow, private ButtonDefault
{

	DateSelect date_select;

	DropList drp_hour;
	Label lbl_colon1;
	DropList drp_minute;
	Label lbl_colon2;
	DropList drp_second;

	Button btn_now;
	Button btn_ok;

	Layout layout_main;
	Layout layout_time;
	Layout layout_buttons;

	bool canceled;

public:
	explicit WindowTime (ReadableDateTime);
	~WindowTime ();

	bool get_canceled () const { return canceled; }
	DateTime get_result () const;

private:

	void slot_now ();
	void slot_ok ();
};

const wchar_t num_names [][3] = {
	L"00", L"01", L"02", L"03", L"04", L"05", L"06", L"07", L"08", L"09",
	L"10", L"11", L"12", L"13", L"14", L"15", L"16", L"17", L"18", L"19",
	L"20", L"21", L"22", L"23", L"24", L"25", L"26", L"27", L"28", L"29",
	L"30", L"31", L"32", L"33", L"34", L"35", L"36", L"37", L"38", L"39",
	L"40", L"41", L"42", L"43", L"44", L"45", L"46", L"47", L"48", L"49",
	L"50", L"51", L"52", L"53", L"54", L"55", L"56", L"57", L"58", L"59",
};

WindowTime::WindowTime (ReadableDateTime date_time)
	: Window ()
	, FixedWindow ()
	, date_select (*this)
	, drp_hour (*this, std::vector<std::wstring>(num_names, num_names+24), date_time.H)
	, lbl_colon1 (*this, L":")
	, drp_minute (*this, std::vector<std::wstring>(num_names, num_names+60), date_time.M)
	, lbl_colon2 (*this, L":")
	, drp_second (*this, drp_minute.get_items (), date_time.S)
	, btn_now (*this, L"&Now")
	, btn_ok (*this, L"&OK")
	, layout_main (VERTICAL)
	, layout_time (HORIZONTAL)
	, layout_buttons (HORIZONTAL)
	, canceled (true)
{
	date_select.set_date (Date (date_time));

	ChainControlsHorizontal () (date_select.year) (date_select.month) (date_select.day)
		(drp_hour) (drp_minute) (drp_second)
		(btn_now) (btn_ok);

	btn_now.ctrl_up = btn_ok.ctrl_up = &drp_hour;

	set_default_button (btn_ok);

	FixedWindow::resize (make_size (33, 15));

	layout_time.add
		(drp_hour, 2, 2)
		(lbl_colon1, 1, 1)
		(drp_minute, 2, 2)
		(lbl_colon2, 1, 1)
		(drp_second, 2, 2)
		;
	layout_buttons.add
		(btn_now, 10, 10)
		(2, 2)
		(btn_ok, 10, 10)
		;
	layout_main.add
		(date_select, 7, 7)
		(1, 1)
		(layout_time, 1, 1)
		(1, 1)
		(layout_buttons, 3, 3)
		;

	layout_main.move_resize (make_size (2, 1), make_size (29, 13));

	btn_now.sig_clicked.connect (this, &WindowTime::slot_now);
	btn_ok.sig_clicked.connect (this, &WindowTime::slot_ok);
	register_hotkey (ESCAPE, Signal (this, &Window::request_close));

	WindowTime::redraw ();
}

WindowTime::~WindowTime ()
{
}

DateTime WindowTime::get_result () const
{
	return DateTime (date_select.get_date (), Time (
			drp_hour.get_select (), drp_minute.get_select (), drp_second.get_select ())
			);
}

void WindowTime::slot_now ()
{
	DateTime now (DateTime::LOCAL);
	date_select.set_date (now, false);

	ReadableTime rt = Time (now).extract ();
	drp_hour.set_select (rt.H, false);
	drp_minute.set_select (rt.M, false);
	drp_second.set_select (rt.S, false);
}

void WindowTime::slot_ok ()
{
	canceled = false;
	request_close ();
}

} // anonymous namespace

bool edit_entry_time (DiaryEntry &ent)
{
	WindowTime win (ent.local_time.extract ());
	win.event_loop ();
	if (win.get_canceled ()) {
		return false;
	}
	DateTime old = ent.local_time;
	ent.local_time = win.get_result ();
	return (old != ent.local_time);
}


} // namespace tiary
