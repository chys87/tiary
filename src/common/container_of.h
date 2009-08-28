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


#ifndef TIARY_COMMON_CONTAINER_OF_H
#define TIARY_COMMON_CONTAINER_OF_H

/**
 * @file	common/container_of.h
 * @author	chys <admin@chys.info>
 * @brief	Make a list, vector or similar container easily
 *
 * The idea is very similar to boost::assign::list_of.
 * (I did not refer to the codes of Boost. Even if considering
 * this as derivative works of Boost, I am happy that Boost has
 * a very friendly license.)
 *
 * In my implementation, many redudant copies can be eliminated
 * if the compiler supports r-value references.
 * (And, of course, STL containers have constructors that take
 * r-value-reference parameters defined.)
 *
 * The world would be nicer when we have std::initializer_list
 * support. If support for initializer list is available (as is in
 * GCC 4.4), we actually use it, simpler and more efficient.
 *
 * Example:
 *
 * std::list<int> my_list = TIARY_LIST_OF(int)   3, 4, 5, 6   TIARY_LIST_OF_END;
 */

#define TIARY_LIST_OF(type)		TIARY_CONTAINER_OF(::std::list<type >)
#define TIARY_LIST_OF_END		TIARY_CONTAINER_OF_END
#define TIARY_VECTOR_OF(type)	TIARY_CONTAINER_OF(::std::vector<type >)
#define TIARY_VECTOR_OF_END		TIARY_CONTAINER_OF_END

#ifdef TIARY_HAVE_INITIALIZER_LIST

# define TIARY_CONTAINER_OF(type)	(type {
# define TIARY_CONTAINER_OF_END		} )

#else // ifndef TIARY_HAVE_INITIALIZER_LIST

# define TIARY_CONTAINER_OF(type)	(::tiary::detail::ContainerOf<type >(),
# define TIARY_CONTAINER_OF_END		)()

#include <utility> // std::forward

namespace tiary {

namespace detail {

template <typename C>
struct ContainerOf : public C
{
	typedef typename C::value_type T;

#ifdef TIARY_HAVE_RVALUE_REFERENCES
	ContainerOf &operator ,  (T &&x) { C::push_back (std::forward<T> (x)); return *this; }
#endif
	ContainerOf &operator ,  (const T &x) { C::push_back (x); return *this; }

#ifdef TIARY_HAVE_RVALUE_REFERENCES
	C && operator () () { return *this; }
#else
	C & operator () () { return *this; }
#endif
};

} // namespace detail
} // namespace tiary

#endif // !TIARY_HAVE_INITIALIZER_LIST

#endif // Include guard
