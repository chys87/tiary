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


#include "ui/paletteid.h"
#include "ui/ui.h"
#include <algorithm>

namespace tiary {
namespace ui {

namespace {

constexpr ColorAttr BACKGROUND = {BLACK, WHITE, 0};
constexpr ColorAttr BACKGROUND_HIGH = {WHITE, WHITE, HIGHLIGHT};
constexpr ColorAttr BACKGROUND_GRAY = {YELLOW, WHITE, 0};
constexpr ColorAttr BACKGROUND_BLUE = {BLUE, WHITE, HIGHLIGHT};
constexpr ColorAttr EDIT_AREA = {WHITE, BLACK, 0};
constexpr ColorAttr EDIT_AREA_SELECT = {BLACK, YELLOW, 0};
constexpr ColorAttr EDIT_AREA_GRAY = {YELLOW, BLACK, 0};
constexpr ColorAttr ENTRY_TEXT = {DEFAULT_FORECOLOR, DEFAULT_BACKCOLOR, 0};
constexpr ColorAttr ENTRY_TITLE = {RED, DEFAULT_BACKCOLOR, HIGHLIGHT};
constexpr ColorAttr ENTRY_LABEL = {CYAN, DEFAULT_BACKCOLOR, 0};
constexpr ColorAttr SELECT_ENTRY_TEXT = {WHITE, MAGENTA, HIGHLIGHT};
constexpr ColorAttr SELECT_ENTRY_TITLE = {YELLOW, MAGENTA, HIGHLIGHT};
constexpr ColorAttr SELECT_ENTRY_LABEL = {CYAN, MAGENTA, HIGHLIGHT};
constexpr ColorAttr RICHTEXT_NORMAL = {WHITE, BLACK, 0};
constexpr ColorAttr RICHTEXT_BOLD = {WHITE, BLACK, HIGHLIGHT};
constexpr ColorAttr RICHTEXT_QUOTE = {CYAN, BLACK, 0};

struct PaletteMap
{
	PaletteID id;
	ColorAttr a;
};

const PaletteMap mapping[] = {
	{ PALETTE_ID_BACKGROUND,         BACKGROUND },
	{ PALETTE_ID_MOVING_BORDER,      BACKGROUND_HIGH },
	{ PALETTE_ID_LABEL,              BACKGROUND },
	{ PALETTE_ID_BUTTON_FOCUS,       EDIT_AREA_SELECT },
	{ PALETTE_ID_BUTTON_NORMAL,      BACKGROUND },
	{ PALETTE_ID_BUTTON_DEFAULT,     EDIT_AREA },
	{ PALETTE_ID_BUTTON_INVALID,     BACKGROUND_GRAY },
	{ PALETTE_ID_BUTTON_FOCUS_INVALID,EDIT_AREA_GRAY },
	{ PALETTE_ID_TEXTBOX,            EDIT_AREA },
	{ PALETTE_ID_TEXTBOX_FOCUS,      EDIT_AREA_SELECT },
	{ PALETTE_ID_LISTBOX,            EDIT_AREA },
	{ PALETTE_ID_LISTBOX_SELECT,     EDIT_AREA_SELECT },
	{ PALETTE_ID_CHECKBOX,           EDIT_AREA },
	{ PALETTE_ID_CHECKBOX_FOCUS,     EDIT_AREA_SELECT },
	{ PALETTE_ID_MENU,               BACKGROUND },
	{ PALETTE_ID_MENU_SELECT,        EDIT_AREA },
	{ PALETTE_ID_MENU_INVALID,       BACKGROUND_GRAY },
	{ PALETTE_ID_MENUBAR,            BACKGROUND },
	{ PALETTE_ID_MENUBAR_SELECT,     EDIT_AREA },
	{ PALETTE_ID_DROPLIST,           EDIT_AREA },
	{ PALETTE_ID_DROPLIST_FOCUS,     EDIT_AREA_SELECT },
	{ PALETTE_ID_RICHTEXT,           EDIT_AREA },
	{ PALETTE_ID_GRID,               EDIT_AREA },
	{ PALETTE_ID_GRID_INVALID,       EDIT_AREA_GRAY },
	{ PALETTE_ID_GRID_SELECT,        EDIT_AREA_SELECT },
	{PALETTE_ID_HOTKEY_HINT,         BACKGROUND },
	{PALETTE_ID_HOTKEY_HINT_KEY,     BACKGROUND_BLUE },

	{ PALETTE_ID_ENTRY,              ENTRY_TEXT },
	{ PALETTE_ID_ENTRY_TEXT,         ENTRY_TEXT },
	{ PALETTE_ID_ENTRY_DATE,         ENTRY_TEXT },
	{ PALETTE_ID_ENTRY_TITLE,        ENTRY_TITLE },
	{ PALETTE_ID_ENTRY_LABELS,       ENTRY_LABEL },
	{ PALETTE_ID_ENTRY_SELECT,       SELECT_ENTRY_TEXT },
	{ PALETTE_ID_ENTRY_TEXT_SELECT,  SELECT_ENTRY_TEXT },
	{ PALETTE_ID_ENTRY_DATE_SELECT,  SELECT_ENTRY_TEXT },
	{ PALETTE_ID_ENTRY_TITLE_SELECT, SELECT_ENTRY_TITLE },
	{ PALETTE_ID_ENTRY_LABELS_SELECT,SELECT_ENTRY_LABEL },

	{ PALETTE_ID_SHOW_NORMAL,        RICHTEXT_NORMAL },
	{ PALETTE_ID_SHOW_BOLD,          RICHTEXT_BOLD },
	{ PALETTE_ID_SHOW_QUOTE,         RICHTEXT_QUOTE },

};

ColorAttr palette_table[NUMBER_PALETTES];

} // anonymous namespace

void set_palettes ()
{
	for (const PaletteMap &m: mapping) {
		palette_table[m.id] = m.a;
	}
}

ColorAttr get_palette (PaletteID id)
{
	if (id < NUMBER_PALETTES) {
		return palette_table[id];
	}
	else {
		return ColorAttr{};
	}
}

} // namespace tiary::ui
} // namespace tiary

