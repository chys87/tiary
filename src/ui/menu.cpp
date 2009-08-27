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


#include "ui/menu.h"
#include "ui/dialog.h"
#include "ui/control.h"
#include "ui/uistring.h"
#include "ui/paletteid.h"
#include "ui/chain.h"
#include "common/container_of.h"
#include "common/containers.h"
#include <utility> // std::forward

namespace tiary {
namespace ui {

MenuItem::MenuItem (const std::wstring &text_, const Signal &sig_)
	: text (text_)
	, sig (sig_)
	, hidden (false)
	, submenu (0)
{
}

MenuItem::MenuItem (const MenuItem &other)
	: text (other.text)
	, sig (other.sig)
	, hidden (other.hidden)
	, submenu (other.submenu ? new Menu (*other.submenu) : 0)
{
}

MenuItem &MenuItem::operator = (const MenuItem &other)
{
	text = other.text;
	sig = other.sig;
	hidden = other.hidden;
	submenu = other.submenu ? new Menu (*other.submenu) : 0;
	return *this;
}

#ifdef TIARY_HAVE_RVALUE_REFERENCES
MenuItem::MenuItem (const std::wstring &text_, Signal &&sig_)
	: text (text_)
	, sig (std::forward<Signal> (sig_))
	, hidden (false)
	, submenu (0)
{
}

MenuItem::MenuItem (MenuItem &&other)
	: text (std::forward<std::wstring> (other.text))
	, sig (std::forward<Signal> (other.sig))
	, hidden (other.hidden)
	, submenu (other.submenu)
{
	other.submenu = 0;
}

MenuItem &MenuItem::operator = (MenuItem &&other)
{
	text = std::forward<std::wstring> (other.text);
	sig = std::forward<Signal> (other.sig);
	hidden = other.hidden;
	submenu = other.submenu;
	other.submenu = 0;
	return *this;
}
#endif

MenuItem::~MenuItem ()
{
	delete submenu;
}

Menu &MenuItem::get_submenu ()
{
	if (submenu == 0)
		submenu = new Menu;
	return *submenu;
}




MenuItem &Menu::add (const std::wstring &text, const Signal &sig)
{
	item_list.push_back (MenuItem (text, sig));
	return item_list.back ();
}

MenuItem &Menu::add (const wchar_t *text, const Signal &sig)
{
	return add (std::wstring (text), sig);
}

#ifdef TIARY_HAVE_RVALUE_REFERENCES
MenuItem &Menu::add (const std::wstring &text, Signal &&sig)
{
	item_list.push_back (MenuItem (text, std::forward<Signal> (sig)));
	return item_list.back ();
}

MenuItem &Menu::add (const wchar_t *text, Signal &&sig)
{
	return add (std::wstring (text), std::forward<Signal> (sig));
}
#endif

Menu &Menu::add_submenu (const std::wstring &text)
{
	return add (text, Signal ()).get_submenu ();
}

namespace {

class ItemControl;
class MenuDialog;

class ItemControl : public Control
{
public:
	ItemControl (MenuDialog &, const MenuItem &);
	~ItemControl ();

	bool on_focus ();
	void on_defocus ();
	bool on_key (wchar_t);
	bool on_mouse (MouseEvent);
	void redraw ();

	void slot_trigger ();

private:
	const MenuItem &item;
	UIStringOne text;

	friend class MenuDialog;
};

class MenuDialog : public Dialog
{
public:
	MenuDialog (const Menu &, Size left, Size right, bool unget_left_);
	~MenuDialog ();

	void on_winch ();
	bool on_key (wchar_t);
	bool on_mouse_outside (MouseEvent);
	void redraw (); // Dialog::redraw is ok

	MenuItem *get_result () const { return result; }

private:

	MenuItem *result;
	const Menu &menu;
	bool unget_left;

