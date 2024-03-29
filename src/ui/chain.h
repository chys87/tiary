// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009-2023, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#ifndef TIARY_UI_CHAIN_H
#define TIARY_UI_CHAIN_H

/**
 * @file	ui/chain.h
 * @author	chys <admin@chys.info>
 * @brief	Defines class tiary::ui::ChainControls
 */

#include "ui/control.h"
#include "ui/direction.h"
#include <initializer_list>

namespace tiary {
namespace ui {
namespace chain_detail {

/**
 * @brief	Helps "chaining" controls
 *
 * "Chaining" control means to make the UI system know a list of controls
 * are horizontally or vertically lined up, so that we will be able to
 * move focus among them using arrow keys. This is achieved by setting
 * the ctrl_{left,right,up,down} members of Controls.
 *
 * If circle = true, the last control is connected to the first.
 *
 * If other = true, the relative controls on the other direction are
 * copied from the first control to all others.
 */
template <Direction direction, bool circle, bool other>
class ChainControls
{
public:
	ChainControls ()
		: first_control (0)
		, last_control (0)
		, nonempty (false)
	{
	}

	template <std::derived_from<Control> T>
	ChainControls (T *const *lst, size_t n)
		: first_control (n ? lst[0] : 0)
		, last_control (n ? lst[n-1] : 0)
		, nonempty (n)
	{
		for (size_t i=1; i<n; ++i) {
			chain (lst[i-1], lst[i]);
			if (other) {
				copy_other (lst[0], lst[i]);
			}
		}
	}

	ChainControls(std::initializer_list<Control *> il)
		: ChainControls(il.begin(), il.size()) {
	}

	~ChainControls ()
	{
		if (circle && nonempty) {
			chain (last_control, first_control);
		}
	}

private:
	Control *first_control;
	Control *last_control;
	bool nonempty; // For efficiency purpose. Do not use sth. like (first_control == 0)

	static void chain (Control *a, Control *b)
	{
		if (direction == VERTICAL) {
			a->ctrl_down = b;
			b->ctrl_up = a;
		}
		else {
			a->ctrl_right = b;
			b->ctrl_left = a;
		}
	}

	static void copy_other (Control *a, Control *b)
	{
		if (direction == VERTICAL) {
			b->ctrl_left = a->ctrl_left;
			b->ctrl_right = a->ctrl_right;
		}
		else {
			b->ctrl_up = a->ctrl_up;
			b->ctrl_down = a->ctrl_down;
		}
	}
};

} // namespace chain_detail

typedef chain_detail::ChainControls<VERTICAL,  true,  false> ChainControlsVertical;
typedef chain_detail::ChainControls<HORIZONTAL,true,  false> ChainControlsHorizontal;
typedef chain_detail::ChainControls<VERTICAL,  false, false> ChainControlsVerticalNC;
typedef chain_detail::ChainControls<HORIZONTAL,false, false> ChainControlsHorizontalNC;
typedef chain_detail::ChainControls<VERTICAL,  true,  true > ChainControlsVerticalO;
typedef chain_detail::ChainControls<HORIZONTAL,true,  true > ChainControlsHorizontalO;
typedef chain_detail::ChainControls<VERTICAL,  false, true > ChainControlsVerticalNCO;
typedef chain_detail::ChainControls<HORIZONTAL,false, true > ChainControlsHorizontalNCO;



} // namespace tiary::ui
} // namespace tiary
#endif // Include guard
