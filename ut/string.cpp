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
#include "common/string.h"


namespace tiary {

TEST(SplitTest, Split) {
	EXPECT_EQ((std::vector<std::string_view>{"a", "b", "c"}), split_string_view("a b c"sv, ' '));
	EXPECT_EQ((std::vector<std::string_view>{"a", "bb", "ccc"}), split_string_view("  a  bb  ccc "sv, ' '));
}

} // namespace tiary
