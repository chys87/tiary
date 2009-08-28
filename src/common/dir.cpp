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


#include "common/dir.h"
#include "common/unicode.h"
#include "common/string.h"
#include "common/algorithm.h"
#include <unistd.h>
#include <stdlib.h>
#include <limits.h> // PATH_MAX
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <pwd.h>
#include <locale>
#include <functional>
#include <algorithm>

#ifndef PATH_MAX
# define PATH_MAX 4096
#endif

namespace tiary {

namespace {

std::string get_home_dir_base ()
{
	const char *result = getenv ("HOME");
	if (result == 0) {
		if (struct passwd *data = getpwuid (getuid ()))
			result = data->pw_dir;
	}
	if (result==0 || *result=='\0')
		result = "/";
	return result;
}

} // anonymous namespace

template <> const std::basic_string<char> &get_home_dir <char> ()
{
	static std::string dir = get_home_dir_base ();
	return dir;
}

template <> const std::basic_string<wchar_t> &get_home_dir <wchar_t> ()
{
	static std::wstring dir = mbs_to_wstring (get_home_dir <char> ());
	return dir;
}

std::string get_home_dir (const char *user)
{
	const char *result;
	if (!user || !*user)
		result = get_home_dir <char> ().c_str ();
	else {
		if (struct passwd *data = getpwnam (user))
			result = data->pw_dir;
	}
	if (result==0 || *result=='\0')
		result = "/";
	return result;
}

std::wstring get_home_dir (const wchar_t *user)
{
	return mbs_to_wstring (get_home_dir (wstring_to_mbs (user).c_str()));
}

std::string get_home_dir (const std::string &user)
{
	return get_home_dir (user.c_str ());
}

std::wstring get_home_dir (const std::wstring &user)
{
	return mbs_to_wstring (get_home_dir (wstring_to_mbs (user).c_str()));
}

std::string make_home_dirname (const char *file)
{
	std::string ret = get_home_dir <char> ();
	if (file) {
		if (file[0] != '/')
			ret += '/';
		ret += file;
	}
	return ret;
}

std::wstring make_home_dirname (const wchar_t *file)
{
	std::wstring ret = get_home_dir <wchar_t> ();
	if (file) {
		if (file[0] != L'/')
			ret += L'/';
		ret += file;
	}
	return ret;
}


template <> std::basic_string<char> get_current_dir <char> ()
{
#if defined TIARY_HAVE_EXTENDED_GETCWD
	char *dir = getcwd (NULL, 0);
	std::string r = dir;
	free (dir);
	return r;
#elif defined TIARY_HAVE_EXTENDED_REALPATH
	char *dir = realpath (".", NULL);
	std::string r = dir;
	free (dir);
	return r;
#else
	char buffer[PATH_MAX];
	if (getcwd (buffer, sizeof buffer) == buffer)
		return buffer;
	else
		return std::string ();
#endif
}

template <> std::basic_string<wchar_t> get_current_dir <wchar_t> ()
{
	return mbs_to_wstring (get_current_dir<char>());
}

std::string get_full_pathname (const char *name)
{
	std::string ret;
#ifdef TIARY_HAVE_EXTENDED_REALPATH
	if (char *full = realpath (name, 0)) {
		ret = full;
		free (full);
	}
#else
	char buffer[PATH_MAX];
	if (realpath (name, buffer) == buffer)
		ret = buffer;
#endif
	if (*name && name[strlen(name)-1]=='/' && *c(ret).rbegin()!='/')
		ret += '/';
	return ret;
}

std::wstring get_full_pathname (const wchar_t *name)
{
	return mbs_to_wstring (get_full_pathname (wstring_to_mbs (name).c_str ()));
}

std::string get_full_pathname (const std::string &name)
{
	return get_full_pathname (name.c_str ());
}

std::wstring get_full_pathname (const std::wstring &name)
{
	return mbs_to_wstring (get_full_pathname (wstring_to_mbs (name).c_str ()));
}

template <typename ChT>
std::basic_string<ChT> home_fold_pathname (const std::basic_string<ChT> &name)
{
	std::basic_string <ChT> fullname = name;
	const std::basic_string <ChT> &homedir = get_home_dir <ChT> ();
	size_t homelen = homedir.length ();
	if (fullname.length () >= homelen) {
		if (fullname.length() == homelen || c(fullname)[homelen] == ChT('/')) {
			if (fullname.compare (0, homelen, homedir) == 0)
				fullname.replace (0, homelen, 1, ChT('~'));
		}
	}
	return fullname;
}

#ifdef TIARY_HAVE_RVALUE_REFERENCES
template <typename ChT>
std::basic_string<ChT> home_fold_pathname (std::basic_string<ChT> &&name)
{
	const std::basic_string <ChT> &homedir = get_home_dir <ChT> ();
	size_t homelen = homedir.length ();
	if (name.length () >= homelen) {
		if (name.length() == homelen || c(name)[homelen] == ChT('/')) {
			if (name.compare (0, homelen, homedir) == 0)
				name.replace (0, homelen, 1, ChT('~'));
		}
	}
	std::basic_string<ChT> ret;
	ret.swap (name);
	return ret;
}
#endif

template <typename ChT>
std::basic_string<ChT> home_expand_pathname (const std::basic_string<ChT> &name)
{
	if (name[0] == ChT('~')) {
		typename std::basic_string<ChT>::const_iterator it =
			std::find (name.begin () + 1, name.end (), ChT('/'));
		std::basic_string<ChT> homedir = get_home_dir (std::basic_string <ChT> (name.begin ()+1, it));
		if (!homedir.empty ()) {
			homedir.append (it, name.end ());
			return homedir;
		}
	}
	return name;
}

#ifdef TIARY_HAVE_RVALUE_REFERENCES
template <typename ChT>
std::basic_string<ChT> home_expand_pathname (std::basic_string<ChT> &&name)
{
	if (c(name)[0] == ChT('~')) {
		typename std::basic_string<ChT>::const_iterator it =
			std::find (c(name).begin () + 1, c(name).end (), ChT('/'));
		std::basic_string<ChT> homedir = get_home_dir (std::basic_string <ChT> (c(name).begin ()+1, it));
		if (!homedir.empty ())
			name.replace (0, it - c(name).begin (), homedir);
	}
	std::basic_string<ChT> ret;
	ret.swap (name);
	return ret;
}
#endif

template <typename ChT> std::basic_string<ChT> get_nice_pathname (const ChT *name)
{
	std::basic_string<ChT> fullname = get_full_pathname (name);
	std::basic_string<ChT> homefold = home_fold_pathname (fullname);
	std::basic_string<ChT> curdir = get_current_dir<ChT>();
	// Special case: curdir == '/'
	if (curdir.length () < 2) {
		fullname.swap (homefold);
		return fullname;
	}
	// Special case: curdir == filename
	if (curdir == fullname) {
		fullname = ChT('.');
		return fullname;
	}
	curdir += ChT('/');
	fullname += ChT('/'); // In case fullname is an ancestor dir of curdir
	/*
	 * curdir:     ............/b_1/.../b_y/
	 * fullname:   ............/c_1/.../c_z/
	 *             | <---  --->|
	 *               length ox
	 */
	unsigned y;
	size_t ox;
	typename std::basic_string<ChT>::const_iterator ox_it = std::mismatch (
			c(fullname).begin (), c(fullname).end (),
			curdir.c_str () // Not to be replaced by begin() (Not all implementations always maintain a null at the end!)
			).first;
	while (*(ox_it-1) != ChT('/'))
		--ox_it;
	ox = ox_it - c(fullname).begin ();

	y = std::count (c(curdir).begin () + ox, c(curdir).end (), ChT('/'));
	// Use either dot-dots or fullnames. These codes also apply to things under current directory
	unsigned dotdotlen = y*3;
	if ((y<3 && dotdotlen<ox) || (y==3 && dotdotlen*2<ox) || dotdotlen*4<ox) {
		// Use dot-dots iff 
		// (1) No more than two levels, and shorter; or
		// (2) More than two levels, and MUCH shorter
		fullname.replace (0, ox, dotdotlen, ChT('.'));
		// Fill y dot-dots
		typename std::basic_string<ChT>::iterator it = fullname.begin ();
		typename std::basic_string<ChT>::iterator end = it + dotdotlen;
		while (it < end) {
			it += 2;
			*it++ = ChT('/');
		}
	}
	fullname.resize (fullname.length () - 1);
	if (homefold.length () < fullname.length ())
		fullname.swap (homefold);
	return fullname;
}

template <typename ChT>
std::basic_string<ChT> get_nice_pathname (const std::basic_string<ChT> &name)
{
	return get_nice_pathname (name.c_str ());
}

unsigned get_file_attr (const char *name)
{
	struct stat st_buf;
	if (stat (name, &st_buf) == 0)
		return
#ifdef S_ISDIR
			S_ISDIR(st_buf.st_mode)
#elif defined S_IFDIR
			(st_buf.st_mode & S_IFDIR)
#else
# error "Neither S_ISDIR nor S_IFDIR is defined???"
#endif
			? FILE_ATTR_DIRECTORY : 0;
	else
		return FILE_ATTR_NONEXIST;
}

unsigned get_file_attr (const wchar_t *name)
{
	return get_file_attr (wstring_to_mbs (name).c_str ());
}

namespace {

template <typename ChT> inline
std::basic_string <ChT> optional_canonicalize (const std::basic_string<ChT> &name, size_t len, bool canonicalize)
{
	std::basic_string <ChT> tmp (name, 0, len);
	if (canonicalize)
		get_full_pathname (tmp).swap (tmp);
	return tmp;
}

} // anonymous namespace

template <typename ChT>
std::pair<std::basic_string<ChT>,std::basic_string<ChT> > split_pathname (const std::basic_string<ChT> &name, bool canonicalize)
{
	typename std::basic_string<ChT>::size_type last_split = name.rfind (ChT ('/'));
	return std::pair<std::basic_string<ChT>,std::basic_string<ChT> > (
			last_split == std::basic_string<ChT>::npos ?
				(canonicalize ? get_current_dir<ChT> () : std::basic_string<ChT>(1, ChT('.'))) :
				optional_canonicalize (name, last_split ? last_split : 1, canonicalize),
			std::basic_string<ChT> (name, last_split+1) // Standard guarantees npos == -1
		);
}

template <typename ChT>
std::basic_string<ChT> combine_pathname (const std::basic_string<ChT> &path, const std::basic_string<ChT> &basename)
{
	std::basic_string<ChT> ret;
	if (path.length() != 1 || path[0] != ChT('.')) {
		ret = path;
		if (ret.empty() || *c(ret).rbegin()!=ChT('/'))
			ret += ChT('/');
	}
	ret += basename;
	return ret;
}






template <typename ChT>
bool DirEnt<ChT>::DefaultComparator::operator () (const DirEnt<ChT> &a, const DirEnt<ChT> &b) const
{
	// Directory < Non-directory
	if ((a.attr ^ b.attr) & FILE_ATTR_DIRECTORY)
		return (a.attr & FILE_ATTR_DIRECTORY);
	// Otherwise, compare name
	return loc (a.name, b.name);
}



namespace {

template <typename T> inline std::basic_string <T> convert (const char *);
template <typename T> inline std::basic_string <T> convert (const wchar_t *);
template <> inline std::basic_string<char> convert<char> (const char *src)
{
	return src;
}
template <> inline std::basic_string<wchar_t> convert<wchar_t> (const wchar_t *src)
{
	return src;
}
template <> inline std::basic_string<char> convert<char> (const wchar_t *src)
{
	return wstring_to_mbs (src);
}
template <> inline std::basic_string<wchar_t> convert<wchar_t> (const char *src)
{
	return mbs_to_wstring (src);
}

inline DIR *wrap_opendir (const char *name)
{
	return ::opendir (name);
}
inline DIR *wrap_opendir (const wchar_t *name)
{
	return ::opendir (wstring_to_mbs (name).c_str());
}

} // anonymous namespace



template <typename ChT> std::list<DirEnt<ChT> > list_dir (
			const ChT *directory,
			const UnaryCallback <const DirEnt<ChT> &, bool> &filter,
			const BinaryCallback <const DirEnt<ChT> &, const DirEnt<ChT> &, bool> &comp
		)
{
	std::list<DirEnt<ChT> > filelist;
	if (DIR *dir = wrap_opendir (directory)) {
		DirEnt<ChT> tmp_ent;
		struct stat st_buf;
#ifdef TIARY_HAVE_AT_FILE
		int dirfd = ::dirfd (directory);
#else
		std::string dirname = convert<char>(directory);
		if (*c(dirname).rbegin() != '/')
			dirname += '/';
#endif
		while (struct dirent *ent = readdir (dir)) {
			convert<ChT> (ent->d_name).swap (tmp_ent.name);
			tmp_ent.attr = 0;
#ifdef TIARY_HAVE_AT_FILE
			if (fstatat (dirfd, ent->d_name, &st_buf, 0) == 0)
#else
			if (stat ((dirname + ent->d_name).c_str(), &st_buf) == 0)
#endif
				if (S_ISDIR (st_buf.st_mode))
					tmp_ent.attr = FILE_ATTR_DIRECTORY;
			if (!filter (tmp_ent))
				filelist.push_back (TIARY_STD_MOVE (tmp_ent));
		}
		closedir (dir);
		// Sort them
		filelist.sort (wrap (comp));
	}
	return filelist;
}

// Explicit instantiations (char)
template const std::basic_string<char> &get_home_dir<char> ();
template std::basic_string<char> get_current_dir<char> ();
template std::basic_string<char> home_fold_pathname (const std::basic_string <char> &);
template std::basic_string<char> home_expand_pathname (const std::basic_string <char> &);
#ifdef TIARY_HAVE_RVALUE_REFERENCES
template std::basic_string<char> home_fold_pathname (std::basic_string <char> &&);
template std::basic_string<char> home_expand_pathname (std::basic_string <char> &&);
#endif
template std::basic_string<char> get_nice_pathname<char> (const char *);
template std::basic_string<char> get_nice_pathname<char> (const std::basic_string <char> &);
template std::pair<std::basic_string<char>,std::basic_string<char> > split_pathname (const std::basic_string<char> &, bool);
template std::basic_string<char> combine_pathname (const std::basic_string<char> &, const std::basic_string<char> &);
template bool DirEnt<char>::DefaultComparator::operator () (const DirEnt<char> &a, const DirEnt<char> &b) const;
template std::list<DirEnt<char> > list_dir (const char *dir,
			const UnaryCallback <const DirEnt<char> &, bool> &,
			const BinaryCallback <const DirEnt<char> &, const DirEnt<char> &, bool> &);

// Explicit instantiations (wchar_t)
template const std::basic_string<wchar_t> &get_home_dir<wchar_t> ();
template std::basic_string<wchar_t> get_current_dir<wchar_t> ();
template std::basic_string<wchar_t> home_fold_pathname (const std::basic_string <wchar_t> &);
template std::basic_string<wchar_t> home_expand_pathname (const std::basic_string <wchar_t> &);
#ifdef TIARY_HAVE_RVALUE_REFERENCES
template std::basic_string<wchar_t> home_fold_pathname (std::basic_string <wchar_t> &&);
template std::basic_string<wchar_t> home_expand_pathname (std::basic_string <wchar_t> &&);
#endif
template std::basic_string<wchar_t> get_nice_pathname<wchar_t> (const wchar_t *);
template std::basic_string<wchar_t> get_nice_pathname<wchar_t> (const std::basic_string <wchar_t> &);
template std::pair<std::basic_string<wchar_t>,std::basic_string<wchar_t> > split_pathname (const std::basic_string<wchar_t> &, bool);
template std::basic_string<wchar_t> combine_pathname (const std::basic_string<wchar_t> &, const std::basic_string<wchar_t> &);
template bool DirEnt<wchar_t>::DefaultComparator::operator () (const DirEnt<wchar_t> &a, const DirEnt<wchar_t> &b) const;
template std::list<DirEnt<wchar_t> > list_dir (const wchar_t *dir,
			const UnaryCallback <const DirEnt<wchar_t> &, bool> &,
			const BinaryCallback <const DirEnt<wchar_t> &, const DirEnt<wchar_t> &, bool> &);



std::string find_executable (const std::string &exe)
{
	std::string result;
	if (exe.empty ()) {
		// Empty input. Return empty string
	} else if (memchr (exe.data (), '/', exe.length ())) {
		// exe is a full/relative pathname
		if (exe[0] == '~')
			home_expand_pathname (exe).swap (result);
		else
			result = exe;
		if (access (result.c_str (), X_OK) != 0)
			result.clear ();
	} else if (const char *path = getenv ("PATH")) {
		// exe is a "bare" filename
		for (;;) {
			const char *colon = strchrnul (path, ':');
			if (path == colon)
				result = '.';
			else
				result.assign (path, colon);
			result += '/';
			result += exe;
			if (access (result.c_str (), X_OK) == 0)
				break;
			if (*colon != ':') {
				result.clear ();
				break;
			}
			path = colon+1;
		}
	}
	return result;
}




} // namespace tiary
