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


#ifndef TIARY_UI_SEARCH_INFO_H
#define TIARY_UI_SEARCH_INFO_H

#include "common/pcre.h"
#include <memory> // std:;auto_ptr
#include <string>

namespace tiary {
namespace ui {

/**
 * This class mains information of a search request,
 * including the text, direction and 
 */
struct SearchInfo
{
public:
	SearchInfo ();
	~SearchInfo ();

	/**
	 * @brief	Show a dialog to ask user what to search for
	 * @result	If true, we can go on with search
	 */
	bool dialog (bool default_backward);

	struct BooleanConvert { int valid; };
	/**
	 * @brief	Whether this class contains valid search info
	 */
	operator int BooleanConvert::* () const;

	bool match (const std::wstring &) const;

	bool get_backward () const { return backward; }

private:
	std::wstring text;         ///< The text to search for
	bool backward;             ///< Search backward?
#ifdef TIARY_USE_PCRE
	std::auto_ptr<PcRe> regex; ///< PcRe object related to search_text, if it is a regular expression
#endif
};

} // namespace ui
} // namespace tiary

#endif // include guard
