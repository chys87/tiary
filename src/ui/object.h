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


#ifndef TIARY_UI_OBJECT_H
#define TIARY_UI_OBJECT_H

/**
 * @file	ui/object.h
 * @author	chys <admin@chys.info>
 * @brief	Header for tiary::ui::Object
 */

namespace tiary {
namespace ui {

/**
 * @brief	Ancestor of all UI objects
 */
class Object
{
	Object (const Object &);
	void operator = (const Object &);
public:
	Object () {}
	virtual ~Object () = 0;
};

/*
 * Pure virtual desctructors must be defined!
 */
inline Object::~Object ()
{
}

} // namespace tiary::ui
} // namespace tiary

#endif // Include guard
