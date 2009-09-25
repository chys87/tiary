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


#ifndef TIARY_UI_UI_H
#define TIARY_UI_UI_H

/**
 * @file	ui/ui.h
 * @author	chys <admin@chys.info>
 * @brief	Header for the fundamentals of the UI system
 *
 * For details on the UI system, see namespace @c tiary::ui
 */

/**
 * @defgroup uisystem The UI system
 * @brief Introduces the use of the UI system
 *
 * All staff related to user interface is place in the @c tiary::ui
 * namespace.
 *
 * First the UI system need be initialized. Before the initialization,
 * @e both the C and C++ locales must be properly set:
 * <pre>
 *       setlocale (LC_ALL, "");
 *       std::locale::global (std::locale (""));
 *       tiary::ui::init ();
 * </pre>
 * When everything is done, shut it down: <code>tiary::ui::finalize ();</code>.
 * @c tiary::ui::finalize is also registered to be automatically executed
 * at exit. So you can safely omit calling it.
 *
 * All I/O are done in "Windows."
 * We implement our own Window instead of directly wrapping ncurses windows.
 * Our program is intended to handle CJK characters as well as western.
 * Although ncursesw handles Unicode characters much better than ncurses,
 * there are still unaddressed issues, e.g. when we have a CJK character
 * at columns 0 and 1, and create a window starting from column 1.
 * (Similar situations are unavoidable.)
 *
 * Windows are object-oriented and event-driven. The only way to use it
 * is to derive from it, and overload the interface virtual functions.
 * The Window object provides an event loop. Once you run
 * tiary::ui::Window::event_loop(), it polls the keyboard and mouse
 * and calls the corresponding interface functions, not exiting
 * until a close request is received (to send a close request,
 * use tiary::ui::Window::request.)
 *
 * Colors are supported via global "Palettes." You should set the palettes
 * using tiary::ui::set_palette before doing any actual output.
 * Palettes are numbered from 0 to NUMBER_PALETTES-1.
 * Palette IDs are defined in file ui/paletteid.h.
 * Every palette defines a pair of forecolor and backcolor, and a set
 * of attributes.
 *
 * You can also force the colors and/or attributes, which overrides the
 * corresponding settings in palettes.
 *
 * It is preferrable to do I/O in Unicode, though we also have
 * some interfaces not in Unicode.
 *
 * To write to a Window, using tiary::ui::Window::put.
 * But more often, you should not do so, but rely on Controls
 *
 * For efficiency purposes, output functions only support printable
 * characters. It is the caller's responsibility to guarantee this.
 *
 * @{
 * @}
 */

