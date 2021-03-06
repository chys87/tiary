// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2018, 2019, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#ifndef TIARY_COMMON_STRING_MATCH_H
#define TIARY_COMMON_STRING_MATCH_H

#include "common/re.h"
#include <memory>
#include <string_view>
#include <vector>

namespace tiary {

class StringMatch
{
public:
	StringMatch ();
	StringMatch(StringMatch &&) = default;
	explicit StringMatch(std::wstring_view, bool use_regex = false);
	StringMatch &operator = (StringMatch &&) = default;
	~StringMatch ();

	/**
	 * @brief	Match against a string, returning detailed results
	 *
	 * @result	Every match corresponds to an item in the vector, the first
	 * of which is the offset and the second the length of the matched
	 * substring.
	 */
	std::vector<std::pair<size_t, size_t>> match(std::wstring_view) const;

	/**
	 * @brief	Match against a string, returning only true/false
	 */
	bool basic_match(std::wstring_view) const;


	/**
	 * @brief	Whether this class contains valid search info
	 */
	explicit operator bool() const { return !pattern_.empty(); }

	const std::wstring &get_pattern() const { return pattern_; }
	bool get_use_regex () const
	{
#ifdef TIARY_USE_RE2
		return static_cast<bool>(regex_);
#else
		return false;
#endif
	}

private:
	std::wstring pattern_;
#ifdef TIARY_USE_RE2
	std::unique_ptr<Re> regex_; ///< Re object related to search_text, if it is a regular expression
#endif

};

} // namespace tiary


#endif // include guard
