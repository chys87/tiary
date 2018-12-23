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


#ifndef TIARY_COMMON_SIGNAL_H
#define TIARY_COMMON_SIGNAL_H

/**
 * @file	common/signal.h
 * @author	chys <admin@chys.info>
 * @brief	Declares the class tiary::Signal
 */

#include <memory>
#include <utility> // std::forward
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

template <typename R> struct SignalNV : SignalBase
{
	R (*fun)();
	SignalNV (R (*f)()) : fun(f) {}
	void emit () { (*fun)(); }
	SignalNV *copy () const { return new SignalNV(fun); }
};
template <typename R, typename T> struct SignalN1 : SignalBase
{
	R (*fun)(T);
	T arg;
	SignalN1 (void (*f)(T), T a) : fun(f), arg(a) {}
	void emit () { (*fun)(arg); }
	SignalN1 *copy () const { return new SignalN1(fun,arg); }
};
template <typename R, typename C> struct SignalMV : SignalBase
{
	C *obj;
	R (C::*fun)();
	SignalMV (C *o, void (C::*f)()) : obj(o), fun(f) {}
	void emit () { (obj->*fun)(); }
	SignalMV *copy () const { return new SignalMV(obj, fun); }
};
template <typename R, typename C, typename T> struct SignalM1 : SignalBase
{
	C *obj;
	void (C::*fun)(T);
	T arg;
	SignalM1 (C *o, void (C::*f)(T), T a) : obj(o), fun(f), arg(a) {}
	void emit () { (obj->*fun)(arg); }
	SignalM1 *copy () const { return new SignalM1(obj, fun, arg); }
};
template <typename R, typename C, typename T1, typename T2> struct SignalM2 : SignalBase
{
	C *obj;
	R (C::*fun)(T1,T2);
	T1 arg1;
	T2 arg2;
	SignalM2 (C *o, R (C::*f)(T1,T2), T1 a, T2 b) : obj(o), fun(f), arg1(a), arg2(b) {}
	void emit () { (obj->*fun)(arg1,arg2); }
	SignalM2 *copy () const { return new SignalM2(obj, fun, arg1, arg2); }
};
// Connect to another signal
struct SignalRecursive : SignalBase
{
	Signal &obj;
	SignalRecursive (Signal &sig) : obj(sig) {}
	~SignalRecursive ();
	void emit ();
	SignalRecursive *copy () const;
	bool is_really_connected() const override;
};

// Cannot be defined until Signal is a complete type
struct SignalGroup;

} // namespace detail

/**
 * @brief	Class for a "signal"
 *
 * This class simulates Qt's singal-slot mechanism. Though much less powerful,
 * it still can be very handy.
 *
 * Unfortunately variadic templates (a C++0x feature) are not widely available,
 * otherwise we can make it really as powerful as a Qt signal - even more
 * powerful and handier.
 *
 * Main differences from Qt signals:
 *  -* Need no special preprocessing (moc);
 *  -* Signals cannot have parameters (can be easily extended
 *     when C++0x's varidic templatets feature is widely available);
 *  -* One signal cannot be connected to more than one slots;
 *  -* Slots are ordinary functions (member or non-member;
 *      optionally with one context parameter)
 *
 * 
 */
class Signal
{
public:
	Signal() = default;
	Signal(Signal &&) = default;
	Signal &operator = (Signal &&sig) = default;

	template<typename R> explicit Signal (R (*f)()) :
		f_(new detail::SignalNV<R>(f)) {}
	template<typename R, typename T, typename Ta> Signal (R (*f)(T), Ta a) :
		f_(new detail::SignalN1<R,T>(f,a)) {}
	template<typename R, typename C, typename D> Signal (C &o, R (D::*f)()) :
		f_(new detail::SignalMV<R,D>(&o, f)) {}
	template<typename R, typename C, typename D> Signal (C *o, R (D::*f)()) :
		f_(new detail::SignalMV<R,D>(o, f)) {}
	template<typename R, typename C, typename D, typename T, typename Ta> Signal (C &o, R (D::*f)(T), Ta a) :
		f_(new detail::SignalM1<R, D,T>(&o, f, a)) {}
	template<typename R, typename C, typename D, typename T, typename Ta> Signal (C *o, R (D::*f)(T), Ta a) :
		f_(new detail::SignalM1<R, D,T>(o, f, a)) {}
	template<typename R, typename C, typename D, typename T1, typename T2, typename Ta, typename Tb> Signal (C &o, R (D::*f)(T1,T2), Ta a, Tb b)
		: f_(new detail::SignalM2<R, D, T1, T2>(&o, f, a, b)) {}
	template<typename R, typename C, typename D, typename T1, typename T2, typename Ta, typename Tb> Signal (C *o, R (D::*f)(T1,T2), Ta a, Tb b)
		: f_(new detail::SignalM2<R, D, T1, T2>(o, f, a, b)) {}
	Signal(const Signal &sig) : f_(sig.f_ ? sig.f_->copy() : nullptr) {}
	// Note the second parameter.
	Signal(Signal &sig, int) : f_(new detail::SignalRecursive(sig)) {}
	Signal(Signal *sig, int) : f_(new detail::SignalRecursive(*sig)) {}
	// Connect to a group of signals
	Signal(const std::vector<Signal> &);

