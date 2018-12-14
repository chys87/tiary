// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2011, 2016, 2018 chys <admin@CHYS.INFO>
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



MainWin::MainWin (const std::wstring &initial_filename)
	: ui::Window (ui::Window::WINDOW_NO_BORDER)
	, menu_bar (*this)
	, context_menu ()
	, saved (true)
	, filter ()
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

	menu_bar.add (L"&File")
		(L"&New              Ctrl+N", action_new_file)
		(L"&Open...          Ctrl+O", action_open_file)
		(L"Open &recent file..."    , action_open_recent_file)
		()
		(L"&Save           w Ctrl+S", action_save)
		(L"Save &as...     W",        action_save_as)
		()
		(L"&Password...    p",        action_password)
		()
		(L"S&tatistics     s",        action_statistics)
		()
		(L"&Quit           q Ctrl+Q", action_quit)
		;
	context_menu = &menu_bar.add (L"&Entry");
	(*context_menu)
		(L"&New entry      a INSERT", action_append)
		(L"&Delete         d DELETE", action_delete)
		()
		(L"&Edit           e",        action_edit)
		(L"Edit &labels... l",        action_item_labels)
		(L"Edit t&ime...   t",        action_time)
		()
		(L"&View           v ENTER",  action_view)
		(L"View &all       V",        action_view_all)
		()
		(L"&Move up        m",        action_move_up)
		(L"Move dow&n      M",        action_move_down)
		(L"&Sort all       S",        action_sort_all)
		;
	menu_bar.add (L"&View")
		(L"&Filter...        CtrL+G", action_filter)
		(L"&Clear filter     LEFT",   action_clear_filter)
		;
	menu_bar.add (L"&Search")
		(L"&Find...        / Ctrl+F", action_search)
		(L"Find &next      n F3",     action_search_next)
		(L"Find &previous  N",        action_search_previous)
		;
	menu_bar.add (L"Se&ttings")
		(L"&Labels...      L",        action_all_labels)
		()
		(L"&Preferences... R",        action_global_options)
		(L"&File perferences...",     action_perfile_options)
		;
	menu_bar.add (L"&Help")
		(L"&Help             F1",     action_show_doc)
		()
		(L"&License",                 action_show_license)
		(L"&About",                   action_show_about)
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
		(8000, L"Esc",     L"Menu",             std::move (action_menu))
		(7000, L"LEFT",    L"Clear filter",     std::move (action_clear_filter))
		(1000, L"a",       L"New entry",        std::move (action_append))
		(1000, L"e",       L"Edit",             std::move (action_edit))
		(1000, L"d",       L"Delete",           std::move (action_delete))
		(500,  L"^G",      L"Filter",           std::move (action_filter))
		(1000, L"/",       L"Search",           std::move (action_search))
		(2000, L"n",       L"Next",             std::move (action_search_next))
		(1000, L"t",       L"Time",             std::move (action_time))
		(1000, L"l",       L"Labels",           std::move (action_item_labels))
		(500,  L"L",       L"Manage labels",    std::move (action_all_labels))
		(1000, L"m",       L"Move up",          std::move (action_move_up))
		(1000, L"M",       L"Move down",        std::move (action_move_down))
		(0,    L"s",       L"Statistics",       std::move (action_statistics))
		(500,  L"S",       L"Sort",             std::move (action_sort_all))
		(500,  L"p",       L"Password",         std::move (action_password))
		(0,    L"F1",      L"Help",             std::move (action_show_doc))
		(8000, L"q",       L"Quit",             std::move (action_quit))
		;

	MainWin::redraw ();

	switch (load_global_options (global_options, recent_files)) {
		case LOAD_FILE_SUCCESS:
			break;
		case LOAD_FILE_NOT_FOUND: // No file. Just use the defaults
			break;
		default: // Warning, and use the defaults
			ui::dialog_message (format (L"WARNING: Config file ~/%a reading error. Using defaults.")
					<< TIARY_WIDIFY (GLOBAL_OPTION_FILE));
			break;
	}

	std::wstring filename = initial_filename;
	if (filename.empty ()) {
		filename = utf8_to_wstring (global_options.get (GLOBAL_OPTION_DEFAULT_FILE));
	}
	if (!filename.empty ()) {
		load (filename);
	}
}

MainWin::~MainWin ()
{
	std::for_each (entries.begin (), entries.end (), delete_fun <DiaryEntry> ());
}

