// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2011, 2016, 2018, 2019 chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/

/**
 * @file	main/mainwin.cpp
 * @author	chys <admin@chys.info>
 * @brief	Implements the tiary::MainWin class
 */

#include "main/mainwin.h"
#include "common/string.h"
#include "ui/dialog_message.h"
#include "ui/dialog_select_file.h"
#include "ui/dialog_input.h"
#include "ui/search_info.h"
#include "common/unicode.h"
#include "common/format.h"
#include "diary/file.h"
#include "diary/diary.h"
#include "diary/config.h"
#include "diary/filter.h"
#include "common/datetime.h"
#include "common/algorithm.h"
#include "common/dir.h"
#include "main/doc.h"
#include "main/dialog_filter.h"
#include "main/dialog_global_pref.h"
#include "main/dialog_perfile_pref.h"
#include "main/dialog_labels.h"
#include "main/dialog_all_labels.h"
#include "main/dialog_edit_time.h"
#include "main/dialog_view_edit.h"
#include "main/dialog_open_recent.h"
#include "main/stat.h"
#include <limits>
#include <unistd.h>

namespace tiary {



MainWin::MainWin(std::wstring_view initial_filename)
	: ui::Window (ui::Window::WINDOW_NO_BORDER)
	, menu_bar (*this)
	, context_menu ()
	, saved (true)
	, filter_()
	, main_ctrl (*this)
	, hotkey_hint (*this)
	, last_search ()
{
	// Frequently used queries
	Condition q_normal (this, &MainWin::query_normal_mode);
	Condition q_filtered (this, &MainWin::query_filter_mode);
	Condition q_nonempty (this, &MainWin::query_nonempty_filtered);
	Condition q_nonempty_all (this, &MainWin::query_nonempty_all);
	Condition q_normal_nonempty = q_normal && q_nonempty_all;
	Condition q_allow_up (this, &MainWin::query_allow_up);
	Condition q_allow_down (this, &MainWin::query_allow_down);
	Condition q_search_continue (this, &MainWin::query_search_continuable);

	// All actions
	Signal action_menu (menu_bar, &ui::MenuBar::slot_clicked, 0);
	Signal action_new_file (this, &MainWin::new_file);
	Signal action_open_file (this, &MainWin::open_file);
	Signal action_open_recent_file (this, &MainWin::open_recent_file);
	Signal action_save (this, &MainWin::default_save);
	Signal action_save_as (this, &MainWin::save_as);
	Signal action_password (this, &MainWin::edit_password);
	Action action_statistics (Signal (this, &MainWin::display_statistics), q_nonempty_all);
	Signal action_quit (this, &MainWin::quit);
	Action action_append (Signal (this, &MainWin::append), q_normal);
	Action action_delete (Signal (this, &MainWin::remove_current), q_normal_nonempty);
	Action action_edit (Signal (this, &MainWin::edit_current), q_nonempty);
	Action action_item_labels (Signal (this, &MainWin::edit_labels_current), q_nonempty);
	Action action_time (Signal (this, &MainWin::edit_time_current), q_nonempty);
	Action action_view (Signal (this, &MainWin::view_current), q_nonempty);
	Action action_view_all (Signal (this, &MainWin::view_all), q_nonempty);
	Action action_move_up (Signal (this, &MainWin::move_up_current), q_normal_nonempty && q_allow_up);
	Action action_move_down (Signal (this, &MainWin::move_down_current), q_normal_nonempty && q_allow_down);
	Action action_sort_all (Signal (this, &MainWin::sort_all), q_normal_nonempty);
	Action action_filter (Signal (this, &MainWin::edit_filter), q_nonempty);
	Action action_clear_filter (Signal (this, &MainWin::clear_filter), q_filtered);
	Action action_search (Signal (this, &MainWin::search, false), q_nonempty);
	Action action_search_backward (Signal (this, &MainWin::search, true), q_nonempty);
	Action action_search_next (Signal (this, &MainWin::search_continue, false), q_search_continue);
	Action action_search_previous (Signal (this, &MainWin::search_continue, true), q_search_continue);
	Action action_all_labels (Signal (this, &MainWin::edit_all_labels), q_nonempty_all);
	Signal action_global_options (this, &MainWin::edit_global_options);
	Signal action_perfile_options (this, &MainWin::edit_perfile_options);
	Signal action_show_doc (&show_doc);
	Signal action_show_license (&show_license);
	Signal action_show_about (&show_about);
	Action action_focus_home (Signal (main_ctrl, &MainCtrl::set_focus, 0), q_nonempty);
	Action action_focus_end (Signal (main_ctrl, &MainCtrl::set_focus, std::numeric_limits<int>::max ()), q_nonempty);
	Action action_up (Signal (main_ctrl, &MainCtrl::set_focus_up), q_nonempty && q_allow_up);
	Action action_down (Signal (main_ctrl, &MainCtrl::set_focus_down), q_nonempty && q_allow_down);
	Action action_pageup (Signal (main_ctrl, &MainCtrl::set_focus_pageup), q_allow_up);
	Action action_pagedown (Signal (main_ctrl, &MainCtrl::set_focus_pagedown), q_allow_down);

	menu_bar.add(L"&File"sv)
		(L"&New              Ctrl+N"sv, action_new_file)
		(L"&Open...          Ctrl+O"sv, action_open_file)
		(L"Open &recent file..."sv    , action_open_recent_file)
		()
		(L"&Save           w Ctrl+S"sv, action_save)
		(L"Save &as...     W"sv,        action_save_as)
		()
		(L"&Password...    p"sv,        action_password)
		()
		(L"S&tatistics     s"sv,        action_statistics)
		()
		(L"&Quit           q Ctrl+Q"sv, action_quit)
		;
	context_menu = &menu_bar.add(L"&Entry"sv);
	(*context_menu)
		(L"&New entry      a INSERT"sv, action_append)
		(L"&Delete         d DELETE"sv, action_delete)
		()
		(L"&Edit           e"sv,        action_edit)
		(L"Edit &labels... l"sv,        action_item_labels)
		(L"Edit t&ime...   t"sv,        action_time)
		()
		(L"&View           v ENTER"sv,  action_view)
		(L"View &all       V"sv,        action_view_all)
		()
		(L"&Move up        m"sv,        action_move_up)
		(L"Move dow&n      M"sv,        action_move_down)
		(L"&Sort all       S"sv,        action_sort_all)
		;
	menu_bar.add(L"&View"sv)
		(L"&Filter...        CtrL+G"sv, action_filter)
		(L"&Clear filter     LEFT"sv,   action_clear_filter)
		;
	menu_bar.add(L"&Search"sv)
		(L"&Find...        / Ctrl+F"sv, action_search)
		(L"Find &next      n F3"sv,     action_search_next)
		(L"Find &previous  N"sv,        action_search_previous)
		;
	menu_bar.add(L"Se&ttings"sv)
		(L"&Labels...      L"sv,        action_all_labels)
		()
		(L"&Preferences... R"sv,        action_global_options)
		(L"&File perferences..."sv,     action_perfile_options)
		;
	menu_bar.add(L"&Help"sv)
		(L"&Help             F1"sv,     action_show_doc)
		()
		(L"&License"sv,                 action_show_license)
		(L"&About"sv,                   action_show_about)
		;

	main_ctrl.register_hotkey (ui::ESCAPE,   action_menu);
	main_ctrl.register_hotkey (L'a',         action_append);
	main_ctrl.register_hotkey (L'A',         action_append);
	main_ctrl.register_hotkey (ui::INSERT,   action_append);
	main_ctrl.register_hotkey (L'b',         action_pageup);
	main_ctrl.register_hotkey (ui::PAGEUP,   action_pageup);
	main_ctrl.register_hotkey (L'd',         action_delete);
	main_ctrl.register_hotkey (L'D',         action_delete);
	main_ctrl.register_hotkey (ui::DELETE,   action_delete);
	main_ctrl.register_hotkey (L'e',         action_edit);
	main_ctrl.register_hotkey (L'E',         action_edit);
	main_ctrl.register_hotkey (L'f',         action_pagedown);
	main_ctrl.register_hotkey (ui::PAGEDOWN, action_pagedown);
	main_ctrl.register_hotkey (L'g',         action_focus_home);
	main_ctrl.register_hotkey (L'^',         action_focus_home);
	main_ctrl.register_hotkey (L'<',         action_focus_home);
	main_ctrl.register_hotkey (ui::HOME,     action_focus_home);
	main_ctrl.register_hotkey (L'G',         action_focus_end);
	main_ctrl.register_hotkey (L'$',         action_focus_end);
	main_ctrl.register_hotkey (L'>',         action_focus_end);
	main_ctrl.register_hotkey (ui::END,      action_focus_end);
	main_ctrl.register_hotkey (L'h',         action_show_doc);
	main_ctrl.register_hotkey (L'H',         action_show_doc);
	main_ctrl.register_hotkey (ui::F1,       action_show_doc);
	main_ctrl.register_hotkey (L'j',         action_down);
	main_ctrl.register_hotkey (ui::DOWN,     action_down);
	main_ctrl.register_hotkey (L'k',         action_up);
	main_ctrl.register_hotkey (ui::UP,       action_up);
	main_ctrl.register_hotkey (L'l',         action_item_labels);
	main_ctrl.register_hotkey (L'L',         action_all_labels);
	main_ctrl.register_hotkey (L'm',         action_move_up);
	main_ctrl.register_hotkey (L'M',         action_move_down);
	main_ctrl.register_hotkey (L'n',         action_search_next);
	main_ctrl.register_hotkey (ui::F3,       action_search_next);
	main_ctrl.register_hotkey (L'N',         action_search_previous);
	main_ctrl.register_hotkey (L'p',         action_password);
	main_ctrl.register_hotkey (L'P',         action_password);
	main_ctrl.register_hotkey (L'q',         action_quit);
	main_ctrl.register_hotkey (L'Q',         action_quit);
	main_ctrl.register_hotkey (ui::CTRL_Q,   action_quit);
	main_ctrl.register_hotkey (L'r',         action_perfile_options);
	main_ctrl.register_hotkey (L'R',         action_global_options);
	main_ctrl.register_hotkey (L's',         action_statistics);
	main_ctrl.register_hotkey (L'S',         action_sort_all);
	main_ctrl.register_hotkey (L't',         action_time);
	main_ctrl.register_hotkey (L'T',         action_time);
	main_ctrl.register_hotkey (L'v',         action_view);
	main_ctrl.register_hotkey (ui::RETURN,   action_view);
	main_ctrl.register_hotkey (ui::NEWLINE,  action_view);
	main_ctrl.register_hotkey (ui::RIGHT,    action_view);
	main_ctrl.register_hotkey (L'V',         action_view_all);
	main_ctrl.register_hotkey (L'w',         action_save);
	main_ctrl.register_hotkey (ui::CTRL_S,   action_save);
	main_ctrl.register_hotkey (L'W',         action_save_as);
	main_ctrl.register_hotkey (ui::CTRL_F,   action_search);
	main_ctrl.register_hotkey (L'/',         action_search);
	main_ctrl.register_hotkey (L'?',         action_search_backward);
	main_ctrl.register_hotkey (ui::CTRL_G,   action_filter);
	main_ctrl.register_hotkey (ui::CTRL_N,   action_new_file);
	main_ctrl.register_hotkey (ui::CTRL_O,   action_open_file);
	main_ctrl.register_hotkey (ui::LEFT,     action_clear_filter);

	hotkey_hint
		(8000, L"Esc"sv,     L"Menu"sv,             std::move(action_menu))
		(7000, L"LEFT"sv,    L"Clear filter"sv,     std::move(action_clear_filter))
		(1000, L"a"sv,       L"New entry"sv,        std::move(action_append))
		(1000, L"e"sv,       L"Edit"sv,             std::move(action_edit))
		(1000, L"d"sv,       L"Delete"sv,           std::move(action_delete))
		(500,  L"^G"sv,      L"Filter"sv,           std::move(action_filter))
		(1000, L"/"sv,       L"Search"sv,           std::move(action_search))
		(2000, L"n"sv,       L"Next"sv,             std::move(action_search_next))
		(1000, L"t"sv,       L"Time"sv,             std::move(action_time))
		(1000, L"l"sv,       L"Labels"sv,           std::move(action_item_labels))
		(500,  L"L"sv,       L"Manage labels"sv,    std::move(action_all_labels))
		(1000, L"m"sv,       L"Move up"sv,          std::move(action_move_up))
		(1000, L"M"sv,       L"Move down"sv,        std::move(action_move_down))
		(0,    L"s"sv,       L"Statistics"sv,       std::move(action_statistics))
		(500,  L"S"sv,       L"Sort"sv,             std::move(action_sort_all))
		(500,  L"p"sv,       L"Password"sv,         std::move(action_password))
		(0,    L"F1"sv,      L"Help"sv,             std::move(action_show_doc))
		(8000, L"q"sv,       L"Quit"sv,             std::move(action_quit))
		;

	MainWin::redraw ();

	switch (load_global_options (global_options, recent_files)) {
		case LOAD_FILE_SUCCESS:
			break;
		case LOAD_FILE_NOT_FOUND: // No file. Just use the defaults
			break;
		default: // Warning, and use the defaults
			ui::dialog_message(format(L"WARNING: Config file ~/%a reading error. Using defaults."sv,
					TIARY_WIDIFY(GLOBAL_OPTION_FILE)));
			break;
	}

	if (!initial_filename.empty()) {
		load(initial_filename);
	} else {
		std::wstring filename = utf8_to_wstring(global_options.get(GLOBAL_OPTION_DEFAULT_FILE));
		if (!filename.empty()) {
			load(filename);
		}
	}
}

MainWin::~MainWin ()
{
	for (DiaryEntry *entry: entries) {
		delete entry;
	}
}

void MainWin::redraw ()
{
	ui::Size scrsize = ui::get_screen_size ();
	scrsize -= ui::Size{1, 0};
	move_resize(ui::Size{}, scrsize);
	menu_bar.move_resize(ui::Size{}, ui::Size{scrsize.x, 1});
	main_ctrl.move_resize(ui::Size{0, 1}, scrsize - ui::Size{0, 2});
	hotkey_hint.move_resize(ui::Size{0, scrsize.y - 1}, ui::Size{scrsize.x, 1});
	Window::redraw ();
}

void MainWin::on_ready ()
{
	// Update the status string on the right-hand side of the menubar
	// [+] [Filtering] [Filename]
	std::wstring status;
	if (!saved) {
		status = L"+ "sv;
	}
	if (filter_) {
		status += L"[Filter] "sv;
	}
	if (current_filename_.empty ()) {
		status += L"<New file>"sv;
	} else {
		status += get_nice_pathname(current_filename_);
	}
	menu_bar.set_text (std::move (status));
	hotkey_hint.redraw ();
}

void MainWin::updated_filter ()
{
	if (filter_) {
		filtered_entries_.reset(new DiaryEntryList(filter_->filter(entries)));
		main_ctrl.modify_number(filtered_entries_->size ());
	} else {
		filtered_entries_.reset ();
		main_ctrl.modify_number(entries.size ());
	}
	redraw ();
}

bool MainWin::unavailable_filtered ()
{
	if (filter_) {
		ui::dialog_message (
				L"This operation cannot be done in filtered mode.\n"
				L"Pressed LEFT to return to normal mode."sv);
		return false;
	}
	return true;
}

void MainWin::load(std::wstring_view filename) {
	// Clear everything that's been loaded.
	reset_file ();

	std::wstring full_filename = get_full_pathname (filename);
	std::wstring nice_filename = get_nice_pathname (full_filename);
	std::wstring error_info;

	auto enter_password = [&nice_filename]() -> std::string {
		return wstring_to_utf8(ui::dialog_input2(
				L"Enter password"sv,
				format(L"File \"%a\" is password protected. Please enter the password:"sv, nice_filename),
				std::wstring(),
				35,
				ui::INPUT_PASSWORD));
	};

	LoadFileRet load_ret = load_file (wstring_to_mbs (full_filename).c_str (),
				enter_password,
				entries,
				per_file_options,
				password_);
	switch (load_ret) {
		case LOAD_FILE_DEPRECATED:
		case LOAD_FILE_SUCCESS:
			current_filename_ = full_filename;
			main_ctrl.touch ();
			saved = true;
			{
				auto it = std::find(recent_files.begin(), recent_files.end(), current_filename_);
				if (it != recent_files.end ()) {
					main_ctrl.set_focus (it->focus_entry);
				}
			}
			if (load_ret == LOAD_FILE_DEPRECATED) {
				// Do it here so that the content has been drawn on screen
				ui::dialog_message(L"This file is stored in a cryptographically non-secure format, dating back from 2009.\n"
					L"Please re-save as soon as possible, but please note that the re-saved file cannot be read "
					L"by old versions of tiary."sv);
			}
			return;

		case LOAD_FILE_NOT_FOUND: // Not found. Warning
			error_info = format(L"File not found: %a"sv, nice_filename);
			break;
		case LOAD_FILE_PASSWORD: // Password incorrect.
			error_info = L"Incorrect password."sv;
			sleep (1);
			break;
		case LOAD_FILE_READ_ERROR:
			error_info = format(L"Cannot read file: %a"sv, nice_filename);
			break;
		case LOAD_FILE_BUNZIP2:
		case LOAD_FILE_XML:
			error_info = format(L"File format error: %a"sv, nice_filename);
			break;
		case LOAD_FILE_DECRYPTION:
			error_info = format(L"File decryption error: %a"sv, nice_filename);
			break;
		case LOAD_FILE_CONTENT:
			error_info = format(L"File content error: %a\n"
					L"This may be due to a bug. If possible, please send a copy of this file"
					L" to chys <admin@chys.info> so that we can fix the problem and help "
					L"recover the contents."sv, nice_filename);
			break;
	}
	reset_file ();
	main_ctrl.touch ();
	saved = true;
	ui::Window::touch_windows ();
	if (!error_info.empty ()) {
		ui::dialog_message (error_info);
	}
}

void MainWin::save(std::wstring_view filename) {
	std::wstring_view fmt;
	if (save_file(wstring_to_mbs(filename).c_str(), entries, per_file_options, password_)) {
		current_filename_ = filename;
		saved = true;
		fmt = L"Successfully saved \"%a\"."sv;
	} else {
		fmt = L"Cannot save file \"%a\"."sv;
	}
	ui::dialog_message(format(fmt, filename));
}

void MainWin::default_save ()
{
	if (current_filename_.empty()) {
		save_as ();
	} else {
		save(current_filename_);
	}
}

void MainWin::save_as ()
{
	std::wstring filename = ui::dialog_select_file (
			L"Save as"sv,
			current_filename_,
			ui::SELECT_FILE_WRITE | ui::SELECT_FILE_WARN_OVERWRITE);
	if (!filename.empty ()) {
		filename = get_full_pathname (filename);
		save (filename);
		update_recent_files ();
	}
}

void MainWin::append ()
{
	if (!unavailable_filtered ())
		return;
	DiaryEntry *ent = new DiaryEntry{
		DateTime(DateTime::LOCAL, time(nullptr)),
		std::wstring(L"Your title goes here."sv),
		std::wstring(L"Your contents go here."sv),
		{}
	};
	if (edit_entry (*ent, global_options.get (GLOBAL_OPTION_EDITOR).c_str())
			&& (!ent->title.empty () || !ent->text.empty ())) {
		entries.push_back (ent);
		main_ctrl.touch ();
		main_ctrl.set_focus (std::numeric_limits<int>::max ());
	}
	else {
		delete ent;
	}
}

DiaryEntryList &MainWin::get_current_list ()
{
	if (filtered_entries_) {
		return *filtered_entries_;
	}
	else {
		return entries;
	}
}

const DiaryEntryList &MainWin::get_current_list () const
{
	if (filtered_entries_) {
		return *filtered_entries_;
	}
	else {
		return entries;
	}
}

DiaryEntry *MainWin::get_current ()
{
	DiaryEntryList &lst = get_current_list ();
	if (lst.empty ()) {
		return 0;
	}
	else {
		return lst[main_ctrl.get_current_focus ()];
	}
}

void MainWin::edit_current ()
{
	if (DiaryEntry *ent = get_current ()) {
		DateTime edit_time = DateTime (DateTime::LOCAL);
		if (edit_entry (*ent, global_options.get (GLOBAL_OPTION_EDITOR).c_str())) {
			if (per_file_options.get_bool (PERFILE_OPTION_MODTIME)) {
				ent->local_time = edit_time;
			}
			updated_filter ();
			main_ctrl.touch ();
		}
	}
}

void MainWin::edit_labels_current ()
{
	if (DiaryEntry *ent = get_current ()) {
		if (edit_labels (ent->labels, entries)) {
			updated_filter ();
			main_ctrl.touch ();
		}
	}
}

void MainWin::edit_time_current ()
{
	if (DiaryEntry *ent = get_current ()) {
		if (edit_entry_time (*ent)) {
			main_ctrl.touch ();
		}
	}
}


void MainWin::remove_current ()
{
	if (!unavailable_filtered ()) {
		return;
	}
	if (entries.empty ()) {
		return;
	}
	size_t k = main_ctrl.get_current_focus ();
	if (ui::dialog_message (
				L"Are you sure to remove the currently selected entry?"sv,
				ui::MESSAGE_YES|ui::MESSAGE_NO|ui::MESSAGE_DEFAULT_NO) == ui::MESSAGE_YES) {
		delete entries[k];
		entries.erase (entries.begin () + k);
		main_ctrl.touch ();
	}
}

void MainWin::move_up_current ()
{
	if (!unavailable_filtered ()) {
		return;
	}
	if (entries.size () < 2) {
		return;
	}
	size_t k = main_ctrl.get_current_focus ();
	if (k == 0) {
		return;
	}
	std::swap (entries[k-1], entries[k]);
	main_ctrl.touch ();
	main_ctrl.set_focus (k-1);
}

void MainWin::move_down_current ()
{
	if (!unavailable_filtered ()) {
		return;
	}
	if (entries.size () < 2) {
		return;
	}
	size_t k = main_ctrl.get_current_focus ();
	if (k+1 >= entries.size ()) {
		return;
	}
	std::swap (entries[k+1], entries[k]);
	main_ctrl.touch ();
	main_ctrl.set_focus (k+1);
}

namespace {

struct CompareEntry {
	bool operator () (const DiaryEntry *a, const DiaryEntry *b) const
	{
		return (a->local_time < b->local_time);
	}
};

} // anonymous namespace

void MainWin::sort_all ()
{
	if (!unavailable_filtered ()) {
		return;
	}
	if (ui::dialog_message(L"Are you sure you want to sort all entries by time? This operation cannot be undone."sv,
				ui::MESSAGE_YES|ui::MESSAGE_NO|ui::MESSAGE_DEFAULT_NO) == ui::MESSAGE_YES) {
		std::stable_sort (entries.begin (), entries.end (), CompareEntry ());
		main_ctrl.touch ();
	}
}

void MainWin::view_current ()
{
	if (DiaryEntry *ent = get_current ()) {
		view_entry (*ent, global_options.get_wstring (GLOBAL_OPTION_LONGTIME_FORMAT));
	}
}

void MainWin::view_all ()
{
	DiaryEntryList &lst = get_current_list ();
	if (!lst.empty ()) {
		view_all_entries (lst, global_options.get_wstring (GLOBAL_OPTION_LONGTIME_FORMAT));
	}
}

/*
 * This function should be called when the user seems to be
 * discarding changes to the file.
 *
 * Return value:
 * true: Continue to the next operation
 * false: Cancel the next operation
 */
bool MainWin::check_save ()
{
	update_recent_files ();
	if (saved) {
		return true;
	}
	switch (ui::dialog_message(
				format(L"Save changes to \"%a\"?"sv, (
					current_filename_.empty() ? L"<Untitled>"sv : std::wstring_view(current_filename_)
				)),
				ui::MESSAGE_YES|ui::MESSAGE_NO|ui::MESSAGE_CANCEL)) {
		case ui::MESSAGE_YES:
			default_save ();
			return saved;
		case ui::MESSAGE_NO:
			return true;
		case ui::MESSAGE_CANCEL:
		default:
			return false;
	}
}

void MainWin::update_recent_files ()
{
	if (current_filename_.empty ())
		return;
	bool changed = true; // Whether any change was made to recent_files
	unsigned current_focus = main_ctrl.get_current_focus ();
	auto it = std::find(recent_files.begin(), recent_files.end(), current_filename_);
	if (it == recent_files.end ()) {
		recent_files.insert(recent_files.begin(), {current_filename_, current_focus});
		changed = true;
	} else {
		if (it != recent_files.begin ()) {
			std::rotate(recent_files.begin(), it, it + 1);
			it = recent_files.begin ();
			changed = true;
		}
		if (current_focus != it->focus_entry) {
			it->focus_entry = current_focus;
			changed = true;
		}
	}

	unsigned n_files = global_options.get_num (GLOBAL_OPTION_RECENT_FILES);
	if (recent_files.size() > n_files) {
		recent_files.resize(n_files);
		changed = true;
	}
	if (changed) {
		save_global_options (global_options, recent_files);
	}
}

void MainWin::new_file ()
{
	if (check_save ()) {
		reset_file ();
		main_ctrl.touch ();
		saved = true;
	}
}

void MainWin::open_file ()
{
	if (check_save ()) {
		std::wstring new_filename = ui::dialog_select_file (
				L"Open"sv,
				std::wstring_view(),
				ui::SELECT_FILE_READ);
		if (!new_filename.empty ()) {
			load (new_filename);
		}
	}
}

void MainWin::open_recent_file ()
{
	if (check_save ()) {
		unsigned n_recent_files = recent_files.size ();
		if (n_recent_files == 0) {
			ui::dialog_message(L"No recent file"sv);
			return;
		}
		bool modified = false;
		std::wstring file = dialog_open_recent_file (recent_files, modified);
		if (modified) {
			save_global_options (global_options, recent_files);
		}
		if (!file.empty ()) {
			load (file);
		}
	}
}

void MainWin::edit_filter ()
{
	if (entries.empty ()) {
		return;
	}
	if (!filter_) {
		filter_.reset(new FilterGroup);
	}
	dialog_filter(entries, *filter_);
	if (filter_->empty ()) {
		filter_.reset ();
	}
	updated_filter ();
}

void MainWin::clear_filter ()
{
	if (filter_) {
		filter_.reset();
		updated_filter ();
	}
}

void MainWin::search (bool bkwd)
{
	if (get_current_list ().empty ()) {
		return;
	}
	if (last_search.dialog (bkwd)) {
		do_search (false, true);
	}
}

void MainWin::search_continue (bool bkwd)
{
	if (!last_search) {
		search (bkwd);
	}
	else {
		do_search (bkwd, false);
	}
}

void MainWin::do_search (bool bkwd, bool include_current_entry)
{
	DiaryEntryList &entry_list = get_current_list ();
	unsigned num_ents = entry_list.size ();
	if (num_ents == 0) {
		return;
	}
	if (!last_search) {
		return;
	}
	unsigned k = main_ctrl.get_current_focus ();
	int inc = (!bkwd == !last_search.get_backward ()) ? 1 : -1;
	if (!include_current_entry) {
		k += inc;
	}
	for (; k < num_ents; k += inc) {
		if (last_search.basic_match (entry_list[k]->title) || last_search.basic_match (entry_list[k]->text)) {
			main_ctrl.set_focus (k);
			return;
		}
	}
	// Not found.
	ui::dialog_message(L"Not found."sv);
}

void MainWin::reset_file ()
{
	per_file_options.reset ();
	current_filename_.clear ();
	password_.clear();
	for (DiaryEntry *entry: entries) {
		delete entry;
	}
	entries.clear ();
}

void MainWin::edit_password ()
{
	if (!password_.empty ()) {
		std::wstring old_password = ui::dialog_input(L"Please enter your old password:"sv,
				std::wstring_view(), 35, ui::INPUT_PASSWORD, std::wstring_view());
		if (old_password.empty ()) {
			return;
		}
		if (wstring_to_utf8(old_password) != password_) {
			ui::dialog_message(L"Incorrect password."sv);
			return;
		}
	}

	std::wstring new_password1 = ui::dialog_input(L"Please enter your new password:"sv,
			std::wstring_view(), 35, ui::INPUT_PASSWORD, L"\n\r"sv);
	if (new_password1 == L"\n\r"sv) { // Canceled
		return;
	}

	std::wstring_view info;
	if (new_password1.empty ()) {
		if (ui::dialog_message(L"Are you sure you want to remove the password?"sv,
					ui::MESSAGE_YES|ui::MESSAGE_NO) == ui::MESSAGE_YES) {
			password_.clear ();
			main_ctrl.touch ();
			info = L"Password removed."sv;
		}
	}
	else {
		std::wstring new_password2 = ui::dialog_input(L"Please enter again:"sv,
				std::wstring_view(), 35, ui::INPUT_PASSWORD, std::wstring_view());
		if (new_password1 == new_password2) {
			password_ = wstring_to_utf8(new_password1);
			main_ctrl.touch ();
			info = L"Password changed."sv;
		} else {
			info = L"Password NOT changed."sv;
		}
	}
	if (!info.empty()) {
		ui::dialog_message (info);
	}
}

void MainWin::edit_all_labels ()
{
	if (tiary::edit_all_labels (entries)) {
		main_ctrl.touch ();
	}
}

void MainWin::edit_global_options ()
{
	tiary::edit_global_options(global_options, current_filename_);
	save_global_options (global_options, recent_files);
	main_ctrl.redraw();
}

void MainWin::edit_perfile_options ()
{
	if (tiary::edit_perfile_options (per_file_options)) {
		main_ctrl.touch ();
	}
}

void MainWin::display_statistics ()
{
	if (!entries.empty ()) {
		tiary::display_statistics(entries, filtered_entries_.get(), get_current());
	}
}

void MainWin::quit ()
{
	if (check_save ()) {
		request_close ();
	}
}

bool MainWin::query_normal_mode () const
{
	return !filter_;
}

bool MainWin::query_filter_mode () const
{
	return static_cast<bool>(filter_);
}

bool MainWin::query_nonempty_filtered () const
{
	return !get_current_list ().empty ();
}

bool MainWin::query_nonempty_all () const
{
	return !entries.empty ();
}

bool MainWin::query_allow_up () const
{
	return main_ctrl.get_current_focus ();
}

bool MainWin::query_allow_down () const
{
	return (main_ctrl.get_current_focus () + 1 < get_current_list ().size ());
}

bool MainWin::query_search_continuable () const
{
	return (query_nonempty_filtered() && !last_search.get_matcher().get_pattern().empty());
}

} // namespace tiary
