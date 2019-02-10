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

	RichText(Window &, const MultiLineRichText &mrt);
	RichText(Window &, MultiLineRichText &&mrt);
	~RichText ();

	void redraw ();
	bool on_mouse (MouseEvent); // For scroll bar only
	bool on_key (wchar_t);

	const MultiLineRichText &get_mrt() const { return mrt_; }

	void slot_search (bool backward);
	void slot_search_continue (bool previous);

private:
	void do_search (bool previous, bool include_current);

private:
	const MultiLineRichText mrt_;
	unsigned top_line_ = 0;

	// first = starting offset of highlight spots
	// second = length of highlight spots
	std::map<size_t, size_t> highlight_list_;

	SearchInfo search_info_;
};


} // namespace tiary::ui
} // namespace tiary

#endif // include guard
