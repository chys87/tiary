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


#ifndef TIARY_UI_CHAIN_H
#define TIARY_UI_CHAIN_H

/**
 * @file	ui/chain.h
 * @author	chys <admin@chys.info>
 * @brief	Defines class tiary::ui::ChainControls
 */

#include "ui/control.h"
#include "ui/direction.h"

namespace tiary {
namespace ui {
namespace detail {

/**
 * @brief	Helps "chaining" controls
 *
 * "Chaining" control means to make the UI system know a list of controls
 * are horizontally or vertically lined up, so that we will be able to
 * move focus among them using arrow keys. This is achieved by setting
 * the ctrl_{left,right,up,down} members of Controls.
 *
 * This function only provides an easier-to-use interface to set them.
 *
 * ChainControlsVertical () (first_control) (next_control) (third_control) ... (last_control)
 *
 * Lists are also supported, e.g.:
 *
 * ChainControlsHorizontal (controls, n_controls) (next_control) (controls,n_controls);
 *
 * The empty brackets "()" is recommended if the first argument is one single Control, not a list.
 * This is for technical reaons:
 *
 * ChainControlsVertical (control_a) (control_b);
 *
 * means
 *
 * ChainControlsVertical control_a (control_b);
 *
 * instead of
 *
 * ChainControlsVertical (control_a).operator () (control_b);
 *
 * When variadic template (C++0x feature) is widely available, we can
 * make more comfortable interfaces.
 *
 * If circle = true, the last control is connected to the first.
 *
 * If other = true, the relative controls on the other direction are
 * copied from the first control to all others.
 */
template <Direction direction, bool circle, bool other>
class ChainControls : private noncopyable
{
public:
	ChainControls ()
		: first_control (0)
		, last_control (0)
		, nonempty (false)
	{
	}

	// "explicit" is intentionally missing. So that we can also write
	// (ChainControls<..>)(first_control)(second_control);
	ChainControls (Control *ctrl)
		: first_control (ctrl)
		, last_control (ctrl)
		, nonempty (true)
	{
	}

	ChainControls (Control &ctrl)
		: first_control (&ctrl)
		, last_control (&ctrl)
		, nonempty (true)
	{
	}

	// T must be either Control or a derivative class of Control
	template <typename T> ChainControls (T *const *lst, size_t n)
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

	~ChainControls ()
	{
		if (circle && nonempty) {
			chain (last_control, first_control);
		}
	}

	ChainControls & operator () (Control *next)
	{
		if (nonempty) {
			chain (last_control, next);
			if (other) {
				copy_other (first_control, next);
			}
		}
		else {
			first_control = next;
		}
		last_control = next;
		nonempty = true;
		return *this;
	}

	ChainControls & operator () (Control &next)
	{
		return operator () (&next);
	}

	// T must be either Control or a derivative class of Control
	template <typename T> ChainControls & operator () (T *const *lst, size_t n)
	{
		for (size_t i=0; i<n; ++i) {
			operator () (lst[i]);
		}
		return *this;
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

} // namespace detail

typedef detail::ChainControls<VERTICAL,  true,  false> ChainControlsVertical;
typedef detail::ChainControls<HORIZONTAL,true,  false> ChainControlsHorizontal;
typedef detail::ChainControls<VERTICAL,  false, false> ChainControlsVerticalNC;
typedef detail::ChainControls<HORIZONTAL,false, false> ChainControlsHorizontalNC;
typedef detail::ChainControls<VERTICAL,  true,  true > ChainControlsVerticalO;
typedef detail::ChainControls<HORIZONTAL,true,  true > ChainControlsHorizontalO;
typedef detail::ChainControls<VERTICAL,  false, true > ChainControlsVerticalNCO;
typedef detail::ChainControls<HORIZONTAL,false, true > ChainControlsHorizontalNCO;



} // namespace tiary::ui
} // namespace tiary
#endif // Include guard
