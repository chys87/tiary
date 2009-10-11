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


#ifndef TIARY_UI_UISTRING_H
#define TIARY_UI_UISTRING_H

/**
 * An "UI string" stores a string, and does some useful analysis
 * so that controls can use it conveniently:
 *
 * (1) Remove the first '&' character and remember its position;
 * (2) Number of lines, max line width, etc.
 * (3) Cache the results of split_line
 *
 * UIStringOne is a simplified version, supporting only one line
 */

#include "ui/uistring_base.h"
#include "common/split_line.h"
#include <string>

namespace tiary {
namespace ui {

struct Size;

class Control;
class Window;

class UIString : public UIStringBase
{
public:
	explicit UIString (const std::wstring & = std::wstring (), unsigned options = 0);
	void set_text (const std::wstring &, unsigned options = 0);

	const SplitStringLineList &split_line (unsigned wid) const;

	/**
	 * @brief	Output text to a control, highlighting hotkey character
	 * @param	pos	Position relative to the control
	 */
	void output (Control &, Size pos, Size size) const;

	unsigned get_lines () const { return lines; }
	unsigned get_max_width () const { return max_width; }

private:
	unsigned lines;
	unsigned max_width;

	// They are mutable because split_line need modify them
	mutable unsigned split_cache_wid;
	mutable SplitStringLineList split_cache;

	void update ();
};


} // namespace tiary::ui
} // namespace ui

#endif // Include guard
