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


#ifndef TIARY_COMMON_NONCOPYABLE_H
#define TIARY_COMMON_NONCOPYABLE_H

/**
 * @file	common/noncopyable.h
 * @author	chys <admin@chys.info>
 * @brief	Defines classes tiary::noncopyable and tiary::nonassignable
 */


namespace tiary {


/**
 * @brief	Makes a class noncopyable
 *
 * Any class deriving from this class cannot be copied or copy-assigned.
 */
class noncopyable
{
#ifdef TIARY_HAVE_DEFAULT_DELETE_FUNCTIONS
protected:
	noncopyable (const noncopyable &) = delete;
	void operator = (const noncopyable &) = delete;
	noncopyable () = default;
	~noncopyable () = default;
#else
private:
	noncopyable (const noncopyable &);
	void operator = (const noncopyable &);
protected:
	noncopyable () {}
	~noncopyable () {}
#endif
};


/**
 * @brief	Makes a class nonassignable
 *
 * Any class deriving from this class cannot be copy-assigned (can be copied).
 */
class nonassignable
{
#ifdef TIARY_HAVE_DEFAULT_DELETE_FUNCTIONS
protected:
	nonassignable &operator = (const nonassignable &) = delete;
	nonassignable () = default;
	nonassignable (const nonassignable &) = default;
	~nonassignable () = default;
#else
private:
	nonassignable &operator = (const nonassignable &);
protected:
	nonassignable () {}
	nonassignable (const nonassignable &) {}
	~nonassignable () {}
#endif
};

} // namespace tiary

#endif // Include guard