void MainWin::redraw ()
{
	ui::Size scrsize = ui::get_screen_size ();
	scrsize -= ui::make_size (1, 0);
	move_resize (ui::make_size (), scrsize);
	menu_bar.move_resize (ui::make_size (), ui::make_size (scrsize.x, 1));
	main_ctrl.move_resize (ui::make_size (0, 1), scrsize - ui::make_size (0, 2));
	hotkey_hint.move_resize (ui::make_size (0, scrsize.y-1), ui::make_size (scrsize.x, 1));
	Window::redraw ();
}

void MainWin::on_ready ()
{
	// Update the status string on the right-hand side of the menubar
	// [+] [Filtering] [Filename]
	std::wstring status;
	if (!saved) {
		status = L"+ ";
	}
	if (filter.get ()) {
		status += L"[Filter] ";
	}
	if (current_filename.empty ()) {
		status += L"<New file>";
	}
	else {
		status += get_nice_pathname (current_filename);
	}
	menu_bar.set_text (std::move (status));
	hotkey_hint.HotkeyHint::redraw ();
}

void MainWin::updated_filter ()
{
	if (filter.get ()) {
		filtered_entries.reset (new DiaryEntryList (filter->filter (entries)));
		main_ctrl.ui::Scroll::modify_number (filtered_entries->size ());
	}
	else {
		filtered_entries.reset ();
		main_ctrl.ui::Scroll::modify_number (entries.size ());
	}
	MainWin::redraw ();
}

bool MainWin::unavailable_filtered ()
{
	if (filter.get ()) {
		ui::dialog_message (
				L"This operation cannot be done in filtered mode.\n"
				L"Pressed LEFT to return to normal mode.");
		return false;
	}
	return true;
}

namespace {

struct EnterPassword
{
	const std::wstring &filename;
	EnterPassword (const std::wstring &fname) : filename (fname) { }
	std::wstring operator () () const;
};

std::wstring EnterPassword::operator () () const
{
	return ui::dialog_input2 (
			L"Enter password",
			format (L"File \"%a\" is password protected. Please enter the password:") << filename,
			std::wstring (),
			35,
			ui::INPUT_PASSWORD);
}

} // anonymous namespace

