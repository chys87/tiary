// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2018, 2019, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#include "ui/ncurses_common.h"
#include "ui/window.h"
#include "common/containers.h"
#include "common/unicode.h"
#include "common/algorithm.h"
#include "ui/terminal_emulator.h"
#include "ui/control.h"
#include "ui/paletteid.h"
#include <vector>
#include <string.h>
#include <assert.h>
#include <signal.h>


namespace tiary {
namespace ui {





namespace {

// Not using std::vector<bool> because it's specialized by the standard
// I don't really want that little efficiency in space, compared to the overhead
std::vector<uint8_t> touched_lines; // 1 = Touched; 0 = Not

// Touches the whole screen
void touch_all_lines() {
	std::fill(touched_lines.begin(), touched_lines.end(), 1);
}

void touch_lines (unsigned top, unsigned height)
{
	unsigned lines = touched_lines.size ();
	if (top >= lines) {
		return;
	}
	height = minU (height, lines - top);
	std::fill_n (touched_lines.begin () + top, height, 1);
}

void touch_line (unsigned line)
{
	if (line < touched_lines.size ()) {
		touched_lines[line] = 1;
	}
}

void commit_to_screen() {
	static Size last_commit_size = { 0, 0 };

	Size size = get_screen_size ();
	unsigned width = size.x;
	unsigned height = size.y;

	if (last_commit_size != size) {
		last_commit_size = size;
		std::fill_n(touched_lines.data(), minU(touched_lines.size(), height), 1);
		touched_lines.resize(height, 1);
	} else {
		if (std::none_of(touched_lines.begin(), touched_lines.end(), [](char c) { return c; })) {
			// Even if no line is touched, we need to at least make sure the cursor is at the right position
			if (Window *win = Window::get_topmost_window()) {
				Size where = win->get_pos () + win->get_cursor_pos ();
				move(where.y, where.x);
				curs_set(win->get_cursor_visibility() ? 2 : 0);
				refresh();
			}
			return;
		}
	}

	std::unique_ptr<CharColorAttr[]> line{new CharColorAttr[width]};
	std::unique_ptr<cchar_t[]> cchar_line{new cchar_t[width + 1]};
	CharColorAttr def = { L' ', { DEFAULT_FORECOLOR, DEFAULT_BACKCOLOR, 0 } };

	bool use_acs_border = terminal_emulator_correct_wcwidth ();

	for (unsigned y = 0; y < height; ++y) {
		if (!touched_lines[y]) {
			continue;
		}
		touched_lines[y] = 0;

		// Commit the y-th line
		std::fill_n(line.get(), width, def);

		for (const Window *win = Window::get_bottommost_window ();
				win; win = win->get_top_window ()) {
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

			memcpy(line.get() + win_left, win->get_char_table (y - win->get_pos().y),
					(win_right - win_left) * sizeof(line[0]));
		}

		// Now actually commit to ncurses
		cchar_t *p = (cchar_t *)memset(cchar_line.get(), 0, (width + 1) * sizeof(cchar_t));
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
		mvadd_wchstr(y, 0, cchar_line.get());
	}

	// Cursor should be placed where the topmost window wants it to be
	// And the visibility should be decided by the topmost window
	if (Window *win = Window::get_topmost_window ()) {
		Size where = win->get_pos () + win->get_cursor_pos ();
		move (where.y, where.x);
		curs_set (win->get_cursor_visibility () ? 2 : 0);
	}

	// Now actually commit to screen
	refresh ();
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
			static const std::pair<wint_t,wchar_t> map[] = {
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
struct UnGetStackItem
{
	wchar_t key;
	MouseEvent mouse_event;
};
const size_t MAX_UNGETS = 8;
UnGetStackItem stk_unget [MAX_UNGETS] = {};
UnGetStackItem *stk_unget_top = stk_unget;

void unget_input (wchar_t c, MouseEvent mouse_event)
{
	if (stk_unget_top < array_end (stk_unget)) {
		stk_unget_top->key = c;
		stk_unget_top->mouse_event = mouse_event;
		++stk_unget_top;
	}
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
	if (stk_unget_top != stk_unget) {
		--stk_unget_top;
		wchar_t c = stk_unget_top->key;
		if (c == MOUSE) {
			*pmouse_event = stk_unget_top->mouse_event;
		}
		return c;
	}
	commit_to_screen();
	wchar_t c = get_input_base (pmouse_event, block);
	if (c == ESCAPE) {
		c = get_input_base (pmouse_event, false);
		// If it's a letter, we interpret it as Alt + Letter
		if ((c >= L'A' && c <= L'Z') || (c >= L'a' && c <= L'z')) {
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


const uint8_t REQUEST_CLOSE = 1;

} // anonymous namespace

Window::DummyControl::~DummyControl ()
{
}

bool Window::DummyControl::on_focus ()
{
	return false;
}

void Window::DummyControl::redraw ()
{
}

Window *Window::topmost_window = 0;
Window *Window::bottommost_window = 0;

Window::Window(unsigned options, std::wstring_view title)
	: MovableObject ()
	, hotkeys_()
	, requests_(0)
	, cur_attr()
	, status_(STATUS_NORMAL)
	, options_(options)
	, title_(title)
	, title_scr_width_(ucs_width(title_))
	, dummy_ctrl_(*this)
{
	bottom_window_ = topmost_window;
	if (topmost_window == 0) {
		bottommost_window = this;
	}
	else {
		topmost_window->top_window_ = this;
	}
	topmost_window = this;
}

Window::Window (unsigned options, std::wstring &&title)
	: MovableObject()
	, hotkeys_()
	, requests_(0)
	, cur_attr()
	, status_(STATUS_NORMAL)
	, options_(options)
	, title_(std::move(title))
	, title_scr_width_(ucs_width(title_))
	, dummy_ctrl_(*this) {
	bottom_window_ = topmost_window;
	if (topmost_window == 0) {
		bottommost_window = this;
	} else {
		topmost_window->top_window_ = this;
	}
	topmost_window = this;
}

Window::~Window ()
{
	touch_lines(get_pos().y, get_size().y);

	if (bottom_window_) {
		bottom_window_->top_window_ = top_window_;
	}
	else {
		bottommost_window = top_window_;
	}

	if (top_window_) {
		top_window_->bottom_window_ = bottom_window_;
	}
	else {
		topmost_window = bottom_window_;
	}
}

void Window::reallocate_char_table ()
{
	unsigned width = get_size().x;
	unsigned height = get_size().y;
	unsigned count = width * height;

	char_table_.resize(count);
	std::fill_n(char_table_.data(), count, CharColorAttr{L' ', cur_attr});

}

void Window::on_winch ()
{
	redraw ();
}

bool Window::on_mouse_outside (MouseEvent)
{
	return false;
}

void Window::on_ready ()
{
}

void Window::on_focus_changed ()
{
}

void Window::event_loop ()
{
	if (!focus_ctrl_) {
		set_focus_ptr(dummy_ctrl_.next, 1);
	}

	redraw();

	while (!(requests_ & REQUEST_CLOSE)) {

		on_ready ();

		MouseEvent mouse_event;
		wchar_t c;
		c = get (&mouse_event);

		if (status_ == STATUS_MOVING) {
			Size pos = get_pos();
			Size size = get_size();
			switch (c) {
				case LEFT:
					if (pos.x) {
						pos.x--;
						set_pos(pos);
						touch_lines (pos.y, size.y);
					}
					break;
				case RIGHT:
					if (pos.x + size.x < get_screen_width ()) {
						pos.x++;
						set_pos(pos);
						touch_lines (pos.y, size.y);
					}
					break;
				case UP:
					if (pos.y) {
						pos.y--;
						set_pos(pos);
						touch_lines (pos.y, size.y+1);
					}
					break;
				case DOWN:
					if (pos.y + size.y < get_screen_height ()) {
						pos.y++;
						set_pos(pos);
						touch_lines (pos.y-1, size.y+1);
					}
					break;
				default:
					status_ = STATUS_NORMAL;
					redraw ();
					break;
			}
		}
		else if (c == WINCH) {
			// WINCH and MOUSE should be treated specially
			touch_all_lines();
			// Signal every window. So that background windows can also redraw themselves.
			for (Window *win = bottommost_window; win; win = win->top_window_) {
				win->on_winch ();
			}
		}
		else if (c == MOUSE) {
			// Is the position within this window?
			MouseEvent mouse_event_relative = mouse_event.rebase(get_pos());
			if (mouse_event_relative.p < get_size()) {
				on_mouse (mouse_event_relative);
			}
			else {
				on_mouse_outside (mouse_event);
			}
		}
		else { // Normal keyboard inputs
			on_key (c);
		}

	}
}







void Window::choose_palette (PaletteID id)
{
	if (id < NUMBER_PALETTES) {
		cur_attr = get_palette (id);
	}
}

void Window::choose_fore_color (Color fore)
{
	if (fore < NOCOLOR) {
		cur_attr.fore = fore;
	}
}

void Window::choose_back_color (Color back)
{
	if (back < NOCOLOR) {
		cur_attr.back = back;
	}
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

void Window::attribute_toggle (Attr attr)
{
	cur_attr.attr ^= attr;
}

void Window::set_attr (ColorAttr at)
{
	cur_attr = at;
}


Size Window::put (Size blkpos, Size blksize, Size relpos, wchar_t ch)
{
	return put (blkpos, blksize, relpos, &ch, 1);
}

void Window::move_resize (Size newpos, Size newsize)
{
	if (newsize == get_size()) {
		if (newpos == get_pos()) {
			return;
		}
		// If the old position still makes the window fit on screen,
		// do not move it.
		// (This is in order to implement user-initiated window moving.)
		if (get_pos() + newsize <= get_screen_size ()) {
			newpos = get_pos();
		}
	}
	unsigned touch_begin = minU(get_pos().y, newpos.y);
	unsigned touch_height = maxU(get_pos().y + get_size().y, newpos.y + newsize.y) - touch_begin;
	set_pos(newpos);
	set_size(newsize);
	touch_lines (touch_begin, touch_height);
	reallocate_char_table ();
}

void Window::clear ()
{
	std::fill_n(char_table_.data(), get_size().x * get_size().y, CharColorAttr{L' ', cur_attr});
	touch_lines(get_pos().y, get_size().y);
}

void Window::clear (Size fill_pos, Size fill_size)
{
	fill (fill_pos, fill_size, L' ');
}

void Window::fill (Size top_left, Size fill_size, wchar_t ch)
{
	// WONTFIX: Deal with full-width characters properly
	// Caller's responsibility
	unsigned fill_top = top_left.y;
	unsigned fill_left = top_left.x;
	unsigned fill_width = fill_size.x;
	unsigned fill_height = fill_size.y;
	unsigned width = get_size().x;
	unsigned height = get_size().y;

	if (fill_left>=width || fill_top>=height) {
		return;
	}

	fill_width = minU (fill_width, width - fill_left);
	fill_height = minU (fill_height, height - fill_top);

	touch_lines(get_pos().y + fill_top, fill_height);

	CharColorAttr val {ch, cur_attr};
	CharColorAttr *ptr_left = get_char_table(fill_top) + fill_left;
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
	if (either (blkpos >= get_size())) {
		return relpos;
	}
	if (either (blkpos + blksize > get_size())) {
		return relpos;
	}
	if (either (relpos >= blksize)) {
		return relpos;
	}

	unsigned x = relpos.x;
	unsigned y = relpos.y;

	// Position relative to window
	unsigned winx = blkpos.x + x;
	unsigned winy = blkpos.y + y;

	touch_line(get_pos().y + winy);
	CharColorAttr *ptr = get_char_table(winy) + winx;

#if 0 // Caller's responsibility
	if (winx && ptr->c == L'\0') {
		ptr[-1].c = L' ';
	}
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

		if (x + w > blksize.x) {
			break;
		}

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
	if (winx<size.x && ptr->c==L'\0') {
		ptr->c = L' ';
	}
#endif
	return {x, y};
}

Size Window::put(Size blkpos, Size blksize, Size relpos, std::wstring_view s) {
	return put (blkpos, blksize, relpos, s.data(), s.length ());
}

Size Window::put (Size relpos, wchar_t ch)
{
	return put({}, get_size(), relpos, ch);
}

Size Window::put (Size relpos, const wchar_t *s)
{
	return put({}, get_size(), relpos, s);
}

Size Window::put (Size relpos, const wchar_t *s, size_t n)
{
	return put({}, get_size(), relpos, s, n);
}

Size Window::put(Size relpos, std::wstring_view s) {
	return put({}, get_size(), relpos, s);
}

void Window::touch_screen ()
{
	touch_all_lines();
	clearok (stdscr, 1);
}

void Window::touch_windows ()
{
	for (Window *win = bottommost_window; win; win = win->top_window_) {
		win->redraw ();
	}
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
	}
	else {
		return false;
	}
}

void Window::request_close ()
{
	requests_ |= REQUEST_CLOSE;
}

void Window::add_control (Control *ctrl)
{
	assert(&ctrl->window() == this);

	// The "dummy" of this Window.
	// Always the first one added to the window
	if (ctrl == &dummy_ctrl_) {
		dummy_ctrl_.next = dummy_ctrl_.prev = &dummy_ctrl_;
	}
	else {
		Control *old_last = dummy_ctrl_.prev;
		old_last->next = ctrl;
		dummy_ctrl_.prev = ctrl;
		ctrl->prev = old_last;
		ctrl->next = &dummy_ctrl_;
	}
}

bool Window::set_focus_ptr (Control *ctrl, int fall_direction)
{
	if (!ctrl) {
		return false;
	}
	Control *old_focus = focus_ctrl_;
	if (old_focus == ctrl) {
		return true;
	}
	if (old_focus) {
		focus_ctrl_ = 0;
		old_focus->on_defocus ();
	}
	Control *try_ctrl = ctrl;
	for (;;) {
		focus_ctrl_ = try_ctrl;
		if (try_ctrl->on_focus ()) {
			on_focus_changed ();
			// Emit two signals
			if (old_focus) {
				old_focus->sig_defocus.emit ();
			}
			try_ctrl->sig_focus.emit ();
			return true;
		}
		if (fall_direction == 0) {
			break;
		} else if (fall_direction < 0) {
			try_ctrl = try_ctrl->prev;
		} else {
			try_ctrl = try_ctrl->next;
		}
		if (try_ctrl == ctrl) {
			break;
		}
	}

	// Completely failed. Try refocusing the old focus
	focus_ctrl_ = old_focus;
	if (old_focus) {
		if (!old_focus->on_focus ()) {
			focus_ctrl_ = 0;
			on_focus_changed ();
			old_focus->sig_defocus.emit ();
		}
	}
	return false;
}

bool Window::set_focus_next (bool keep_trying)
{
	Control *ctrl = &dummy_ctrl_;
	if (focus_ctrl_) {
		ctrl = focus_ctrl_;
	}
	return set_focus_ptr (ctrl->next, keep_trying ? 1 : 0);
}

bool Window::set_focus_prev (bool keep_trying)
{
	Control *ctrl = &dummy_ctrl_;
	if (focus_ctrl_) {
		ctrl = focus_ctrl_;
	}
	return set_focus_ptr (ctrl->prev, keep_trying ? -1 : 0);
}

bool Window::set_focus_direction (Control *Control::*dir)
{
	if (focus_ctrl_) {
		if (Control *new_focus = focus_ctrl_->*dir) {
			return set_focus_ptr (new_focus, 0);
		}
	}
	return false;
}

bool Window::on_mouse (MouseEvent mouse_event)
{
	bool processed = false;

	// First decide which control this event happens on
	for (Control *ctrl = dummy_ctrl_.next; ctrl != &dummy_ctrl_; ctrl = ctrl->next) {
		if (mouse_event.p - ctrl->get_pos() < ctrl->get_size()) {
			// If it's any mouse key event, we should first try to focus it
			// Otherwise, we do not.
			if (mouse_event.m & MOUSE_ALL_BUTTON) {
				set_focus_ptr (ctrl, 0); // Regardless whether it's successful or not
			}
			processed = ctrl->on_mouse(mouse_event.rebase(ctrl->get_pos()));
			if (!processed) {
				if ((mouse_event.m & LEFT_CLICK) &&
						ctrl->sig_clicked.is_really_connected () &&
						ctrl->sig_clicked.call_condition()) {
					ctrl->sig_clicked.emit ();
					processed = true;
				}
			}
			break;
		}
	}

	// Clicked on the top border (including close button)?
	if (!processed && !(options_ & WINDOW_NO_BORDER)) {
		if ((mouse_event.m & LEFT_CLICK) && (mouse_event.p.y == 0)) {
			if (!(options_ & WINDOW_NO_CLOSE_BUTTON) && (get_size ().x - 2 - mouse_event.p.x < 3)) {
				processed = on_key (ESCAPE);
			}
			else if (!(options_ & WINDOW_NONMOVABLE)) {
				status_ = STATUS_MOVING;
				redraw ();
				processed = true;
			}
		}
	}

	return processed;
}

bool Window::on_key (wchar_t c)
{
	bool processed = false;
	// First try the focused control
	if (focus_ctrl_) {
		processed = focus_ctrl_->on_key(c);
		// Not processed? How about local hotkeys?
		if (!processed) {
			processed = focus_ctrl_->hotkeys_.emit(c);
		}
	}
	// Not processed? How about dialog hotkeys?
	if (!processed) {
		processed = hotkeys_.emit(c);
	}
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
	if (!(options_ & WINDOW_NO_BORDER) && both(size >= Size{2, 2})) {

		if (status_ == STATUS_MOVING) {
			choose_palette (PALETTE_ID_MOVING_BORDER);
		}

		put({}, BORDER_1); // Top, left
		fill({1, 0}, {size.x - 2, 1}, BORDER_H); // Up
		put({size.x - 1, 0}, BORDER_2); // Top, right
		fill({0, 1}, {1, size.y - 2}, BORDER_V); // Left
		fill({size.x - 1, 1}, {1, size.y - 2}, BORDER_V); // Right
		put({0, size.y - 1}, BORDER_3); // Bottom, left
		fill({1, size.y - 1}, {size.x - 2, 1}, BORDER_H); // Down
		put({size.x - 1, size.y - 1}, BORDER_4); // Bottom, right

		// Show the title
		if (!title_.empty ()) {
			unsigned left = (size.x - title_scr_width_ - 2) / 2;
			Size pos{left, 0};
			pos = put (pos, L' ');
			pos = put({}, {size.x, 1}, pos, title_);
			pos = put (pos, L' ');
		}

		// Show the close button
		if (!(options_ & WINDOW_NO_CLOSE_BUTTON) && size.x >= 4) {
			Size pos{size.x - 4, 0};
			// u00d7 is multiplication sign
			put (pos, terminal_emulator_correct_wcwidth () ? L"[\u00d7]" : L"[x]");
		}
	}

	for (Control *ctrl = dummy_ctrl_.next; ctrl != &dummy_ctrl_; ctrl = ctrl->next) {
		ctrl->redraw ();
	}
}

Size Window::get_cursor_pos () const
{
	if (Control *ctrl = get_focus ()) {
		return ctrl->get_cursor_pos () + ctrl->get_pos ();
	}
	return Size{};
}

bool Window::get_cursor_visibility () const
{
	if (Control *ctrl = get_focus ()) {
		return ctrl->get_cursor_visibility ();
	}
	return false;
}

} // namespace tiary::ui
} // namespace tiary
