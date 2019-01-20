// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2019, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/

#ifndef TIARY_COMMON_TYPE_IDENTITY_H
#define TIARY_COMMON_TYPE_IDENTITY_H

namespace tiary {

/// Identical to C++20's std::type_identity
template <typename T>
struct type_identity {
	using type = T;
};

template <typename T>
using type_identity_t = typename type_identity<T>::type;

} // namespace tiary

#endif // Include guard
