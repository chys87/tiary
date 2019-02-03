// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2019, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#ifndef TIARY_COMMON_MISC_H
#define TIARY_COMMON_MISC_H

#include <stdio.h>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace tiary {

bool read_whole_file(FILE *, std::string *, size_t estimated_size = 4096);
bool read_whole_file(int, std::string *, size_t estimated_size = 4096);

// Save or overwrite a file as safely as possible
bool safe_write_file(const char *filename, std::string_view data, std::string_view data2 = {});

// Expand environment variable representations like $param and ${param}
// Returns the number of expansions
std::string environment_expand(std::string_view);


} // namespace tiary


#endif // include guard
