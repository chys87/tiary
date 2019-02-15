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
#include "common/format.h"

namespace tiary {

TEST(Format, Basic) {
	EXPECT_EQ(L"abc", Format() << Format::Result{L"abc"});
	EXPECT_EQ(L"abc", Format() << L"x" << Format::Result{L"abc"});
	EXPECT_EQ(L"abcx", Format() << L"x" << Format::Result{L"abc%a"});
	EXPECT_EQ(L"abc65536xyz", Format() << 65536u << Format::Result{L"abc%axyz"});
	EXPECT_EQ(L"abc10000xyz", Format() << hex(65536u) << Format::Result{L"abc%axyz"});
	EXPECT_EQ(L"abc10000xyz!10000", Format() << hex(65536u) << L'!' << L"www" << Format::Result{L"abc%axyz%b%a"});
	EXPECT_EQ(L"The rate is 100%", Format() << 100u << Format::Result{L"The rate is %a%%"});
}

TEST(Format, Align) {
	EXPECT_EQ(L" 1", Format() << 1u << Format::Result{L"%2a"});
	EXPECT_EQ(L"01", Format() << 1u << Format::Result{L"%02a"});
	EXPECT_EQ(L"1 ", Format() << 1u << Format::Result{L"%-2a"});
	EXPECT_EQ(L"1 ", Format() << 1u << Format::Result{L"%-02a"});
	EXPECT_EQ(L"100", Format() << 100u << Format::Result{L"%2a"});
}

TEST(Format, ConvenienceFunction) {
	EXPECT_EQ(L"Hello world", format(L"%a %b", L"Hello", L"world"));
}

} // namespace tiary
