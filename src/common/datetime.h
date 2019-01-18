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


#ifndef TIARY_COMMON_DATETIME_H
#define TIARY_COMMON_DATETIME_H

#include <stdint.h>
#include <string>
#include <time.h>

namespace tiary {

constexpr unsigned SECONDS_PER_DAY = 24 * 60 * 60;

/**
 * @brief	Returns whether a year is leap under the Gregorian calendar
 *
 * A leap year in the Gregorian calendar is also a leap year in the pseudo calendar
 * in the implementation.
 */
inline constexpr bool is_leap_year(unsigned y) noexcept {
	return (y % 4 == 0) && (y % 100 != 0 || (y / 100 % 4 == 0));
}

static_assert( is_leap_year(1996));
static_assert(!is_leap_year(1997));
static_assert(!is_leap_year(1998));
static_assert(!is_leap_year(1999));
static_assert( is_leap_year(2000));
static_assert(!is_leap_year(2100));

/**
 * @brief	Returns the number of days in a given month under the Gregorian calendar
 */
unsigned days_of_month(unsigned y, unsigned m) noexcept ATTRIBUTE_CONST;

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
 * This function allows "loose representations" like 2018-12-32
 */
uint32_t make_date(const ReadableDate &) noexcept ATTRIBUTE_PURE;
/**
 * This function disallows "loose representations" like 2018-12-32
 */
uint32_t make_date_strict(const ReadableDate &) noexcept ATTRIBUTE_PURE;

constexpr uint32_t kDateAtUnixEpoch = 719163;
constexpr ReadableDate kUnixEpoch = {1970, 1, 1, 4};
constexpr uint64_t kDateTimeAtUnixEpoch = uint64_t(kDateAtUnixEpoch) * SECONDS_PER_DAY;

/**
 * @brief	Make a 32-bit unsigned integer representing the specified time
 *
 * There is no make_time_strict counterpart. Always allow substandard times
 * like 25:00:00
 */
uint32_t make_time(const ReadableTime &) noexcept ATTRIBUTE_PURE;

/**
 * @brief	Make a 64-bit unsigned integer representing the specified date and time.
 * @result	INVALID_DATETIME = Error
 */
uint64_t make_datetime_strict(const ReadableDate &, const ReadableTime &) noexcept ATTRIBUTE_PURE;
uint64_t make_datetime_strict(const ReadableDateTime &) noexcept ATTRIBUTE_PURE;
/**
 * This function allows "loose representations" like 2018-12-32
 */
uint64_t make_datetime(const ReadableDate &, const ReadableTime &) noexcept ATTRIBUTE_PURE;
uint64_t make_datetime(const ReadableDateTime &) noexcept ATTRIBUTE_PURE;
inline constexpr uint64_t make_datetime(uint32_t date, uint32_t time) noexcept {
	return (date * uint64_t (SECONDS_PER_DAY) + time);
}
inline constexpr uint64_t make_datetime_utc(time_t t = ::time (0)) noexcept {
	return kDateTimeAtUnixEpoch + t;
}
uint64_t make_datetime_local (time_t = ::time (0)) noexcept;


ReadableDate extract_date(uint32_t) noexcept ATTRIBUTE_CONST;
ReadableTime extract_time(uint32_t) noexcept ATTRIBUTE_CONST;

inline constexpr uint32_t extract_date_from_datetime(uint64_t v) noexcept {
	return uint32_t(v / SECONDS_PER_DAY);
}
inline constexpr uint32_t extract_time_from_datetime(uint64_t v) noexcept {
	return uint32_t(v % SECONDS_PER_DAY);
}

ReadableDateTime extract_datetime (uint64_t) noexcept ATTRIBUTE_CONST;

/*
 * %Y	4-digit year (2019)
 * %y	2-digit year (19)
 * %m	2-digit month (01)
 * %d	2-digit day (10)
 * %b	3-character month (Mon)
 * %w	3-character weekday (Thu)
 * %B	Full month (January)
 * %W	Full weekday (Thursday)
 * %H	2-digit hour (0-24) (03)
 * %h	2-digit hour (1-12) (03)
 * %M	2-digit minute (20)
 * %S	2-digit second (00)
 * %P	2-character AM/PM (AM)
 * %p	2-character am/pm (am)
 */
std::wstring format_datetime (uint64_t, const wchar_t *format);

class Date;
class Time;
class DateTime;

class Date {
public:
	constexpr Date() : v_(0) {}
	Date(const ReadableDate &rd, bool strict = false) :
		v_(strict ? make_date_strict(rd) : make_date(rd)) {}
	Date(unsigned y, unsigned m, unsigned d, bool strict = false) :
		Date({y, m, d}, strict) {}
	explicit constexpr Date(uint32_t x) : v_(x) {}
	ReadableDate extract() const { return extract_date(v_); }
	constexpr uint32_t get_value() const { return v_; }

private:
	uint32_t v_;
};

class Time {
public:
	constexpr Time() : v_(0) {}
	Time(const ReadableTime &rd) : v_(make_time (rd)) {}
	Time(unsigned H, unsigned M, unsigned S) : Time(ReadableTime{H, M, S}) {}
	explicit constexpr Time(uint32_t x) : v_(x) {}
	ReadableTime extract() const { return extract_time (v_); }
	constexpr uint32_t get_value() const { return v_; }

private:
	uint32_t v_;
};

class DateTime {
public:
	enum UTCLocal { UTC, LOCAL };

	constexpr DateTime() : v_(0) {}
	constexpr DateTime(Date date, Time time) : v_(make_datetime(date.get_value(), time.get_value())) {}
	DateTime (const ReadableDate &rd, const ReadableTime &rt, bool strict = false)
		: v_(strict ? make_datetime_strict(rd, rt) : make_datetime(rd, rt)) {}
	DateTime (const ReadableDateTime &rdt, bool strict = false)
		: v_(strict ? make_datetime_strict(rdt) : make_datetime(rdt)) {}
	explicit constexpr DateTime(uint64_t val) : v_(val) {}
	DateTime(UTCLocal ul, time_t tv = ::time(nullptr)) : v_(ul == UTC ? make_datetime_utc(tv) : make_datetime_local(tv)) {}

	ReadableDateTime extract() const { return extract_datetime(v_); }
	constexpr uint64_t get_value() const { return v_; }

	constexpr operator Date() const { return Date(extract_date_from_datetime(v_)); }
	constexpr operator Time() const { return Time(extract_time_from_datetime(v_)); }

	std::wstring format(const wchar_t *format) const { return format_datetime(v_, format); }
	std::wstring format(const std::wstring &format) const { return format_datetime(v_, format.c_str()); }

private:
	uint64_t v_;
};

inline bool operator == (const DateTime &a, const DateTime &b) { return a.get_value() == b.get_value(); }
inline bool operator != (const DateTime &a, const DateTime &b) { return a.get_value() != b.get_value(); }
inline bool operator <= (const DateTime &a, const DateTime &b) { return a.get_value() <= b.get_value(); }
inline bool operator >= (const DateTime &a, const DateTime &b) { return a.get_value() >= b.get_value(); }
inline bool operator < (const DateTime &a, const DateTime &b) { return a.get_value() < b.get_value(); }
inline bool operator > (const DateTime &a, const DateTime &b) { return a.get_value() > b.get_value(); }

} // namespace tiary

#endif // Include guard
