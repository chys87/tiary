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


#include "ui/terminal_emulator.h"
#include "common/string.h"
#include "common/unicode.h"

namespace tiary {
namespace ui {


namespace {

bool is_locale_utf8 ()
{
	// Traditional Chinese "I love you" in UTF-8 and UCS-4
	return (wstring_to_mbs(L"\u6211\u611b\u4f60"sv)
			== "\xe6\x88\x91\xe6\x84\x9b\xe4\xbd\xa0"sv);
}

} // anonymous namespace


bool terminal_emulator_correct_wcwidth ()
{
	// In non-Unicode locales there are more problems
	static bool res = is_locale_utf8();
	return res;
}


} // namespace ui
} // namespace tiary
