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


#include "diary/diary.h"
#include "ui/fixed_dialog.h"
#include "ui/droplist.h"
#include "ui/label.h"
#include "ui/button.h"
#include "ui/chain.h"
#include "ui/layout.h"
#include "common/datetime.h"
#include "common/format.h"

namespace tiary {

namespace {

using namespace ui;

class DialogTime : public FixedDialog
{

	Label lbl_weekday;
	DropList drp_month;
	DropList drp_day;
	Label lbl_comma;
	DropList drp_year;

	DropList drp_hour;
	Label lbl_colon1;
	DropList drp_minute;
	Label lbl_colon2;
	DropList drp_second;

	Button btn_now;
	Button btn_ok;

	Layout layout_main;
	Layout layout_date;
	Layout layout_time;
	Layout layout_buttons;

	bool canceled;

public:
	explicit DialogTime (ReadableDateTime);
	~DialogTime ();

	bool get_canceled () const { return canceled; }
	DateTime get_result () const;

private:

	void slot_now ();
	void slot_ok ();
	void slot_date_changed ();
};

const wchar_t month_names [][4] = {
	L"Jan", L"Feb", L"Mar", L"Apr", L"May", L"Jun",
	L"Jul", L"Aug", L"Sep", L"Oct", L"Nov", L"Dec"
};
const wchar_t day_names [][3] = {
	L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9", L"10",
	L"11", L"12", L"13", L"14", L"15", L"16", L"17", L"18", L"19", L"20",
	L"21", L"22", L"23", L"24", L"25", L"26", L"27", L"28", L"29", L"30",
	L"31"
};

const unsigned first_year = 1900;
const unsigned last_year = 2100;

std::vector<std::wstring> make_year_names ()
{
	std::vector<std::wstring> r (last_year - first_year + 1);
	for (unsigned k=0; k<last_year-first_year+1; ++k)
		r[k] = format (L"%a") << (k + first_year);
	return r;
}

const wchar_t num_names [][3] = {
	L"0", L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9",
	L"10", L"11", L"12", L"13", L"14", L"15", L"16", L"17", L"18", L"19",
	L"20", L"21", L"22", L"23", L"24", L"25", L"26", L"27", L"28", L"29",
	L"30", L"31", L"32", L"33", L"34", L"35", L"36", L"37", L"38", L"39",
	L"40", L"41", L"42", L"43", L"44", L"45", L"46", L"47", L"48", L"49",
	L"50", L"51", L"52", L"53", L"54", L"55", L"56", L"57", L"58", L"59",
};

DialogTime::DialogTime (ReadableDateTime date_time)
	: FixedDialog ()
	, lbl_weekday (*this, std::wstring ())
	, drp_month (*this, std::vector<std::wstring>(month_names, array_end(month_names)), date_time.m-1)
	, drp_day (*this, std::vector<std::wstring>(day_names, array_end(day_names)), date_time.d-1)
	, lbl_comma (*this, L",")
	, drp_year (*this, make_year_names (), date_time.y - first_year)
	, drp_hour (*this, std::vector<std::wstring>(num_names, num_names+24), date_time.H)
	, lbl_colon1 (*this, L":")
	, drp_minute (*this, std::vector<std::wstring>(num_names, num_names+60), date_time.M)
	, lbl_colon2 (*this, L":")
	, drp_second (*this, drp_minute.get_items (), date_time.S)
	, btn_now (*this, L"&Now")
	, btn_ok (*this, L"&OK")
	, layout_main (VERTICAL)
	, layout_date (HORIZONTAL)
	, layout_time (HORIZONTAL)
	, layout_buttons (HORIZONTAL)
	, canceled (true)
{
	ChainControlsHorizontal () (drp_month) (drp_day) (drp_year) (drp_hour) (drp_minute) (drp_second)
		(btn_now) (btn_ok);

	btn_now.ctrl_up = btn_ok.ctrl_up = &drp_hour;

	btn_ok.set_attribute (Button::DEFAULT_BUTTON);

	FixedDialog::resize (make_size (26, 9));

	layout_date.add
		(lbl_weekday, 3, 3)
		(1, 1)
		(drp_month, 3, 3)
		(1, 1)
		(drp_day, 2, 2)
		(lbl_comma, 2, 2)
		(drp_year, 4, 4)
		;
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
		(layout_date, 1, 1)
		(1, 1)
		(layout_time, 1, 1)
		(1, 1)
		(layout_buttons, 3, 3)
		;

	layout_main.move_resize (make_size (2, 1), make_size (22, 7));

	drp_month.sig_select_changed.connect (this, &DialogTime::slot_date_changed);
	drp_day.sig_select_changed.connect (this, &DialogTime::slot_date_changed);
	drp_year.sig_select_changed.connect (this, &DialogTime::slot_date_changed);
	btn_now.sig_clicked.connect (this, &DialogTime::slot_now);
	btn_ok.sig_clicked.connect (this, &DialogTime::slot_ok);
	register_hotkey (ESCAPE, Signal (this, &Window::request_close));

	slot_date_changed ();

	DialogTime::redraw ();
}

DialogTime::~DialogTime ()
{
}

DateTime DialogTime::get_result () const
{
	unsigned y = drp_year.get_select () + first_year;
	unsigned m = drp_month.get_select () + 1;
	unsigned d = drp_day.get_select () + 1;
	unsigned H = drp_hour.get_select ();
	unsigned M = drp_minute.get_select ();
	unsigned S = drp_second.get_select ();
	return DateTime (y, m, d, H, M, S);
}

void DialogTime::slot_now ()
{
	ReadableDateTime dt = DateTime (DateTime::LOCAL).extract ();
	drp_year.set_select (dt.y - first_year, false);
	drp_month.set_select (dt.m - 1, false);
	drp_day.set_select (dt.d - 1, false);
	drp_hour.set_select (dt.H, false);
	drp_minute.set_select (dt.M, false);
	drp_second.set_select (dt.S, false);
}

void DialogTime::slot_ok ()
{
	canceled = false;
	request_close ();
}

void DialogTime::slot_date_changed ()
{
	DateTime dt = get_result ();
	ReadableDateTime rdt = dt.extract ();
	if (rdt.d != drp_day.get_select () + 1) {
		drp_month.set_select (rdt.m - 1, false);
		drp_day.set_select (rdt.d - 1, false);
		drp_year.set_select (rdt.y - first_year, false);
	}
	static const wchar_t weekday_name[] = L"SunMonTueWedThuFriSat";
	unsigned w = rdt.w;
	lbl_weekday.set_text (std::wstring (weekday_name+w*3, weekday_name+(w+1)*3));
}

} // anonymous namespace

bool DiaryEntry::edit_time ()
{
	DialogTime win (local_time.extract ());
	win.event_loop ();
	if (win.get_canceled ())
		return false;
	DateTime old = local_time;
	local_time = win.get_result ();
	return (old != local_time);
}


} // namespace tiary
