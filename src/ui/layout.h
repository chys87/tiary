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


#ifndef TIARY_UI_LAYOUT_H
#define TIARY_UI_LAYOUT_H

#include "ui/direction.h"
#include "ui/movable_object.h"
#include <stdint.h>
#include <vector>

/**
 * @file	ui/layout.h
 * @author	chys <admin@chys.info>
 * @brief	Header for tiary::ui::LayoutVertical and tiary::ui::LayoutHorizontal
 *
 * tiary::ui::LayoutVertical and tiary::ui::LayoutHorizontal are only a little
 * similar to QLayout in Qt.
 *
 * You must provide the total size, and Layout arrange the controls it contains
 * (calling the corresponding move_resize functions.)
 * And also, "Layout" classes here are purely helper classes (different from Qt).
 * You never have to use them.
 */

namespace tiary {
namespace ui {

struct Size;


class Layout : public MovableObject
{
public:
	static const uint16_t UNLIMITED = uint16_t(-1);

	struct Item
	{
		MovableObject *obj;
		uint16_t min, max;
		uint16_t other;  // Max width on the other direction (UNLIMITED = full)
		int16_t align_other; // Alignment on the other direction: -1/0/1 = left/center/right

		constexpr Item(MovableObject *o, unsigned m, unsigned M, unsigned ot = UNLIMITED, int ao = -1)
			: obj(o), min(m), max(M), other(ot), align_other(ao) {
		}
		constexpr Item(MovableObject &o, unsigned m, unsigned M, unsigned ot = UNLIMITED, int ao = -1)
			: obj(&o), min(m), max(M), other(ot), align_other(ao) {
		}
		constexpr Item(unsigned m, unsigned M) // Spacer
			: Item(nullptr, m, M) {
		}
	};

	explicit Layout (Direction);
	~Layout ();

	void add(Item item);
	void add(std::initializer_list<Item> item_il) {
		for (const Item &item: item_il) {
			add(item);
		}
	}

	void move_resize (Size pos, Size size);

private:
	std::vector<Item> lst;
	unsigned min_sum; ///< Sum of all "min"s of controls
	unsigned mid_sum; ///< Controls counted as max, spacing counted as min
	unsigned max_sum; ///< Sum of all "max"s of controls. If any one of them is UNLIMITED, this is UNLIMITED
	Direction direction;
};

} // namespace tiary::ui
} // namespace tiary

#endif // Include guard
