// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2018, 2019, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/

#include "ui/uistring_base.h"
#include "common/unicode.h"
#include <wctype.h>

namespace tiary {
namespace ui {

namespace {

// Find the hotkey character position,
// and remove the first '&' from the string
// Returns the position of the hotkey character or -1
size_t remove_amp (std::wstring &text, unsigned options);

} // anonymous namespace

UIStringBase::UIStringBase(std::wstring_view s, unsigned options)
	: text (s)
	, hotkey_pos (remove_amp (text, options))
{
}

UIStringBase::UIStringBase(std::wstring &&s, unsigned options)
	: text(std::move(s))
	, hotkey_pos (remove_amp (text, options)) {
}

void UIStringBase::set_text(std::wstring_view s, unsigned options) {
	text = s;
	hotkey_pos = remove_amp (text, options);
}

void UIStringBase::set_text(std::wstring &&s, unsigned options) {
	text = std::move(s);
	hotkey_pos = remove_amp (text, options);
}

wchar_t UIStringBase::get_hotkey () const
{
	if (hotkey_pos < text.length ()) {
		return text[hotkey_pos];
	}
	else {
		return L'\0';
	}
}

namespace {

size_t remove_amp (std::wstring &text, unsigned options)
{
	if (options & UIStringBase::NO_HOTKEY) {
		return size_t (-1);
	}
	size_t found = 0;
	for (;;) {
		// Standard guarantees npos == size_t(-1)
		found = text.find (L'&', found) + 1;
		if (found == 0 /* No "&" found */ ||
				found >= text.size () /* "&" at the end of string */) {
			// No "&" found.
			return size_t (-1);
		}
		text.erase (found-1, 1);
		wchar_t c = text.data ()[found-1];
		if (c != L'&') {
			return (found-1);
		}
	}
}

} // anonymous namespace

} // namespace tiary::ui
} // namespace ui
