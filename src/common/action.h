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


#ifndef TIARY_COMMON_ACTION_H
#define TIARY_COMMON_ACTION_H

#include "common/signal.h"
#include "common/condition.h"
#include <utility> // std::move

namespace tiary {

/**
 * @brief	Class for an "action"
 *
 * An Action is a Signal plus a Condition to verify its current usability
 */
struct Action
{
	Signal signal;
	Condition condition;

	Action () : signal (), condition () {}
	~Action () {}
	explicit Action (const Signal &sig) : signal (sig), condition () {}
	explicit Action (const Condition &cond) : signal (), condition (cond) {}
	Action (const Signal &sig, const Condition &cond) : signal (sig), condition (cond) {}
	Action (const Action &act) : signal (act.signal), condition (act.condition) {}
	Action &operator = (const Signal &sig) { signal = sig; return *this; }
	Action &operator = (const Condition &cond) { condition = cond; return *this; }
	Action &operator = (const Action &act) { signal = act.signal; condition = act.condition; return *this; }

#ifdef TIARY_HAVE_RVALUE_REFERENCES
	explicit Action (Signal &&sig) : signal (std::move (sig)), condition () {}
	explicit Action (Condition &&cond) : signal (), condition (std::move (cond)) {}
	explicit Action (Signal &&sig, const Condition &cond) : signal (std::move (sig)), condition (cond) {}
	explicit Action (const Signal &sig, Condition &&cond) : signal (sig), condition (std::move (cond)) {}
	explicit Action (Signal &&sig, Condition &&cond) : signal (std::move (sig)), condition (std::move (cond)) {}
	Action &operator = (Signal &&sig) { signal = std::move (sig); return *this; }
	Action &operator = (Condition &&cond) { condition = std::move (cond); return *this; }
	Action &operator = (Action &&act) { signal = std::move (act.signal); condition = std::move (act.condition); return *this; }
#endif // rvalue ref

#ifdef TIARY_HAVE_RVALUE_REFERENCES
	void swap (Action &&other)
#else
	void swap (Action &other)
#endif
	{
		signal.swap (other.signal);
		condition.swap (other.condition);
	}

	// Forward is_connected and is_really_connected to signal
	bool is_connected () const { return signal.is_connected (); }
	bool is_really_connected () const { return signal.is_really_connected (); }
	// Forward all calls to connect to signal
#if defined TIARY_HAVE_RVALUE_REFERENCES && defined TIARY_HAVE_TUPLES
	template <typename... Args> void connect (Args &&...args)
	{
		signal.connect (std::forward <Args> (args)...);
	}
#elif defined TIARY_HAVE_RVALUE_REFERENCES
	template <typename A> void connect (A &&a)
	{
		signal.connect (std::forward <A> (a));
	}
	template <typename A, typename B> void connect (A &&a, B &&b)
	{
		signal.connect (std::forward <A> (a), std::forward <B> (b));
	}
	template <typename A, typename B, typename C> void connect (A &&a, B &&b, C &&c)
	{
		signal.connect (std::forward <A> (a), std::forward <B> (b), std::forward <C> (c));
	}
	template <typename A, typename B, typename C, typename D> void connect (A &&a, B &&b, C &&c, D &&d)
	{
		signal.connect (std::forward <A> (a), std::forward <B> (b), std::forward <C> (c), std::forward <D> (d));
	}
#else
	template <typename A> void connect (const A &a)
	{
		signal.connect (a);
	}
	template <typename A, typename B> void connect (const A &a, const B &b)
	{
		signal.connect (a, b);
	}
	template <typename A, typename B, typename C> void connect (const A &a, const B &b, const C &c)
	{
		signal.connect (a, b, c);
	}
	template <typename A, typename B, typename C, typename D> void connect (const A &a, const B &b, const C &c, const D &d)
	{
		signal.connect (a, b, c, d);
	}
#endif

	void emit () { signal.emit (); }
	bool call_condition (bool default_return) const { return condition.call (default_return); }

	// rvalue-reference of *this is not supported by GCC yet
	operator Signal & () { return signal; }
	operator Condition & () { return condition; }
	operator const Signal & () const { return signal; }
	operator const Condition & () const { return condition; }
};

} // namespace tiary

#endif // include guard
