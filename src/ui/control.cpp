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


Control::Control (Window &dlg_)
	: MovableObject ()
	, Hotkeys ()
	, dlg (dlg_)
	, curpos (make_size ())
	, cursor_visible (true)
	, ctrl_left (0)
	, ctrl_right (0)
	, ctrl_up (0)
	, ctrl_down (0)
{
	dlg_.add_control (this);
}

Control::~Control ()
{
}

void Control::move_resize (Size newpos, Size newsize)
{
	if ((pos != newpos) || (size != newsize)) {
		dlg.choose_palette (PALETTE_ID_BACKGROUND);
		dlg.clear (pos, size);
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
	return (dlg.get_focus () == this);
}

void Control::focus ()
{
	dlg.set_focus (this, 0);
}

void Control::move_cursor (Size newpos)
{
	curpos = newpos;
}

void Control::choose_palette (PaletteID id)
{
	dlg.choose_palette (id);
}

void Control::choose_fore_color (Color fore)
{
	dlg.choose_fore_color (fore);
}

void Control::choose_back_color (Color back)
{
	dlg.choose_back_color (back);
}

void Control::choose_color (Color fore, Color back)
{
	dlg.choose_color (fore, back);
}

void Control::attribute_on (Attr attr)
{
	dlg.attribute_on (attr);
}

void Control::attribute_off (Attr attr)
{
	dlg.attribute_off (attr);
}

ColorAttr Control::get_attr () const
{
	return dlg.get_attr ();
}

void Control::set_attr (ColorAttr at)
{
	dlg.set_attr (at);
}

Size Control::put (Size xy, wchar_t c)
{
	return dlg.put (pos, size, xy, c);
}

Size Control::put (Size xy, const wchar_t *s)
{
	return dlg.put (pos, size, xy, s);
}

Size Control::put (Size xy, const wchar_t *s, size_t n)
{
	return dlg.put (pos, size, xy, s, n);
}

Size Control::put (Size xy, const std::wstring &s)
{
	return dlg.put (pos, size, xy, s);
}

void Control::clear ()
{
	dlg.clear (pos, size);
}

void Control::clear (Size fill_pos, Size fill_size)
{
	fill (fill_pos, fill_size, L' ');
}

void Control::fill (Size fill_pos, Size fill_size, wchar_t ch)
{
	fill_pos &= size;
	fill_size &= size - fill_pos;
	dlg.fill (fill_pos + pos, fill_size, ch);
}


UnfocusableControl::~UnfocusableControl ()
{
}

bool UnfocusableControl::on_focus ()
{
	return false;
}


} // namespace tiary::ui
} // namespace tiary