	friend class ItemControl;
};

ItemControl::ItemControl (MenuDialog &dlg, const MenuItem &item_)
	: Control (dlg)
	, item (item_)
	, text (item_.text)
{
	set_cursor_visibility (false);
	if (wchar_t c = text.get_hotkey ())
		dlg.register_hotkey (c, Signal (this, &ItemControl::slot_trigger));
}

ItemControl::~ItemControl ()
{
}

bool ItemControl::on_focus ()
{
	ItemControl::redraw ();
	return true;
}

void ItemControl::on_defocus ()
{
	ItemControl::redraw ();
}

bool ItemControl::on_key (wchar_t c)
{
	switch (c) {
		case NEWLINE:
		case RETURN:
		case L' ':
			slot_trigger ();
			return true;
		case RIGHT:
			if (item.submenu) {
				slot_trigger ();
				return true;
			}
			return false;
		default:
			return false;
	}
}

bool ItemControl::on_mouse (MouseEvent me)
{
	if (me.m & MOUSE_ALL_BUTTON)
		slot_trigger ();
	else
		focus ();
	return true;
}

void ItemControl::redraw ()
{
	choose_palette (dlg.get_focus () == this ? PALETTE_ID_MENU_SELECT : PALETTE_ID_MENU);
	clear ();
	text.output (*this, make_size (), get_size ().x);
	if (item.submenu)
		put (make_size (get_size ().x-1, 0), L'>');
}

void ItemControl::slot_trigger ()
{
	focus ();
	if (item.submenu == 0) {
		// No submenu
		static_cast <MenuDialog &>(dlg).result = const_cast <MenuItem *>(&item);
		dlg.request_close ();
	} else {
		// Has submenu
		Size right = dlg.get_pos () + get_pos ();
		Size left = right + make_size (get_size ().x, 0);
		MenuDialog subwin (*item.submenu, left, right, false);
		subwin.event_loop ();
		if (MenuItem *subret = subwin.get_result ()) {
			static_cast<MenuDialog&>(dlg).result = subret;
			dlg.request_close ();
		}
	}
}

MenuDialog::MenuDialog (const Menu &menu_, Size left, Size right, bool unget_left_)
	: Dialog (Dialog::DIALOG_NO_CLOSE_BUTTON)
	, result (0)
	, menu (menu_)
	, unget_left (unget_left_)
{
	ItemControl **ctrls = new ItemControl * [menu_.size ()];
	ItemControl **pi = ctrls;
	unsigned maxwid = 0;
	for (Menu::const_iterator it = menu_.begin (); it != menu_.end (); ++it) {
		if (!it->hidden) {
			ItemControl *p = *pi++ = new ItemControl (*this, *it);
			maxwid = maxU (maxwid, p->text.get_width ());
		}
	}

	// Actual number of menu items shown on screen
	size_t actual_size = pi - ctrls;

	ChainControlsVertical (ctrls, actual_size);

	// Now determine the proper position and size
	Size size = make_size (maxwid+2/*Border*/+1/*>*/+3/*Space*/, actual_size + 2);
	Size scrsize = get_screen_size ();
	Size pos;
	if (both (left + size <= scrsize))
		pos = left;
	else {
		pos = right;
		pos.x = maxS (0, pos.x - size.x);
	}
	move_resize (pos, size);
	// Position controls
	size = make_size (size.x-1, 1); // Control size
	for (size_t i=0; i<actual_size; ++i)
		ctrls[i]->move_resize (make_size (1, i+1), make_size (size.x-1, 1));
	delete [] ctrls;
	MenuDialog::redraw ();
}

MenuDialog::~MenuDialog ()
{
	// Delete objects new'd in ctor
	const Dialog::ControlList &lst = get_control_list ();
	for (Dialog::ControlList::const_iterator it = lst.begin (); it != lst.end (); ++it)
		delete dynamic_cast <ItemControl *>(*it);
}

bool MenuDialog::on_key (wchar_t c)
{
	if (Dialog::on_key (c))
		return true;

	if ((unget_left || c!=LEFT) && c!=ESCAPE)
		unget (c);
	request_close ();
	return true;
}

void MenuDialog::on_winch ()
{
	request_close ();
}

void MenuDialog::redraw ()
{
	Dialog::redraw ();
}

bool MenuDialog::on_mouse_outside (MouseEvent me)
{
	if (me.m & MOUSE_ALL_BUTTON) {
		// Mouse key pressed outside window
		unget (me);
		request_close ();
	}
	return true;
}

} // anonymous namespace

MenuItem *Menu::show (Size left, Size right, bool emit_signal)
{
	MenuItem *sel;
	{
		MenuDialog win (*this, left, right, true);
		win.event_loop ();
		sel = win.get_result ();
	}
	// Destruct the window before emitting the signal
	if (emit_signal && sel)
		sel->sig.emit ();
	return sel;
}


MenuItem *Menu::show (Size pos, bool emit_signal)
{
	return show (pos, pos, emit_signal);
}

} // namespace tiary::ui
} // namespace tiary
