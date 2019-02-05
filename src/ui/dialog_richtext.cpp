// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2016, 2018, chys <admin@CHYS.INFO>
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
public:
	WindowRichText(std::wstring_view title, MultiLineRichText &&mrt,
			Size size_hint);
	~WindowRichText ();

	void redraw ();

private:
	RichText text;
	Size size_hint;
	unsigned max_text_width;
};

WindowRichText::WindowRichText(std::wstring_view title, MultiLineRichText &&mrt,
		Size size_hint_)
	: Window (0, title)
	, text(*this, std::move(mrt))
	, size_hint (size_hint_)
{
	max_text_width = 0;
	for (const RichTextLine &line: text.get_mrt().lines) {
		if (max_text_width < line.screen_wid) {
			max_text_width = line.screen_wid;
		}
	}

	Signal sig_close (this, &Window::request_close);
	register_hotkey (RETURN, sig_close);
	register_hotkey (NEWLINE, sig_close);
	register_hotkey (ESCAPE, sig_close);
	register_hotkey (L'q', std::move (sig_close));
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
	if (ideal_size.y < text.get_mrt().lines.size()) {
		ideal_size.y = text.get_mrt().lines.size() + 3;
	}
	ideal_size += Size{4, 2}; // Border
	Size scrsize = get_screen_size ();
	ideal_size &= scrsize;

	Window::move_resize ((scrsize - ideal_size)/2, ideal_size);
	text.move_resize({2, 1}, ideal_size - Size{4, 2});
	Window::redraw ();
}

} // anonymous namespace

void dialog_richtext(std::wstring_view title,
		MultiLineRichText &&mrt,
		Size size_hint) {
	WindowRichText(title, std::move(mrt), size_hint).event_loop();
}

} // namespace tiary::ui
} // namespace tiary

