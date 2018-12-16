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
	static const unsigned UNLIMITED = unsigned(-1);

	struct Item
	{
		MovableObject *obj;
		unsigned min, max;
		unsigned other;  // Max width on the other direction (UNLIMITED = full)
		int align_other; // Alignment on the other direction: -1/0/1 = left/center/right
	};

	explicit Layout (Direction);
	~Layout ();

private:
	void add_impl (MovableObject *obj, unsigned min, unsigned max, unsigned other, int align_other);

public:

	class Adder {
	public:
		explicit constexpr Adder(Layout *p) : p_(p) {}
		Adder(const Adder &other) = default;
		Adder &operator = (const Adder &other) = default;

		Adder &operator () (MovableObject *ctrl, unsigned min, unsigned max, unsigned other = UNLIMITED, int align_other = -1) {
			p_->add_impl (ctrl, min, max, other, align_other);
			return *this;
		}
		Adder &operator () (MovableObject &ctrl, unsigned min, unsigned max, unsigned other = UNLIMITED, int align_other = -1) {
			p_->add_impl (&ctrl, min, max, other, align_other);
			return *this;
		}
		/** Add spacing */
		Adder &operator () (unsigned min, unsigned max) {
			p_->add_impl (0, min, max, UNLIMITED, -1);
			return *this;
		}
	private:
		Layout *p_;
	};

	Adder add ()
	{
		return Adder (this);
	}
	Adder add (MovableObject *ctrl, unsigned min, unsigned max, unsigned other = UNLIMITED, int align_other = -1)
	{
		add_impl (ctrl, min, max, other, align_other);
		return Adder (this);
	}
	Adder add (MovableObject &ctrl, unsigned min, unsigned max, unsigned other = UNLIMITED, int align_other = -1)
	{
		add_impl (&ctrl, min, max, other, align_other);
		return Adder (this);
	}
	/** Add spacing */
	Adder add (unsigned min, unsigned max)
	{
		add_impl (0, min, max, UNLIMITED, -1);
		return Adder (this);
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
