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
	EXPECT_EQ(L"abc", Format(L"abc") << Format::Result());
	EXPECT_EQ(L"abc", Format(L"abc") << L"x" << Format::Result());
	EXPECT_EQ(L"abcx", Format(L"abc%a") << L"x" << Format::Result());
	EXPECT_EQ(L"abc65536xyz", Format(L"abc%axyz") << 65536u << Format::Result());
	EXPECT_EQ(L"abc10000xyz", Format(L"abc%axyz") << hex(65536u) << Format::Result());
	EXPECT_EQ(L"abc10000xyz!10000", Format(L"abc%axyz%b%a") << hex(65536u) << L'!' << L"www" << Format::Result());
	EXPECT_EQ(L"The rate is 100%", Format(L"The rate is %a%%") << 100u << Format::Result());
}

TEST(Format, Align) {
	EXPECT_EQ(L" 1", Format(L"%2a") << 1u << Format::Result());
	EXPECT_EQ(L"01", Format(L"%02a") << 1u << Format::Result());
	EXPECT_EQ(L"1 ", Format(L"%-2a") << 1u << Format::Result());
	EXPECT_EQ(L"1 ", Format(L"%-02a") << 1u << Format::Result());
	EXPECT_EQ(L"100", Format(L"%2a") << 100u << Format::Result());
}

TEST(Format, ConvenienceFunction) {
	EXPECT_EQ(L"Hello world", format(L"%a %b", L"Hello", L"world"));
}

} // namespace tiary
