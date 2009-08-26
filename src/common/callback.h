// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the so-called 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#ifndef TIARY_COMMON_CALLBACK_H
#define TIARY_COMMON_CALLBACK_H

#include <functional>

namespace tiary {


template <typename Arg, typename Ret>
struct UnaryCallback : public std::unary_function <Arg,Ret>
{
	virtual Ret operator () (Arg) const = 0;
	virtual ~UnaryCallback () { }
};

template <typename Arg1, typename Arg2, typename Ret>
struct BinaryCallback : public std::binary_function <Arg1,Arg2,Ret>
{
	virtual Ret operator () (Arg1, Arg2) const = 0;
	virtual ~BinaryCallback () { }
};

template <typename Ret>
struct NoArgCallback
{
	typedef Ret result_type;
	virtual Ret operator () () const = 0;
	virtual ~NoArgCallback () { }
};

template <typename Arg, typename Ret, Ret Val>
struct ConstantUnaryCallback : public UnaryCallback <Arg,Ret>
{
	Ret operator () (Arg) const
	{
		return Val;
	}
};

template <typename Arg1, typename Arg2, typename Ret, Ret Val>
struct ConstantBinaryCallback : public BinaryCallback <Arg1,Arg2,Ret>
{
	Ret operator () (Arg1, Arg2) const
	{
		return Val;
	}
};

template <typename Ret, Ret Val>
struct ConstantNoArgCallback : public NoArgCallback <Ret>
{
	Ret operator () () const
	{
		return Val;
	}
};

/**
 * @brief	A wrapper for UnaryCallback
 *
 * In order to pass tiary::UnaryCallback or tiary::BinaryCallback
 * to a template function, a wrapper is often needed.
 *
 * For example, @c std::list<T>::sort(callback) would resolve to
 * @c std::list<T>::sort<tiary::UnaryCallback> instead of
 * @c std::list<T>::sort<const tiary::UnaryCallback &>.
 * tiary::UnaryCallback and tiary::BinaryCallback cannot be copied
 * or instantiated. So we need to "wrap" them using some copyable type.
 */
template <typename Arg, typename Ret>
struct UnaryCallbackWrapper : public std::unary_function <Arg, Ret>
{
	const UnaryCallback<Arg,Ret> &callback;
	explicit UnaryCallbackWrapper (const UnaryCallback<Arg, Ret> &c)
		: callback (c)
	{
	}
	UnaryCallbackWrapper (const UnaryCallbackWrapper &other)
		: callback (other.callback)
	{
	}
	Ret operator () (Arg arg) const
	{
		return callback (arg);
	}
};

/**
 * @brief	A wrapper for BinaryCallback
 *
 * Similar to tiary::UnaryCallbackWrapper
 */
template <typename Arg1, typename Arg2, typename Ret>
struct BinaryCallbackWrapper : public std::binary_function <Arg1, Arg2, Ret>
{
	const BinaryCallback<Arg1,Arg2,Ret> &callback;
	explicit BinaryCallbackWrapper (const BinaryCallback<Arg1, Arg2, Ret> &c)
		: callback (c)
	{
	}
	BinaryCallbackWrapper (const BinaryCallbackWrapper &other)
		: callback (other.callback)
	{
	}
	Ret operator () (Arg1 arg1, Arg2 arg2) const
	{
		return callback (arg1, arg2);
	}
};

/**
 * @brief	A wrapper for NoArgCallback
 *
 * Similar to tiary::UnaryCallbackWrapper
 */
template <typename Ret>
struct NoArgCallbackWrapper
{
	typedef Ret result_type;
	const NoArgCallback<Ret> &callback;
	explicit NoArgCallbackWrapper (const NoArgCallback<Ret> &c)
		: callback (c)
	{
	}
	NoArgCallbackWrapper (const NoArgCallbackWrapper &other)
		: callback (other.c)
	{
	}
	Ret operator () () const
	{
		return callback ();
	}
};

/**
 * @brief	"Wrap" a tiary::UnaryCallback object
 */
template <typename Arg, typename Ret>
UnaryCallbackWrapper<Arg,Ret> wrap (const UnaryCallback<Arg,Ret> &callback)
{
	return UnaryCallbackWrapper<Arg,Ret> (callback);
}

/**
 * @brief	"Wrap" a tiary::BinaryCallback object
 */
template <typename Arg1, typename Arg2, typename Ret>
BinaryCallbackWrapper<Arg1,Arg2,Ret> wrap (const BinaryCallback<Arg1,Arg2,Ret> &callback)
{
	return BinaryCallbackWrapper<Arg1,Arg2,Ret> (callback);
}

/**
 * @brief	"Wrap" a tiary::NoArgCallback object
 */
template <typename Ret>
NoArgCallbackWrapper<Ret> wrap (const NoArgCallback<Ret> &callback)
{
	return NoArgCallbackWrapper<Ret> (callback);
}

} // namespace tiary

#endif // Include guard
