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


#ifndef TIARY_UI_NCURSES_COMMON_H
#define TIARY_UI_NCURSES_COMMON_H

/**
 * For internal use by the UI system.
 */

#include <ncursesw/curses.h>
// ncurses defines many macros that have very short names.
// We include it first to make sure
// that an error is issued in case we have a symbol with the
// same name
#include "ui/ui.h"
#include "ui/mouse.h"

namespace tiary {
namespace ui {

// Map color pair to internal color pair
unsigned internal_color_pair (Color f, Color b);

// Map our attribute values to those used by ncurses
int internal_attributes (Color fore, Color back, Attr attr);

#ifdef TIARY_USE_MOUSE
MouseMask mousemask_from_internal (mmask_t);
mmask_t   mousemask_to_internal (MouseMask);
#endif // TIARY_USE_MOUSE

} // namespace tiary::ui
} // namespace tiary

#endif // include guard
