// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the so-called 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#include "ui/ncurses_common.h"
#include "ui/window.h"
#include "common/containers.h"
#include "common/unicode.h"
#include "common/misc.h"
#include "ui/terminal_emulator.h"
#include <vector>
#include <stack>
#include <string.h>


namespace tiary {
namespace ui {





namespace {

// Global variables for Window

// All instances of windows, ordered by order on screen
// Topmost windows are at the back of the list
typedef std::vector <Window *> WindowList;
WindowList window_list;



// Not using std::vector<bool> because it's specialized by the standard
// I don't really want that little efficiency in space, compared to the overhead
std::vector<char> touched_lines; // 1 = Touched; 0 = Not

// Touches the whole screen
void touch_lines ()
{
	std::fill (touched_lines.begin (), touched_lines.end (), '\1');
	unsigned lines = get_screen_size ().y;
	if (lines != touched_lines.size ())
		touched_lines.resize (lines, '\1');
}

void touch_lines (unsigned top, unsigned height)
{
	unsigned lines = get_screen_size ().y;
	if (lines == touched_lines.size ()) {
		if (top >= lines)
			return;
		height = minU (height, lines - top);
	} else {
		touched_lines.resize (lines, '\1');
		top = 0;
		height = lines;
	}
	std::fill_n (touched_lines.begin () + top, height, '\1');
}

void touch_line (unsigned line)
{
	if (line < touched_lines.size ())
		touched_lines[line] = '\1';
	unsigned scrheight = get_screen_size ().y;
	if (scrheight != touched_lines.size ()) {
		std::fill (touched_lines.begin (), touched_lines.end (), '\1');
		touched_lines.resize (scrheight, '\1');
	}
}

void commit_touched_lines ()
{
	Size size = get_screen_size ();
	unsigned width = size.x;
	unsigned height = size.y;

	if (height != touched_lines.size ()) {
		std::fill (touched_lines.begin (), touched_lines.end (), '\1');
		touched_lines.resize (height, '\1');
	}

	CharColorAttr *line =new CharColorAttr[width];
	cchar_t *cchar_line = new cchar_t[width+1];
	CharColorAttr def = { L' ', { DEFAULT_FORECOLOR, DEFAULT_BACKCOLOR, 0 } };

	for (unsigned y = 0; y < height; ++y) {
		if (!touched_lines[y])
			continue;
		touched_lines[y] = 0;

		// Commit the y-th line
		std::fill_n (line, width, def);

		for (WindowList::const_iterator it = window_list.begin (); it != window_list.end (); ++it) {
			const Window *win = *it;
			if (y - win->get_pos().y >= win->get_size().y)
				continue;
			unsigned win_left = win->get_pos().x;
			if (win_left >= width)
				continue;
			unsigned win_right = minU (width, win->get_size().x + win_left);

			if (win_left && line[win_left].c == L'\0')
				line[win_left-1].c = L' ';
			if (win_right<width && line[win_right].c == L'\0')
				line[win_right].c = L' ';

			memcpy (line+win_left, win->get_char_table (y - win->get_pos().y),
					(win_right - win_left) * sizeof (*line));
		}

		bool use_acs_border = terminal_emulator_correct_wcwidth ();

		// Now actually commit to ncurses
		cchar_t *p = (cchar_t *)memset (cchar_line, 0, (width+1) * sizeof (cchar_t));
		for (unsigned x = 0; x < width; ++x) {
			if (line[x].c == L'\0')
				continue;
			switch (line[x].c) {
				case BORDER_V:
					if (use_acs_border)
						*p = *WACS_VLINE;
					else
						p->chars[0] = L'|';
					break;
				case BORDER_H:
					if (use_acs_border)
						*p = *WACS_HLINE;
					else
						p->chars[0] = L'-';
					break;
				case BORDER_1:
					if (use_acs_border)
						*p = *WACS_ULCORNER;
					else
						p->chars[0] = L'/';
					break;
				case BORDER_2:
					if (use_acs_border)
						*p = *WACS_URCORNER;
					else
						p->chars[0] = L'\\';
					break;
				case BORDER_3:
					if (use_acs_border)
						*p = *WACS_LLCORNER;
					else
						p->chars[0] = L'\\';
					break;
				case BORDER_4:
					if (use_acs_border)
						*p = *WACS_LRCORNER;
					else
						p->chars[0] = L'/';
					break;
				default:
					p->chars[0] = line[x].c;
			}
			p->attr = internal_attributes (line[x].a.fore, line[x].a.back, line[x].a.attr);
			p++;
		}
		mvadd_wchstr (y, 0, cchar_line);
	}

	// Cursor should be placed where the topmost window wants it to be
	// And the visibility should be decided by the topmost window
	if (!window_list.empty ()) {
		Window *win = window_list.back ();
		Size where = win->get_pos () + win->get_cursor_pos ();
		move (where.y, where.x);
		curs_set (win->get_cursor_visibility () ? 2 : 0);
	}

	// Now actually commit to screen
	refresh ();

	delete [] cchar_line;
	delete [] line;
}


unsigned dig1num (char c)
{
	unsigned a = (unsigned char)c;
	if ((a -= '0') < 10)
		return a;
	return 0;
}

unsigned dig1num (wchar_t c)
{
	unsigned a = c;
	if ((a -= L'0') < 10)
		return a;
	return 0;
}

unsigned hex2num (const char *s)
{
	return (hex_to_num (s[0]) * 16 + hex_to_num (s[1]));
}

unsigned hex2num (const wchar_t *s)
{
	return (hex_to_num (s[0]) * 16 + hex_to_num (s[1]));
}

/*
 * Does not handle Alt + Letter
 */
wchar_t get_input_base (MouseEvent *pmouse_event, bool block)
{
	wint_t c;
	if (!block)
		nodelay (stdscr, TRUE);
	int getret = get_wch (&c);
	if (!block)
		nodelay (stdscr, FALSE);
	switch (getret) {
		case OK: // Normal key
			return c;
		case KEY_CODE_YES: // Special key
#if defined TIARY_USE_MOUSE && defined KEY_MOUSE
			if (c == KEY_MOUSE) {
				MEVENT nc_mouse_event;
				if (getmouse (&nc_mouse_event) != OK)
					return L'\0';
				if (pmouse_event) {
					pmouse_event->p.y = nc_mouse_event.y;
					pmouse_event->p.x = nc_mouse_event.x;
					pmouse_event->m = mousemask_from_internal (nc_mouse_event.bstate);
				}
				return MOUSE;
			}
#endif // TIARY_USE_MOUSE && KEY_MOUSE

			// Other keys - look into the table
			static const MapStruct<wint_t,wchar_t> map [] = {
				{ KEY_UP, UP }, { KEY_DOWN, DOWN }, { KEY_LEFT, LEFT }, { KEY_RIGHT, RIGHT },
				{ KEY_HOME, HOME }, { KEY_END, END },
				{ KEY_NPAGE, PAGEDOWN }, { KEY_PPAGE, PAGEUP },
				{ KEY_BACKSPACE, BACKSPACE1 }, { KEY_DC, DELETE },
				{ KEY_IC, INSERT },
				{ KEY_BTAB, BACKTAB },
				{ KEY_F(1), F1 }, { KEY_F(2), F2 }, { KEY_F(3), F3 }, { KEY_F(4), F4 }, { KEY_F(5), F5 },
				{ KEY_F(6), F6 }, { KEY_F(7), F7 }, { KEY_F(8), F8 }, { KEY_F(9), F9 }, { KEY_F(10), F10 },
				{ KEY_F(11), F11 }, { KEY_F(12), F12 }, { KEY_RESIZE, WINCH }
			};
			return linear_transform (map, array_end (map), c, L'\0');
		default: // Something wrong
			return L'\0';
	}
}

typedef std::stack <std::pair <wchar_t, MouseEvent> > UnGetStack;
UnGetStack stk_unget;

void unget_input (wchar_t c, MouseEvent mouse_event)
{
	stk_unget.push (std::make_pair (c, mouse_event));
}

/*
 * Flushes output and handles Alt + Letter
 *
 * Alt+<letter> is returned as <ESC> <Letter>
 * (at least on Linux x86/amd64)
 * Try to distinguish this from two separate keystrokes
 */
wchar_t get_input (MouseEvent *pmouse_event, bool block = true)
{
	if (!stk_unget.empty ()) {
		wchar_t c = stk_unget.top ().first;
		if (c == MOUSE)
			*pmouse_event = stk_unget.top ().second;
		stk_unget.pop ();
		return c;
	}
	commit_touched_lines ();
	wchar_t c = get_input_base (pmouse_event, block);
	if (c == ESCAPE) {
		c = get_input_base (pmouse_event, false);
		// If it's a letter, we interpret it as Alt + Letter
		if (unsigned (c - L'A') < 26 || unsigned (c - L'a') < 26)
			c += ALT_BASE;
		else {
			// Otherwise, still returns ESCAPE. If we have read another key, unget it
			if (c)
				unget_input (c, *pmouse_event);
			c = ESCAPE;
		}
	}
	return c;
}

}


Window::Window (Size pos_, Size size_)
	: requests (0)
	, pos (pos_)
	, size (size_)
	, curpos (make_size (0, 0))
	, cur_attr (ColorAttr::make_default ())
	, cursor_visible (true)
	, char_table (0)
{
	reallocate_char_table ();
	touch_lines (pos_.y, size_.y);
	window_list.push_back (this);
}

Window::~Window ()
{
	touch_lines (pos.y, size.y);

	deallocate_char_table ();

	/**
	 * Take care! The following line does NOT work...
	 *
	 * std::remove (window_list.begin (), window_list.end (), this);
	 */
	remove_first (window_list, this);
}

void Window::reallocate_char_table ()
{
	deallocate_char_table ();
	unsigned width = size.x;
	unsigned height = size.y;
	if (width && height) {
		// If we allow height==0 and allocate, we will have problem at deallocate_char_table
		char_table = new CharColorAttr * [height];
		CharColorAttr *ptr = new CharColorAttr [height * width];
		CharColorAttr val;
		val.c = L' ';
		val.a = cur_attr;
		std::fill_n (ptr, height*width, val);
		for (unsigned i=0; i<height; ++i) {
			char_table[i] = ptr;
			ptr += width;
		}
	}
}

void Window::deallocate_char_table ()
{
	if (char_table) {
		delete [] char_table[0];
		delete [] char_table;
		char_table = 0;
	}
}

// Default behavior of interface functions
bool Window::on_key (wchar_t)
{
	return false;
}

bool Window::on_mouse (MouseEvent)
{
	return false;
}

bool Window::on_mouse_outside (MouseEvent)
{
	return false;
}

void Window::event_loop ()
{
	while (!(requests & REQUEST_CLOSE)) {
		MouseEvent mouse_event;

		wchar_t c;
		c = get (&mouse_event);

		// WINCH and MOUSE should be treated specially

		if (c == WINCH) {
			touch_lines ();
			// Signal every window. So that background windows can also redraw themselves.
			for (WindowList::iterator it = window_list.begin (); it != window_list.end (); ++it)
				(*it)->on_winch ();
		} else if (c == MOUSE) {
			// Is the position within this window?
			MouseEvent mouse_event_relative = mouse_event;
			mouse_event_relative.p -= pos;
			if (both (mouse_event_relative.p < size))
				on_mouse (mouse_event_relative);
			else
				on_mouse_outside (mouse_event);
		} else { // Normal keyboard inputs
			on_key (c);
		}

	}
}







void Window::choose_palette (PaletteID id)
{
	if (id < NUMBER_PALETTES)
		cur_attr = get_palette (id);
}

void Window::choose_fore_color (Color fore)
{
	if (fore < NOCOLOR)
		cur_attr.fore = fore;
}

void Window::choose_back_color (Color back)
{
	if (back < NOCOLOR)
		cur_attr.back = back;
}

void Window::choose_color (Color fore, Color back)
{
	choose_fore_color (fore);
	choose_back_color (back);
}

void Window::attribute_on (Attr attr)
{
	cur_attr.attr |= attr;
}

void Window::attribute_off (Attr attr)
{
	cur_attr.attr &= ~attr;
}

void Window::set_attr (const ColorAttr &at)
{
	cur_attr = at;
}

void Window::move_cursor (Size newpos)
{
	curpos = newpos;
}


Size Window::put (Size blkpos, Size blksize, Size relpos, char ch)
{
	return put (blkpos, blksize, relpos, wchar_t (ch));
}

Size Window::put (Size blkpos, Size blksize, Size relpos, wchar_t ch)
{
	if (either (blkpos >= size))
		return relpos;
	if (either (blkpos + blksize > size))
		return relpos;
	if (either (relpos >= blksize))
		return relpos;

//	unsigned left = pos.x + blkpos.x;
	unsigned top  = pos.y + blkpos.y;
	unsigned x = relpos.x;
	unsigned y = relpos.y;

	unsigned w;

	switch (ch) {
		case L'\b':
			if (x)
				--x;
			break;
		case L'\n':
			x = 0;
			if (y+1 < blksize.y)
				++y;
			break;
		case L'\r':
			x = 0;
			break;
		case L'\t':
			x = minU (blksize.x, (x + 8) & ~unsigned(7));
			break;
		default:
			if (!special_printable(ch) && !iswprint (ch))
				break;
			w = ucs_width (ch);
			if (x + w > blksize.x)
				break;
			touch_line (top + y);

			// Position relative to window
			unsigned winx = blkpos.x + x;
			unsigned winy = blkpos.y + y;
			CharColorAttr *line = char_table[winy];

			if (winx && line[winx].c == L'\0')
				line[winx-1].c = L' ';

			line[winx].c = ch;
			line[winx].a = cur_attr;
			++x;
			++winx;
			if (w == 2) {
				line[winx].c = L'\0';
				line[winx].a = cur_attr;
				++x;
				++winx;
			}
			if (winx<size.x && line[winx].c==L'\0')
				line[winx].c = L' ';
	}
	return make_size (x, y);
}

void Window::move_resize (Size newpos, Size newsize)
{
	unsigned touch_begin = minU (pos.y, newpos.y);
	unsigned touch_height = maxU (pos.y + size.y, newpos.y + newsize.y) - touch_begin;
	pos = newpos;
	size = newsize;
	touch_lines (touch_begin, touch_height);
	reallocate_char_table ();
}

void Window::clear ()
{
	CharColorAttr val;
	val.c = L' ';
	val.a = cur_attr;
	std::fill_n (char_table[0], size.x*size.y, val);
	touch_lines (pos.y, size.y);
}

void Window::clear (Size fill_pos, Size fill_size)
{
	fill (fill_pos, fill_size, L' ');
}

void Window::fill (Size top_left, Size fill_size, wchar_t ch)
{
	// FIXME: Deal with full-width characters properly
	unsigned fill_top = top_left.y;
	unsigned fill_left = top_left.x;
	unsigned fill_width = fill_size.x;
	unsigned fill_height = fill_size.y;
	unsigned width = size.x;
	unsigned height = size.y;

	if (fill_left>=width || fill_top>=height)
		return;

	fill_width = minU (fill_width, width - fill_left);
	fill_height = minU (fill_height, height - fill_top);

	touch_lines (pos.y + fill_top, fill_height);

	CharColorAttr val;
	val.a = cur_attr;
	val.c = ch;
	CharColorAttr *ptr_left = char_table[fill_top] + fill_left;
	for (unsigned y = 0; y < fill_height; ++y) {
		std::fill_n (ptr_left, fill_width, val);
		ptr_left += width;
	}
}

Size Window::put (Size blkpos, Size blksize, Size relpos, const char *s, size_t n)
{
	// This function supports multi-byte characters
	if (n == size_t(-1))
		n = strlen (s);
	const char *end = s + n;
	mbstate_t state;
	memset (&state, 0, sizeof state);
	while (s < end) {
		if ((uint8_t)*s < 0x80)
			relpos = put (blkpos, blksize, relpos, wchar_t (*s++));
		else {
			// Convert to Unicode
			wchar_t wc;
			ssize_t r = mbrtowc (&wc, s, end-s, &state);
			if (r <= 0) {
				if (size_t (r) == size_t (-2))
					break;
				memset (&state, 0, sizeof state);
				++s;
			} else {
				s += r;
				relpos = put (blkpos, blksize, relpos, wc);
			}
		}
	}
	return relpos;
}

Size Window::put (Size blkpos, Size blksize, Size relpos, const wchar_t *s, size_t n)
{
	if (n == size_t(-1))
		n = wcslen (s);
	const wchar_t *end = s + n;
	while (s < end)
			relpos = put (blkpos, blksize, relpos, *s++);
	return relpos;
}

Size Window::put (Size blkpos, Size blksize, Size relpos, const std::string &s)
{
	return put (blkpos, blksize, relpos, s.data(), s.length ());
}

Size Window::put (Size blkpos, Size blksize, Size relpos, const std::wstring &s)
{
	return put (blkpos, blksize, relpos, s.data(), s.length ());
}

Size Window::put (Size relpos, char ch)
{
	return put (make_size (), size, relpos, ch);
}

Size Window::put (Size relpos, wchar_t ch)
{
	return put (make_size (), size, relpos, ch);
}

Size Window::put (Size relpos, const char *s, size_t n)
{
	return put (make_size (), size, relpos, s, n);
}

Size Window::put (Size relpos, const wchar_t *s, size_t n)
{
	return put (make_size (), size, relpos, s, n);
}

Size Window::put (Size relpos, const std::string &s)
{
	return put (make_size (), size, relpos, s);
}

Size Window::put (Size relpos, const std::wstring & s)
{
	return put (make_size (), size, relpos, s);
}

void Window::put (char c)
{
	curpos = put (curpos, c);
}

void Window::put (wchar_t c)
{
	curpos = put (curpos, c);
}

void Window::put (const char *s, size_t n)
{
	curpos = put (curpos, s, n);
}

void Window::put (const wchar_t *s, size_t n)
{
	curpos = put (curpos, s, n);
}

void Window::put (const std::string &s)
{
	curpos = put (curpos, s);
}

void Window::put (const std::wstring &s)
{
	curpos = put (curpos, s);
}

void Window::touch_screen ()
{
	touch_lines ();
	clearok (stdscr, 1);
}

wchar_t Window::get (MouseEvent *pmouse_event)
{
	return get_input (pmouse_event);
}

wchar_t Window::get_noblock (MouseEvent *pmouse_event)
{
	return get_input (pmouse_event, false);
}

void Window::unget (wchar_t c)
{
	unget_input (c, MouseEvent ());
}

void Window::unget (MouseEvent mouse_event)
{
	unget_input (MOUSE, mouse_event);
}

void Window::suspend ()
{
	finalize ();
}

bool Window::resume ()
{
	if (init ()) {
		unget (WINCH);
		touch_screen ();
		return true;
	} else {
		return false;
	}
}

void Window::request (unsigned req)
{
	requests |= req;
}

void Window::request_close ()
{
	request (REQUEST_CLOSE);
}


} // namespace tiary::ui
} // namespace tiary