namespace tiary {
/// @ingroup uisystem
namespace ui {

struct Size;

/**
 * @brief	Initialize the UI system
 * @result	@c true if and only if successful
 */
bool init ();
/**
 * @brief	Determines whether the UI system has already been initialized
 * @result	@c true if and only if the UI system has been successfully initialized
 */
bool initialized ();
/**
 * @brief	Get the current mouse supporting status
 */
bool get_mouse_status ();
/**
 * @brief	Enable or disable mouse supporting
 */
void set_mouse_status (bool);
/**
 * @brief	Enable or disable mouse supporting
 *
 * This function reverts the supporting status for mouse
 */
void toggle_mouse_status ();
/**
 * @brief	Closes the UI system
 *
 * If the programmer fails to explicitly call this function before the program
 * terminates, it is automatically called.
 */
void finalize ();

/**
 * @brief	Returns the size of the screen
 *
 * Note that in terminal emulators, the screen size can change from time to time.
 */
Size get_screen_size ();


/*
 * List of special keys. All have type wchar_t
 *
 * What a pity we don't have strongly typed enumerations until C++0x.
 */
const wchar_t CTRL_A    = 0x01;
const wchar_t CTRL_B    = 0x02;
const wchar_t CTRL_C    = 0x03;
const wchar_t CTRL_D    = 0x04;
const wchar_t CTRL_E    = 0x05;
const wchar_t CTRL_F    = 0x06;
const wchar_t CTRL_G    = 0x07;
const wchar_t CTRL_H    = 0x08;
const wchar_t CTRL_I    = 0x09;
const wchar_t CTRL_J    = 0x0A;
const wchar_t CTRL_K    = 0x0B;
const wchar_t CTRL_L    = 0x0C;
const wchar_t CTRL_M    = 0x0D;
const wchar_t CTRL_N    = 0x0E;
const wchar_t CTRL_O    = 0x0F;
const wchar_t CTRL_P    = 0x10;
const wchar_t CTRL_Q    = 0x11;
const wchar_t CTRL_R    = 0x12;
const wchar_t CTRL_S    = 0x13;
const wchar_t CTRL_T    = 0x14;
const wchar_t CTRL_U    = 0x15;
const wchar_t CTRL_V    = 0x16;
const wchar_t CTRL_W    = 0x17;
const wchar_t CTRL_X    = 0x18;
const wchar_t CTRL_Y    = 0x19;
const wchar_t CTRL_Z    = 0x1A;
const wchar_t ESCAPE    = 0x1B;
const wchar_t RETURN    = CTRL_J;
const wchar_t NEWLINE   = CTRL_M;
const wchar_t BACKSPACE1= CTRL_H;
const wchar_t TAB       = CTRL_I;
const wchar_t BACKSPACE2= 0x7F;

const wchar_t UP		= 0x70000000;
const wchar_t DOWN		= 0x70000001;
const wchar_t LEFT		= 0x70000002;
const wchar_t RIGHT		= 0x70000003;
const wchar_t HOME		= 0x70000004;
const wchar_t END		= 0x70000005;
const wchar_t PAGEUP	= 0x70000006;
const wchar_t PAGEDOWN	= 0x70000007;
const wchar_t DELETE	= 0x70000008;
const wchar_t INSERT    = 0x70000009;
const wchar_t BACKTAB   = 0x7000000A;

const wchar_t F1		= 0x70000101;
const wchar_t F2		= 0x70000102;
const wchar_t F3		= 0x70000103;
const wchar_t F4		= 0x70000103;
const wchar_t F5		= 0x70000104;
const wchar_t F6		= 0x70000105;
const wchar_t F7		= 0x70000106;
const wchar_t F8		= 0x70000107;
const wchar_t F9		= 0x70000108;
const wchar_t F10		= 0x70000109;
const wchar_t F11		= 0x7000010A;
const wchar_t F12		= 0x7000010B;

// ALT_BASE+L'a' == Alt - a
// ALT_BASE+L'A' == Alt - A
const wchar_t ALT_BASE  = 0x70000200;

/**
 * @brief	Size of screen changed
 *
 * When the size of screen changes, the program receives a @c SIGWINCH
 * signal, which ncurses receives and returns as if it was a keyboard event.
 */
const wchar_t WINCH		= 0x70000400;
const wchar_t MOUSE		= 0x70000401; ///< Mouse event


// The following constants are for outputing borders
const wchar_t BORDER_V  = 0x60000000;
const wchar_t BORDER_H  = 0x60000001;
const wchar_t BORDER_1  = 0x60000002; // Top, left
const wchar_t BORDER_2  = 0x60000003; // Top, right
const wchar_t BORDER_3  = 0x60000004; // Bottom, left
const wchar_t BORDER_4  = 0x60000005; // Bottom, right

inline bool special_printable (wchar_t c)
{
	unsigned ch = c;
	return ((ch - 0x60000000u) < 256);
}

// No global I/O methods available. Use tiary::ui::Window or tiary::ui::Control!


typedef unsigned char Attr;
typedef unsigned char Color;
typedef unsigned PaletteID;
const unsigned NUMBER_PALETTES = 256;

const Attr NORMAL    = 0;
const Attr UNDERLINE = 1;
const Attr HIGHLIGHT = 2;
const Attr REVERSE   = 4;
const Attr BLINK     = 8;
const Attr ALL_ATTR  = UNDERLINE|HIGHLIGHT|REVERSE|BLINK;

const Color BLACK   = 0;
const Color RED     = 1;
const Color GREEN   = 2;
const Color YELLOW  = 3;
const Color BLUE    = 4;
const Color MAGENTA = 5;
const Color CYAN    = 6;
const Color WHITE   = 7;

const Color NOCOLOR = 8;

const Color DEFAULT_FORECOLOR = WHITE;
const Color DEFAULT_BACKCOLOR = BLACK;

struct ColorAttr
{
	Color fore, back;
	Attr attr;
	unsigned char padding___;

	static ColorAttr make_default ();
};

inline ColorAttr ColorAttr::make_default ()
{
	ColorAttr r = { DEFAULT_FORECOLOR, DEFAULT_BACKCOLOR, 0 };
	return r;
}

struct CharColorAttr
{
	/// Sometimes, L'\0' is the placeholder for the second position
	/// for full-width character
	wchar_t c;
	ColorAttr a;
};


} // namespace tiary::ui
} // namespace tiary

#endif // include guard
