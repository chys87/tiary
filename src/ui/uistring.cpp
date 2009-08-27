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
#include "ui/control.h"
#include "common/unicode.h"
#include "common/algorithm.h"
#include "ui/window.h"

namespace tiary {
namespace ui {

UIStringBase::UIStringBase (const std::wstring &s, unsigned options)
	: text (s)
	, hotkey_pos (size_t(-1))
{
	if (!(options & NO_HOTKEY))
		update ();
}

void UIStringBase::set_text (const std::wstring &s, unsigned options)
{
	text = s;
	hotkey_pos = size_t(-1);
	if (!(options & NO_HOTKEY))
		update ();
}

wchar_t UIStringBase::get_hotkey () const
{
	if (hotkey_pos < text.length ())
		return text[hotkey_pos];
	else
		return L'\0';
}

void UIStringBase::update ()
{
	hotkey_pos = size_t(-1);
	size_t found = 0;
	while (found<text.size() && (found = text.find (L'&', found)) != std::wstring::npos) {
		wchar_t c;
		if (++found<text.size() && iswprint (c = text.data()[found])) {
			text.erase (--found, 1);
			if (hotkey_pos==size_t(-1) && c!=L'&')
				hotkey_pos = found;
			++found;
		}
	}
}


UIStringOne::UIStringOne (const std::wstring &s, unsigned options)
	: UIStringBase (s, options)
	, width (0)
{
	update ();
}

void UIStringOne::set_text (const std::wstring &s, unsigned options)
{
	UIStringBase::set_text (s, options);
	update ();
}

Size UIStringOne::output (Control &ctrl, Size pos, unsigned wid)
{
	size_t end = get_text ().length ();
	if (wid < width) {
		SplitStringLine oneline;
		split_line (oneline, wid, get_text ());
		end = oneline.len;
	}
	if (get_hotkey_pos () < end) {
		pos = ctrl.put (pos, get_text().data(), get_hotkey_pos ());
		ColorAttr remember = ctrl.get_attr ();
		ctrl.attribute_on (UNDERLINE);
		pos = ctrl.put (pos, get_text().data()[get_hotkey_pos()]);
		ctrl.set_attr (remember);
		pos = ctrl.put (pos, get_text().data()+get_hotkey_pos()+1, end - get_hotkey_pos () - 1);
	} else
		pos = ctrl.put (pos, get_text().data (), end);
	return pos;
}

Size UIStringOne::output (Window &win, Size pos, unsigned wid)
{
	size_t end = get_text ().length ();
	if (wid < width) {
		SplitStringLine oneline;
		split_line (oneline, wid, get_text ());
		end = oneline.len;
	}
	if (get_hotkey_pos () < end) {
		pos = win.put (pos, get_text().data(), get_hotkey_pos ());
		ColorAttr remember = win.get_attr ();
		win.attribute_on (UNDERLINE);
		pos = win.put (pos, get_text().data()[get_hotkey_pos()]);
		win.set_attr (remember);
		pos = win.put (pos, get_text().data()+get_hotkey_pos()+1, end - get_hotkey_pos () - 1);
	} else
		pos = win.put (pos, get_text().data (), end);
	return pos;
}

void UIStringOne::update ()
{
	width = ucs_width (get_text ());
}

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

const SplitStringLineList &UIString::split_line (unsigned wid)
{
	if (wid != split_cache_wid) {
		split_cache_wid = wid;
		tiary::split_line (wid, get_text ()). swap (split_cache);
	}
	return split_cache;
}

void UIString::output (Control &ctrl, Size pos, Size size)
{
	unsigned wid = size.x;
	unsigned height = size.y;
	const SplitStringLineList &lst = split_line (wid);

	for (unsigned i=0, n=minU(height,lst.size()); i<n; ++i) {
		if (get_hotkey_pos () - lst[i].begin < lst[i].len) {
			Size next = ctrl.put (pos, get_text().data()+lst[i].begin, get_hotkey_pos () - lst[i].begin);
			ColorAttr remember = ctrl.get_attr ();
			ctrl.attribute_on (UNDERLINE);
			next = ctrl.put (next, get_text().data()[get_hotkey_pos ()]);
			ctrl.set_attr (remember);
			next = ctrl.put (next, get_text().data()+get_hotkey_pos()+1, lst[i].len - (get_hotkey_pos () - lst[i].begin + 1));
		} else
			ctrl.put (pos, get_text().data()+lst[i].begin, lst[i].len);
		pos.y++;
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
		} else {
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
