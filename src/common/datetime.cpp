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

// Cumulative leap days from Year 1 to Year y-1 (either true/pseudo cal)
inline unsigned cumul_leap_days (unsigned y) throw ()
{
	return y/4 - y/100 + y/100/4;
}

} // anonymous namespace

// Checks whether a year is leap; either true/pseudo cal
bool is_leap_year (unsigned y) throw ()
{
	if (y % 4) {
		return false;
	}
	if (!(y%100) && (y/100%4)) {
		return false;
	}
	return true;
}

unsigned day_of_month (unsigned y, unsigned m) throw ()
{
	--m;
	if (m >= 12) {
		return 0;
	}
	static const unsigned char mdays [12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	if (m != 1) {
		return mdays[m];
	}
	else if (is_leap_year (y)) {
		return 29;
	}
	else {
		return 28;
	}
}


uint32_t make_date_strict (const ReadableDate &rd) throw ()
{
	unsigned y = rd.y;
	unsigned m = rd.m;
	unsigned d = rd.d;
	if (m-1>=12 || !y) {
		return INVALID_DATE;
	}
	if ((int)(m -= 2) <= 0) {
		m += 12;
		--y;
	} // Now y == pseudoyear - 1
	if (d-1>=(unsigned)(days[m]-days[m-1])) {
		return 0;
	}
	if (m>=12 && d>=29 && !is_leap_year (y+1)) {
		return 0;
	}
	return (y * 365 + cumul_leap_days (y) + days[m-1] + d - 306);
}

uint32_t make_date (const ReadableDate &rd) throw ()
{
	unsigned y = rd.y;
	unsigned m = rd.m;
	unsigned d = rd.d;
	if (m-1 >= 12) { // Adjust year/month if month not in [1,12]
		// Division of negative integers has implementation-defined results
		// Avoid it!
		if (int(m-1) >= 0) {
			y += (m-1) / 12;
			m = (m-1) % 12 + 1;
		}
		else {
			y -= unsigned(-m) / 12;
			m = 12 - unsigned(-m) % 12;
		}
	}
	if ((int)(m -= 2) <= 0) {
		m += 12;
		--y;
	} // Now y == pseudoyear - 1
	return (y * 365 + cumul_leap_days (y) + days[m-1] + d - 306);
}

uint32_t make_time (const ReadableTime &rt) throw ()
{
	return ((rt.H*60 + rt.M)*60 + rt.S);
}

uint64_t make_datetime (uint32_t date, uint32_t time) throw ()
{
	return (date * uint64_t (SECONDS_PER_DAY) + time);
}

uint64_t make_datetime_strict (const ReadableDate &rd, const ReadableTime &rt) throw ()
{
	if (rt.H>=24 || rt.M>=60 || rt.S>=60) {
		return INVALID_DATETIME;
	}
	uint32_t time_v = make_time (rt);
	uint32_t date_v = make_date_strict (rd);
	if (date_v == INVALID_DATE) {
		return INVALID_DATETIME;
	}
	return make_datetime (date_v, time_v);
}

uint64_t make_datetime_strict (const ReadableDateTime &rdt) throw ()
{
	return make_datetime_strict (rdt, rdt);
}

uint64_t make_datetime (const ReadableDate &rd, const ReadableTime &rt) throw ()
{
	return make_datetime (make_date (rd), make_time (rt));
}

uint64_t make_datetime (const ReadableDateTime &rdt) throw ()
{
	return make_datetime (rdt, rdt);
}

uint64_t make_datetime_local (time_t t) throw ()
{
#if 0 //def TIARY_HAVE_LOCALTIME_R_AND_GMTIME_R
	struct tm tmbuf;
	struct tm *T = localtime_r (&t, &tmbuf);
#else
	struct tm *T = localtime (&t);
#endif
	struct ReadableDate rd = { (unsigned)T->tm_year+1900, (unsigned)T->tm_mon+1, (unsigned)T->tm_mday };
	struct ReadableTime rt = { (unsigned)T->tm_hour, (unsigned)T->tm_min, (unsigned)T->tm_sec };
	return make_datetime (rd, rt);
}

uint64_t make_datetime_utc (time_t t) throw ()
{
#if 0 //def TIARY_HAVE_LOCALTIME_R_AND_GMTIME_R
	struct tm tmbuf;
	struct tm *T = gmtime_r (&t, &tmbuf);
#else
	struct tm *T = gmtime (&t);
#endif
	struct ReadableDate rd = { (unsigned)T->tm_year+1900, (unsigned)T->tm_mon+1, (unsigned)T->tm_mday };
	struct ReadableTime rt = { (unsigned)T->tm_hour, (unsigned)T->tm_min, (unsigned)T->tm_sec };
	return make_datetime (rd, rt);
}


ReadableDate extract_date (uint32_t v) throw ()
{
	unsigned y, m, d, w;
	unsigned tmp;

	w = v % 7;

	v += 306; // To Pseudodate; 306 = days from Mar to Dec
	--v; // Starting 0
	y = v/(365*400+97)*400;
	v %= 365*400+97; // Now the date is within 400 years
	if ((tmp = v/(365*100+24)) > 3) { // Which century ?
		tmp = 3;
	}
	v -= tmp*(365*100+24); // Days elapsed since beginning of the century
	y += tmp*100 + v/(365*4+1)*4;
	v %= 365*4+1; // Days elapsed since beginning of the 4-year period
	if ((tmp = v / 365) > 3) {
		tmp = 3;
	}
	v -= tmp * 365;
	++v; // Now v = number of day in year; starting 1
	y += tmp + 1; // +1: Convert to human-readable year number
	m = (int)revdays[v-1];
	d = v - days[m + 9];
	if ((int)m <= 0) {
		m += 12;
		--y;
	}

	ReadableDate rd = { y, m, d, w };
	return rd;
}

ReadableTime extract_time (uint32_t v) throw ()
{
	ReadableTime rt;

	rt.S = v % 60;
	unsigned t = v/60;
	rt.M = t % 60;
	rt.H = t / 60;
	return rt;
}

uint32_t extract_date_from_datetime (uint64_t v) throw ()
{
	return uint32_t (v / SECONDS_PER_DAY);
}

uint32_t extract_time_from_datetime (uint64_t v) throw ()
{
	return uint32_t (v % SECONDS_PER_DAY);
}

ReadableDateTime extract_datetime (uint64_t v) throw ()
{
	ReadableDateTime ret;

	uint32_t date_v = extract_date_from_datetime (v);
	uint32_t time_v = extract_time_from_datetime (v);
	(ReadableDate &)ret = extract_date (date_v);
	(ReadableTime &)ret = extract_time (time_v);
	return ret;
}

} // namespace tiary
