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
	: pattern ()
#ifdef TIARY_USE_RE2
	, regex ()
#endif
{
}

StringMatch::~StringMatch ()
{
}


bool StringMatch::assign (const std::wstring &pattern_, bool use_regex)
{
	if (pattern_.empty ()) {
		return false;
	}
#ifdef TIARY_USE_RE2
	Re *new_pcre = 0;
	if (use_regex) {
		new_pcre = new Re (pattern_);
		if (!*new_pcre) {
			delete new_pcre;
			return false;
		}
	}
	regex.reset (new_pcre);
#endif
	pattern = pattern_;
	return true;
}

std::vector <std::pair <size_t, size_t> > StringMatch::match (const std::wstring &haystack) const
{
#ifdef TIARY_USE_RE2
	if (Re *rex = regex.get ()) {
		return rex->match (haystack);
	}
	else
#endif
	{
		return find_all (strlower (haystack), strlower (pattern));
	}
}

bool StringMatch::basic_match (const std::wstring &haystack) const
{
#ifdef TIARY_USE_RE2
	if (Re *rex = regex.get ()) {
		return rex->basic_match (haystack);
	}
	else
#endif
	{
		return (strlower (haystack).find (strlower (pattern)) != std::wstring::npos);
	}
}


} // namespace tiary
