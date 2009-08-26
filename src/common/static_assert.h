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


#ifndef TIARY_COMMON_STATIC_ASSERT_H
#define TIARY_COMMON_STATIC_ASSERT_H

/**
 * @file	common/static_assert.h
 * @author	chys <admin@chys.info>
 * @brief	Defines macro TIARY_STATIC_ASSERT
 *
 * Static assertions are used to detect errors at compile time.
 * If the compiler itself supports static assertions (a C++0x feature), we use it.
 */

// Static assertion
// Generate a compilation error if the condition (compile-time constant) is not satisfied

#ifdef TIARY_HAVE_STATIC_ASSERT // Use built-in static assertions

# define TIARY_STATIC_ASSERT static_assert

#else

// This is probably the simplest implementation and SHOULD work with any compiler
# define TIARY_STATIC_ASSERT(c,info) TIARY_STATIC_ASSERT_IMPL(__LINE__,c)
# define TIARY_STATIC_ASSERT_IMPL(n,c) TIARY_STATIC_ASSERT_IMPL2(n,c)
# define TIARY_STATIC_ASSERT_IMPL2(n,c)						\
		typedef int static_assert_type_##n[(c) ? 1 : -1];

#endif


#endif // include guard
