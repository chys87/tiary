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


#ifndef TIARY_MAIN_DOC_H
#define TIARY_MAIN_DOC_H

/**
 * @file	main/doc.h
 * @author	chys <admin@chys.info>
 * @brief	Header for tiary::show_doc
 */

namespace tiary {

/**
 * @brief	Displays help info
 */
void show_doc ();

/**
 * @brief	Displays the license box
 */
void show_license ();

/**
 * @brief	Displays the about box
 */
void show_about ();

}

#endif // include guard
