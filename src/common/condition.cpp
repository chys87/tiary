// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2018, 2019, chys <admin@CHYS.INFO>
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
		info_.reset(other.info_ ? other.info_->copy() : nullptr);
	}
	return *this;
}

namespace detail {

CondNot::~CondNot ()
{
}

bool CondNot::call(bool default_return) const {
	return !obj_.call(!default_return);
}

CondNot *CondNot::copy () const
{
	return new CondNot(obj_);
}

CondAnd::~CondAnd ()
{
}

bool CondAnd::call(bool default_return) const {
	if (!a_.call(default_return)) {
		return false;
	}
	return b_.call(default_return);
}

CondAnd *CondAnd::copy () const
{
	return new CondAnd(a_, b_);
}

CondOr::~CondOr ()
{
}

bool CondOr::call(bool default_return) const {
	if (a_.call(default_return)) {
		return true;
	}
	return b_.call(default_return);
}

CondOr *CondOr::copy () const
{
	return new CondOr(a_, b_);
}

} // namespace tiary::detail

} // namespace tiary
