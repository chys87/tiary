// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2010, 2018, 2019, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#include "common/string.h"
#include <algorithm>
#include <wctype.h>

namespace tiary {

#ifndef HAVE_MEMPCPY
void *mempcpy (void *dst, const void *src, size_t len)
{
	return len + (char *) memcpy (dst, src, len);
}
#endif

#ifndef HAVE_STRCHRNUL
const char *strchrnul (const char *str, int ch)
{
	size_t len = strlen (str);
	const char *ptr = (const char *)memchr (str, ch, len);
	if (ptr == 0) {
		ptr = str + len;
	}
	return ptr;
}
#endif

const wchar_t *strchrnul (const wchar_t *str, wchar_t ch)
{
#ifdef HAVE_WCSCHRNUL
	return wcschrnul(str, ch);
#else
	while (*str && *str!=ch) {
		++str;
	}
	return str;
#endif
}

std::wstring strlower(std::wstring_view str) {
	std::wstring result;
	result.resize (str.length ());
	std::transform (str.begin (), str.end (), result.begin (), towlower);
	return result;
}

std::vector <std::pair <size_t, size_t> >
find_all(std::wstring_view haystack, std::wstring_view needle)
{
	std::vector <std::pair <size_t, size_t> > ret;
	size_t neelen = needle.length ();
	size_t haylen = haystack.length ();
	size_t offset = 0;
	while (haylen - offset >= neelen) {
		size_t found = haystack.find (needle, offset);
		if (found == std::wstring_view::npos) {
			break;
		}
		ret.push_back (std::make_pair (found, neelen));
		offset = found + neelen;
	}
	return ret;
}


namespace {

template <typename T> inline
bool strip_in_place_impl(std::basic_string<T> &str, std::basic_string_view<T> trim) {
	if (str.empty ()) {
		return false;
	}
	size_t startpos = str.find_first_not_of(trim.data(), 0, trim.length());
	if (startpos == std::basic_string<T>::npos) {
		str.clear ();
		return true;
	}
	else {
		size_t endpos = str.find_last_not_of(trim.data(), str.npos, trim.length()) + 1;
		if (startpos == 0) {
			if (endpos >= str.size ()) {
				return false;
			}
			else {
				str.resize (endpos);
				return true;
			}
		}
		else {
			// If any implementation of string does not handle
			// the following alias, it should be thrown away
			str.assign (str, startpos, endpos-startpos);
			return true;
		}
	}
}

template <typename T>
inline std::basic_string<T> strip_impl(std::basic_string_view<T> str, std::basic_string_view<T> trim) {
	size_t start = str.find_first_not_of(trim);
	if (start == str.npos) {
		return {};
	} else {
		size_t end = str.find_last_not_of(trim);
		return std::basic_string<T>(str.substr(start, end - start + 1));
	}
}

} // namespace

bool strip_in_place(std::string &str) {
	return strip_in_place_impl(str, " \t\v\r\n"sv);
}

bool strip_in_place(std::wstring &str) {
	return strip_in_place_impl(str, L" \t\v\r\n"sv);
}

std::string strip(std::string_view str) {
	return strip_impl(str, " \t\v\r\n"sv);
}

std::wstring strip(std::wstring_view str) {
	return strip_impl(str, L" \t\v\r\n"sv);
}

namespace {

template <typename T> inline
std::vector<std::basic_string_view<T>> split_string_view_impl(std::basic_string_view<T> str, T delimiter) {
	std::vector<std::basic_string_view<T>> ret_list;
	size_t pos = 0;
	while (pos < str.length()) {
		while (pos < str.length() && str[pos] == delimiter) {
			++pos;
		}
		if (pos >= str.length()) {
			break;
		}
		size_t end_pos = str.find(delimiter, pos);
		ret_list.push_back(str.substr(pos, end_pos - pos));
		pos = end_pos;
	}
	return ret_list;
}

} // anonymous namespace

std::vector<std::string_view> split_string_view(std::string_view str, char delimiter) {
	return split_string_view_impl(str, delimiter);
}

std::vector<std::wstring_view> split_string_view(std::wstring_view str, wchar_t delimiter) {
	return split_string_view_impl(str, delimiter);
}

std::vector<std::u32string_view> split_string_view(std::u32string_view str, char32_t delimiter) {
	return split_string_view_impl(str, delimiter);
}

} // namespace tiary
