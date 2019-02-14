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

#ifndef TIARY_UI_SCROLLBAR_H
#define TIARY_UI_SCROLLBAR_H

namespace tiary {
namespace ui {

struct ScrollBarInfo
{
	unsigned pos;
	unsigned size;
};

/**
 * @param	height	Screen height
 * @param	num	Number of items
 * @param	pagestart	The first visible item
 * @return	The position and size of the scroll bar
 */
ScrollBarInfo scrollbar_info (unsigned height, unsigned num, unsigned pagestart);

/**
 * Determines the new pagestart when the specific position of the scroll bar is clicked
 */
unsigned scrollbar_click (unsigned height, unsigned num, unsigned click_pos);

} // namespace tiary::ui
} // namespace tiary

#endif // include guard
