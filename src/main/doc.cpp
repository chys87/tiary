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


/**
 * @file	main/doc.cpp
 * @author	chys <admin@chys.info>
 * @brief	Implements tiary::show_doc
 */

// TODO: Use a Window instead of an external pager

#include "main/doc.h"
#include "common/external.h"
#include "ui/window.h"
#include "ui/dialog_message.h"
#include "ui/dialog_richtext.h"
#include "ui/paletteid.h"
#include "common/algorithm.h"
#include "common/format.h"

namespace tiary {

const ui::RichTextLineC info[] = {
	{ ui::PALETTE_ID_SHOW_BOLD  , L"KEYBOARD COMMANDS" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"=================" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"    Key                  Description" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"    ============         =================================================" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"    Esc                  Go to menu" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"    k UP                 Backward one line" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"    j DOWN               Forward one line" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"    ^ g < HOME           Jump to the first entry" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"    $ G > END            Jump to the last entry" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"    b PAGEUP             Backward one page" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"    f PAGEDOWN           Forward one page" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"    v ENTER RIGHT        View the selected entry" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"    V                    View all entries" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"    e E                  Edit the selected entry" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"    l                    Edit labels of the selected entry" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"    L                    Rename or delete labels, applying to all entries" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"    t T                  Change the time of the selected entry" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"    a A INSERT           Add a new entry" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"    d D DELETE           Remove the selected entry" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"    m                    Move the selected entry up" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"    M                    Move the selected entry down" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"    S                    Sort all entries by date and time" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"    / CTRL+F             Search forward (supports regular expressions)" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"    ?                    Search backward (supports regular expressions)" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"    n F3                 Search next" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"    N                    Search previous" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"    p P                  Create/modify/remove password" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"    r                    Edit per-file preferences" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"    R                    Edit global preferences" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"    CTRL+L               Refresh the screen" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"    h H F1               View this help info" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"    CTRL+N               New file" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"    CTRL+O               Open file" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"    w CTRL+S             Save" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"    W                    Save as" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"    q Q                  Quit" },
	{ ui::PALETTE_ID_SHOW_NORMAL, L"" }
};



void show_doc ()
{
	ui::dialog_richtext (L"Help",
			ui::RichTextList (info, array_end (info))
			);
}

const wchar_t license [] = L"\
Copyright (c) 2009, chys <admin@CHYS.INFO>\n\
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
";

void show_license ()
{
	ui::dialog_message (license, L"License (Three-clause BSD license)");
}

const wchar_t about [] = L"\
Tiary %a.%b.%c\n\
\n\
\n\
Tiary, a terminal-based diary keeping system for Unix-like systems\n\
Copyright (C) %d, chys <admin@CHYS.INFO>\n\
\n\
This software is licensed under the 3-clause BSD license.\n\
See License for details.\n\
";

void show_about ()
{
	ui::dialog_message (format (about) << unsigned (TIARY_MAJOR_VERSION)
			<< unsigned (TIARY_MINOR_VERSION)
			<< unsigned (TIARY_PATCH_VERSION)
			<< unsigned (TIARY_COPYRIGHT_YEAR), L"About");
}

}
