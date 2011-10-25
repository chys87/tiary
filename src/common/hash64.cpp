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

#include "common/hash64.h"

namespace tiary {

uint64_t hash64 (const void *data, size_t len)
{
	// Fowler-Noll-Vo (FNV-1a) hash function

	const uint8_t *p = reinterpret_cast <const uint8_t *> (data);
	uint64_t A = 0xcbf29ce484222325ull;
	for (; len; --len) {
		A ^= *p++;
		A *= 0x100000001b3ull;
	}
	return A;
}

uint64_t hash64_wstring (const std::wstring &s)
{
	uint64_t A = 0xcbf29ce484222325ull;
	for (const wchar_t *p = s.c_str (); *p; ++p) {
		wchar_t c = *p;
		A ^= uint8_t (c);
		A *= 0x100000001b3ull;
		A ^= uint32_t (c) >> 8;
		A *= 0x100000001b3ull;
	}
	return A;
}

} // namespace tiary
