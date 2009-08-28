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


#include "main/mainwin.h"
#include "common/string.h"
#include "ui/dialog_message.h"
#include "ui/dialog_select_file.h"
#include "ui/dialog_input.h"
#include "common/unicode.h"
#include "common/format.h"
#include "diary/file.h"
#include "diary/diary.h"
#include "ui/paletteid.h"
#include "diary/config.h"
#include "common/datetime.h"
#include "common/algorithm.h"
#include "main/doc.h"
#include "main/dialog_search.h"
#include "common/pcre.h"
#include "main/pref_edit.h"
#include "main/dialog_tags.h"
#include <limits>

namespace tiary {

MainCtrl::MainCtrl (MainWin &dlg)
	: ui::Control (dlg)
	, ui::Scroll (1 /* To be set later */, false)
{
	set_cursor_visibility (false);
}

MainCtrl::~MainCtrl ()
{
}

bool MainCtrl::on_key (wchar_t key)
{
	switch (key) {
		case L'a':
		case L'A':
		case ui::INSERT:
			w().append ();
			return true;

		case L'e':
		case L'E':
			w().edit_current ();
			return true;

		case L'i':
		case L'I':
			w().edit_time_current ();
			return true;

		case L't':
			w().edit_tags_current ();
			return true;

		case L'T':
			w().edit_tags_current_expert ();
			return true;

		case L'd':
		case L'D':
		case ui::DELETE:
			w().remove_current ();
			return true;

		case L'm':
			w().move_up_current ();
			return true;

		case L'M':
			w().move_down_current ();
			return true;

		case L'S':
			w().sort_all ();
			return true;

		case L'p':
		case L'P':
			w().edit_password ();
			return true;

		case L'r':
			w().edit_perfile_options ();
			return true;

		case L'R':
			w().edit_global_options ();
			return true;

		case L'q':
		case L'Q':
		case ui::CTRL_Q:
			w().quit ();
			return true;

		case ui::ESCAPE:
			w().menu_bar.slot_clicked (0);
			return true;

		case L'k':
		case ui::UP:
			set_focus (current_focus () - 1);
			return true;

		case L'j':
		case ui::DOWN:
			set_focus (current_focus () + 1);
			return true;

		case L'b':
		case ui::PAGEUP:
			set_focus (current_focus () - ui::Scroll::get_height () + 1);
			return true;

		case L'f':
		case ui::PAGEDOWN:
			set_focus (current_focus () + ui::Scroll::get_height () - 1);
			return true;

		case L'^':
		case L'g':
		case L'<':
		case ui::HOME:
			set_focus (0);
			return true;

		case L'$':
		case L'G':
		case L'>':
		case ui::END:
			set_focus (std::numeric_limits<int>::max ());
			return true;

		case L'/':
		case ui::CTRL_F:
			w().search (false);
			return true;

		case L'?':
			w().search (true);
			return true;

		case L'n':
		case ui::F3:
			w().search_continue (false);
			return true;

		case L'N':
			w().search_continue (true);
			return true;

		case L'v':
		case ui::NEWLINE:
		case ui::RETURN:
		case ui::RIGHT:
			w().view_current ();
			return true;

		case L'V':
			w().view_all ();
			return true;

		case ui::CTRL_N:
			w().new_file ();
			return true;

		case ui::CTRL_O:
			w().open_file ();
			return true;

		case L'w':
		case ui::CTRL_S:
			w().default_save ();
			return true;

		case L'W':
			w().save_as ();
			return true;

		case L'h':
		case L'H':
		case ui::F1:
			show_doc ();
			return true;

		default:
			return false;
	}
	return false;
}

bool MainCtrl::on_mouse (ui::MouseEvent mouse_event)
{
	if (!(mouse_event.m & (ui::LEFT_CLICK | ui::RIGHT_CLICK)))
		return false;
	ui::Scroll::Info info = ui::Scroll::get_info ();
	unsigned k = mouse_event.p.y + info.first;
	if (k >= info.focus) {
		unsigned expand_lines = w().global_options.get_num (GLOBAL_OPTION_EXPAND_LINES);
		if (k < info.focus + expand_lines)
			k = info.focus;
		else
			k -= expand_lines - 1;
	}
	if (k >= w().entries.size ())
		return false;
	set_focus (k);
	if (mouse_event.m & ui::RIGHT_CLICK)
		w().context_menu->show (mouse_event.p + get_pos (), true);
	return true;
}


void MainCtrl::redraw ()
{
	choose_palette (ui::PALETTE_ID_ENTRY);
	clear ();

	if (w().entries.empty ()) {
		put (ui::make_size (), L"No entry yet.");
		put (ui::make_size (0, 1), L"Press \"a\" to create one; press Esc for the menu.");
#ifdef TIARY_USE_MOUSE
		put (ui::make_size (0, 2), L"You can also use your mouse.");
#endif
	}

	unsigned expand_lines = w().global_options.get_num (GLOBAL_OPTION_EXPAND_LINES);
	ui::Scroll::modify_height (get_size ().y - expand_lines + 1);

	Scroll::Info info = Scroll::get_info ();

	ui::Size pos = ui::make_size ();

	std::wstring date_format = w().global_options.get_wstring (GLOBAL_OPTION_DATETIME_FORMAT);

	wchar_t *disp_buffer = new wchar_t [get_size ().x];

	for (unsigned i=0; i<info.len; ++i) {

		choose_palette (i == info.focus_pos ? ui::PALETTE_ID_ENTRY_SELECT : ui::PALETTE_ID_ENTRY);

		if (i == info.focus_pos) {
			move_cursor (pos);
			clear (pos, ui::make_size (get_size().x, expand_lines));
		}
		const DiaryEntry &entry = *w().entries[i+info.first];

		// Entry ID
		pos = put (pos, format (L"%04a  ") << (info.first + i + 1));

		// Date
		choose_palette (i == info.focus_pos ? ui::PALETTE_ID_ENTRY_DATE_SELECT : ui::PALETTE_ID_ENTRY_DATE);
		pos = put (pos, entry.local_time.format (date_format.c_str ()));
		pos.x++;

		// Title
		SplitStringLine split_info;
		const std::wstring &title = entry.title;
		choose_palette (i == info.focus_pos ? ui::PALETTE_ID_ENTRY_TITLE_SELECT : ui::PALETTE_ID_ENTRY_TITLE);
		split_line (split_info, maxS (0, get_size().x-pos.x), title, 0, SPLIT_NEWLINE_AS_SPACE|SPLIT_CUT_WORD);
		pos = put (pos, disp_buffer,
				std::replace_copy_if (
						&title[split_info.begin], &title[split_info.begin+split_info.len],
						disp_buffer, std::not1 (std::ptr_fun (iswprint)), L' ') - disp_buffer);
		pos.x++;

		// Tags
		const DiaryEntry::TagList &tags = entry.tags;
		choose_palette (i == info.focus_pos ? ui::PALETTE_ID_ENTRY_TAGS_SELECT : ui::PALETTE_ID_ENTRY_TAGS);
		int left_wid = get_size().x - pos.x;
		for (DiaryEntry::TagList::const_iterator it=tags.begin(); it!=tags.end(); ) {
			if (left_wid < 3)
				break;
			unsigned tagwid = ucs_width (*it);
			if (tagwid + 2 > unsigned (left_wid)) {
				pos = put (pos, L"...", 3);
				break;
			}
			pos = put (pos, *it);
			if (++it != tags.end ())
				pos = put (pos, L',');
		}
		pos.x++;

		choose_palette (i == info.focus_pos ? ui::PALETTE_ID_ENTRY_TEXT_SELECT : ui::PALETTE_ID_ENTRY_TEXT);
		const std::wstring &text = entry.text;
		size_t offset = 0;
		if (i == info.focus_pos && expand_lines >= 2) {
			// Current entry
			// [Date] [Title] [Tags]
			// [...]
			for (unsigned j=1; j<expand_lines; ++j) {
				pos = ui::make_size (0, pos.y+1);
				offset = split_line (split_info, get_size().x, text, offset,
						SPLIT_NEWLINE_AS_SPACE);
				wchar_t *bufend = std::replace_copy_if (
						&text[split_info.begin], &text[split_info.begin+split_info.len],
						disp_buffer, std::not1 (std::ptr_fun (iswprint)), L' ');
				pos = put (pos, disp_buffer, bufend-disp_buffer);
			}
		} else {
			// Other entry
			// [Date] [Title] [Tags] [...]
			offset = split_line (split_info, maxS (0, get_size().x - pos.x), text, offset,
					SPLIT_NEWLINE_AS_SPACE|SPLIT_CUT_WORD);
			wchar_t *bufend = std::replace_copy_if (
					&text[split_info.begin], &text[split_info.begin+split_info.len],
					disp_buffer, std::not1 (std::ptr_fun (iswprint)), L' ');
			pos = put (pos, disp_buffer, bufend-disp_buffer);
		}
		pos = ui::make_size (0, pos.y+1);
	}
	delete [] disp_buffer;
}

void MainCtrl::set_focus (unsigned k)
{
	unsigned num_ent = w().entries.size ();
	if (num_ent == 0)
		return;
	if (int (k) < 0)
		k = 0;
	if (k >= num_ent)
		k = num_ent - 1;
	ui::Scroll::modify_focus (k);
	MainCtrl::redraw ();
}

void MainCtrl::touch ()
{
	ui::Scroll::modify_number (w ().entries.size ());
	w().saved = false;
	MainCtrl::redraw ();
}



MainWin::MainWin (const std::wstring &initial_filename)
	: ui::Dialog (ui::Dialog::DIALOG_NO_BORDER)
	, menu_bar (*this)
	, context_menu ()
	, saved (true)
	, main_ctrl (*this)
	, last_search_backward (false)
	, last_search_regex (false)
{
	menu_bar.add (L"&File")
		(L"&New            Ctrl+N", Signal (this, &MainWin::new_file))
		(L"&Open...        Ctrl+O", Signal (this, &MainWin::open_file))
		()
		(L"&Save           Ctrl+S", Signal (this, &MainWin::default_save))
		(L"Save &as...",            Signal (this, &MainWin::save_as))
		()
		(L"&Password...",           Signal (this, &MainWin::edit_password))
		()
		(L"&Quit           Ctrl+Q", Signal (this, &MainWin::quit))
		;
	context_menu = &menu_bar.add (L"&Entry");
	(*context_menu)
		(L"&New entry",             Signal (this, &MainWin::append))
		(L"&Delete",                Signal (this, &MainWin::remove_current))
		()
		(L"&Edit",                  Signal (this, &MainWin::edit_current))
		(L"Edit &tags...",          Signal (this, &MainWin::edit_tags_current))
		(L"Edit tags (&quick)...",  Signal (this, &MainWin::edit_tags_current_expert))
		(L"Edit t&ime...",          Signal (this, &MainWin::edit_time_current))
		()
		(L"&View",                  Signal (this, &MainWin::view_current))
		(L"View &all",              Signal (this, &MainWin::view_all))
		()
		(L"&Move up",               Signal (this, &MainWin::move_up_current))
		(L"Move dow&n",             Signal (this, &MainWin::move_down_current))
		(L"&Sort all",              Signal (this, &MainWin::sort_all))
		;
	menu_bar.add (L"&Search")
		(L"&Find...        Ctrl+F", Signal (this, &MainWin::search, false))
		(L"Find &next      F3",     Signal (this, &MainWin::search_continue, false))
		(L"Find &previous",         Signal (this, &MainWin::search_continue, true))
		;
	menu_bar.add (L"&Preferences")
		(L"&Global preferences...", Signal (this, &MainWin::edit_global_options))
		(L"&File perferences...",   Signal (this, &MainWin::edit_perfile_options))
		;
	menu_bar.add (L"&Help")
		(L"&Help",                  Signal (show_doc))
		(L"&About",                 Signal (show_about))
		;
	MainWin::redraw ();

	switch (load_global_options (global_options)) {
		case LOAD_FILE_SUCCESS:
			break;
		case LOAD_FILE_NOT_FOUND: // No file. Just use the defaults
			break;
		default: // Warning, and use the defaults
			ui::dialog_message (L"WARNING: Config file ~/" TIARY_WIDIFY(GLOBAL_OPTION_FILE) L" reading error. Using defaults.");
			break;
	}

	std::wstring filename = initial_filename;
	if (filename.empty ())
		filename = utf8_to_wstring (global_options.get (GLOBAL_OPTION_DEFAULT_FILE));
	if (!filename.empty ())
		load (filename);
}

MainWin::~MainWin ()
{
	std::for_each (entries.begin (), entries.end (), delete_fun <DiaryEntry> ());
}

void MainWin::on_winch ()
{
	MainWin::redraw ();
}

void MainWin::redraw ()
{
	ui::Size scrsize = ui::get_screen_size ();
	scrsize -= ui::make_size (1, 0);
	move_resize (ui::make_size (), scrsize);
	menu_bar.move_resize (ui::make_size (), ui::make_size (scrsize.x, 1));
	main_ctrl.move_resize (ui::make_size (0, 1), scrsize - ui::make_size (0, 1));
	Dialog::redraw ();
}

namespace {

struct EnterPassword : NoArgCallback<std::wstring>
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

