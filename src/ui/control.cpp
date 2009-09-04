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


#include "ui/control.h"
#include "ui/window.h"
#include "ui/paletteid.h"

namespace tiary {
namespace ui {


Control::Control (Window &win_)
	: MovableObject ()
	, Hotkeys ()
	, win (win_)
	, curpos (make_size ())
	, cursor_visible (true)
	, ctrl_left (0)
	, ctrl_right (0)
	, ctrl_up (0)
	, ctrl_down (0)
{
	win_.add_control (this);
}

Control::~Control ()
{
}

void Control::move_resize (Size newpos, Size newsize)
{
	if ((pos != newpos) || (size != newsize)) {
		win.choose_palette (PALETTE_ID_BACKGROUND);
		win.clear (pos, size);
		pos = newpos;
		size = newsize;
	}
}

bool Control::on_mouse (MouseEvent)
{
	return false;
}

bool Control::on_key (wchar_t)
{
	// Need not handle hotkeys here.
	// Done by Window::on_key
	return false;
}

void Control::on_defocus ()
{
}

bool Control::on_focus ()
{
	return true;
}

void Control::on_focus_changed ()
{
}

bool Control::is_focus () const
{
	return (win.get_focus () == this);
}

void Control::focus ()
{
	win.set_focus (this, 0);
}

void Control::move_cursor (Size newpos)
{
	curpos = newpos;
}

void Control::choose_palette (PaletteID id)
{
	win.choose_palette (id);
}

void Control::choose_fore_color (Color fore)
{
	win.choose_fore_color (fore);
}

void Control::choose_back_color (Color back)
{
	win.choose_back_color (back);
}

void Control::choose_color (Color fore, Color back)
{
	win.choose_color (fore, back);
}

void Control::attribute_on (Attr attr)
{
	win.attribute_on (attr);
}

void Control::attribute_off (Attr attr)
{
	win.attribute_off (attr);
}

ColorAttr Control::get_attr () const
{
	return win.get_attr ();
}

void Control::set_attr (ColorAttr at)
{
	win.set_attr (at);
}

Size Control::put (Size xy, wchar_t c)
{
	return win.put (pos, size, xy, c);
}

Size Control::put (Size xy, const wchar_t *s)
{
	return win.put (pos, size, xy, s);
}

Size Control::put (Size xy, const wchar_t *s, size_t n)
{
	return win.put (pos, size, xy, s, n);
}

Size Control::put (Size xy, const std::wstring &s)
{
	return win.put (pos, size, xy, s);
}

void Control::clear ()
{
	win.clear (pos, size);
}

void Control::clear (Size fill_pos, Size fill_size)
{
	fill (fill_pos, fill_size, L' ');
}

void Control::fill (Size fill_pos, Size fill_size, wchar_t ch)
{
	fill_pos &= size;
	fill_size &= size - fill_pos;
	win.fill (fill_pos + pos, fill_size, ch);
}


UnfocusableControl::~UnfocusableControl ()
{
}

bool UnfocusableControl::on_focus ()
{
	return false;
}

FocusColorControl::~FocusColorControl ()
{
}

bool FocusColorControl::on_focus ()
{
	redraw ();
	return true;
}

void FocusColorControl::on_defocus ()
{
	redraw ();
}

} // namespace tiary::ui
} // namespace tiary
