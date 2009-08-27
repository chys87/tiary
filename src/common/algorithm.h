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


#ifndef TIARY_COMMON_ALGORITHM_H
#define TIARY_COMMON_ALGORITHM_H

#include "common/types.h"
#include <iterator>
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

template <typename T>
inline int sign (T x) { return (x<0) ? -1 : (x==0) ? 0 : 1; }

// Bswap: Reverse byte order (little <==> big)

inline uint16_t bswap16 (uint16_t x) { return (x << 8) | (x >> 8); }

inline uint32_t bswap32 (uint32_t x)
{
	                              // AABBCCDD
	x = (x >> 16) | (x << 16);    // CCDDAABB
	x = ((x >> 8) | (0x00FF00FFu)) | // 00CC00AA
		((x << 8) | (0xFF00FF00u));  // DD00BB00
	return x;
}

inline uint64_t bswap64 (uint64_t x)
{
	                                                       // AABBCCDD EEFFGGHH
	x = (x >> 32) | (x << 32);                             // EEFFGGHH AABBCCDD
	x = ((x >> 16) | TIARY_UINT64_C(0x0000ffffu,0x0000ffffu)) | // 0000EEFF 0000AABB
		((x << 16) | TIARY_UINT64_C(0xffff0000u,0xffff0000u));  // GGHH0000 CCDD0000
	                                                            // GGHHEEFF CCDDAABB
	x = ((x >> 8) | TIARY_UINT64_C(0x00ff00ffu,0x00ff00ffu)) |  // 00GG00EE 00CC00AA
		((x << 8) | TIARY_UINT64_C(0xff00ff00u,0xff00ff00u));   // HH00FF00 DD00BB00
	return x;
}


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
template <typename T> inline DeleteFunctor<T> delete_fun (T *)
{
	return DeleteFunctor<T>();
}
template <typename T> inline DeleteFunctor<T> delete_fun (T &)
{
	return DeleteFunctor<T>();
}

/*
 * A "cast functor" class
 */
template <typename TO, typename TI>
struct CastFunctor : public std::unary_function <TI, TO>
{
	TO operator () (TI a)
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
template <typename T> inline IdentityFunctor<T> identity_fun (const T &)
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



#ifdef TIARY_HAVE_RVALUE_REFERENCES
# define TIARY_STD_MOVE ::std::move
#else
# define TIARY_STD_MOVE
#endif


// Performs a binary search.
// Returns pointer (or 0 if not found)
template <typename T, typename T2, typename K>
T *binary_search_null (T *lo, T *hi, T2 v, K key)
{
	while (lo < hi) {
		T *m = lo + size_t(hi-lo)/2;
#ifdef TIARY_HAVE_DECLTYPE
		decltype(key(*m)) keym (key (*m));
		if (keym < v)
			lo = m + 1;
		else if (keym == v)
#else
		if (key (*m) < v)
			lo = m + 1;
		else if (key (*m) == v)
#endif
			return m;
		else
			hi = m;
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
		if (key (*lo) == v)
			return lo;
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
	if (const MapStruct<A,B> *p = binary_search_null (lo, hi, from, get_member_fun (&MapStruct<A,B>::from)))
		return p->to;
	return defto;
}

// Map transform using linear search
template <typename A, typename B>
B linear_transform (const MapStruct<A,B> *lo, const MapStruct<A,B> *hi, A from, B defto)
{
	if (const MapStruct<A,B> *p = linear_search_null (lo, hi, from, get_member_fun (&MapStruct<A,B>::from)))
		return p->to;
	return defto;
}



// Transform between two bit-wise schemes
template <typename A, typename B>
B bitwise_transform (const MapStruct<A,B> *lo, const MapStruct<A,B> *hi, A from)
{
	B to = 0;
	while (lo < hi) {
		if (from & lo->from)
			to |= lo->to;
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
		if (to & lo->to)
			from |= lo->from;
		++lo;
	}
	return from;
}


/*
 * Removes the first element in container equal to value
 *
 * The third argument helps guaranteeing that the first argument is a container,
 * similar to "concept" (former candidate for C++0x)
 */
template <typename Container, typename T>
void remove_first (Container &container, const T &value, typename Container::iterator = typename Container::iterator ())
{
	typename Container::iterator it = std::find (container.begin (), container.end (), value);
	if (it != container.end ())
		container.erase (it);
}

/*
 * Removes every element in container equal to value
 */
template <typename Container, typename T>
void remove_all (Container &container, const T &value, typename Container::iterator = typename Container::iterator ())
{
	typename Container::iterator it = std::remove (container.begin (), container.end (), value);
	container.erase (it, container.end ());
}


/*
 * Get the sum of everything in an interval
 */
template <typename Iter, typename F>
typename F::result_type sum (Iter first, Iter last, F foo, typename F::result_type initial_value = typename F::result_type ())
{
	typename F::result_type r = initial_value;
	while (first != last)
		r += foo (*first++);
	return r;
}

/*
 * No transform
 */
template <typename Iter>
typename Iter::value_type sum (Iter first, Iter last, typename Iter::value_type initial_value = typename Iter::value_type ())
{
	typename Iter::value_type r = initial_value;
	while (first != last)
		r += *first++;
	return r;
}

/*
 * Also possible to replace last with a number
 */
template <typename Iter, typename F>
typename F::result_type sum (Iter first, size_t n, F foo, typename F::result_type initial_value = typename F::result_type ())
{
	typename F::result_type r = initial_value;
	for (; n; --n)
		r += foo (*first++);
	return r;
}
template <typename Iter>
typename std::iterator_traits<Iter>::value_type sum (Iter first, size_t n,
		typename std::iterator_traits<Iter>::value_type initial_value = typename std::iterator_traits<Iter>::value_type ())
{
	typename std::iterator_traits<Iter>::value_type r = initial_value;
	for (; n; --n)
		r += *first++;
	return r;
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
template <typename T, size_t N> inline size_t array_length (T (&)[N]) { return N; }

// If the result must be a compile-time constant, then the following
// difficult-to-read implementation can be helpful
namespace detail {
template <size_t N> struct array_length_helper2
{
	char y[N];
};
template <typename T, size_t N>
inline array_length_helper2<N> array_length_helper (T (&)[N])
{
	return array_length_helper2<N>();
}
} // namespace detail
#define TIARY_ARRAY_LENGTH(x) sizeof(::tiary::detail::array_length_helper(x).y)

// Get the end of an array conveniently
template <typename T, size_t N> inline T* array_end (T (&a)[N]) { return (a+N); }
// Compile-time constant
#define TIARY_ARRAY_END(x) ((x) + TIARY_ARRAY_LENGTH(x))


} // namespace tiary

#endif // Include guard
