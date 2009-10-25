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
	uint64_t A = TIARY_UINT64_C (0xcbf29ce4, 0x84222325);
	for (; len; --len) {
		A ^= *p++;
		A *= TIARY_UINT64_C (0x100, 0x1b3);
	}
	return A;
}

uint64_t hash64_wchar (const wchar_t *p, size_t len)
{
	uint64_t A = TIARY_UINT64_C (0xcbf29ce4, 0x84222325);
	for (; len; --len) {
		wchar_t c = *p++;
		A ^= uint8_t (c);
		A *= TIARY_UINT64_C (0x100, 0x1b3);
		A ^= uint32_t (c) >> 8;
		A *= TIARY_UINT64_C (0x100, 0x1b3);
	}
	return A;
}

uint64_t hash64_wstring (const std::wstring &s)
{
	return hash64_wchar (s.data (), s.length ());
}

} // namespace tiary
