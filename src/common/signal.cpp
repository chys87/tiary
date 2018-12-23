// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2018, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#include "common/signal.h"

namespace tiary {

namespace detail {

SignalRecursive::~SignalRecursive ()
{
}

void SignalRecursive::emit ()
{
	obj.emit ();
}

SignalRecursive *SignalRecursive::copy () const
{
	return new SignalRecursive (obj);
}

bool SignalRecursive::is_really_connected() const {
	return obj.is_really_connected();
}



SignalGroup::~SignalGroup ()
{
}

void SignalGroup::emit ()
{
	for (Signal &signal: obj_) {
		signal.emit ();
	}
}

SignalGroup *SignalGroup::copy () const
{
	return new SignalGroup(obj_);
}

} // namespace detail

bool Signal::is_really_connected () const
{
	const detail::SignalBase *p = f_.get();
	return (p && p->is_really_connected());
}

} // namespace tiary
