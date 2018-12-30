// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2016, 2018, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#ifndef TIARY_COMMON_CONDITION_H
#define TIARY_COMMON_CONDITION_H

/**
 * @file	common/condition.h
 * @author	chys <admin@chys.info>
 * @brief	Declares the class tiary::Condition
 *
 * A class used to test a condition - true or false.
 *
 * We use it to check the current usability of functions
 * (menu items, buttons, etc.)
 */

#include <memory>
#include <list>
#include <utility> // std::forward

namespace tiary {

class Condition;

namespace detail {

class CondBase {
public:
	virtual bool call(bool default_return) const = 0;
	virtual CondBase *copy () const = 0;
	virtual ~CondBase () {}
};

template <typename C>
class CondMV final : public CondBase {
public:
	CondMV(C *obj, bool (C::*foo)()) : obj_(obj), foo_(foo) {}
	bool call(bool) const override { return (obj_->*foo_)(); }
	CondMV *copy() const override { return new CondMV(obj_, foo_); }

private:
	C *obj_;
	bool (C::*foo_)();
};

template <typename C>
class CondCMV final : public CondBase {
public:
	CondCMV(const C *obj, bool (C::*foo)() const) : obj_(obj), foo_(foo) {}
	bool call(bool) const override { return (obj_->*foo_)(); }
	CondCMV *copy() const override { return new CondCMV(obj_, foo_); }

private:
	const C *obj_;
	bool (C::*foo_)() const;
};

} // namespace detail

class Condition
{
public:
	Condition() = default;
	Condition(const Condition &other) : info (other.info ? other.info->copy() : nullptr) {}
	Condition &operator = (const Condition &other);
	Condition(Condition &&other) = default;
	Condition &operator = (Condition &&other) = default;

	template <typename D> Condition (D &obj, bool (D::*foo)()) : info (new detail::CondMV <D> (&obj, foo)) {}
	template <typename D> Condition (D *obj, bool (D::*foo)()) : info (new detail::CondMV <D> (obj, foo)) {}
	template <typename D> Condition (const D &obj, bool (D::*foo)() const) : info (new detail::CondCMV <D> (&obj, foo)) {}
	template <typename D> Condition (const D *obj, bool (D::*foo)() const) : info (new detail::CondCMV <D> (obj, foo)) {}

	bool call (bool default_return) const { return (info ? info->call (default_return) : default_return); }

private:
	explicit Condition (detail::CondBase *ptr) : info (ptr) {}

	friend Condition operator ! (const Condition &);
	friend Condition operator && (const Condition &, const Condition &);
	friend Condition operator || (const Condition &, const Condition &);
	friend Condition operator ! (Condition &&);
	friend Condition operator && (const Condition &, Condition &&);
	friend Condition operator && (Condition &&, const Condition &);
	friend Condition operator && (Condition &&, Condition &&);
	friend Condition operator || (const Condition &, Condition &&);
	friend Condition operator || (Condition &&, const Condition &);
	friend Condition operator || (Condition &&, Condition &&);

private:
	std::unique_ptr<detail::CondBase> info;
};

namespace detail {

class CondNot final : public CondBase {
public:
	explicit CondNot(const Condition &o) : obj_(o) {}
	explicit CondNot(Condition &&o) : obj_(std::move (o)) {}
	~CondNot ();
	bool call(bool) const override;
	CondNot *copy() const override;

private:
	Condition obj_;
};

#define TIARY_COND_BIN_CLASS(cname) \
	struct cname : public CondBase {\
		Condition obja_;\
		Condition objb_;\
		cname (const Condition &oa, const Condition &ob)\
			: obja_(oa), objb_(ob) {}\
		cname(const Condition &oa, Condition &&ob) : obja_(oa), objb_(std::move(ob)) {}\
		cname(Condition &&oa, const Condition &ob) : obja_(std::move(oa)), objb_(ob) {}\
		cname(Condition &&oa, Condition &&ob) : obja_(std::move(oa)), objb_(std::move(ob)) {}\
		~cname ();\
		bool call(bool) const override;\
		cname *copy () const override;\
	};

TIARY_COND_BIN_CLASS(CondAnd)
TIARY_COND_BIN_CLASS(CondOr)

#undef TIARY_COND_BIN_CLASS

} // namespace detail

inline Condition operator ! (const Condition &obj)
{
	return Condition (new detail::CondNot (obj));
}

inline Condition operator && (const Condition &obja, const Condition &objb)
{
	return Condition (new detail::CondAnd (obja, objb));
}

inline Condition operator || (const Condition &obja, const Condition &objb)
{
	return Condition (new detail::CondOr (obja, objb));
}

inline Condition operator ! (Condition &&obj)
{
	return Condition (new detail::CondNot (std::move (obj)));
}

inline Condition operator && (const Condition &obja, Condition &&objb)
{
	return Condition (new detail::CondAnd (obja, std::move (objb)));
}

inline Condition operator && (Condition &&obja, const Condition &objb)
{
	return Condition (new detail::CondAnd (std::move (obja), objb));
}

inline Condition operator && (Condition &&obja, Condition &&objb)
{
	return Condition (new detail::CondAnd (std::move (obja), std::move (objb)));
}

inline Condition operator || (const Condition &obja, Condition &&objb)
{
	return Condition (new detail::CondOr (obja, std::move (objb)));
}

inline Condition operator || (Condition &&obja, const Condition &objb)
{
	return Condition (new detail::CondOr (std::move (obja), objb));
}

inline Condition operator || (Condition &&obja, Condition &&objb)
{
	return Condition (new detail::CondOr (std::move (obja), std::move (objb)));
}

} // namespace tiary

#endif // Include guard
