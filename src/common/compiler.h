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


#ifndef TIARY_COMMON_COMPILER_H
#define TIARY_COMMON_COMPILER_H

/*
 * This file contains macros that help adapting to different compilers
 */

#ifdef __GNUG__
# define TIARY_ALIGNED(n) __attribute__((__aligned__(n)))
#else
# define TIARY_ALIGNED(n)
#endif


#endif // Include guard