	std::wstring error_info;
	switch (load_file (wstring_to_mbs (filename).c_str (),
				EnterPassword (filename),
				entries,
				per_file_options,
				password)) {
		case LOAD_FILE_SUCCESS:
			current_filename = filename;
			main_ctrl.touch ();
			saved = true;
			return;

		case LOAD_FILE_NOT_FOUND: // Not found. Warning
			error_info = format (L"File not found: %a") << filename;
			break;
		case LOAD_FILE_PASSWORD: // Password incorrect.
			error_info = L"Incorrect password.";
			break;
		case LOAD_FILE_READ_ERROR:
			error_info = format (L"Cannot read file: %a") << filename;
			break;
		case LOAD_FILE_BUNZIP2:
		case LOAD_FILE_XML:
			error_info = format (L"File format error: %a") << filename;
			break;
		case LOAD_FILE_CONTENT:
			error_info = format (L"File content error: %a\n"
					L"This may be due to a bug. If possible, please send a copy of this file"
					L" to chys <admin@chys.info> so that we can fix the problem and help "
					L"recover the contents.") << filename;
			break;
	}
	reset_file ();
	if (!error_info.empty ())
		ui::dialog_message (error_info);
}

void MainWin::save (const std::wstring &filename)
{
	const wchar_t *fmt;
	if (save_file (wstring_to_mbs (filename).c_str (), entries, per_file_options, password)) {
		current_filename = filename;
		saved = true;
		fmt = L"Successfully saved \"%a\".";
	} else
		fmt = L"Cannot save file \"%a\".";
	ui::dialog_message (format (fmt) << filename);
}

void MainWin::default_save ()
{
	if (current_filename.empty ())
		save_as ();
	else
		save (current_filename);
}

void MainWin::save_as ()
{
	std::wstring filename = ui::dialog_select_file (
			L"Save as",
			current_filename,
			ui::SELECT_FILE_WRITE | ui::SELECT_FILE_WARN_OVERWRITE);
	if (!filename.empty ())
		save (filename);
}

void MainWin::append ()
{
	DiaryEntry *ent = new DiaryEntry;
	time_t cur_time = time (0);
//	ent->utc_time.assign_utc (cur_time);
	ent->local_time = DateTime (DateTime::LOCAL, cur_time);
	ent->title = L"Your title goes here.";
	ent->text = L"Your contents go here.";
	if (ent->edit (global_options.get (GLOBAL_OPTION_EDITOR).c_str()) && !ent->text.empty ()) {
		entries.push_back (ent);
		main_ctrl.touch ();
		main_ctrl.set_focus (std::numeric_limits<int>::max ());
	} else
		delete ent;
}

DiaryEntry *MainWin::get_current ()
{
	if (entries.empty ())
		return 0;
	else
		return entries[main_ctrl.current_focus ()];
}

void MainWin::edit_current ()
{
	if (DiaryEntry *ent = get_current ()) {
		if (ent->edit (global_options.get (GLOBAL_OPTION_EDITOR).c_str()))
			main_ctrl.touch ();
	}
}

void MainWin::edit_tags_current ()
{
	if (DiaryEntry *ent = get_current ()) {
		if (edit_tags (ent->tags, entries))
			main_ctrl.touch ();
	}
}

void MainWin::edit_tags_current_expert ()
{
	if (DiaryEntry *ent = get_current ()) {
		if (ent->edit_tags ())
			main_ctrl.touch ();
	}
}

void MainWin::edit_time_current ()
{
	if (DiaryEntry *ent = get_current ()) {
		if (ent->edit_time ())
			main_ctrl.touch ();
	}
}


void MainWin::remove_current ()
{
	if (entries.empty ())
		return;
	size_t k = main_ctrl.current_focus ();
	if (ui::dialog_message (
				L"Are you sure to remove the currently selected entry?",
				ui::MESSAGE_YES|ui::MESSAGE_NO) == ui::MESSAGE_YES) {
		delete entries[k];
		entries.erase (entries.begin () + k);
		main_ctrl.touch ();
	}
}

void MainWin::move_up_current ()
{
	if (entries.size () < 2)
		return;
	size_t k = main_ctrl.current_focus ();
	if (k == 0)
		return;
	std::swap (entries[k-1], entries[k]);
	main_ctrl.touch ();
	main_ctrl.set_focus (k-1);
}

void MainWin::move_down_current ()
{
	if (entries.size () < 2)
		return;
	size_t k = main_ctrl.current_focus ();
	if (k+1 >= entries.size ())
		return;
	std::swap (entries[k+1], entries[k]);
	main_ctrl.touch ();
	main_ctrl.set_focus (k+1);
}

namespace {

bool compare_entry (const DiaryEntry *a, const DiaryEntry *b)
{
	if (a->local_time < b->local_time)
		return true;
	else
		return false;
}

} // anonymous namespace

void MainWin::sort_all ()
{
	if (ui::dialog_message (L"Are you sure you want to sort all entries by time? This operation cannot be undone.",
				ui::MESSAGE_YES|ui::MESSAGE_NO) == ui::MESSAGE_YES) {
		std::stable_sort (entries.begin (), entries.end (), compare_entry);
		main_ctrl.touch ();
	}
}

void MainWin::view_current ()
{
	if (!entries.empty ()) {
		DiaryEntry *ent = entries[main_ctrl.current_focus ()];
		ent->view ();
	}
}

void MainWin::view_all ()
{
	DiaryEntry::view_all (entries);
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
	if (saved)
		return true;
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
		if (!new_filename.empty ())
			load (new_filename);
	}
}

