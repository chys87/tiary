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
 * @file	common/bswap.h
 * @author	chys <admin@chys.info>
 * @brief	Defines bswap functions, converting between little/big endian integers
 */

#ifndef TIARY_COMMON_BSWAP_H
#define TIARY_COMMON_BSWAP_H


#include "common/types.h"

namespace tiary {

// Bswap: Reverse byte order (little <==> big)

inline uint16_t bswap16 (uint16_t x) { return (x << 8) | (x >> 8); }

inline uint32_t bswap32 (uint32_t x)
{
	                              // AABBCCDD
	x = (x >> 16) | (x << 16);    // CCDDAABB
	x = ((x >> 8) & (0x00FF00FFu)) | // 00CC00AA
		((x << 8) & (0xFF00FF00u));  // DD00BB00
	return x;
}

inline uint64_t bswap64 (uint64_t x)
{
	                                                       // AABBCCDD EEFFGGHH
	x = (x >> 32) | (x << 32);                             // EEFFGGHH AABBCCDD
	x = ((x >> 16) & TIARY_UINT64_C(0x0000ffffu,0x0000ffffu)) | // 0000EEFF 0000AABB
		((x << 16) & TIARY_UINT64_C(0xffff0000u,0xffff0000u));  // GGHH0000 CCDD0000
	                                                            // GGHHEEFF CCDDAABB
	x = ((x >> 8) & TIARY_UINT64_C(0x00ff00ffu,0x00ff00ffu)) |  // 00GG00EE 00CC00AA
		((x << 8) & TIARY_UINT64_C(0xff00ff00u,0xff00ff00u));   // HH00FF00 DD00BB00
	return x;
}

} // namespace tiary

#endif // include guard
