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

/**
 * @file	common/debuglog.h
 * @author	chys <admin@chys.info>
 * @brief	Implements utilities helpful to debugging
 */

#ifndef NDEBUG

#include "common/debuglog.h"
#include <stdarg.h>
#include <stdio.h>

namespace tiary {
namespace debug {

void vdebug_log (const char *filename, int line, const char *func, const char *format, va_list ap)
{
	static FILE *fp = fopen ("/tmp/tiary_debug.txt", "w");
	if (fp) {
		fprintf_unlocked (fp, "%s:%d:%s:", filename, line, func);
		vfprintf_unlocked (fp, format, ap);
		fputc_unlocked ('\n', fp);
		fflush_unlocked (fp);
	}
}

void debug_log (const char *filename, int line, const char *func, const char *format, ...)
{
	va_list ap;
	va_start (ap, format);
	vdebug_log (filename, line, func, format, ap);
	va_end (ap);
}

void debug_log2 (const char *format, ...)
{
	va_list ap;
	va_start (ap, format);
	vdebug_log (0, 0, 0, format, ap);
	va_end (ap);
}



} // namespace tiary::debug
} // namespace tiary

#endif // !NDEBUG
