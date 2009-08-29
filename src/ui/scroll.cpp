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


#include "ui/scroll.h"
#include "common/algorithm.h"
#include <assert.h>

namespace tiary {
namespace ui {


Scroll::Scroll (unsigned height_, bool allow_focus_end_)
	: height (maxU (1, height_))
	, number (0)
	, first (0)
	, len (0)
	, focus (0)
	, allow_focus_end (allow_focus_end_)
	, accumulate_height (2, 0)
{
	accumulate_height[1] = 1;
	recalculate_len ();
}

Scroll::~Scroll ()
{
}


Scroll::Info Scroll::get_info () const
{
	Info ret;
	ret.first = first;
	ret.len = len;
	ret.focus = focus;
	ret.focus_pos = accumulate_height[focus] - accumulate_height[first];
	return ret;
}

unsigned Scroll::get_item_screen_size (unsigned) const
{
	return 1;
}

void Scroll::modify_focus (unsigned new_focus)
{
	if (new_focus > max_possible_focus ())
		return;
	focus = new_focus;
	if (new_focus - first >= len) {
		if (new_focus < first) { // Scroll backward (upward)
			first = new_focus;
		} else { // Scroll forward (downward)
			// Let's put the focus in the last line.
			put_focus_last_line ();
		}
	}
	// Always call recalculate_len
	// modify_focus is called by other member functions of Scroll
	recalculate_len ();
}

void Scroll::modify_focus_pos (unsigned new_focus_pos)
{
	if (new_focus_pos >= height)
		return;
	// Find the maximal possible j, s.t.
	// first <= k <= max_possible_focus ()
	// accumulate_height[k] <= accumulate_height[first] + new_focus_pos
	unsigned k = first;
	unsigned max = max_possible_focus ();
	unsigned limit = accumulate_height[first] + new_focus_pos;
	while (k<max && accumulate_height[k+1]<=limit)
		++k;
	focus = k;
}

void Scroll::modify_height (unsigned new_height)
{
	if (new_height == 0)
		return;
	height = new_height;
	if (accumulate_height[focus+1] > accumulate_height[first] + new_height) {
		// We need to scroll forward (downward)
		// Let's put the focus in the last line.
		put_focus_last_line ();
	}
	recalculate_len ();
}

void Scroll::modify_number (unsigned new_number)
{
	number = new_number;
	recalculate_accumulate_height ();
	unsigned max_focus = max_possible_focus ();
	if (first >= number) {
		focus = max_focus;
		put_focus_last_line ();
	} else if (focus > max_possible_focus ()) {
		focus = max_possible_focus ();
		assert (focus >= first);
	}
	recalculate_len ();
}

void Scroll::scroll_focus_to_first ()
{
	first = focus;
	recalculate_len ();
}

void Scroll::modify_number_delete ()
{
	if (focus >= number)
		return; // Nothing to delete

	// Only in one situation will the focus be moved
	unsigned old_wid = accumulate_height[focus+1] - accumulate_height[focus];
	for (unsigned i=focus; i<number; ++i)
		accumulate_height[i+1] = accumulate_height[i+2] - old_wid;
	--number;
	accumulate_height.resize (number + 2);
	if (!allow_focus_end && number && focus>=number) {
		focus = number - 1;
		put_focus_last_line ();
	}
	recalculate_len ();
}

void Scroll::modify_number_backspace ()
{
	if (focus == 0)
		return; // Nothing to remove
	modify_focus (focus - 1);
	modify_number_delete ();
}

void Scroll::modify_number_insert ()
{
	++number;
	accumulate_height.resize (number + 2);
	unsigned add_wid = get_item_screen_size (focus);
	unsigned focus_next = focus + 1;
	for (unsigned i=number+1; i>=focus_next; --i)
		accumulate_height[i] = accumulate_height[i-1] + add_wid;
	modify_focus (focus_next);
}

void Scroll::recalculate_accumulate_height ()
{
	accumulate_height.resize (number + 2);
	accumulate_height[0] = 0;
	unsigned acc = 0;
	for (unsigned i=0; i<number; ++i)
		accumulate_height[i+1] = acc += get_item_screen_size(i);
	accumulate_height[number+1] = acc += 1;
}

unsigned Scroll::recalculate_len ()
{
	// Find the maximum possible len, such that
	// accumulate_height[first+len] <= accumulate_height[first] + height
	unsigned j = first;
	unsigned max = accumulate_height[first] + height;
	while (j<number && accumulate_height[j+1]<=max)
		++j;
	return (len = j-first);
}

unsigned Scroll::max_possible_focus () const
{
	if (allow_focus_end || number == 0)
		return number;
	else
		return (number - 1);
}

void Scroll::put_focus_last_line ()
{
	// Find the minimal possible first, such that
	// accumulate_height[focus+1] <= accumulate_height[first] + height
	unsigned j = focus;
	unsigned tmp = accumulate_height[focus+1];
	while (j && tmp<=accumulate_height[j-1]+height)
		--j;
	first = j;
}

} // namespace tiary::ui
} // namespace tiary
