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
#if TIARY_HAVE_STD_ARRAY
# include <array>
#elif TIARY_HAVE_TR1_ARRAY
# include <tr1/array>
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



#if TIARY_HAVE_STD_ARRAY

   using std::array;

#elif TIARY_HAVE_TR1_ARRAY

   using std::tr1::array;

#else // Implement it ourselves (incomplete, but sufficient)

template <typename T, size_t N> struct array
{
	T elems[N];

	T &operator [] (size_t j) { return elems[j]; }
	const T &operator [] (size_t j) const { return elems[j]; }

	T *data () { return elems; }
	const T *data () const { return elems; }
};

template <typename T> struct array <T, 0> // In case compilers dislike zero-size arrays
{
	T &operator [] (size_t) { throw 0; }
	const T &operator [] (size_t) const { throw 0; }

	T *data () { return 0; }
	const T *data () const { return 0; }
};

#endif


/*
 * AutoArray<T>: Allocate in heap only if the block is large
 */
template <typename T, size_t StackSize = 512> class AutoArray
{
	static const size_t STACK_NUM = StackSize/sizeof(T);
	T *p;
	array<T,STACK_NUM> buf;
public:
	explicit AutoArray(size_t n) { if (n <= STACK_NUM) p=buf.data(); else p=new T[n]; }
	~AutoArray () { if (p != buf.data ()) delete [] p; }

	T& operator [] (size_t j) { return p[j]; }
	const T& operator [] (size_t j) const { return p[j]; }

	T *data () { return p; }
	const T *data () const { return p; }

#ifdef TIARY_HAVE_DEFAULT_DELETE_FUNCTIONS
	// This class is intended to be used on stack
	void * operator new (size_t) = delete;
	void * operator new [] (size_t) = delete;
#endif
};



} // namespace tiary


#endif // Include guard
