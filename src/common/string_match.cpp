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


#include "common/string_match.h"
#include "common/string.h"


namespace tiary {


StringMatch::StringMatch ()
	: pattern_()
#ifdef TIARY_USE_RE2
	, regex_()
#endif
{
}

StringMatch::StringMatch(const std::wstring &pattern, bool use_regex)
	: pattern_(pattern)
#ifdef TIARY_USE_RE2
	, regex_(use_regex ? new Re(pattern) : nullptr)
#endif
{
	if (pattern_.empty()) {
#ifdef TIARY_USE_RE2
		regex_.reset();
#endif
		return;
	}
#ifdef TIARY_USE_RE2
	if (regex_ && !*regex_) {
		pattern_.clear();
		regex_.reset();
	}
#endif
}

StringMatch::~StringMatch ()
{
}

std::vector <std::pair <size_t, size_t> > StringMatch::match (const std::wstring &haystack) const
{
#ifdef TIARY_USE_RE2
	if (regex_) {
		return regex_->match (haystack);
	}
	else
#endif
	{
		return find_all(strlower(haystack), strlower(pattern_));
	}
}

bool StringMatch::basic_match (const std::wstring &haystack) const
{
#ifdef TIARY_USE_RE2
	if (regex_) {
		return regex_->basic_match(haystack);
	}
	else
#endif
	{
		return (strlower(haystack).find(strlower(pattern_)) != std::wstring::npos);
	}
}


} // namespace tiary
