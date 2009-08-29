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

const PaletteID PALETTE_ID_BACKGROUND           = 0;
const PaletteID PALETTE_ID_LABEL                = 1;
const PaletteID PALETTE_ID_BUTTON_FOCUS         = 2;
const PaletteID PALETTE_ID_BUTTON_UNFOCUS       = 3;
const PaletteID PALETTE_ID_TEXTBOX              = 4;
const PaletteID PALETTE_ID_LISTBOX              = 5;
const PaletteID PALETTE_ID_LISTBOX_SELECT       = 6;
const PaletteID PALETTE_ID_CHECKBOX             = 7;
const PaletteID PALETTE_ID_MENU                 = 8;
const PaletteID PALETTE_ID_MENU_SELECT          = 9;
const PaletteID PALETTE_ID_MENUBAR              = 10;
const PaletteID PALETTE_ID_MENUBAR_SELECT       = 11;
const PaletteID PALETTE_ID_DROPLIST             = 12;
const PaletteID PALETTE_ID_RICHTEXT             = 13;

const PaletteID PALETTE_ID_ENTRY                = 50;
const PaletteID PALETTE_ID_ENTRY_TEXT           = 51;
const PaletteID PALETTE_ID_ENTRY_DATE           = 52;
const PaletteID PALETTE_ID_ENTRY_TITLE          = 53;
const PaletteID PALETTE_ID_ENTRY_LABELS         = 54;
const PaletteID PALETTE_ID_ENTRY_SELECT         = 60;
const PaletteID PALETTE_ID_ENTRY_TEXT_SELECT    = 61;
const PaletteID PALETTE_ID_ENTRY_DATE_SELECT    = 62;
const PaletteID PALETTE_ID_ENTRY_TITLE_SELECT   = 63;
const PaletteID PALETTE_ID_ENTRY_LABELS_SELECT  = 64;

const PaletteID PALETTE_ID_SHOW_NORMAL          = 100;
const PaletteID PALETTE_ID_SHOW_BOLD            = 101;

void set_palettes ();

} // namespace tiary::ui
} // namespace tiary


#endif // include guard
