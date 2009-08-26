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


#ifndef TIARY_UI_DIALOG_H
#define TIARY_UI_DIALOG_H

/**
 * A "dialog" is largely identical to a "window" in other GUI systems.
 * We use the term "dialog" because "window" has already been used as
 * is in curses.
 *
 * Components of a dialog is are called "controls".
 * Visible controls cannot overlap in a dialog. Otherwise,
 * the behavior is undefined.
 *
 * A dialog is a window, and also a container of controls.
 */

#include "ui/window.h"
#include "common/signal.h"
#include "ui/hotkeys.h"
#include "ui/uistring.h"
#include <vector>
#include <string>

namespace tiary {
namespace ui {

class Control;


class Dialog : public Window, public Hotkeys
{
public:

	/*
	 * If you use a border, it is your responsibility to guarantee
	 * no control uses the top/bottom lines and left/right columns.
	 *
	 * If the close button is clicked, it is as if Escape is pressed.
	 */

	static const unsigned DIALOG_NO_BORDER = 1;
	static const unsigned DIALOG_NO_CLOSE_BUTTON = 2;

	explicit Dialog (unsigned options = 0, const std::wstring &title = std::wstring ());
	virtual ~Dialog ();

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

	bool on_mouse (MouseEvent);
	bool on_key (wchar_t);
	//void on_winch (); // Derivative classes should implement this

	// Clear window and call every control's redraw functions
	void redraw ();

	// Do some preparations and call Window::event_loop
	void event_loop ();

protected:
	typedef std::vector<Control *> ControlList;

	const ControlList &get_control_list () const { return control_list; }

private:

	// To be used by Control's contructor only
	void add_control (Control *);

	unsigned options;
	UIStringOne title;

	ControlList control_list;

	int focus_id;

	friend class Control;
};


} // namespace tiary::ui
} // namespace tiary

#endif // include guard
