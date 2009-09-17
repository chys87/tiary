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

void to_string_2 (std::wstring &dst, unsigned x)
{
	wchar_t buffer[2] = { L'0' + (x/10)%10, L'0' + (x%10) };
	dst.append (buffer, 2);
}

void to_string_4 (std::wstring &dst, unsigned x)
{
	to_string_2 (dst, x/100);
	to_string_2 (dst, x);
}

const wchar_t weekday_name[] = L"SunMonTueWedThuFriSat";

const wchar_t full_weekday_name[][10] = {
	L"Sunday",
	L"Monday",
	L"Tuesday",
	L"Wednesday",
	L"Thursday",
	L"Friday",
	L"Saturday"
};

const wchar_t month_name[] = L"JanFebMarAprMayJunJulAugSepOctNovDec";

const wchar_t full_month_name[][10] = {
	L"January",
	L"February",
	L"March",
	L"April",
	L"May",
	L"June",
	L"July",
	L"August",
	L"September",
	L"October",
	L"November",
	L"December"
};

} // anonymous namespace

std::wstring format_datetime (uint64_t v, const wchar_t *fmt)
{
	ReadableDateTime rdt = extract_datetime (v);
	std::wstring ret;
	ret.reserve (strlen (fmt)*2);
	while (const wchar_t *p = wcschr (fmt, L'%')) {
		if (p[1] == L'\0') {
			break;
		}
		ret.append (fmt, p);
		fmt = p+2;
		switch (p[1]) {
			default:
			case L'%':
				ret += p[1];
				break;
			case L'Y':
				to_string_4 (ret, rdt.y);
				break;
			case L'y':
				to_string_2 (ret, rdt.y);
				break;
			case L'm':
				to_string_2 (ret, rdt.m);
				break;
			case L'd':
				to_string_2 (ret, rdt.d);
				break;
			case L'b':
				ret.append (month_name+(rdt.m-1)*3, 3);
				break;
			case L'w':
				ret.append (weekday_name+rdt.w*3, 3);
				break;
			case L'B':
				ret += full_month_name [rdt.m-1];
				break;
			case L'W':
				ret += full_weekday_name [rdt.w];
				break;
			case L'H':
				to_string_2 (ret, rdt.H);
				break;
			case L'h':
				to_string_2 (ret, (rdt.H+11)%12+1);
				break;
			case L'M':
				to_string_2 (ret, rdt.M);
				break;
			case L'S':
				to_string_2 (ret, rdt.S);
				break;
			case L'P':
				ret += (rdt.H < 12) ? L'A' : L'P';
				ret += L'M';
				break;
			case L'p':
				ret += (rdt.H < 12) ? L'a' : 'p';
				ret += L'm';
				break;
		}
	}
	ret += fmt;
	return ret;
}







} // namespace tiary
