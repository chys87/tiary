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


#include "ui/label.h"
#include "ui/dialog.h"
#include "common/unicode.h"
#include "common/containers.h"
#include "ui/paletteid.h"
#include "common/signal.h"

namespace tiary {
namespace ui {


Label::Label (Dialog &dlg, const std::wstring &str, unsigned options)
	: Control (dlg)
	, text (str, options)
{
	// Register hotkey
	if (wchar_t c = text.get_hotkey ()) {
		dlg.register_hotkey (c, Signal (sig_hotkey, 0));
		sig_hotkey.connect (dlg, &Dialog::set_focus_ptr, this, 1);
	}
}

Label::~Label ()
{
}

bool Label::on_focus ()
{
	return false;
}

void Label::redraw ()
{
	choose_palette (PALETTE_ID_LABEL);
	clear ();
	text.output (*this, make_size(), get_size());
}

void Label::set_text (const std::wstring &str, unsigned options)
{
	text.set_text (str, options);
	Label::redraw ();
}


} // namespace tiary::ui
} // namespace tiary
