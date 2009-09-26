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
 * @file	main.cpp
 * @author	chys <admin@chys.info>
 * @brief	Implements the main function of this program
 */

#include "ui/ui.h"
#include "common/unicode.h"
#include "main/mainui.h"
#include <string.h>
#include <stdlib.h>
#include <locale>
#include <stdio.h>
#include <signal.h>

using namespace tiary;

namespace {

void parse_options (int &, char **&);

} // anonymous namespace


int main (int argc, char **argv)
{
	std::locale::global (std::locale (""));

	signal (SIGPIPE, SIG_IGN);

	parse_options (argc, argv);
	if (argc >= 3) {
		fputs ("Too many arguments.\n", stderr);
		exit (EXIT_FAILURE);
	}

	tiary::ui::init ();
	tiary::ui::set_mouse_status (true);

	std::wstring filename;
	if (argc >= 2) {
		filename = mbs_to_wstring (argv[1]);
	}
	return main_body (filename);
}

namespace {

void option_version ()
{
	fprintf_unlocked (stderr, "\
Tiary %u.%u.%u\n\
\n\
\n\
Tiary, a terminal-based diary keeping system for Unix-like systems\n\
Copyright (C) %u, chys <admin@CHYS.INFO>\n\
\n\
This software is licensed under the 3-clause BSD license.\n\
See LICENSE in the source package and/or online info for details.\n",
		TIARY_MAJOR_VERSION,
		TIARY_MINOR_VERSION,
		TIARY_PATCH_VERSION,
		TIARY_COPYRIGHT_YEAR);
	exit (EXIT_SUCCESS);
}

void option_help ()
{
	fputs_unlocked (
			"tiary [filename]\n",
			stderr);
	exit (EXIT_SUCCESS);
}

void option_unknown_long (const char *s)
{
	fprintf_unlocked (stderr, "Unknown option - --%s\n", s);
	exit (EXIT_FAILURE);
}

void option_unknown_short (int c)
{
	fprintf_unlocked (stderr, "Unknown option - -%c\n", c);
	exit (EXIT_FAILURE);
}

// We do not use getopt_long because it's GNU specific
void parse_options (int &pargc, char **&pargv)
{
	int argc = pargc;
	char **argv = pargv;
	int wi = 1; // Writing point

	for (int i=1; i<argc; ++i) {
		char *arg = argv[i];
		if (arg[0] != '-') { // Not an option
			argv[wi++] = arg;
		}
		else if (arg[1] == '-') { // Long option
			if (arg[2] == '\0') {
				// Bare "--". Ending options. Copy the remainders verbatim
				while (++i < argc) {
					argv[wi++] = argv[i];
				}
				break;
			}
			arg += 2;
			if (!strcmp (arg, "version")) {
				option_version ();
			}
			else if (!strcmp (arg, "help")) {
				option_help ();
			}
			else {
				option_unknown_long (arg);
			}
		}
		else { // Short options
			while (int c = *++arg) {
				switch (c) {
					case 'v':
						option_version ();
						break;
					case 'h':
						option_help ();
						break;
					default:
						option_unknown_short (c);
						break;
				}
			}
		}
	}
	argv[wi] = 0;
	pargc = wi;
}

} // Anonymous namespace
