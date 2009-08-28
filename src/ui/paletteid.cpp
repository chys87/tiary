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


#include "ui/paletteid.h"
#include "ui/ui.h"
#include <algorithm>

namespace tiary {
namespace ui {

namespace {

// Many palettes can map to a single meta-palette

typedef unsigned MetaPaletteID;

struct MetaPaletteMapping {
	PaletteID id;
	MetaPaletteID mid;
};

const ColorAttr mdesc[] = {
	// Put the commas in the beginning of lines to make VCS happy

	 /* 0: Background   */ { BLACK, WHITE, 0 }
	,/* 1: Edit area    */ { WHITE, BLACK, 0 }
	,/* 2: Edit area sel*/ { BLACK, YELLOW, 0 }
	,/* 3: Reserved     */ { }
	,/* 4: Reserved     */ { }
	,/* 5: Reserved     */ { }
	,/* 6: Reserved     */ { }
	,/* 7: Reserved     */ { }
	,/* 8: Reserved     */ { }
	,/* 9: Reserved     */ { }
	,/*10: Entry text   */ { DEFAULT_FORECOLOR, DEFAULT_BACKCOLOR, 0 }
	,/*11: Entry title  */ { RED, DEFAULT_BACKCOLOR, HIGHLIGHT }
	,/*12: Entry tags   */ { CYAN, DEFAULT_BACKCOLOR, 0 }
	,/*13: Reserved     */ { }
	,/*14: Reserved     */ { }
	,/*15: Reserved     */ { }
	,/*16: Reserved     */ { }
	,/*17: Reserved     */ { }
	,/*18: Reserved     */ { }
	,/*19: Reserved     */ { }
	,/*20: Select text  */ { WHITE, MAGENTA, HIGHLIGHT }
	,/*21: Select title */ { YELLOW, MAGENTA, HIGHLIGHT }
	,/*22: Select tags  */ { CYAN, MAGENTA, HIGHLIGHT }
	,/*23: Reserved     */ { }
	,/*24: Reserved     */ { }
	,/*25: Reserved     */ { }
	,/*26: Reserved     */ { }
	,/*27: Reserved     */ { }
	,/*28: Reserved     */ { }
	,/*29: Reserved     */ { }
	,/*30: Rich normal  */ { WHITE, BLACK, 0 }
	,/*31: Rich bold    */ { WHITE, BLACK, HIGHLIGHT }
};

MetaPaletteMapping mapping[] = {
	// Put the commas in the beginning of lines to make VCS happy
	{ PALETTE_ID_BACKGROUND,           0 }
	, { PALETTE_ID_LABEL,              0 }
	, { PALETTE_ID_BUTTON_FOCUS,       1 }
	, { PALETTE_ID_BUTTON_UNFOCUS,     0 }
	, { PALETTE_ID_TEXTBOX,            1 }
	, { PALETTE_ID_LISTBOX,            1 }
	, { PALETTE_ID_LISTBOX_SELECT,     2 }
	, { PALETTE_ID_CHECKBOX,           1 }
	, { PALETTE_ID_MENU,               0 }
	, { PALETTE_ID_MENU_SELECT,        1 }
	, { PALETTE_ID_MENUBAR,            0 }
	, { PALETTE_ID_MENUBAR_SELECT,     1 }
	, { PALETTE_ID_DROPLIST,           1 }
	, { PALETTE_ID_RICHTEXT,           1 }

	, { PALETTE_ID_ENTRY,              10 }
	, { PALETTE_ID_ENTRY_TEXT,         10 }
	, { PALETTE_ID_ENTRY_DATE,         10 }
	, { PALETTE_ID_ENTRY_TITLE,        11 }
	, { PALETTE_ID_ENTRY_TAGS,         12 }
	, { PALETTE_ID_ENTRY_SELECT,       20 }
	, { PALETTE_ID_ENTRY_TEXT_SELECT,  20 }
	, { PALETTE_ID_ENTRY_DATE_SELECT,  20 }
	, { PALETTE_ID_ENTRY_TITLE_SELECT, 21 }
	, { PALETTE_ID_ENTRY_TAGS_SELECT,  22 }

	, { PALETTE_ID_SHOW_NORMAL,        30 }
	, { PALETTE_ID_SHOW_BOLD,          31 }

};

} // anonymous namespace

void set_palettes ()
{
	for (const MetaPaletteMapping *x = mapping;
			x != mapping + sizeof mapping / sizeof *mapping;
			++x) {
		set_palette (x->id, mdesc[x->mid]);
	}
}

} // namespace tiary::ui
} // namespace tiary

