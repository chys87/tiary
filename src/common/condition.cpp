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


#include "common/condition.h"


namespace tiary {

Condition &Condition::operator = (const Condition &other)
{
	if (this != &other) {
		delete info;
		info = other.info ? other.info->copy () : 0;
	}
	return *this;
}

namespace detail {

CondNot::~CondNot ()
{
}

bool CondNot::call (bool default_return)
{
	return !obj.call (!default_return);
}

CondNot *CondNot::copy () const
{
	return new CondNot (obj);
}

CondAnd::~CondAnd ()
{
}

bool CondAnd::call (bool default_return)
{
	if (!obja.call (default_return)) {
		return false;
	}
	return objb.call (default_return);
}

CondAnd *CondAnd::copy () const
{
	return new CondAnd (obja, objb);
}

CondOr::~CondOr ()
{
}

bool CondOr::call (bool default_return)
{
	if (obja.call (default_return)) {
		return true;
	}
	return objb.call (default_return);
}

CondOr *CondOr::copy () const
{
	return new CondOr (obja, objb);
}

} // namespace tiary::detail

} // namespace tiary
