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


#include "common/datetime.h"
#include "common/string.h"
#include "common/containers.h"

namespace tiary {

namespace {

template <typename T>
void to_string_2 (std::basic_string <T> &dst, unsigned x)
{
	dst += T('0') + (x/10)%10;
	dst += T('0') + (x%10);
}

template <typename T>
void to_string_4 (std::basic_string <T> &dst, unsigned x)
{
	to_string_2 (dst, x/100);
	to_string_2 (dst, x);
}

const char weekday_name[] = "SunMonTueWedThuFriSat";

template <typename T> inline
void fill_weekday_name (std::basic_string <T> &dst, unsigned n)
{
	const char *p = &weekday_name[n*3];
	dst += T(*p++);
	dst += T(*p++);
	dst += T(*p++);
}

void append_str (std::string &dst, const char *str)
{
	dst += str;
}

void append_str (std::wstring &dst, const char *str)
{
	dst.append (str, strend (str));
}

const char full_weekday_name[][10] = {
	"Sunday",
	"Monday",
	"Tuesday",
	"Wednesday",
	"Thursday",
	"Friday",
	"Saturday"
};

template <typename T> inline
void fill_full_weekday_name (std::basic_string <T> &dst, unsigned n)
{
	append_str (dst, full_weekday_name[n]);
}

const char month_name[] = "JanFebMarAprMayJunJulAugSepOctNovDec";

template <typename T> inline
void fill_month_name (std::basic_string <T> &dst, unsigned m)
{
	const char *p = &month_name[m*3-3];
	dst += T(*p++);
	dst += T(*p++);
	dst += T(*p++);
}

const char full_month_name[][10] = {
	"January",
	"February",
	"March",
	"April",
	"May",
	"June",
	"July",
	"August",
	"September",
	"October",
	"November",
	"December"
};

template <typename T> inline
void fill_full_month_name (std::basic_string <T> &dst, unsigned m)
{
	append_str (dst, full_month_name[m-1]);
}

template <typename T> inline
std::basic_string<T> format_time_impl (uint64_t v, const T *fmt)
{
	ReadableDateTime rdt = extract_datetime (v);
	std::basic_string<T> ret;
	ret.reserve (strlen (fmt)*2);
	while (const T *p = strchr (fmt, T('%'))) {
		if (p[1] == T('\0'))
			break;
		ret.append (fmt, p);
		fmt = p+2;
		switch (p[1]) {
			default:
			case T('%'):
				ret += p[1];
				break;
			case T('Y'):
				to_string_4 (ret, rdt.y);
				break;
			case T('y'):
				to_string_2 (ret, rdt.y);
				break;
			case T('m'):
				to_string_2 (ret, rdt.m);
				break;
			case T('d'):
				to_string_2 (ret, rdt.d);
				break;
			case T('b'):
				fill_month_name (ret, rdt.m);
				break;
			case T('w'):
				fill_weekday_name (ret, rdt.w);
				break;
			case T('B'):
				fill_full_month_name (ret, rdt.m);
				break;
			case T('W'):
				fill_full_weekday_name (ret, rdt.w);
				break;
			case T('H'):
				to_string_2 (ret, rdt.H);
				break;
			case T('h'):
				to_string_2 (ret, (rdt.H+11)%12+1);
				break;
			case T('M'):
				to_string_2 (ret, rdt.M);
				break;
			case T('S'):
				to_string_2 (ret, rdt.S);
				break;
			case T('P'):
				ret += (rdt.H < 12) ? T('A') : T('P');
				ret += T('M');
				break;
			case T('p'):
				ret += (rdt.H < 12) ? T('a') : T('p');
				ret += T('m');
				break;
		}
	}
	ret += fmt;
	return ret;
}


} // anonymous namespace

std::string format_time (uint64_t val, const char *fmtstr)
{
	return format_time_impl (val, fmtstr);
}

std::wstring format_time (uint64_t val, const wchar_t *fmtstr)
{
	return format_time_impl (val, fmtstr);
}







} // namespace tiary
