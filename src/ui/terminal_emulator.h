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
	TRUE_XTERM,          // _true_ xterm
	GENERAL_XTERM,       // Too many f**king terminal emulators say they are xterm
	UNKNOWN_TERMINAL     // Unknown
};

TerminalEmulator get_terminal_emulator ();


} // namespace ui
} // namespace tiary

#endif // include guard
