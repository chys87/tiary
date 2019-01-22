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

Condition::Condition(const Condition &other)
	: info_(other.info_ ? other.info_->copy() : nullptr) {
}

void Condition::assign(const Condition &other) {
	if (this != &other) {
		info_.reset(other.info_ ? other.info_->copy() : nullptr);
	}
}

namespace detail {

CondNot::~CondNot ()
{
}

bool CondNot::call() const {
	return !obj_.call();
}

CondNot *CondNot::copy () const
{
	return new CondNot(obj_);
}

CondAnd::~CondAnd ()
{
}

bool CondAnd::call() const {
	return (a_.call() && b_.call());
}

CondAnd *CondAnd::copy () const
{
	return new CondAnd(a_, b_);
}

CondOr::~CondOr ()
{
}

bool CondOr::call() const {
	return (a_.call() || b_.call());
}

CondOr *CondOr::copy () const
{
	return new CondOr(a_, b_);
}

} // namespace tiary::detail

} // namespace tiary
