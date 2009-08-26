// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the so-called 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#ifndef TIARY_UI_SCROLL_H
#define TIARY_UI_SCROLL_H

/**
 * @file	ui/scroll.h
 * @author	chys <admin@chys.info>
 * @brief	Declares the class tiary::ui::Scroll
 */

#include <vector>

namespace tiary {
namespace ui {

/**
 * @brief	Handles scrolling
 *
 * This class can be used by any UI component that supports
 * scrolling (textbox, list, etc.)
 *
 * Example: We have a list control, displaying N items
 * in a box of height H. If N>H, only part of the items
 * can be displayed at any given time.
 * We need to determine [A,B) and C, respectively
 * the interval of items actually shown on screen,
 * and the cursor position.
 *
 * The height (width) of each item can be different
 * (e.g. TextBox can have half- and full-width characters mixed)
 */
class Scroll
{
public:
	struct Info
	{
		unsigned first;    ///< First item on screen
		unsigned len;      ///< Number of items on screen
		unsigned focus;    ///< Focus (cursor) position (item ID)
		unsigned focus_pos;///< Focus (cursor) screen position
	};

	/*
	 * @brief	Constructor
	 * @param	allow_focus_end
	 * Do we allow focusing a phantom "end" place?
	 * Allowed in e.g. TextBox.
	 * (Always allowed if number == 0; of coz)
	 *
	 * number_ is always initialized to zero:
	 * If not, we must call virtual function get_item_screen_size
	 * to initialize accumulate_height, which is not safe in the
	 * constructor.
	 */
	Scroll (unsigned height_, bool allow_focus_end_);
	virtual ~Scroll ();

	Info get_info () const;

	/// Get the screen height (width) for a specified item. Default: All ones
	virtual unsigned get_item_screen_size (unsigned) const;

	void modify_focus (unsigned);     // Change cursor position
	void modify_focus_pos (unsigned); // Change cursor position by screen coordinate
	void modify_height (unsigned);    // Change screen height. Caller's responsibility to make sure height >= max possible single item height
	void modify_number (unsigned);    // Change the total number of items. accumulate_height is recalculated
	// Three special cases: accumulate_height is not totally recalculated.
	// Like pressing the Delete key
	void modify_number_delete ();
	// Like pressing the Backspace key
	void modify_number_backspace ();
	// Like inserting a character (This calls get_item_screen_size,
	// so make sure it returns the correct new value before calling me)
	void modify_number_insert ();

	// Alias (for left-right scrolling)
	void modify_width (unsigned wid) { modify_height (wid); }

	// Convenience functions
	unsigned get_height () const { return height; }
	unsigned get_width () const { return height; }
	unsigned get_first () const { return first; }
	unsigned get_number () const { return number; }
	unsigned get_focus () const { return focus; }

private:
	unsigned height; // Screen height
	unsigned number; // Number of items
	unsigned first;  // First item that should be shown
	unsigned len;    // Number of items on screen
	unsigned focus;  // Current focus position (absolute)

	bool allow_focus_end;

	// accumulate_height[i+1] = sum height(j), j=0..i
	// Another 1-size thing is appended to represent the end point
	std::vector<unsigned> accumulate_height;

	void recalculate_accumulate_height ();
	unsigned recalculate_len ();
	unsigned max_possible_focus () const;
	void put_focus_last_line (); // Updates first only. Does not call recalculate_len.

};

} // namespace tiary::ui
} // namespace tiary

#endif // include guard
