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
T *to_string_2 (T *buffer, unsigned x)
{
	buffer[0] = T('0') + (x/10)%10;
	buffer[1] = T('0') + (x%10);
	return buffer+2;
}

template <typename T>
T *to_string_4 (T *buffer, unsigned x)
{
	buffer = to_string_2 (buffer, x/100);
	buffer = to_string_2 (buffer, x);
	return buffer;
}

inline char *strcpy_end (char *buffer, const char *s)
{
	return stpcpy (buffer, s);
}

wchar_t *strcpy_end (wchar_t *buffer, const char *s)
{
	while (*s)
		*buffer++ = *s++;
	return buffer;
}

const char weekday_name[] = "SunMonTueWedThuFriSat";

template <typename T> inline
T *fill_weekday_name (T *buffer, unsigned n)
{
	const char *p = &weekday_name[n*3];
	*buffer++ = T(*p++);
	*buffer++ = T(*p++);
	*buffer++ = T(*p++);
	return buffer;
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
T *fill_full_weekday_name (T *buffer, unsigned n)
{
	return strcpy_end (buffer, full_weekday_name[n]);
}

const char month_name[] = "JanFebMarAprMayJunJulAugSepOctNovDec";

template <typename T> inline
T *fill_month_name (T *buffer, unsigned m)
{
	const char *p = &month_name[m*3-3];
	*buffer++ = T(*p++);
	*buffer++ = T(*p++);
	*buffer++ = T(*p++);
	return buffer;
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
T *fill_full_month_name (T *buffer, unsigned m)
{
	return strcpy_end (buffer, full_month_name[m-1]);
}

template <typename T> inline
std::basic_string<T> format_datetime (const DateTime &dt, const T *fmt)
{
	// FIXME: Directly write to a string object, barring any overflow potential
	ReadableDateTime rdt = dt.extract ();
	T *buffer = new T[strlen(fmt)*5+1];
	T *q = buffer;
	while (const T *p = strchr (fmt, T('%'))) {
		if (p[1] == T('\0'))
			break;
		q = (T*) mempcpy (q, fmt, uintptr_t(p) - uintptr_t(fmt));
		fmt = p+2;
		switch (p[1]) {
			default:
			case T('%'):
				*q++ = p[1];
				break;
			case T('Y'):
				q = to_string_4 (q, rdt.y);
				break;
			case T('y'):
				q = to_string_2 (q, rdt.y);
				break;
			case T('m'):
				q = to_string_2 (q, rdt.m);
				break;
			case T('d'):
				q = to_string_2 (q, rdt.d);
				break;
			case T('b'):
				q = fill_month_name (q, rdt.m);
				break;
			case T('w'):
				q = fill_weekday_name (q, rdt.w);
				break;
			case T('B'):
				q = fill_full_month_name (q, rdt.m);
				break;
			case T('W'):
				q = fill_full_weekday_name (q, rdt.w);
				break;
			case T('H'):
				q = to_string_2 (q, rdt.H);
				break;
			case T('h'):
				q = to_string_2 (q, (rdt.H+11)%12+1);
				break;
			case T('M'):
				q = to_string_2 (q, rdt.S);
				break;
			case T('S'):
				q = to_string_2 (q, rdt.S);
				break;
			case T('P'):
				*q++ = (rdt.H < 12) ? T('A') : T('P');
				*q++ = T('M');
				break;
			case T('p'):
				*q++ = (rdt.H < 12) ? T('a') : T('p');
				*q++ = T('m');
				break;
		}
	}
	q = stpcpy (q, fmt);
	std::basic_string<T> ret (buffer, q);
	delete [] buffer;
	return ret;
}


} // anonymous namespace

std::string DateTime::format (const char *fmtstr) const
{
	return format_datetime (*this, fmtstr);
}

std::wstring DateTime::format (const wchar_t *fmtstr) const
{
	return format_datetime (*this, fmtstr);
}







} // namespace tiary
