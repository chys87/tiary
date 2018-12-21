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


#ifndef TIARY_DIARY_DIARY_H
#define TIARY_DIARY_DIARY_H

#include "common/datetime.h"
#include "common/containers.h"
#include <string>
#include <string_view>
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
};



struct RecentFile
{
	std::wstring filename;
	unsigned focus_entry;

	bool operator == (std::wstring_view name) const {
		return (filename == name);
	}
};

// New files are in the front
typedef std::vector<RecentFile> RecentFileList;


} // namespace tiary


#endif // Include guard
