// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the so-called 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#include "common/datetime.h"
#include <time.h>

/*
 * We invent a new calendar (`pseudo-calendar' herinafter):
 * Pseudo Year (n+1) starts March 1, n, and ends February 28(29), n+1
 * So that leap days are always at the end of year;
 * and the leap-year rule is the same as in the Gregorian calendar
 *
 * This algorithm is almost identical to the one used in glibc and many other places
 */

namespace tiary {

namespace {

const unsigned SECONDS_PER_DAY = 24*60*60;

// Cumulative days of months in a pseudoyear
const unsigned days[13] = {
	0, 31, 61, 92, 122, 153, 184, 214, 245, 275, 306, 337, 366
};

/* A reverse index; 10 subtracted */
#define REPEAT_2(x) x,x
#define REPEAT_4(x) REPEAT_2(x),REPEAT_2(x)
#define REPEAT_8(x) REPEAT_4(x),REPEAT_4(x)
#define REPEAT_16(x) REPEAT_8(x),REPEAT_8(x)
#define REPEAT_28(x) REPEAT_16(x),REPEAT_8(x),REPEAT_4(x)
#define REPEAT_29(x) REPEAT_28(x),x
#define REPEAT_30(x) REPEAT_29(x),x
#define REPEAT_31(x) REPEAT_30(x),x
const signed char revdays[366] = {
	REPEAT_31(-9), // Mar
	REPEAT_30(-8), // Apr
	REPEAT_31(-7), // May
	REPEAT_30(-6), // Jun
	REPEAT_31(-5), // Jul
	REPEAT_31(-4), // Aug
	REPEAT_30(-3), // Sep
	REPEAT_31(-2), // Oct
	REPEAT_30(-1), // Nov
	REPEAT_31(0),  // Dec
	REPEAT_31(1),  // Jan
	REPEAT_29(2)   // Feb
};

// Checks whether a year is leap; either true/pseudo cal
inline bool is_leap (unsigned y) throw ()
{
	if (y % 4)
		return false;
	if (!(y%100) && (y/100%4))
		return false;
	return true;
}

// Cumulative leap days from Year 1 to Year y-1 (either true/pseudo cal)
inline unsigned cumul_leap_days (unsigned y) throw ()
{
	return y/4 - y/100 + y/100/4;
}

} // anonymous namespace


uint64_t make_time_strict (unsigned y, unsigned m, unsigned d, unsigned H, unsigned M, unsigned S) throw ()
{
	if (H>=24 || M>=60 || S>=60)
		return 0;
	unsigned time_v = (H * 60 + M) * 60 + S;
	--d;
	if (m-1>=12 || !y)
		return 0;
	if ((int)(m -= 2) <= 0) {
		m += 12;
		--y;
	} // Now y == pseudoyear - 1
	if (d>=(unsigned)(days[m]-days[m-1]))
		return 0;
	if (m>=12 && d>=28 && !is_leap(y+1))
		return 0;
	unsigned date_v = y * 365 + cumul_leap_days (y) + days[m-1] + d - 306;
	return date_v * uint64_t(SECONDS_PER_DAY) + time_v;
}

uint64_t make_time_strict (const ReadableDateTime &rdt) throw ()
{
	return make_time_strict (rdt.y, rdt.m, rdt.d, rdt.H, rdt.M, rdt.S);
}

uint64_t make_time (unsigned y, unsigned m, unsigned d, unsigned H, unsigned M, unsigned S) throw ()
{
	unsigned time_v = (H * 60 + M) * 60 + S;
	--d;
	if (m-1 >= 12) { // Adjust year/month if month not in [1,12]
		// Division of negative integers has implementation-defined results
		// Avoid it!
		if (int(m-1) >= 0) {
			y += (m-1) / 12;
			m = (m-1) % 12 + 1;
		} else {
			y -= unsigned(-m) / 12;
			m = 12 - unsigned(-m) % 12;
		}
	}
	if ((int)(m -= 2) <= 0) {
		m += 12;
		--y;
	} // Now y == pseudoyear - 1
	unsigned date_v = y * 365 + cumul_leap_days (y) + days[m-1] + d - 306;
	return date_v * uint64_t(SECONDS_PER_DAY) + time_v;
}

uint64_t make_time (const ReadableDateTime &rdt) throw ()
{
	return make_time (rdt.y, rdt.m, rdt.d, rdt.H, rdt.M, rdt.S);
}

uint64_t make_time_local (time_t t) throw ()
{
#if 0 //def TIARY_HAVE_LOCALTIME_R_AND_GMTIME_R
	struct tm tmbuf;
	struct tm *T = localtime_r (&t, &tmbuf);
#else
	struct tm *T = localtime (&t);
#endif
	return make_time (T->tm_year+1900, T->tm_mon+1, T->tm_mday, T->tm_hour, T->tm_min, T->tm_sec);
}

uint64_t make_time_utc (time_t t) throw ()
{
#if 0 //def TIARY_HAVE_LOCALTIME_R_AND_GMTIME_R
	struct tm tmbuf;
	struct tm *T = gmtime_r (&t, &tmbuf);
#else
	struct tm *T = gmtime (&t);
#endif
	return make_time (T->tm_year+1900, T->tm_mon+1, T->tm_mday, T->tm_hour, T->tm_min, T->tm_sec);
}


namespace {

void extract_date_ (unsigned v, unsigned *year, unsigned *month, unsigned *day) throw ()
{
	unsigned y, m, d;
	unsigned tmp;

	v += 306; // To Pseudodate; 306 = days from Mar to Dec
	y = v/(365*400+97)*400;
	v %= 365*400+97; // Now the date is within 400 years
	if ((tmp = v/(365*100+24)) > 3) // Which century ?
		tmp = 3;
	v -= tmp*(365*100+24); // Days elapsed since beginning of the century
	y += tmp*100 + v/(365*4+1)*4;
	v %= 365*4+1; // Days elapsed since beginning of the 4-year period
	if ((tmp = v / 365) > 3)
		tmp = 3;
	v -= tmp * 365;
	++v; // Now v = number of day in year; starting 1
	y += tmp + 1; // +1: Convert to human-extractable year number
	m = (int)revdays[v-1];
	d = v - days[m + 9];
	if ((int)m <= 0) {
		m += 12;
		--y;
	}

	if (year)
		*year = y;
	if (month)
		*month = m;
	if (day)
		*day = d;
}

unsigned extract_weekday_ (unsigned v) throw ()
{
	return (v + 1) % 7;
}

void extract_time_ (unsigned v, unsigned *pH, unsigned *pM, unsigned *pS) throw ()
{
	unsigned H, M, S;
	S = v % 60;
	unsigned t = v/60;
	M = t % 60;
	H = t / 60;
	if (pH)
		*pH = H;
	if (pM)
		*pM = M;
	if (pS)
		*pS = S;
}

} // anonymous namespace


ReadableDateTime extract_time (uint64_t v) throw ()
{
	ReadableDateTime ret;

	unsigned date_v = unsigned(v / SECONDS_PER_DAY);
	unsigned time_v = unsigned(v % SECONDS_PER_DAY);
	ret.w = extract_weekday_ (date_v);
	extract_date_ (date_v, &ret.y, &ret.m, &ret.d);
	extract_time_ (time_v, &ret.H, &ret.M, &ret.S);
	return ret;
}

void extract_time (uint64_t v, unsigned *y, unsigned *m, unsigned *d, unsigned *H, unsigned *M, unsigned *S, unsigned *w) throw ()
{
	unsigned date_v = unsigned(v / SECONDS_PER_DAY);
	unsigned time_v = unsigned(v % SECONDS_PER_DAY);
	if (w)
		*w = extract_weekday_ (date_v);
	extract_date_ (date_v, y, m, d);
	extract_time_ (time_v, H, M, S);
}

unsigned extract_time_weekday (uint64_t v) throw ()
{
	return extract_weekday_ (unsigned (v / SECONDS_PER_DAY));
}
} // namespace tiary
