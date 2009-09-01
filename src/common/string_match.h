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


#ifndef TIARY_COMMON_STRING_MATCH_H
#define TIARY_COMMON_STRING_MATCH_H

#include "common/pcre.h"
#include <string>
#include <vector>
#include <memory> // std::auto_ptr

namespace tiary {


class StringMatch
{
public:
	StringMatch ();
	~StringMatch ();

	/**
	 * @result	Returns true only if the pattern is empty,
	 * or the regular expression is invalid
	 */
	bool assign (const std::wstring &, bool use_regex = false);

	/**
	 * @brief	Match against a string, returning detailed results
	 */
	std::vector <std::pair <size_t, size_t> > match (const std::wstring &) const;

	/**
	 * @brief	Match against a string, returning only true/false
	 */
	bool basic_match (const std::wstring &) const;


	struct BooleanConvert { int valid; };
	/**
	 * @brief	Whether this class contains valid search info
	 */
	operator int BooleanConvert::* () const { return !pattern.empty () ? &BooleanConvert::valid : 0; }

	const std::wstring &get_pattern () const { return pattern; }
	bool get_use_regex () const
	{
#ifdef TIARY_USE_PCRE
		return regex.get ();
#else
		return false;
#endif
	}

private:
	std::wstring pattern;
#ifdef TIARY_USE_PCRE
	std::auto_ptr<PcRe> regex; ///< PcRe object related to search_text, if it is a regular expression
#endif

};

} // namespace tiary


#endif // include guard
