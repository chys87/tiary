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

void Signal::copy_from (const Signal &sig)
{
	if (this != &sig) {
		delete info;
		info = sig.info ? sig.info->copy () : 0;
	}
}

bool Signal::is_really_connected () const
{
	const detail::SignalBase *p = info;
	while (const detail::SignalRecursive *rec = dynamic_cast <const detail::SignalRecursive *> (p)) {
		p = rec->obj.info;
	}
	if (const detail::SignalGroup *grp = dynamic_cast <const detail::SignalGroup *> (p)) {
		for (const Signal &signal: *grp) {
			if (signal.is_really_connected ()) {
				return true;
			}
		}
		return false;
	}
	return p;
}

} // namespace tiary
