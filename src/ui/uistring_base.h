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


#ifndef TIARY_UI_UISTRING_BASE_H
#define TIARY_UI_UISTRING_BASE_H

#include <string>

namespace tiary {
namespace ui {

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
};

} // namespace tiary::ui
} // namespace ui

#endif // Include guard
