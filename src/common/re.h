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


#ifndef TIARY_COMMON_RE_H
#define TIARY_COMMON_RE_H

#ifdef TIARY_USE_RE2

#include <string_view>
#include <vector>
#include <re2/re2.h>

namespace tiary {

class Re {
public:
	// Construct an invalid regular expression
	explicit Re(std::wstring_view);
	Re(const Re &) = delete;
	void operator = (const Re &) = delete;

	explicit operator bool() const { return re_.ok(); }

	/**
	 * @brief	Match the pattern against a string
	 * @result	For each pair, @c first is the offset, @c second is the length
	 */
	std::vector<std::pair<size_t, size_t>> match(std::wstring_view) const;

	/**
	 * @brief	Match the pattern against a string
	 * @result	@c true if there is any match; @c false otherwise
	 */
	bool basic_match(std::wstring_view) const;

private:
	std::string utf8_re_string_;
	RE2 re_;
};

} // namespace tiary

#endif // TIARY_USE_RE2

#endif // include guard
