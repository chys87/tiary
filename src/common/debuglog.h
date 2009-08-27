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


#ifndef TIARY_UI_DEBUGLOG_H
#define TIARY_UI_DEBUGLOG_H

#include <stdarg.h>

namespace tiary {
namespace debug {

void vdebug_log (const char *file, int line, const char *func, const char *format, va_list ap);
void debug_log (const char *file, int line, const char *func, const char *format, ...);
void debug_log2 (const char *format, ...);

} // namespace tiary::debug
} // namespace tiary

#if !defined NDEBUG
# ifdef __GNUC__
#  define DEBUG_LOG(s...) ::tiary::debug::debug_log(__FILE__,__LINE__,__func__,s)
# elif __STDC_VERSION >= 199901L
#  define DEBUG_LOG(...) ::tiary::debug::debug_log(__FILE__,__LINE__,__func__,__VA_ARGS__)
# else
#  define DEBUG_LOG ::tiary::debug::debug_log2
# endif
#else
inline void DEBUG_LOG(...) {}
#endif

#endif // Include guard
