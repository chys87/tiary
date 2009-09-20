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


#ifndef TIARY_COMMON_DATETIME_H
#define TIARY_COMMON_DATETIME_H

#include "common/types.h"
#include <string>
#include <time.h>

namespace tiary {

/**
 * @brief	Returns whether a year is leap under the Gregorian calendar
 */
bool is_leap_year (unsigned) throw ();
/**
 * @brief	Returns the number of days in a given month under the Gregorian calendar
 */
unsigned day_of_month (unsigned y, unsigned m) throw ();

/*
 * Date is represented in a 32-bit unsigned integer number
 *
 * 1 = Jan 1, AD 1 (Always assuming Gregorian)
 *
 * Time is represented in a 32-bit unsigned integer number, the number of
 * seconds elapsed since midnight
 *
 * DateTime is represented in a 64-bit unsigned integer number:
 * (Date*SECONDS_PER_DAY + Time)
 */

const uint32_t INVALID_DATE = 0;
const uint64_t INVALID_DATETIME = 0;

struct ReadableDate
{
	unsigned y, m, d;
	unsigned w; // 0 = Sun, 1 = Mon, ...
};
struct ReadableTime
{
	unsigned H, M, S;
};
struct ReadableDateTime : ReadableDate, ReadableTime
{
};

/**
 * @brief	Make a 32-bit unsigned integer representing the specified date
 * @result	INVALID_DATE = Error
 *
 * This function allows "loose representations" like 1989-5-35
 */
uint32_t make_date (const ReadableDate &) throw ();
/**
 * This function disallows "loose representations" like 1989-5-35
 */
uint32_t make_date_strict (const ReadableDate &) throw ();

/**
 * @brief	Make a 32-bit unsigned integer representing the specified time
 *
 * There is no make_time_strict counterpart. Always allow substandard times
 * like 25:00:00
 */
uint32_t make_time (const ReadableTime &) throw ();

/**
 * @brief	Make a 64-bit unsigned integer representing the specified date and time.
 * @result	INVALID_DATETIME = Error
 */
uint64_t make_datetime_strict (const ReadableDate &, const ReadableTime &) throw ();
uint64_t make_datetime_strict (const ReadableDateTime &) throw ();
/**
 * This function allows representations like 1989-5-35
 */
uint64_t make_datetime (const ReadableDate &, const ReadableTime &) throw ();
uint64_t make_datetime (const ReadableDateTime &) throw ();
uint64_t make_datetime (uint32_t date, uint32_t time) throw ();
uint64_t make_datetime_utc (time_t = ::time (0)) throw ();
uint64_t make_datetime_local (time_t = ::time (0)) throw ();


ReadableDate extract_date (uint32_t) throw ();
ReadableTime extract_time (uint32_t) throw ();

uint32_t extract_date_from_datetime (uint64_t) throw ();
uint32_t extract_time_from_datetime (uint64_t) throw ();

ReadableDateTime extract_datetime (uint64_t) throw ();

/*
 * %Y	4-digit year (1989)
 * %y	2-digit year (89)
 * %m	2-digit month (06)
 * %d	2-digit day (04)
 * %b	3-character month (Jun)
 * %w	3-character weekday (Sun)
 * %B	Full month (June)
 * %W	Full weekday (July)
 * %H	2-digit hour (0-24) (03)
 * %h	2-digit hour (1-12) (03)
 * %M	2-digit minute (20)
 * %S	2-digit second (00)
 * %P	2-character AM/PM (AM)
 * %p	2-character am/pm (am)
 */
std::wstring format_datetime (uint64_t, const wchar_t *format);

struct Date;
struct Time;
struct DateTime;

struct Date
{
	uint32_t v;

	Date () : v (0) {}
	Date (unsigned y, unsigned m, unsigned d, bool strict = false)
	{
		ReadableDate rd = { y, m, d };
		v = strict ? make_date_strict (rd) : make_date (rd);
	}
	Date (const ReadableDate &rd, bool strict = false) : v (strict ? make_date_strict (rd) : make_date (rd)) {}
	explicit Date (uint32_t x) : v(x) {}
	ReadableDate extract () const { return extract_date (v); }
};

struct Time
{
	uint32_t v;
	Time () : v (0) {}
	Time (unsigned H, unsigned M, unsigned S)
	{
		ReadableTime rt = { H, M, S };
		v = make_time (rt);
	}
	Time (const ReadableTime &rd) : v (make_time (rd)) {}
	explicit Time (uint32_t x) : v(x) {}
	ReadableTime extract () const { return extract_time (v); }
};

struct DateTime
{

	uint64_t v;

	enum UTCLocal { UTC, LOCAL };

	DateTime () : v(0) {}
	DateTime (Date date, Time time) : v (make_datetime (date.v, time.v)) {}
	DateTime (const ReadableDate &rd, const ReadableTime &rt, bool strict = false)
		: v(strict ? make_datetime_strict (rd, rt) : make_datetime (rd, rt)) {}
	DateTime (const ReadableDateTime &rdt, bool strict = false)
		: v(strict ? make_datetime_strict (rdt) : make_datetime (rdt)) {}
	explicit DateTime (uint64_t val) : v(val) {}
	DateTime (UTCLocal ul, time_t tv = ::time (0)) : v (ul==UTC ? make_datetime_utc (tv) : make_datetime_local (tv)) {}

	ReadableDateTime extract () const { return extract_datetime (v); }

	operator Date () const { return Date (extract_date_from_datetime (v)); }
	operator Time () const { return Time (extract_time_from_datetime (v)); }

	std::wstring format (const wchar_t *format) const { return format_datetime (v, format); }
	std::wstring format (const std::wstring &format) const { return format_datetime (v, format.c_str ()); }
};

inline bool operator == (const DateTime &a, const DateTime &b) { return a.v == b.v; }
inline bool operator != (const DateTime &a, const DateTime &b) { return a.v != b.v; }
inline bool operator <= (const DateTime &a, const DateTime &b) { return a.v <= b.v; }
inline bool operator >= (const DateTime &a, const DateTime &b) { return a.v >= b.v; }
inline bool operator < (const DateTime &a, const DateTime &b) { return a.v < b.v; }
inline bool operator > (const DateTime &a, const DateTime &b) { return a.v > b.v; }

} // namespace tiary

#endif // Include guard
