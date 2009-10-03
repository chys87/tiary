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


#include "ui/terminal_emulator.h"
#include "common/unicode.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>

namespace tiary {
namespace ui {


namespace {

/**
 * @brief	Detect the terminal emulator
 *
 * This function probably works only on Linux, but that's good enough..
 */
TerminalEmulator detect_terminal_emulator ()
{
	const char *term_env = getenv ("TERM");
	if (term_env) {
		if (strcmp (term_env, "rxvt-unicode") == 0) {
			return RXVT_UNICODE;
		}
		if (strcmp (term_env, "Eterm") == 0) {
			return ETERM;
		}
		if (memcmp (term_env, "screen", 6) == 0) {
			return UNKNOWN_TERMINAL;
		}
	}

	char bufa[256];
	char bufb[256];
	pid_t pid = getpid ();
	do {
		sprintf (bufa, "/proc/%u/exe", unsigned (pid));
		ssize_t l = readlink (bufa, bufb, sizeof bufb - 1);
		if (l <= 0) {
			// Cannot read /proc/pid/exe. Try cmdline
			sprintf (bufa, "/proc/%u/cmdline", unsigned (pid));
			int fd = open (bufa, O_RDONLY);
			if (fd < 0) { // Still failed. Return unknown
				break;
			}
			l = read (fd, bufb, sizeof bufb - 1);
			close (fd);
			if (l <= 0) {
				break;
			}
		}
		bufb[l] = '\0';
		const char *p = strrchr (bufb, '/');
		if (p == 0) {
			p = bufb;
		}
		else {
			++p;
		}

		if (strcmp (p, "gnome-terminal") == 0) {
			return VTE;
		}
		if (strcmp (p, "konsole") == 0) {
			return KONSOLE;
		}
		// May be "kdeinit", "kdeinit4", etc..
		if (memcmp (p, "kdeinit", 7) == 0) {
			return KONSOLE;
		}
		if (strcmp (p, "xterm") == 0) {
			return TRUE_XTERM;
		}
		if (strcmp (p, "mlterm") == 0) {
			return MLTERM;
		}
		if (strcmp (p, "zhcon") == 0) {
			return ZHCON;
		}

		// Attempt to get PPID of PID
		sprintf (bufa, "/proc/%u/status", unsigned (pid));
		int fd = open (bufa, O_RDONLY);
		if (fd < 0) {
			break;
		}
		l = read (fd, bufa, sizeof bufa - 1);
		close (fd);
		if (l < 0) {
			break;
		}
		bufa[l] = '\0';
		p = strstr (bufa, "PPid:");
		if (p == 0) {
			break;
		}
		pid = (pid_t) strtoul (p+5, 0, 10);

	} while (pid >= 2); // 0 = Nohting; 1 = init

	if (term_env && strcmp (term_env, "xterm") == 0) {
		return GENERAL_XTERM;
	}
	else if (term_env && strcmp (term_env, "linux") == 0) {
		return LINUX_CONSOLE;
	}
	else {
		return UNKNOWN_TERMINAL;
	}
}

bool is_locale_utf8 ()
{
	// Traditional Chinese "I love you" in UTF-8 and UCS-4
	return (mbs_to_wstring ("\xe6\x88\x91\xe6\x84\x9b\xe4\xbd\xa0")
			== L"\u6211\u611b\u4f60");
}

} // anonymous namespace


TerminalEmulator get_terminal_emulator ()
{
	static TerminalEmulator emulator = detect_terminal_emulator ();
	return emulator;
}


bool terminal_emulator_correct_wcwidth ()
{
	static bool is_utf8 = is_locale_utf8 ();
	// In non-Unicode locales there are more problems
	if (!is_utf8) {
		return false;
	}
	switch (get_terminal_emulator ()) {
		case LINUX_CONSOLE:
		case RXVT_UNICODE:
		case KONSOLE:
		case ETERM:
		case MLTERM:
			return true;
		default:
			return false;

	}
}


} // namespace ui
} // namespace tiary
