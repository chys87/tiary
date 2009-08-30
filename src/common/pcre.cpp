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


#ifdef TIARY_USE_PCRE

#include "common/pcre.h"
#include "common/unicode.h"
#include <pcre.h>

namespace tiary {


PcRe::PcRe (const std::wstring &regex)
{
    const char *err;
	int erroffset;
	re = pcre_compile2 (wstring_to_utf8 (regex).c_str (),
#ifdef PCRE_UTF8
			PCRE_UTF8|
#endif
#ifdef PCRE_NO_UTF8_CHECK
			PCRE_NO_UTF8_CHECK|
#endif
			PCRE_MULTILINE|PCRE_CASELESS,
			0, &err, &erroffset, 0);
	if (re) {
		re_ex = pcre_study ((const pcre *)re, 0, &err);
	} else {
		re_ex = 0;
	}
}

PcRe::~PcRe ()
{
	if (re_ex)
		pcre_free (re_ex);
	if (re)
		pcre_free (re);
}

std::vector <std::pair <size_t, size_t> > PcRe::match (const std::wstring &str) const
{
	size_t ovector_n = str.length () * 3;
	int *ovector = new int [ovector_n]; // Only first two-thirds are used for return value

	std::string utf8 = wstring_to_utf8 (str);

	std::vector <std::pair <size_t, size_t> > ret;

	int rc;
	if ((rc = pcre_exec ((const pcre *)re, (const pcre_extra *)re_ex,
			utf8.data (), utf8.length (), 0, 0, ovector, ovector_n)) > 0) {
		// rc==0 means too many substrings.
		// We have guaranteed enough space

		// Values in ovector are in bytes, not in UTF-8 characters
		ret.resize (rc);
		// FIXME: We can do some optimizations here
		for (int i=0; i<rc; ++i) {
			ret[i].first = mbs_to_wstring (utf8.substr (0, ovector[i*2])).length ();
			ret[i].second = mbs_to_wstring (utf8.substr (ovector[i*2], ovector[i*2+1]-ovector[i*2])).length ();
		}
	}

	delete [] ovector;
	return ret;
}

} // namespace tiary



#endif // TIARY_USE_PCRE
