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

#include "common/noncopyable.h"
#include <string>

namespace tiary {

class PcRe : noncopyable {
public:
	// Construct an invalid regular expression
	PcRe (const std::wstring &);
	~PcRe ();

	operator void * () const { return re; }

	bool match (const std::wstring &) const; ///< Returns true iff matches

public:
	void *re, *re_ex;
};

} // namespace tiary

#endif // TIARY_USE_PCRE

#endif // include guard
