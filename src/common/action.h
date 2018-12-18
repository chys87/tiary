// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2016 chys <admin@CHYS.INFO>
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
	Action (const Signal &sig) : signal (sig), condition () {}
	Action (const Condition &cond) : signal (), condition (cond) {}
	Action (const Signal &sig, const Condition &cond) : signal (sig), condition (cond) {}
	Action (const Action &act) : signal (act.signal), condition (act.condition) {}
	Signal &operator = (const Signal &sig) { return signal = sig; }
	Condition &operator = (const Condition &cond) { return condition = cond; }
	Action &operator = (const Action &act) { signal = act.signal; condition = act.condition; return *this; }

	Action (Signal &&sig) : signal (std::move (sig)), condition () {}
	Action (Condition &&cond) : signal (), condition (std::move (cond)) {}
	Action (Signal &&sig, const Condition &cond) : signal (std::move (sig)), condition (cond) {}
	Action (const Signal &sig, Condition &&cond) : signal (sig), condition (std::move (cond)) {}
	Action (Signal &&sig, Condition &&cond) : signal (std::move (sig)), condition (std::move (cond)) {}
	Signal &operator = (Signal &&sig) { return signal = std::move (sig); }
	Condition &operator = (Condition &&cond) { return condition = std::move (cond); }
	Action &operator = (Action &&act) { signal = std::move (act.signal); condition = std::move (act.condition); return *this; }

	// Forward is_connected and is_really_connected to signal
	bool is_connected () const { return signal.is_connected (); }
	bool is_really_connected () const { return signal.is_really_connected (); }
	// Forward all calls to connect to signal
	template <typename... Args> void connect (Args &&...args)
	{
		signal.connect (std::forward <Args> (args)...);
	}

	void emit () { signal.emit (); }
	bool call_condition (bool default_return) const { return condition.call (default_return); }
};

} // namespace tiary

#endif // include guard
