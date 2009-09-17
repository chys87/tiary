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


#include "ui/grid_select.h"
#include "ui/paletteid.h"


namespace tiary {
namespace ui {


GridSelect::GridSelect (Window &win)
	: Control (win)
	, grid_width (0)
	, cols (0)
	, rows (0)
	, items ()
	, select (size_t (-1))
{
}

GridSelect::~GridSelect ()
{
}

void GridSelect::set_grid (unsigned grid_width, unsigned grid_cols,
			unsigned grid_rows, const std::vector <Item> &grid_items,
			size_t new_select)
{
	this->grid_width = grid_width;
	cols = grid_cols;
	rows = grid_rows;
	items = grid_items;
	items.resize (grid_cols * grid_rows);
	select = size_t (-1);
	if (new_select<items.size() && items[new_select].selectable) {
		select = new_select;
	}
	GridSelect::redraw ();
}

void GridSelect::set_select (size_t new_select, bool emit_signal)
{
	if (new_select<items.size() && items[new_select].selectable) {
		if (new_select != select) {
			select = new_select;
			if (emit_signal) {
				sig_select_changed.emit ();
			}
			GridSelect::redraw ();
		}
	} else {
		if (select < items.size ()) {
			select = size_t (-1);
			if (emit_signal) {
				sig_select_changed.emit ();
			}
			GridSelect::redraw ();
		}
	}
}

void GridSelect::redraw ()
{
	choose_palette (PALETTE_ID_GRID);
	clear ();
	move_cursor (make_size ());

	for (unsigned y = 0; y < rows; ++y) {
		for (unsigned x = 0; x < cols; ++x) {
			Size pos = make_size (x * grid_width, y);
			unsigned i = y * cols + x;
			const Item &item = items[i];
			PaletteID id = PALETTE_ID_GRID;
			if (!item.selectable) {
				id = PALETTE_ID_GRID_INVALID;
			}
			else if (i == select) {
				move_cursor (pos);
				id = PALETTE_ID_GRID_SELECT;
			}
			choose_palette (id);
			pos = put (pos, item.text);
		}
	}
}

namespace {

size_t get_next_by_direction (unsigned id, unsigned cols,
		unsigned rows, wchar_t direction)
{
	unsigned N = rows*cols;
	switch (direction) {
		case LEFT:
			if (int (--id) < 0) {
				id = N - 1;
			}
			break;
		case RIGHT:
			if (++id >= N) {
				id = 0;
			}
			break;
		case UP:
			if (id == 0) {
				id = N - 1;
			}
			else if (int (id -= cols) < 0) {
				id += N - 1;
			}
			break;
		case DOWN:
			if (id >= N - 1) {
				id = 0;
			}
			else if ((id += cols) >= N) {
				id = id - N + 1;
			}
			break;
	}
	return id;
}

size_t select_direction (unsigned current_id, unsigned cols, unsigned rows,
		const std::vector<GridSelect::Item> &items, wchar_t direction)
{
	unsigned id = current_id;
	if (id >= items.size ()) {
		if (direction==UP || direction==LEFT) {
			id = items.size() - 1;
		}
		else {
			id = 0;
		}
	}
	else {
		id = get_next_by_direction (id, cols, rows, direction);
	}
	while ((id != current_id) && !items[id].selectable) {
		id = get_next_by_direction (id, cols, rows, direction);
	}
	return id;
}

} // anonymous namespace

bool GridSelect::on_key (wchar_t key)
{
	if (items.empty ()) {
		return false;
	}
	unsigned new_select;
	switch (key) {
		case LEFT:
		case RIGHT:
		case UP:
		case DOWN:
			new_select = select_direction (select, cols, rows, items, key);
			if (new_select != select) {
				set_select (new_select);
				return true;
			}
			return false;
		case HOME:
			new_select = select_direction (-1u, cols, rows, items, RIGHT);
			if (new_select != select) {
				set_select (new_select);
				return true;
			}
			return false;
		case END:
			new_select = select_direction (-1u, cols, rows, items, LEFT);
			if (new_select != select) {
				set_select (new_select);
				return true;
			}
			return false;
		default:
			return false;
	}
}

bool GridSelect::on_mouse (MouseEvent mouse_event)
{
	if (!(mouse_event.m & (LEFT_CLICK|LEFT_DCLICK))) {
		return false;
	}
	unsigned click_row = mouse_event.p.y;
	unsigned click_col = mouse_event.p.x / grid_width;
	if (click_row>=rows || click_col>=cols) {
		return false;
	}
	unsigned id = click_row * cols + click_col;
	if (!items[id].selectable) {
		return false;
	}
	if (id != select) {
		set_select (id, true);
	}
	if (mouse_event.m & LEFT_CLICK) {
		sig_double_clicked.emit ();
	}
	return true;
}

} // namespace tiary::ui
} // namespace tiary
