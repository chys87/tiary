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
#include <utility> // std::pair
#include <time.h>

namespace tiary {

/*
 * Time is represented in a 64-bit unsigned integer number
 *
 * 0 = 00:00:00 Jan 1, AD 1 (Always assuming Gregorian; either local or UTC time)
 */

struct ReadableDateTime
{
	unsigned y, m, d;
	unsigned H, M, S;
	unsigned w; // 0 = Sun, 1 = Mon, ...
};

/**
 * @brief	Make a 64-bit unsigned integer representing the specified time.
 * @result	0 = Error
 *
 * This function performs strict checking. Disallowing representations like 1989-5-35
 */
uint64_t make_time_strict (unsigned y, unsigned m, unsigned d, unsigned H, unsigned M, unsigned S) throw ();
uint64_t make_time_strict (const ReadableDateTime &);
/**
 * This function allows representations like 1989-5-35
 */
uint64_t make_time (unsigned y, unsigned m, unsigned d, unsigned H, unsigned M, unsigned S) throw ();
uint64_t make_time (const ReadableDateTime &);
uint64_t make_time_utc (time_t = ::time (0)) throw ();
uint64_t make_time_local (time_t = ::time (0)) throw ();
ReadableDateTime extract_time (uint64_t) throw ();
void extract_time (uint64_t v, unsigned *y, unsigned *m, unsigned *d, unsigned *H, unsigned *M, unsigned *S, unsigned *w) throw ();
unsigned extract_time_weekday (uint64_t) throw ();

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
std::string format_time (uint64_t, const char *format);
std::wstring format_time (uint64_t, const wchar_t *format);


/**
 * @brief	A OO wrapper of date-time
 *
 */
struct DateTime
{

	uint64_t v;

	enum UTCLocal { UTC, LOCAL };

	DateTime () : v(0) {}
	explicit DateTime (uint64_t val) : v(val) {}
	DateTime (unsigned y, unsigned m, unsigned d, unsigned H, unsigned M, unsigned S, bool strict = false)
		: v (strict ? make_time_strict (y, m, d, H, M, S) : make_time (y, m, d, H, M, S)) {}
	DateTime (const ReadableDateTime &rdt, bool strict = false) : v (strict ? make_time_strict (rdt) : make_time (rdt)) {}
	DateTime (const DateTime &other) : v(other.v) {}
	DateTime (UTCLocal ul, time_t tv = ::time (0)) : v (ul==UTC ? make_time_utc (tv) : make_time_local (tv)) {}
	DateTime &operator = (const DateTime &other) { v = other.v; return *this; }
	DateTime &operator = (uint64_t val) { v = val; return *this; }
	DateTime &operator = (const ReadableDateTime &other) { v = make_time (other); return *this; }

	ReadableDateTime extract () const
	{
		return extract_time (v);
	}
	void extract (unsigned *y, unsigned *m, unsigned *d, unsigned *H = 0, unsigned *M = 0, unsigned *S = 0, unsigned *w = 0) const
	{
		extract_time (v, y, m, d, H, M, S, w);
	}
	unsigned extract_weekday () const /* 0 = Sun, 1 = Mon, ... */ { return extract_time_weekday (v); }

	std::string format (const char *format) const { return format_time (v, format); }
	std::wstring format (const wchar_t *format) const { return format_time (v, format); }
};

inline bool operator == (const DateTime &a, const DateTime &b) { return a.v == b.v; }
inline bool operator != (const DateTime &a, const DateTime &b) { return a.v != b.v; }
inline bool operator <= (const DateTime &a, const DateTime &b) { return a.v <= b.v; }
inline bool operator >= (const DateTime &a, const DateTime &b) { return a.v >= b.v; }
inline bool operator < (const DateTime &a, const DateTime &b) { return a.v < b.v; }
inline bool operator > (const DateTime &a, const DateTime &b) { return a.v > b.v; }

} // namespace tiary

#endif // Include guard
