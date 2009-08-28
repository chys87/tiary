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

#include "algorithm.h"
#include <stddef.h> // size_t
#include <locale>
#include <string>
#include <set>
#include <map>
#if TIARY_HAVE_STD_UNORDERED_SET_MAP
# include <unordered_set>
# include <unordered_map>
#elif TIARY_HAVE_TR1_UNORDERED_SET_MAP
# include <tr1/unordered_set>
# include <tr1/unordered_map>
#endif


namespace tiary {

#if TIARY_HAVE_STD_UNORDERED_SET_MAP

   using std::unordered_set;
   using std::unordered_map;
   using std::hash;

#elif TIARY_HAVE_TR1_UNORDERED_SET_MAP // unordered_{set,map} not in std, but available in std::tr1

   using std::tr1::unordered_set;
   using std::tr1::unordered_map;
   using std::tr1::hash;

#else // Fallback to std::set and std::map

  template <typename T> struct hash {};
  template <typename T, typename H = hash<T> > class unordered_set : public std::set <T> {};
  template <typename K, typename T, typename H = hash<T> > class unordered_map : public std::map <K,T> {};

#endif

typedef std::set<std::wstring,std::locale> WStringLocaleOrderedSet;
typedef std::set<std::string, std::locale> StringLocaleOrderedSet;
typedef std::map<std::wstring,std::wstring,std::locale> WStringLocaleOrderedMap;
typedef std::map<std::string, std::string, std::locale> StringLocaleOrderedMap;

typedef std::set<std::wstring> WStringOrderedSet;
typedef std::set<std::string > StringOrderedSet;
typedef std::map<std::wstring,std::wstring> WStringOrderedMap;
typedef std::map<std::string, std::string > StringOrderedMap;

typedef /* No std:: here */unordered_set<std::wstring> WStringUnorderedSet;
typedef /* No std:: here */unordered_set<std::string > StringUnorderedSet;

typedef /* No std:: here */unordered_map<std::wstring, std::wstring> WStringUnorderedMap;
typedef /* No std:: here */unordered_map<std::string,  std::string > StringUnorderedMap;


} // namespace tiary


#endif // Include guard
