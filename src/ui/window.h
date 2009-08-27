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


#ifndef TIARY_UI_WINDOW_H
#define TIARY_UI_WINDOW_H

#include "ui/object.h"
#include "ui/ui.h"
#include <stddef.h> // ::size_t
#include <string>

namespace tiary {
namespace ui {

/**
 * @brief	Window: An object-oriented event-driven portion of the screen
 *
 * Again, remember the difference between a Window and a Dialog:
 *  - A Dialog is derived from Window
 *  - A Dialog is a container of Controls; a Window is not
 */
class Window : public Object
{
public:

	explicit Window (Size pos = make_size (), Size size = make_size ());
	virtual ~Window ();

	// Interfaces:
	virtual bool on_key (wchar_t); ///< Returns true/false = the input is/isn't handled
	virtual bool on_mouse (MouseEvent); // Mouse coordinates are relative to window
	virtual void on_winch () = 0; // There's no default way to handle WINCH
	virtual bool on_mouse_outside (MouseEvent); // Mouse event outside the window boundary. Coordinates are absolute

	void event_loop (); // Main event loop

	void choose_palette (PaletteID); ///< Choose palette, invalidating all forced attributes
	void choose_fore_color (Color); ///< Choose forecolor
	void choose_back_color (Color); ///< Choose backcolor
	void choose_color (Color fore, Color back); ///< Choose both fore and back color
	void attribute_on (Attr); ///< Force attributes on notwithstanding palette (bitwise OR'd)
	void attribute_off (Attr); ///< Force attributes off notwithstanding palette
	ColorAttr get_attr () const { return cur_attr; }
	void set_attr (const ColorAttr &);

	void move_cursor (Size); // Move cursor
	Size get_cursor_pos () const { return curpos; }
	void set_cursor_visibility (bool v) { cursor_visible = v; }
	bool get_cursor_visibility () const { return cursor_visible; }

	Size get_pos () const { return pos; }
	Size get_size () const { return size; }

	void move_resize (Size, Size); // Move and resize window, NOT preserving content

	void clear (); // Fill the whole window with spaces and current attribute
	void clear (Size fill_pos, Size fill_size);
	// Fill a sub-region with spaces and current attribute
	// The character must be half-widthed; otherwise the behavior is undefined.
	void fill (Size fill_pos, Size fill_size, wchar_t);

	// These functions output at a specified cursor position,
	// does NOT move the current cursor position, and returns the new position
	Size put (Size, wchar_t);
	Size put (Size, const wchar_t *);
	Size put (Size, const wchar_t *, size_t);
	Size put (Size, const std::wstring &);

	// These functions are designed to facilitate the implementation of dialogs and controls
	// put (Size blk_pos, Size blk_size, Size pos, ...)
	// Assumes a small block defined by (blk_pos, blk_size), and all outputs are carried
	// out in that small block.
	// The return value is the new cursor position relative to the small block
	Size put (Size, Size, Size, wchar_t);
	Size put (Size, Size, Size, const wchar_t *);
	Size put (Size, Size, Size, const wchar_t *, size_t);
	Size put (Size, Size, Size, const std::wstring &);

	// Touches (but not updates) the whole screen.
	// Next time refresh is called, the whole screen is redrawn (like ^L in many apps)
	static void touch_screen ();
	// Get a key/mouse event
	static wchar_t get (MouseEvent *);
	static wchar_t get_noblock (MouseEvent *); ///< Returns L'\0' if no input
	// Put a key/mouse event back to the queue
	static void unget (wchar_t);
	static void unget (MouseEvent);
	// Suspend all windows temporarily and exit the UI system
	static void suspend ();
	// Restore all windows and the UI system
	static bool resume ();

	const CharColorAttr * const* get_char_table () const { return char_table; }
	const CharColorAttr * get_char_table (unsigned line) const { return char_table[line]; }

	static const unsigned REQUEST_CLOSE = 1;
	void request (unsigned);
	void request_close (); ///< Equivalent to request (REQUEST_CLOSE)

private:
	unsigned requests;

	Size pos, size; // Position and size
	Size curpos;    // Cursor position
	ColorAttr cur_attr; // Current attribute

	bool cursor_visible; ///< Whether the cursor should be visible

	// Remember the character and attribute at every point
	// The area is contiguous. i.e.
	// char_table = new CharColorAttr*[height];
	// char_table[0] = new CharColorAttr[height*width];
	// char_table[i+1] = char_table[i] + width
	CharColorAttr **char_table;

	void reallocate_char_table ();
	void deallocate_char_table ();
};

} // namespace tiary::ui
} // namespace tiary


#endif // include guard
