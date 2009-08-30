/**
 * @file	ui/richtext.cpp
 * @author	chys <admin@chys.info>
 * @brief	Implement class tiary::ui::RichText
 */

#include "ui/richtext.h"
#include "ui/paletteid.h"
#include "ui/dialog_message.h"
#include "common/format.h"
#include <utility> // std::forward

namespace tiary {
namespace ui {

RichText::RichText (Window &dlg, const LineList &lst)
	: Control (dlg)
	, line_list (lst)
	, top_line (0)
	, highlight_list ()
	, search_info ()
{
	set_cursor_visibility (false);
}

#ifdef TIARY_HAVE_RVALUE_REFERENCES
RichText::RichText (Window &dlg, LineList &&lst)
	: Control (dlg)
	, line_list (std::forward <LineList> (lst))
	, top_line (0)
	, highlight_list ()
	, search_info ()
{
	set_cursor_visibility (false);
}
#endif

RichText::~RichText ()
{
}

void RichText::redraw ()
{
	unsigned wid = get_size ().x - 1;
	unsigned hgt = get_size ().y - 1;
	if (int (wid) < 0 || int (hgt) < 0)
		return;
	HighlightList *hllst = highlight_list.get ();
	unsigned show_lines = minU (hgt, line_list.size () - top_line);

	for (unsigned i=0; i<show_lines; ++i) {
		Size pos = make_size (0, i);
		choose_palette (line_list[top_line+i].id);
		clear (pos, make_size (wid, 1));
		const std::wstring &text = line_list[top_line+i].text;
		if (hllst && !(*hllst)[top_line+i].empty ()) {
			const std::vector <std::pair <size_t, size_t> > &hl = (*hllst)[top_line+i];
			size_t offset = 0;
			for (std::vector <std::pair <size_t, size_t> >::const_iterator it = hl.begin ();
					it != hl.end (); ++it) {
				pos = put (pos, text.data()+offset, it->first-offset);
				attribute_on (REVERSE);
				pos = put (pos, text.data()+it->first, it->second);
				attribute_off (REVERSE);
				offset = it->first + it->second;
			}
			pos = put (pos, text.data()+offset);
		} else {
			// Nothing to highlight
			put (pos, text);
		}
	}
	if (show_lines < hgt) {
		choose_palette (PALETTE_ID_RICHTEXT);
		clear (make_size (0, show_lines), make_size (wid, hgt - show_lines));
	}
	// Status bar
	choose_palette (PALETTE_ID_BACKGROUND);
	put (make_size (0, hgt),
			format (L"Lines %a-%b/%c                      ") << top_line+1 << top_line+show_lines
				<< unsigned (line_list.size ())
			);
	// Scroll bar
	clear (make_size (wid, 0), make_size (1, hgt+1));
	unsigned bar_start = top_line * hgt / line_list.size ();
	unsigned bar_height = maxU (1, (top_line + show_lines) * hgt / line_list.size () - bar_start);
	attribute_on (REVERSE);
	clear (make_size (wid, bar_start), make_size (1, bar_height));
}

bool RichText::on_mouse (MouseEvent mouse_event)
{
	if (!(mouse_event.m & (LEFT_CLICK|LEFT_PRESS|LEFT_DCLICK)) ||
			(mouse_event.p.x+1 < get_size().x))
		return false;
	top_line = mouse_event.p.y * line_list.size () / (get_size ().y - 1);
	RichText::redraw ();
	return true;
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

		case CTRL_F:
		case L'/':
			slot_search (false);
			return true;

		case L'?':
			slot_search (true);
			return true;

		case F3:
		case L'n':
			slot_search_continue (false);
			return true;

		case L'N':
			slot_search_continue (true);
			return false;
	}
	return false;
}

void RichText::slot_search (bool bkwd)
{
	if (search_info.dialog (bkwd)) {
		bkwd = search_info.get_backward ();
		HighlightList *hllst = new HighlightList (line_list.size ());
		highlight_list.reset (hllst);
		for (size_t i=0, n=line_list.size(); i<n; ++i)
			(*hllst)[i] = search_info.match (line_list[i].text);
		do_search (false, true);
	}
}

void RichText::slot_search_continue (bool previous)
{
	if (!search_info)
		slot_search (previous);
	else
		do_search (previous, false);
}

void RichText::do_search (bool previous, bool include_current)
{
	unsigned k = top_line;
	unsigned num_ents = line_list.size ();
	int inc = (!previous == !search_info.get_backward ()) ? 1 : -1;
	HighlightList *hllst = highlight_list.get ();
	if (!include_current)
		k += inc;
	for (; k < num_ents; k += inc) {
		if (!(*hllst)[k].empty ()) {
			top_line = k;
			RichText::redraw ();
			return;
		}
	}
	dialog_message (L"Not found", L"Error");
}

} // namespace tiary::ui
} // namespace tiary
