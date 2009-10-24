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


#ifndef TIARY_COMMON_FORMAT_H
#define TIARY_COMMON_FORMAT_H
/**
 * @file	common/format.h
 * @author	chys <admin@chys.info>
 * @brief	Easily formats a string
 *
 * Example:
 *
 * std::wstring s = format (L"I am %a. I am %b years old now.") << L"chys" << 22;
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

namespace tiary {

struct HexTag
{
	unsigned val;
};

inline HexTag hex (unsigned v)
{
	HexTag ret = { v };
	return ret;
}


// Format one decimal number
std::wstring format_dec (unsigned x, unsigned wid = 0, wchar_t fill = L' ');
std::string format_dec_narrow (unsigned x);
// Format one hexadecimal number
std::wstring format_hex (unsigned x);
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

	explicit Format (const wchar_t *fmt) : format (fmt), args(), nargs (0) { offset[0] = 0; }
	~Format ();

	Format &operator << (wchar_t);
	Format &operator << (const wchar_t *);
	Format &operator << (const std::wstring &);

	// Decimal
	Format &operator << (unsigned);
	// Hexidecimal
	Format &operator << (HexTag);

	operator std::wstring () const;

private:
	const wchar_t *format;      ///< Format string
	std::wstring args;          ///< The concatenation of all args
	unsigned nargs;             ///< Current number of args
	unsigned offset[MAX_ARGS+1];///< Offset in args

};

inline Format format (const wchar_t *fmt)
{
	return Format (fmt);
}

} // namespace tiary

#endif // Include guard
