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


#ifndef TIARY_UI_DIALOG_INPUT_H
#define TIARY_UI_DIALOG_INPUT_H

#include <string>

namespace tiary {
namespace ui {

typedef unsigned DialogInputAttribute;

/// If INPUTBOX_PASSWORD is set in the attribute argument in
/// tiary::ui::input_box, the input is not echoed. Instead,
/// asterisks are displayed.
const DialogInputAttribute INPUT_PASSWORD = 1;

/// @brief	Displays a dialog to receive keyboard inputs from the user
/// @result	The entered text, or @c return_on_error if the user presses Esc
std::wstring dialog_input (
		const std::wstring &hint, ///< The hint text.
		const std::wstring &default_text = std::wstring (), ///< Default text in the box
		unsigned text_box_width = 16, ///< Screen width of the textbox
		DialogInputAttribute attributes = 0, ///< Currently only 0 or tiary::ui::INPUT_PASSWORD
		const std::wstring &return_on_error = std::wstring () ///< What to return if the user presses Esc
		);

/// @brief	Displays a dialog to receive keyboard inputs from the user
std::wstring dialog_input2 (
		const std::wstring &title,
		const std::wstring &hint, ///< The hint text.
		const std::wstring &default_text = std::wstring (), ///< Default text in the box
		unsigned text_box_width = 16, ///< Screen width of the textbox
		DialogInputAttribute attributes = 0, ///< Currently only 0 or tiary::ui::INPUT_PASSWORD
		const std::wstring &return_on_error = std::wstring () ///< What to return if the user presses Esc
		);

} // namespace tiary::ui
} // namespace tiary


#endif // include guard
