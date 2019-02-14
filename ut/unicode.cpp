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
#include <locale.h>
#include "common/unicode.h"
#include "common/string.h"

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

TEST(UTF8, FromWstring) {
	EXPECT_EQ((const char *)u8"\uabcd\uaaaaABCD", wstring_to_utf8(L"\uabcd\uaaaaABCD"));
	EXPECT_EQ((const char *)u8"\uabcd\uaaaaABCD", wstring_to_utf8(U"\uabcd\uaaaaABCD"));
}

class Mbs : public ::testing::Test {
public:
	void SetUp() override {
		setlocale(LC_ALL, "zh_CN.UTF-8");
	}
};

TEST_F(Mbs, MbsToWstring) {
	auto s = u8"ABC\u0080\u0800\u8000\0\u8000"sv;
	EXPECT_EQ(L"ABC\u0080\u0800\u8000\0\u8000"sv, mbs_to_wstring({(const char *)s.data(), s.length()}));
}

class WcWidthTest : public ::testing::Test {
public:
	void SetUp() override {
		setlocale(LC_ALL, "zh_CN.UTF-8");
	}
};

TEST_F(WcWidthTest, ucs_width) {
	EXPECT_EQ(1, ucs_width(U'\n'));
	EXPECT_EQ(2, ucs_width(U'国'));
	EXPECT_EQ(1, ucs_width(U'α'));
}

TEST_F(WcWidthTest, ucs_width_str) {
	EXPECT_EQ(4, ucs_width(L"\n国α"));
	EXPECT_EQ(4, ucs_width(U"\n国α"));
}

TEST_F(WcWidthTest, max_chars_in_width) {
	EXPECT_EQ(0, max_chars_in_width(L"\n国α", 0));
	EXPECT_EQ(1, max_chars_in_width(L"\n国α", 1));
	EXPECT_EQ(1, max_chars_in_width(L"\n国α", 2));
	EXPECT_EQ(2, max_chars_in_width(L"\n国α", 3));
	EXPECT_EQ(3, max_chars_in_width(L"\n国α", 4));
	EXPECT_EQ(3, max_chars_in_width(L"\n国α", 5));

	EXPECT_EQ(0, max_chars_in_width(U"\n国α", 0));
	EXPECT_EQ(1, max_chars_in_width(U"\n国α", 1));
	EXPECT_EQ(1, max_chars_in_width(U"\n国α", 2));
	EXPECT_EQ(2, max_chars_in_width(U"\n国α", 3));
	EXPECT_EQ(3, max_chars_in_width(U"\n国α", 4));
	EXPECT_EQ(3, max_chars_in_width(U"\n国α", 5));
}

} // namespace tiary
