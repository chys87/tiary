// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2010, 2018, chys <admin@CHYS.INFO>
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

/**
 * @brief	A class for a movable/resizable object
 *
 * The sole purpose of this class is to provide the @c pos
 * and @c size members and a pure virtual @c move_reisze function.
 */
class MovableObject : public Object
{
public:
	virtual void move_resize (Size newpos, Size newsize) = 0;

	Size get_pos() const { return pos_; }
	Size get_size() const { return size_; }

protected:
	void set_pos(Size pos) { pos_ = pos; }
	void set_size(Size size) { size_ = size; }

private:
	Size pos_ {};
	Size size_ {};
};

} // namespace ui
} // namespace tiary


#endif // include guard
