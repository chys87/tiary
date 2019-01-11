// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2019, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#ifndef TIARY_UI_DIALOG_INPUT_H
#define TIARY_UI_DIALOG_INPUT_H

#include <string>

namespace tiary {
namespace ui {

typedef unsigned WindowInputAttribute;

/// If INPUTBOX_PASSWORD is set in the attribute argument in
/// tiary::ui::input_box, the input is not echoed. Instead,
/// asterisks are displayed.
const WindowInputAttribute INPUT_PASSWORD = 1;

/// @brief	Displays a dialog to receive keyboard inputs from the user
/// @result	The entered text, or @c return_on_error if the user presses Esc
std::wstring dialog_input (
		std::wstring_view hint, ///< The hint text.
		std::wstring_view default_text = {}, ///< Default text in the box
		unsigned text_box_width = 16, ///< Screen width of the textbox
		WindowInputAttribute attributes = 0, ///< Currently only 0 or tiary::ui::INPUT_PASSWORD
		std::wstring_view return_on_error = {} ///< What to return if the user presses Esc
		);

/// @brief	Displays a dialog to receive keyboard inputs from the user
std::wstring dialog_input2 (
		std::wstring_view title,
		std::wstring_view hint, ///< The hint text.
		std::wstring_view default_text = {}, ///< Default text in the box
		unsigned text_box_width = 16, ///< Screen width of the textbox
		WindowInputAttribute attributes = 0, ///< Currently only 0 or tiary::ui::INPUT_PASSWORD
		std::wstring_view return_on_error = {} ///< What to return if the user presses Esc
		);

} // namespace tiary::ui
} // namespace tiary


#endif // include guard
