/***********************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2010, 2011, 2012, 2014, 2019,
 * chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **********************************************************************/

// Some distributions have GCC predefine _FORTIFY_SOURCE
#undef _FORTIFY_SOURCE
#define _FORTIFY_SOURCE 2

#define _FILE_OFFSET_BITS 64

#ifdef HAVE_FUNC_ATTRIBUTE_CONST
# define ATTRIBUTE_CONST __attribute__((const))
#else
# define ATTRIBUTE_CONST
#endif

#ifdef HAVE_FUNC_ATTRIBUTE_PURE
# define ATTRIBUTE_PURE __attribute__((pure))
#else
# define ATTRIBUTE_PURE
#endif
