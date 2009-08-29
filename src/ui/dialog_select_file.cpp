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


#include "ui/dialog_select_file.h"
#include "ui/window.h"
#include "ui/label.h"
#include "ui/textbox.h"
#include "ui/listbox.h"
#include "ui/button.h"
#include "ui/button_default.h"
#include "ui/chain.h"
#include "ui/layout.h"
#include "common/dir.h"
#include "ui/dialog_message.h"
#include "common/format.h"
#include "common/algorithm.h"
#include "ui/checkbox.h"
#include "common/container_of.h"
#include <list>

namespace tiary {
namespace ui {

namespace {

/*
[Hint]
[Input/Display box]
[List]
[Show Hidden?][Text][OK][Cancel]
 */

class WindowSelectFile : public virtual Window, private ButtonDefault
{
public:
	WindowSelectFile (const std::wstring &hint_, const std::wstring &default_file, unsigned options_);
	~WindowSelectFile ();

	void redraw ();

	const std::wstring &get_result () const { return result; }

private:
	TextBox text_input;
	ListBox list_files;
	CheckBox check_hidden_files;
	Label lbl_hidden_files;
	Button btn_ok;
	Button btn_cancel;
	Layout layout_main;
	Layout layout_buttons;
	std::wstring result;
	std::wstring list_dir; // Full name of the directory listed in list_files (initial "")
	unsigned options;

	void slot_input ();
	void slot_select ();
	void slot_ok ();
	void slot_refresh ();

