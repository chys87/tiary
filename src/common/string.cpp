// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2010, 2018, chys <admin@CHYS.INFO>
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

#ifndef HAVE_STPCPY
namespace {

template <typename T> inline
T *stpcpy_impl (T *dst, const T *src)
{
	size_t len = strlen(src) + 1;
	return -1 + (T*) mempcpy (dst, src, len * sizeof (T));
}

} // Anonymous namespace

char *stpcpy (char *dst, const char *src)
{
	return stpcpy_impl (dst, src);
}

wchar_t *stpcpy (wchar_t *dst, const wchar_t *src)
{
	return stpcpy_impl (dst, src);
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
	while (*str && *str!=ch) {
		++str;
	}
	return str;
}

std::wstring strlower (const std::wstring &str)
{
	std::wstring result;
	result.resize (str.length ());
	std::transform (str.begin (), str.end (), result.begin (), towlower);
	return result;
}

std::vector <std::pair <size_t, size_t> >
find_all (const std::wstring &haystack, const std::wstring &needle)
{
	std::vector <std::pair <size_t, size_t> > ret;
	size_t neelen = needle.length ();
	size_t haylen = haystack.length ();
	size_t offset = 0;
	while (haylen - offset >= neelen) {
		size_t found = haystack.find (needle, offset);
		if (found == std::wstring::npos) {
			break;
		}
		ret.push_back (std::make_pair (found, neelen));
		offset = found + neelen;
	}
	return ret;
}


namespace {

template <typename T> inline
bool strip_impl (std::basic_string<T> &str, const T *trim)
{
	if (str.empty ()) {
		return false;
	}
	size_t startpos = str.find_first_not_of (trim);
	if (startpos == std::basic_string<T>::npos) {
		str.clear ();
		return true;
	}
	else {
		size_t endpos = str.find_last_not_of (trim) + 1;
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

}

bool strip (std::wstring &str)
{
	return strip_impl (str, L" \t\v\r\n");
}

bool strip (std::string &str)
{
	return strip_impl (str, " \t\v\r\n");
}


namespace {

template <typename T> inline
std::vector<std::basic_string<T>> split_string_impl(const T *str, T delimiter) {
	std::vector<std::basic_string<T>> ret_list;
	for (;;) {
		while (*str == delimiter) {
			++str;
		}
		if (*str == T ()) {
			break;
		}
		const T *del = strchrnul (str, delimiter);
		ret_list.emplace_back(str, del);
		if (*del != delimiter) {
			break;
		}
		str = del + 1;
	}
	return ret_list;
}

} // anonymous namespace

std::vector<std::string> split_string(const char *str, char delimiter) {
	return split_string_impl<char> (str, delimiter);
}
std::vector<std::wstring> split_string(const wchar_t *str, wchar_t delimiter) {
	return split_string_impl (str, delimiter);
}


} // namespace tiary
