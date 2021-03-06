// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2016, 2018, 2019, 2020, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#include "main/dialog_labels.h"
#include "diary/diary.h"
#include "ui/window.h"
#include "ui/layout.h"
#include "ui/label.h"
#include "ui/button.h"
#include "ui/button_default.h"
#include "ui/listbox.h"
#include "ui/chain.h"
#include "ui/textbox.h"
#include "common/algorithm.h"
#include "common/string.h"
#include <functional>

namespace tiary {

namespace {

using namespace ui;

/**
 * @brief	Class for the Label editing dialog
 */
class WindowLabels : public virtual Window, private ButtonDefault {

	WStringLocaleOrderedSet &labels;

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

	WindowLabels (WStringLocaleOrderedSet &labels_, const WStringLocaleOrderedSet &all_labels_);
	~WindowLabels ();

	void redraw ();

	void slot_add ();
	void slot_ok ();
};

WindowLabels::WindowLabels (WStringLocaleOrderedSet &labels_, const WStringLocaleOrderedSet &all_labels_)
	: ui::Window(0, L"Labels"sv)
	, ButtonDefault()
	, labels (labels_)
	, lbl_selected(*this, L"&Labels for this entry:"sv)
	, txt_selected (*this)
	, lbl_all(*this, L"All la&bels"sv)
	, lst_all (*this)
	, btn_add(*this, L"&Add"sv)
	, btn_ok(*this, L"&OK"sv)
	, btn_cancel(*this, L"&Cancel"sv)
	, layout_main (VERTICAL)
	, layout_buttons (HORIZONTAL)
{
	txt_selected.set_text (join (labels_.begin (), labels_.end (), L','), false, -1u);
	lst_all.set_items (std::vector <std::wstring> (all_labels_.begin (), all_labels_.end ()), size_t(-1), false);

	ChainControlsVertical{&txt_selected, &lst_all, &btn_ok};
	ChainControlsHorizontalO{&btn_ok, &btn_cancel, &btn_add};

	layout_buttons.add({
			{btn_add, 10, 10},
			{1, Layout::UNLIMITED},
			{btn_ok, 10, 10},
			{1, Layout::UNLIMITED},
			{btn_cancel, 10, 10},
		});

	layout_main.add({
			{lbl_selected, 1, 1},
			{txt_selected, 1, 1},
			{1, 1},
			{lbl_all, 1, 1},
			{lst_all, 1, Layout::UNLIMITED},
			{1, 1},
			{layout_buttons, 3, 3},
		});

	set_default_button(&btn_ok, {{&lst_all, &btn_add}});

	btn_ok.sig_clicked.connect (this, &WindowLabels::slot_ok);

	btn_add.sig_clicked = Condition (lst_all, &ListBox::is_valid_select);
	btn_add.sig_clicked = lst_all.sig_double_clicked = Signal (this, &WindowLabels::slot_add);
	lst_all.sig_select_changed.connect (btn_add, &Button::redraw);

	btn_cancel.sig_clicked.connect (this, &Window::request_close);
	register_hotkey (ESCAPE, btn_cancel.sig_clicked);
}

WindowLabels::~WindowLabels ()
{
}

void WindowLabels::redraw ()
{
	Size scrsize = get_screen_size ();
	Size size = scrsize & Size{80, 25};
	move_resize ((scrsize - size) / 2, size);
	layout_main.move_resize({2,1}, size - Size{4,2});
	Window::redraw ();
}

WStringLocaleOrderedSet set_from_text (const std::wstring &text)
{
	WStringLocaleOrderedSet set;

	for (std::wstring_view label : split_string_view(text, L',')) {
		auto stripped_label = strip(label);
		if (!stripped_label.empty()) {
			set.emplace(stripped_label);
		}
	}

	return set;
}

void WindowLabels::slot_add ()
{
	size_t k = lst_all.get_select ();
	if (k < lst_all.get_items().size ()) {
		const std::wstring &label = lst_all.get_items () [k];
		WStringLocaleOrderedSet current_set = set_from_text (txt_selected.get_text ());
		if (current_set.find (label) == current_set.end ()) {
			std::wstring new_text = txt_selected.get_text ();
			if (!new_text.empty () && *new_text.rbegin () != L',') {
				new_text += L',';
			}
			new_text += label;
			txt_selected.set_text (std::move (new_text), false, new_text.size ());
		}
	}
}

void WindowLabels::slot_ok ()
{
	labels = set_from_text (txt_selected.get_text ());
	Window::request_close ();
}

} // anonymous namespace

bool edit_labels (WStringLocaleOrderedSet &labels, const std::vector<DiaryEntry *> &entries)
{
	WStringLocaleOrderedSet old_labels = labels;
	WStringLocaleOrderedSet all_labels; // The union of all label lists
	for (std::vector<DiaryEntry *>::const_iterator it = entries.begin (); it != entries.end (); ++it) {
		all_labels.insert ((*it)->labels.begin (), (*it)->labels.end ());
	}
	WindowLabels (labels, all_labels).event_loop ();
	return (labels != old_labels);
}

} // namespace tiary
