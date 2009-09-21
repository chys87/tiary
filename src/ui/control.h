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


#ifndef TIARY_UI_CONTROL_H
#define TIARY_UI_CONTROL_H

#include "common/signal.h"
#include "ui/movable_object.h"
#include "ui/size.h"
#include "ui/ui.h"
#include "ui/hotkeys.h"
#include <string>

namespace tiary {
namespace ui {

class Window;
struct MouseEvent;

class Control : public MovableObject, public Hotkeys
{
public:
	explicit Control (Window &);
	virtual ~Control ();

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
	ColorAttr get_attr () const;
	void set_attr (ColorAttr);
	Size put (Size xy, wchar_t c);
	Size put (Size xy, const wchar_t *);
	Size put (Size xy, const wchar_t *, size_t);
	Size put (Size xy, const std::wstring &);
	void clear ();
	void clear (Size, Size);
	void fill (Size, Size, wchar_t);

	Window &win; // The window containing the control

private:
	Size curpos; ///< Cursor position
	bool cursor_visible; ///< Whether the cursor should be visible

public:
	// The following four pointer describe the relative position of
	// controls so that users can use arrow keys to navigate
	Control *ctrl_left, *ctrl_right, *ctrl_up, *ctrl_down;

	Signal sig_defocus;
	Signal sig_focus;
	Signal sig_clicked;

	friend class Window;
};


// Some attributes of Controls that can be "pasted" using virtual inheritence
class UnfocusableControl : public virtual Control
{
public:
	UnfocusableControl (Window &win) : Control (win) {}
	~UnfocusableControl ();
	bool on_focus ();
};

// Some controls look different when focused or not.
// So for them, on_focus and on_defocus should be forwarded to redraw
class FocusColorControl : public virtual Control
{
public:
	FocusColorControl (Window &win) : Control (win) {}
	~FocusColorControl ();
	bool on_focus ();
	void on_defocus ();
};

} // namespace tiary::ui
} // namespace tiary

#endif // Include guard
