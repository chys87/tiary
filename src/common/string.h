// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2010, 2019, 2020, chys <admin@CHYS.INFO>
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
#include <string_view>
#include <iterator>

namespace tiary {

using namespace std::literals::string_literals;
using namespace std::literals::string_view_literals;

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
std::wstring strlower(std::wstring_view);

/**
 * @brief	Find all occurrences of a substring
 * @result	For each occurrence, return the offset and length
 *
 * We return the length in the result to align with PcRe::match
 */
std::vector <std::pair <size_t, size_t> >
	find_all(std::wstring_view haystack, std::wstring_view needle);

// Remove spaces at the beginning and the end
// Returns if the string was changed
bool strip_in_place(std::string &);
bool strip_in_place(std::wstring &);
std::string_view strip(std::string_view);
std::wstring_view strip(std::wstring_view);


// Split a string into tokens
std::vector<std::string_view> split_string_view(std::string_view str, char delimiter);
std::vector<std::wstring_view> split_string_view(std::wstring_view str, wchar_t delimiter);
std::vector<std::u32string_view> split_string_view(std::u32string_view str, char32_t delimiter);


// Join tokens into a string
template <typename InputIterator, typename JoinT>
std::basic_string<std::remove_cv_t<std::remove_reference_t<decltype((*std::declval<InputIterator>())[0])>>>
join(InputIterator first, InputIterator last, const JoinT &joiner) {
	std::basic_string<std::remove_cv_t<std::remove_reference_t<decltype((*std::declval<InputIterator>())[0])>>> ret;
	if (first != last) {
		ret = *first;
		while (++first != last) {
			ret += joiner;
			ret += *first;
		}
	}
	return ret;
}

inline bool starts_with(std::string_view haystack, std::string_view needle) {
	return haystack.length() >= needle.length() && haystack.substr(0, needle.length()) == needle;
}

inline bool starts_with(std::wstring_view haystack, std::wstring_view needle) {
	return haystack.length() >= needle.length() && haystack.substr(0, needle.length()) == needle;
}


} // namespace tiary

#endif // include guard
