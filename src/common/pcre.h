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


#ifndef TIARY_COMMON_PCRE_H
#define TIARY_COMMON_PCRE_H

#ifdef TIARY_USE_PCRE

#include <string>
#include <vector>

namespace tiary {

class PcRe {
public:
	// Construct an invalid regular expression
	PcRe (const std::wstring &);
	~PcRe ();

	operator void * () const { return re; }

	/**
	 * @brief	Match the pattern against a string
	 * @result	For each pair, @c first is the offset, @c second is the length
	 */
	std::vector<std::pair <size_t, size_t> > match (const std::wstring &) const;

	/**
	 * @brief	Match the pattern against a string
	 * @result	@c true if there is any match; @c false otherwise
	 */
	bool basic_match (const std::wstring &) const;

private:
	void *re, *re_ex;

	PcRe (const PcRe &);
	void operator = (const PcRe &);
};

} // namespace tiary

#endif // TIARY_USE_PCRE

#endif // include guard
