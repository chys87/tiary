// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2016, 2018, 2019, chys <admin@CHYS.INFO>
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
#include "ui/dialog_message.h"
#include "ui/checkbox_label.h"
#include "common/algorithm.h"
#include "common/dir.h"
#include "common/format.h"
#include "common/string.h"
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

class WindowSelectFile final : public virtual Window, private ButtonDefault
{
public:
	WindowSelectFile(std::wstring_view hint, std::wstring_view default_file, unsigned options_);
	~WindowSelectFile ();

	void redraw() override;

	const std::wstring &get_result() const { return result_; }
	std::wstring &&move_result() { return std::move(result_); }

private:
	void slot_input();
	void slot_select();
	void slot_ok();
	void slot_refresh();

	void set_text(std::wstring_view, bool rewrite_input_box = true);

private:
	TextBox text_input_;
	ListBox list_files_;
	CheckBoxLabel check_hidden_files_;
	Button btn_ok_;
	Button btn_cancel_;
	Layout layout_main_;
	Layout layout_buttons_;
	std::wstring result_;
	std::wstring list_dir_; // Full name of the directory listed in list_files_ (initial "")
	unsigned options_;
};

const wchar_t string_show_hidden_files[] = L"Show &hidden files";
// Minus 2: Null terminator and the "&" character
const size_t len_show_hidden_files = sizeof string_show_hidden_files / sizeof (wchar_t) - 2;

WindowSelectFile::WindowSelectFile(std::wstring_view hint, std::wstring_view default_file, unsigned options)
	: Window (0, hint)
	, ButtonDefault ()
	, text_input_(*this)
	, list_files_(*this)
	, check_hidden_files_(*this, std::wstring_view(string_show_hidden_files, sizeof(string_show_hidden_files) - 1), true)
	, btn_ok_(*this, L"&OK"sv)
	, btn_cancel_(*this, L"&Cancel"sv)
	, layout_main_(VERTICAL)
	, layout_buttons_(HORIZONTAL)
	, result_(default_file)
	, list_dir_()
	, options_(options)
{
	ChainControlsHorizontal{&check_hidden_files_.checkbox, &btn_ok_, &btn_cancel_};
	ChainControlsVerticalNC{&text_input_, &list_files_, &btn_ok_};
	check_hidden_files_.checkbox.ctrl_up = btn_cancel_.ctrl_up = &list_files_;
	check_hidden_files_.checkbox.ctrl_down = btn_cancel_.ctrl_down = &text_input_;

	text_input_.sig_changed.connect(this, &WindowSelectFile::slot_input);
	check_hidden_files_.checkbox.sig_toggled.connect(this, &WindowSelectFile::slot_refresh);
	list_files_.sig_focus.connect(this, &WindowSelectFile::slot_select);
	list_files_.sig_select_changed.connect(this, &WindowSelectFile::slot_select);
	list_files_.sig_double_clicked.connect(this, &WindowSelectFile::slot_ok);
	btn_ok_.sig_clicked.connect(this, &WindowSelectFile::slot_ok);
	btn_cancel_.sig_clicked.connect(this, &Window::request_close);

	set_default_button(btn_ok_);
	register_hotkey(ESCAPE, btn_cancel_.sig_clicked);

//	register_hotkey (F5, Signal (this, &WindowSelectFile::slot_refresh));

	layout_buttons_.add({
			{check_hidden_files_, 4 + len_show_hidden_files, 4 + len_show_hidden_files, 1, 0},
			{0, 3},
			{btn_ok_, 10, 10, 3},
			{0, 2},
			{btn_cancel_, 10, 10, 3},
		});

	layout_main_.add({
			{text_input_, 1, 1},
			{1, 1},
			{list_files_, 2, Layout::UNLIMITED},
			{1, 1},
			{layout_buttons_, 3, 3},
		});

	// redraw before set_text so that the focus is properly positioned
	// (redraw guarantees that list_files_ has a non-zero height)
	WindowSelectFile::redraw ();

	set_text (default_file);
}

WindowSelectFile::~WindowSelectFile()
{
}

void WindowSelectFile::redraw ()
{
	Size scrsize = get_screen_size ();
	Size size = scrsize & Size{100, 40};
	move_resize ((scrsize - size) / 2, size);

	layout_main_.move_resize({2, 1}, size - Size{4, 2});

	Window::redraw ();
}

