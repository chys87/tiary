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

#include "ui/richtextlist.h"
#include "common/unicode.h"
#include "common/split_line.h"
#include <functional>
#include <algorithm>
#include <wchar.h>


namespace tiary {
namespace ui {

void MultiLineRichText::append(PaletteID id, std::wstring_view line_text) {
	lines.push_back({text.length(), line_text.length(), id, ucs_width(line_text)});
	text += line_text;
}

void MultiLineRichText::append(PaletteID id, std::wstring_view text_a, std::wstring_view text_b) {
	lines.push_back({
		text.length(),
		text_a.length() + text_b.length(),
		id,
		ucs_width(text_a) + ucs_width(text_b)
	});
	text.append(text_a);
	text.append(text_b);
}

void MultiLineRichText::append(PaletteID id, unsigned repeat, wchar_t ch) {
	lines.push_back({text.length(), repeat, id, repeat * ucs_width(ch)});
	text.append(repeat, ch);
}

void MultiLineRichText::append(PaletteID id) {
	lines.push_back({text.length(), 0, id, 0});
}

MultiLineRichText combine_lines(std::initializer_list<RichTextLineC> linec_list) {
	MultiLineRichText mrt;
	mrt.lines.reserve(linec_list.size());

	for (const RichTextLineC &linec: linec_list) {
		mrt.lines.push_back({mrt.text.length(), linec.text.length(), linec.id, ucs_width(linec.text)});
		mrt.text += linec.text;
	}
	return mrt;
}

RichTextLineList split_richtext_lines(std::wstring_view str, PaletteID id, unsigned wid) {
	RichTextLineList ret;
	for (const SplitStringLine &line: split_line(wid, str)) {
		ret.push_back({line.begin, line.len, id, line.wid});
	}
	return ret;
}

} // namespace tiary::ui
} // namespace tiary

