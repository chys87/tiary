#include "ui/richtextlist.h"
#include "common/unicode.h"
#include "common/split_line.h"
#include <functional>
#include <algorithm>


namespace tiary {
namespace ui {

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

struct SplitStringLine2RichTextLine : public std::unary_function <const SplitStringLine &, RichTextLine>
{
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

