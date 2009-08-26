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


#include "common/format.h"
#include "common/unicode.h"
#include "common/string.h"
#include <wchar.h>
#include <string.h>

namespace tiary {
namespace detail {

FormatArgs::~FormatArgs ()
{
}

void FormatArgs::add (wchar_t c)
{
	if (nargs < MAX_ARGS) {
		args += c;
		offset[++nargs] = args.size ();
	}
}

void FormatArgs::add (char c)
{
	if (nargs < MAX_ARGS) {
		wint_t wc = btowc (c);
		if (wc == WEOF)
			wc = L'?';
		args += wchar_t(wc);
		offset[++nargs] = args.size ();
	}
}

void FormatArgs::add (UTF8Tag<char> ch)
{
	if (nargs < MAX_ARGS) {
		char c = ch.val;
		wchar_t wc = ((unsigned char)c < 0x80) ? c : L'?';
		args += wc;
		offset[++nargs] = args.size ();
	}
}

void FormatArgs::add (const wchar_t *s)
{
	if (nargs < MAX_ARGS) {
		args += s;
		offset[++nargs] = args.size ();
	}
}

void FormatArgs::add (const std::wstring &s)
{
	if (nargs < MAX_ARGS) {
		args += s;
		offset[++nargs] = args.size ();
	}
}

void FormatArgs::add (const char *s)
{
	if (nargs < MAX_ARGS) {
		args += mbs_to_wstring (s);
		offset[++nargs] = args.size ();
	}
}

void FormatArgs::add (const std::string &s)
{
	if (nargs < MAX_ARGS) {
		args += mbs_to_wstring (s);
		offset[++nargs] = args.size ();
	}
}

void FormatArgs::add (UTF8Tag<const char *> s)
{
	if (nargs < MAX_ARGS) {
		args += utf8_to_wstring (s.val);
		offset[++nargs] = args.size ();
	}
}

void FormatArgs::add (UTF8Tag<const std::string *> s)
{
	if (nargs < MAX_ARGS) {
		args += utf8_to_wstring (*s.val);
		offset[++nargs] = args.size ();
	}
}

void FormatArgs::add (unsigned x)
{
	const size_t BUFFER_SIZE = 3 * sizeof (unsigned);
	if (nargs < MAX_ARGS) {
		wchar_t buffer [BUFFER_SIZE];
		wchar_t *p = buffer + BUFFER_SIZE;
		do {
			*--p = L'0' + (x % 10);
		} while (x /= 10);
		args.append (p, buffer + BUFFER_SIZE);
		offset[++nargs] = args.size ();
	}
}

void FormatArgs::add (HexTag a)
{
	unsigned x = a.val;
	const size_t BUFFER_SIZE = 2 * sizeof (unsigned);
	if (nargs < MAX_ARGS) {
		wchar_t buffer [BUFFER_SIZE];
		wchar_t *p = buffer + BUFFER_SIZE;
		do {
			unsigned tmp = x % 16;
			x /= 16;
			*--p = (tmp < 10) ? (L'0'+tmp) : (L'a'-10+tmp);
		} while (x);
		args.append (p, buffer + BUFFER_SIZE);
		offset[++nargs] = args.size ();
	}
}

namespace {

template <typename ChT>
	std::basic_string<ChT> uniform_output (
			const ChT *format,
			const std::wstring &original_args, // Original args in wchar_t, used to compute the screen width
			const unsigned *original_offset,   // Original offset corresponding to original_args
			const std::basic_string<ChT> &args,// Converted args, can be the same with original_args if ChT=wchar_t
			const unsigned *offset,            // Converted offset, can be the same with original_offset if ChT=wchar_t
			unsigned nargs)
{
	std::basic_string<ChT> ret;
	const ChT *p_scan = format;
	while (const ChT *percentage = strchr (p_scan, ChT('%'))) { // tiary::strchr takes both char and wchar_t arguments
		ChT next = percentage[1];
		if (next == 0)
			break;
		else if (next == ChT('%')) {
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
			while (unsigned (*p_scan - ChT('0')) < 10)
				wid = wid * 10 + unsigned (*p_scan++ - ChT('0'));
			unsigned id = *p_scan++ - ChT('a');
			if (id < nargs) {
				unsigned scrwid = 0;
				if (wid)
					scrwid = ucs_width (original_args.data () + original_offset[id], original_offset[id+1] - original_offset[id]);
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

} // anonymous namespace


std::wstring FormatArgs::output (const wchar_t *format) const
{
	return uniform_output (format, args, offset, args, offset, nargs);
}

std::string FormatArgs::output (const char *format) const
{
	std::string bargs; // Converted
	unsigned boffset[16]; // Converted
	unsigned bn = 0;

	bargs.reserve (args.size() * 2); // Estimate

	mbstate_t state;
	memset (&state, 0, sizeof state);

	for (size_t i=0, len=args.size(); i<len; ++i) {
		while (bn<nargs && i>=offset[bn])
			boffset[bn++] = bargs.size ();
		char buffer[16];
		size_t l = wcrtomb (buffer, args[i], &state);
		if (l == size_t(-1)) {
			memset (&state, 0, sizeof state);
			buffer[0] = '?';
			l = 1;
		}
		bargs.append (buffer, l);
	}
	size_t tmp = bargs.size ();
	while (bn<nargs)
		boffset[bn++] = tmp;
	boffset[bn] = tmp;

	return uniform_output (format, args, offset, bargs, boffset, bn);
}

std::string FormatArgs::output (UTF8Tag<const char *> format) const
{
	std::string bargs; // Converted
	unsigned boffset[16]; // Converted
	unsigned bn = 0;

	bargs.reserve (args.size() * 2); // Estimate

	for (size_t i=0, len=args.size(); i<len; ++i) {
		while (bn<nargs && i>=offset[bn])
			boffset[bn++] = bargs.size ();
		char buffer[4];
		char *p = wchar_to_utf8 (buffer, args[i]);
		bargs.append (buffer, p);
	}
	size_t tmp = bargs.size ();
	while (bn<nargs)
		boffset[bn++] = tmp;
	boffset[bn] = tmp;

	return uniform_output (format.val, args, offset, bargs, boffset, bn);
}

} // namespace detail
} // namespace tiary