void WindowSelectFile::slot_input ()
{
	set_text(text_input_.get_text(), false);
}

void WindowSelectFile::slot_select ()
{
	size_t select = list_files_.get_select();
	if (select < list_files_.get_items(). size()) {
		std::wstring tmp = list_dir_;
		if (tmp.empty() || *tmp.rbegin() != L'/') {
			tmp += L'/';
		}
		const std::wstring &file = list_files_.get_items()[select];
		tmp.append (file, 0,  file.length() - (*file.rbegin () == L'/'));
		set_text (tmp);
	}
}

void WindowSelectFile::slot_ok ()
{
	std::wstring expanded_name = home_expand_pathname(text_input_.get_text());
	unsigned attr = get_file_attr (expanded_name);
	if (attr & FILE_ATTR_DIRECTORY) {
		// Selected a directory. Open it
		set_text (expanded_name + L'/', true);
		if (get_focus () == &list_files_) {
			slot_select ();
		}
		return;
	}
	if (options_ & SELECT_FILE_WRITE) {
		// File selected for writing.
		// Possibly need to warn against overwriting
		if (!(attr & FILE_ATTR_NONEXIST) && (options_ & SELECT_FILE_WARN_OVERWRITE)) {
			// Warn
			if (dialog_message(format(L"File \"%a\" already exists. Overwrite it?"sv, text_input_.get_text()),
						MESSAGE_YES|MESSAGE_NO|MESSAGE_DEFAULT_NO) != MESSAGE_YES) {
				return;
			}
		}
	}
	else {
		// File selected for reading. Must exist
		if (attr & FILE_ATTR_NONEXIST) {
			dialog_message(L"You must select an existing file."sv);
			return;
		}
	}
	result_ = std::move(expanded_name);
	request_close ();
}

void WindowSelectFile::slot_refresh ()
{
	list_dir_.clear();
	slot_input ();
}

struct FilterDots
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
		if (name.length() == 1) {
			return true;
		}
		if (name.length() == 2 && name[1] == L'.') {
			return is_root;
		}
		return !show_hidden;
	}
	return false;
}

void WindowSelectFile::set_text(std::wstring_view newname, bool rewrite_input_box) {
	std::wstring newname_expanded = home_expand_pathname (newname);
	std::pair<std::wstring, std::wstring> split_fullname = split_pathname (newname_expanded, true);
	if (rewrite_input_box) {
		std::wstring display_name = combine_pathname (get_nice_pathname (split_fullname.first), split_fullname.second);
		text_input_.set_text(display_name, false, display_name.length());
	}
	if (list_dir_ != split_fullname.first) {
		list_dir_ = split_fullname.first;
		// Refresh items in list_files_
		DirEntList files = list_dir(list_dir_, FilterDots(list_dir_ == L"/"sv, check_hidden_files_.get_status()));
		ListBox::ItemList display_list;
		display_list.reserve (files.size ());
		for (DirEnt &ent: files) {
			if (ent.attr & FILE_ATTR_DIRECTORY) {
				ent.name += L'/';
			}
			display_list.push_back(ent.name);
		}
		list_files_.set_items(std::move(display_list), size_t(-1), false);
	}
	// Select a corresponding item
	// If our item is "r", and we have "rel" and "r", we must make
	// sure "r" instead of "rel" is selected, even though "rel" may
	// be listed first
	size_t select = size_t(-1);
	for (const std::wstring &item: list_files_.get_items()) {
		if (item == split_fullname.second) {
			select = &item - &list_files_.get_items().front();
			break;
		}
		if (select==size_t(-1) &&
				std::wstring_view(item).substr(0, split_fullname.second.length()) == split_fullname.second) {
			select = &item - &list_files_.get_items().front();
		}
	}
	if (select != list_files_.get_select()) {
		list_files_.set_select(select, false);
	}
}

} // anonymous namespace

std::wstring dialog_select_file (
		std::wstring_view hint,
		std::wstring_view default_file,
		unsigned options) {
	WindowSelectFile win (hint, default_file, options);
	win.event_loop ();
	return std::move(win.move_result());
}

} // namespace tiary::ui
} // namespace tiary
