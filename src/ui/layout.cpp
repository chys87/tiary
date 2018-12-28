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


#include "ui/layout.h"
#include "common/algorithm.h"
#include <algorithm>
#include <memory>
#include <functional>

namespace tiary {
namespace ui {


Layout::Layout (Direction dir)
	: lst ()
	, min_sum (0)
	, mid_sum (0)
	, max_sum (0)
	, direction (dir)
{
}

Layout::~Layout ()
{
}

void Layout::add(Item item) {
	min_sum += item.min;
	if (mid_sum != UNLIMITED) {
		if (item.obj == nullptr) {
			mid_sum += item.min;
		}
		else if (item.max == UNLIMITED) {
			mid_sum = UNLIMITED;
		}
		else {
			mid_sum += item.max;
		}
	}
	if (max_sum != UNLIMITED) {
		if (item.max == UNLIMITED) {
			max_sum = UNLIMITED;
		}
		else {
			max_sum += item.max;
		}
	}
	lst.push_back(item);
}

namespace {

Size combine_size (unsigned this_size, unsigned other_size, Direction direction)
{
	Size ret;
	if (direction == VERTICAL) {
		ret.y = this_size;
		ret.x = other_size;
	}
	else {
		ret.x = this_size;
		ret.y = other_size;
	}
	return ret;
}

Size move_resize_one (Layout::Item &item, Size pos, unsigned this_size, unsigned other_size, Direction direction)
{
	if (item.obj) {
		Size obj_pos = pos;
		if (other_size > item.other) {
			if (item.align_other >= 0) {
				unsigned left_space = other_size - item.other;
				if (item.align_other == 0) {
					left_space /= 2;
				}
				if (direction == VERTICAL) {
					obj_pos.x += left_space;
				}
				else {
					obj_pos.y += left_space;
				}
			}
			other_size = item.other;
		}
		Size obj_size = combine_size (this_size, other_size, direction);
		item.obj->move_resize (obj_pos, obj_size);
	}
	if (direction == VERTICAL) {
		pos.y += this_size;
	}
	else {
		pos.x += this_size;
	}
	return pos;
}

/**
 * If it is a real object, return the if_ctrl member.
 * If it is a spacer, return the if_spacer member.
 */
template <uint16_t Layout::Item::*if_ctrl, uint16_t Layout::Item::*if_spacer>
struct SelectMinMax {
	unsigned operator () (const Layout::Item &item) const
	{
		if (item.obj) { // Real object
			return item.*if_ctrl;
		}
		else {
			return item.*if_spacer;
		}
	}
};

} // anonymous namespace

void Layout::move_resize (Size pos, Size size)
{
	set_pos(pos);
	set_size(size);

	unsigned total_this, total_other;
	if (direction == VERTICAL) {
		total_this = size.y;
		total_other = size.x;
	}
	else {
		total_this = size.x;
		total_other = size.y;
	}
	unsigned n = lst.size ();
	
	if (total_this <= min_sum) {
		// Ooops! We don't have enough space. (if total_this < min_sum)
		// But we have to work!
		for (Item &item: lst) {
			pos = move_resize_one(item, pos, item.min, total_other, direction);
		}
	}
	else if (total_this <= max_sum) {
		// Everything is between [min, max]
		std::unique_ptr<unsigned[]> min{new unsigned [n * 3]};
		unsigned *max;
		unsigned *result;
		if (total_this <= mid_sum) {
			// All controls have size between min and max
			// All spacers have min size
			max = std::transform(lst.begin(), lst.end(), min.get(), std::mem_fn(&Item::min));
			result = std::transform (lst.begin (), lst.end (), max, SelectMinMax<&Item::max, &Item::min>());
		}
		else {
			// All controls have max size
			// All spacers have size between min and max
			max = std::transform(lst.begin(), lst.end(), min.get(), SelectMinMax<&Item::max, &Item::min>());
			result = std::transform(lst.begin(), lst.end(), max, std::mem_fn(&Item::max));
		}
		min_max_programming(result, min.get(), max, n, total_this);
		for (Item &item: lst) {
			pos = move_resize_one(item, pos, *result++, total_other, direction);
		}
	}
	else {
		// All controls and spacers have max size,
		// Center alignment
		if (direction == VERTICAL) {
			pos.y += (total_this - max_sum) / 2;
		}
		else {
			pos.x += (total_this - max_sum) / 2;
		}

		for (Item &item: lst) {
			pos = move_resize_one(item, pos, item.max, total_other, direction);
		}
	}
}

} // namespace tiary::ui
} // namespace tiary
