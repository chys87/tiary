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


#ifndef TIARY_UI_PALETTEID_H
#define TIARY_UI_PALETTEID_H

/**
 * @file	ui/paletteid.h
 * @author	chys <admin@chys.info>
 * @brief	Defines the "standard" palette IDs for "standard" controls.
 */

#include "ui/ui.h"

namespace tiary {
namespace ui {

const unsigned PALETTEID_H_LINE = __LINE__ + 1;
const PaletteID PALETTE_ID_BACKGROUND           = __LINE__ - PALETTEID_H_LINE;
const PaletteID PALETTE_ID_MOVING_BORDER        = __LINE__ - PALETTEID_H_LINE;
const PaletteID PALETTE_ID_LABEL                = __LINE__ - PALETTEID_H_LINE;
const PaletteID PALETTE_ID_BUTTON_FOCUS         = __LINE__ - PALETTEID_H_LINE;
const PaletteID PALETTE_ID_BUTTON_NORMAL        = __LINE__ - PALETTEID_H_LINE;
const PaletteID PALETTE_ID_BUTTON_DEFAULT       = __LINE__ - PALETTEID_H_LINE;
const PaletteID PALETTE_ID_TEXTBOX              = __LINE__ - PALETTEID_H_LINE;
const PaletteID PALETTE_ID_TEXTBOX_FOCUS        = __LINE__ - PALETTEID_H_LINE;
const PaletteID PALETTE_ID_LISTBOX              = __LINE__ - PALETTEID_H_LINE;
const PaletteID PALETTE_ID_LISTBOX_SELECT       = __LINE__ - PALETTEID_H_LINE;
const PaletteID PALETTE_ID_CHECKBOX             = __LINE__ - PALETTEID_H_LINE;
const PaletteID PALETTE_ID_CHECKBOX_FOCUS       = __LINE__ - PALETTEID_H_LINE;
const PaletteID PALETTE_ID_MENU                 = __LINE__ - PALETTEID_H_LINE;
const PaletteID PALETTE_ID_MENU_SELECT          = __LINE__ - PALETTEID_H_LINE;
const PaletteID PALETTE_ID_MENU_INVALID         = __LINE__ - PALETTEID_H_LINE;
const PaletteID PALETTE_ID_MENUBAR              = __LINE__ - PALETTEID_H_LINE;
const PaletteID PALETTE_ID_MENUBAR_SELECT       = __LINE__ - PALETTEID_H_LINE;
const PaletteID PALETTE_ID_DROPLIST             = __LINE__ - PALETTEID_H_LINE;
const PaletteID PALETTE_ID_DROPLIST_FOCUS       = __LINE__ - PALETTEID_H_LINE;
const PaletteID PALETTE_ID_RICHTEXT             = __LINE__ - PALETTEID_H_LINE;
const PaletteID PALETTE_ID_GRID                 = __LINE__ - PALETTEID_H_LINE;
const PaletteID PALETTE_ID_GRID_INVALID         = __LINE__ - PALETTEID_H_LINE;
const PaletteID PALETTE_ID_GRID_SELECT          = __LINE__ - PALETTEID_H_LINE;

const PaletteID PALETTE_ID_ENTRY                = __LINE__ - PALETTEID_H_LINE;
const PaletteID PALETTE_ID_ENTRY_TEXT           = __LINE__ - PALETTEID_H_LINE;
const PaletteID PALETTE_ID_ENTRY_DATE           = __LINE__ - PALETTEID_H_LINE;
const PaletteID PALETTE_ID_ENTRY_TITLE          = __LINE__ - PALETTEID_H_LINE;
const PaletteID PALETTE_ID_ENTRY_LABELS         = __LINE__ - PALETTEID_H_LINE;
const PaletteID PALETTE_ID_ENTRY_SELECT         = __LINE__ - PALETTEID_H_LINE;
const PaletteID PALETTE_ID_ENTRY_TEXT_SELECT    = __LINE__ - PALETTEID_H_LINE;
const PaletteID PALETTE_ID_ENTRY_DATE_SELECT    = __LINE__ - PALETTEID_H_LINE;
const PaletteID PALETTE_ID_ENTRY_TITLE_SELECT   = __LINE__ - PALETTEID_H_LINE;
const PaletteID PALETTE_ID_ENTRY_LABELS_SELECT  = __LINE__ - PALETTEID_H_LINE;

const PaletteID PALETTE_ID_SHOW_NORMAL          = __LINE__ - PALETTEID_H_LINE;
const PaletteID PALETTE_ID_SHOW_BOLD            = __LINE__ - PALETTEID_H_LINE;

void set_palettes ();
ColorAttr get_palette (PaletteID);

} // namespace tiary::ui
} // namespace tiary


#endif // include guard
