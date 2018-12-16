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
 * @file	common/external.cpp
 * @author	chys <admin@chys.info>
 * @brief	Implements functions declared in common/external.h
 */

#include "common/external.h"
#include "common/misc.h"
#include "common/dir.h"
#include "common/string.h"
#include <string>
#include <list>
#include <stdlib.h>


namespace tiary {

namespace {

std::string make_external_command_line (const char *prog, const char *extra_param)
{
	/*
	 * We do not use fork-exec ourselves.
	 * There are a lot of details out there (signals)
	 * Instead, we generate a command and use system.
	 */

	std::string command = "false";

	// Split prog into tokens deliminated by pipe strings
	std::vector<std::string> progs = split_string(prog, '|');
	for (auto it = progs.begin(); it != progs.end(); ++it) {
		environment_expand (*it);
		strip (*it);
		if (it->empty ()) {
			continue;
		}
		const char *p = it->c_str ();
		size_t tokenlen = strchrnul (p, ' ') - p;
		if (!find_executable (std::string (p, tokenlen)).empty ()) {
			command = *it + ' ' + extra_param;
			break;
		}
	}
	return command;
}

} // anonymous namespace



int call_external_program_system (const char *prog, const char *extra_param)
{
	return system (make_external_command_line (prog, extra_param).c_str ());
}

FILE *call_external_program_popen (const char *prog, const char *extra_param, const char *type)
{
	return popen (make_external_command_line (prog, extra_param).c_str (), type);
}






} // namespace tiary
