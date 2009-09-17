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
	for (std::list<Signal>::iterator it = obj.begin ();  it != obj.end (); ++it) {
		it->emit ();
	}
}

SignalGroup *SignalGroup::copy () const
{
	return new SignalGroup (obj);
}

} // namespace detail

Signal::~Signal ()
{
	delete info;
}

bool Signal::is_really_connected () const
{
	detail::SignalBase *p = info;
	while (detail::SignalRecursive *rec = dynamic_cast <detail::SignalRecursive *> (p)) {
		p = rec->obj.info;
	}
	if (detail::SignalGroup *grp = dynamic_cast <detail::SignalGroup *> (p)) {
		for (std::list<Signal>::const_iterator it = grp->obj.begin ();
				it != grp->obj.end (); ++it) {
			if (it->is_really_connected ()) {
				return true;
			}
		}
		return false;
	}
	return p;
}

} // namespace tiary
