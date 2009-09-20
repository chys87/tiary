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


#ifndef TIARY_UI_UISTRING_ONE_H
#define TIARY_UI_UISTRING_ONE_H

#include "ui/uistring_base.h"
#include <string>

namespace tiary {
namespace ui {

struct Size;
class Window;
class Control;

/**
 * @brief	One-line UIString
 *
 * Simplified version of tiary::ui::UIString, supporting only one line.
 * Behavior is undefined if newline characters are found.
 */
class UIStringOne : public UIStringBase
{
public:
	explicit UIStringOne (const std::wstring & = std::wstring (), unsigned options = 0);
	void set_text (const std::wstring &, unsigned options = 0);

	/**
	 * @brief	Output text to a control, highlighting hotkey character
	 * @param	pos	Position relative to the control
	 * @param	wid	Maximal screen width to use
	 */
	Size output (Control &, Size pos, unsigned wid);
	/**
	 * @brief	Output text directly to a Window, highlighting hotkey character
	 * @param	pos	Position relative to the window
	 * @param	wid	Maximum screen width to use
	 */
	Size output (Window &, Size pos, unsigned wid);

	unsigned get_width () const { return width; }

private:
	unsigned width;

	// Update width info
	void update ();
};

} // namespace tiary::ui
} // namespace ui

#endif // Include guard
