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


#include "main/dialog_labels.h"
#include "diary/diary.h"
#include "ui/dialog.h"
#include "ui/layout.h"
#include "ui/label.h"
#include "ui/button.h"
#include "ui/button_default.h"
#include "ui/listbox.h"
#include "ui/chain.h"
#include "ui/textbox.h"
#include "common/algorithm.h"
#include "common/string.h"

namespace tiary {

namespace {

using namespace ui;

/**
 * @brief	Class for the Label editing dialog
 */
class DialogLabels : public virtual Dialog, private ButtonDefault
{

	WStringLocaleOrderedSet &labels;
	const WStringLocaleOrderedSet &all_labels;

	Label lbl_selected;
	TextBox txt_selected;
	Label lbl_all;
	ListBox lst_all;

	Button btn_add;
	Button btn_ok;
	Button btn_cancel;

	Layout layout_main;
	Layout layout_buttons;

public:

	DialogLabels (WStringLocaleOrderedSet &labels_, const WStringLocaleOrderedSet &all_labels_);
	~DialogLabels ();

	void redraw ();
	void on_winch ();

	void slot_add ();
	void slot_ok ();

	static void refresh_list (ListBox &lst_box, const WStringLocaleOrderedSet &labels, const std::wstring &selection);
};

DialogLabels::DialogLabels (WStringLocaleOrderedSet &labels_, const WStringLocaleOrderedSet &all_labels_)
	: ui::Dialog (0, L"Labels")
	, ButtonDefault ()
	, labels (labels_)
	, all_labels (all_labels_)
	, lbl_selected (*this, L"&Labels for this entry:")
	, txt_selected (*this)
	, lbl_all (*this, L"All la&bels")
	, lst_all (*this)
	, btn_add (*this, L"&Add")
	, btn_ok (*this, L"&OK")
	, btn_cancel (*this, L"&Cancel")
	, layout_main (VERTICAL)
	, layout_buttons (HORIZONTAL)
{
	txt_selected.set_text (join (labels_.begin (), labels_.end (), L','));
	lst_all.set_items (std::vector <std::wstring> (all_labels_.begin (), all_labels_.end ()), size_t(-1), false);

	ChainControlsVertical () (txt_selected) (lst_all) (btn_ok);
	ChainControlsHorizontal () (btn_ok) (btn_cancel);

	layout_buttons.add
		(btn_add, 10, 10)
		(1, Layout::UNLIMITED)
		(btn_ok, 10, 10)
		(1, Layout::UNLIMITED)
		(btn_cancel, 10, 10)
		;

	layout_main.add
		(lbl_selected, 1, 1)
		(txt_selected, 1, 1)
		(1, 1)
		(lbl_all, 1, 1)
		(lst_all, 1, Layout::UNLIMITED)
		(1, 1)
		(layout_buttons, 3, 3)
		;

	set_default_button (btn_ok);
	set_special_default_button (lst_all, btn_add);

	btn_ok.sig_clicked.connect (this, &DialogLabels::slot_ok);

	btn_add.sig_clicked = lst_all.sig_double_clicked = Signal (this, &DialogLabels::slot_add);

	btn_cancel.sig_clicked.connect (this, &Window::request_close);
	register_hotkey (ESCAPE, btn_cancel.sig_clicked);

	DialogLabels::redraw ();
}

DialogLabels::~DialogLabels ()
{
}

void DialogLabels::redraw ()
{
	Size scrsize = get_screen_size ();
	Size size = scrsize & make_size (80, 25);
	move_resize ((scrsize - size) / 2, size);
	layout_main.move_resize (make_size (2,1), size - make_size (4,2));
	Dialog::redraw ();
}

void DialogLabels::on_winch ()
{
	DialogLabels::redraw ();
}

WStringLocaleOrderedSet set_from_text (const std::wstring &text)
{
	std::list<std::wstring> lst = split_string (text, L',');
	std::for_each (lst.begin (), lst.end (), (void (*)(std::wstring &))strip);
	WStringLocaleOrderedSet set (lst.begin (), lst.end ());
	set.erase (std::wstring ());
	return set;
}

void DialogLabels::slot_add ()
{
	size_t k = lst_all.get_select ();
	if (k < lst_all.get_items().size ()) {
		const std::wstring &label = lst_all.get_items () [k];
		WStringLocaleOrderedSet current_set = set_from_text (txt_selected.get_text ());
		if (current_set.find (label) == current_set.end ()) {
			std::wstring new_text = txt_selected.get_text () + L',' + label;
			txt_selected.set_text (new_text, false, new_text.size ());
		}
	}
}

void DialogLabels::slot_ok ()
{
	labels = set_from_text (txt_selected.get_text ());
	Window::request_close ();
}

} // anonymous namespace

bool edit_labels (WStringLocaleOrderedSet &labels, const std::vector<DiaryEntry *> &entries)
{
	WStringLocaleOrderedSet old_labels = labels;
	WStringLocaleOrderedSet all_labels; // The union of all label lists
	for (std::vector<DiaryEntry *>::const_iterator it = entries.begin (); it != entries.end (); ++it)
		all_labels.insert ((*it)->labels.begin (), (*it)->labels.end ());
	DialogLabels (labels, all_labels).event_loop ();
	return (labels != old_labels);
}

} // namespace tiary
