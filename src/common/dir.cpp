// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2016, 2018 chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/

/**
 * @file	common/dir.cpp
 * @author	chys <admin@chys.info>
 * @brief	Implements functions declared in common/dir.h
 */


#include "common/dir.h"
#include "common/unicode.h"
#include "common/string.h"
#include "common/algorithm.h"
#include <unistd.h>
#include <stdlib.h>
#include <limits.h> // PATH_MAX
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <dirent.h>
#include <pwd.h>
#include <locale>
#include <functional>
#include <algorithm>
#include <locale>
#include <string_view>
#include <stdint.h>

#ifndef PATH_MAX
# define PATH_MAX 4096
#endif

#ifndef S_ISDIR
# if defined S_IFMT && defined S_IFDIR
#  define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
# else
#  error "WTF system are you using?"
# endif
#endif

#ifndef HAVE_EUIDACCESS
# ifdef HAVE_EACCESS
#  define euidaccess eaccess
# else
#  define euidaccess access
# endif
#endif

#if defined HAVE_FACCESSAT && defined AT_FDCWD && defined AT_EACCESS
# undef euidaccess
# define euidaccess(name,mode) faccessat(AT_FDCWD,name,mode,AT_EACCESS)
#endif


namespace tiary {

namespace {

std::string get_home_dir_base ()
{
	const char *result = getenv ("HOME");
	if (result == 0 || *result != '/') {
		if (struct passwd *data = getpwuid (getuid ())) {
			result = data->pw_dir;
		}
	}
	if (result == 0 || *result != '/') {
		result = "/";
	}
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
	std::string_view result;
	if (!user || !*user) {
		result = get_home_dir<char>();
	} else {
		if (struct passwd *data = getpwnam (user)) {
			result = data->pw_dir;
		}
	}
	if (result.empty() || result[0] != '/') {
		result = "/";
	}
	return std::string(result);
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
		if (file[0] != '/') {
			ret += '/';
		}
		ret += file;
	}
	return ret;
}

std::wstring make_home_dirname (const wchar_t *file)
{
	std::wstring ret = get_home_dir <wchar_t> ();
	if (file) {
		if (file[0] != L'/') {
			ret += L'/';
		}
		ret += file;
	}
	return ret;
}


template <> std::basic_string<char> get_current_dir <char> ()
{
#ifdef HAVE_GET_CURRENT_DIR_NAME
	if (char *dir = get_current_dir_name ()) {
		std::string r = dir;
		free (dir);
		return r;
	}
	else {
		return std::string ();
	}
#else
	char buffer[PATH_MAX];
	if (getcwd (buffer, sizeof buffer) == buffer) {
		return buffer;
	}
	else {
		return std::string ();
	}
#endif
}

template <> std::basic_string<wchar_t> get_current_dir <wchar_t> ()
{
	return mbs_to_wstring (get_current_dir<char>());
}

std::wstring get_full_pathname (const std::wstring &name)
{
	std::vector<std::wstring> split = split_string (
			(name[0] != L'/' ? get_current_dir <wchar_t> () + L'/' + name : name),
			L'/');
	// Empty tokens are eliminated by split_string, effectively
	// getting "//" replaced by "/"

	// Eliminate single dots and double dots
	auto iw = split.begin();
	for (auto it = iw; it != split.end(); ++it) {
		if (*it == L".") {
		} else if (*it == L"..") {
			if (iw != split.begin()) {
				--iw;
			}
		} else if (iw != it) {
			*iw++ = std::move(*it);
		} else {
			++iw;
		}
	}
	if (iw != split.end()) {
		split.resize(iw - split.begin());
	}

	if (split.empty ()) {
		split.push_back (std::wstring ());
	}

	std::wstring ret;
	for (auto it = split.begin (); it != split.end (); ++it) {
		ret += L'/';
		ret += *it;
	}
	return ret;
}

namespace {

template <typename ChT> inline
std::basic_string<ChT> home_fold_pathname_impl (const std::basic_string<ChT> &name)
{
	std::basic_string <ChT> fullname = name;
	const std::basic_string <ChT> &homedir = get_home_dir <ChT> ();
	size_t homelen = homedir.length ();
	if (fullname.length () >= homelen) {
		if (fullname.length() == homelen || fullname[homelen] == ChT('/')) {
			if (fullname.compare (0, homelen, homedir) == 0) {
				fullname.replace (0, homelen, 1, ChT('~'));
			}
		}
	}
	return fullname;
}

} // anonymous namespace

std::string home_fold_pathname (const std::string &name)
{
	return home_fold_pathname_impl (name);
}

std::wstring home_fold_pathname (const std::wstring &name)
{
	return home_fold_pathname_impl (name);
}

namespace {

template <typename ChT> inline
std::basic_string<ChT> home_expand_pathname_impl(std::basic_string_view<ChT> name) {
	if (!name.empty() && name[0] == ChT('~')) {
		auto it = std::find(name.begin() + 1, name.end(), ChT('/'));
		std::basic_string<ChT> homedir = get_home_dir (std::basic_string <ChT> (name.begin ()+1, it));
		if (!homedir.empty ()) {
			homedir.append (it, name.end ());
			return homedir;
		}
	}
	return std::basic_string<ChT>(name);
}

} // anonymous namespace

std::string home_expand_pathname(std::string_view name) {
	return home_expand_pathname_impl (name);
}

std::wstring home_expand_pathname(std::wstring_view name) {
	return home_expand_pathname_impl (name);
}

std::wstring get_nice_pathname (const std::wstring &name)
{
	std::wstring fullname = get_full_pathname (name);
	std::wstring homefold = home_fold_pathname (fullname);
	std::wstring curdir = get_current_dir<wchar_t>();
	// Special case: curdir == '/'
	if (curdir.length () < 2) {
		fullname.swap (homefold);
		return fullname;
	}
	// Special case: curdir == filename
	if (curdir == fullname) {
		fullname = L'.';
		return fullname;
	}
	curdir += L'/';
	fullname += L'/'; // In case fullname is an ancestor dir of curdir
	/*
	 * curdir:     ............/b_1/.../b_y/
	 * fullname:   ............/c_1/.../c_z/
	 *             | <---  --->|
	 *               length ox
	 */
	unsigned y;
	size_t ox;
	std::wstring::const_iterator ox_it = std::mismatch (
			fullname.begin(), fullname.end(),
			curdir.c_str () // Not to be replaced by begin() (Not all implementations always maintain a null at the end!)
			).first;
	while (*(ox_it-1) != L'/') {
		--ox_it;
	}
	ox = ox_it - fullname.begin();

	y = std::count(curdir.begin() + ox, curdir.end(), L'/');
	// Use either dot-dots or fullnames. These codes also apply to things under current directory
	unsigned dotdotlen = y*3;
	if ((y<3 && dotdotlen<ox) || (y==3 && dotdotlen*2<ox) || dotdotlen*4<ox) {
		// Use dot-dots iff 
		// (1) No more than two levels, and shorter; or
		// (2) More than two levels, and MUCH shorter
		fullname.replace (0, ox, dotdotlen, L'.');
		// Fill y dot-dots
		std::wstring::iterator it = fullname.begin ();
		std::wstring::iterator end = it + dotdotlen;
		while (it < end) {
			it += 2;
			*it++ = L'/';
		}
	}
	fullname.resize (fullname.length () - 1);
	if (homefold.length () < fullname.length ()) {
		fullname.swap (homefold);
	}
	return fullname;
}

unsigned get_file_attr (const char *name)
{
	struct stat st_buf;
	unsigned attr = 0;
	if (stat (name, &st_buf) != 0) {
		attr = FILE_ATTR_NONEXIST;
	}
	else {
		if (S_ISDIR (st_buf.st_mode)) {
			attr |= FILE_ATTR_DIRECTORY;
		}
		if (st_buf.st_mode & 0111) {
			// It is executable by someone. But how about me?
			// Ask the kernel!
			if (euidaccess (name, X_OK) == 0) {
				attr |= FILE_ATTR_EXECUTABLE;
			}
		}
	}
	return attr;
}

unsigned get_file_attr (const wchar_t *name)
{
	return get_file_attr (wstring_to_mbs (name).c_str ());
}

std::pair<std::wstring,std::wstring> split_pathname (const std::wstring &name, bool canonicalize)
{
	std::wstring::size_type last_split = name.rfind (L'/');

	// If a backslash exists in name, this is true;
	// If not, last_split = npos = -1, this is also true;
	std::wstring basename (name, last_split + 1);

	std::wstring dirname;
	if (last_split == std::wstring::npos) { // No backslash exist in name
		if (canonicalize) {
			dirname = get_current_dir <wchar_t> ();
		}
		else {
			dirname.assign (1, L'.');
		}
	}
	else {
		if (last_split == 0) { // Dirname is root
			++last_split;
		}
		dirname.assign (name, 0, last_split);
		if (canonicalize) {
			dirname = get_full_pathname (dirname);
		}
	}

	return {std::move(dirname), std::move(basename)};
}

std::wstring combine_pathname(std::wstring_view path, std::wstring_view basename) {
	std::wstring ret;
	if (path.length() != 1 || path[0] != L'.') {
		ret = path;
		if (ret.empty() || *ret.rbegin()!=L'/') {
			ret += L'/';
		}
	}
	ret += basename;
	return ret;
}



namespace {

class DefaultDirEntComparator
{
	std::locale loc;
public:
	bool operator () (const DirEnt &, const DirEnt &) const;
};

bool DefaultDirEntComparator::operator () (const DirEnt &a, const DirEnt &b) const
{
	// Directory < Non-directory
	if ((a.attr ^ b.attr) & FILE_ATTR_DIRECTORY) {
		return (a.attr & FILE_ATTR_DIRECTORY);
	}
	// Otherwise, compare name
	return loc (a.name, b.name);
}

} // anonymous namespace


DirEntList list_dir (
			const std::wstring &directory,
			const std::function<bool(const DirEnt &)> &filter,
			const std::function<bool(const DirEnt &, const DirEnt &)> &comp
		)
{
	DirEntList filelist;

	std::string dirname = wstring_to_mbs (directory);

	if (DIR *dir = ::opendir (dirname.c_str ())) {
		DirEnt tmp_ent;
		if (*dirname.rbegin() != '/') {
			dirname += '/';
		}
		while (struct dirent *ent = readdir (dir)) {
			tmp_ent.name = mbs_to_wstring (ent->d_name);
			tmp_ent.attr = get_file_attr (dirname + ent->d_name);
			if (!filter (tmp_ent)) {
				filelist.push_back (std::move (tmp_ent));
			}
		}
		closedir (dir);
		// Sort them
		std::sort(filelist.begin(), filelist.end(), comp);
	}
	return filelist;
}

DirEntList list_dir(const std::wstring &directory, const std::function<bool(const DirEnt &)> &filter)
{
	static const DefaultDirEntComparator comp;
	return list_dir(directory, filter, std::ref(comp));
}

// Explicit instantiations (char)
template const std::basic_string<char> &get_home_dir<char> ();
template std::basic_string<char> get_current_dir<char> ();

// Explicit instantiations (wchar_t)
template const std::basic_string<wchar_t> &get_home_dir<wchar_t> ();
template std::basic_string<wchar_t> get_current_dir<wchar_t> ();



std::string find_executable(std::string_view exe) {
	std::string result;
	if (exe.empty ()) {
		// Empty input. Return empty string
	}
	else if (memchr (exe.data (), '/', exe.length ())) {
		// exe is a full/relative pathname
		result = home_expand_pathname (exe);
		if ((get_file_attr (result) & (FILE_ATTR_DIRECTORY|FILE_ATTR_EXECUTABLE))
				!= FILE_ATTR_EXECUTABLE) {
			result.clear ();
		}
	}
	else if (const char *path = getenv ("PATH")) {
		// exe is a "bare" filename
		for (;;) {
			const char *colon = strchrnul (path, ':');
			if (path == colon) {
				result = '.';
			}
			else {
				result.assign (path, colon);
			}
			result += '/';
			result += exe;
			if ((get_file_attr (result) & (FILE_ATTR_DIRECTORY|FILE_ATTR_EXECUTABLE))
					== FILE_ATTR_EXECUTABLE) {
				break;
			}
			if (*colon != ':') {
				result.clear ();
				break;
			}
			path = colon+1;
		}
	}
	return result;
}

int my_mkstemp (std::string &name)
{
	size_t pipe_sign = name.find ('|');
	if (pipe_sign == std::string::npos) {
		// No pipe sign exist in name.
		return -1;
	}
	struct timeval tv;
	static uint64_t seed = 0;
	gettimeofday (&tv, 0);
	seed += uint64_t (getpid ()) ^ (uint64_t (tv.tv_usec) << 16) ^ tv.tv_sec;

	name.insert (pipe_sign, 5, '|');
	// '|' is now replaced by 6 pipesigns

	for (unsigned left_attempts = 20; left_attempts; seed += 7777, --left_attempts) {
		static const char letters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_+";
		uint64_t v = seed;
		std::string::iterator iw = name.begin () + pipe_sign;
		*iw++ = letters[v % 64];
		v /= 64;
		*iw++ = letters[v % 64];
		v /= 64;
		*iw++ = letters[v % 64];
		v /= 64;
		*iw++ = letters[v % 64];
		v /= 64;
		*iw++ = letters[v % 64];
		v /= 64;
		*iw++ = letters[v % 64];
		int fd = open (name.c_str (), O_RDWR|O_CREAT|O_EXCL
#ifdef O_CLOEXEC
				|O_CLOEXEC
#endif
				, S_IRUSR|S_IWUSR);
		if (fd >= 0) {
#if !defined O_CLOEXEC && defined F_GETFD && defined F_SETFD && defined FD_CLOEXEC
			int fdflag = fcntl (fd, F_GETFD);
			if ((fdflag >= 0) && !(fdflag & FD_CLOEXEC)) {
				fcntl (fd, F_SETFD, fdflag|FD_CLOEXEC);
			}
#endif
			return fd;
		}
	}
	return -1;
}

} // namespace tiary
