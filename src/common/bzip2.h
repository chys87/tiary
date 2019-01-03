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


/**
 * @file	common/bzip2.h
 * @author	chys <admin@chys.info>
 * @brief	Declares two simple wrappers for libbz2
 */

#ifndef TIARY_COMMON_BZIP2_H
#define TIARY_COMMON_BZIP2_H

#include <stddef.h>
#include <vector>
#include <string>
#include <string_view>

namespace tiary {

// If anything unexpected happens, returns an empty vector
// We trust contemporary compilers, assuming that the returned vector will
// not be deep-copied once and again.
//
// The second argument, though having type size_t, cannot exceed
// the limit of signed int. (Sure, this could be fixed, which however
// is unnecessary here.)

std::string bunzip2(const void *, size_t);
std::string bzip2(const void *, size_t);

inline std::string bunzip2(std::string_view s) {
	return bunzip2(s.data(), s.length());
}

inline std::string bzip2(std::string_view s) {
	return bzip2 (s.data (), s.length ());
}

} // namespace tiary

#endif // Include guard
