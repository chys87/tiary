// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2018, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/



/**
 * @file	main/dialog_filter.cpp
 * @author	chys <admin@chys.info>
 * @brief	Implements a dialog allowing the user to edit filters
 */

#include "main/dialog_filter.h"
#include "diary/diary.h"
#include "diary/filter.h"
#include "ui/droplist.h"
#include "ui/label.h"
#include "ui/textbox.h"
#include "ui/button.h"
#include "ui/window.h"
#include "ui/fixed_window.h"
#include "ui/checkbox_label.h"
#include "ui/button_default.h"
#include "ui/dialog_message.h"
#include "ui/dialog_select.h"
#include "ui/layout.h"
#include "ui/chain.h"
#include "common/format.h"

namespace tiary {

namespace {

using namespace ui;

class DialogFilter : public FixedWindow, private ButtonDefault
{
	const DiaryEntry::LabelList &all_labels;
	FilterGroup &result;

	Label lbl_label;
	TextBox txt_label;
	Button btn_label;
	Layout layout_label;

	Label lbl_title;
	TextBox txt_title;
	Layout layout_title;
#ifdef TIARY_USE_PCRE
	CheckBoxLabel chk_title_regex;
	Layout layout_title_regex;
#endif

	Label lbl_text;
	TextBox txt_text;
	Layout layout_text;
#ifdef TIARY_USE_PCRE
	CheckBoxLabel chk_text_regex;
	Layout layout_text_regex;
#endif

	Button btn_ok;
	Button btn_cancel;
	Layout layout_buttons;

	Layout layout_main;

	void slot_ok ();
	void slot_choose_label ();

public:
	DialogFilter (const DiaryEntry::LabelList &, FilterGroup &);
	~DialogFilter ();