	template<typename R> void connect(R (*f)()) {
		f_.reset(new detail::SignalNV<R>(f));
	}
	template<typename R, typename T, typename Ta> void connect (R (*f)(T), Ta a) {
		f_.reset(new detail::SignalN1<R,T>(f,a));
	}
	template<typename R, typename C, typename D> void connect (C &o, R (D::*f)()) {
		f_.reset(new detail::SignalMV<R,D>(&o, f));
	}
	template<typename R, typename C, typename D> void connect (C *o, R (D::*f)()) {
		f_.reset(new detail::SignalMV<R,D>(o, f));
	}
	template<typename R, typename C, typename D, typename T, typename Ta> void connect (C &o, R (D::*f)(T), Ta a) {
		f_.reset(new detail::SignalM1<R,D,T>(&o, f, a));
	}
	template<typename R, typename C, typename D, typename T, typename Ta> void connect (C *o, R (D::*f)(T), Ta a) {
		f_.reset(new detail::SignalM1<R,D,T>(o, f, a));
	}
	template<typename R, typename C, typename D, typename T1, typename T2, typename Ta, typename Tb> void connect (C &o, R (D::*f)(T1,T2), Ta a, Tb b) {
		f_.reset(new detail::SignalM2<R,D,T1,T2>(&o, f, a, b));
	}
	template<typename R, typename C, typename D, typename T1, typename T2, typename Ta, typename Tb> void connect (C *o, R (D::*f)(T1,T2), Ta a, Tb b) {
		f_.reset(new detail::SignalM2<R,D,T1,T2>(o, f, a, b));
	}
	// Connect to another Signal
	void connect (Signal &sig) {
		if (this != &sig) {
			f_.reset(new detail::SignalRecursive(sig));
		}
	}
	// Connect to a list of Signals
	void connect(const std::vector<Signal> &);

	Signal &operator = (const Signal &sig) { f_.reset(sig.f_ ? sig.f_->copy() : nullptr); return *this; }

	Signal(std::vector<Signal> &&);
	void connect(std::vector<Signal> &&);

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


namespace detail {

// Connect to a group of other signals. The order is guaranteed
// This class must be defined after Signal is complete
class SignalGroup : public SignalBase {
public:
	SignalGroup(const std::vector<Signal> &lst) : obj_(lst) {}
	~SignalGroup ();
	void emit ();
	SignalGroup *copy () const;
	SignalGroup(std::vector<Signal> &&lst) : obj_(std::move(lst)) {}

	auto begin() const { return obj_.begin(); }
	auto end() const { return obj_.end(); }

	bool is_really_connected() const override {
		for (const Signal &sig: obj_) {
			if (sig.is_really_connected())
				return true;
		}
		return false;
	}

private:
	std::vector<Signal> obj_;
};

} // namespace detail

inline Signal::Signal(const std::vector<Signal> &lst)
	: f_(new detail::SignalGroup(lst)) {
}

inline void Signal::connect(const std::vector<Signal> &lst) {
	f_.reset(new detail::SignalGroup(lst));
}

inline Signal::Signal(std::vector<Signal> &&lst)
	: f_(new detail::SignalGroup(std::move(lst))) {
}

inline void Signal::connect(std::vector<Signal> &&lst) {
	f_.reset(new detail::SignalGroup(std::move(lst)));
}

} // namespace tiary

#endif // Include guard
