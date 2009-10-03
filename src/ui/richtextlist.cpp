#include "ui/richtextlist.h"
#include "common/unicode.h"


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

} // namespace tiary::ui
} // namespace tiary

