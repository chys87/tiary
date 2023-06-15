// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2019-2023, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/

#include <gtest/gtest.h>
#include <locale.h>
#include <list>
#include <set>
#include "common/string.h"


namespace tiary {

TEST(StrLowerTest, StrLower) {
	setlocale(LC_ALL, "zh_CN.UTF-8");
	EXPECT_EQ(strlower(L"AbCäÄÈÑÕ"sv), L"abcääèñõ"sv);
}

TEST(SplitTest, Split) {
	EXPECT_EQ((std::vector<std::string_view>{"a", "b", "c"}), split_string_view("a b c"sv, ' '));
	EXPECT_EQ((std::vector<std::string_view>{"a", "bb", "ccc"}), split_string_view("  a  bb  ccc "sv, ' '));
}

TEST(JoinTest, JoinPointers) {
	const char* strings[] {"Hello", "world"};
	EXPECT_EQ("Hello world", join(std::begin(strings), std::end(strings), " "));
}

TEST(JoinTest, ListOfStrings) {
	std::list<std::wstring> ls{L"world", L"Hello"};
	EXPECT_EQ(L"Hello world", join(ls.rbegin(), ls.rend(), L' '));
}

TEST(JoinTest, SetOfStringViews) {
	std::set<std::string_view> ss{"Hello", "world"};
	EXPECT_EQ("Hello world", join(ss.begin(), ss.end(), ' '));
}

} // namespace tiary
