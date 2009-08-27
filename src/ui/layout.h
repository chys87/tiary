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

#include "ui/object.h"
#include "ui/direction.h"
#include <list>

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

class Control;
struct Size;


class Layout : public Object // It's not a real "object", but this will simplify our implementation
{
public:
	static const unsigned UNLIMITED = unsigned(-1);

	struct Item
	{
		Object *obj;     // Must be Control or Layout<true> or Layout<false>
		unsigned min, max;
		unsigned other;  // Max width on the other direction (UNLIMITED = full)
		int align_other; // Alignment on the other direction: -1/0/1 = left/center/right
	};

	explicit Layout (Direction);
	~Layout ();

private:
	void add (Object *obj, unsigned min, unsigned max, unsigned other, int align_other);

public:

	class Adder {
	public:
		explicit Adder (Layout *p_) : p (p_) {}
		Adder (const Adder &other) : p (other.p) {}
		Adder & operator = (const Adder &other) { p = other.p; return *this; }

		Adder operator () (Control *ctrl, unsigned min, unsigned max, unsigned other = UNLIMITED, int align_other = -1)
		{
			p->add ((Object *)ctrl, min, max, other, align_other);
			return *this;
		}
		Adder operator () (Control &ctrl, unsigned min, unsigned max, unsigned other = UNLIMITED, int align_other = -1)
		{
			p->add ((Object *)&ctrl, min, max, other, align_other);
			return *this;
		}
		Adder operator () (Layout *layout, unsigned min, unsigned max, unsigned other = UNLIMITED, int align_other = -1)
		{
			p->add ((Object *)layout, min, max, other, align_other);
			return *this;
		}
		Adder operator () (Layout &layout, unsigned min, unsigned max, unsigned other = UNLIMITED, int align_other = -1)
		{
			p->add ((Object *)&layout, min, max, other, align_other);
			return *this;
		}
		/** Add spacing */
		Adder operator () (unsigned min, unsigned max)
		{
			p->add ((Object *)0, min, max, UNLIMITED, -1);
			return *this;
		}
	private:
		Layout *p;
	};

	Adder add ()
	{
		return Adder (this);
	}
	Adder add (Control *ctrl, unsigned min, unsigned max, unsigned other = UNLIMITED, int align_other = -1)
	{
		add ((Object *)ctrl, min, max, other, align_other);
		return Adder (this);
	}
	Adder add (Control &ctrl, unsigned min, unsigned max, unsigned other = UNLIMITED, int align_other = -1)
	{
		add ((Object *)&ctrl, min, max, other, align_other);
		return Adder (this);
	}
	Adder add (Layout *layout, unsigned min, unsigned max, unsigned other = UNLIMITED, int align_other = -1)
	{
		add ((Object *)layout, min, max, other, align_other);
		return Adder (this);
	}
	Adder add (Layout &layout, unsigned min, unsigned max, unsigned other = UNLIMITED, int align_other = -1)
	{
		add ((Object *)&layout, min, max, other, align_other);
		return Adder (this);
	}
	/** Add spacing */
	Adder add (unsigned min, unsigned max)
	{
		add ((Object *)0, min, max, UNLIMITED, -1);
		return Adder (this);
	}

	void move_resize (Size pos, Size size);

private:

	typedef std::list <Item> ItemList;
	ItemList lst;
	unsigned min_sum; ///< Sum of all "min"s of controls
	unsigned mid_sum; ///< Controls counted as max, spacing counted as min
	unsigned max_sum; ///< Sum of all "max"s of controls. If any one of them is UNLIMITED, this is UNLIMITED
	Direction direction;
};

} // namespace tiary::ui
} // namespace tiary

#endif // Include guard
