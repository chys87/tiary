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


#ifndef TIARY_UI_SCROLL_H
#define TIARY_UI_SCROLL_H

/**
 * @file	ui/scroll.h
 * @author	chys <admin@chys.info>
 * @brief	Declares the class tiary::ui::Scroll
 */

#include <functional>
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

	/**
	 * @brief	Constructor
	 * @param	allow_focus_end
	 * Do we allow focusing a phantom "end" place?
	 * Allowed in e.g. TextBox.
	 * (Always allowed if number == 0; of coz)
	 * @param	get_item_screen_size Returns the screen height (width) for a specified item.  If omitted, always returns 1.
	 *
	 * number_ is always initialized to zero:
	 * If not, we must call function get_item_screen_size
	 * to initialize accumulate_height_, which is probably not safe in the
	 * constructor, assuming the callback may invoke some virtual function.
	 */
	Scroll(unsigned height, bool allow_focus_end, std::function<unsigned(unsigned)> get_item_screen_size = nullptr);
	~Scroll();

	Info get_info () const;

	void modify_focus (unsigned);     // Change cursor position
	void modify_focus_pos (unsigned); // Change cursor position by screen coordinate
	void modify_height (unsigned);    // Change screen height. Caller's responsibility to make sure height >= max possible single item height
	void modify_number (unsigned);    // Change the total number of items. accumulate_height_ is recalculated
	void scroll_focus_to_first ();    // Scroll to make the focus the first one
	void scroll_focus_to_last ();     // Scroll to make the focus the last one on screen
	// Three special cases: accumulate_height_ is not fully recalculated.
	// Like pressing the Delete key
	void modify_number_delete ();
	// Like pressing the Backspace key
	void modify_number_backspace ();
	// Like inserting a character (This calls get_item_screen_size_,
	// so make sure it returns the correct new value before calling me)
	void modify_number_insert ();

	// Alias (for left-right scrolling)
	void modify_width (unsigned wid) { modify_height (wid); }

	// Convenience functions
	unsigned get_height() const { return height_; }
	unsigned get_width() const { return height_; }
	unsigned get_first() const { return first_; }
	unsigned get_number() const { return number_; }
	unsigned get_focus() const { return focus_; }
	unsigned get_focus_pos() const { return accumulate_height_[focus_] - accumulate_height_[first_]; }
	unsigned get_len() const { return len_; }

private:
	void recalculate_accumulate_height ();
	unsigned recalculate_len ();
	unsigned max_possible_focus () const;
	void put_focus_last_line (); // Updates first only. Does not call recalculate_len.

private:
	std::function<unsigned(unsigned)> get_item_screen_size_;

	// accumulate_height_[i+1] = sum height(j), j=0..i
	// Another 1-size thing is appended to represent the end point
	std::vector<unsigned> accumulate_height_;

	unsigned height_ = 1; // Screen height
	unsigned number_ = 0; // Number of items
	unsigned first_ = 0;  // First item that should be shown
	unsigned len_ = 0;    // Number of items on screen
	unsigned focus_ = 0;  // Current focus position (absolute)

	bool allow_focus_end_;
};

} // namespace tiary::ui
} // namespace tiary

#endif // include guard
