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


#ifndef TIARY_COMMON_DIR_H
#define TIARY_COMMON_DIR_H

#include "common/callback.h"
#include <list>
#include <string>
#include <locale>
#include <utility>

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
std::wstring get_home_dir (const wchar_t *user);
std::string get_home_dir (const std::string &);
std::wstring get_home_dir (const std::wstring &);

/**
 * @brief	Concatenates the home dir with a name under the home
 *
 * E.g. make_home_dirname((char*)0) = "/home/xxx";
 *
 * make_home_dirname("abc") = "/home/xxx/abc";
 */
std::string make_home_dirname (const char *);
std::wstring make_home_dirname (const wchar_t *);

/**
 * @brief	Return the absolute path of current directory
 */
template <typename ChT> std::basic_string<ChT> get_current_dir ();

/**
 * @brief	Returns the full path
 */
std::string get_full_pathname (const char *);
std::wstring get_full_pathname (const wchar_t *);
std::string get_full_pathname (const std::string &);
std::wstring get_full_pathname (const std::wstring &);

/**
 * @brief	Replaces home dir name with "~"
 */
std::string home_fold_pathname (const std::string &);
std::wstring home_fold_pathname (const std::wstring &);

/**
 * @brief	Expands "~" and "~user" with actual dir names
 */
std::string home_expand_pathname (const std::string &);
std::wstring home_expand_pathname (const std::wstring &);

/**
 * @brief	Return the "nice" name of the argument
 *
 * It may be absolute or starting with "~" or relative,
 * whichever is "nicer" (shorter and/or easier to read)
 */
std::string get_nice_pathname (const std::string &);
std::wstring get_nice_pathname (const std::wstring &);




const unsigned FILE_ATTR_DIRECTORY = 1;
const unsigned FILE_ATTR_NONEXIST  = 0x8000u;

/**
 * @brief	Get the attributes of a given file/directory
 *
 * Currently only supports ui::FILE_ATTR_DIRECTORY and ui::FILE_ATTR_NONEXIST
 */
unsigned get_file_attr (const char *);
unsigned get_file_attr (const wchar_t *);
inline unsigned get_file_attr (const std::string &s) { return get_file_attr (s.c_str ()); }
inline unsigned get_file_attr (const std::wstring &s) { return get_file_attr (s.c_str ()); }

/**
 * @brief	Split a full pathname to directory name and basename
 */
std::pair<std::string,std::string> split_pathname (const std::string &, bool canonicalize = false);
std::pair<std::wstring,std::wstring> split_pathname (const std::wstring &, bool canonicalize = false);

/**
 * @brief	Combine a directory name and basename to a full name
 */
std::string combine_pathname (const std::string &, const std::string &);
std::wstring combine_pathname (const std::wstring &, const std::wstring &);

struct DirEnt
{
	std::wstring name; ///< "Bare" filename (without path)
	unsigned attr; ///< Attribute. Currently only supports FIILE_ATTR_DIRECTORY

	class DefaultComparator : public BinaryCallback<const DirEnt &, const DirEnt &, bool>
	{
		std::locale loc;
	public:
		bool operator () (const DirEnt &, const DirEnt &) const;
	};
};

typedef std::list<DirEnt> DirEntList;

/**
 * @brief	List all files/directories in a directory
 *
 * By default, all items (include . and ..) are included. Strings are compared
 * using std::locale (equivalently, strcoll)
 */
DirEntList list_dir (const std::wstring &dir, ///< Dir name
		const UnaryCallback <const DirEnt &, bool> &filter
			= ConstantUnaryCallback<const DirEnt &, bool, false> (), ///< A callback function to filter out unwanted items
		const BinaryCallback <const DirEnt &, const DirEnt &, bool> &comp
			= DirEnt::DefaultComparator () ///< A callback function to compare two items (less_than semantics)
		);



/**
 * @brief	Find the specified executable in $PATH
 * @result	Full path of the executable or empty string if failed
 */
std::string find_executable (const std::string &);


} // namespace tiary

#endif // Include guard
