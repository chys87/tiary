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
	, backward_(false)
{
}

SearchInfo::~SearchInfo ()
{
}

bool SearchInfo::dialog(bool default_backward) {
	SearchDesc new_search;

	ui::dialog_search(&new_search,
			{matcher_.get_pattern(), default_backward, matcher_.get_use_regex()});
	if (new_search.text.empty ()) {
		return false;
	}

	if (StringMatch matcher = StringMatch(new_search.text, new_search.regex)) {
		matcher_ = std::move(matcher);
		backward_ = new_search.backward;
		return true;
	}
	else {
		dialog_message (L"Invalid regular expression", L"Error");
		return false;
	}
}


} // namespace tiary::ui
} // namespace tiary
