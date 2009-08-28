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


#ifndef TIARY_COMMON_BZIP2_H
#define TIARY_COMMON_BZIP2_H

#include <stddef.h>
#include <vector>

namespace tiary {

typedef std::vector<char> BZip2Result;

// If anything unexpected happens, returns an empty vector
// We trust contemporary compilers, assuming that the returned vector will
// not be deep-copied once and again.
//
// The second argument, though having type size_t, cannot exceed
// the limit of signed int. (Sure, this could be fixed, which however
// is unnecessary here.)

BZip2Result bunzip2 (const void *, size_t);
BZip2Result bzip2 (const void *, size_t);


} // namespace tiary

#endif // Include guard
