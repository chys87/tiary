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
	size_t found = 0;
	for (;;) {
		// Standard guarantees npos == size_t(-1)
		found = text.find (L'&', found) + 1;
		if (found == 0 /* No "&" found */ ||
				found >= text.size () /* "&" at the end of string */) {
			// No "&" found.
			hotkey_pos = size_t (-1);
			break;
		}
		text.erase (found-1, 1);
		wchar_t c = text.data ()[found-1];
		// XXX What's "iswprint" here for?
		if ((c != L'&') && iswprint (c)) {
			hotkey_pos = found-1;
			break;
		}
	}
}


} // namespace tiary::ui
} // namespace ui
