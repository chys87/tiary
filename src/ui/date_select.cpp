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


#include "ui/date_select.h"
#include "common/algorithm.h"
#include "common/format.h"
#include "common/string.h"
#include <utility> // std::forward

namespace tiary {
namespace ui {

namespace {

constexpr unsigned kFirstYear = 1900;
constexpr unsigned kLastYear = 2100;
constexpr unsigned kYears = kLastYear - kFirstYear + 1;

std::vector<std::wstring> make_year_names() {
	std::vector<std::wstring> r(kYears);
	for (unsigned k = 0; k < kYears; ++k) {
		r[k] = format_dec(k + kFirstYear);
	}
	return r;
}

const wchar_t month_names[][4] = {
	L"Jan", L"Jul",
	L"Feb", L"Aug",
	L"Mar", L"Sep",
	L"Apr", L"Oct",
	L"May", L"Nov",
	L"Jun", L"Dec"
};

const wchar_t day_names [][3] = {
	L" 1", L" 2", L" 3", L" 4", L" 5", L" 6", L" 7", L" 8", L" 9", L"10",
	L"11", L"12", L"13", L"14", L"15", L"16", L"17", L"18", L"19", L"20",
	L"21", L"22", L"23", L"24", L"25", L"26", L"27", L"28", L"29", L"30",
	L"31"
};

} // anonymous namespace

DateSelect::DateSelect (Window &win)
	: year (win, make_year_names (), 0)
	, month (win)
	, lbl_weekday(win, L" S  M  T  W  T  F  S"sv)
	, day (win)
	, sig_date_changed ()
	, offset_(0)
	, dom_(0)
{
	month.set_grid(4, 2, 6, std::vector<GridItem>(std::begin(month_names), std::end(month_names)));

	year.sig_select_changed = month.sig_select_changed = Signal (this, &DateSelect::update_day_list, true);
	day.sig_select_changed.connect (sig_date_changed);
}

DateSelect::~DateSelect() = default;

void DateSelect::set_date (Date date, bool emit_signal)
{
	ReadableDate rd = date.extract ();
	unsigned y = minU(maxU(rd.y, kFirstYear), kLastYear);
	year.set_select(y - kFirstYear, false);
	unsigned m = rd.m - 1;
	month.set_select (m/6 + (m%6)*2, false);
	update_day_list (false);
	day.set_select(offset_ + rd.d - 1);
	if (emit_signal) {
		sig_date_changed.emit ();
	}
	DateSelect::redraw ();
}

Date DateSelect::get_date () const
{
	unsigned d = day.get_select() - offset_;
	if (d >= dom_) {
		return Date (INVALID_DATE);
	}

	unsigned m = month.get_select ();
	return Date(
		year.get_select() + kFirstYear,
		m / 2 + (m % 2) * 6 + 1,
		d + 1);
}

void DateSelect::move_resize (Size new_pos, Size new_size)
{
	set_pos(new_pos);
	set_size(new_size);
	year.move_resize(new_pos + Size{1, 0}, {4, 1});
	month.move_resize(new_pos + Size{0, 1}, {7, 6});
	lbl_weekday.move_resize(new_pos + Size{8, 0}, {21, 1});
	day.move_resize(new_pos + Size{8, 1}, {21, 6});
}

void DateSelect::redraw ()
{
	year.redraw ();
	month.redraw ();
	lbl_weekday.redraw ();
	day.redraw ();
}

void DateSelect::update_day_list (bool emit_signal)
{
	unsigned current_select_day = day.get_select() - offset_;
	if (current_select_day >= dom_) {
		current_select_day = unsigned (-1);
	}

	unsigned y = year.get_select() + kFirstYear;
	unsigned m = month.get_select ();
	m = m/2 + (m%2)*6 + 1;
	Date first_day_of_month (y, m, 1);
	offset_ = first_day_of_month.weekday();
	dom_ = days_of_month(y, m);

	std::vector<GridItem> grid_items(6 * 7);
	std::copy(day_names, day_names + dom_, grid_items.begin() + offset_);

	size_t grid_select = size_t (-1);
	if (current_select_day < 31) {
		grid_select = minU(current_select_day, dom_ - 1) + offset_;
	}

	day.set_grid(3, 7, 6, std::move(grid_items), grid_select);

	if (emit_signal) {
		sig_date_changed.emit ();
	}
}

} // namespace tiary::ui
} // namespace tiary
