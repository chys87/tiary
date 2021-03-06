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


#ifndef TIARY_UI_DIALOG_RICHTEXT_H
#define TIARY_UI_DIALOG_RICHTEXT_H

#include "ui/size.h"
#include "ui/richtextlist.h"
#include <string_view>
#include <vector>

namespace tiary {
namespace ui {

/**
 * @brief	Display text using a RichText control
 *
 */
void dialog_richtext (
		std::wstring_view title, ///< Title for the dialog
		MultiLineRichText &&mrt, ///< Text info
		Size size_hint = {} ///< A text area size hint (may be silently ignored)
		);

} // namespace tiary::ui
} // namespace tiary


#endif // include guard
