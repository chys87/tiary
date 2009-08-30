// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, chys <admin@CHYS.INFO>
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
	: text ()
	, backward (false)
#ifdef TIARY_USE_PCRE
	, regex (0)
#endif
{
}

SearchInfo::~SearchInfo ()
{
}

bool SearchInfo::dialog (bool default_backward)
{
	std::wstring new_text;
	bool new_backward;
	bool new_regex;

	ui::dialog_search (new_text, new_backward, new_regex, text, default_backward,
#ifdef TIARY_USE_PCRE
			bool (regex.get ())
#else
			false
#endif
			);
	if (new_text.empty ())
		return false;
#ifdef TIARY_USE_PCRE
	if (new_regex) {
		PcRe *new_pcre = new PcRe (new_text);
		if (!*new_pcre) {
			// Invalid regular expression
			ui::dialog_message (L"Invalid regular expression", L"Search");
			delete new_pcre;
			return false;
		}
		regex.reset (new_pcre);
	}
#endif
	text = new_text;
	backward = new_backward;
	return true;
}

SearchInfo::operator int BooleanConvert::* () const
{
	return (!text.empty () ? &BooleanConvert::valid : 0);
}

std::vector <std::pair <size_t, size_t> > SearchInfo::match (const std::wstring &haystack) const
{
#ifdef TIARY_USE_PCRE
	if (PcRe *rex = regex.get ())
		return rex->match (haystack);
	else
#endif
		return find_all (strlower (haystack), strlower (text));
}

bool SearchInfo::basic_match (const std::wstring &haystack) const
{
#ifdef TIARY_USE_PCRE
	if (PcRe *rex = regex.get ())
		return rex->basic_match (haystack);
	else
#endif
		return (strlower (haystack).find (strlower (text)) != std::wstring::npos);
}


} // namespace tiary::ui
} // namespace tiary
