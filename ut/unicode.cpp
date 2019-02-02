// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2019, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/

#include <gtest/gtest.h>
#include "common/unicode.h"

namespace tiary {

TEST(UTF8, ToWstring) {
	EXPECT_EQ(L"\uabcd?\uaaaa", utf8_to_wstring((const char *)u8"\uabcd\xff\uaaaa"));
	EXPECT_EQ(L"\uabcd\uaaaa", utf8_to_wstring((const char *)u8"\uabcd\xff\uaaaa", L'\0'));
}

TEST(UTF8, OverLong) {
	EXPECT_EQ(L"?", utf8_to_wstring("\xf0\x82\x82\xac")); // Over-long encoding for €
}

TEST(UTF8, CountChars) {
	EXPECT_EQ(6, utf8_count_chars((const char *)u8"A\u0080\u0800\u8000\U00010000\U0010ABCD"));
}

} // namespace tiary
