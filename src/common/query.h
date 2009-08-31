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


#ifndef TIARY_COMMON_QUERY_H
#define TIARY_COMMON_QUERY_H

/**
 * @file	common/query.h
 * @author	chys <admin@chys.info>
 * @brief	Declares the class tiary::Query
 *
 * A Query class is basically a Signal with a return value.
 * We do not provide as much functionality here as in common/signal.h
 */

#include <list>
#include <utility> // std::forward

namespace tiary {

template <typename R>
class Query
{
	struct Base
	{
		virtual R call () = 0;
		virtual Base *copy () const = 0;
#ifdef TIARY_HAVE_DEFAULT_DELETE_FUNCTIONS
		virtual ~Base () = default;
#else
		virtual ~Base () {}
#endif
	};

	template <typename C>
	struct ChildMV : Base
	{
		C *obj;
		R (C::*foo)();

		ChildMV (C *obj_, R (C::*foo_)()) : obj(obj_), foo(foo_) {}
		R call () { return (obj->*foo)(); }
		ChildMV *copy () const { return new ChildMV (obj, foo); }
	};
	template <typename C>
	struct ChildCMV : Base
	{
		const C *obj;
		R (C::*foo)() const;

		ChildCMV (const C *obj_, R (C::*foo_)() const) : obj(obj_), foo(foo_) {}
		R call () { return (obj->*foo)(); }
		ChildCMV *copy () const { return new ChildCMV (obj, foo); }
	};

	Base *info;

public:

	~Query () { delete info; }
	Query () : info (0) {}
	Query (const Query &other) : info (other.info ? other.info->copy () : 0) {}
	Query &operator = (const Query &other) { delete info; info = other.info ? other.info->copy () : 0; return *this; }
#ifdef TIARY_HAVE_RVALUE_REFERENCES
	Query (Query &&other) : info (other.info) { other.info = 0; }
	Query &operator = (Query &&other) { delete info; info = other.info; other.info = 0; return *this; }
#endif

	template <typename D> Query (D &obj, R (D::*foo)()) : info (new ChildMV <D> (&obj, foo)) {}
	template <typename D> Query (D *obj, R (D::*foo)()) : info (new ChildMV <D> (obj, foo)) {}
	template <typename D> Query (const D &obj, R (D::*foo)() const) : info (new ChildCMV <D> (&obj, foo)) {}
	template <typename D> Query (const D *obj, R (D::*foo)() const) : info (new ChildCMV <D> (obj, foo)) {}

	R call (R default_return) const { return (info ? info->call () : default_return); }
};


} // namespace tiary

#endif // Include guard
