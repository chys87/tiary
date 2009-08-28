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
#include "ui/paletteid.h"
#include <stdlib.h>
#include <algorithm> // std::fill_n


namespace tiary {
namespace ui {

namespace {

bool is_initialized;

void init_color_pairs ()
{
	use_default_colors ();
	for (unsigned back=0; back<8; ++back)
		for (unsigned fore=0; fore<8; ++fore)
			init_pair (internal_color_pair (fore, back), fore, back ? int/*Supress warning*/(back) : -1);
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
		start_color ();
		init_color_pairs ();
#ifdef TIARY_USE_MOUSE
		mmask_t m = mousemask_to_internal (MOUSE_ALL_BUTTON);
		mousemask (m, 0);
#endif
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

void finalize ()
{
	if (is_initialized) {
		endwin ();
		is_initialized = false;
	}
}

Size get_screen_size ()
{
	return make_size (COLS, LINES);
}

namespace {

const ColorAttr palette_default = { DEFAULT_FORECOLOR, DEFAULT_BACKCOLOR, 0 };

struct Palettes
{
	ColorAttr t[NUMBER_PALETTES];

	Palettes ()
	{
		std::fill_n (t, NUMBER_PALETTES, palette_default);
	}

	ColorAttr &operator [] (size_t k) { return t[k]; }
	const ColorAttr &operator [] (size_t k) const { return t[k]; }
};

Palettes palette_table;

} // anonymous namespace


void set_palette (PaletteID id, ColorAttr attr)
{
	if (id < NUMBER_PALETTES)
		palette_table[id] = attr;
}

ColorAttr get_palette (PaletteID id)
{
	if (id < NUMBER_PALETTES)
		return palette_table[id];
	else
		return palette_default;
}



} // namespace tiary::ui
} // namespace tiary
