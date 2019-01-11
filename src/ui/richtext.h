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


#ifndef TIARY_UI_RICHTEXT_H
#define TIARY_UI_RICHTEXT_H

/**
 * @file	ui/richtext.h
 * @author	chys <admin@chys.info>
 * @brief	Defines the tiary::ui::RichText class
 */


#include "ui/control.h"
#include "ui/richtextlist.h"
#include "ui/search_info.h"
#include <string>
#include <string_view>
#include <vector>

namespace tiary {
namespace ui {

/**
 * @brief	A control for displaying "RichText"
 *
 * This control displays "rich text", i.e., allowing setting
 * a different PaletteID for each line.
 *
 * No automatic line wrap.
 *
 * The contents cannot be modified
 */
class RichText final : public Control {
public:

	typedef RichTextLine Line;
	typedef RichTextLineList LineList;

	RichText(Window &, std::wstring_view, const LineList &);
	RichText(Window &, std::wstring_view, LineList &&);
	~RichText ();

	void redraw ();
	bool on_mouse (MouseEvent); // For scroll bar only
	bool on_key (wchar_t);

	const LineList &get_list () const { return line_list; }

	void slot_search (bool backward);
	void slot_search_continue (bool previous);

private:

	const std::wstring text;
	const LineList line_list;
	unsigned top_line;

	// first = starting offset of highlight spots
	// second = length of highlight spots
	typedef std::map <size_t, size_t> HighlightList;
	HighlightList highlight_list;

	SearchInfo search_info;

	void do_search (bool previous, bool include_current);
};


} // namespace tiary::ui
} // namespace tiary

#endif // include guard
