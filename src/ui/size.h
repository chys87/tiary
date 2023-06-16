// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009-2023, chys <admin@CHYS.INFO>
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

#include <algorithm>

namespace tiary {
namespace ui {

/**
 * @ingroup	uisystem
 * @brief	A pair of integer coordinates to describe a size or position
 */
struct Size {
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
	Size & operator &= (const Size &other) {
		x = std::min(x, other.x);
		y = std::min(y, other.y);
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
	Size & operator |= (const Size &other) {
		x = std::max(x, other.x);
		y = std::max(y, other.y);
		return *this;
	}
};

inline constexpr Size operator + (const Size &a, const Size &b)
{
	return {a.x + b.x, a.y + b.y};
}

inline constexpr Size operator - (const Size &a, const Size &b)
{
	return {a.x - b.x, a.y - b.y};
}

inline constexpr Size operator * (const Size &a, unsigned n)
{
	return {a.x * n, a.y * n};
}

inline constexpr Size operator / (const Size &a, unsigned n)
{
	return {a.x / n, a.y / n};
}

/**
 * See tiary::ui::Size::operator&=()
 */
inline constexpr Size operator & (const Size &a, const Size &b) {
	return {std::min(a.x, b.x), std::min(a.y, b.y)};
}

/**
 * See tiary::ui::Size::operator|=()
 */
inline constexpr Size operator | (const Size &a, const Size &b) {
	return {std::max(a.x, b.x), std::max(a.y, b.y)};
}



/**
 * @defgroup	sizecompare Compare the size of two Size strucutres
 *
 * The result of a comparison between two Size strucutres has type
 * tiary::ui::SizeCompareResult. The comparison is respectively
 * carried out for the x and y components.
 * Use @c both, @c either and @c neither to analyze the results.
 *
 * Type tiary::ui::SizeCompareResult can also be explicitly cast to bool.
 *
 * @{
 *
 */

enum struct SizeCompareBoolPolicy {kBoth, kEither};

/**
 * @brief	The result of the comparison between to Size strucutres
 */
template <SizeCompareBoolPolicy p = SizeCompareBoolPolicy::kBoth>
struct SizeCompareResult {

	bool x, y;

	constexpr bool both() const { return x && y; }
	constexpr bool either() const { return x || y; }
	constexpr bool neither() const { return !x && !y; }

	explicit constexpr operator bool() const { return (p == SizeCompareBoolPolicy::kBoth) ? both() : either(); }
	constexpr bool operator !() const { return !operator bool(); }

	friend inline constexpr bool both(const SizeCompareResult &r) { return r.both(); }
	friend inline constexpr bool either(const SizeCompareResult &r) { return r.either(); }
	friend inline constexpr bool neither(const SizeCompareResult &r) { return r.neither(); }
};

inline constexpr SizeCompareResult<> operator == (Size a, Size b) {
	return {a.x == b.x, a.y == b.y};
}

inline constexpr SizeCompareResult<SizeCompareBoolPolicy::kEither> operator != (Size a, Size b) {
	return {a.x != b.x, a.y != b.y};
}

inline constexpr SizeCompareResult<> operator > (Size a, Size b) {
	return {a.x > b.x, a.y > b.y};
}

inline constexpr SizeCompareResult<> operator < (Size a, Size b) {
	return {a.x < b.x, a.y < b.y};
}

inline constexpr SizeCompareResult<> operator >= (Size a, Size b) {
	return {a.x >= b.x, a.y >= b.y};
}

inline constexpr SizeCompareResult<> operator <= (Size a, Size b) {
	return {a.x <= b.x, a.y <= b.y};
}

/// @}

} // namespace tiary::ui
} // namespace tiary

#endif // include guard
