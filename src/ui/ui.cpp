// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2018, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#include "ui/ncurses_common.h"
#include "ui/paletteid.h"
#include <stdlib.h>
#include <algorithm> // std::fill_n


namespace tiary {
namespace ui {

namespace {

bool is_initialized = false;
#ifdef TIARY_USE_MOUSE
bool is_mouse_supported = false;
#endif

void init_color_pairs ()
{
	use_default_colors ();
	for (unsigned back=0; back<8; ++back) {
		for (unsigned fore=0; fore<8; ++fore) {
			init_pair(internal_color_pair(Color(fore), Color(back)), fore, back ? int/*Supress warning*/(back) : -1);
		}
	}
}

} // anonymous namespace

bool init ()
{
	if (!is_initialized) {
		initscr ();
		if (!has_colors ()) {
			endwin ();
			return false;
		}
		raw ();
		noecho ();
		nonl ();
		keypad (stdscr, TRUE);
		intrflush (stdscr, FALSE);

#ifdef HAVE_SET_ESCDELAY
		// The default value set in ncurses is way too long
		// VIM uses 25 milliseconds
		if (getenv ("ESCDELAY") == 0) {
			set_escdelay (50);
		}
#endif

		start_color ();
		init_color_pairs ();
		set_palettes ();
		static bool ever_registered = false;
		if (!ever_registered) {
			atexit (&finalize);
			ever_registered = true;
		}
		is_initialized = true;
	}
	return true;
}

bool initialized ()
{
	return is_initialized;
}

bool get_mouse_status ()
{
#ifdef TIARY_USE_MOUSE
	return is_mouse_supported;
#else
	return false;
#endif
}

void set_mouse_status (bool status)
{
#ifdef TIARY_USE_MOUSE
	mmask_t m = status ? mousemask_to_internal (MOUSE_ALL_BUTTON) : 0;
	mousemask (m, 0);
	is_mouse_supported = status;
#else
	(void) status;
#endif
}

void toggle_mouse_status ()
{
#ifdef TIARY_USE_MOUSE
	set_mouse_status (!get_mouse_status ());
#endif
}

void finalize ()
{
	if (is_initialized) {
		endwin ();
		is_initialized = false;
	}
}

Size get_screen_size ()
{
	return Size(COLS, LINES);
}

unsigned get_screen_width ()
{
	return COLS;
}

unsigned get_screen_height ()
{
	return LINES;
}




} // namespace tiary::ui
} // namespace tiary