void MainWin::load (const std::wstring &filename)
{
	// Clear everything that's been loaded.
	reset_file ();

	std::wstring full_filename = get_full_pathname (filename);
	std::wstring nice_filename = get_nice_pathname (full_filename);
	std::wstring error_info;
	switch (load_file (wstring_to_mbs (full_filename).c_str (),
				EnterPassword (nice_filename),
				entries,
				per_file_options,
				password)) {
		case LOAD_FILE_SUCCESS:
			current_filename = full_filename;
			main_ctrl.touch ();
			saved = true;
			{
				RecentFileList::const_iterator it = std::find (recent_files.begin (),
						recent_files.end (), current_filename);
				if (it != recent_files.end ()) {
					main_ctrl.set_focus (it->focus_entry);
				}
			}
			return;

		case LOAD_FILE_NOT_FOUND: // Not found. Warning
			error_info = format (L"File not found: %a") << nice_filename;
			break;
		case LOAD_FILE_PASSWORD: // Password incorrect.
			error_info = L"Incorrect password.";
			sleep (1);
			break;
		case LOAD_FILE_READ_ERROR:
			error_info = format (L"Cannot read file: %a") << nice_filename;
			break;
		case LOAD_FILE_BUNZIP2:
		case LOAD_FILE_XML:
			error_info = format (L"File format error: %a") << nice_filename;
			break;
		case LOAD_FILE_CONTENT:
			error_info = format (L"File content error: %a\n"
					L"This may be due to a bug. If possible, please send a copy of this file"
					L" to chys <admin@chys.info> so that we can fix the problem and help "
					L"recover the contents.") << nice_filename;
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

void MainWin::save (const std::wstring &filename)
{
	const wchar_t *fmt;
	if (save_file (wstring_to_mbs (filename).c_str (), entries, per_file_options, password)) {
		current_filename = filename;
		saved = true;
		fmt = L"Successfully saved \"%a\".";
	}
	else {
		fmt = L"Cannot save file \"%a\".";
	}
	ui::dialog_message (format (fmt) << filename);
}

void MainWin::default_save ()
{
	if (current_filename.empty ()) {
		save_as ();
	}
	else {
		save (current_filename);
	}
}

void MainWin::save_as ()
{
	std::wstring filename = ui::dialog_select_file (
			L"Save as",
			current_filename,
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
	DiaryEntry *ent = new DiaryEntry;
	time_t cur_time = time (0);
//	ent->utc_time.assign_utc (cur_time);
	ent->local_time = DateTime (DateTime::LOCAL, cur_time);
	ent->title = L"Your title goes here.";
	ent->text = L"Your contents go here.";
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
	if (filtered_entries.get ()) {
		return *filtered_entries;
	}
	else {
		return entries;
	}
}

const DiaryEntryList &MainWin::get_current_list () const
{
	if (filtered_entries.get ()) {
		return *filtered_entries;
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
				L"Are you sure to remove the currently selected entry?",
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
	if (ui::dialog_message (L"Are you sure you want to sort all entries by time? This operation cannot be undone.",
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
	switch (ui::dialog_message (
				format (L"Save changes to \"%a\"?") << (
					current_filename.empty () ? L"<Untitled>" : current_filename.c_str ()
				),
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
	if (current_filename.empty ())
		return;
	bool changed = true; // Whether any change was made to recent_files
	RecentFileList::iterator it = std::find (recent_files.begin (), recent_files.end (),
			current_filename);
	if (it == recent_files.end ()) {
		recent_files.push_front (RecentFile ());
		it = recent_files.begin ();
		it->filename = current_filename;
		it->focus_entry = main_ctrl.get_current_focus ();
		changed = true;
	}
	else {
		if (it != recent_files.begin ()) {
			// C++ standard explicitly allows splicing from the list itself
			recent_files.splice (recent_files.begin (), recent_files, it);
			it = recent_files.begin ();
			changed = true;
		}
		unsigned current_focus = main_ctrl.get_current_focus ();
		if (current_focus != it->focus_entry) {
			it->focus_entry = current_focus;
			changed = true;
		}
	}

	unsigned n_files = global_options.get_num (GLOBAL_OPTION_RECENT_FILES);
	unsigned extra = recent_files.size () - n_files;
	if (int (extra) > 0) {
		for (; extra; --extra) {
			recent_files.pop_back ();
		}
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
				L"Open",
				std::wstring (),
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
			ui::dialog_message (L"No recent file");
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
	if (!filter.get ()) {
		filter.reset (new FilterGroup);
	}
	dialog_filter (entries, *filter);
	if (filter->empty ()) {
		filter.reset ();
	}
	updated_filter ();
}

void MainWin::clear_filter ()
{
	if (filter.get ()) {
		filter.reset ();
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
	ui::dialog_message (L"Not found.");
}

void MainWin::reset_file ()
{
	per_file_options.reset ();
	current_filename.clear ();
	password.clear ();
	std::for_each (entries.begin (), entries.end (), delete_fun <DiaryEntry> ());
	entries.clear ();
}

void MainWin::edit_password ()
{
	if (!password.empty ()) {
		std::wstring old_password = ui::dialog_input (L"Please enter your old password:",
				std::wstring (), 35, ui::INPUT_PASSWORD, std::wstring ());
		if (old_password.empty ()) {
			return;
		}
		if (old_password != password) {
			ui::dialog_message (L"Incorrect password.");
			return;
		}
	}

	std::wstring new_password1 = ui::dialog_input (L"Please enter your new password:",
			std::wstring (), 35, ui::INPUT_PASSWORD, L"\n\r");
	if (new_password1 == L"\n\r") { // Canceled
		return;
	}

	const wchar_t *info = 0;
	if (new_password1.empty ()) {
		if (ui::dialog_message (L"Are you sure you want to remove the password?",
					ui::MESSAGE_YES|ui::MESSAGE_NO) == ui::MESSAGE_YES) {
			password.clear ();
			main_ctrl.touch ();
			info = L"Password removed.";
		}
	}
	else {
		std::wstring new_password2 = ui::dialog_input (L"Please enter again:",
				std::wstring (), 35, ui::INPUT_PASSWORD, std::wstring ());
		if (new_password1 == new_password2) {
			password.swap (new_password1);
			main_ctrl.touch ();
			info = L"Password changed.";
		}
		else {
			info = L"Password NOT changed.";
		}
	}
	if (info) {
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
	tiary::edit_global_options (global_options, current_filename);
	save_global_options (global_options, recent_files);
	main_ctrl.MainCtrl::redraw ();
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
		tiary::display_statistics (entries, filtered_entries.get (), get_current ());
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
	return !filter.get ();
}

bool MainWin::query_filter_mode () const
{
	return filter.get ();
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
	return (query_nonempty_filtered () && !last_search.get_pattern ().empty ());
}

} // namespace tiary
