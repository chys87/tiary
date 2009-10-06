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

#include "main/dialog_open_recent.h"
#include "ui/window.h"
#include "ui/fixed_window.h"
#include "ui/button_default.h"
#include "ui/listbox.h"
#include "ui/button.h"
#include "ui/layout.h"
#include "ui/chain.h"
#include "common/unicode.h"
#include "common/algorithm.h"
#include "common/dir.h"

namespace tiary {

namespace {

using namespace ui;

class WindowRecentFiles : virtual public Window, public FixedWindow, public ButtonDefault
{
	ListBox lst_files;
	Button btn_ok;
	Button btn_cancel;
	Button btn_remove;
	Button btn_remove_all;
	Layout layout_main;
	Layout layout_buttons;

	unsigned max_width; ///< Max screen width of all selectable items
	std::wstring result;
	bool modified;

	RecentFileList &lst_orig;

public:
	explicit WindowRecentFiles (RecentFileList &);
	~WindowRecentFiles ();

	void redraw ();

	void slot_ok ();
	void slot_remove ();
	void slot_remove_all ();

	const std::wstring &get_result () const { return result; }
	bool get_modified () const { return modified; }
};

WindowRecentFiles::WindowRecentFiles (RecentFileList &lst)
	: Window (0, L"Open recent files")
	, FixedWindow ()
	, ButtonDefault ()
	, lst_files (*this)
	, btn_ok (*this, L"&OK")
	, btn_cancel (*this, L"&Cancel")
	, btn_remove (*this, L"&Remove")
	, btn_remove_all (*this, L"C&lear all")
	, layout_main (VERTICAL)
	, layout_buttons (HORIZONTAL)
	, max_width (0)
	, result ()
	, modified (false)
	, lst_orig (lst)
{
	{
		std::vector <std::wstring> lst_display;
		lst_display.reserve (lst.size ());
		for (RecentFileList::const_iterator it = lst.begin (); it != lst.end (); ++it) {
			lst_display.push_back (get_nice_pathname (it->filename));
			max_width = maxU (max_width, ucs_width (lst_display.back ()));
		}

		lst_files.set_items (TIARY_STD_MOVE (lst_display), 0, false);
	}

	layout_buttons.add
		(btn_ok, 10, 10)
		(1, 1)
		(btn_cancel, 10, 10)
		(1, 1)
		(btn_remove, 10, 10)
		(1, 1)
		(btn_remove_all, 13, 13)
		;
	layout_main.add
		(lst_files, 1, Layout::UNLIMITED)
		(1, 1)
		(layout_buttons, 3, 3)
		;

	ChainControlsVertical () (lst_files) (btn_ok);
	ChainControlsHorizontalO () (btn_ok) (btn_cancel) (btn_remove) (btn_remove_all);

	lst_files.sig_double_clicked.connect (this, &WindowRecentFiles::slot_ok);
	btn_ok.sig_clicked.connect (this, &WindowRecentFiles::slot_ok);
	btn_cancel.sig_clicked.connect (this, &Window::request_close);
	btn_remove.sig_clicked.connect (this, &WindowRecentFiles::slot_remove);
	btn_remove_all.sig_clicked.connect (this, &WindowRecentFiles::slot_remove_all);

	set_default_button (btn_ok);
	register_hotkey (ESCAPE, btn_cancel.sig_clicked);
	lst_files.register_hotkey (DELETE, btn_remove.sig_clicked);

	WindowRecentFiles::redraw ();
}

WindowRecentFiles::~WindowRecentFiles ()
{
}

void WindowRecentFiles::redraw ()
{
	unsigned items = lst_files.get_items ().size ();

	Size winsize = (make_size (max_width+10, items+6) | make_size (50, 12))
		& get_screen_size ();

	FixedWindow::resize (winsize);

	layout_main.move_resize (make_size (2, 1), winsize - make_size (4, 2));

	Window::redraw ();
}

void WindowRecentFiles::slot_ok ()
{
	size_t select = lst_files.get_select ();
	if (select < lst_orig.size ()) {
		RecentFileList::const_iterator it = lst_orig.begin ();
		std::advance (it, select);
		result = it->filename;
	}
	Window::request_close ();
}

void WindowRecentFiles::slot_remove ()
{
	size_t select = lst_files.get_select ();
	if (select < lst_orig.size ()) {
		RecentFileList::iterator it = lst_orig.begin ();
		std::advance (it, select);
		lst_orig.erase (it);
		ListBox::ItemList lst_display = lst_files.get_items ();
		lst_display.erase (lst_display.begin() + select);
		lst_files.set_items (TIARY_STD_MOVE (lst_display), size_t (-1), false);
		modified = true;
	}
}

void WindowRecentFiles::slot_remove_all ()
{
	lst_orig.clear ();
	modified = true;
	Window::request_close ();
}

} // anonymous namespace

std::wstring dialog_open_recent_file (RecentFileList &lst, bool &lst_modified)
{
	WindowRecentFiles win (lst);
	win.event_loop ();
	lst_modified = win.get_modified ();
	return win.get_result ();
}

} // namespace tiary
