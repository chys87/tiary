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
#include "common/query.h"
#include <utility> // std::move

namespace tiary {

/**
 * @brief	Class for an "action"
 *
 * An Action is a Signal plus a Query<bool> to verify its current usability
 */
struct Action
{
	Signal signal;
	Query<bool> query;

	Action () : signal (), query () {}
	~Action () {}
	explicit Action (const Signal &sig) : signal (sig), query () {}
	explicit Action (const Query<bool> &qry) : signal (), query (qry) {}
	Action (const Signal &sig, const Query<bool> &qry) : signal (sig), query (qry) {}
	Action (const Action &act) : signal (act.signal), query (act.query) {}
	Action &operator = (const Signal &sig) { signal = sig; return *this; }
	Action &operator = (const Query<bool> &qry) { query = qry; return *this; }
	Action &operator = (const Action &act) { signal = act.signal; query = act.query; return *this; }

#ifdef TIARY_HAVE_RVALUE_REFERENCES
	explicit Action (Signal &&sig) : signal (std::move (sig)), query () {}
	explicit Action (Query<bool> &&qry) : signal (), query (std::move (qry)) {}
	explicit Action (Signal &&sig, const Query<bool> &qry) : signal (std::move (sig)), query (qry) {}
	explicit Action (const Signal &sig, Query<bool> &&qry) : signal (sig), query (std::move (qry)) {}
	explicit Action (Signal &&sig, Query<bool> &&qry) : signal (std::move (sig)), query (std::move (qry)) {}
	Action &operator = (Signal &&sig) { signal = std::move (sig); return *this; }
	Action &operator = (Query<bool> &&qry) { query = std::move (qry); return *this; }
	Action &operator = (Action &&act) { signal = std::move (act.signal); query = std::move (act.query); return *this; }
#endif // rvalue ref

	void emit () { signal.emit (); }
	bool call_query (bool default_return) const { return query.call (default_return); }

	// rvalue-reference of *this is not supported by GCC yet
	operator Signal & () { return signal; }
	operator Query<bool> & () { return query; }
	operator const Signal & () const { return signal; }
	operator const Query<bool> & () const { return query; }
};

} // namespace tiary

#endif // include guard