	void set_text (const std::wstring &, bool rewrite_input_box = true);
};

const wchar_t string_show_hidden_files[] = L"Show &hidden files";
// Minus 2: Null terminator and the "&" character
const size_t len_show_hidden_files = sizeof string_show_hidden_files / sizeof (wchar_t) - 2;

WindowSelectFile::WindowSelectFile (const std::wstring &hint, const std::wstring &default_file, unsigned options_)
	: Window (0, hint)
	, ButtonDefault ()
	, text_input (*this)
	, list_files (*this)
	, check_hidden_files (*this, true)
	, lbl_hidden_files (*this, string_show_hidden_files)
	, btn_ok (*this, L"&OK")
	, btn_cancel (*this, L"&Cancel")
	, layout_main (VERTICAL)
	, layout_buttons (HORIZONTAL)
	, result (default_file)
	, list_dir ()
	, options (options_)
{
	ChainControlsHorizontal () (check_hidden_files) (btn_ok) (btn_cancel);
	ChainControlsVerticalNC () (text_input) (list_files) (btn_ok);
	check_hidden_files.ctrl_up = btn_cancel.ctrl_up = &list_files;
	check_hidden_files.ctrl_down = btn_cancel.ctrl_down = &text_input;

	text_input.sig_changed.connect (this, &WindowSelectFile::slot_input);
	check_hidden_files.sig_toggled.connect (this, &WindowSelectFile::slot_refresh);
	list_files.sig_focus.connect (this, &WindowSelectFile::slot_select);
	list_files.sig_select_changed.connect (this, &WindowSelectFile::slot_select);
	list_files.sig_double_clicked.connect (this, &WindowSelectFile::slot_ok);
	btn_ok.sig_clicked.connect (this, &WindowSelectFile::slot_ok);
	btn_cancel.sig_clicked.connect (this, &Window::request_close);

	set_default_button (btn_ok);
	register_hotkey (ESCAPE, btn_cancel.sig_clicked);

	lbl_hidden_files.sig_hotkey.connect (
			TIARY_LIST_OF(Signal)
				Signal (check_hidden_files, &CheckBox::toggle, true),
				Signal (this, &Window::set_focus_ptr, &check_hidden_files, 0)
			TIARY_LIST_OF_END
			);
	lbl_hidden_files.sig_clicked.connect (lbl_hidden_files.sig_hotkey);
//	register_hotkey (F5, Signal (this, &WindowSelectFile::slot_refresh));

	layout_buttons.add
		(check_hidden_files, 3, 3, 1, 0)
		(lbl_hidden_files, len_show_hidden_files, len_show_hidden_files, 1, 0)
		(0, 3)
		(btn_ok, 10, 10, 3)
		(0, 2)
		(btn_cancel, 10, 10, 3);

	layout_main.add
		(text_input, 1, 1)
		(1, 1)
		(list_files, 2, Layout::UNLIMITED)
		(1, 1)
		(layout_buttons, 3, 3);

	// redraw before set_text so that the focus is properly positioned
	// (redraw guarantees that list_files has a non-zero height)
	WindowSelectFile::redraw ();

	set_text (default_file);
}

WindowSelectFile::~WindowSelectFile()
{
}

void WindowSelectFile::redraw ()
{
	Size scrsize = get_screen_size ();
	Size size = scrsize & make_size (100, 40);
	move_resize ((scrsize - size) / 2, size);

	layout_main.move_resize (make_size (2,1), size - make_size (4, 2));

	Window::redraw ();
}

void WindowSelectFile::slot_input ()
{
	set_text (text_input.get_text (), false);
}

void WindowSelectFile::slot_select ()
{
	size_t select = list_files.get_select ();
	if (select < list_files.get_items (). size()) {
		std::wstring tmp = list_dir;
		if (tmp.empty() || *c(tmp).rbegin () != L'/')
			tmp += L'/';
		const std::wstring &file = list_files.get_items()[select];
		tmp.append (file, 0,  file.length() - (*file.rbegin () == L'/'));
		set_text (tmp);
	}
}

void WindowSelectFile::slot_ok ()
{
	std::wstring expanded_name = home_expand_pathname (text_input.get_text ());
	unsigned attr = get_file_attr (expanded_name);
	if (attr & FILE_ATTR_DIRECTORY) {
		// Selected a directory. Open it
		set_text (expanded_name + L'/', true);
		if (get_focus () == &list_files)
			slot_select ();
		return;
	}
	if (options & SELECT_FILE_WRITE) {
		// File selected for writing.
		// Possibly need to warn against overwriting
		if (!(attr & FILE_ATTR_NONEXIST) && (options & SELECT_FILE_WARN_OVERWRITE)) {
			// Warn
			if (dialog_message (format (L"File \"%a\" already exists. Overwrite it?") << text_input.get_text(),
						MESSAGE_YES|MESSAGE_NO) != MESSAGE_YES)
				return;
		}
	} else {
		// File selected for reading. Must exist
		if (attr & FILE_ATTR_NONEXIST) {
			dialog_message (L"You must select an existing file.");
			return;
		}
	}
	result.swap (expanded_name);
	request_close ();
}

void WindowSelectFile::slot_refresh ()
{
	list_dir.clear ();
	slot_input ();
}

struct FilterDots : public UnaryCallback<const DirEnt &, bool>
{
	bool is_root;
	bool show_hidden;
	explicit FilterDots (bool is_root_, bool show_hidden_) : is_root (is_root_), show_hidden (show_hidden_) {}
	bool operator () (const DirEnt &ent) const;
};

bool FilterDots::operator () (const DirEnt &ent) const
{
	const std::wstring &name = ent.name;
	if (name[0] == L'.') {
		if (name.length() == 1)
			return true;
		if (name.length() == 2 && name[1] == L'.')
			return is_root;
		return !show_hidden;
	}
	return false;
}

void WindowSelectFile::set_text (const std::wstring &newname, bool rewrite_input_box)
{
	std::wstring newname_expanded = home_expand_pathname (newname);
	std::pair<std::wstring, std::wstring> split_fullname = split_pathname (newname_expanded, true);
	if (rewrite_input_box) {
		std::wstring display_name = combine_pathname (get_nice_pathname (split_fullname.first), split_fullname.second);
		text_input.set_text (display_name, false, display_name.length ());
	}
	if (list_dir != split_fullname.first) {
		list_dir = split_fullname.first;
		// Refresh items in list_files
		DirEntList files = tiary::list_dir (list_dir, FilterDots(list_dir == L"/", check_hidden_files.get_status ()));
		ListBox::ItemList display_list;
		display_list.reserve (files.size ());
		for (DirEntList::iterator it = files.begin();
				it != files.end();
				++it) {
			if (it->attr & FILE_ATTR_DIRECTORY)
				it->name += L'/';
			display_list.push_back (it->name);
		}
		list_files.set_items (TIARY_STD_MOVE (display_list), size_t(-1), false);
	}
	// Select a corresponding item
	// If our item is "r", and we have "rel" and "r", we must make
	// sure "r" instead of "rel" is selected, even though "rel" may
	// be listed first
	size_t select = size_t(-1);
	for (ListBox::ItemList::const_iterator it = list_files.get_items().begin();
			it != list_files.get_items().end();
			++it) {
		if (*it == split_fullname.second) {
			select = it - list_files.get_items().begin();
			break;
		}
		if (select==size_t(-1) &&
				it->compare (0, split_fullname.second.length (), split_fullname.second) == 0)
			select = it - list_files.get_items().begin();
	}
	if (select != list_files.get_select ())
		list_files.set_select (select, false);
}

} // anonymous namespace

std::wstring dialog_select_file (
		const std::wstring &hint,
		const std::wstring &default_file,
		unsigned options
		)
{
	WindowSelectFile win (hint, default_file, options);
	win.event_loop ();
	return win.get_result ();
}

} // namespace tiary::ui
} // namespace tiary
