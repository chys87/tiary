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


#include "ui/ui.h"
#include "ui/fixed_dialog.h"


namespace tiary {
namespace ui {

namespace {

Size calculate_pos (Size size)
{
	return ((get_screen_size () | size) - size) / 2;
}

} // anonymous namespace

FixedDialog::FixedDialog (unsigned options, const std::wstring &title)
	: Dialog (options, title)
{
}

FixedDialog::FixedDialog (Size size, unsigned options, const std::wstring &title)
	: Dialog (options, title)
{
	resize (size);
}

FixedDialog::~FixedDialog ()
{
}

void FixedDialog::resize (Size newsize)
{
	move_resize (calculate_pos (newsize), newsize);
}

void FixedDialog::on_winch ()
{
	Size size = get_size ();
	move_resize (calculate_pos (size), size);
	redraw ();
}



} // namespace tiary::ui
} // namespace tiary

