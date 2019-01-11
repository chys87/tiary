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

// The following options control the focus button when the dialog is displayed
// If not set, it's the leftmost one
const WindowMessageButton MESSAGE_DEFAULT_OK     = 0x10;
const WindowMessageButton MESSAGE_DEFAULT_YES    = 0x20;
const WindowMessageButton MESSAGE_DEFAULT_NO     = 0x40;
const WindowMessageButton MESSAGE_DEFAULT_CANCEL = 0x80;

WindowMessageButton dialog_message(std::wstring_view text, WindowMessageButton buttons = MESSAGE_OK);
WindowMessageButton dialog_message(std::wstring_view text, std::wstring_view title,
		WindowMessageButton buttons = MESSAGE_OK);

} // namespace tiary::ui
} // namespace tiary

#endif // include guard
