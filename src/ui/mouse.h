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

#ifndef TIARY_UI_MOUSE_H
#define TIARY_UI_MOUSE_H

#include "ui/size.h"

namespace tiary {
namespace ui {


typedef unsigned MouseMask;
/**
 * @brief	Describes a mouse event
 *
 * Define this class even if TIARY_USE_MOUSE is not defined.
 */
struct MouseEvent {
	Size p; ///< @brief Position of the mouse event
	MouseMask m; ///< @brief Bitwise OR'd of LEFT_PRESS, MOUSE_ALT, etc.
};

const MouseMask LEFT_PRESS       = 0x0001;
const MouseMask LEFT_RELEASE     = 0x0002;
const MouseMask LEFT_CLICK       = 0x0004;
const MouseMask LEFT_DCLICK      = 0x0008;
const MouseMask MIDDLE_PRESS     = 0x0010;
const MouseMask MIDDLE_RELEASE   = 0x0020;
const MouseMask MIDDLE_CLICK     = 0x0040;
const MouseMask MIDDLE_DCLICK    = 0x0080;
const MouseMask RIGHT_PRESS      = 0x0100;
const MouseMask RIGHT_RELEASE    = 0x0200;
const MouseMask RIGHT_CLICK      = 0x0400;
const MouseMask RIGHT_DCLICK     = 0x0800;
const MouseMask MOUSE_ALL_BUTTON = 0x0fff;
const MouseMask MOUSE_SHIFT      = 0x1000;
const MouseMask MOUSE_CTRL       = 0x2000;
const MouseMask MOUSE_ALT        = 0x4000;
const MouseMask MOUSE_MOVE       = 0x8000;

} // namespace ui
} // namespace tiary

#endif // include guard
