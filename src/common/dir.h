// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the so-called 3-clause BSD license.
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
template <typename ChT> std::basic_string<ChT> get_home_dir (const ChT *);
template <typename ChT> std::basic_string<ChT> get_home_dir (const std::basic_string <ChT> &);

/**
 * @brief	Concatenates the home dir with a name under the home
 *
 * E.g. make_home_dirname((char*)0) = "/home/xxx";
 *
 * make_home_dirname("abc") = "/home/xxx/abc";
 */
template <typename ChT> std::basic_string<ChT> make_home_dirname (const ChT *);

/**
 * @brief	Return the absolute path of current directory
 */
template <typename ChT> std::basic_string<ChT> get_current_dir ();

/**
 * @brief	Returns the full path
 */
template <typename ChT> std::basic_string<ChT> get_full_pathname (const ChT *);
template <typename ChT> std::basic_string<ChT> get_full_pathname (const std::basic_string <ChT> &);

/**
 * @brief	Replaces home dir name with "~"
 */
template <typename ChT> std::basic_string<ChT> home_fold_pathname (const std::basic_string <ChT> &);
#ifdef TIARY_HAVE_RVALUE_REFERENCES
template <typename ChT> std::basic_string<ChT> home_fold_pathname (std::basic_string <ChT> &&);
#endif

/**
 * @brief	Expands "~" and "~user" with actual dir names
 */
template <typename ChT> std::basic_string<ChT> home_expand_pathname (const std::basic_string <ChT> &);
#ifdef TIARY_HAVE_RVALUE_REFERENCES
template <typename ChT> std::basic_string<ChT> home_expand_pathname (std::basic_string <ChT> &&);
#endif

/**
 * @brief	Return the "nice" name of the argument
 *
 * It may be absolute or starting with "~" or relative,
 * whichever is "nicer" (shorter and/or easier to read)
 */
template <typename ChT> std::basic_string<ChT> get_nice_pathname (const ChT *);
template <typename ChT> std::basic_string<ChT> get_nice_pathname (const std::basic_string <ChT> &);

const unsigned FILE_ATTR_DIRECTORY = 1;
const unsigned FILE_ATTR_NONEXIST  = 0x8000u;

template <typename ChT> unsigned get_file_attr (const ChT *);
template <typename ChT> inline unsigned get_file_attr (const std::basic_string<ChT> &s)
{
	return get_file_attr (s.c_str ());
}

/**
 * @brief	Split a full pathname to directory name and basename
 */
template <typename ChT>
std::pair<std::basic_string<ChT>,std::basic_string<ChT> > split_pathname (const std::basic_string<ChT> &, bool canonicalize = false);

/**
 * @brief	Combine a directory name and basename to a full name
 */
template <typename ChT>
std::basic_string<ChT> combine_pathname (const std::basic_string <ChT> &, const std::basic_string <ChT> &);

template <typename ChT>
struct DirEnt
{
	std::basic_string<ChT> name; ///< "Bare" filename (without path)
	unsigned attr; ///< Attribute. Currently only supports FIILE_ATTR_DIRECTORY

	class DefaultComparator : public BinaryCallback<const DirEnt<ChT> &, const DirEnt<ChT> &, bool>
	{
		std::locale loc;
	public:
		bool operator () (const DirEnt<ChT> &, const DirEnt<ChT> &) const;
	};
};

/**
 * @brief	List all files/directories in a directory
 *
 * By default, all items (include . and ..) are included. Strings are compared
 * using std::locale (equivalently, strcoll)
 */
template <typename ChT> std::list<DirEnt<ChT> > list_dir (const ChT *dir, ///< Dir name
		const UnaryCallback <const DirEnt<ChT> &, bool> &
			= ConstantUnaryCallback<const DirEnt<ChT> &, bool, false> (), ///< A callback function to filter out unwanted items
		const BinaryCallback <const DirEnt<ChT> &, const DirEnt<ChT> &, bool> &
			= typename DirEnt<ChT>::DefaultComparator () ///< A callback function to compare two items (less_than semantics)
		);

template <typename ChT> std::list<DirEnt<ChT> > list_dir (const std::basic_string<ChT> &dir, ///< Dir name
		const UnaryCallback <const DirEnt<ChT> &, bool> &filter
			= ConstantUnaryCallback<const DirEnt<ChT> &, bool, false> (), ///< A callback function to filter out unwanted items
		const BinaryCallback <const DirEnt<ChT> &, const DirEnt<ChT> &, bool> &comp
			= typename DirEnt<ChT>::DefaultComparator () ///< A callback function to compare two items (less_than semantics)
		)
{
	return list_dir (dir.c_str (), filter, comp);
}



/**
 * @brief	Find the specified executable in $PATH
 * @result	Full path of the executable or empty string if failed
 */
std::string find_executable (const std::string &);


} // namespace tiary

#endif // Include guard
