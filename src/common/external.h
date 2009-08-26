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


#ifndef TIARY_COMMON_EXTERNAL_H
#define TIARY_COMMON_EXTERNAL_H

#include <stdio.h>

/**
 * @file	diary/external.h
 * @author	chys <admin@chys.info>
 * @brief	Declarations for calling external programs
 */

namespace tiary {


/**
 * @brief	Call an external program, using system
 * @param	prog	The program to execute. Can be a set of applicable program delimited by pipe sign (|)
 * @param	extra_param	Extra parameters to be appended to prog
 * @result	Exit status of the executed program
 *
 * For example, prog="$EDITOR|vim|emacs -nw|~/my_editor", extra_param="a.txt"
 *
 * Commands will be attempted in the following order:
 * -# $EDITOR a.txt
 * -# vim a.txt
 * -# eamcs -nw a.txt
 * -# ~/my_editor a.txt
 */
int call_external_program_system (const char *prog, const char *extra_param);

/**
 * @brief	Call an external program, using popen
 *
 * Similar with call_external_program_system, but use popen. A third argument is used
 * as the second argument of popen
 */
FILE *call_external_program_popen (const char *prog, const char *extra_param, const char *type);

} // namespace tiary

#endif // include guard
