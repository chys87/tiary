/**
 * @file	ui/richtext.cpp
 * @author	chys <admin@chys.info>
 * @brief	Implement class tiary::ui::RichText
 */

#include "ui/richtext.h"
#include "ui/paletteid.h"
#include "common/format.h"
#include <utility> // std::forward

namespace tiary {
namespace ui {

RichText::RichText (Window &dlg, const LineList &lst)
	: Control (dlg)
	, line_list (lst)
	, top_line (0)
{
	set_cursor_visibility (false);
}

#ifdef TIARY_HAVE_RVALUE_REFERENCES
RichText::RichText (Window &dlg, LineList &&lst)
	: Control (dlg)
	, line_list (std::forward <LineList> (lst))
	, top_line (0)
{
	set_cursor_visibility (false);
}
#endif

RichText::~RichText ()
{
}

void RichText::redraw ()
{
	unsigned wid = get_size ().x;
	unsigned hgt = get_size ().y - 1;
	LineList::const_iterator it = line_list.begin () + top_line;
	unsigned show_lines = minU (hgt, line_list.size () - top_line);
	for (unsigned i=0; i<show_lines; ++it,++i) {
		Size pos = make_size (0, i);
		choose_palette (it->id);
		clear (pos, make_size (wid, 1));
		put (pos, it->text);
	}
	if (show_lines < hgt) {
		choose_palette (PALETTE_ID_RICHTEXT);
		clear (make_size (0, show_lines), make_size (wid, hgt - show_lines));
	}
	choose_palette (PALETTE_ID_BACKGROUND);
	put (make_size (0, hgt),
			format (L"Lines %a-%b/%c                      ") << top_line+1 << top_line+show_lines
				<< unsigned (line_list.size ())
			);
}

void RichText::on_move_resize (Size oldpos, Size oldsize)
{
	RichText::redraw ();
}

bool RichText::on_key (wchar_t key)
{
	switch (key) {
		case L'k':
		case UP:
			if (top_line) {
				--top_line;
				RichText::redraw ();
				return true;
			}
			break;
		case L'j':
		case DOWN:
			if (top_line + 1 < line_list.size ()) {
				++top_line;
				RichText::redraw ();
				return true;
			}
			break;
		case HOME:
		case L'^':
		case L'g':
		case L'<':
			top_line = 0;
			RichText::redraw ();
			return true;
		case END:
		case L'$':
		case L'>':
		case L'G':
			if (line_list.size ()) {
				top_line = line_list.size () - 1;
				RichText::redraw ();
				return true;
			}
			break;
		case PAGEUP:
		case L'b':
			if (top_line) {
				top_line = maxS (0, top_line - get_size ().y + 2);
				RichText::redraw ();
			}
			break;

		case PAGEDOWN:
		case L'f':
		case L' ':
			if (top_line + 1 < line_list.size ()) {
				top_line = minU (line_list.size () - 1, top_line + get_size ().y - 2);
				RichText::redraw ();
				return true;
			}
			break;
	}
	return false;
}

} // namespace tiary::ui
} // namespace tiary
