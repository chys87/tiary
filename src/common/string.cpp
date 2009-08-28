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


#include "common/string.h"
#include <algorithm>
#include <wctype.h>

namespace tiary {

#ifndef TIARY_HAVE_MEMPCPY
void *mempcpy (void *dst, const void *src, size_t len)
{
	return len + (char *) memcpy (dst, src, len);
}
#endif

#ifndef TIARY_HAVE_STPCPY
namespace {

template <typename T> inline
T *stpcpy_impl (T *dst, const T *src)
{
	size_t len = strlen (src);
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

#ifndef TIARY_HAVE_STRCHRNUL
const char *strchrnul (const char *str, int ch)
{
	size_t len = strlen (str);
	const char *ptr = (const char *)memchr (str, len, ch);
	if (ptr == NULL)
		ptr = str + len;
	return ptr;
}
#endif

const wchar_t *strchrnul (const wchar_t *str, wchar_t ch)
{
	while (*str && *str!=ch)
		++str;
	return str;
}

void strlower (std::wstring &str)
{
	std::transform (str.begin (), str.end (), str.begin (), towlower);
}

size_t find_caseless (const std::wstring &haystack, const std::wstring &needle)
{
	if (haystack.length () < needle.length ())
		return std::wstring::npos;
	std::wstring hay (haystack);
	std::wstring nee (needle);
	strlower (hay);
	strlower (nee);
	return hay.find (nee);
}


namespace {

template <typename T> inline
void strip (std::basic_string<T> &str, const T *trim)
{
	size_t startpos = str.find_first_not_of (trim);
	if (startpos == std::basic_string<T>::npos) {
		str.clear ();
	} else {
		size_t endpos = str.find_last_not_of (trim);
		// If any implementation of string does not handle
		// the following alias, it should be thrown away
		str.assign (str, startpos, endpos-startpos+1);
	}
}

}

void strip (std::wstring &str)
{
	strip (str, L" \t\v\r\n");
}

void strip (std::string &str)
{
	strip (str, " \t\v\r\n");
}


namespace {

template <typename T> inline
std::list <std::basic_string <T> > split_string_impl (const T *str, T delimiter, bool remove_empties)
{
	std::basic_string <T> empty_string;
	std::list <std::basic_string <T> > ret_list;
	for (;;) {
		if (remove_empties) {
			while (*str == delimiter)
				++str;
		}
		if (*str == T ())
			break;
		const T *del = strchrnul (str, delimiter);
		ret_list.push_back (empty_string);
		ret_list.back ().assign (str, del);
		if (*del != delimiter)
			break;
		str = del + 1;
	}
	return ret_list;
}

} // anonymous namespace

std::list <std::string> split_string (const char *str, int delimiter, bool remove_empties)
{
	return split_string_impl<char> (str, delimiter, remove_empties);
}
std::list <std::wstring> split_string (const wchar_t *str, wchar_t delimiter, bool remove_empties)
{
	return split_string_impl (str, delimiter, remove_empties);
}


} // namespace tiary
