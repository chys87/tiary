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


#ifndef TIARY_UI_OUTPUT_H
#define TIARY_UI_OUTPUT_H

#include "ui/size.h"
#include "ui/ui.h"
#include <string>

namespace tiary {
namespace ui {

// Forward declarations
class Window;
class Control;
namespace detail { template <typename> class Output; }
inline detail::Output<Window> operator << (Window &, Size);
inline detail::Output<Control> operator << (Control &, Size);




namespace detail {

struct LenStr
{
	const wchar_t *s;
	size_t len;
};

struct AttributeToggle
{
	Attr attr;
};

/**
 * @brief	Helper output class for ui::Window and ui::Control
 */
template <typename C> class Output
{
public:

	// Change output position
	Output &operator << (Size newpos)
	{
		pos = newpos;
		return *this;
	}
	// Change palette
	Output &operator << (PaletteID id)
	{
		obj.choose_palette (id);
		return *this;
	}
	// Toggle attributes
	Output &operator << (AttributeToggle toggle)
	{
		obj.attribute_toggle (toggle.attr);
		return *this;
	}
	Output &operator << (wchar_t c)
	{
		pos = obj.put (pos, c);
		return *this;
	}
	Output &operator << (const wchar_t *s)
	{
		pos = obj.put (pos, s);
		return *this;
	}
	Output &operator << (LenStr s)
	{
		pos = obj.put (pos, s.s, s.len);
		return *this;
	}
	Output &operator << (const std::wstring &s)
	{
		pos = obj.put (pos, s);
		return *this;
	}

	operator Size () const
	{
		return pos;
	}

private:
	C &obj;
	Size pos;

	void operator = (const Output &);
	// Constructor is intentionally private,
	// thus it's only callable from friend functions
	Output (C &obj_, Size pos_)
		: obj (obj_), pos (pos_)
	{
	}

	friend Output<Window> ui::operator << (Window &, Size);
	friend Output<Control> ui::operator << (Control &, Size);
};

} // namespace detail

inline detail::LenStr str (const wchar_t *s, size_t len)
{
	detail::LenStr ret = { s, len };
	return ret;
}

inline detail::AttributeToggle toggle (Attr attr)
{
	detail::AttributeToggle ret = { attr };
	return ret;
}

inline detail::Output<Window> operator << (Window &win, Size pos)
{
	return detail::Output<Window> (win, pos);
}

inline detail::Output<Window> operator << (Window *win, Size pos)
{
	return (*win << pos);
}

inline detail::Output<Control> operator << (Control &ctrl, Size pos)
{
	return detail::Output<Control> (ctrl, pos);
}

inline detail::Output<Control> operator << (Control *ctrl, Size pos)
{
	return (*ctrl << pos);
}

} // namespace tiary::ui
} // namespace tiary


#endif // include guard
