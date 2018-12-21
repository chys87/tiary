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
 * @file	main/mainwin.h
 * @author	chys <admin@chys.info>
 * @brief	Header for the "Main Window" class, i.e. the main user interface
 */


#ifndef TIARY_MAIN_MAINWIN_H
#define TIARY_MAIN_MAINWIN_H

#include "ui/window.h"
#include "ui/menubar.h"
#include "ui/hotkey_hint.h"
#include "ui/search_info.h"
#include "diary/config.h"
#include "diary/diary.h"
#include "main/mainctrl.h"
#include <memory>
#include <string>
#include <vector>

namespace tiary {

// Defined in main/filter.h
struct FilterGroup;

class MainWin final : public ui::Window {
public:
	// Constructor & destructor
	explicit MainWin (const std::wstring &initial_filename);
	~MainWin ();

	// Overload virtual functions
	void redraw ();
	void on_ready ();

	friend class MainCtrl;

private:
	ui::MenuBar menu_bar;

	ui::Menu *context_menu;

	GlobalOptionGroup global_options; ///< Global options

	PerFileOptionGroup per_file_options; ///< Per-file options

	std::wstring current_filename; ///< Currently working filename. Empty = none
	std::wstring password; ///< Password. Empty = none
	std::vector<DiaryEntry *> entries; ///< Diary entries
	RecentFileList recent_files; ///< Recent files
	bool saved; ///< Whether all modifications have been saved

	std::unique_ptr<FilterGroup> filter; ///< Current filter
	std::unique_ptr<std::vector<DiaryEntry *>> filtered_entries; ///< filter.filter(entries)
	void updated_filter (); ///< Must be called every time filter is modified
	bool unavailable_filtered (); ///< Display an error message "Unavaiable in filtering mode"

	// main_ctrl must be after other members, which they use in its constructor
	MainCtrl main_ctrl;

	ui::HotkeyHint hotkey_hint;

	ui::SearchInfo last_search; ///< Remember what we last searched for

	// If successful, set current_filename
	// If failed, reset everything
	void load (const std::wstring &filename);
	// If successful, set current_filename
	void save (const std::wstring &filename);
	void default_save ();
	void save_as ();

	void new_file ();
	void open_file ();
	void open_recent_file ();
	void edit_password ();
	void edit_all_labels ();
	void edit_global_options ();
	void edit_perfile_options ();
	void display_statistics ();
	void quit ();

	void edit_filter ();
	void clear_filter ();

	void append ();
	std::vector <DiaryEntry *> &get_current_list ();
	const std::vector <DiaryEntry *> &get_current_list () const;
	DiaryEntry *get_current ();
	void edit_current ();
	void edit_labels_current ();
	void edit_time_current ();
	void view_current ();
	void view_all ();
	void remove_current ();
	void move_up_current ();
	void move_down_current ();
	void sort_all ();

	void search (bool /**< false = backward */);
	void search_continue (bool /**< false = previous */);
	void do_search (bool /**< false = previous */, bool include_current_entry);

	void reset_file ();

	/**
	 * @result	true: The caller should continue to the next operation;
	 * false: The caller should cancel
	 *
	 * If any change was not saved, ask the user.
	 * If the user instructs to save, do it.
	 */
	bool check_save ();

	void update_recent_files ();

	bool query_normal_mode () const;
	bool query_filter_mode () const;
	bool query_nonempty_filtered () const;
	bool query_nonempty_all () const;
	bool query_allow_up () const;
	bool query_allow_down () const;
	bool query_search_continuable () const;
};


inline MainWin &MainCtrl::w () { return static_cast<MainWin &>(window()); }
inline const MainWin &MainCtrl::w () const { return static_cast<const MainWin &>(window()); }


} // namespace tiary

#endif // include guard
