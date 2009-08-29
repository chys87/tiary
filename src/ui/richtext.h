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


#ifndef TIARY_UI_RICHTEXT_H
#define TIARY_UI_RICHTEXT_H

/**
 * @file	ui/richtext.h
 * @author	chys <admin@chys.info>
 * @brief	Defines the tiary::ui::RichText class
 */


#include "ui/control.h"
#include "ui/richtextlist.h"
#include <vector>

namespace tiary {
namespace ui {

/**
 * @brief	A control for displaying "RichText"
 *
 * This control displays "rich text", i.e., allowing .
 *
 * No automatic line wrap.
 *
 * The contents cannot be modified
 */
class RichText : public Control
{
public:

	typedef RichTextLine Line;
	typedef RichTextList LineList;

	RichText (Window &, const LineList &);
#ifdef TIARY_HAVE_RVALUE_REFERENCES
	RichText (Window &, LineList &&);
#endif
	~RichText ();

	void redraw ();
	void on_move_resize (Size, Size);
	bool on_key (wchar_t);

	const LineList &get_list () const { return line_list; }

private:

	const LineList line_list;
	unsigned top_line;
};


} // namespace tiary::ui
} // namespace tiary

#endif // include guard
