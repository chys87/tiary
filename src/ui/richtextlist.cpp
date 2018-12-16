#include "ui/richtextlist.h"
#include "common/unicode.h"
#include "common/split_line.h"
#include <functional>
#include <algorithm>
#include <wchar.h>


namespace tiary {
namespace ui {

void append_richtext_line (std::wstring &text, RichTextLineList &lst,
		PaletteID id, const std::wstring &line_text)
{
	RichTextLine tmp_line = { text.length (), line_text.length (), id, ucs_width (line_text) };
	lst.push_back (tmp_line);
	text += line_text;
}

void append_richtext_line (std::wstring &text, RichTextLineList &lst,
		PaletteID id, const wchar_t *line_text)
{
	size_t len = wcslen (line_text);
	RichTextLine tmp_line = { text.length (), len, id, ucs_width (line_text, len) };
	lst.push_back (tmp_line);
	text.append (line_text, len);
}

void append_richtext_line (std::wstring &text, RichTextLineList &lst,
		PaletteID id, const wchar_t *text_a, const std::wstring &text_b)
{
	size_t len_a = wcslen (text_a);
	RichTextLine tmp_line = {
		text.length (),
		len_a + text_b.length (),
		id,
		ucs_width (text_a, len_a) + ucs_width (text_b)
	};
	lst.push_back (tmp_line);
	text.append (text_a, len_a);
	text.append (text_b);
}

void append_richtext_line (std::wstring &text, RichTextLineList &lst,
		PaletteID id, unsigned repeat, wchar_t ch)
{
	RichTextLine tmp_line = { text.length (), repeat, id, repeat * ucs_width (ch) };
	lst.push_back (tmp_line);
	text.append (repeat, ch);
}

void append_richtext_line (std::wstring &text, RichTextLineList &lst,
		PaletteID id)
{
	RichTextLine tmp_line = { text.length (), 0, id, 0 };
	lst.push_back (tmp_line);
}

RichTextLineList combine_lines (std::wstring &str, const RichTextLineC *linec, size_t nlines)
{
	str.clear ();
	RichTextLineList line_list (nlines);
	RichTextLineList::iterator it = line_list.begin ();
	for (; nlines; --nlines) {
		it->id = linec->id;
		size_t len = wcslen (linec->text);
		it->offset = str.length ();
		it->len = len;
		it->screen_wid = ucs_width (linec->text, len);
		str.append (linec->text, len);
		++linec;
		++it;
	}
	return line_list;
}

namespace {

struct SplitStringLine2RichTextLine {
	PaletteID id;
	explicit SplitStringLine2RichTextLine (PaletteID &id_) : id(id_) {}
	RichTextLine operator () (const SplitStringLine &in) const
	{
		RichTextLine ret;
		ret.offset = in.begin;
		ret.len = in.len;
		ret.id = id;
		ret.screen_wid = in.wid;
		return ret;
	}
};

} // anonymous namespace

RichTextLineList split_richtext_lines (const std::wstring &str, PaletteID id, unsigned wid)
{
	SplitStringLineList split = split_line (wid, str);
	RichTextLineList ret (split.size ());
	std::transform (split.begin (), split.end (), ret.begin (), SplitStringLine2RichTextLine (id));
	return ret;
}

} // namespace tiary::ui
} // namespace tiary

