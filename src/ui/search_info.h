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


#ifndef TIARY_UI_SEARCH_INFO_H
#define TIARY_UI_SEARCH_INFO_H

#include "common/string_match.h"
#include <string>

namespace tiary {
namespace ui {

/**
 * This class mains information of a search request,
 * including the text, direction and 
 */
class SearchInfo {
public:
	SearchInfo ();
	~SearchInfo ();

	/**
	 * @brief	Show a dialog to ask user what to search for
	 * @result	If true, we can go on with search
	 */
	bool dialog (bool default_backward);

	bool get_backward () const { return backward_; }

	explicit operator bool() const { return static_cast<bool>(matcher_); }
	bool basic_match(const std::wstring &s) const { return matcher_.basic_match(s); }
	std::vector<std::pair<size_t, size_t>> match(const std::wstring &s) const { return matcher_.match(s); }
	const StringMatch &get_matcher() const { return matcher_; }

private:
	StringMatch matcher_;
	bool backward_; ///< Search backward?
};

} // namespace ui
} // namespace tiary

#endif // include guard
