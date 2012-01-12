// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2010, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#ifndef TIARY_COMMON_STRING_H
#define TIARY_COMMON_STRING_H

/**
 * @file	common/string.h
 * @author	chys <admin@chys.info>
 * @brief	Header for string manipulation
 */

#include <vector>
#include <string.h>
#include <list>
#include <string>
#include <iterator>

namespace tiary {

// Allow some "char" functions for wchar_t as well
inline const wchar_t *strchr (const wchar_t *str, wchar_t ch) { return wcschr (str, ch); }
inline wchar_t *strchr (wchar_t *str, wchar_t ch) { return wcschr (str, ch); }
inline const char *strchr (const char *str, int ch) { return ::strchr (str, ch); }
inline char *strchr (char *str, int ch) { return ::strchr (str, ch); }
inline size_t strlen (const char *str) { return ::strlen (str); }
inline size_t strlen (const wchar_t *str) { return ::wcslen (str); }
#ifdef HAVE_MEMPCPY
using ::mempcpy;
#else
void *mempcpy (void *, const void *, size_t);
#endif
#ifdef HAVE_STPCPY
inline char *stpcpy (char *dst, const char *src) { return ::stpcpy (dst, src); }
inline wchar_t *stpcpy (wchar_t *dst, const wchar_t *src) { return ::wcpcpy (dst, src); }
#else
char *stpcpy (char *dst, const char *src);
wchar_t *stpcpy (wchar_t *dst, const wchar_t *src);
#endif
#ifdef HAVE_STRCHRNUL
inline const char *strchrnul (const char *str, int ch) { return ::strchrnul (str, ch); }
inline char *strchrnul (char *str, int ch) { return ::strchrnul (str, ch); }
#else
const char *strchrnul (const char *, int);
inline char *strchrnul (char *str, int ch) { return const_cast <char *> (strchrnul ((const char *)str, ch)); }
#endif
const wchar_t *strchrnul (const wchar_t *, wchar_t);
inline wchar_t *strchrnul (wchar_t *str, wchar_t ch) { return const_cast <wchar_t *> (strchrnul ((const wchar_t *)str, ch)); }


/// @brief	Make a string lowercase
std::wstring strlower (const std::wstring &);

/**
 * @brief	Find all occurrences of a substring
 * @result	For each occurrence, return the offset and length
 *
 * We return the length in the result to align with PcRe::match
 */
std::vector <std::pair <size_t, size_t> >
	find_all (const std::wstring &haystack, const std::wstring &needle);

// Remove spaces at the beginning and the end
// Returns if the string was changed
bool strip (std::string &);
bool strip (std::wstring &);


// Split a string into tokens
std::list <std::string> split_string (const char *, char delimiter);
std::list <std::wstring> split_string (const wchar_t *, wchar_t delimiter);
inline std::list <std::string> split_string (const std::string &str, char delimiter)
{
	return split_string (str.c_str (), delimiter);
}
inline std::list <std::wstring> split_string (const std::wstring &str, wchar_t delimiter)
{
	return split_string (str.c_str (), delimiter);
}


// Join tokens into a string
template <typename InputIterator, typename JoinT>
typename std::iterator_traits<InputIterator>::value_type join (InputIterator first, InputIterator last, const JoinT &joiner)
{
	typename std::iterator_traits<InputIterator>::value_type ret;
	if (first != last) {
		ret = *first;
		while (++first != last) {
			ret += joiner;
			ret += *first;
		}
	}
	return ret;
}

} // namespace tiary

#endif // include guard
