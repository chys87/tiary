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


#include "ui/dialog.h"
#include "ui/control.h"
#include "ui/paletteid.h"
#include "ui/hotkeys.h"
#include "ui/terminal_emulator.h"
#include <algorithm>
#include <assert.h>
#include <signal.h>


namespace tiary {
namespace ui {


Dialog::Dialog (unsigned options_, const std::wstring &title_)
	: Window ()
	, Hotkeys ()
	, options (options_)
	, title (title_, UIStringBase::NO_HOTKEY)
	, control_list ()
	, focus_id (-1)
{
}

Dialog::~Dialog ()
{
}

void Dialog::add_control (Control *ctrl)
{
	assert (&ctrl->dlg == this);

	control_list.push_back (ctrl);
}

bool Dialog::set_focus_id (unsigned id, int fall_direction)
{
	if (id >= control_list.size ())
		return false;
	int old_focus = focus_id;
	if (old_focus == int (id))
		return true;
	if (old_focus >= 0) {
		focus_id = -1;
		control_list[old_focus]->on_defocus ();
	}
	unsigned N = control_list.size ();
	unsigned fall_add = N + fall_direction;
	unsigned try_id = id;
	for (;;) {
		focus_id = try_id;
		Control *try_ctrl = control_list[try_id];
		if (try_ctrl->on_focus ()) {
			// Success. Change cursor position
			move_cursor (try_ctrl->curpos + try_ctrl->pos);
			// Change cursor visibility
			set_cursor_visibility (try_ctrl->get_cursor_visibility ());
			// Notify every control
			for (ControlList::iterator it = control_list.begin ();
					it != control_list.end (); ++it)
				(*it)->on_focus_changed ();
			// Emit two signals
			if (old_focus >= 0)
				control_list[old_focus]->sig_defocus.emit ();
			try_ctrl->sig_focus.emit ();
			return true;
		}
		try_id = (try_id + fall_add) % N;
		if (try_id == id) {
			// Completely failed. Try refocusing the old focus
			focus_id = old_focus;
			if (old_focus >= 0)
				control_list[old_focus]->on_focus ();
			return false;
		}
	}
}

bool Dialog::set_focus_ptr (Control *ctrl, int fall_direction)
{
	ControlList::iterator it = std::find (control_list.begin (), control_list.end (), ctrl);
	if (it == control_list.end ())
		return false;
	return set_focus_id (it - control_list.begin (), fall_direction);
}

bool Dialog::set_focus_next (bool keep_trying)
{
	unsigned id = focus_id + 1;
	if (id>=control_list.size())
		id = 0;
	return set_focus_id (id, keep_trying ? 1 : 0);
}

bool Dialog::set_focus_prev (bool keep_trying)
{
	unsigned id = focus_id - 1;
	if (id >= control_list.size())
		id = control_list.size() - 1;
	return set_focus_id (id, keep_trying ? -1 : 0);
}

bool Dialog::set_focus_direction (Control *Control::*dir)
{
	unsigned id = focus_id;
	if (id < control_list.size ()) {
		if (Control *new_focus = control_list[id]->*dir)
			return set_focus_ptr (new_focus, 0);
	}
	return false;
}

Control *Dialog::get_focus () const
{
	unsigned id = focus_id;
	return (id >= control_list.size ()) ? 0 : control_list[id];
}

bool Dialog::on_mouse (MouseEvent mouse_event)
{
	bool processed = false;

	// First decide which control this event happens on
	for (ControlList::iterator it = control_list.begin (); it != control_list.end (); ++it) {
		if (both (mouse_event.p - (*it)->pos < (*it)->size)) {
			// If it's any mouse key event, we should first try to focus it
			// Otherwise, we do not.
			if (mouse_event.m & MOUSE_ALL_BUTTON)
				set_focus (it - control_list.begin (), 0); // Regardless whether it's successful or not
			mouse_event.p -= (*it)->pos;
			bool processed = (*it)->on_mouse (mouse_event);
			if (!processed) {
				if ((mouse_event.m&LEFT_CLICK) && (*it)->sig_clicked.is_really_connected ()) {
					(*it)->sig_clicked.emit ();
					processed = true;
				}
			}
			break;
		}
	}

	// Clicked close button?
	if (!processed) {
		if ((mouse_event.m & LEFT_CLICK) && (mouse_event.p.y == 0) &&
				(get_size ().x - 2 - mouse_event.p.x < 3))
			processed = on_key (ESCAPE);
	}

	return processed;
}

bool Dialog::on_key (wchar_t c)
{
	bool processed = false;
	// First try the focused control
	if (unsigned (focus_id) < control_list.size ()) {
		processed = control_list[focus_id]->on_key (c);
		// Not processed? How about local hotkeys?
		if (!processed)
			processed = control_list[focus_id]->emit_hotkey (c);
	}
	// Not processed? How about dialog hotkeys?
	if (!processed)
		processed = emit_hotkey (c);
	// Still not processed? Interpret it ourselves
	if (!processed) {
		// Currently supports '\t', BACKTAB and arrow keys
		switch (c) {
			case L'\t':
				processed = set_focus_next (true);
				break;
			case BACKTAB:
				processed = set_focus_prev (true);
				break;
			case UP:
				processed = set_focus_direction (&Control::ctrl_up);
				break;
			case DOWN:
				processed = set_focus_direction (&Control::ctrl_down);
				break;
			case LEFT:
				processed = set_focus_direction (&Control::ctrl_left);
				break;
			case RIGHT:
				processed = set_focus_direction (&Control::ctrl_right);
				break;
			case CTRL_Z:
				Window::suspend ();
				raise (SIGTSTP);
				Window::resume ();
				// Fall into ...
			case CTRL_L:
				Window::touch_screen ();
				processed = true;
				break;
		}
	}
	return processed;
}

void Dialog::redraw ()
{
	choose_palette (PALETTE_ID_BACKGROUND);
	clear ();

	Size size = get_size ();
	// Draw the border
	if (!(options & DIALOG_NO_BORDER) && both (size >= make_size (2, 2))) {

		put (make_size (), BORDER_1); // Top, left
		fill (make_size (1, 0), make_size (size.x-2, 1), BORDER_H); // Up
		put (make_size (size.x-1, 0), BORDER_2); // Top, right
		fill (make_size (0, 1), make_size (1, size.y-2), BORDER_V); // Left
		fill (make_size (size.x-1, 1), make_size (1, size.y-2), BORDER_V); // Right
		put (make_size (0, size.y-1), BORDER_3); // Bottom, left
		fill (make_size (1, size.y-1), make_size (size.x-2, 1), BORDER_H); // Down
		put (make_size (size.x-1, size.y-1), BORDER_4); // Bottom, right

		// Show the title
		if (!title.get_text().empty ()) {
			unsigned title_width = minU (title.get_width (), size.x);
			unsigned left = (size.x - title_width - 2) / 2;
			Size pos = make_size (left, 0);
			pos = put (pos, L' ');
			pos = title.output (*this, pos, title_width);
			pos = put (pos, L' ');
		}

		// Show the close button
		if (!(options & DIALOG_NO_CLOSE_BUTTON) && size.x>=4) {
			Size pos = make_size (size.x - 4, 0);
			// u00d7 is multiplication sign
			put (pos, terminal_emulator_correct_wcwidth () ? L"[\u00d7]" : L"[x]");
		}
	}

	for (ControlList::iterator it = control_list.begin ();
			it != control_list.end (); ++it)
		(*it)->redraw ();
}

void Dialog::event_loop ()
{
	if (unsigned (focus_id) >= control_list.size ())
		set_focus (0u, 1);
	Window::event_loop ();
}

} // namespace tiary::ui
} // namespace tiary
