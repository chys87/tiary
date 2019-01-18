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
#include "common/datetime.h"

namespace tiary {

TEST(datetime, days_of_month) {
	EXPECT_EQ(0, days_of_month(2019, 0));
	EXPECT_EQ(31, days_of_month(2019, 1));
	EXPECT_EQ(28, days_of_month(2019, 2));
	EXPECT_EQ(29, days_of_month(2020, 2));
	EXPECT_EQ(28, days_of_month(2100, 2));
	EXPECT_EQ(29, days_of_month(2400, 2));
	EXPECT_EQ(31, days_of_month(2019, 3));
}

TEST(datetime, make_date) {
	EXPECT_EQ(1, make_date({1, 1, 1}));
	EXPECT_EQ(kDateAtUnixEpoch, make_date(kUnixEpoch));
}

TEST(datetime, extract_date) {
	EXPECT_EQ(kUnixEpoch.y, extract_date(kDateAtUnixEpoch).y);
	EXPECT_EQ(kUnixEpoch.m, extract_date(kDateAtUnixEpoch).m);
	EXPECT_EQ(kUnixEpoch.d, extract_date(kDateAtUnixEpoch).d);
	EXPECT_EQ(kUnixEpoch.w, extract_date(kDateAtUnixEpoch).w);
}

TEST(datetime, make_datetime_utc) {
	EXPECT_EQ(make_datetime(kDateAtUnixEpoch, 0), make_datetime_utc(0));
}

} // namespace tiary
