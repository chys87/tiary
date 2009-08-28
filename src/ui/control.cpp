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
#include "ui/dialog.h"
#include "ui/paletteid.h"

namespace tiary {
namespace ui {


Control::Control (Dialog &dlg_)
	: dlg (dlg_)
	, pos (make_size ())
	, size (make_size ())
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
		Size oldpos = pos;
		Size oldsize = size;
		dlg.clear (oldpos, oldsize);
		pos = newpos;
		size = newsize;
		on_move_resize (oldpos, oldsize);
	}
}

bool Control::on_mouse (MouseEvent)
{
	return false;
}

bool Control::on_key (wchar_t)
{
	// Need not handle hotkeys here.
	// Done by Dialog::on_key
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

void Control::on_move_resize (Size, Size)
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
	if (dlg.get_focus () == this)
		dlg.move_cursor (newpos + pos);
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



} // namespace tiary::ui
} // namespace tiary
