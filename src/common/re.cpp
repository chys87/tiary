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


#ifdef TIARY_USE_RE2

#include "common/re.h"
#include "common/unicode.h"

namespace tiary {
namespace {

const RE2::Options &make_options() {
	static const RE2::Options options = []{
		RE2::Options options;
		options.set_case_sensitive(false);
		return options;
	}();
	return options;
}

} // namespace

Re::Re(const std::wstring &regex) : utf8_re_string_('(' + wstring_to_utf8(regex) + ')'), re_(utf8_re_string_, make_options()) {
}

std::vector<std::pair<size_t, size_t>> Re::match(const std::wstring &str) const {
	std::string utf8 = wstring_to_utf8 (str);

	std::vector <std::pair <size_t, size_t> > ret;

	re2::StringPiece input(utf8);
	re2::StringPiece match;

	while (RE2::FindAndConsume(&input, re_, &match)) {
		// Values in ovector are in bytes, not in UTF-8 characters
		size_t wchar_offset = utf8_count_chars({utf8.data(), size_t(match.data() - utf8.data())});
		size_t wchar_len = utf8_count_chars({match.data(), match.length()});
		ret.push_back (std::make_pair (wchar_offset, wchar_len));
	}

	return ret;
}

bool Re::basic_match(const std::wstring &str) const {
	return RE2::PartialMatch(wstring_to_utf8(str), re_);
}

} // namespace tiary



#endif // TIARY_USE_RE2
