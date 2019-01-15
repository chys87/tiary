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


#include "ui/menu.h"
#include "ui/window.h"
#include "ui/control.h"
#include "ui/uistring_one.h"
#include "ui/paletteid.h"
#include "ui/chain.h"
#include "ui/mouse.h"
#include "common/algorithm.h"
#include <utility> // std::forward

namespace tiary {
namespace ui {

Menu &MenuItem::get_submenu ()
{
	if (submenu == nullptr) {
		submenu.reset(new Menu);
	}
	return *submenu;
}




MenuItem &Menu::add() {
	return *item_list.emplace_back(new MenuItem);
}

MenuItem &Menu::add(std::wstring_view text, const Signal &sig) {
	return *item_list.emplace_back(new MenuItem{std::wstring(text), sig});
}

MenuItem &Menu::add(std::wstring_view text, const Action &act) {
	return *item_list.emplace_back(new MenuItem{std::wstring(text), act});
}

MenuItem &Menu::add(std::wstring_view text, Signal &&sig) {
	return *item_list.emplace_back(new MenuItem{std::wstring(text), std::move(sig)});
}

MenuItem &Menu::add(std::wstring_view text, Action &&act) {
	return *item_list.emplace_back(new MenuItem{std::wstring(text), std::move(act)});
}

Menu &Menu::add_submenu(std::wstring_view text) {
	auto &ptr = item_list.emplace_back(new MenuItem{std::wstring(text)});
	return ptr->get_submenu ();
}

namespace {

class ItemControl;
class MenuWindow;

class ItemControl final : public Control {
public:
	ItemControl (MenuWindow &, const MenuItem &, bool);
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
	bool valid;

	friend class MenuWindow;
};

class MenuWindow final : public Window {
public:
	MenuWindow (const Menu &, Size left, Size right, bool unget_left_);
	~MenuWindow ();

	void on_winch ();
	bool on_key (wchar_t);
	bool on_mouse_outside (MouseEvent);
	// void redraw (); // Window::redraw is ok

	MenuItem *get_result() const { return result_; }

private:

	MenuItem *result_ = nullptr;
	bool unget_left_;

	friend class ItemControl;
};

ItemControl::ItemControl (MenuWindow &win, const MenuItem &item_, bool valid_)
	: Control (win)
	, item (item_)
	, text (item_.text)
	, valid (valid_)
{
	set_cursor_visibility (false);
	if (valid_) {
		if (wchar_t c = text.get_hotkey ()) {
			win.register_hotkey (c, Signal (this, &ItemControl::slot_trigger),
					Hotkeys::CASE_INSENSITIVE | Hotkeys::ALLOW_ALT);
		}
	}
}

ItemControl::~ItemControl ()
{
}

bool ItemControl::on_focus ()
{
	if (!valid || item.text.empty ()) {
		return false;
	}
	else {
		redraw();
		return true;
	}
}

void ItemControl::on_defocus() {
	redraw();
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
	if (!valid || item.text.empty ()) {
		return false;
	}
	if (me.m & MOUSE_ALL_BUTTON) {
		slot_trigger ();
	}
	else {
		focus ();
	}
	return true;
}

void ItemControl::redraw ()
{
	if (item.text.empty ()) {
		// Separator

		choose_palette (PALETTE_ID_MENU);
		fill(Size{}, get_size (), BORDER_H);

	}
	else {

		PaletteID id;

		if (!valid) {
			id = PALETTE_ID_MENU_INVALID;
		}
		else if (window().get_focus() == this) {
			id = PALETTE_ID_MENU_SELECT;
		}
		else {
			id = PALETTE_ID_MENU;
		}
		choose_palette (id);
		clear ();
		text.output(*this, Size{}, get_size().x);
		if (item.submenu) {
			put(Size{get_size().x - 1, 0}, L'>');
		}

	}
}

void ItemControl::slot_trigger ()
{
	focus ();
	if (item.submenu == 0) {
		// No submenu
		static_cast<MenuWindow &>(window()).result_ = const_cast<MenuItem *>(&item);
		window().request_close ();
	}
	else {
		// Has submenu
		Size right = window().get_pos () + get_pos ();
		Size left = right + Size{get_size().x, 0};
		MenuWindow subwin (*item.submenu, left, right, false);
		subwin.event_loop ();
		if (MenuItem *subret = subwin.get_result ()) {
			static_cast<MenuWindow&>(window()).result_ = subret;
			window().request_close();
		}
	}
}

MenuWindow::MenuWindow (const Menu &menu_, Size left, Size right, bool unget_left)
	: Window (Window::WINDOW_NO_CLOSE_BUTTON|Window::WINDOW_NONMOVABLE)
	, unget_left_(unget_left) {
	ItemControl **ctrls = new ItemControl * [menu_.size ()];
	ItemControl **valid_ctrls = new ItemControl * [menu_.size ()]; // Excluding separators
	ItemControl **pi = ctrls;
	ItemControl **pv = valid_ctrls;
	unsigned maxwid = 0;
	for (const std::unique_ptr<MenuItem> &pmi: menu_) {
		if (!pmi->hidden) {
			bool validity = pmi->action.call_condition (true);
			ItemControl *p = *pi++ = new ItemControl (*this, *pmi, validity);
			if (validity && !p->text.get_text ().empty ()) {
				*pv++ = p;
			}
			maxwid = maxU (maxwid, p->text.get_width ());
		}
	}

	// Actual number of menu items shown on screen
	size_t actual_size = pi - ctrls;

	ChainControlsVertical (valid_ctrls, pv-valid_ctrls);
	delete [] valid_ctrls;

	// Now determine the proper position and size
	Size size{maxwid+2/*Border*/+1/*>*/+3/*Space*/, unsigned(actual_size) + 2};
	Size scrsize = get_screen_size ();
	Size pos;
	if (both (left + size <= scrsize)) {
		pos = left;
	}
	else {
		pos = right;
		pos.x = maxS (0, pos.x - size.x);
	}
	move_resize (pos, size);
	// Position controls
	size = {size.x - 1, 1}; // Control size
	for (size_t i=0; i<actual_size; ++i) {
		ctrls[i]->move_resize({1, unsigned(i) + 1}, {size.x - 1, 1});
	}
	delete [] ctrls;
}

MenuWindow::~MenuWindow ()
{
	// Delete objects new'd in ctor
	for (Control *ctrl = get_dummy_ctrl ()->get_next ();
			ctrl != get_dummy_ctrl (); ) {
		Control *next_ctrl = ctrl->get_next ();
		delete dynamic_cast <ItemControl *>(ctrl);
		ctrl = next_ctrl;
	}
}

bool MenuWindow::on_key (wchar_t c)
{
	if (Window::on_key (c)) {
		return true;
	}

	if ((unget_left_ || c != LEFT) && c != ESCAPE) {
		unget (c);
	}
	request_close ();
	return true;
}

void MenuWindow::on_winch ()
{
	request_close ();
}

bool MenuWindow::on_mouse_outside (MouseEvent me)
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
		MenuWindow win (*this, left, right, true);
		win.event_loop ();
		sel = win.get_result ();
	}
	// Destruct the window before emitting the signal
	if (emit_signal && sel) {
		sel->action.emit ();
	}
	return sel;
}


MenuItem *Menu::show (Size pos, bool emit_signal)
{
	return show (pos, pos, emit_signal);
}

} // namespace tiary::ui
} // namespace tiary
