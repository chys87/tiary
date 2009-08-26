// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the so-called 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#ifndef TIARY_COMMON_TYPES_H
#define TIARY_COMMON_TYPES_H

#include <stddef.h> // ::size_t

namespace tiary {

// These types are placed in namespace tiary, so there will not be any
// conflict even if <stdint.h> is also included
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned uint32_t;
typedef TIARY_INT64_TYPE int64_t;
typedef unsigned TIARY_INT64_TYPE uint64_t;
#if TIARY_SIZEOF_POINTER == 4
typedef int32_t intptr_t;
typedef uint32_t uintptr_t;
#elif TIARY_SIZEOF_POINTER == 8
typedef int64_t intptr_t;
typedef uint64_t uintptr_t;
#else
# error "Unsupported pointer size."
#endif

// Define a 64-bit integer constant
#define TIARY_UINT64_C(hi,lo)	((::tiary::uint64_t(hi) << 32) | ::tiary::uint32_t(lo))

} // namespace tiary

#endif // Include guard
