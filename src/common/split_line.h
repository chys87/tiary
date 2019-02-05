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

#ifndef TIARY_COMMON_SPLIT_LINE_H
#define TIARY_COMMON_SPLIT_LINE_H

#include <wchar.h>
#include <vector>
#include <string_view>

namespace tiary {

struct SplitStringLine
{
	size_t begin; // Characters in [begin,begin+len) should be on this line. (Newline characters are excluded)
	size_t len;
	unsigned wid; // Width on screen
};

typedef std::vector<SplitStringLine> SplitStringLineList;

// Treat newline characters as if they were spaces
const unsigned SPLIT_NEWLINE_AS_SPACE = 1;
// Allow splitting in the middle of a word
const unsigned SPLIT_CUT_WORD = 2;

// Only one line. Returns the offset of the starting point of the next line
size_t split_line(SplitStringLine *, unsigned wid, std::wstring_view s, size_t offset = 0, unsigned options = 0);

// Limited number of lines. Returns the actual number of lines
unsigned split_line(SplitStringLine [], unsigned max_lines, unsigned wid, std::wstring_view);
// Unlimited number of lines. Returns a vector
SplitStringLineList split_line (unsigned wid, std::wstring_view);

} // namespace tiary

#endif // Include guard
