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

#include "ui/uistring_base.h"
#include "common/unicode.h"
#include <wctype.h>

namespace tiary {
namespace ui {


UIStringBase::UIStringBase (const std::wstring &s, unsigned options)
	: text (s)
	, hotkey_pos (size_t(-1))
{
	if (!(options & NO_HOTKEY)) {
		update ();
	}
}

void UIStringBase::set_text (const std::wstring &s, unsigned options)
{
	text = s;
	hotkey_pos = size_t(-1);
	if (!(options & NO_HOTKEY)) {
		update ();
	}
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

void UIStringBase::update ()
{
	hotkey_pos = size_t(-1);
	size_t found = 0;
	while (found<text.size() && (found = text.find (L'&', found)) != std::wstring::npos) {
		wchar_t c;
		if (++found<text.size() && iswprint (c = text.data()[found])) {
			text.erase (--found, 1);
			if (hotkey_pos==size_t(-1) && c!=L'&') {
				hotkey_pos = found;
			}
			++found;
		}
	}
}


} // namespace tiary::ui
} // namespace ui
