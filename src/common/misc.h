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

#include <stdio.h>
#include <string>
#include <type_traits>
#include <vector>

namespace tiary {

bool read_whole_file(FILE *, std::string &, size_t estimated_size = 4096);

namespace map_query_detail {

template <typename T> struct char_type {};
template <typename C, typename... T> struct char_type<std::basic_string<C, T...>> { using type = C; };

} // namespace map_query_detail

/*
 * Query the value from a string-to-string map. Returns nullptr if not found
 *
 * Only for:
 *   ChT = char/wchar_t
 *   MapType = corresponding std::map (locale/nolocale)/unordered_map
 */
template <typename MapT, typename K>
const typename map_query_detail::char_type<typename MapT::mapped_type>::type *map_query(const MapT &map, const K &key) {
	typename MapT::const_iterator it = map.find(key);
	if (it != map.end()) {
		return it->second.c_str();
	} else {
		return nullptr;
	}
}

// Save or overwrite a file as safely as possible
bool safe_write_file(const char *filename, std::string_view data, std::string_view data2 = {});

// Expand environment variable representations like $param and ${param}
// Returns the number of expansions
unsigned environment_expand (std::string &);


} // namespace tiary


#endif // include guard
