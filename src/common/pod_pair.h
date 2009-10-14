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
 * @file	common/pod_pair.h
 * @author	chys <admin@chys.info>
 * @brief	Declares type tiary::Pair
 */


#ifndef TIARY_COMMON_POD_PAIR_H
#define TIARY_COMMON_POD_PAIR_H

namespace tiary {

/**
 * @brief	A "pod pair" template type
 *
 * This is similar to @c std::pair, but @c Pair<TA,TB> is a POD type
 * if both @c TA and @c TB are POD types. This helps optimizations.
 */
template <typename TA, typename TB>
struct Pair
{
	TA first;
	TB second;
};

// Rvalue references are not used here.
// If using that would improve efficiency for TA or TB,
// then there's no difference for std::pair and tiary::Pair
// for them.
template <typename TA, typename TB> inline
Pair <TA, TB> make_Pair (const TA &a, const TB &b)
{
	Pair <TA, TB> ret = { a, b };
	return ret;
}

} // namespace tiary

#endif // include guard
