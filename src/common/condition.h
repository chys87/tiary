// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2016, 2018, 2019, chys <admin@CHYS.INFO>
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

#include <functional>
#include <memory>
#include <utility>
#include "common/type_identity.h"

namespace tiary {

namespace detail {

class CondBase {
public:
	virtual bool call() const = 0;
	virtual CondBase *copy () const = 0;
	virtual ~CondBase () {}
};

template <typename C, typename = typename std::enable_if<std::is_convertible<typename std::invoke_result<C>::type, bool>::value, void>::type>
class CondC final : public CondBase {
public:
	CondC(const C &callable) : callable_(callable) {}
	CondC(C &&callable) : callable_(callable) {}
	bool call() const override { return bool(callable_()); }
	CondC *copy() const override { return new CondC(callable_); }

private:
	C callable_;
};

} // namespace detail

class Condition
{
public:
	Condition() = default;
	Condition(const Condition &other) : info_(other.info_ ? other.info_->copy() : nullptr) {}
	Condition &operator = (const Condition &other);
	Condition(Condition &&other) = default;
	Condition &operator = (Condition &&other) = default;

	// Condition itself doesn't meet the requirement that it's callable, so we don't need to explicitly exclude it
	template <typename C, typename = typename std::enable_if<std::is_convertible<typename std::invoke_result<C>::type, bool>::value>::type>
	Condition(C &&callable) : info_(new detail::CondC(std::forward<C>(callable))) {}

	template <typename D>
		Condition(type_identity_t<D> &obj, bool (D::*foo)()) : info_(new detail::CondC(std::bind(std::mem_fn(foo), &obj))) {}
	template <typename D>
		Condition(type_identity_t<D> *obj, bool (D::*foo)()) : info_(new detail::CondC(std::bind(std::mem_fn(foo), obj))) {}
	template <typename D>
		Condition (const type_identity_t<D> &obj, bool (D::*foo)() const) : info_(new detail::CondC(std::bind(std::mem_fn(foo), &obj))) {}
	template <typename D>
		Condition (const type_identity_t<D> *obj, bool (D::*foo)() const) : info_(new detail::CondC(std::bind(std::mem_fn(foo), obj))) {}

	bool call() const { return (info_ ? info_->call() : true); }
	explicit operator bool() const { return static_cast<bool>(info_); }

	struct And {};
	struct Or {};
	struct Not {};
	static constexpr And AND{};
	static constexpr Or OR{};
	static constexpr Not NOT{};

	Condition(const Condition &a, And, const Condition &b);
	Condition(const Condition &a, And, Condition &&b);
	Condition(Condition &&a, And, const Condition &b);
	Condition(const Condition &a, Or, const Condition &b);
	Condition(const Condition &a, Or, Condition &&b);
	Condition(Condition &&a, Or, const Condition &b);
	Condition(Not, const Condition &);
	Condition(Not, Condition &&);

private:
	std::unique_ptr<detail::CondBase> info_;
};

namespace detail {

class CondNot final : public CondBase {
public:
	explicit CondNot(const Condition &o) : obj_(o) {}
	explicit CondNot(Condition &&o) : obj_(std::move (o)) {}
	~CondNot ();
	bool call() const override;
	CondNot *copy() const override;

private:
	Condition obj_;
};

class CondAnd : public CondBase {
public:
	CondAnd(const Condition &oa, const Condition &ob) : a_(oa), b_(ob) {}
	CondAnd(const Condition &oa, Condition &&ob) : a_(oa), b_(std::move(ob)) {}
	CondAnd(Condition &&oa, const Condition &ob) : a_(std::move(oa)), b_(ob) {}
	CondAnd(Condition &&oa, Condition &&ob) : a_(std::move(oa)), b_(std::move(ob)) {}
	~CondAnd();
	bool call() const override;
	CondAnd *copy () const override;

private:
	Condition a_;
	Condition b_;
};

class CondOr : public CondBase {
public:
	CondOr(const Condition &oa, const Condition &ob) : a_(oa), b_(ob) {}
	CondOr(const Condition &oa, Condition &&ob) : a_(oa), b_(std::move(ob)) {}
	CondOr(Condition &&oa, const Condition &ob) : a_(std::move(oa)), b_(ob) {}
	CondOr(Condition &&oa, Condition &&ob) : a_(std::move(oa)), b_(std::move(ob)) {}
	~CondOr();
	bool call() const override;
	CondOr *copy () const override;

private:
	Condition a_;
	Condition b_;
};

} // namespace detail

inline Condition::Condition(const Condition &a, And, const Condition &b) : info_(new detail::CondAnd(a, b)) {}
inline Condition::Condition(const Condition &a, And, Condition &&b) : info_(new detail::CondAnd(a, std::move(b))) {}
inline Condition::Condition(Condition &&a, And, const Condition &b) : info_(new detail::CondAnd(std::move(a), b)) {}
inline Condition::Condition(const Condition &a, Or, const Condition &b) : info_(new detail::CondOr(a, b)) {}
inline Condition::Condition(const Condition &a, Or, Condition &&b) : info_(new detail::CondOr(a, std::move(b))) {}
inline Condition::Condition(Condition &&a, Or, const Condition &b) : info_(new detail::CondOr(std::move(a), b)) {}
inline Condition::Condition(Not, const Condition &a) : info_(new detail::CondNot(a)) {}
inline Condition::Condition(Not, Condition &&a) : info_(new detail::CondNot(std::move(a))) {}

inline Condition operator ! (const Condition &obj) { return Condition(Condition::NOT, obj); }
inline Condition operator ! (Condition &&obj) { return Condition(Condition::NOT, std::move(obj)); }
inline Condition operator && (const Condition &obja, const Condition &objb) { return Condition(obja, Condition::AND, objb); }
inline Condition operator && (const Condition &obja, Condition &&objb) { return Condition(obja, Condition::AND, std::move(objb)); }
inline Condition operator && (Condition &&obja, const Condition &objb) { return Condition(std::move(obja), Condition::AND, objb); }
inline Condition operator || (const Condition &obja, const Condition &objb) { return Condition(obja, Condition::OR, objb); }
inline Condition operator || (const Condition &obja, Condition &&objb) { return Condition(obja, Condition::OR, std::move(objb)); }
inline Condition operator || (Condition &&obja, const Condition &objb) { return Condition(std::move(obja), Condition::OR, objb); }

} // namespace tiary

#endif // Include guard
