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


#include "common/format.h"
#include "common/unicode.h"
#include "common/string.h"
#include <wchar.h>
#include <string.h>
#include <math.h>

namespace tiary {

void format_dec(std::wstring *pres, unsigned x, unsigned width, wchar_t fill) {
	const size_t BUFFER_SIZE = 3 * sizeof (unsigned);
	wchar_t buffer [BUFFER_SIZE];
	wchar_t *p = buffer + BUFFER_SIZE;
	do {
		*--p = L'0' + (x % 10);
	} while (x /= 10);
	if (width <= BUFFER_SIZE) {
		int diff = (buffer + BUFFER_SIZE - p) - width;
		if (diff < 0) {
			for (; diff; ++diff) {
				*--p = fill;
			}
		}
		pres->append(p, buffer + BUFFER_SIZE);
	} else {
		pres->append(width - (buffer + BUFFER_SIZE - p), fill);
		pres->append(p, buffer + BUFFER_SIZE);
	}
}

std::wstring format_dec(unsigned x, unsigned width, wchar_t fill) {
	std::wstring res;
	format_dec(&res, x, width, fill);
	return res;
}

std::string format_dec_narrow (unsigned x)
{
	const size_t BUFFER_SIZE = 3 * sizeof (unsigned);
	char buffer [BUFFER_SIZE];
	char *p = buffer + BUFFER_SIZE;
	do {
		*--p = '0' + (x % 10);
	} while (x /= 10);
	return std::string (p, buffer + BUFFER_SIZE);
}

void format_hex(std::wstring *pres, unsigned x) {
	const size_t BUFFER_SIZE = 2 * sizeof (unsigned);
	wchar_t buffer [BUFFER_SIZE];
	wchar_t *p = buffer + BUFFER_SIZE;
	do {
		unsigned tmp = x % 16;
		x /= 16;
		*--p = (tmp < 10) ? (L'0'+tmp) : (L'a'-10+tmp);
	} while (x);
	pres->append(p, buffer + BUFFER_SIZE);
}

std::wstring format_hex(unsigned x) {
	std::wstring res;
	format_hex(&res, x);
	return res;
}

std::string format_hex_narrow (unsigned x)
{
	const size_t BUFFER_SIZE = 2 * sizeof (unsigned);
	char buffer [BUFFER_SIZE];
	char *p = buffer + BUFFER_SIZE;
	do {
		unsigned tmp = x % 16;
		x /= 16;
		*--p = (tmp < 10) ? ('0'+tmp) : ('a'-10+tmp);
	} while (x);
	return std::string (p, buffer + BUFFER_SIZE);
}

std::wstring format_double(double x, unsigned int_digits, unsigned frac_digits) {
	constexpr int kBufSize = 128;
	wchar_t buf[kBufSize];
	int n = swprintf(buf, kBufSize, L"%*.*f", int_digits + frac_digits + 1, frac_digits, x);
	if (n >= 0 && n < kBufSize) {
		return std::wstring(buf, n);
	} else {
		return {};
	}
}

Format::~Format() = default;

void Format::add(wchar_t c) {
	if (nargs_ < MAX_ARGS) {
		args_ += c;
		offset_[++nargs_] = args_.size();
	}
}

void Format::add(std::wstring_view s) {
	if (nargs_ < MAX_ARGS) {
		args_ += s;
		offset_[++nargs_] = args_.size();
	}
}

void Format::add(unsigned x) {
	if (nargs_ < MAX_ARGS) {
		format_dec(&args_, x);
		offset_[++nargs_] = args_.size();
	}
}

void Format::add(HexTag a) {
	if (nargs_ < MAX_ARGS) {
		format_hex(&args_, static_cast<unsigned>(a));
		offset_[++nargs_] = args_.size();
	}
}

std::wstring Format::result() const {
	std::wstring ret;
	ret.reserve(format_.size() + args_.size());
	size_t start = 0;
	size_t percent;
	while ((percent = format_.find(L'%', start)) != format_.npos && percent + 1 < format_.size()) {
		wchar_t next = format_[percent + 1];
		if (next == L'%') {
			ret.append(&format_[start], &format_[percent + 1]);
			start = percent + 2;
			continue;
		}

		ret.append(&format_[start], &format_[percent]);
		start = percent + 1;

		bool left_align = false;
		wchar_t fill_char = L' ';
		if (next == L'-') {
			left_align = true;
			++start;
		} else if (next == L'0') {
			fill_char = L'0';
			++start;
		}
		unsigned wid = 0;
		while (start < format_.size() - 1 && (format_[start] >= L'0' && format_[start] <= L'9')) {
			wid = wid * 10 + unsigned(format_[start++] - L'0');
		}
		unsigned id = format_[start++] - L'a';
		if (id < nargs_) {
			std::wstring_view data{args_.data() + offset_[id], offset_[id + 1] - offset_[id]};
			unsigned scrwid;
			if (wid && wid > (scrwid = ucs_width(data))) {
				unsigned fix_width = wid - scrwid;
				if (left_align) {
					ret += data;
					ret.append(fix_width, fill_char);
				} else {
					ret.append(fix_width, fill_char);
					ret += data;
				}
			} else {
				ret += data;
			}
		}
	}
	ret.append(&format_[start], format_.size() - start);
	return ret;
}

} // namespace tiary
