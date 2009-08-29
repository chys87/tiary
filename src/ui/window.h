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


#ifndef TIARY_UI_WINDOW_H
#define TIARY_UI_WINDOW_H

#include "ui/object.h"
#include "ui/ui.h"
#include "ui/hotkeys.h"
#include "ui/uistring.h"
#include <stddef.h> // ::size_t
#include <string>

namespace tiary {
namespace ui {

class Control;

/**
 * @brief	Window: An object-oriented event-driven portion of the screen
 */
class Window : public Object, public Hotkeys
{
public:
	/*
	 * If you use a border, it is your responsibility to guarantee
	 * no control uses the top/bottom lines and left/right columns.
	 *
	 * If the close button is clicked, it is as if Escape is pressed.
	 */

	static const unsigned WINDOW_NO_BORDER = 1;
	static const unsigned WINDOW_NO_CLOSE_BUTTON = 2;

	explicit Window (unsigned = 0, const std::wstring &title = std::wstring ());
	virtual ~Window ();

	/**
	 * fall_direction = 
	 * 0: Try only the specified control;
	 * 1: Repeatedly try the next control until success or a complete failure
	 * -1: Repeatedly try the previous control until success or a complete failure
	 */
	bool set_focus_id (unsigned, int fall_direction = 0);
	bool set_focus_ptr (Control *, int fall_direction = 0);
	bool set_focus (unsigned id, int fall_direction = 0) { return set_focus_id (id, fall_direction); }
	bool set_focus (Control &ctrl, int fall_direction = 0) { return set_focus_ptr (&ctrl, fall_direction); }
	bool set_focus (Control *ctrl, int fall_direction = 0) { return set_focus_ptr (ctrl, fall_direction); }
	bool set_focus_next (bool keep_trying = false);
	bool set_focus_prev (bool keep_trying = false);
	// Argument = one of Control::ctrl_{up,down,left,right}
	bool set_focus_direction (Control *Control::*);
	Control *get_focus () const;


	// Interfaces:
	virtual bool on_key (wchar_t); ///< Returns true/false = the input is/isn't handled
	virtual bool on_mouse (MouseEvent); // Mouse coordinates are relative to window
	virtual void on_winch (); ///< Called when screen size changed. Default: call redraw
	virtual bool on_mouse_outside (MouseEvent); // Mouse event outside the window boundary. Coordinates are absolute
	virtual void redraw (); // Default behavior: Clear window and call every control's redraw functions

	void event_loop (); // Main event loop

	void choose_palette (PaletteID); ///< Choose palette, invalidating all forced attributes
	void choose_fore_color (Color); ///< Choose forecolor
	void choose_back_color (Color); ///< Choose backcolor
	void choose_color (Color fore, Color back); ///< Choose both fore and back color
	void attribute_on (Attr); ///< Force attributes on notwithstanding palette (bitwise OR'd)
	void attribute_off (Attr); ///< Force attributes off notwithstanding palette
	ColorAttr get_attr () const { return cur_attr; }
	void set_attr (const ColorAttr &);

	// Cursor is positioned where the focus control wants it to be
	Size get_cursor_pos () const;
	bool get_cursor_visibility () const;

	Size get_pos () const { return pos; }
	Size get_size () const { return size; }

	void move_resize (Size, Size); // Move and resize window, NOT preserving content

	void clear (); // Fill the whole window with spaces and current attribute
	void clear (Size fill_pos, Size fill_size);
	// Fill a sub-region with spaces and current attribute
	// The character must be half-widthed; otherwise the behavior is undefined.
	void fill (Size fill_pos, Size fill_size, wchar_t);

	// These functions output at a specified cursor position,
	// Returns the new position
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
	// Forces all windows to redraw themselves
	// Useful after modifications of palettes (also in other special cases)
	static void touch_windows ();
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


	typedef std::vector<Control *> ControlList;

	const ControlList &get_control_list () const { return control_list; }

private:
	unsigned requests;

	Size pos, size; // Position and size
	ColorAttr cur_attr; // Current attribute

	// Remember the character and attribute at every point
	// The area is contiguous. i.e.
	// char_table = new CharColorAttr*[height];
	// char_table[0] = new CharColorAttr[height*width];
	// char_table[i+1] = char_table[i] + width
	CharColorAttr **char_table;

	void reallocate_char_table ();
	void deallocate_char_table ();


	unsigned options;
	UIStringOne title;

	ControlList control_list;
	int focus_id; ///< -1 = none

	// To be used by Control's contructor only
	void add_control (Control *);

	friend class Control;
};

} // namespace tiary::ui
} // namespace tiary


#endif // include guard
