#include "ui/richtextlist.h"
#include "common/unicode.h"


namespace tiary {
namespace ui {

RichTextLine::RichTextLine (const RichTextLineC &other)
	: id (other.id)
	, text (other.text)
	, screen_wid (ucs_width (text))
{
}

RichTextLine::RichTextLine (PaletteID id_, const std::wstring &text_)
	: id (id_)
	, text (text_)
	, screen_wid (ucs_width (text))
{
}

RichTextLine::RichTextLine (PaletteID id_, const wchar_t *text_)
	: id (id_)
	, text (text_)
	, screen_wid (ucs_width (text))
{
}


} // namespace tiary::ui
} // namespace tiary

