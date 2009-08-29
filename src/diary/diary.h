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


#ifndef TIARY_DIARY_DIARY_H
#define TIARY_DIARY_DIARY_H

#include "common/datetime.h"
#include "common/containers.h"
#include <string>
#include <vector>


namespace tiary {

struct DiaryEntry;
typedef std::vector<DiaryEntry *> DiaryEntryList;

struct DiaryEntry
{
	typedef WStringLocaleOrderedSet LabelList;

	DateTime local_time; // Local date and time
//	DateTime utc_time; // UTC date and time
	std::wstring title;
	std::wstring text;
	LabelList labels;


	/**
	 * @brief	Edit the entry in an external editor
	 * @param	editor	External editor used to edit the text.
	 * "$param" is supported (no braces)
	 *
	 * "$EDITOR|rvim|vim|emacs -nw"
	 * @result	Whether the entry is modified
	 */
	bool edit (const char *editor);
	/**
	 * @brief	Edit the time for this entry
	 * @result	Whether there is any modification
	 */
	bool edit_time ();
	/**
	 * @brief	View the content in a dialog
	 */
	void view ();

	/**
	 * @brief	View many items at one time
	 */
	static void view_all (const DiaryEntryList &);
};





} // namespace tiary


#endif // Include guard
