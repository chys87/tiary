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


#include "ui/scroll.h"
#include "common/algorithm.h"
#include <assert.h>

namespace tiary {
namespace ui {

Scroll::Scroll(unsigned height, bool allow_focus_end, std::function<unsigned(unsigned)> get_item_screen_size)
	: get_item_screen_size_(std::move(get_item_screen_size))
	, accumulate_height_{0, 1}
	, height_(maxU(1, height))
	, allow_focus_end_(allow_focus_end)
{
	recalculate_len ();
}

Scroll::~Scroll() = default;

Scroll::Info Scroll::get_info() const {
	return {
		first_,
		len_,
		focus_,
		get_focus_pos(),
	};
}

void Scroll::modify_focus (unsigned new_focus)
{
	if (new_focus > max_possible_focus ()) {
		return;
	}
	focus_ = new_focus;
	if (new_focus - first_ >= len_) {
		if (new_focus < first_) { // Scroll backward (upward)
			first_ = new_focus;
		}
		else { // Scroll forward (downward)
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
	if (new_focus_pos >= height_) {
		return;
	}
	// Find the maximal possible j, s.t.
	// first_ <= k <= max_possible_focus ()
	// accumulate_height_[k] <= accumulate_height_[first_] + new_focus_pos
	unsigned k = first_;
	unsigned max = max_possible_focus ();
	unsigned limit = accumulate_height_[first_] + new_focus_pos;
	while (k < max && accumulate_height_[k + 1] <= limit) {
		++k;
	}
	focus_ = k;
}

void Scroll::modify_height (unsigned new_height)
{
	if (new_height == 0) {
		return;
	}
	unsigned old_height = height_;
	height_ = new_height;
	if (new_height < old_height) {
		// Became narrower.
		// We may need to scroll forward (downward)
		// to keep the selected item visible
		if (accumulate_height_[focus_ + 1] > accumulate_height_[first_] + new_height) {
			// Let's put the focus in the last line.
			put_focus_last_line ();
		}
	}
	else {
		// Became wider.
		// If this will lead to space in the end, scroll backward (upward)
		unsigned max = max_possible_focus ();
		if (accumulate_height_[max + 1] - accumulate_height_[first_] < new_height) {
			// Find the smallest possible first, such that
			// accumulate_height_[max + 1] <= accumulate_height_[first] + height_
			unsigned j = max;
			unsigned tmp = accumulate_height_[max + 1];
			while (j && tmp<=accumulate_height_[j - 1] + height_) {
				--j;
			}
			first_ = j;
		}
	}
	recalculate_len ();
}

void Scroll::modify_number (unsigned new_number)
{
	number_ = new_number;
	recalculate_accumulate_height ();
	unsigned max_focus = max_possible_focus ();
	if (first_ >= number_) {
		focus_ = max_focus;
		put_focus_last_line ();
	} else if (focus_ > max_focus) {
		focus_ = max_focus;
		assert(focus_ >= first_);
	}
	recalculate_len ();
}

void Scroll::scroll_focus_to_first() {
	first_ = focus_;
	recalculate_len ();
}

void Scroll::scroll_focus_to_last ()
{
	put_focus_last_line ();
	recalculate_len ();
}

void Scroll::modify_number_delete ()
{
	if (focus_ >= number_) {
		return; // Nothing to delete
	}

	// Only in one situation will the focus be moved
	unsigned old_wid = accumulate_height_[focus_ + 1] - accumulate_height_[focus_];
	for (unsigned i = focus_; i < number_; ++i) {
		accumulate_height_[i + 1] = accumulate_height_[i + 2] - old_wid;
	}
	--number_;
	accumulate_height_.resize(number_ + 2);
	if (!allow_focus_end_ && number_ && focus_ >= number_) {
		focus_ = number_ - 1;
		put_focus_last_line ();
	}
	recalculate_len ();
}

void Scroll::modify_number_backspace ()
{
	if (focus_ == 0) {
		return; // Nothing to remove
	}
	modify_focus(focus_ - 1);
	modify_number_delete ();
}

void Scroll::modify_number_insert ()
{
	++number_;
	accumulate_height_.resize(number_ + 2);
	unsigned add_wid = get_item_screen_size_ ? get_item_screen_size_(focus_) : 1;
	unsigned focus_next = focus_ + 1;
	for (unsigned i = number_ + 1; i >= focus_next; --i) {
		accumulate_height_[i] = accumulate_height_[i - 1] + add_wid;
	}
	modify_focus (focus_next);
}

void Scroll::recalculate_accumulate_height ()
{
	unsigned n = number_;
	accumulate_height_.resize(n + 2);
	accumulate_height_[0] = 0;
	unsigned acc = 0;
	for (unsigned i = 0; i < n; ++i) {
		accumulate_height_[i + 1] = acc += get_item_screen_size_ ? get_item_screen_size_(i) : 1;
	}
	accumulate_height_[n + 1] = acc += 1;
}

unsigned Scroll::recalculate_len ()
{
	// Find the maximum possible len_, such that
	// accumulate_height_[first_ + len_] <= accumulate_height_[first_] + height_
	unsigned j = first_;
	unsigned max = accumulate_height_[first_] + height_;
	while (j < number_ && accumulate_height_[j + 1] <= max) {
		++j;
	}
	return (len_ = j - first_);
}

unsigned Scroll::max_possible_focus() const {
	if (allow_focus_end_ || number_ == 0) {
		return number_;
	}
	else {
		return (number_ - 1);
	}
}

void Scroll::put_focus_last_line ()
{
	// Find the minimal possible first_, such that
	// accumulate_height_[focus_ + 1] <= accumulate_height_[first_] + height_
	unsigned j = focus_;
	unsigned tmp = accumulate_height_[focus_ + 1];
	while (j && tmp <= accumulate_height_[j - 1] + height_) {
		--j;
	}
	first_ = j;
}

} // namespace tiary::ui
} // namespace tiary