	void redraw ();
};

DialogFilter::DialogFilter (const DiaryEntry::LabelList &all_labels_, FilterGroup &result_)
	: Window (0, L"Filtering")
	, FixedWindow ()
	, ButtonDefault ()
	, all_labels (all_labels_)
	, result (result_)
	, lbl_label (*this, L"L&abel:")
	, txt_label (*this)
	, btn_label (*this, L"C&hoose")
	, layout_label (HORIZONTAL)
	, lbl_title (*this, L"&Title")
	, txt_title (*this)
	, layout_title (HORIZONTAL)
#ifdef TIARY_USE_PCRE
	, chk_title_regex (*this, L"&Regular expression", false)
	, layout_title_regex (HORIZONTAL)
#endif
	, lbl_text (*this, L"&Content:")
	, txt_text (*this)
	, layout_text (HORIZONTAL)
#ifdef TIARY_USE_PCRE
	, chk_text_regex (*this, L"Regular e&xpression", false)
	, layout_text_regex (HORIZONTAL)
#endif
	, btn_ok (*this, L"&OK")
	, btn_cancel (*this, L"Cancel")
	, layout_buttons (HORIZONTAL)
	, layout_main (VERTICAL)
{
	// Assign control values
	for (FilterGroup::const_iterator it = result_.begin (); it != result_.end (); ++it) {
		if (FilterByLabel *filter_lbl = dynamic_cast<FilterByLabel *>(it->get())) {
			txt_label.set_text (filter_lbl->label, false, filter_lbl->label.length ());
		}
		else if (FilterByTitle *filter_title = dynamic_cast<FilterByTitle *>(it->get())) {
			txt_title.set_text (filter_title->get_pattern (), false, filter_title->get_pattern ().length ());
#ifdef TIARY_USE_PCRE
			chk_title_regex.checkbox.set_status (filter_title->get_use_regex ());
#endif
		}
		else if (FilterByText *filter_text = dynamic_cast<FilterByText *>(it->get())) {
			txt_text.set_text (filter_text->get_pattern (), false, filter_text->get_pattern ().length ());
#ifdef TIARY_USE_PCRE
			chk_text_regex.checkbox.set_status (filter_text->get_use_regex ());
#endif
		}
	}

	// Setting up layouts
	layout_label.add
		(lbl_label, 10, 10)
		(1, 1)
		(txt_label, 1, Layout::UNLIMITED)
		(1, 1)
		(btn_label, 10, 10)
		;
	layout_title.add
		(lbl_title, 10, 10)
		(1, 1)
		(txt_title, 1, Layout::UNLIMITED)
		;
#ifdef TIARY_USE_PCRE
	layout_title_regex.add
		(11, 11)
		(chk_title_regex, 3, Layout::UNLIMITED)
		;
#endif
	layout_text.add
		(lbl_text, 10, 10)
		(1, 1)
		(txt_text, 1, Layout::UNLIMITED)
		;
#ifdef TIARY_USE_PCRE
	layout_text_regex.add
		(11, 11)
		(chk_text_regex, 3, Layout::UNLIMITED)
		;
#endif
	layout_buttons.add
		(btn_ok, 10, 10)
		(1, 1)
		(btn_cancel, 10, 10)
		;

	layout_main.add
		(layout_label, 1, 1)
		(1, 1)
		(layout_title, 1, 1)
#ifdef TIARY_USE_PCRE
		(layout_title_regex, 1, 1)
#endif
		(1, 1)
		(layout_text, 1, 1)
#ifdef TIARY_USE_PCRE
		(layout_text_regex, 1, 1)
#endif
		(1, 1)
		(layout_buttons, 3, 3)
		;

	// Setting up chains
	ChainControlsVertical ()
		(txt_label)
		(txt_title)
#ifdef TIARY_USE_PCRE
		(chk_title_regex.checkbox)
#endif
		(txt_text)
#ifdef TIARY_USE_PCRE
		(chk_text_regex.checkbox)
#endif
		(btn_ok)
		;
	ChainControlsHorizontal () (txt_label) (btn_label);
	ChainControlsHorizontal () (btn_ok) (btn_cancel);

	btn_label.ctrl_up = txt_label.ctrl_up;
	btn_label.ctrl_down = txt_label.ctrl_down;
	btn_cancel.ctrl_up = btn_ok.ctrl_up;
	btn_cancel.ctrl_down = btn_ok.ctrl_down;

	// Setting up signals
	btn_label.sig_clicked.connect (this, &DialogFilter::slot_choose_label);
	btn_ok.sig_clicked.connect (this, &DialogFilter::slot_ok);
	btn_cancel.sig_clicked.connect (this, &Window::request_close);
	set_default_button (btn_ok);
	register_hotkey (ESCAPE, btn_cancel.sig_clicked);

	DialogFilter::redraw ();
}

DialogFilter::~DialogFilter ()
{
}

void DialogFilter::redraw ()
{
	Size size = Size{40, 13} & get_screen_size ();
	FixedWindow::resize (size);
	layout_main.move_resize({2, 1}, size - Size{4, 2});
	Window::redraw ();
}

void DialogFilter::slot_ok ()
{
	FilterGroup new_filter;

	if (!txt_label.get_text ().empty ()) {
		FilterByLabel *filter = new FilterByLabel;
		filter->label = txt_label.get_text ();
		new_filter.emplace_back(filter);
	}
	if (!txt_title.get_text ().empty ()) {
		FilterByTitle *filter = new FilterByTitle;
		bool assign_ret = filter->assign (txt_title.get_text ()
#ifdef TIARY_USE_PCRE
				, chk_title_regex.get_status ()
#endif
				);
#ifdef TIARY_USE_PCRE
		if (!assign_ret) {
			dialog_message (format (L"Invalid regular expression: \"%a\"") << txt_title.get_text ());
			set_focus_ptr (&txt_title);
			return;
		}
#endif
		new_filter.emplace_back(filter);
	}
	if (!txt_text.get_text ().empty ()) {
		FilterByText *filter = new FilterByText;
		bool assign_ret = filter->assign (txt_text.get_text ()
#ifdef TIARY_USE_PCRE
				, chk_text_regex.get_status ()
#endif
				);
#ifdef TIARY_USE_PCRE
		if (!assign_ret) {
			dialog_message (format (L"Invalid regular expression: \"%a\"") << txt_text.get_text ());
			set_focus_ptr (&txt_text);
			return;
		}
#endif
		new_filter.emplace_back(filter);
	}

	new_filter.relation = FilterGroup::AND;

	result = std::move(new_filter);

	Window::request_close ();
}

void DialogFilter::slot_choose_label ()
{
	std::vector <std::wstring> label_list (all_labels.begin (), all_labels.end ());
	size_t choice = dialog_select (L"Select a label", label_list);
	if (choice < label_list.size ()) {
		const std::wstring &label = label_list[choice];
		txt_label.set_text (label, false, label.size ());
	}
}

} // anonoymous namespace

void dialog_filter (const DiaryEntryList &entries, FilterGroup &filter)
{
	DiaryEntry::LabelList all_labels;
	for (DiaryEntryList::const_iterator it = entries.begin (); it != entries.end (); ++it) {
		all_labels.insert ((*it)->labels.begin (), (*it)->labels.end ());
	}
	DialogFilter (all_labels, filter).event_loop ();
}



} // namespace tiary
