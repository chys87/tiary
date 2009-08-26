// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the so-called 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#include "main/dialog_tags.h"
#include "diary/diary.h"
#include "ui/dialog.h"
#include "ui/layout.h"
#include "ui/label.h"
#include "ui/button.h"
#include "ui/listbox.h"
#include "ui/chain.h"
#include "common/algorithm.h"
#include "ui/dialog_input.h"
#include "common/string.h"

namespace tiary {

namespace {

using namespace ui;

/**
 * @brief	Class for the Tag editing dialog
 */
class DialogTags : public Dialog
{

	WStringLocaleOrderedSet &tags;
	WStringLocaleOrderedSet &all_tags;

	Label lbl_selected;
	ListBox lst_selected;
	Button btn_add;
	Button btn_remove;
	Button btn_new;
	Button btn_ok;
	Label lbl_all;
	ListBox lst_all;

	Layout layout_main;
	Layout layout_left;
	Layout layout_middle;
	Layout layout_right;

public:

	// all_tags will be clobbered
	DialogTags (WStringLocaleOrderedSet &tags_, WStringLocaleOrderedSet &all_tags_);
	~DialogTags ();

	void redraw ();
	void on_winch ();

	void slot_add ();
	void slot_remove ();
	void slot_new ();

	static void refresh_list (ListBox &lst_box, const WStringLocaleOrderedSet &tags, const std::wstring &selection);
};

DialogTags::DialogTags (WStringLocaleOrderedSet &tags_, WStringLocaleOrderedSet &all_tags_)
	: ui::Dialog (0, L"Tags")
	, tags (tags_)
	, all_tags (all_tags_)
	, lbl_selected (*this, L"Selected tags")
	, lst_selected (*this)
	, btn_add (*this, L"&Add")
	, btn_remove (*this, L"&Remove")
	, btn_new (*this, L"&New tag")
	, btn_ok (*this, L"&OK")
	, lbl_all (*this, L"All tags")
	, lst_all (*this)
	, layout_main (HORIZONTAL)
	, layout_left (VERTICAL)
	, layout_middle (VERTICAL)
	, layout_right (VERTICAL)
{
	refresh_list (lst_selected, tags_, std::wstring ());
	refresh_list (lst_all, all_tags_, std::wstring ());

	lst_selected.ctrl_right = &btn_remove;
	lst_all.ctrl_left = &btn_add;
	btn_add.ctrl_left = btn_remove.ctrl_left = btn_new.ctrl_left = btn_ok.ctrl_left = &lst_selected;
	btn_add.ctrl_right = btn_remove.ctrl_right = btn_new.ctrl_right = btn_ok.ctrl_right = &lst_all;
	ChainControlsVertical () (btn_add) (btn_remove) (btn_new) (btn_ok);

	layout_left.add
		(lbl_selected, 1, 1)
		(1, 1)
		(lst_selected, 2, Layout::UNLIMITED)
		;
	layout_middle.add
		(2, 2)
		(btn_add, 3, 3, 10)
		(1, 1)
		(btn_remove, 3, 3, 10)
		(1, 1)
		(btn_new, 3, 3, 10)
		(1, Layout::UNLIMITED)
		(btn_ok, 3, 3, 10)
		;
	layout_right.add
		(lbl_all, 1, 1)
		(1, 1)
		(lst_all, 2, Layout::UNLIMITED)
		;
	layout_main.add
		(layout_left, 2, Layout::UNLIMITED)
		(2, 2)
		(layout_middle, 10, 10)
		(2, 2)
		(layout_right, 2, Layout::UNLIMITED)
		;

	btn_new.sig_clicked.connect (this, &DialogTags::slot_new);
	btn_ok.set_attribute (Button::DEFAULT_BUTTON);
	btn_ok.sig_clicked.connect (this, &Window::request_close);

	Signal tmp_sig (this, &DialogTags::slot_remove);
	lst_selected.sig_double_clicked = tmp_sig;
	lst_selected.register_hotkey (NEWLINE, tmp_sig, Hotkeys::CASE_SENSITIVE|Hotkeys::DISALLOW_ALT);
	lst_selected.register_hotkey (RETURN, tmp_sig, Hotkeys::CASE_SENSITIVE|Hotkeys::DISALLOW_ALT);
	btn_remove.sig_clicked = TIARY_STD_MOVE (tmp_sig);

	tmp_sig.connect (this, &DialogTags::slot_add);
	lst_all.sig_double_clicked = tmp_sig;
	lst_all.register_hotkey (NEWLINE, tmp_sig, Hotkeys::CASE_SENSITIVE|Hotkeys::DISALLOW_ALT);
	lst_all.register_hotkey (RETURN, tmp_sig, Hotkeys::CASE_SENSITIVE|Hotkeys::DISALLOW_ALT);
	btn_add.sig_clicked = TIARY_STD_MOVE (tmp_sig);

	register_hotkey (ESCAPE, Signal (this, &Window::request_close));

	DialogTags::redraw ();
}

DialogTags::~DialogTags ()
{
}

void DialogTags::redraw ()
{
	Size scrsize = get_screen_size ();
	Size size = scrsize & make_size (80, 25);
	move_resize ((scrsize - size) / 2, size);
	layout_main.move_resize (make_size (2,1), size - make_size (4,2));
	Dialog::redraw ();
}

void DialogTags::on_winch ()
{
	DialogTags::redraw ();
}

void DialogTags::slot_add ()
{
	size_t k = lst_all.get_select ();
	if (k != size_t (-1)) {
		const std::wstring &tag = lst_all.get_items () [k];
		if (tags.insert (tag).second)
			refresh_list (lst_selected, tags, tag);
	}
}

void DialogTags::slot_remove ()
{
	size_t k = lst_selected.get_select ();
	if (k != size_t (-1)) {
		const std::wstring &tag = lst_selected.get_items () [k];
		tags.erase (tag);
		refresh_list (lst_selected, tags, std::wstring ());
	}
}

void DialogTags::slot_new ()
{
	std::wstring name = dialog_input (L"Please enter the name of the new tag:", std::wstring (), 40);
	strip (name);
	if (!name.empty ()) {
		if (tags.insert (name).second) {
			refresh_list (lst_selected, tags, name);
			if (all_tags.insert (name).second)
				refresh_list (lst_all, all_tags, name);
		}
	}
}

void DialogTags::refresh_list (ListBox &lst_box, const WStringLocaleOrderedSet &tags, const std::wstring &selection)
{
	std::vector <std::wstring> lst (tags.begin (), tags.end ());
	size_t new_select = size_t (-1);
	if (!selection.empty ()) {
		std::vector <std::wstring>::const_iterator pos = std::lower_bound (lst.begin (), lst.end (), selection, std::locale ());
		if (pos!=lst.end() && *pos==selection)
			new_select = pos - lst.begin ();
	}
	lst_box.set_items (TIARY_STD_MOVE (lst), new_select, false);
}

} // anonymous namespace

bool edit_tags (WStringLocaleOrderedSet &tags, const std::vector<DiaryEntry *> &entries)
{
	WStringLocaleOrderedSet old_tags = tags;
	WStringLocaleOrderedSet all_tags; // The union of all tag lists
	for (std::vector<DiaryEntry *>::const_iterator it = entries.begin (); it != entries.end (); ++it)
		all_tags.insert ((*it)->tags.begin (), (*it)->tags.end ());
	DialogTags win (tags, all_tags);
	win.event_loop ();
	return (tags != old_tags);
}

} // namespace tiary
