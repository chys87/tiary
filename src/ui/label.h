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


#ifndef TIARY_UI_LABEL_H
#define TIARY_UI_LABEL_H

/**
 * @file	ui/label.h
 * @author	chys <admin@chys.info>
 * @brief	Defines the tiary::ui::Label class
 */

#include "ui/control.h"
#include "ui/uistring.h"
#include "common/signal.h"
#include <string>

namespace tiary {
namespace ui {

/**
 * @brief	A label (a kind of control)
 *
 * If you use '&' and then modify the text, the hotkey is _not_ re-registered
 * (Behavior may change in the future.)
 */
class Label final : public Control {
public:
	Label(Window &, std::wstring_view, unsigned = 0 /**< UIString options */);
	Label(Window &, std::wstring &&, unsigned = 0 /**< UIString options */);
	Label(Window &win, const wchar_t *s, unsigned options = 0) : Label(win, std::wstring_view(s), options) {}
	~Label ();

	void redraw ();

	void set_text(std::wstring_view, unsigned = 0 /**< UIString options */);
	void set_text(std::wstring &&, unsigned = 0 /**< UIString options */);
	const UIString &get_uistring() const { return text_; }
	const std::wstring &get_text() const { return text_.get_text(); }

	// Two "relay" wrappers
	unsigned get_max_text_width() const { return text_.get_max_width(); }
	const SplitStringLineList & split_line(unsigned wid) { return text_.split_line(wid); }

	/**
	 * @brief	Handles the hotkey of the label
	 *
	 * This signal is emitted when the user presses a hotkey specified by the '&amp;'
	 * character in the text.
	 * The default action is to try focusing the control next to this label.
	 * You can override this behavior.
	 */
	Signal sig_hotkey;

private:
	void common_initialize();

private:
	UIString text_;
};

} // namespace tiary::ui
} // namespace tiary

#endif // include guard
