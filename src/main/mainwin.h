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


#ifndef TIARY_MAIN_MAINWIN_H
#define TIARY_MAIN_MAINWIN_H

#include "ui/dialog.h"
#include "ui/control.h"
#include "ui/scroll.h"
#include "ui/menubar.h"
#include "diary/config.h"
#include <vector>
#include <string>

namespace tiary {

// Defined in diary/diary.h
struct DiaryEntry;

class MainCtrl;
class MainWin;

class MainCtrl : public ui::Control, private ui::Scroll
{
public:
	// Constructor & destructor
	explicit MainCtrl (MainWin &);
	~MainCtrl ();

	// Overload virtual functions
	bool on_key (wchar_t);
	bool on_mouse (ui::MouseEvent);
	void redraw ();

private:

	MainWin &w ();
	const MainWin &w () const;

	friend class MainWin;

	inline unsigned current_focus () const { return ui::Scroll::get_focus (); }

	void set_focus (unsigned); ///< Change focus
	void touch (); ///< Call this when any entry is modified
};

class MainWin : public ui::Dialog
{
public:
	// Constructor & destructor
	explicit MainWin (const std::wstring &initial_filename);
	~MainWin ();

	// Overload virtual functions
	void on_winch ();

	void redraw ();

	friend class MainCtrl;

private:
	ui::MenuBar menu_bar;

	ui::Menu *context_menu;

	GlobalOptionGroup global_options; ///< Global options

	PerFileOptionGroup per_file_options; ///< Per-file options

	std::wstring current_filename; ///< Currently working filename. Empty = none
	std::wstring password; ///< Password. Empty = none
	std::vector<DiaryEntry *> entries; ///< Diary entries
	bool saved; ///< Whether all modifications have been saved

	// main_ctrl must be after other members, which they use in its constructor
	MainCtrl main_ctrl;

	std::wstring last_search_text; ///< The text of last search
	bool last_search_backward; ///< The direction of last search. false = backward
	bool last_search_regex; ///< Whether regex was used in the last search

	// If successful, set current_filename
	// If failed, reset everything
	void load (const std::wstring &filename);
	// If successful, set current_filename
	void save (const std::wstring &filename);
	void default_save ();
	void save_as ();

	void new_file ();
	void open_file ();
	void edit_password ();
	void edit_global_options ();
	void edit_perfile_options ();
	void quit ();

	void append ();
	DiaryEntry *get_current ();
	void edit_current ();
	void edit_labels_current ();
	void edit_labels_current_expert ();
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
};


inline MainWin &MainCtrl::w () { return static_cast<MainWin &>(dlg); }
inline const MainWin &MainCtrl::w () const { return static_cast<const MainWin &>(dlg); }


} // namespace tiary

#endif // include guard
