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

#include "ui/uistring_one.h"
#include "ui/size.h"
#include "ui/control.h"
#include "ui/window.h"
#include "ui/output.h"
#include "common/unicode.h"
#include "common/split_line.h"

namespace tiary {
namespace ui {


UIStringOne::UIStringOne (const std::wstring &s, unsigned options)
	: UIStringBase (s, options)
	, width (ucs_width (get_text ()))
{
}

UIStringOne::UIStringOne(std::wstring &&s, unsigned options)
	: UIStringBase(std::move(s), options)
	, width(ucs_width(get_text()))
{
}

void UIStringOne::set_text (const std::wstring &s, unsigned options)
{
	UIStringBase::set_text (s, options);
	width = ucs_width (get_text ());
}

void UIStringOne::set_text(std::wstring &&s, unsigned options) {
	UIStringBase::set_text(std::move(s), options);
	width = ucs_width(get_text());
}

Size UIStringOne::output (Control &ctrl, Size pos, unsigned wid) const
{
	size_t end = get_text ().length ();
	if (wid < width) {
		SplitStringLine oneline;
		split_line (oneline, wid, get_text ());
		end = oneline.len;
	}
	if (get_hotkey_pos () < end) {
		pos = ctrl << pos
			<< std::wstring_view{get_text().data(), get_hotkey_pos()}
			<< toggle (UNDERLINE)
			<< get_text().data()[get_hotkey_pos()]
			<< toggle (UNDERLINE)
			<< std::wstring_view{get_text().data()+get_hotkey_pos() + 1, end - get_hotkey_pos () - 1};
	} else {
		pos = ctrl << pos << std::wstring_view{get_text().data(), end};
	}
	return pos;
}

Size UIStringOne::output (Window &win, Size pos, unsigned wid) const
{
	size_t end = get_text ().length ();
	if (wid < width) {
		SplitStringLine oneline;
		split_line (oneline, wid, get_text ());
		end = oneline.len;
	}
	if (get_hotkey_pos () < end) {
		pos = win << pos
			<< std::wstring_view{get_text().data(), get_hotkey_pos()}
			<< toggle (UNDERLINE)
			<< get_text().data()[get_hotkey_pos()]
			<< toggle (UNDERLINE)
			<< std::wstring_view{get_text().data() + get_hotkey_pos() + 1, end - get_hotkey_pos () - 1};
	} else {
		pos = win << pos << std::wstring_view{get_text().data(), end};
	}
	return pos;
}


} // namespace tiary::ui
} // namespace ui
