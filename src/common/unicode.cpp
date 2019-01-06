// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2018, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


/**
 * @file	common/unicode.cpp
 * @author	chys <admin@chys.info>
 * @brief	Implements common/unicode.h
 */
#include "common/unicode.h"
#include "common/algorithm.h"
#include <memory>
#include <wchar.h>
#include <wctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

namespace tiary {

namespace {

/**
 * @brief	Defines an interval of Unicode characters
 *
 * The defined interval is from @c first, inclusive, to @c lastnext, exclusive,
 * i.e. <code>[first,lastnext)</code>
 */
struct WcharInterval
{
	unsigned first;
	unsigned lastnext;
};

/**
 * @brief	Determines whether a character is in any of a given set of intervals
 * @param	ch	Input character
 * @param	lo,hi	Defines the set of intervals of Unicode characters
 * @result	@c true if @c ch is in any of the intervals in @c [lo,hi). \n
 *			@c false otherwise.
 *
 * We use binary search, so the input intervals must have been sorted
 * in ascending order.
 */
bool find (const WcharInterval *lo, const WcharInterval *hi, unsigned ch)
{
	while (lo < hi) {
		const WcharInterval *mid = lo + size_t(hi-lo)/2;
		if (ch < mid->first) {
			hi = mid;
		}
		else if (ch >= mid->lastnext) {
			lo = mid + 1;
		}
		else {
			return true;
		}
	}
	return false;
}

} // Anonymous namespace

int utf8_len_by_wchar (wchar_t w)
{
	unsigned u = w;
	if (u < 0x800) {
		if (u < 0x80) {
			return 1;
		}
		else {
			return 2;
		}
	}
	else if (u < 0x10000) {
		return 3;
	}
	else if (u < 0x110000) {
		return 4;
	}
	else {
		return 0;
	}
}

int utf8_len_by_first_byte (unsigned char b)
{
	/*
	 * 1: 0xxx xxxx
	 * 0: 10xx xxxx (Error)
	 * 2: 110x xxxx
	 * 3: 1110 xxxx
	 * 4: 1111 0xxx
	 * 0: 1111 1xxx (Error)
	 */
	static const unsigned char ret[32] = {
		1, 1, 1, 1, 1, 1, 1, 1, // 00xx x
		1, 1, 1, 1, 1, 1, 1, 1, // 01xx x
		0, 0, 0, 0, 0, 0, 0, 0, // 10xx x
		2, 2, 2, 2, // 110x x
		3, 3, // 1110 x
		4, // 1111 0
		0 // 1111 1
	};
	return ret[b >> 3];
}

wchar_t utf8_to_wchar (const char *p, const char **end)
{
	uint32_t u;
	unsigned b;
	int len;

	u = (uint8_t)*p++;
	len = utf8_len_by_first_byte (u);
	u &= 0xffu >> len;
	switch (len) {
	default: /* case 4: */
		if (((b = (uint8_t)*p++) & 0xc0) != 0x80) {
			break;
		}
		u = (u<<6) | (b & 0x3f);
	case 3:
		if (((b = (uint8_t)*p++) & 0xc0) != 0x80) {
			break;
		}
		u = (u<<6) | (b & 0x3f);
	case 2:
		if (((b = (uint8_t)*p++) & 0xc0) != 0x80) {
			break;
		}
		u = (u<<6) | (b & 0x3f);
	case 1:
		if (end) {
			*end = p;
		}
		return u;
	case 0:
		if (end) {
			*end = p;
		}
		return L'\0';
	}
	if (end) {
		*end = p - 1;
	}
	return L'\0';
}

wchar_t utf8_to_wchar (char *src, char **end)
{
	return utf8_to_wchar (src, (const char **)end);
}

namespace {

/**
 * This function requries BOTH the string to be null-terminated
 * and the length to be given
 */
std::wstring utf8_to_wstring_impl (const char *s, size_t l, wchar_t substitute)
{
	std::wstring r (l, L'\0');
	std::wstring::iterator iw = r.begin ();
	while (*s) {
		const char *next;
		wchar_t c = utf8_to_wchar (s, &next);
		if (c == 0) {
			c = substitute;
		}
		if (c) {
			*iw++ = c;
		}
		s = next;
	}
	r.erase (iw, r.end ());
	return r;
}

} // anonymous namespace

std::wstring utf8_to_wstring (const char *s, wchar_t substitute)
{
	return utf8_to_wstring_impl (s, strlen (s), substitute);
}

std::wstring utf8_to_wstring (const std::string &s, wchar_t substitute)
{
	return utf8_to_wstring_impl (s.c_str() /* Not to be replaced by data () */,
			s.length (), substitute);
}

size_t utf8_count_chars(std::string_view str) {
	const char *s = str.data();
	const char *e = s + str.length();
	size_t ret = 0;
	while (s < e) {
		int n = utf8_len_by_first_byte(*s++);
		if (n > 1)
			s += n - 1;
		++ret;
	}
	return ret;
}

char *wchar_to_utf8 (char *dst, wchar_t w)
{
	uint32_t u = w;
	if (u < 0x800) {
		if (u < 0x80) {
			*dst++ = u;
		}
		else {
			*dst++ = (u >> 6) | 0xc0u;
			*dst++ = (u & 0x3fu) | 0x80u;
		}
	}
	else if (u < 0x10000) {
		*dst++ = ((u >> 12) | 0xe0u);
		*dst++ = (((u >> 6) & 0x3fu) | 0x80u);
		*dst++ = ((u & 0x3fu) | 0x80u);
	}
	else if (u < 0x110000) {
		*dst++ = ((u >> 18) | 0xf0u);
		*dst++ = (((u >> 12) & 0x3fu) | 0x80u);
		*dst++ = (((u >> 6) & 0x3fu) | 0x80u);
		*dst++ = ((u & 0x3fu) | 0x80u);
	}
	return dst;
}

char *wchar_to_utf8 (char *d, const wchar_t *s)
{
	for (; *s; ++s) {
		d = wchar_to_utf8 (d, *s);
	}
	*d = 0;
	return d;
}

char *wchar_to_utf8 (char *d, const wchar_t *s, size_t n)
{
	for (; n; --n) {
		d = wchar_to_utf8 (d, *s++);
	}
	return d;
}

std::string wstring_to_utf8 (const std::wstring &src)
{
	std::string dst;
	dst.reserve (src.length () * 2);
	for (const wchar_t *s = src.c_str(); *s; ++s) {
		char buf[4];
		char *end = wchar_to_utf8 (buf, *s);
		dst.append (buf, end);
	}
	return dst;
}

std::wstring mbs_to_wstring (const char *src)
{
	// mbstowcs always starts at initial shift state,
	// needless to use mbsrtowcs here
	size_t len = strlen (src);
	std::unique_ptr<wchar_t[]> buffer{new wchar_t[len+1]};
	if (mbstowcs(buffer.get(), src, len+1) == size_t(-1)) {
		buffer[0] = L'\0';
	}
	std::wstring ret = buffer.get();
	return ret;
}

std::wstring mbs_to_wstring (const char *src, size_t len)
{
	return mbs_to_wstring (std::string (src, len));
}

std::wstring mbs_to_wstring (const std::string &src)
{
	size_t len = src.size ();
	std::unique_ptr<wchar_t[]> buffer{new wchar_t[len+1]};
	if (mbstowcs(buffer.get(), src.c_str (), len+1) == size_t(-1)) {
		buffer[0] = L'\0';
	}
	std::wstring ret = buffer.get();
	return ret;
}


std::string wstring_to_mbs (const wchar_t *src, size_t srclen, char substitute)
{
	std::string ret;
	mbstate_t state;
	memset (&state, 0, sizeof state);
	for ( ; srclen; --srclen) {
		char buffer[16];
		size_t convret = wcrtomb (buffer, *src++, &state);
		if (convret == size_t(-1)) {
			memset (&state, 0, sizeof state);
			if (substitute == '\0') {
				continue;
			}
			buffer[0] = substitute;
			convret = 1;
		}
		ret.append (buffer, convret);
	}
	return ret;
}

std::string wstring_to_mbs (const wchar_t *src, char substitute)
{
	return wstring_to_mbs (src, wcslen (src), substitute);
}

std::string wstring_to_mbs (const std::wstring &src, char substitute)
{
	return wstring_to_mbs (src.data (), src.length (), substitute);
}

/**
 * This function is implemented with </code>wcwidth</code>,
 * But be aware of the return value for abnormal and nonprintable characters.
 */
unsigned ucs_width (wchar_t c)
{
	if (uint32_t (c) < 0x80) {
		return 1;
	}
	if (wcwidth (c) > 1) {
		return 2;
	}
	else {
		return 1;
	}
}

unsigned ucs_width (const wchar_t *s)
{
	unsigned w = 0;
	while (*s) {
		w += ucs_width (*s++);
	}
	return w;
}

unsigned ucs_width (const wchar_t *s, size_t n)
{
	unsigned w = 0;
	for (; n; --n) {
		w += ucs_width (*s++);
	}
	return w;
}

unsigned ucs_width(std::wstring_view s) {
	return ucs_width (s.data(), s.length());
}

size_t max_chars_in_width(std::wstring_view s, unsigned scrwid) {
	return max_chars_in_width (s.data (), s.length (), scrwid);
}

size_t max_chars_in_width (const wchar_t *s, unsigned scrwid)
{
	const wchar_t *p = s;
	while (wchar_t c = *p) {
		unsigned w = ucs_width (c);
		if (int (scrwid -= w) <= 0) {
			if (scrwid == 0) {
				++p;
			}
			break;
		}
		++p;
	}
	return (p - s);
}

size_t max_chars_in_width (const wchar_t *s, size_t len, unsigned scrwid)
{
	const wchar_t *p = s;
	for (; len; --len) {
		unsigned w = ucs_width (*p);
		if (int (scrwid -= w) <= 0) {
			if (scrwid == 0) {
				++p;
			}
			break;
		}
		++p;
	}
	return (p - s);
}

wchar_t ucs_reverse_case (wchar_t c)
{
	wchar_t d;
	if ((d = towlower (c)) != c) {
		return d;
	}
	else if ((d = towupper (c)) != c) {
		return d;
	}
	else {
		return c;
	}
}

namespace {
const WcharInterval alnum_table[] = {
	{ L'0', L'9'+1 }, // Numerals
	{ L'A', L'Z'+1 }, { L'a', L'z'+1 }, // Basic latin
	{ 0x00c0, 0x00d7 }, { 0x00d8, 0x00f7 }, { 0x00f8, /*0x0100 }, // Latin-1 supplement
	{ 0x0100,*/ /* 0x0180 }, // Latin Extended-A
	{ 0x0180,*/ /* 0x0250 }, // Latin Extended-B
	{ 0x0250,*/ 0x02AF }, // IPA
	{ 0x02B0, 0x02ff }, // Spacing modifier letters
	{ 0x0385, /*0x0400 }, // Greek
	{ 0x0400,*/ /* 0x0500 }, // Cyrillic
	{ 0x0500,*/ 0x0514 }, // Cyrillic supplement
	{ 0x0531, 0x0559 }, { 0x0561, 0x0589 }, // Armenian
	{ 0x05d0, 0x05f0 }, // Hebrew
	{ 0x0621, 0x0656 }, { 0x066e, 0x06d4 }, { 0x06fa, 0x06fd}, { 0x06ff, 0x0700 }, // Arabic/Urdu/Farsi
	{ 0x0710, 0x07e0 }, { 0x074d, /*0x0750 }, // Syriac
	{ 0x0750,*/ 0x076e }, // Arabic supplement
	{ 0x0780, 0x07a6 }, // Thaana
	{ 0x07ca, 0x07f4 }, // N'Ko
	{ 0x2c60, 0x2c78 } // Latin Extended-C
};
} // Anonymous namespace

bool ucs_isalpha (wchar_t c)
{
	return find (alnum_table+1, array_end (alnum_table), c);
}

bool ucs_isalnum (wchar_t c)
{
	return find (alnum_table, array_end (alnum_table), c);
}

bool ucs_iscjk(wchar_t c) {
	uint32_t ch = c;
	// Quickly return false for ASCII
	if (ch < 0x3400)
		return false;
	// CJK Unified Ideographs
	if (ch >= 0x4E00 && ch <= 0x9fff)
		return true;
	// CJK Unified Ideographs Extension A (Unicode 3.0, 1999)
	if (ch >= 0x3400 && ch <= 0x4dbf)
		return true;
	// CJK Unified Ideographs Extension B (Unicode 3.1, 2001): 20000–2A6DF
	if (ch >= 0x20000 && ch <= 0x2A6DF)
		return true;
	// CJK Unified Ideographs Extension C (Unicode 5.2, 2009): 2A700–2B73F
	if (ch >= 0x2A700 && ch <= 0x2B73F)
		return true;
	// CJK Unified Ideographs Extension D (Unicode 6.0, 2010): 2B740–2B81F
	if (ch >= 0x2B740 && ch <= 0x2B81F)
		return true;
	// CJK Unified Ideographs Extension E (Unicode 8.0, 2015): 2B820–2CEAF
	if (ch >= 0x2B820 && ch <= 0x2CEAF)
		return true;
	// CJK Unified Ideographs Extension F (Unicode 10.0, 2017): 2CEB0–2EBEF
	if (ch >= 0x2CEB0 && ch <= 0x2EBEF)
		return true;
	// CJK Compatibility Ideographs: F900–FAFF
	if (ch >= 0xF900 && ch <= 0xFAFF)
		return true;

	return false;
}

bool allow_line_beginning (wchar_t c)
{
	static const unsigned chars[] = {
		0x0021, 0x0029, 0x002c, 0x002e, 0x003a, 0x003b, 0x003f, 0x005d,
		0x007d, 0x00a8, 0x00b7, 0x02c7, 0x02c9, 0x2015, 0x2016, 0x2019,
		0x201d, 0x2026, 0x2236, 0x3001, 0x3002, 0x3003, 0x3005, 0x3009,
		0x300b, 0x300d, 0x300f, 0x3011, 0x3015, 0x3017, 0xff01, 0xff02,
		0xff07, 0xff09, 0xff0c, 0xff0e, 0xff1a, 0xff1b, 0xff1f, 0xff3d,
		0xff40, 0xff5c, 0xff5d, 0xff5e, 0xffe0
	};
	return !binary_search_null (chars, array_end(chars), unsigned(c));
}

bool allow_line_end (wchar_t c)
{
	static const unsigned chars[] = {
		0x0028, 0x005b, 0x007b, 0x00b7, 0x2018, 0x201c, 0x3008, 0x300a,
		0x300c, 0x300e, 0x3010, 0x3014, 0x3016, 0xff08, 0xff0e, 0xff3b,
		0xff5b, 0xffe1, 0xffe5
	};
	return !binary_search_null (chars, array_end(chars), unsigned(c));
}

} // namespace tiary
