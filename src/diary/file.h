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


#ifndef TIARY_DIARY_FILE_H
#define TIARY_DIARY_FILE_H

#include "diary/config.h"
#include <functional>
#include <string>
#include <vector>
#include <list>

namespace tiary {

struct DiaryEntry; // Defined in diary.h
struct RecentFile; // Defined in diary.h

enum LoadFileRet {
	// We put the commas in the beginning of a line to make diff happy.
	LOAD_FILE_SUCCESS
	, LOAD_FILE_NOT_FOUND  // File not found
	, LOAD_FILE_READ_ERROR // Reading error
	, LOAD_FILE_PASSWORD   // Password error
	, LOAD_FILE_DECRYPTION // Decryption error
	, LOAD_FILE_BUNZIP2    // Decompression error
	, LOAD_FILE_XML        // XML parsing error
	, LOAD_FILE_CONTENT    // XML content error
};


// Read global options from ~/.tiary
LoadFileRet load_global_options(GlobalOptionGroup &, std::vector<RecentFile> &);

/**
 * @brief	Load tiary file
 *
 * Attempting loading file to the following variables:
 *  - per-file options
 *  - diary entries
 *  - password (Will call foo to ask user)
 *
 * If successful, set g_current_filename and returns true;
 * Otherwise, clear g_current_filename and returns false
 *
 */
LoadFileRet load_file (
		const char *filename, ///< Filename
		const std::function<std::wstring()> &foo, ///< A callback function that asks the user for password
		std::vector <DiaryEntry *> &entries,
		PerFileOptionGroup &,
		std::wstring &password ///< Empty = no password
		);



bool save_global_options(const GlobalOptionGroup &, const std::vector<RecentFile> &);

bool save_file (const char *filename, const std::vector<DiaryEntry *> &entries, const PerFileOptionGroup &, const std::wstring &password);


} // namespace tiary

#endif // Include guard
