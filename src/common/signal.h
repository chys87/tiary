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


#ifndef TIARY_COMMON_SIGNAL_H
#define TIARY_COMMON_SIGNAL_H

/**
 * @file	common/signal.h
 * @author	chys <admin@chys.info>
 * @brief	Declares the class tiary::Signal
 */

#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace tiary {

class Signal;

namespace detail {

class SignalBase {
public:
	virtual void emit () = 0;
	virtual SignalBase *copy () const = 0;
	virtual ~SignalBase () {}
	virtual bool is_really_connected() const { return true; }
};

template <typename Callable, typename... T>
class SignalCallable final : public SignalBase {
public:
	constexpr SignalCallable(Callable f, T... args): f_(std::move(f)), args_(std::move(args)...) {}
	constexpr SignalCallable(Callable f, std::tuple<T...> args): f_(std::move(f)), args_(std::move(args)) {}
	void emit() override { std::apply(f_, args_); }
	SignalCallable *copy() const override { return new SignalCallable(f_, args_); }

private:
	Callable f_;
	std::tuple<T...> args_;
};

// Connect to another signal
class SignalRecursive final : public SignalBase {
public:
	constexpr SignalRecursive(Signal &sig) : sig_(sig) {}
	~SignalRecursive ();
	void emit() override;
	SignalRecursive *copy() const override;
	bool is_really_connected() const override;

private:
	Signal &sig_;
};

} // namespace detail

/**
 * @brief	Class for a "signal"
 *
 * This class simulates Qt's singal-slot mechanism. Though much less powerful,
 * it still can be very handy.
 * Thanks to C++11 variadic templates, we can have a neat and clean implementation.
 *
 * Main differences from Qt signals:
 *  -* Need no special preprocessing (moc);
 *  -* Signals cannot have parameters;
 *  -* One signal cannot be connected to more than one slots;
 *  -* Slots are ordinary functions or function objects.
 *
 */
class Signal
{
public:
	constexpr Signal() = default;
	Signal(Signal &&) = default;
	Signal &operator = (Signal &&sig) = default;

	template <typename R, typename...T, typename = typename std::result_of<R(T...)>::type>
	explicit Signal(R f, T... args) :
		f_(new detail::SignalCallable<R, T...>(std::move(f), std::move(args)...)) {}

	template<typename R, typename C, typename D, typename...T, typename = decltype((std::declval<C*>()->*std::declval<R D::*>())(std::declval<T>()...))>
	Signal(C &o, R D::*f, T... args) :
		f_(new detail::SignalCallable<decltype(std::mem_fn(f)), D*, T...>(std::mem_fn(f), &o, std::move(args)...)) {}

	template<typename R, typename C, typename D, typename...T, typename = decltype((std::declval<C*>()->*std::declval<R D::*>())(std::declval<T>()...))>
	Signal(C *o, R D::*f, T... args) :
		f_(new detail::SignalCallable<decltype(std::mem_fn(f)), D*, T...>(std::mem_fn(f), o, std::move(args)...)) {}

	Signal(const Signal &sig) : f_(sig.f_ ? sig.f_->copy() : nullptr) {}
	// Note the second parameter.
	Signal(Signal &sig, int) : f_(new detail::SignalRecursive(sig)) {}
	Signal(Signal *sig, int) : f_(new detail::SignalRecursive(*sig)) {}

	template <typename R, typename... T>
	std::void_t<typename std::result_of<R(T...)>::type> connect(R f, T... args) {
		f_.reset(new detail::SignalCallable<R, T...>(std::move(f), std::move(args)...));
	}
	template<typename R, typename C, typename D, typename... T>
	auto connect(C &o, R D::*f, T... args) -> std::void_t<decltype((o.*f)(args...))> {
		f_.reset(new detail::SignalCallable<decltype(std::mem_fn(f)), C*, T...>(std::mem_fn(f), &o, std::move(args)...));
	}
	template<typename R, typename C, typename D, typename... T>
	auto connect(C *o, R D::*f, T... args) -> std::void_t<decltype((o->*f)(args...))> {
		f_.reset(new detail::SignalCallable<decltype(std::mem_fn(f)), C*, T...>(std::mem_fn(f), o, std::move(args)...));
	}

	// Connect to another Signal
	void connect (Signal &sig) {
		if (this != &sig) {
			f_.reset(new detail::SignalRecursive(sig));
		}
	}

	Signal &operator = (const Signal &sig) { f_.reset(sig.f_ ? sig.f_->copy() : nullptr); return *this; }

	// disconnect
	void disconnect () { f_.reset(); }
	// emit
	void emit () { if (f_) f_->emit(); }
	// Check if the signal is connected
	bool is_connected () const { return static_cast<bool>(f_); }
	// Check recursively if the signal is "really" connected
	bool is_really_connected () const;

private:
	std::unique_ptr<detail::SignalBase> f_;
};

} // namespace tiary

#endif // Include guard