void MainWin::search (bool bkwd)
{
	if (entries.empty ())
		return;
	std::wstring search_text;
	bool search_backward;
	bool search_regex;
	dialog_search (search_text, search_backward, search_regex,
			last_search_text, bkwd, last_search_regex);
	if (!search_text.empty ()) {
		last_search_text.swap (search_text);
		last_search_backward = search_backward;
		last_search_regex = search_regex;
		do_search (search_backward, true);
	}
}

void MainWin::search_continue (bool bkwd)
{
	do_search (bkwd, false);
}

void MainWin::do_search (bool bkwd, bool include_current_entry)
{
	unsigned num_ents = entries.size ();
	if (num_ents == 0)
		return;
	if (last_search_text.empty ())
		return;
	unsigned k = main_ctrl.current_focus ();
	int inc = (!bkwd == !last_search_backward) ? 1 : -1;
	if (!include_current_entry)
		k += inc;
	if (k < num_ents) {
#ifdef TIARY_USE_PCRE
		if (last_search_regex) {
			PcRe regex_obj (last_search_text);
			if (!regex_obj) {
				ui::dialog_message (L"Invalid regular expression");
				return;
			}
			for (; k < num_ents; k += inc) {
				if (regex_obj.match (entries[k]->title) || regex_obj.match (entries[k]->text)) {
					main_ctrl.set_focus (k);
					return;
				}
			}
		} else
#endif
		{
			for (; k < num_ents; k += inc) {
				if (find_caseless (entries[k]->title, last_search_text) != std::wstring::npos ||
						find_caseless (entries[k]->text, last_search_text) != std::wstring::npos) {
					main_ctrl.set_focus (k);
					return;
				}
			}
		}
	}
	// Not found.
	ui::dialog_message (L"Not found.", L"Error");
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
		if (old_password.empty ())
			return;
		if (old_password != password) {
			ui::dialog_message (L"Incorrect password.");
			return;
		}
	}

	std::wstring new_password1 = ui::dialog_input (L"Please enter your new password:",
			password, 35, ui::INPUT_PASSWORD, password);
	if (new_password1 == password) // Not modified
		return;

	const wchar_t *info = 0;
	if (new_password1.empty ()) {
		if (ui::dialog_message (L"Are you sure you want to remove the password?",
					ui::MESSAGE_YES|ui::MESSAGE_NO) == ui::MESSAGE_YES) {
			password.clear ();
			main_ctrl.touch ();
			info = L"Password removed.";
		}
	} else {
		std::wstring new_password2 = ui::dialog_input (L"Please enter again:",
				std::wstring (), 35, ui::INPUT_PASSWORD, std::wstring ());
		if (new_password1 == new_password2) {
			password.swap (new_password1);
			main_ctrl.touch ();
			info = L"Password changed.";
		} else {
			info = L"Password _not_ changed.";
		}
	}
	if (info)
		ui::dialog_message (info);
}

void MainWin::edit_global_options ()
{
	edit_options (global_options);
	save_global_options (global_options);
	main_ctrl.MainCtrl::redraw ();
}

void MainWin::edit_perfile_options ()
{
	edit_options (per_file_options);
	// TODO: If modified, mark saved=false (or call main_ctrl.touch ())
}

void MainWin::quit ()
{
	if (check_save ())
		request_close ();
}

} // namespace tiary
