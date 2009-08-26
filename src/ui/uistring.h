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

#include "common/unicode.h"
#include <string>

namespace tiary {
namespace ui {

struct Size;

class Control;
class Window;

class UIStringBase;
class UIString;
class UIStringOne;

/**
 * @brief	"Basic" UIString
 *
 * Responsible for finding the first "&" character in a string
 * (used for hotkey in UI)
 */
class UIStringBase
{
public:
	static const unsigned NO_HOTKEY = 1;

	explicit UIStringBase (const std::wstring & = std::wstring (), unsigned options = 0);

	void set_text (const std::wstring &, unsigned options = 0);

	wchar_t get_hotkey () const; // Returns L'\0' if none
	size_t get_hotkey_pos () const { return hotkey_pos; }

	const std::wstring &get_text () const { return text; }

private:
	std::wstring text;
	size_t hotkey_pos; // size_t(-1) if none

	// Find the hotkey character position,
	// and remove the first '&' from the string
	void update ();
};

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

class UIString : public UIStringBase
{
public:
	explicit UIString (const std::wstring & = std::wstring (), unsigned options = 0);
	void set_text (const std::wstring &, unsigned options = 0);

	const SplitStringLineList &split_line (unsigned wid);

	/**
	 * @brief	Output text to a control, highlighting hotkey character
	 * @param	pos	Position relative to the control
	 */
	void output (Control &, Size pos, Size size);

	unsigned get_lines () const { return lines; }
	unsigned get_max_width () const { return max_width; }

private:
	unsigned lines;
	unsigned max_width;

	unsigned split_cache_wid;
	SplitStringLineList split_cache;

	void update ();
};


} // namespace tiary::ui
} // namespace ui

#endif // Include guard
