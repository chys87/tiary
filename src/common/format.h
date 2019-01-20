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


#ifndef TIARY_COMMON_FORMAT_H
#define TIARY_COMMON_FORMAT_H
/**
 * @file	common/format.h
 * @author	chys <admin@chys.info>
 * @brief	Easily formats a string
 *
 * Example:
 *
 * std::wstring s = format(L"I am %a. I am %b years old.", L"chys", 31u);
 *
 * The maximal number of arguments is 26, from %a to %z (case sensitive)
 *
 * Can prepend width info before the specifiers, e.g. %5a
 * (Screen width, not number of bytes or wchar_t's)
 * Prepend an extra 0 to fill the space with zeroes: e.g. %05a
 * Prepend an extra - to specify left alignment: e.g. %-4b
 * (Cannot specify 0 and - at the same time)
 *
 *
 * In fact, I really would like to wrap the printf-family functions.
 * I like the printf-family functions. But:
 *   -# snprintf is unavailable in some old systems; the return value of
 *       snprintf is non-standard in many implementations.
 *   -# swprintf can have different prototypes; and swprintf has different
 *       return values even according to C99.
 *   -# Some C libraries interpret "%s" differently from C99 in the
 *       wide-character version.
 */

#include <string>
#include <string_view>

namespace tiary {

enum struct HexTag: unsigned {};

inline constexpr HexTag hex(unsigned v) {
	return static_cast<HexTag>(v);
}


// Format one decimal number
std::wstring format_dec (unsigned x, unsigned wid = 0, wchar_t fill = L' ');
void format_dec(std::wstring *, unsigned x, unsigned wid = 0, wchar_t fill = L' ');
std::string format_dec_narrow (unsigned x);
// Format one hexadecimal number
std::wstring format_hex (unsigned x);
void format_hex(std::wstring *, unsigned x);
std::string format_hex_narrow (unsigned x);

// Format floating point numbers, currently not suppoted by Format class
// This is a very simple and naive implementation, lacking
// support for INF or NAN.
// Even negative numbers are _not_ supported!!
// Nor are very large numbers or large frac_digits supported..
// ( x * pow(10,frac_digits) must be within the limits of unsigned )
std::wstring format_double (double x, unsigned int_digits, unsigned frac_digits);


class Format {
public:
	static const unsigned MAX_ARGS = 26;
	struct Result {};

	explicit Format(std::wstring_view fmt) : format_(fmt), args_(), nargs_(0) { offset_[0] = 0; }
	~Format ();

	void add(wchar_t);
	void add(std::wstring_view);
	void add(unsigned);
	void add(HexTag);

	Format &operator << (wchar_t c) { add(c); return *this; }
	Format &operator << (std::wstring_view s) { add(s); return *this; }
	// Decimal
	Format &operator << (unsigned v) { add(v); return *this; }
	// Hexidecimal
	Format &operator << (HexTag v) { add(v); return *this; }

	std::wstring result() const;

	operator std::wstring() const { return result(); }
	std::wstring operator << (Result) const { return result(); }

private:
	std::wstring_view format_;      ///< Format string
	std::wstring args_;             ///< The concatenation of all args
	unsigned nargs_;                ///< Current number of args
	unsigned offset_[MAX_ARGS + 1]; ///< Offset in args

};

template <typename... Args>
inline std::wstring format(std::wstring_view fmt, Args&&... args) {
	return (Format(fmt) << ... << std::forward<Args>(args));
}

} // namespace tiary

#endif // Include guard
