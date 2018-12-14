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
 * Can be used as arguments of for_each, etc.
 */
template <typename T>
struct DeleteFunctor : public std::unary_function <T *, void>
{
	void operator () (T *ptr) const
	{
		delete ptr;
	}
};

template <typename T> inline DeleteFunctor<T> delete_fun ()
{
	return DeleteFunctor<T>();
}

/*
 * A "cast functor" class
 */
template <typename TO, typename TI>
struct CastFunctor : public std::unary_function <TI, TO>
{
	TO operator () (TI a) const
	{
		return a;
	}
};

template <typename TO, typename TI> inline CastFunctor <TO, TI> cast_fun ()
{
	return CastFunctor <TO, TI> ();
}

/*
 * An "identity functor" class
 */
template <typename T>
struct IdentityFunctor : public CastFunctor <T,T>
{
};

template <typename T> inline IdentityFunctor<T> identity_fun ()
{
	return IdentityFunctor<T> ();
}


/*
 * A "get member functor" class
 */
template <typename Class, typename Member>
struct GetMemberFunctor : public std::unary_function<Class &, Member &>
{
	Member Class::*ptr;
	GetMemberFunctor (Member Class::*p) : ptr (p) {}

	Member &operator () (Class &a) const
	{
		return a.*ptr;
	}
	const Member &operator () (const Class &a) const
	{
		return a.*ptr;
	}
};

template <typename Class, typename Member> inline
GetMemberFunctor<Class,Member> get_member_fun (Member Class::*ptr)
{
	return GetMemberFunctor<Class,Member>(ptr);
}



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
	return binary_search_null (lo, hi, v, identity_fun<const T &>());
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
	return linear_search_null (lo, hi, v, identity_fun<const T &> ());
}


/*
 * Used to store a mapping structure. For example:
 *
 * static const MapStruct<char,char> upper_map[] = {
 *   { 'a', 'A' },
 *   { 'b', 'B' },
 *   .....
 *   { 'z', 'Z' }
 * };
 * Then binary_transform(upper_map, ch, ch) is equivalent to toupper(ch) (in C locale)
 */
template <typename A, typename B> struct MapStruct
{
	A from;
	B to;
};

// Map transform using binary search
template <typename A, typename B>
B binary_transform (const MapStruct<A,B> *lo, const MapStruct<A,B> *hi, A from, B defto)
{
	if (const MapStruct<A,B> *p = binary_search_null (lo, hi, from, get_member_fun (&MapStruct<A,B>::from))) {
		return p->to;
	}
	return defto;
}

// Map transform using linear search
template <typename A, typename B>
B linear_transform (const MapStruct<A,B> *lo, const MapStruct<A,B> *hi, A from, B defto)
{
	if (const MapStruct<A,B> *p = linear_search_null (lo, hi, from, get_member_fun (&MapStruct<A,B>::from))) {
		return p->to;
	}
	return defto;
}



// Transform between two bit-wise schemes
template <typename A, typename B>
B bitwise_transform (const MapStruct<A,B> *lo, const MapStruct<A,B> *hi, A from)
{
	B to = 0;
	while (lo < hi) {
		if (from & lo->from) {
			to |= lo->to;
		}
		++lo;
	}
	return to;
}

// The other way
template <typename A, typename B>
A bitwise_reverse_transform (const MapStruct<A,B> *lo, const MapStruct<A,B> *hi, B to)
{
	A from = 0;
	while (lo < hi) {
		if (to & lo->to) {
			from |= lo->from;
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
