// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2019, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#include "common/datetime.h"
#include <wchar.h>

namespace tiary {

namespace {

template <typename C>
void to_string_2(std::basic_string<C> *dst, unsigned x) {
	C buffer[2] = {C('0' + (x / 10) % 10), C('0' + (x % 10)) };
	dst->append(buffer, 2);
}

template <typename C>
void to_string_4(std::basic_string<C> *dst, unsigned x) {
	to_string_2(dst, x / 100);
	to_string_2(dst, x);
}

template <typename C> const C full_weekday_name[7][10];
template <> const wchar_t full_weekday_name<wchar_t>[7][10] = {
	L"Sunday", L"Monday", L"Tuesday", L"Wednesday", L"Thursday", L"Friday", L"Saturday"
};
template <> const char full_weekday_name<char>[7][10] = {
	"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};

template <typename C> const C full_month_name[12][10];
template <> const wchar_t full_month_name<wchar_t>[12][10] = {
	L"January", L"February", L"March", L"April", L"May", L"June",
	L"July", L"August", L"September", L"October", L"November", L"December"
};
template <> const char full_month_name<char>[12][10] = {
	"January", "February", "March", "April", "May", "June",
	"July", "August", "September", "October", "November", "December"
};

template <typename C>
std::basic_string<C> format_datetime_impl(uint64_t v, std::basic_string_view<C> fmt) {
	ReadableDateTime rdt = extract_datetime (v);
	std::basic_string<C> ret;
	ret.reserve(fmt.length() * 2);
	size_t pos = 0;
	size_t percent_pos;
	while ((percent_pos = fmt.find(C('%'), pos)) != fmt.npos) {
		if (percent_pos + 1 >= fmt.length()) {
			break;
		}
		ret += fmt.substr(pos, percent_pos - pos);
		switch (fmt[percent_pos + 1]) {
			default:
			case C('%'):
				ret += fmt[percent_pos + 1];
				break;
			case C('Y'):
				to_string_4(&ret, rdt.y);
				break;
			case C('y'):
				to_string_2(&ret, rdt.y);
				break;
			case C('m'):
				to_string_2(&ret, rdt.m);
				break;
			case C('d'):
				to_string_2(&ret, rdt.d);
				break;
			case C('b'):
				ret.append(full_month_name<C>[rdt.m - 1], 3);
				break;
			case C('w'):
				ret.append(full_weekday_name<C>[rdt.w], 3);
				break;
			case C('B'):
				ret += full_month_name<C>[rdt.m - 1];
				break;
			case C('W'):
				ret += full_weekday_name<C>[rdt.w];
				break;
			case C('H'):
				to_string_2(&ret, rdt.H);
				break;
			case C('h'):
				to_string_2(&ret, (rdt.H + 11) % 12 + 1);
				break;
			case C('M'):
				to_string_2(&ret, rdt.M);
				break;
			case C('S'):
				to_string_2(&ret, rdt.S);
				break;
			case C('P'):
				ret += (rdt.H < 12) ? C('A') : C('P');
				ret += C('M');
				break;
			case C('p'):
				ret += (rdt.H < 12) ? C('a') : C('p');
				ret += C('m');
				break;
		}
		pos = percent_pos + 2;
	}
	ret += fmt.substr(pos);
	return ret;
}

} // anonymous namespace


std::wstring format_datetime(uint64_t v, std::wstring_view fmt) {
	return format_datetime_impl(v, fmt);
}

std::string format_datetime(uint64_t v, std::string_view fmt) {
	return format_datetime_impl(v, fmt);
}


} // namespace tiary
