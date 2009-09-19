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


#include "common/misc.h"
#include "common/containers.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace tiary {

bool read_whole_file (FILE *fp, std::vector<char> &ret, size_t estimated_size)
{
	if (estimated_size < 128) {
		estimated_size = 128;
	}
	size_t used = 0;
	std::vector<char> buf (estimated_size, '\0');
	size_t l;
	while ((l = fread_unlocked (&buf[used], 1, estimated_size-used, fp)) != 0) {
		used += l;
		if (estimated_size < used * 2) {
			estimated_size = used * 2;
			buf.resize (estimated_size);
		}
	}
	if (ferror_unlocked (fp)) {
		return false;
	}
	buf.resize (used);
	buf.swap (ret);
	return true;
}

template <typename MapT>
	const typename MapT::mapped_type::value_type *
	map_query (const MapT &map, const typename MapT::key_type::value_type *key)
{
	typename MapT::const_iterator it = map.find (key);
	if (it != map.end ()) {
		return it->second.c_str ();
	} else {
		return 0;
	}
}

// Explicit instantiations (C++98 standard syntax)
template const char    *map_query <StringOrderedMap       > (const StringOrderedMap &,        const char *);
template const wchar_t *map_query <WStringOrderedMap      > (const WStringOrderedMap &,       const wchar_t *);
template const char    *map_query <StringLocaleOrderedMap > (const StringLocaleOrderedMap &,  const char *);
template const wchar_t *map_query <WStringLocaleOrderedMap> (const WStringLocaleOrderedMap &, const wchar_t *);
template const char    *map_query <StringUnorderedMap     > (const StringUnorderedMap &,      const char *);
template const wchar_t *map_query <WStringUnorderedMap    > (const WStringUnorderedMap &,     const wchar_t *);


bool safe_write_file (const char *filename, const void *ptr, size_t len)
{
	// If we have to write a new file, then it's simple.
	// If we have to overwite an existing file:
	//   If the file has more than one links, there is only one choice;
	//   Otherwise, rename the original file, write the new file, and unlink the original
	std::string backup_name;
	struct stat info;
	if (stat (filename, &info) == 0 // Already exists
#if defined S_ISREG
			&& S_ISREG (info.st_mode) // And is a regular file
#elif defined S_IFREG
			&& (info.st_mode & S_IFREG) // And is a regular file
#endif
			&& info.st_nlink == 1) { // And has only one link
		backup_name = filename;
		backup_name += ".tiary.bak";
		if (rename (filename, backup_name.c_str ()) != 0) { // Back up old file: failed
			backup_name.clear ();
		}
	}

	// Now write content to new file
	if (FILE *fp = fopen (filename, "wb")) {
		size_t fwrite_return = fwrite_unlocked (ptr, 1, len, fp);
		fclose (fp);
		if (fwrite_return == len) { // Successful. Remove backup file
			if (!backup_name.empty ()) {
				unlink (backup_name.c_str ());
			}
			return true;
		}
		// Failed. Remove partially written file
		unlink (filename);
	}

	// Failed. Restore original file
	if (!backup_name.empty ()) {
		rename (backup_name.c_str (), filename);
	}
	return false;
}


unsigned environment_expand (std::string &s)
{
	const char *src = s.c_str ();
	const char *dollar = strchr (src, '$');
	if (dollar == 0) {
		return 0;
	}
	unsigned expansions = 0;
	std::string r;
	do {
		r.append (src, dollar);
		std::string varname;
		if (dollar[1] == '{') {
			const char *dollarend = strchrnul (dollar+2, '}');
			if (*dollarend == '\0') {
				src = dollar;
				dollar = dollarend;
				break;
			}
			varname.assign (dollar+2, dollarend);
			src = dollarend+1;
		}
		else {
			size_t varnamelen = strspn (dollar+1, "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_");
			varname.assign (dollar+1, varnamelen);
			src = dollar+1+varnamelen;
		}
		if (const char *varval = getenv (varname.c_str ())) {
			r += varval;
		}
		++expansions;
		dollar = strchrnul (src, '$');
	} while (*dollar);
	r.append (src, dollar);
	r.swap (s);
	return expansions;
}

}
