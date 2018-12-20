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


#ifndef TIARY_UI_DATE_SELECT_H
#define TIARY_UI_DATE_SELECT_H

#include "ui/movable_object.h"
#include "ui/grid_select.h"
#include "ui/droplist.h"
#include "ui/label.h"
#include "common/datetime.h"
#include <functional>
#include <set>
#include <stdint.h>

namespace tiary {
namespace ui {


class DateSelect final : public MovableObject {
public:
	DropList year;
	GridSelect month;
	Label lbl_weekday;
	GridSelect day;

	Signal sig_date_changed;

	typedef std::set <uint32_t> SelectableDates;

	SelectableDates selectable_dates;

	DateSelect (Window &);
	~DateSelect ();

	void set_date (Date, bool emit_signal = true);
	/**
	 * The second arguments provides a list specifying all the selectable dates.
	 * If empty, all dates are selectable.
	 */
	void set_date (Date, const SelectableDates &, bool emit_signal = true);
	void set_date (Date, SelectableDates &&, bool emit_signal = true);
	Date get_date () const;

	void move_resize (Size, Size);
	void redraw ();

private:
	void update_day_list (bool emit_signal = true);

	unsigned offset; ///< Weekday of the first day of the current month
	unsigned dom; ///< Number of days in the current day
};

} // namespace tiary::ui
} // namespace tiary


#endif // include guard
