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


#include "common/format.h"
#include "common/unicode.h"
#include "common/string.h"
#include <wchar.h>
#include <string.h>

namespace tiary {

std::wstring format_dec (unsigned x)
{
	const size_t BUFFER_SIZE = 3 * sizeof (unsigned);
	wchar_t buffer [BUFFER_SIZE];
	wchar_t *p = buffer + BUFFER_SIZE;
	do {
		*--p = L'0' + (x % 10);
	} while (x /= 10);
	return std::wstring (p, buffer + BUFFER_SIZE);
}

std::wstring format_hex (unsigned x)
{
	const size_t BUFFER_SIZE = 2 * sizeof (unsigned);
	wchar_t buffer [BUFFER_SIZE];
	wchar_t *p = buffer + BUFFER_SIZE;
	do {
		unsigned tmp = x % 16;
		x /= 16;
		*--p = (tmp < 10) ? (L'0'+tmp) : (L'a'-10+tmp);
	} while (x);
	return std::wstring (p, buffer + BUFFER_SIZE);
}

Format::~Format ()
{
}

Format &Format::operator << (wchar_t c)
{
	if (nargs < MAX_ARGS) {
		args += c;
		offset[++nargs] = args.size ();
	}
	return *this;
}

Format &Format::operator << (const wchar_t *s)
{
	if (nargs < MAX_ARGS) {
		args += s;
		offset[++nargs] = args.size ();
	}
	return *this;
}

Format &Format::operator << (const std::wstring &s)
{
	if (nargs < MAX_ARGS) {
		args += s;
		offset[++nargs] = args.size ();
	}
	return *this;
}

Format &Format::operator << (unsigned x)
{
	if (nargs < MAX_ARGS) {
		args += format_dec (x);
		offset[++nargs] = args.size ();
	}
	return *this;
}

Format &Format::operator << (HexTag a)
{
	if (nargs < MAX_ARGS) {
		args += format_hex (a.val);
		offset[++nargs] = args.size ();
	}
	return *this;
}

Format::operator std::wstring () const
{
	std::wstring ret;
	const wchar_t *p_scan = format;
	while (const wchar_t *percentage = wcschr (p_scan, L'%')) {
		wchar_t next = percentage[1];
		if (next == L'\0')
			break;
		else if (next == L'%') {
			ret.append (p_scan, percentage+1);
			p_scan = percentage + 2;
		} else {
			ret.append (p_scan, percentage);
			p_scan = percentage+1;

			const unsigned OPTS_FILL_ZERO = 1;
			const unsigned OPTS_LEFT_ALIGN = 2;
			unsigned opts = 0;
			if (next == L'-') {
				opts |= OPTS_LEFT_ALIGN;
				++p_scan;
			} else if (next == L'0') {
				opts |= OPTS_FILL_ZERO;
				++p_scan;
			}
			unsigned wid = 0;
			while (unsigned (*p_scan - L'0') < 10)
				wid = wid * 10 + unsigned (*p_scan++ - L'0');
			unsigned id = *p_scan++ - L'a';
			if (id < nargs) {
				unsigned scrwid = 0;
				if (wid)
					scrwid = ucs_width (args.data () + offset[id], offset[id+1] - offset[id]);
				if (!(opts&OPTS_LEFT_ALIGN) && wid>scrwid)
					ret.append (wid - scrwid, (opts&OPTS_FILL_ZERO) ? L'0' : L' ');
				ret.append (args.data() + offset[id], offset[id+1] - offset[id]);
				if ((opts&OPTS_LEFT_ALIGN) && wid>scrwid)
					ret.append (wid - scrwid, (opts&OPTS_FILL_ZERO) ? L'0' : L' ');
			}
		}
	}
	ret += p_scan;
	return ret;
}

} // namespace tiary
