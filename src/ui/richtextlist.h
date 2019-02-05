// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2019, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/

#ifndef TIARY_UI_RICHTEXTLIST_H
#define TIARY_UI_RICHTEXTLIST_H

/**
 * @file	ui/richtextlist.h
 * @author	chys <admin@chys.info>
 * @brief	Defines tiary::ui::RichTextLine and tiary::ui::RichTextList
 *
 * This is a separate file to reduce dependencies among headers
 */

#include "ui/ui.h" // PaletteID
#include <initializer_list>
#include <string>
#include <string_view>
#include <vector>

namespace tiary {
namespace ui {

struct RichTextLineC
{
	PaletteID id;
	std::wstring_view text;
};

struct RichTextLine
{
	size_t offset; ///< Offset relative to the beginning of the text
	size_t len; ///< Number of wchar_t's on the screen
	PaletteID id;
	unsigned screen_wid; ///< Width on screen
};

typedef std::vector<RichTextLine> RichTextLineList;

struct MultiLineRichText {
	std::wstring text;
	RichTextLineList lines;

	void append(PaletteID id, std::wstring_view line_text);
	void append(PaletteID id, unsigned repeat, wchar_t ch);
	void append(PaletteID id); // Empty line
	void append(PaletteID id, std::wstring_view line_text, std::wstring_view text2);
};

/// @brief Convert a series of lines represented in RichTextLineC to RichTextLineList
MultiLineRichText combine_lines(std::initializer_list<RichTextLineC>);
/// @brief Split a string to lines, each with the same palette id
RichTextLineList split_richtext_lines(std::wstring_view, PaletteID, unsigned wid);

} // namespace tiary::ui
} // namespace tiary

#endif
