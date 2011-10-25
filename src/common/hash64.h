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

/**
 * @file	common/hash64.h
 * @author	chys <admin@chys.info>
 * @brief	An anything-to-uint64_t hash function
 *
 * This hash function is much simpler and faster than MD5.
 * For practical purposes, it still maps different objects
 * to different hash values.
 */

#ifndef TIARY_COMMON_HASH64_H
#define TIARY_COMMON_HASH64_H

#include <stdint.h>
#include <string>
#include <string.h>

namespace tiary {

inline uint64_t hash64 (uint64_t x)
{
	return x;
}

uint64_t hash64 (const void *, size_t);
uint64_t hash64_wstring (const std::wstring &);

inline uint64_t hash64 (const std::string &s)
{
	return hash64 (s.data (), s.length ());
}

inline uint64_t hash64 (const std::wstring &s)
{
	return hash64_wstring (s);
}

} // namespace tiary

#endif // include guard
