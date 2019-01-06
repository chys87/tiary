// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2019, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/

/**
 * @file	ui/control.h
 * @author	chys <admin@chys.info>
 * @brief	Header for class tiary::ui::Control
 */

#ifndef TIARY_UI_CONTROL_H
#define TIARY_UI_CONTROL_H

#include "common/action.h"
#include "ui/movable_object.h"
#include "ui/size.h"
#include "ui/ui.h"
#include "ui/hotkeys.h"
#include <string>

namespace tiary {
namespace ui {

class Window;
struct MouseEvent;

/**
 * @brief	The base class for all controls
 */
class Control : public MovableObject
{
public:
	static constexpr uint8_t kUnfocusable = 1;
	static constexpr uint8_t kRedrawOnFocusChange = 2;

	explicit Control(Window &, uint8_t properties = 0);

	// move_resize does not imply redraw
	void move_resize (Size pos, Size size);

	// Never receives WINCH (should be handled by window); mouse position relative to control
	virtual bool on_mouse (MouseEvent);
	virtual bool on_key (wchar_t);
	virtual void on_defocus (); // Cannot refuse defocusing
	virtual bool on_focus ();   // Can refuse defocusing (default is acceptance)

	bool is_focus () const;
	void focus (); ///< Make this Control the focus of the Window

	// Redraw the control.
	// Typically called when the pos/size of the control is changed,
	// or the control is focused/defocused (if it should look differently
	// when focused/defocused)
	virtual void redraw () = 0;

	// OUTPUT FUNCTIONS: Positions are relative to control
	// IMPORTANT: Output functions does not move cursor
	//
	// Cursor position, in the context of controls, having
	// nothing to do with outputing, only affects the
	// cursor position on screen if the control is focused
	void move_cursor (Size);
	Size get_cursor_pos () const { return curpos; }

	void set_cursor_visibility (bool v) { cursor_visible = v; }
	bool get_cursor_visibility () const { return cursor_visible; }

	void choose_palette (PaletteID);
	void choose_fore_color (Color);
	void choose_back_color (Color);
	void choose_color (Color, Color);
	void attribute_on (Attr);
	void attribute_off (Attr);
	void attribute_toggle (Attr);
	ColorAttr get_attr () const;
	void set_attr (ColorAttr);
	Size put (Size xy, wchar_t c);
	Size put (Size xy, const wchar_t *);
	Size put (Size xy, const wchar_t *, size_t);
	Size put (Size xy, const std::wstring &);
	Size put(Size xy, std::wstring_view);
	void clear ();
	void clear (Size, Size);
	void fill (Size, Size, wchar_t);

	Control *get_prev () { return prev; }
	Control *get_next () { return next; }

	Window &window() { return win_; }
	const Window &window() const { return win_; }

	template <typename... Args>
	void register_hotkey(wchar_t c, Args&&... args) { hotkeys_.register_hotkey(c, std::forward<Args>(args)...); }

private:
	Window &win_; // The window containing the control
	Hotkeys hotkeys_;
	Size curpos; ///< Cursor position
	bool cursor_visible; ///< Whether the cursor should be visible
	uint8_t properties_;

public:
	// The following four pointer describe the relative position of
	// controls so that users can use arrow keys to navigate
	Control *ctrl_left, *ctrl_right, *ctrl_up, *ctrl_down;

	// Previous/next control in the window.
	// This linklist is cyclic
	Control *prev;
	Control *next;

	Signal sig_defocus;
	Signal sig_focus;
	Action sig_clicked;

	friend class Window;
};

} // namespace tiary::ui
} // namespace tiary

#endif // Include guard
