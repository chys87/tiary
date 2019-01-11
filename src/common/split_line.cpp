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

#include "common/split_line.h"
#include "common/unicode.h"

namespace tiary {

size_t split_line (SplitStringLine &result, unsigned wid, const wchar_t *s, size_t slen, size_t offset, unsigned options)
{
	s += offset;
	if (slen == size_t (-1)) {
		slen = wcslen (s);
	}
	else {
		slen -= offset;
	}

	size_t cur = 0;     // Current position
	unsigned curwid = 0;// Used screen width

	for (;;) {
		if (cur >= slen) {
			result.begin = offset;
			result.len = slen;
			result.wid = curwid;
			return (offset + slen);
		}
		if (!(options & SPLIT_NEWLINE_AS_SPACE) && s[cur] == L'\n') {
			result.begin = offset;
			result.len = cur;
			result.wid = curwid;
			return (offset + cur + 1); // Skip the newline character
		}
		unsigned w = ucs_width (s[cur]);
		if (curwid + w > wid) {
			break;
		}
		curwid += w;
		++cur;
	}
	// Not the whold string. Neither has a newline character been encountered
	// Now scan backward to find a proper line-breaking point
	unsigned extra_skip = 0;
	if (!(options & SPLIT_CUT_WORD)) {
		unsigned xcur = cur;
		for (;;) {
			if (xcur == 0) { // This means that a single word is longer than a line. We have to split it
				break;
			}
			if ((!ucs_isalnum (s[xcur-1]) || !ucs_isalnum (s[xcur])) &&
					allow_line_end (s[xcur-1]) &&
					allow_line_beginning (s[xcur])) {
				cur = xcur;
				break;
			}
			curwid -= ucs_width (s[--xcur]);
		}
		while (cur+extra_skip+1<slen && s[cur+extra_skip]==L' ') {
			++extra_skip;
		}
	}
	result.begin = offset;
	result.len = cur;
	result.wid = curwid;
	return (offset + cur + extra_skip);
}

size_t split_line(SplitStringLine &result, unsigned wid, std::wstring_view str, size_t offset, unsigned options) {
	return split_line (result, wid, str.data (), str.length (), offset, options);
}

unsigned split_line (SplitStringLine *result, unsigned max_lines, unsigned wid, const wchar_t *s, size_t slen)
{
	unsigned offset = 0;
	unsigned lines;
	for (lines = 0; lines<max_lines && offset<slen; ++lines) {
		offset = split_line (*result++, wid, s, slen, offset, 0);
	}
	return lines;
}

unsigned split_line(SplitStringLine *result, unsigned max_lines, unsigned wid, std::wstring_view s) {
	return split_line (result, max_lines, wid, s.data (), s.length ());
}

SplitStringLineList split_line (unsigned wid, const wchar_t *s, size_t slen)
{
	SplitStringLineList ret (slen);
	if (wid < 2) { // Robustness. Avoid dead loops
		for (size_t k=0; k<slen; ++k) {
			ret[k].begin = k;
			ret[k].len = 1;
			ret[k].wid = ucs_width (s[k]);
		}
	}
	else {
		SplitStringLineList::iterator it = ret.begin ();
		for (unsigned offset = 0; offset < slen; ) {
			offset = split_line (*it, wid, s, slen, offset, 0);
			++it;
		}
		ret.erase (it, ret.end ());
	}
	return ret;
}

SplitStringLineList split_line(unsigned wid, std::wstring_view s) {
	return split_line (wid, s.data (), s.length ());
}

} // namespace tiary
