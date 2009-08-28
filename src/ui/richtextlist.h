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
	PaletteID id;
	std::wstring text;
	unsigned screen_wid; ///< Width on screen


	RichTextLine (const RichTextLineC &);
	RichTextLine (PaletteID id_, const std::wstring &text_);
	RichTextLine (PaletteID id_, const wchar_t *text_);

	RichTextLine (const RichTextLine &other)
		: id(other.id)
		, text (other.text)
		, screen_wid (other.screen_wid)
	{
	}

	RichTextLine &operator = (const RichTextLine &other)
	{
		id = other.id;
		text = other.text;
		screen_wid = other.screen_wid;
		return *this;
	}
};

typedef std::vector<RichTextLine> RichTextList;

} // namespace tiary::ui
} // namespace tiary

#endif
