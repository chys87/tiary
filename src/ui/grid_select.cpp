// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2018, 2019, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#include "ui/grid_select.h"
#include "ui/paletteid.h"
#include "ui/mouse.h"


namespace tiary {
namespace ui {


GridSelect::GridSelect (Window &win)
	: Control (win)
	, grid_width_(0)
	, cols_(0)
	, rows_(0)
	, items_()
	, select_(size_t (-1))
{
}

GridSelect::~GridSelect ()
{
}

void GridSelect::set_grid (unsigned grid_width, unsigned grid_cols,
			unsigned grid_rows, const std::vector <Item> &grid_items,
			size_t new_select)
{
	grid_width_ = grid_width;
	cols_ = grid_cols;
	rows_ = grid_rows;
	items_ = grid_items;
	items_.resize (grid_cols * grid_rows);
	select_ = size_t (-1);
	if (new_select < items_.size() && items_[new_select].selectable) {
		select_ = new_select;
	}
	redraw();
}

void GridSelect::set_grid(unsigned grid_width, unsigned grid_cols,
			unsigned grid_rows, std::vector<Item> &&grid_items,
			size_t new_select)
{
	grid_width_ = grid_width;
	cols_ = grid_cols;
	rows_ = grid_rows;
	items_ = std::move(grid_items);
	items_.resize(grid_cols * grid_rows);
	select_ = size_t(-1);
	if (new_select < items_.size() && items_[new_select].selectable) {
		select_ = new_select;
	}
	redraw();
}

void GridSelect::set_select (size_t new_select, bool emit_signal)
{
	if (new_select < items_.size() && items_[new_select].selectable) {
		if (new_select != select_) {
			select_ = new_select;
			if (emit_signal) {
				sig_select_changed.emit ();
			}
			GridSelect::redraw ();
		}
	}
	else {
		if (select_ < items_.size ()) {
			select_ = size_t (-1);
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
	move_cursor(Size{});

	for (unsigned y = 0; y < rows_; ++y) {
		for (unsigned x = 0; x < cols_; ++x) {
			Size pos{x * grid_width_, y};
			unsigned i = y * cols_ + x;
			const Item &item = items_[i];
			PaletteID id = PALETTE_ID_GRID;
			if (!item.selectable) {
				id = PALETTE_ID_GRID_INVALID;
			}
			else if (i == select_) {
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
	unsigned N = rows * cols;
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
		id = get_next_by_direction(id, cols, rows, direction);
	}
	while ((id != current_id) && !items[id].selectable) {
		id = get_next_by_direction(id, cols, rows, direction);
	}
	return id;
}

} // anonymous namespace

bool GridSelect::on_key (wchar_t key)
{
	if (items_.empty ()) {
		return false;
	}
	unsigned new_select;
	switch (key) {
		case LEFT:
		case RIGHT:
		case UP:
		case DOWN:
			new_select = select_direction(select_, cols_, rows_, items_, key);
			if (new_select != select_) {
				set_select (new_select);
				return true;
			}
			return false;
		case HOME:
			new_select = select_direction(-1u, cols_, rows_, items_, RIGHT);
			if (new_select != select_) {
				set_select (new_select);
				return true;
			}
			return false;
		case END:
			new_select = select_direction(-1u, cols_, rows_, items_, LEFT);
			if (new_select != select_) {
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
	unsigned click_col = mouse_event.p.x / grid_width_;
	if (click_row >= rows_ || click_col >= cols_) {
		return false;
	}
	unsigned id = click_row * cols_ + click_col;
	if (!items_[id].selectable) {
		return false;
	}
	if (id != select_) {
		set_select (id, true);
	}
	if (mouse_event.m & LEFT_CLICK) {
		sig_double_clicked.emit ();
	}
	return true;
}

} // namespace tiary::ui
} // namespace tiary
