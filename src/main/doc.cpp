// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2010, 2018, 2019, 2022 chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


/**
 * @file	main/doc.cpp
 * @author	chys <admin@chys.info>
 * @brief	Implements tiary::show_doc, tiary::show_license and tiary::show_about
 */

#include "main/doc.h"
#include "ui/dialog_message.h"
#include "ui/dialog_richtext.h"
#include "ui/richtextlist.h"
#include "ui/paletteid.h"
#include "common/format.h"
#include "common/string.h"

namespace tiary {

void show_doc() {
	ui::dialog_richtext(L"Help"sv, ui::combine_lines({
		{ui::PALETTE_ID_SHOW_BOLD  , L"KEYBOARD COMMANDS"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"================="sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L""sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    Key                  Description"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    ============         ================================================="sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    Esc                  Go to menu"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    k UP                 Backward one line"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    j DOWN               Forward one line"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    ^ g < HOME           Jump to the first entry"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    $ G > END            Jump to the last entry"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    b PAGEUP             Backward one page"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    f PAGEDOWN           Forward one page"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    v ENTER RIGHT        View the selected entry"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    V                    View all entries"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    e E                  Edit the selected entry"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    l                    Edit labels of the selected entry"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    L                    Rename or delete labels, applying to all entries"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    t T                  Change the time of the selected entry"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    a A INSERT           Add a new entry"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    d D DELETE           Remove the selected entry"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    m                    Move the selected entry up"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    M                    Move the selected entry down"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    s                    Display statistics info"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    S                    Sort all entries by date and time"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    / CTRL+F             Search forward"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    ?                    Search backward"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    n F3                 Search next"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    N                    Search previous"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    CTRL+G               Filtering"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    p P                  Create/modify/remove password"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    r                    Edit per-file preferences"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    R                    Edit global preferences"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    CTRL+L               Refresh the screen"sv},
#ifdef TIARY_USE_MOUSE
		{ui::PALETTE_ID_SHOW_NORMAL, L"    F12                  Enable/disable mouse"sv},
#endif
		{ui::PALETTE_ID_SHOW_NORMAL, L"    h H F1               View this help info"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    CTRL+N               New file"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    CTRL+O               Open file"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    w CTRL+S             Save"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    W                    Save as"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L"    q Q                  Quit"sv},
		{ui::PALETTE_ID_SHOW_NORMAL, L""sv}
	}));
}

constexpr std::wstring_view license = L"\
Copyright (c) " TIARY_COPYRIGHT_YEAR_WIDE L", chys <admin@CHYS.INFO>\n\
All rights reserved.\n\
\n\
Redistribution and use in source and binary forms, with or without\n\
modification, are permitted provided that the following conditions are met:\n\
    * Redistributions of source code must retain the above copyright\n\
      notice, this list of conditions and the following disclaimer.\n\
    * Redistributions in binary form must reproduce the above copyright\n\
      notice, this list of conditions and the following disclaimer in the\n\
      documentation and/or other materials provided with the distribution.\n\
    * Neither the name of chys <admin@CHYS.INFO> nor the\n\
      names of its contributors may be used to endorse or promote products\n\
      derived from this software without specific prior written permission.\n\
\n\
THIS SOFTWARE IS PROVIDED BY chys <admin@CHYS.INFO> ''AS IS'' AND ANY\n\
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED\n\
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\n\
DISCLAIMED. IN NO EVENT SHALL chys <admin@CHYS.INFO> BE LIABLE FOR ANY\n\
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES\n\
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;\n\
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND\n\
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT\n\
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS\n\
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n\
"sv;

void show_license ()
{
	ui::dialog_richtext(L"License (Three-clause BSD license)"sv,
			{
				std::wstring(license),
				ui::split_richtext_lines(license, ui::PALETTE_ID_SHOW_NORMAL, 80)
			});
}

constexpr std::wstring_view about = LR"(Tiary %a.%b.%c


Tiary, a terminal-based diary keeping system for Unix-like systems
Copyright (C) %d, chys <admin@CHYS.INFO>

This software is licensed under the 3-clause BSD license.
See License for details.

This product includes software developed by the OpenSSL Project
for use in the OpenSSL Toolkit (http://www.openssl.org/).
)"sv;

void show_about ()
{
	ui::dialog_message(format(about,
				unsigned(TIARY_MAJOR_VERSION),
				unsigned (TIARY_MINOR_VERSION),
				unsigned (TIARY_PATCH_VERSION),
				TIARY_COPYRIGHT_YEAR_WIDE), L"About"sv);
}

} // namespace tiary
