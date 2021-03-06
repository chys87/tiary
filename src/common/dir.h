// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2018, 2019, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/

/**
 * @file	common/dir.h
 * @author	chys <admin@chys.info>
 * @brief	Declares some functions related to file and directory operations
 */

#ifndef TIARY_COMMON_DIR_H
#define TIARY_COMMON_DIR_H

#include <functional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace tiary {

/**
 * @brief	Returns the home directory of the current user
 */
template <typename ChT> const std::basic_string<ChT> &get_home_dir ();

/**
 * @brief	Returns the home directory of another user
 *
 * If the empty is NULL or empty, returns the home dir of the current user
 */
std::string get_home_dir (const char *user);
std::string get_home_dir (const std::string &);
std::wstring get_home_dir(std::wstring_view);

/**
 * @brief	Concatenates the home dir with a name under the home
 *
 * make_home_dirname("abc") = "/home/xxx/abc";
 */
std::string make_home_dirname(std::string_view);

/**
 * @brief	Return the absolute path of current directory
 */
template <typename ChT> std::basic_string<ChT> get_current_dir ();

/**
 * @brief	Returns the absolute path
 *
 * This function does not resolve symbolic links
 */
std::wstring get_full_pathname(std::wstring_view);

/**
 * @brief	Replaces home dir name with "~"
 */
std::string home_fold_pathname(std::string_view);
std::wstring home_fold_pathname(std::wstring_view);

/**
 * @brief	Expands "~" and "~user" with actual dir names
 */
std::string home_expand_pathname(std::string_view);
std::wstring home_expand_pathname(std::wstring_view);

/**
 * @brief	Return the "nice" name of the argument
 *
 * It may be absolute or starting with "~" or relative,
 * whichever is "nicer" (shorter and/or easier to read)
 */
std::wstring get_nice_pathname(std::wstring_view);




const unsigned FILE_ATTR_DIRECTORY  = 1;
// Remeber the meaning of X is different for directories and files
const unsigned FILE_ATTR_EXECUTABLE = 2;
const unsigned FILE_ATTR_NONEXIST   = 0x8000u;

/**
 * @brief	Get the attributes of a given file/directory
 */
unsigned get_file_attr (const char *);
unsigned get_file_attr (const wchar_t *);
inline unsigned get_file_attr (const std::string &s) { return get_file_attr (s.c_str ()); }
inline unsigned get_file_attr (const std::wstring &s) { return get_file_attr (s.c_str ()); }

/**
 * @brief	Split a full pathname to directory name and basename
 */
std::pair<std::wstring,std::wstring> split_pathname(std::wstring_view, bool canonicalize = false);

/**
 * @brief	Combine a directory name and basename to a full name
 */
std::wstring combine_pathname(std::wstring_view, std::wstring_view);

struct DirEnt
{
	std::wstring name; ///< "Bare" filename (without path)
	unsigned attr; ///< Attribute
};

typedef std::vector<DirEnt> DirEntList;

/**
 * @brief	List all files/directories in a directory
 */
DirEntList list_dir (const std::wstring &dir, ///< Dir name
		const std::function<bool(const DirEnt &)> &filter, ///< A callback function to filter out unwanted items
		const std::function<bool(const DirEnt &, const DirEnt &)> &comp ///< A callback function to compare two items (less_than semantics)
		);

// Default order. Strings are compared using std::locale.
// Directories have precedence over normal files
DirEntList list_dir(const std::wstring &dir, const std::function<bool(const DirEnt &)> &filter);



/**
 * @brief	Find the specified executable in $PATH
 * @result	Full path of the executable or empty string if failed
 */
std::string find_executable(std::string_view exe);

/**
 * @brief	Create and open a temporary file, similar with mkstemp
 * @param	name	[OUT] The name of the temporary file
 * @param	template	Template of the the filename, e.g.
 *					"/tmp/tiary.|.txt", where "|" will be replaced by some
 *					random characters
 * @result	The FD for the file, open in O_RDWR|O_CREAT|O_EXCL|O_CLOEXEC
 *
 * @c name is modified in an undefined way on failure.
 */
int my_mkstemp(std::string *name, std::string_view name_template);

} // namespace tiary

#endif // Include guard
