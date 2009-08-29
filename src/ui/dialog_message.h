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


#ifndef TIARY_UI_DIALOG_MESSAGE_H
#define TIARY_UI_DIALOG_MESSAGE_H

#include <string>

namespace tiary {
namespace ui {


typedef unsigned WindowMessageButton;

const WindowMessageButton MESSAGE_OK     = 0x01;
const WindowMessageButton MESSAGE_YES    = 0x02;
const WindowMessageButton MESSAGE_NO     = 0x04;
const WindowMessageButton MESSAGE_CANCEL = 0x08;
const WindowMessageButton MESSAGE_ALL    = 0x0f;

WindowMessageButton dialog_message (const std::wstring &text, WindowMessageButton buttons = MESSAGE_OK);
WindowMessageButton dialog_message (const wchar_t *text, WindowMessageButton buttons = MESSAGE_OK);
WindowMessageButton dialog_message (const std::wstring &text, const std::wstring &title,
		WindowMessageButton buttons = MESSAGE_OK);
WindowMessageButton dialog_message (const wchar_t *text, const wchar_t *title,
		WindowMessageButton buttons = MESSAGE_OK);

} // namespace tiary::ui
} // namespace tiary

#endif // include guard
