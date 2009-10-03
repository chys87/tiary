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



#include "ui/dialog_richtext.h"
#include "ui/richtextlist.h"
#include "ui/richtext.h"
#include "ui/window.h"
#include "common/algorithm.h"


namespace tiary {
namespace ui {

namespace {

class WindowRichText : public Window
{

	RichText text;
	Size size_hint;
	unsigned max_text_width;

public:
	WindowRichText (const std::wstring &title, const std::wstring &text,
			const RichTextLineList &list, Size size_hint);
	~WindowRichText ();

	void redraw ();
};

WindowRichText::WindowRichText (const std::wstring &title, const std::wstring &text,
		const RichTextLineList &lst, Size size_hint_)
	: Window (0, title)
	, text (*this, text, lst)
	, size_hint (size_hint_)
{
	max_text_width = 0;
	for (RichTextLineList::const_iterator it = lst.begin ();
			it != lst.end (); ++it) {
		if (max_text_width < it->screen_wid) {
			max_text_width = it->screen_wid;
		}
	}

	Signal sig_close (this, &Window::request_close);
	register_hotkey (RETURN, sig_close);
	register_hotkey (NEWLINE, sig_close);
	register_hotkey (ESCAPE, sig_close);
	register_hotkey (L'q', TIARY_STD_MOVE (sig_close));

	WindowRichText::redraw ();
}

WindowRichText::~WindowRichText ()
{
}

void WindowRichText::redraw ()
{
	Size ideal_size = size_hint;
	if (ideal_size.x < max_text_width) {
		ideal_size.x = max_text_width + 12;
	}
	if (ideal_size.y < text.get_list ().size ()) {
		ideal_size.y = text.get_list ().size () + 3;
	}
	ideal_size += make_size (4, 2); // Border
	Size scrsize = get_screen_size ();
	ideal_size &= scrsize;

	Window::move_resize ((scrsize - ideal_size)/2, ideal_size);
	text.move_resize (make_size (2, 1), ideal_size - make_size (4, 2));
	Window::redraw ();
}

} // anonymous namespace

void dialog_richtext (const std::wstring &title,
		const std::wstring &text,
		const RichTextLineList &list,
		Size size_hint)
{
	WindowRichText (title, text, list, size_hint).event_loop ();
}

} // namespace tiary::ui
} // namespace tiary

