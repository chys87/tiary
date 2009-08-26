// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the so-called 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#ifndef TIARY_COMMON_FORMAT_H
#define TIARY_COMMON_FORMAT_H
/**
 * @file	common/format.h
 * @author	chys <admin@chys.info>
 * @brief	Easily formats a string
 *
 * Example:
 *
 * std::wstring s = format (L"I am %a. I am %b years old now.") << "chys" << 22;
 *
 * std::string s = format_utf8 ("%b %a") << "world" << "Hello";
 *
 * For efficiency, I suppose the pointer passed to Format/WFormat is always valid
 * in the lifetime of the class.
 *
 * The maximal number of arguments is 26, from %a to %z (case sensitive)
 *
 * Can prepend width info before the specifiers, e.g. %5a
 * (Screen width, not number of bytes or wchar_t's)
 * Prepend an extra 0 to fill the space with zeroes: e.g. %05a
 * Prepend an extra - to specify left alignment: e.g. %-4b
 * (Cannot specify 0 and - at the same time)
 *
 *
 * In fact, I really would like to wrap the printf-family functions.
 * I like the printf-family functions. But:
 *   -# snprintf is unavailable in some old systems; the return value of
 *       snprintf is non-standard in many implementations.
 *   -# swprintf can have different prototypes; and swprintf has different
 *       return values even according to C99.
 *   -# Some C libraries interpret "%s" differently from C99 in the
 *       wide-character version.
 */

#include <string>
#include <utility> // std::forward

namespace tiary {

namespace detail {

struct HexTag
{
	unsigned val;
};

template <typename T>
struct UTF8Tag
{
	T val;
};

} // namespace detail

inline detail::HexTag hex (unsigned v)
{
	detail::HexTag ret = { v };
	return ret;
}

inline detail::UTF8Tag<char> utf8 (char c)
{
	detail::UTF8Tag<char> ret = { c };
	return ret;
}

inline detail::UTF8Tag<const char *> utf8 (const char *s)
{
	detail::UTF8Tag<const char *> ret = { s };
	return ret;
}

inline detail::UTF8Tag<const std::string *> utf8 (const std::string &s)
{
	detail::UTF8Tag<const std::string *> ret = { &s };
	return ret;
}


namespace detail {
class FormatArgs {
public:
	static const unsigned MAX_ARGS = 26;

protected:
	std::wstring args;          // The concatenation of all args, always Unicode
	unsigned nargs;             // Current number of args
	unsigned offset[MAX_ARGS+1];// Offset in args

	FormatArgs () : args(), nargs (0) { offset[0] = 0; }
	~FormatArgs ();

	// wide (Unicode)
	void add (wchar_t);
	void add (const wchar_t *);
	void add (const std::wstring &);
	// Narrow (multi-byte)
	void add (char);
	void add (const char *);
	void add (const std::string &);
	// Narrow (UTF8)
	void add (UTF8Tag<char>);
	void add (UTF8Tag<const char *>);
	void add (UTF8Tag<const std::string *>);

	// Decimal
	void add (unsigned);
	// Hexidecimal
	void add (HexTag);

	std::string output (const char *) const;
	std::string output (UTF8Tag<const char *>) const;
	std::wstring output (const wchar_t *) const;
};

// Argument is one of the following:
// const char *
// UTF8Tag <const char *>
// const wchar_t *
template <typename Type>
struct FormatTypeInfo
{
	typedef void char_type;
};

template <>
struct FormatTypeInfo <UTF8Tag <const char *> >
{
	typedef char char_type;
};

template <typename T>
struct FormatTypeInfo <const T *>
{
	typedef T char_type;
};

template <typename FormatStringType>
class FormatBase : private detail::FormatArgs
{
public:

	// Constructor
	explicit FormatBase (FormatStringType fmt) : FormatArgs (), format (fmt) {}

	// Output
	operator typename std::basic_string <typename FormatTypeInfo<FormatStringType>::char_type> () const { return FormatArgs::output (format); }
	// Identical, but more explicit
	typename std::basic_string <typename FormatTypeInfo<FormatStringType>::char_type> operator () () const { return FormatArgs::output (format); }

#define TIARY_FORMATBASE_FORWARD(type)									\
	FormatBase & operator << (type x) { add (x); return *this; }	\
	FormatBase & operator ,  (type x) { add (x); return *this; }

TIARY_FORMATBASE_FORWARD(wchar_t)
TIARY_FORMATBASE_FORWARD(const wchar_t *)
TIARY_FORMATBASE_FORWARD(const std::wstring &)
TIARY_FORMATBASE_FORWARD(char)
TIARY_FORMATBASE_FORWARD(const char *)
TIARY_FORMATBASE_FORWARD(const std::string &)
TIARY_FORMATBASE_FORWARD(UTF8Tag<char>)
TIARY_FORMATBASE_FORWARD(UTF8Tag<const char *>)
TIARY_FORMATBASE_FORWARD(UTF8Tag<const std::string *>)
TIARY_FORMATBASE_FORWARD(unsigned)
TIARY_FORMATBASE_FORWARD(HexTag)

#undef TIARY_FORMATBASE_FORWARD

private:
	FormatStringType const format; // Format string
};

} // namespace detail

inline detail::FormatBase<const wchar_t *> format (const wchar_t *fmt)
{
	return detail::FormatBase<const wchar_t *> (fmt);
}

inline detail::FormatBase<const char *> format (const char *fmt)
{
	return detail::FormatBase<const char *> (fmt);
}

inline detail::FormatBase<detail::UTF8Tag<const char *> > format (detail::UTF8Tag<const char *> fmt)
{
	return detail::FormatBase<detail::UTF8Tag<const char *> > (fmt);
}

inline detail::FormatBase<detail::UTF8Tag<const char *> > format_utf8 (const char *fmt)
{
	return format (utf8 (fmt));
}

} // namespace tiary

#endif // Include guard
