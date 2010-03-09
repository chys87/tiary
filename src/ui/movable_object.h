// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2010, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#ifndef TIARY_UI_MOVABLE_OBJECT_H
#define TIARY_UI_MOVABLE_OBJECT_H

#include "ui/object.h"
#include "ui/size.h"

namespace tiary {
namespace ui {

class Control;

/**
 * @brief	A class for a movable/resizable object
 *
 * The sole purpose of this class is to provide the @c pos
 * and @c size members and a pure virtual @c move_reisze function.
 */
class MovableObject : public Object
{
protected:
	Size pos;
	Size size;

public:
	MovableObject () : pos (make_size ()), size (make_size ()) {}

	virtual void move_resize (Size newpos, Size newsize) = 0;

	Size get_pos () const { return pos; }
	Size get_size () const { return size; }
};

} // namespace ui
} // namespace tiary


#endif // include guard
