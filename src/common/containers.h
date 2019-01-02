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


#ifndef TIARY_COMMON_CONTAINERS_H
#define TIARY_COMMON_CONTAINERS_H

#include <stddef.h> // size_t
#include <locale>
#include <string>
#include <set>
#include <map>


namespace tiary {

typedef std::set<std::wstring,std::locale> WStringLocaleOrderedSet;
typedef std::set<std::string, std::locale> StringLocaleOrderedSet;
typedef std::map<std::wstring,std::wstring,std::locale> WStringLocaleOrderedMap;
typedef std::map<std::string, std::string, std::locale> StringLocaleOrderedMap;

// Explicitly use std::less<> so that we can take advantage of C++14 transparent comparators -
// std::map<K, V, std::less<>>::find(const AnotherKey &) doesn't require AnotherKey is the
// same as K
typedef std::set<std::wstring, std::less<>> WStringOrderedSet;
typedef std::set<std::string, std::less<>> StringOrderedSet;
typedef std::map<std::wstring, std::wstring, std::less<>> WStringOrderedMap;
typedef std::map<std::string, std::string, std::less<>> StringOrderedMap;

} // namespace tiary


#endif // Include guard
