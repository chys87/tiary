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


#ifndef TIARY_COMMON_QUERY_H
#define TIARY_COMMON_QUERY_H

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

#include <list>
#include <utility> // std::forward

namespace tiary {

class Condition;

namespace detail {

struct CondBase
{
	virtual bool call (bool default_return) = 0;
	virtual CondBase *copy () const = 0;
	virtual ~CondBase () {}
};

template <typename C>
struct CondMV : CondBase
{
	C *obj;
	bool (C::*foo)();

	CondMV (C *obj_, bool (C::*foo_)()) : obj(obj_), foo(foo_) {}
	bool call (bool) { return (obj->*foo)(); }
	CondMV *copy () const { return new CondMV (obj, foo); }
};

template <typename C>
struct CondCMV : CondBase
{
	const C *obj;
	bool (C::*foo)() const;

	CondCMV (const C *obj_, bool (C::*foo_)() const) : obj(obj_), foo(foo_) {}
	bool call (bool) { return (obj->*foo)(); }
	CondCMV *copy () const { return new CondCMV (obj, foo); }
};

} // namespace detail

class Condition
{

	detail::CondBase *info;

public:

	~Condition () { delete info; }
	Condition () : info (0) {}
	Condition (const Condition &other) : info (other.info ? other.info->copy () : 0) {}
	Condition &operator = (const Condition &other);
#ifdef TIARY_HAVE_RVALUE_REFERENCES
	Condition (Condition &&other) : info (other.info) { other.info = 0; }
	Condition &operator = (Condition &&other) { swap (other); return *this; }
#endif

#ifdef TIARY_HAVE_RVALUE_REFERENCES
	void swap (Condition &&other)
#else
	void swap (Condition &other)
#endif
	{
		detail::CondBase *tmp = info;
		info = other.info;
		other.info = tmp;
	}

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
#ifdef TIARY_HAVE_RVALUE_REFERENCES
	friend Condition operator ! (Condition &&);
	friend Condition operator && (const Condition &, Condition &&);
	friend Condition operator && (Condition &&, const Condition &);
	friend Condition operator && (Condition &&, Condition &&);
	friend Condition operator || (const Condition &, Condition &&);
	friend Condition operator || (Condition &&, const Condition &);
	friend Condition operator || (Condition &&, Condition &&);
#endif
};

namespace detail {

struct CondNot : public CondBase
{
	Condition obj;

	explicit CondNot (const Condition &o) : obj(o) {}
#ifdef TIARY_HAVE_RVALUE_REFERENCES
	explicit CondNot (Condition &&o) : obj(std::move (o)) {}
#endif
	~CondNot ();
	bool call (bool);
	CondNot *copy () const;
};

#define TIARY_COND_BIN_CLASS_HEADER(cname) \
	struct cname : public CondBase {\
		Condition obja;\
		Condition objb;\
		cname (const Condition &oa, const Condition &ob)\
			: obja (oa), objb (ob) {}\

#define TIARY_COND_BIN_CLASS_FOOTER(cname) \
		~cname ();\
		bool call (bool);\
		cname *copy () const;\
	};

#ifdef TIARY_HAVE_RVALUE_REFERENCES
# define TIARY_COND_BIN_CLASS(cname) \
		TIARY_COND_BIN_CLASS_HEADER(cname)\
		cname (const Condition &oa, Condition &&ob) : obja(oa), objb(std::move(ob)) {}\
		cname (Condition &&oa, const Condition &ob) : obja(std::move(oa)), objb(ob) {}\
		cname (Condition &&oa, Condition &&ob) : obja(std::move(oa)), objb(std::move(ob)) {}\
		TIARY_COND_BIN_CLASS_FOOTER(cname)
#else
# define TIARY_COND_BIN_CLASS(cname) \
		TIARY_COND_BIN_CLASS_HEADER(cname)\
		TIARY_COND_BIN_CLASS_FOOTER(cname)
#endif

TIARY_COND_BIN_CLASS(CondAnd)
TIARY_COND_BIN_CLASS(CondOr)

#undef TIARY_COND_BIN_CLASS_HEADER
#undef TIARY_COND_BIN_CLASS_FOOTER
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

#ifdef TIARY_HAVE_RVALUE_REFERENCES
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
#endif

} // namespace tiary

#endif // Include guard
