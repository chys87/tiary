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


typedef unsigned DialogMessageButton;

const DialogMessageButton MESSAGE_OK     = 0x01;
const DialogMessageButton MESSAGE_YES    = 0x02;
const DialogMessageButton MESSAGE_NO     = 0x04;
const DialogMessageButton MESSAGE_CANCEL = 0x08;
const DialogMessageButton MESSAGE_ALL    = 0x0f;

DialogMessageButton dialog_message (const std::wstring &text, DialogMessageButton buttons = MESSAGE_OK);
DialogMessageButton dialog_message (const wchar_t *text, DialogMessageButton buttons = MESSAGE_OK);
DialogMessageButton dialog_message (const std::wstring &text, const std::wstring &title,
		DialogMessageButton buttons = MESSAGE_OK);
DialogMessageButton dialog_message (const wchar_t *text, const wchar_t *title,
		DialogMessageButton buttons = MESSAGE_OK);

} // namespace tiary::ui
} // namespace tiary

#endif // include guard
