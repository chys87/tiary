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
#include <vector>
#include <string>

namespace tiary {
namespace ui {

struct RichTextLineC
{
	PaletteID id;
	const wchar_t *text;
};

struct RichTextLine
{
	size_t offset; ///< Offset relative to the beginning of the text
	size_t len; ///< Number of wchar_t's on the screen
	PaletteID id;
	unsigned screen_wid; ///< Width on screen
};

typedef std::vector<RichTextLine> RichTextLineList;

void append_richtext_line (std::wstring &text, RichTextLineList &lst,
		PaletteID id, const std::wstring &line_text);
void append_richtext_line (std::wstring &text, RichTextLineList &lst,
		PaletteID id, const wchar_t *line_text);
void append_richtext_line (std::wstring &text, RichTextLineList &lst,
		PaletteID id, unsigned repeat, wchar_t ch);
void append_richtext_line (std::wstring &text, RichTextLineList &lst,
		PaletteID id); // Empty line
void append_richtext_line (std::wstring &text, RichTextLineList &lst,
		PaletteID id, const wchar_t *line_text, const std::wstring &text2);

/// @brief Convert a series of lines represented in RichTextLineC to RichTextLineList
RichTextLineList combine_lines (std::wstring &str, const RichTextLineC *, size_t);
/// @brief Split a string to lines, each with the same palette id
RichTextLineList split_richtext_lines (const std::wstring &, PaletteID, unsigned wid);

} // namespace tiary::ui
} // namespace tiary

#endif
