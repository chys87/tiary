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



#include "ui/search_info.h"
#include "ui/dialog_search.h"
#include "ui/dialog_message.h"
#include "common/string.h"

namespace tiary {
namespace ui {

SearchInfo::SearchInfo ()
	: matcher_()
	, backward (false)
{
}

SearchInfo::~SearchInfo ()
{
}

bool SearchInfo::dialog (bool default_backward)
{
	std::wstring new_pattern;
	bool new_backward;
	bool new_regex;

	ui::dialog_search (new_pattern, new_backward, new_regex,
			matcher_.get_pattern(), default_backward, matcher_.get_use_regex());
	if (new_pattern.empty ()) {
		return false;
	}

	if (StringMatch matcher = StringMatch(new_pattern, new_regex)) {
		matcher_ = std::move(matcher);
		backward = new_backward;
		return true;
	}
	else {
		dialog_message (L"Invalid regular expression", L"Error");
		return false;
	}
}


} // namespace tiary::ui
} // namespace tiary
