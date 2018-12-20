// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2018, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#ifndef TIARY_UI_GRID_SELECT_H
#define TIARY_UI_GRID_SELECT_H

#include "ui/control.h"
#include <string>
#include <vector>

namespace tiary {
namespace ui {


class GridSelect final : public Control {
public:
	GridSelect (Window &win);
	~GridSelect ();

	struct Item
	{
		std::wstring text;
		bool selectable;

		Item () : text (), selectable (false) {}

		Item (const std::wstring &text_, bool selectable_ = true)
			: text (text_)
			, selectable (selectable_)
		{
		}
		Item (const wchar_t *text_, bool selectable_ = true)
			: text (text_)
			, selectable (selectable_)
		{
		}
	};

	void set_grid (
			unsigned grid_width, ///< The width of each grid on screen
			unsigned grid_cols, ///< Maximum number of grids to display in every row
			unsigned grid_rows, ///< Maximum number of grids to display in every col
			const std::vector <Item> &items, ///< Items
			size_t pre_select = size_t (-1)
		);
	void set_select (size_t, bool emit_signal = true);

	const std::vector <Item> &get_items () const { return items; }
	size_t get_select () const { return select; }

	void redraw ();
	bool on_key (wchar_t);
	bool on_mouse (MouseEvent);

	Signal sig_select_changed;
	Signal sig_double_clicked;

private:
	unsigned grid_width;
	unsigned cols;
	unsigned rows;
	std::vector <Item> items;
	size_t select; ///< Selection (subscript in items)
};

} // namespace tiary::ui
} // namespace tiary

#endif // include guard
