// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2016, 2018 chys <admin@CHYS.INFO>
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
class Action {
public:
	Action() = default;
	Action(const Signal &sig) : signal_(sig), condition_() {}
	Action(const Condition &cond) : signal_(), condition_(cond) {}
	Action(const Signal &sig, const Condition &cond) : signal_(sig), condition_(cond) {}
	Action(const Action &act) = default;
	Signal &operator = (const Signal &sig) { return signal_ = sig; }
	Condition &operator = (const Condition &cond) { return condition_ = cond; }
	Action &operator = (const Action &act) = default;

	Action(Signal &&sig) : signal_(std::move(sig)), condition_() {}
	Action(Condition &&cond) : signal_(), condition_(std::move(cond)) {}
	Action(Signal &&sig, const Condition &cond) : signal_(std::move(sig)), condition_(cond) {}
	Action(const Signal &sig, Condition &&cond) : signal_(sig), condition_(std::move(cond)) {}
	Action(Signal &&sig, Condition &&cond) : signal_(std::move(sig)), condition_(std::move(cond)) {}
	Action(Action &&) = default;
	Signal &operator = (Signal &&sig) { return signal_ = std::move(sig); }
	Condition &operator = (Condition &&cond) { return condition_ = std::move(cond); }
	Action &operator = (Action &&act) = default;

	// Forward is_connected and is_really_connected to signal
	bool is_connected () const { return signal_.is_connected (); }
	bool is_really_connected () const { return signal_.is_really_connected (); }
	// Forward all calls to connect to signal
	template <typename... Args> void connect (Args &&...args) {
		signal_.connect(std::forward<Args>(args)...);
	}

	void emit() { signal_.emit (); }
	bool call_condition(bool default_return) const { return condition_.call (default_return); }

	Signal &signal() { return signal_; }
	Condition &condition() { return condition_; }
	const Signal &signal() const { return signal_; }
	const Condition &condition() const { return condition_; }

private:
	Signal signal_;
	Condition condition_;
};

} // namespace tiary

#endif // include guard
