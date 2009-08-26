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


#include "ui/layout.h"
#include "ui/control.h"
#include "common/algorithm.h"
#include <algorithm>

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

void Layout::add (Object *obj, unsigned min, unsigned max, unsigned other, int align_other)
{
	min_sum += min;
	if (mid_sum != UNLIMITED) {
		if (obj == 0)
			mid_sum += min;
		else if (max == UNLIMITED)
			mid_sum = UNLIMITED;
		else
			mid_sum += max;
	}
	if (max_sum != UNLIMITED) {
		if (max == UNLIMITED)
			max_sum = UNLIMITED;
		else
			max_sum += max;
	}
	Item item;
	item.obj = obj;
	item.min = min;
	item.max = max;
	item.other = other;
	item.align_other = align_other;
	lst.push_back (item);
}

namespace {

Size combine_size (unsigned this_size, unsigned other_size, Direction direction)
{
	Size ret;
	if (direction == VERTICAL) {
		ret.y = this_size;
		ret.x = other_size;
	} else {
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
				if (item.align_other == 0)
					left_space /= 2;
				if (direction == VERTICAL)
					obj_pos.x += left_space;
				else
					obj_pos.y += left_space;
			}
			other_size = item.other;
		}
		Size obj_size = combine_size (this_size, other_size, direction);
		if (Control *ctrl = dynamic_cast <Control *> (item.obj))
			ctrl->move_resize (obj_pos, obj_size);
		else
			static_cast<Layout *>(item.obj)->move_resize (obj_pos, obj_size);
	}
	if (direction == VERTICAL)
		pos.y += this_size;
	else
		pos.x += this_size;
	return pos;
}

/**
 * If it is a real object, return the if_ctrl member.
 * If it is a spacer, return the if_spacer member.
 */
template <unsigned Layout::Item::*if_ctrl, unsigned Layout::Item::*if_spacer>
struct SelectMinMax : public std::unary_function <const Layout::Item &, unsigned>
{
	unsigned operator () (const Layout::Item &item) const
	{
		if (item.obj) // Real object
			return item.*if_ctrl;
		else
			return item.*if_spacer;
	}
};

} // anonymous namespace

void Layout::move_resize (Size pos, Size size)
{
	unsigned total_this, total_other;
	if (direction == VERTICAL) {
		total_this = size.y;
		total_other = size.x;
	} else {
		total_this = size.x;
		total_other = size.y;
	}
	unsigned n = lst.size ();
	
	if (total_this <= min_sum) {
		// Ooops! We don't have enough space. (if total_this < min_sum)
		// But we have to work!
		for (ItemList::iterator it = lst.begin (); it != lst.end (); ++it)
			pos = move_resize_one (*it, pos, it->min, total_other, direction);
	} else if (total_this <= max_sum) {
		// Everything is between [min, max]
		unsigned *min = new unsigned [n * 3];
		unsigned *max;
		unsigned *result;
		if (total_this <= mid_sum) {
			// All controls have size between min and max
			// All spacers have min size
			max = std::transform (lst.begin (), lst.end (), min, get_member_fun (&Item::min));
			result = std::transform (lst.begin (), lst.end (), max, SelectMinMax<&Item::max, &Item::min>());
		} else {
			// All controls have max size
			// All spacers have size between min and max
			max = std::transform (lst.begin (), lst.end (), min, SelectMinMax<&Item::max, &Item::min>());
			result = std::transform (lst.begin (), lst.end (), max, get_member_fun (&Item::max));
		}
		min_max_programming (result, min, max, n, total_this);
		for (ItemList::iterator it = lst.begin (); it != lst.end (); ++it)
			pos = move_resize_one (*it, pos, *result++, total_other, direction);
		delete [] min;
	} else {
		// All controls and spacers have max size,
		// Center alignment
		if (direction == VERTICAL)
			pos.y += (total_this - max_sum) / 2;
		else
			pos.x += (total_this - max_sum) / 2;

		for (ItemList::iterator it = lst.begin (); it != lst.end (); ++it)
			pos = move_resize_one (*it, pos, it->max, total_other, direction);
	}
}

} // namespace tiary::ui
} // namespace tiary
