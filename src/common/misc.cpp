// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2019, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#include "common/misc.h"
#include "common/containers.h"
#include "common/string.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace tiary {

bool read_whole_file(FILE *fp, std::string &ret, size_t estimated_size)
{
	if (estimated_size < 128) {
		estimated_size = 128;
	}

	ret.clear();
	std::unique_ptr<char[]> buf{new char[estimated_size]};

	size_t l;
	while ((l = fread_unlocked(buf.get(), 1, estimated_size, fp)) != 0) {
		ret.append(buf.get(), l);
	}
	if (ferror_unlocked(fp)) {
		return false;
	}
	return true;
}

#ifndef S_ISREG
# if defined S_IFMT && defined S_IFREG
#  define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
# else
#  error "What funny system are you using?"
# endif
#endif

namespace {

bool direct_write(const char *filename, std::string_view data, std::string_view data2) {
	FILE *fp = fopen(filename, "wb");
	if (fp == nullptr) {
		return false;
	}

	if (fwrite_unlocked(data.data(), 1, data.size(), fp) != data.size()) {
		fclose(fp);
		return false;
	}

	if (!data2.empty() && fwrite_unlocked(data2.data(), 1, data2.size(), fp) != data2.size()) {
		fclose(fp);
		return false;
	}

	fclose(fp);
	return true;
}

inline bool must_direct_write(const char *filename) {
	struct stat st;
	return (lstat(filename, &st) == 0 && (!S_ISREG(st.st_mode) || st.st_nlink != 1));
}

} // namespace

bool safe_write_file(const char *filename, std::string_view data, std::string_view data2) {
	if (must_direct_write(filename)) {
		// If filename is not a regular file, or has multiple links, there is only one choice
		return direct_write(filename, data, data2);
	} else {
		// Otherwise, write to a temporary file and rename it
		std::string tmp_name = filename;
		tmp_name += ".tiary.tmp";
		if (!direct_write(tmp_name.c_str(), data, data2)) {
			unlink(tmp_name.c_str());
			return false;
		}
		if (rename(tmp_name.c_str(), filename) != 0) {
			unlink(tmp_name.c_str());
			return false;
		}
		return true;
	}
}


std::string environment_expand(std::string_view s) {
	std::string r;
	size_t start_pos = 0;
	size_t dollar_pos;
	while ((dollar_pos = s.find('$', start_pos)) != s.npos) {
		r += s.substr(start_pos, dollar_pos - start_pos);
		std::string varname;
		if (dollar_pos + 1 < s.length() && s[dollar_pos + 1] == '{') {
			size_t dollar_end = s.find('}', dollar_pos + 2);
			if (dollar_end == s.npos) {
				break;
			}
			varname = s.substr(dollar_pos + 2, dollar_end - (dollar_pos + 2));
			start_pos = dollar_end + 1;
		} else {
			size_t dollar_end = s.find_first_not_of("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_"sv, dollar_pos + 1);
			varname = s.substr(dollar_pos + 1, dollar_end - (dollar_pos + 1));
			start_pos = dollar_end;
		}
		if (const char *varval = getenv (varname.c_str ())) {
			r += varval;
		}
	}
	if (start_pos < s.length()) {
		r += s.substr(start_pos);
	}
	return r;
}

} // namespace tiary
