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


#ifndef TIARY_UI_DIALOG_RICHTEXT_H
#define TIARY_UI_DIALOG_RICHTEXT_H

#include "ui/size.h"
#include <string>
#include <vector>

namespace tiary {
namespace ui {

struct RichTextLine;
typedef std::vector <RichTextLine> RichTextList;

/**
 * @brief	Display text using a RichText control
 *
 */
void dialog_richtext (
		const std::wstring &title, ///< Title for the dialog
		const RichTextList &list, ///< Contents
		Size size_hint = make_size () ///< A text area size hint (may be silently ignored)
		);

#ifdef TIARY_HAVE_RVALUE_REFERENCES
void dialog_richtext (
		const std::wstring &title, ///< Title for the dialog
		RichTextList &&list, ///< Contents
		Size size_hint = make_size () ///< A text area size hint (may be silently ignored)
		);
#endif

} // namespace tiary::ui
} // namespace tiary


#endif // include guard
