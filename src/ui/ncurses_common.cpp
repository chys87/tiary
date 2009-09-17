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


#include "ui/ncurses_common.h"
#include "common/algorithm.h"

namespace tiary {
namespace ui {

#ifdef TIARY_USE_MOUSE

namespace {

const tiary::MapStruct<tiary::ui::MouseMask, mmask_t> mouse_mask_map [] = {
	{ LEFT_PRESS,     BUTTON1_PRESSED },
	{ LEFT_RELEASE,   BUTTON1_RELEASED },
	{ LEFT_CLICK,     BUTTON1_CLICKED },
	{ LEFT_DCLICK,    BUTTON1_DOUBLE_CLICKED },
	{ MIDDLE_PRESS,   BUTTON2_PRESSED },
	{ MIDDLE_RELEASE, BUTTON2_RELEASED },
	{ MIDDLE_CLICK,   BUTTON2_CLICKED },
	{ MIDDLE_DCLICK,  BUTTON2_DOUBLE_CLICKED },
	{ RIGHT_PRESS,    BUTTON3_PRESSED },
	{ RIGHT_RELEASE,  BUTTON3_RELEASED },
	{ RIGHT_CLICK,    BUTTON3_CLICKED },
	{ RIGHT_DCLICK,   BUTTON3_DOUBLE_CLICKED },
	{ MOUSE_SHIFT,    BUTTON_SHIFT },
	{ MOUSE_CTRL,     BUTTON_CTRL },
	{ MOUSE_ALT,      BUTTON_ALT },
	{ MOUSE_MOVE,     REPORT_MOUSE_POSITION }
};

} // Anonymous namespace

mmask_t mousemask_to_internal (MouseMask mask)
{
	return bitwise_transform (mouse_mask_map, array_end (mouse_mask_map), mask);
}

MouseMask mousemask_from_internal (mmask_t mask)
{
	return bitwise_reverse_transform (mouse_mask_map, array_end (mouse_mask_map), mask);
}

#endif // TIARY_USE_MOUSE





namespace {

// Map our attribute values to those used by ncurses
inline int internal_attributes (Attr attr)
{
	int r = 0;
	if (attr & UNDERLINE) {
		r |= A_UNDERLINE;
	}
	if (attr & REVERSE) {
		r |= A_REVERSE;
	}
	if (attr & HIGHLIGHT) {
		r |= A_BOLD;
	}
	if (attr & BLINK) {
		r |= A_BLINK;
	}
	return r;
}

} // anonymous namespace


// Map color pair to internal color pair
unsigned internal_color_pair (Color f, Color b)
{
	if (f>=NOCOLOR || b>=NOCOLOR) {
		return 0;
	}
	// Pair 0 is reserved for (Fore,Back) = (WHITE,BLACK)
	// So we need a simple formula that maps
	// [0,7]x[0,7] to [0,64] while keeping (WHITE,BLACK) is mapped to 0
	
	/* b = (b + 8 - BLACK) % 8; */
	f = (f + 8 - WHITE) % 8;
	return b*8+f;
}

int internal_attributes (Color fore, Color back, Attr attr)
{
	return internal_attributes (attr) | COLOR_PAIR (internal_color_pair (fore, back));
}

} // namespace tiary::ui
} // namespace tiary
