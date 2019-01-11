// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2018, 2019, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#include "ui/control.h"
#include "ui/window.h"
#include "ui/paletteid.h"
#include "ui/mouse.h"

namespace tiary {
namespace ui {


Control::Control(Window &win, uint8_t properties)
	: MovableObject ()
	, win_(win)
	, hotkeys_()
	, curpos{}
	, cursor_visible (true)
	, properties_(properties)
	, ctrl_left (0)
	, ctrl_right (0)
	, ctrl_up (0)
	, ctrl_down (0)
	// Need not set prev and next here. Set by Window::add_control
{
	win_.add_control (this);
}

void Control::move_resize (Size newpos, Size newsize)
{
	if ((get_pos() != newpos) || (get_size() != newsize)) {
		win_.choose_palette(PALETTE_ID_BACKGROUND);
		win_.clear(get_pos(), get_size());
		set_pos(newpos);
		set_size(newsize);
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
	if (properties_ & kRedrawOnFocusChange) {
		redraw();
	}
}

bool Control::on_focus ()
{
	if (properties_ & kUnfocusable) {
		return false;
	}
	if (properties_ & kRedrawOnFocusChange) {
		redraw();
	}
	return true;
}

bool Control::is_focus () const
{
	return (win_.get_focus() == this);
}

void Control::focus ()
{
	win_.set_focus(this, 0);
}

void Control::move_cursor (Size newpos)
{
	curpos = newpos;
}

void Control::choose_palette (PaletteID id)
{
	win_.choose_palette(id);
}

void Control::choose_fore_color (Color fore)
{
	win_.choose_fore_color(fore);
}

void Control::choose_back_color (Color back)
{
	win_.choose_back_color(back);
}

void Control::choose_color (Color fore, Color back)
{
	win_.choose_color(fore, back);
}

void Control::attribute_on (Attr attr)
{
	win_.attribute_on(attr);
}

void Control::attribute_off (Attr attr)
{
	win_.attribute_off(attr);
}

void Control::attribute_toggle (Attr attr)
{
	win_.attribute_toggle(attr);
}

ColorAttr Control::get_attr () const
{
	return win_.get_attr();
}

void Control::set_attr (ColorAttr at)
{
	win_.set_attr(at);
}

Size Control::put (Size xy, wchar_t c)
{
	return win_.put(get_pos(), get_size(), xy, c);
}

Size Control::put (Size xy, const wchar_t *s)
{
	return win_.put(get_pos(), get_size(), xy, s);
}


Size Control::put(Size xy, std::wstring_view s) {
	return win_.put(get_pos(), get_size(), xy, s);
}

void Control::clear ()
{
	win_.clear(get_pos(), get_size());
}

void Control::clear (Size fill_pos, Size fill_size)
{
	fill (fill_pos, fill_size, L' ');
}

void Control::fill (Size fill_pos, Size fill_size, wchar_t ch)
{
	if (get_size() > fill_pos) {
		fill_size &= get_size() - fill_pos;
		win_.fill(fill_pos + get_pos(), fill_size, ch);
	}
}

} // namespace tiary::ui
} // namespace tiary
