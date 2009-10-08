/**
 * @file	ui/richtext.cpp
 * @author	chys <admin@chys.info>
 * @brief	Implement class tiary::ui::RichText
 */

#include "ui/richtext.h"
#include "ui/paletteid.h"
#include "ui/dialog_message.h"
#include "ui/mouse.h"
#include "common/format.h"
#include "common/algorithm.h"
#include <utility> // std::forward

namespace tiary {
namespace ui {

RichText::RichText (Window &win, const std::wstring &txt, const LineList &lst)
	: Control (win)
	, text (txt)
	, line_list (lst)
	, top_line (0)
	, highlight_list ()
	, search_info ()
{
	set_cursor_visibility (false);
}

#ifdef TIARY_HAVE_RVALUE_REFERENCES
RichText::RichText (Window &win, const std::wstring &txt, LineList &&lst)
	: Control (win)
	, text (txt)
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
	if (int (wid) < 0 || int (hgt) < 0) {
		return;
	}
	unsigned show_lines = minU (hgt, line_list.size () - top_line);

	for (unsigned i=0; i<show_lines; ++i) {
		Size pos = make_size (0, i);
		choose_palette (line_list[top_line+i].id);
		clear (pos, make_size (wid, 1));
		size_t offset = line_list[top_line+i].offset;
		size_t end_offset = line_list[top_line+i].len + offset;
		HighlightList::const_iterator lower = highlight_list.lower_bound (offset);
		HighlightList::const_iterator upper = highlight_list.lower_bound (end_offset);
		// We may have a match whose starting point is on the previous line
		// Check for that!
		if (lower != highlight_list.begin ()) {
			HighlightList::const_iterator last = lower;
			--last;
			if (offset < last->first + last->second) {
				lower = last;
			}
		}
		for (HighlightList::const_iterator it = lower; it != upper; ++it) {
			if (offset < it->first) {
				pos = put (pos, text.data()+offset, it->first-offset);
				offset = it->first;
			}
			attribute_toggle (REVERSE);
			size_t highlight_end = minSize (it->first+it->second, end_offset);
			pos = put (pos, text.data()+offset, highlight_end-offset);
			attribute_toggle (REVERSE);
			offset = highlight_end;
		}
		pos = put (pos, text.data()+offset, end_offset-offset);
	}
	if (show_lines < hgt) {
		choose_palette (PALETTE_ID_RICHTEXT);
		clear (make_size (0, show_lines), make_size (wid, hgt - show_lines));
	}
	// Status bar
	choose_palette (PALETTE_ID_BACKGROUND);
	clear (make_size (0, hgt), make_size (wid, 1));
	put (make_size (0, hgt),
			format (L"Lines %a-%b/%c") << top_line+1 << top_line+show_lines
				<< unsigned (line_list.size ())
			);
	// Scroll bar
	clear (make_size (wid, 0), make_size (1, hgt+1));
	unsigned bar_start = top_line * hgt / line_list.size ();
	unsigned bar_height = maxU (1, (top_line + show_lines) * hgt / line_list.size () - bar_start);
	attribute_toggle (REVERSE);
	clear (make_size (wid, bar_start), make_size (1, bar_height));
}

bool RichText::on_mouse (MouseEvent mouse_event)
{
	if (!(mouse_event.m & (LEFT_CLICK|LEFT_PRESS|LEFT_DCLICK)) ||
			(mouse_event.p.x+1 < get_size().x)) {
		return false;
	}
	top_line = mouse_event.p.y * line_list.size () / (get_size ().y - 1);
	RichText::redraw ();
	return true;
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
		std::vector <Pair <size_t, size_t> > result = search_info.match (text);
		highlight_list.clear ();
		for (std::vector <Pair <size_t, size_t> >::const_iterator it = result.begin ();
				it != result.end (); ++it) {
			highlight_list.insert (std::make_pair (it->first, it->second));
		}
		do_search (false, true);
	}
}

void RichText::slot_search_continue (bool previous)
{
	if (!search_info) {
		slot_search (previous);
	}
	else {
		do_search (previous, false);
	}
}

void RichText::do_search (bool previous, bool include_current)
{
	unsigned k = top_line;
	unsigned num_ents = line_list.size ();
	int inc = (!previous == !search_info.get_backward ()) ? 1 : -1;
	if (!include_current) {
		k += inc;
	}
	for (; k < num_ents; k += inc) {
		// Is there any match on the k-th line?
		if (highlight_list.lower_bound (line_list[k].offset)
				!= highlight_list.lower_bound (line_list[k].offset+line_list[k].len)) {
			top_line = k;
			RichText::redraw ();
			return;
		}
	}
	dialog_message (L"Not found", L"Error");
}

} // namespace tiary::ui
} // namespace tiary
