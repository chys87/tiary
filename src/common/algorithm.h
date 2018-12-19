// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2010, 2016, 2018 chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


/**
 * @file	common/algorithm.h
 * @author	chys <admin@chys.info>
 * @brief	Extensions to @c <algorithm>, @c <functional> and some useful utilities
 */

#ifndef TIARY_COMMON_ALGORITHM_H
#define TIARY_COMMON_ALGORITHM_H

#include <functional>
#include <algorithm>

namespace tiary {


// These functions make sure arguments are treated as unsigned or signed
// Almost identical to std::{max,min}<{un,}signed>, but shorter
inline unsigned maxU (unsigned a, unsigned b) { return (a<b) ? b : a; }
inline unsigned minU (unsigned a, unsigned b) { return (a<b) ? a : b; }
inline int maxS (int a, int b) { return (a<b) ? b : a; }
inline int minS (int a, int b) { return (a<b) ? a : b; }
// These functions make sure arguments are cast to size_t
inline size_t maxSize (size_t a, size_t b) { return (a<b) ? b : a; }
inline size_t minSize (size_t a, size_t b) { return (a<b) ? a : b; }

/**
 * @brief	Makes a reference const
 *
 * For some classes, const and non-const instances have different behaviors.
 * This can be helpful in such situations.
 *
 * For example, if std::string has a reference-counted implementation, this
 * can be helpful to eliminate or defer deep copies sometimes.
 */
template <typename T> inline const T &c (T &r)
{
	return r;
}

/**
 * @brief	Makes a pointer const
 *
 * Same reason
 */
template <typename T> inline const T *c (T *r)
{
	return r;
}

/*
 * Same as std::identity in C++20
 */
struct identity {
	template <typename T>
	constexpr T &&operator()(T &&t) const {
		return std::forward<T>(t);
	}
};


namespace detail {

template <typename A, typename B> inline
int compare_helper (const A &x, const B &y)
{
	if (x < y) {
		return -1;
	}
	else if (x == y) {
		return 0;
	}
	else {
		return 1;
	}
}

} // namespace detail

// Performs a binary search.
// Returns pointer (or 0 if not found)
template <typename T, typename T2, typename K>
T *binary_search_null (T *lo, T *hi, T2 v, K key)
{
	while (lo < hi) {
		T *m = lo + size_t(hi-lo)/2;
		switch (detail::compare_helper (key (*m), v)) {
			case -1:
				lo = m + 1;
				break;
			case 0:
				return m;
			case 1:
				hi = m;
				break;
		}
	}
	return 0;
}

// No key necessary
template <typename T, typename T2> T* binary_search_null (T *lo, T *hi, T2 v)
{
	return binary_search_null(lo, hi, v, identity());
}

// Performs a linear search. Returns 0 if not found
template <typename T, typename T2, typename K> T* linear_search_null (T *lo, T *hi, T2 v, K key)
{
	while (lo < hi) {
		if (key (*lo) == v) {
			return lo;
		}
		++lo;
	}
	return 0;
}

// No key necessary
template <typename T, typename T2> T* linear_search_null (T *lo, T *hi, T2 v)
{
	return linear_search_null(lo, hi, v, identity());
}


/* Map transform using binary search. For example:
 * static const std::pair<char,char> upper_map[] = {
 *   { 'a', 'A' },
 *   { 'b', 'B' },
 *   .....
 *   { 'z', 'Z' }
 * };
 * Then binary_transform(upper_map, array_end(upper_map), ch, ch) is equivalent to toupper(ch) (in C locale)
 */
template <typename A, typename B>
B binary_transform(const std::pair<A, B> *lo, const std::pair<A, B> *hi, A from, B defto) {
	if (const std::pair<A, B> *p = binary_search_null(lo, hi, from, std::mem_fn(&std::pair<A, B>::first))) {
		return p->second;
	}
	return defto;
}

// Map transform using linear search
template <typename A, typename B>
B linear_transform(const std::pair<A, B> *lo, const std::pair<A, B> *hi, A from, B defto) {
	if (const std::pair<A, B> *p = linear_search_null(lo, hi, from, std::mem_fn(&std::pair<A, B>::first))) {
		return p->second;
	}
	return defto;
}



// Transform between two bit-wise schemes
template <typename A, typename B>
B bitwise_transform(const std::pair<A, B> *lo, const std::pair<A, B> *hi, A from) {
	B to = 0;
	while (lo < hi) {
		if (from & lo->first) {
			to |= lo->second;
		}
		++lo;
	}
	return to;
}

// The other way
template <typename A, typename B>
A bitwise_reverse_transform(const std::pair<A, B> *lo, const std::pair<A, B> *hi, B to) {
	A from = 0;
	while (lo < hi) {
		if (to & lo->second) {
			from |= lo->first;
		}
		++lo;
	}
	return from;
}



/**
 * @brief	Do a "min max programming"
 *
 * Compute "reasonable" values for result, s.t. min[i]<=result[i]<=max[i], Sum(result[i]:i=0..n-1)=S
 *
 * It's the caller's responsibility to guarantee the feasible set is not empty
 */
void min_max_programming (unsigned *result, const unsigned *min, const unsigned *max, unsigned n, unsigned S);



// Measure the length of an array conveniently
template <typename T, size_t N> inline constexpr size_t array_length(T (&)[N]) { return N; }

// Get the end of an array conveniently
template <typename T, size_t N> inline constexpr T* array_end(T (&a)[N]) { return (a+N); }


} // namespace tiary

#endif // Include guard
