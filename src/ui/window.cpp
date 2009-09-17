// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#include "ui/ncurses_common.h"
#include "ui/window.h"
#include "common/containers.h"
#include "common/unicode.h"
#include "common/misc.h"
#include "ui/terminal_emulator.h"
#include "ui/control.h"
#include "ui/paletteid.h"
#include <vector>
#include <stack>
#include <string.h>
#include <assert.h>
#include <signal.h>


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
}

void touch_lines (unsigned top, unsigned height)
{
	unsigned lines = touched_lines.size ();
	if (top >= lines) {
		return;
	}
	height = minU (height, lines - top);
	std::fill_n (touched_lines.begin () + top, height, '\1');
}

void touch_line (unsigned line)
{
	if (line < touched_lines.size ()) {
		touched_lines[line] = '\1';
	}
}

void commit_touched_lines ()
{
	static Size last_commit_size = { 0, 0 };

	Size size = get_screen_size ();
	unsigned width = size.x;
	unsigned height = size.y;

	if (last_commit_size != size) {
		last_commit_size = size;
		std::fill (touched_lines.begin (), touched_lines.end (), '\1');
		touched_lines.resize (height, '\1');
	}

	CharColorAttr *line = new CharColorAttr[width];
	cchar_t *cchar_line = new cchar_t[width+1];
	CharColorAttr def = { L' ', { DEFAULT_FORECOLOR, DEFAULT_BACKCOLOR, 0 } };

	for (unsigned y = 0; y < height; ++y) {
		if (!touched_lines[y]) {
			continue;
		}
		touched_lines[y] = 0;

		// Commit the y-th line
		std::fill_n (line, width, def);

		for (WindowList::const_iterator it = window_list.begin (); it != window_list.end (); ++it) {
			const Window *win = *it;
			if (y - win->get_pos().y >= win->get_size().y) {
				continue;
			}
			unsigned win_left = win->get_pos().x;
			if (win_left >= width) {
				continue;
			}
			unsigned win_right = minU (width, win->get_size().x + win_left);

			if (win_left && line[win_left].c == L'\0') {
				line[win_left-1].c = L' ';
			}
			if (win_right<width && line[win_right].c == L'\0') {
				line[win_right].c = L' ';
			}

			memcpy (line+win_left, win->get_char_table (y - win->get_pos().y),
					(win_right - win_left) * sizeof (*line));
		}

		bool use_acs_border = terminal_emulator_correct_wcwidth ();

		// Now actually commit to ncurses
		cchar_t *p = (cchar_t *)memset (cchar_line, 0, (width+1) * sizeof (cchar_t));
		for (unsigned x = 0; x < width; ++x) {
			if (line[x].c == L'\0') {
				continue;
			}
			switch (line[x].c) {
				case BORDER_V:
					if (use_acs_border) {
						*p = *WACS_VLINE;
					}
					else {
						p->chars[0] = L'|';
					}
					break;
				case BORDER_H:
					if (use_acs_border) {
						*p = *WACS_HLINE;
					}
					else {
						p->chars[0] = L'-';
					}
					break;
				case BORDER_1:
					if (use_acs_border) {
						*p = *WACS_ULCORNER;
					}
					else {
						p->chars[0] = L'/';
					}
					break;
				case BORDER_2:
					if (use_acs_border) {
						*p = *WACS_URCORNER;
					}
					else {
						p->chars[0] = L'\\';
					}
					break;
				case BORDER_3:
					if (use_acs_border) {
						*p = *WACS_LLCORNER;
					}
					else {
						p->chars[0] = L'\\';
					}
					break;
				case BORDER_4:
					if (use_acs_border) {
						*p = *WACS_LRCORNER;
					}
					else {
						p->chars[0] = L'/';
					}
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


/*
 * Does not handle Alt + Letter
 */
wchar_t get_input_base (MouseEvent *pmouse_event, bool block)
{
	wint_t c;
	if (!block) {
		nodelay (stdscr, TRUE);
	}
	int getret = get_wch (&c);
	if (!block) {
		nodelay (stdscr, FALSE);
	}
	switch (getret) {
		case OK: // Normal key
			return c;
		case KEY_CODE_YES: // Special key
#if defined TIARY_USE_MOUSE && defined KEY_MOUSE
			if (c == KEY_MOUSE) {
				MEVENT nc_mouse_event;
				if (getmouse (&nc_mouse_event) != OK) {
					return L'\0';
				}
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

// There is usually no or just one element in the stack.
// Not necessary to use the complicated deque.
// Vector is enough
typedef std::stack <std::pair <wchar_t, MouseEvent>, std::vector <std::pair <wchar_t, MouseEvent> > > UnGetStack;
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
		if (c == MOUSE) {
			*pmouse_event = stk_unget.top ().second;
		}
		stk_unget.pop ();
		return c;
	}
	commit_touched_lines ();
	wchar_t c = get_input_base (pmouse_event, block);
	if (c == ESCAPE) {
		c = get_input_base (pmouse_event, false);
		// If it's a letter, we interpret it as Alt + Letter
		if (unsigned (c - L'A') < 26 || unsigned (c - L'a') < 26) {
			c += ALT_BASE;
		}
		else {
			// Otherwise, still returns ESCAPE. If we have read another key, unget it
			if (c) {
				unget_input (c, *pmouse_event);
			}
			c = ESCAPE;
		}
	}
	return c;
}


const unsigned REQUEST_CLOSE = 1;

} // anonymous namespace


Window::Window (unsigned options_, const std::wstring &title_)
	: MovableObject ()
	, Hotkeys ()
	, requests (0)
	, cur_attr (ColorAttr::make_default ())
	, char_table (0)
	, options (options_)
	, title (title_, UIStringBase::NO_HOTKEY)
	, control_list ()
	, focus_id (-1)
{
	reallocate_char_table ();
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

void Window::on_winch ()
{
	redraw ();
}

bool Window::on_mouse_outside (MouseEvent)
{
	return false;
}

void Window::event_loop ()
{
	if (size_t (focus_id) >= control_list.size ())
		set_focus_id (0, 1);

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


Size Window::put (Size blkpos, Size blksize, Size relpos, wchar_t ch)
{
	return put (blkpos, blksize, relpos, &ch, 1);
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

Size Window::put (Size blkpos, Size blksize, Size relpos, const wchar_t *s)
{
	return put (blkpos, blksize, relpos, s, wcslen (s));
}

Size Window::put (Size blkpos, Size blksize, Size relpos, const wchar_t *s, size_t n)
{
	if (either (blkpos >= size))
		return relpos;
	if (either (blkpos + blksize > size))
		return relpos;
	if (either (relpos >= blksize))
		return relpos;

	unsigned x = relpos.x;
	unsigned y = relpos.y;

	// Position relative to window
	unsigned winx = blkpos.x + x;
	unsigned winy = blkpos.y + y;

	touch_line (pos.y + winy);
	CharColorAttr *ptr = char_table[winy] + winx;

#if 0 // Caller's responsibility
	if (winx && ptr->c == L'\0')
		ptr[-1].c = L' ';
#endif

	for (; n; --n) {

		wchar_t ch = *s++;
		// Only handle one special character: '\t'
		if (ch == L'\t') {
			unsigned w = minU ((x + 8) & ~7u, blksize.x) - x;
			x += w;
			winx += w;
			for (; w; --w) {
				ptr->c = L' ';
				ptr->a = cur_attr;
				++ptr;
			}
			continue;
		}
		unsigned w = ucs_width (ch);

		if (x + w > blksize.x)
			break;

		ptr->c = ch;
		ptr->a = cur_attr;
		++ptr;
		++x;
		++winx;
		if (w == 2) {
			ptr->c = L'\0';
			ptr->a = cur_attr;
			++ptr;
			++x;
			++winx;
		}
	}
#if 0 // Caller's responsibility
	if (winx<size.x && ptr->c==L'\0')
		ptr->c = L' ';
#endif
	return make_size (x, y);
}

Size Window::put (Size blkpos, Size blksize, Size relpos, const std::wstring &s)
{
	return put (blkpos, blksize, relpos, s.data(), s.length ());
}

Size Window::put (Size relpos, wchar_t ch)
{
	return put (make_size (), size, relpos, ch);
}

Size Window::put (Size relpos, const wchar_t *s)
{
	return put (make_size (), size, relpos, s);
}

Size Window::put (Size relpos, const wchar_t *s, size_t n)
{
	return put (make_size (), size, relpos, s, n);
}

Size Window::put (Size relpos, const std::wstring & s)
{
	return put (make_size (), size, relpos, s);
}

void Window::touch_screen ()
{
	touch_lines ();
	clearok (stdscr, 1);
}

void Window::touch_windows ()
{
	for (WindowList::iterator it = window_list.begin (); it != window_list.end (); ++it)
		(*it)->redraw ();
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

#ifdef TIARY_USE_MOUSE
namespace {
bool suspend_mouse_status;
} // anonymous namespace
#endif

void Window::suspend ()
{
#ifdef TIARY_USE_MOUSE
	suspend_mouse_status = get_mouse_status ();
#endif
	finalize ();
}

bool Window::resume ()
{
	if (init ()) {
#ifdef TIARY_USE_MOUSE
		set_mouse_status (suspend_mouse_status);
#endif
		unget (WINCH);
		touch_screen ();
		return true;
	} else {
		return false;
	}
}

void Window::request_close ()
{
	requests |= REQUEST_CLOSE;
}

void Window::add_control (Control *ctrl)
{
	assert (&ctrl->win == this);

	control_list.push_back (ctrl);
}

bool Window::set_focus_id (unsigned id, int fall_direction)
{
	if (id >= control_list.size ())
		return false;
	int old_focus = focus_id;
	if (old_focus == int (id))
		return true;
	if (old_focus >= 0) {
		focus_id = -1;
		control_list[old_focus]->on_defocus ();
	}
	unsigned N = control_list.size ();
	unsigned fall_add = N + fall_direction;
	unsigned try_id = id;
	for (;;) {
		focus_id = try_id;
		Control *try_ctrl = control_list[try_id];
		if (try_ctrl->on_focus ()) {
			// Notify every control
			for (ControlList::iterator it = control_list.begin ();
					it != control_list.end (); ++it)
				(*it)->on_focus_changed ();
			// Emit two signals
			if (old_focus >= 0)
				control_list[old_focus]->sig_defocus.emit ();
			try_ctrl->sig_focus.emit ();
			return true;
		}
		try_id = (try_id + fall_add) % N;
		if (try_id == id) {
			// Completely failed. Try refocusing the old focus
			focus_id = old_focus;
			if (old_focus >= 0)
				control_list[old_focus]->on_focus ();
			return false;
		}
	}
}

bool Window::set_focus_ptr (Control *ctrl, int fall_direction)
{
	ControlList::iterator it = std::find (control_list.begin (), control_list.end (), ctrl);
	if (it == control_list.end ())
		return false;
	return set_focus_id (it - control_list.begin (), fall_direction);
}

bool Window::set_focus_next (bool keep_trying)
{
	unsigned id = focus_id + 1;
	if (id>=control_list.size())
		id = 0;
	return set_focus_id (id, keep_trying ? 1 : 0);
}

bool Window::set_focus_prev (bool keep_trying)
{
	unsigned id = focus_id - 1;
	if (id >= control_list.size())
		id = control_list.size() - 1;
	return set_focus_id (id, keep_trying ? -1 : 0);
}

bool Window::set_focus_direction (Control *Control::*dir)
{
	unsigned id = focus_id;
	if (id < control_list.size ()) {
		if (Control *new_focus = control_list[id]->*dir)
			return set_focus_ptr (new_focus, 0);
	}
	return false;
}

Control *Window::get_focus () const
{
	unsigned id = focus_id;
	return (id >= control_list.size ()) ? 0 : control_list[id];
}

bool Window::on_mouse (MouseEvent mouse_event)
{
	bool processed = false;

	// First decide which control this event happens on
	for (ControlList::iterator it = control_list.begin (); it != control_list.end (); ++it) {
		if (both (mouse_event.p - (*it)->pos < (*it)->size)) {
			// If it's any mouse key event, we should first try to focus it
			// Otherwise, we do not.
			if (mouse_event.m & MOUSE_ALL_BUTTON)
				set_focus (it - control_list.begin (), 0); // Regardless whether it's successful or not
			mouse_event.p -= (*it)->pos;
			bool processed = (*it)->on_mouse (mouse_event);
			if (!processed) {
				if ((mouse_event.m&LEFT_CLICK) && (*it)->sig_clicked.is_really_connected ()) {
					(*it)->sig_clicked.emit ();
					processed = true;
				}
			}
			break;
		}
	}

	// Clicked close button?
	if (!processed) {
		if ((mouse_event.m & LEFT_CLICK) && (mouse_event.p.y == 0) &&
				(get_size ().x - 2 - mouse_event.p.x < 3))
			processed = on_key (ESCAPE);
	}

	return processed;
}

bool Window::on_key (wchar_t c)
{
	bool processed = false;
	// First try the focused control
	if (unsigned (focus_id) < control_list.size ()) {
		processed = control_list[focus_id]->on_key (c);
		// Not processed? How about local hotkeys?
		if (!processed)
			processed = control_list[focus_id]->emit_hotkey (c);
	}
	// Not processed? How about dialog hotkeys?
	if (!processed)
		processed = emit_hotkey (c);
	// Still not processed? Interpret it ourselves
	if (!processed) {
		// Currently supports '\t', BACKTAB and arrow keys
		switch (c) {
			case L'\t':
				processed = set_focus_next (true);
				break;
			case BACKTAB:
				processed = set_focus_prev (true);
				break;
			case UP:
				processed = set_focus_direction (&Control::ctrl_up);
				break;
			case DOWN:
				processed = set_focus_direction (&Control::ctrl_down);
				break;
			case LEFT:
				processed = set_focus_direction (&Control::ctrl_left);
				break;
			case RIGHT:
				processed = set_focus_direction (&Control::ctrl_right);
				break;
			case CTRL_Z:
				Window::suspend ();
				raise (SIGTSTP);
				Window::resume ();
				// Fall into ...
			case CTRL_L:
				Window::touch_screen ();
				processed = true;
				break;
			case F12:
				toggle_mouse_status ();
				processed = true;
				break;
		}
	}
	return processed;
}

void Window::redraw ()
{
	choose_palette (PALETTE_ID_BACKGROUND);
	clear ();

	Size size = get_size ();
	// Draw the border
	if (!(options & WINDOW_NO_BORDER) && both (size >= make_size (2, 2))) {

		put (make_size (), BORDER_1); // Top, left
		fill (make_size (1, 0), make_size (size.x-2, 1), BORDER_H); // Up
		put (make_size (size.x-1, 0), BORDER_2); // Top, right
		fill (make_size (0, 1), make_size (1, size.y-2), BORDER_V); // Left
		fill (make_size (size.x-1, 1), make_size (1, size.y-2), BORDER_V); // Right
		put (make_size (0, size.y-1), BORDER_3); // Bottom, left
		fill (make_size (1, size.y-1), make_size (size.x-2, 1), BORDER_H); // Down
		put (make_size (size.x-1, size.y-1), BORDER_4); // Bottom, right

		// Show the title
		if (!title.get_text().empty ()) {
			unsigned title_width = minU (title.get_width (), size.x);
			unsigned left = (size.x - title_width - 2) / 2;
			Size pos = make_size (left, 0);
			pos = put (pos, L' ');
			pos = title.output (*this, pos, title_width);
			pos = put (pos, L' ');
		}

		// Show the close button
		if (!(options & WINDOW_NO_CLOSE_BUTTON) && size.x>=4) {
			Size pos = make_size (size.x - 4, 0);
			// u00d7 is multiplication sign
			put (pos, terminal_emulator_correct_wcwidth () ? L"[\u00d7]" : L"[x]");
		}
	}

	for (ControlList::iterator it = control_list.begin ();
			it != control_list.end (); ++it)
		(*it)->redraw ();
}

Size Window::get_cursor_pos () const
{
	if (Control *ctrl = get_focus ())
		return ctrl->get_cursor_pos () + ctrl->get_pos ();
	return make_size ();
}

bool Window::get_cursor_visibility () const
{
	if (Control *ctrl = get_focus ())
		return ctrl->get_cursor_visibility ();
	return false;
}

} // namespace tiary::ui
} // namespace tiary
