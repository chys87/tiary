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

#include "ui/scrollbar.h"
#include <stdint.h>

namespace tiary {
namespace ui {

ScrollBarInfo scrollbar_info (unsigned height, unsigned num, unsigned pagestart)
{
	unsigned pos;
	unsigned size;

	if (num <= height) {
		pos = 0;
		size = height;
	}
	else {
		size = height * height / num;
		if (size == 0) {
			size = 1;
		}
		pos = (height - size + 1) * pagestart / (num - height + 1);
		if (pos > height - size) {
			pos = height - size;
		}
	}

	ScrollBarInfo ret = { pos, size };
	return ret;
}

unsigned scrollbar_click (unsigned height, unsigned num, unsigned click_pos)
{
	if (num <= height) {
		return 0;
	}
	else {
		unsigned bar_size = height * height / num;
		if (bar_size == 0) {
			bar_size = 1;
		}
		if (int (click_pos -= bar_size / 2) < 0) {
			return 0;
		}
		num -= height;
		height -= bar_size;
		if (click_pos >= height) {
			return num;
		}

		return (num + 1) * click_pos / height;
	}
}

} // namespace tiary::ui
} // namespace tiary
