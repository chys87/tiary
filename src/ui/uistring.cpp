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


#include "ui/uistring.h"
#include "ui/size.h"
#include "ui/control.h"
#include "common/unicode.h"
#include "common/algorithm.h"

namespace tiary {
namespace ui {



UIString::UIString (const std::wstring &s, unsigned options)
	: UIStringBase (s, options)
	, lines (0)
	, max_width (0)
	, split_cache_wid (0)
	, split_cache ()
{
	update ();
}

void UIString::set_text (const std::wstring &s, unsigned options)
{
	UIStringBase::set_text (s, options);
	update ();
}

const SplitStringLineList &UIString::split_line (unsigned wid) const
{
	if (wid != split_cache_wid) {
		split_cache_wid = wid;
		split_cache = tiary::split_line (wid, get_text ());
	}
	return split_cache;
}

void UIString::output (Control &ctrl, Size pos, Size size) const
{
	unsigned wid = size.x;
	unsigned height = size.y;
	const SplitStringLineList &lst = split_line (wid);
	const SplitStringLine *ptr = &lst[0];

	for (unsigned n=minU(height,lst.size()); n; --n) {
		if (get_hotkey_pos () - ptr->begin < ptr->len) {
			Size next = ctrl.put (pos, get_text().data()+ptr->begin, get_hotkey_pos () - ptr->begin);
			ctrl.attribute_toggle (UNDERLINE);
			next = ctrl.put (next, get_text().data()[get_hotkey_pos ()]);
			ctrl.attribute_toggle (UNDERLINE);
			next = ctrl.put (next, get_text().data()+get_hotkey_pos()+1, ptr->len - (get_hotkey_pos () - ptr->begin + 1));
		}
		else {
			ctrl.put (pos, get_text().data()+ptr->begin, ptr->len);
		}
		pos.y++;
		++ptr;
	}
}

void UIString::update ()
{
	unsigned lines = 0;
	unsigned max_width = 0;
	unsigned cur_line_width = 0;
	const wchar_t *cls = get_text ().c_str (); // Current line starting point
	const wchar_t *s;
	for (s = cls; *s; ++s) {
		if (*s == L'\n') {
			++lines;
			max_width = maxU (max_width, cur_line_width);
			cur_line_width = 0;
			cls = s + 1;
		}
		else {
			cur_line_width += ucs_width (*s);
		}
	}
	if (s != cls) {
		++lines;
		max_width = maxU (max_width, cur_line_width);
	}

	this->lines = lines;
	this->max_width = max_width;

	split_cache_wid = 0;
	split_cache.clear ();
}



} // namespace tiary::ui
} // namespace ui
