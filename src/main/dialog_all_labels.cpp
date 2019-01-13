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

#include "main/dialog_all_labels.h"
#include "ui/button.h"
#include "ui/button_default.h"
#include "ui/window.h"
#include "ui/listbox.h"
#include "ui/fixed_window.h"
#include "ui/label.h"
#include "ui/layout.h"
#include "ui/chain.h"
#include "ui/dialog_input.h"
#include "ui/dialog_message.h"
#include "diary/diary.h"
#include "common/format.h"
#include "common/containers.h"
#include "common/string.h"

/**
 * @file	main/dialog_all_labels.cpp
 * @author	chys <admin@chys.info>
 * @brief	Implements the dialog allowing the user to control all labels
 */

namespace tiary {


namespace {

using namespace ui;

class WindowAllLabels final : public virtual Window, public FixedWindow, private ButtonDefault {

	ListBox lst_labels;

	Button btn_rename;
	Button btn_delete;
	Button btn_ok;

	Layout layout_main;
	Layout layout_right;

	DiaryEntryList &entries;

	WStringLocaleOrderedSet all_labels;

	bool modified;

public:
	WindowAllLabels (DiaryEntryList &);
	~WindowAllLabels ();

	void redraw ();

	void slot_rename ();
	void slot_delete ();
	void slot_ok ();

	bool get_modified () const { return modified; }

private:
	void refresh_list (const std::wstring &select_hint = std::wstring ());
};

WindowAllLabels::WindowAllLabels (DiaryEntryList &entries_)
	: Window(0, L"All labels"sv)
	, FixedWindow ()
	, ButtonDefault ()
	, lst_labels (*this)
	, btn_rename(*this, L"&Rename"sv)
	, btn_delete(*this, L"&Delete"sv)
	, btn_ok(*this, L"&OK"sv)
	, layout_main (HORIZONTAL)
	, layout_right (VERTICAL)
	, entries (entries_)
	, all_labels ()
	, modified (false)
{
	for (DiaryEntryList::const_iterator it = entries.begin (); it != entries.end (); ++it) {
		all_labels.insert ((*it)->labels.begin (), (*it)->labels.end ());
	}
	refresh_list ();

	layout_right.add({
			{btn_rename, 3, 3},
			{1, 1},
			{btn_delete, 3, 3},
			{1, Layout::UNLIMITED},
			{btn_ok, 3, 3},
		});
	layout_main.add({
			{lst_labels, 1, Layout::UNLIMITED},
			{1, 1},
			{layout_right, 10, 10},
		});

	btn_rename.sig_clicked = btn_delete.sig_clicked = Condition (lst_labels, &ListBox::is_valid_select);
	btn_rename.sig_clicked.connect (this, &WindowAllLabels::slot_rename);
	btn_delete.sig_clicked.connect (this, &WindowAllLabels::slot_delete);
	btn_ok.sig_clicked.connect (this, &WindowAllLabels::slot_ok);

	lst_labels.register_hotkey (DELETE, btn_delete.sig_clicked);
	lst_labels.sig_select_changed.connect([this] {
			btn_rename.redraw();
			btn_delete.redraw();
		});

	register_hotkey (ESCAPE, btn_ok.sig_clicked);
	set_default_button (btn_ok);
}

WindowAllLabels::~WindowAllLabels ()
{
}

void WindowAllLabels::redraw ()
{
	Size size = get_screen_size () & Size{40, 40};
	FixedWindow::resize (size);
	layout_main.move_resize({2, 1}, size - Size{4, 2});
	Window::redraw ();
}

void WindowAllLabels::slot_rename ()
{
	size_t k = lst_labels.get_select ();
	if (k < lst_labels.get_items ().size ()) {
		const std::wstring &old_name = lst_labels.get_items () [k];
		std::wstring new_name = dialog_input2 (
				L"Rename label"sv,
				std::wstring(format(L"Enter the new name for \"%a\":") << old_name),
				old_name,
				32);
		if (new_name.empty () || new_name == old_name) {
			return;
		}

		const wchar_t *warning_template;
		WindowMessageButton msg_buttons;

		if (all_labels.find (new_name) != all_labels.end ()) { // This label already exists
			warning_template = L"Label \"%b\" already exists. Are you sure you want to merge \"%a\" into \"%b\"?\n"
						L"This operation cannot be undone!";
			msg_buttons = MESSAGE_YES|MESSAGE_NO|MESSAGE_DEFAULT_NO;
		}
		else {
			warning_template = L"Are you sure you want to rename \"%a\" to \"%b\"?";
			msg_buttons = MESSAGE_YES|MESSAGE_NO;
		}

		if (dialog_message(std::wstring(format(warning_template) << old_name << new_name),
					L"Rename label"sv, msg_buttons) == MESSAGE_YES) {

			all_labels.erase (old_name);
			all_labels.insert (new_name);

			for (DiaryEntryList::iterator it = entries.begin (); it != entries.end (); ++it) {
				DiaryEntry::LabelList &labels = (*it)->labels;
				DiaryEntry::LabelList::iterator jt = labels.find (old_name);
				if (jt != labels.end ()) {
					labels.erase (jt);
					labels.insert (new_name);
				}
			}
			modified = true;
			refresh_list (new_name);
			touch_windows (); // Reflect changes in MainWin
		}
	}
}

void WindowAllLabels::slot_delete ()
{
	size_t k = lst_labels.get_select ();
	if (k < lst_labels.get_items ().size ()) {
		const std::wstring &old_name = lst_labels.get_items () [k];
		if (dialog_message(std::wstring(format (L"Are you sure you want to delete label \"%a\"?\nThis operation cannot be undone!")
					<< old_name), L"Delete label"sv, MESSAGE_YES|MESSAGE_NO) == MESSAGE_YES) {
			all_labels.erase (old_name);
			for (DiaryEntryList::iterator it = entries.begin (); it != entries.end (); ++it) {
				(*it)->labels.erase (old_name);
			}
			modified = true;
			refresh_list ();
			touch_windows (); // Reflect changes in MainWin
		}
	}
}

void WindowAllLabels::slot_ok ()
{
	Window::request_close ();
}

void WindowAllLabels::refresh_list (const std::wstring &select_hint)
{
	size_t new_select;
	WStringLocaleOrderedSet::const_iterator it = all_labels.find (select_hint);
	if (it == all_labels.end ()) {
		new_select = size_t (-1);
	}
	else {
		new_select = std::distance (all_labels.begin (), it);
	}
	lst_labels.set_items (std::vector<std::wstring> (all_labels.begin (), all_labels.end ()),
			new_select, true);
}

} // anonymous namespace

bool edit_all_labels (DiaryEntryList &entries)
{
	WindowAllLabels win (entries);
	win.event_loop ();
	return win.get_modified ();
}

} // namespace tiary
