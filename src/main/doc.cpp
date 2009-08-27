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

// TODO: Use a Dialog instead of an external pager

#include "main/doc.h"
#include "common/external.h"
#include "ui/window.h"
#include "ui/dialog_message.h"

namespace tiary {

const char info [] = "\
KEYBOARD COMMANDS\n\
=================\n\
\n\
      Key                  Description\n\
      ============         =================================================\n\
      Esc                  Go to menu\n\
      k UP                 Backward one line\n\
      j DOWN               Forward one line\n\
      ^ g < HOME           Jump to the first entry\n\
      $ G > END            Jump to the last entry\n\
      b PAGEUP             Backward one page\n\
      f PAGEDOWN           Forward one page\n\
      v ENTER RIGHT        View the selected entry\n\
      V                    View all entries\n\
      e E                  Edit the selected entry\n\
      t                    Edit tags of the selected entry\n\
      T                    Edit tags of the selected entry (quick)\n\
      i I                  Change the time of the selected entry\n\
      a A INSERT           Add a new entry\n\
      d D DELETE           Remove the selected entry\n\
      m                    Move the selected entry up\n\
      M                    Move the selected entry down\n\
      / CTRL+F             Search forward (supports regular expressions)\n\
      ?                    Search backward (supports regular expressions)\n\
      n F3                 Search next\n\
      N                    Search previous\n\
      p P                  Create/modify/remove password\n\
      r                    Edit per-file preferences\n\
      R                    Edit global preferences\n\
      CTRL+L               Refresh the screen\n\
      h H F1               View this help info\n\
      CTRL+N               New file\n\
      CTRL+O               Open file\n\
      w CTRL+S             Save\n\
      W                    Save as\n\
      q Q                  Quit\n\
\n\
\n\
For technical reasons, if you press Esc, there will be a delay (approximately\n\
.5 seconds) before the the program responds.\n\
";



void show_doc ()
{
	ui::Window::suspend ();
	FILE *fp = call_external_program_popen ("less", "", "w");
	// SIGPIPE is ignored (in main)
	fwrite_unlocked (info, 1, sizeof info - 1, fp);
	pclose (fp);
	ui::Window::resume ();
}

#define N(x) W(S(x))
#define W(x) W_IMPL(x)
#define W_IMPL(x) L##x
#define S(x) S_IMPL(x)
#define S_IMPL(x) #x

const wchar_t about [] = L"\
Tiary " N(TIARY_MAJOR_VERSION) L"." N(TIARY_MINOR_VERSION) L"." N(TIARY_PATCH_VERSION) L" (c) " N(TIARY_COPYRIGHT_YEAR) L"\n\
\n\
\n\
Tiary, a terminal-based diary keeping system for Unix-like systems\n\
Copyright (C) 2009, chys <admin@CHYS.INFO>\n\
\n\
This software is licensed under the 3-clause BSD license.\n\
See LICENSE in the source package and/or online info for details.\n\
";

void show_about ()
{
	ui::dialog_message (about, L"About");
}

}
