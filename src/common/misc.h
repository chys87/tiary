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


#ifndef TIARY_COMMON_MISC_H
#define TIARY_COMMON_MISC_H

#include <vector>
#include <string>
#include <stdio.h>

namespace tiary {

bool read_whole_file (FILE *, std::vector<char> &, size_t estimated_size = 4096);

/*
 * Query the value from a string-to-string map. Returns 0 if not found
 *
 * Only for:
 *   ChT = char/wchar_t
 *   MapType = corresponding std::map (locale/nolocale)/unordered_map
 */
template <typename MapT>
	const typename MapT::mapped_type::value_type *
	map_query (const MapT &map, const typename MapT::key_type::value_type *key);

// Save or overwrite a file as safely as possible
bool safe_write_file (const char *, const void *, size_t);

template <typename ChT> unsigned hex_to_num (ChT, unsigned error_return = 0);

// Expand environment variable representations like $param and ${param}
// Returns the number of expansions
unsigned environment_expand (std::string &);


} // namespace tiary


#endif // include guard
