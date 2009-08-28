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


#ifndef TIARY_UI_SIZE_H
#define TIARY_UI_SIZE_H

/**
 * @file	ui/size.h
 * @author	chys <admin@chys.info>
 * @brief	Defines struct tiary::ui::Size
 */

namespace tiary {
namespace ui {

/**
 * @ingroup	uisystem
 * @brief	A pair of integer coordinates to describe a size or position
 *
 * This structure has no constructor or destructor, because we want to keep
 * it a POD type. (The world will be better when C++0x is ready.)
 */
struct Size
{
	unsigned x, y;

	Size & operator += (const Size &other) { x += other.x; y += other.y; return *this; }
	Size & operator -= (const Size &other) { x -= other.x; y -= other.y; return *this; }
	Size & operator *= (unsigned n) { x *= n; y *= n; return *this; }
	Size & operator /= (unsigned n) { x /= n; y /= n; return *this; }

	/**
	 * @brief	"min" two Size structures
	 * 
	 * This function is identical to:
	 * <pre>
	 *  this->x = std::min (this->x, other.x);
	 *  this->y = std::min (this->y, other.y);
	 * </pre>
	 */
	Size & operator &= (const Size &other)
	{
		if (x > other.x)
			x = other.x;
		if (y > other.y)
			y = other.y;
		return *this;
	}
	/**
	 * @brief	"max" two Size structures
	 * 
	 * This function is identical to:
	 * <pre>
	 *  this->x = std::max (this->x, other.x);
	 *  this->y = std::max (this->y, other.y);
	 * </pre>
	 */
	Size & operator |= (const Size &other)
	{
		if (x < other.x)
			x = other.x;
		if (y < other.y)
			y = other.y;
		return *this;
	}
};

inline Size make_size (unsigned x, unsigned y)
{
	Size ret;
	ret.x = x;
	ret.y = y;
	return ret;
}

inline Size make_size ()
{
	Size ret = { 0, 0 };
	return ret;
}

inline Size operator + (const Size &a, const Size &b)
{
	return make_size (a.x + b.x, a.y + b.y);
}

inline Size operator - (const Size &a, const Size &b)
{
	return make_size (a.x - b.x, a.y - b.y);
}

inline Size operator * (const Size &a, unsigned n)
{
	return (Size (a) *= n);
}

inline Size operator / (const Size &a, unsigned n)
{
	return (Size (a) /= n);
}

/**
 * See tiary::ui::Size::operator&=()
 */
inline Size operator & (const Size &a, const Size &b)
{
	return (Size(a) &= b);
}

/**
 * See tiary::ui::Size::operator|=()
 */
inline Size operator | (const Size &a, const Size &b)
{
	return (Size (a) |= b);
}



/**
 * @defgroup	sizecompare Compare the size of two Size strucutres
 *
 * The result of == or != is a single boolean value, whose meaning is clear
 *
 * The result of a comparison between two Size strucutres has type
 * tiary::ui::SizeCompareResult. The comparison is respectively
 * carried our for the x and y components, and the result is
 * in tiary::ui::SizeCompareResult::x and tiary::ui::SizeCompareResult::y.
 * The member functions of tiary::ui::SizeCompareResult
 * helps simplify the process of checking the result.
 *
 * @{
 *
 */

inline bool operator == (Size a, Size b)
{
	return (a.x == b.x) && (a.y == b.y);
}

inline bool operator != (Size a, Size b)
{
	return !(a == b);
}

/**
 * @brief	The result of the comparison between to Size strucutres
 */
struct SizeCompareResult
{
	bool x, y;

	bool both () const { return (x && y); }
	bool either () const { return (x || y); }
	bool neither () const { return (!x && !y); }
};

inline bool both (SizeCompareResult r) { return r.both (); }
inline bool either (SizeCompareResult r) { return r.either (); }
inline bool neither (SizeCompareResult r) { return r.neither (); }

#define TIARY_UI_SIZE_DEF_COMP(op) \
	inline SizeCompareResult operator op (Size a, Size b) {	\
		SizeCompareResult ret;								\
		ret.x = (a.x op b.x);								\
		ret.y = (a.y op b.y);								\
		return ret;											\
	}

TIARY_UI_SIZE_DEF_COMP(<)
TIARY_UI_SIZE_DEF_COMP(>)
TIARY_UI_SIZE_DEF_COMP(<=)
TIARY_UI_SIZE_DEF_COMP(>=)

#undef TIARY_UI_SIZE_DEF_COMP

/// @}

} // namespace tiary::ui
} // namespace tiary

#endif // include guard
