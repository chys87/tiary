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


#ifndef TIARY_UI_TERMINAL_EMULATOR_H
#define TIARY_UI_TERMINAL_EMULATOR_H

namespace tiary {
namespace ui {

enum TerminalEmulator
{
	LINUX_CONSOLE,       // Linux console
	RXVT_UNICODE,        // urxvt:   Unicode-friendly enough
	KONSOLE,             // Konsole: Unicode-friendly enough
	VTE,                 // vte:     Width from font engine
	ETERM,               // ??
	MLTERM,              // ??
	ZHCON,               // zhcon: Actually incompatible with ncursesw
	TRUE_XTERM,          // _true_ xterm
	GENERAL_XTERM,       // Too many f**king terminal emulators say they are xterm
	UNKNOWN_TERMINAL     // Unknown
};

TerminalEmulator get_terminal_emulator ();

/**
 * Returns whether the terminal emulator is likely to use wcwidth for character width
 */
bool terminal_emulator_correct_wcwidth ();

} // namespace ui
} // namespace tiary

#endif // include guard
