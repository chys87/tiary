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


#ifndef TIARY_DIARY_FILTER_H
#define TIARY_DIARY_FILTER_H

#include "common/string_match.h"
#include "common/noncopyable.h"
#include <string>
#include <vector>
#include <functional>

namespace tiary {

struct DiaryEntry;

struct Filter : public std::unary_function <const DiaryEntry &, bool>
{
	/**
	 * @brief	Whether the given entry should be displayed
	 * @result	Returns @c true if the entry should be displayed
	 */
	virtual bool operator () (const DiaryEntry &) const = 0;
	virtual ~Filter () { }

	/**
	 * @brief	Filter the whole entry list
	 */
	std::vector <DiaryEntry *> filter (const std::vector <DiaryEntry *> &) const;
};

/**
 * @brief	Filter by label
 *
 * Displays only entries with one specific label
 */
struct FilterByLabel : public Filter
{
	std::wstring label;

	bool operator () (const DiaryEntry &) const;
	~FilterByLabel ();
};


/**
 * @brief	Filter by text
 *
 * Displays only entries contain some specific text
 */
struct FilterByText : public Filter, public StringMatch
{
	bool operator () (const DiaryEntry &) const;
	~FilterByText ();
};

/**
 * @brief	Filter by title
 *
 * Same as tiary::FilterByText, except that only the title is matched against
 */
struct FilterByTitle : public Filter, public StringMatch
{
	bool operator () (const DiaryEntry &) const;
	~FilterByTitle ();
};



typedef std::vector <Filter *> FilterList;

/**
 * @brief	Complex filter
 *
 * Use AND or OR to combine a number of other filters
 */
struct FilterGroup : public Filter, private FilterList, noncopyable
{
public:

	using FilterList::const_iterator;
	using FilterList::begin;
	using FilterList::end;
	using FilterList::push_back;
	using FilterList::empty;

	enum Relation { AND, OR };

	Relation relation;

	FilterGroup ();
	~FilterGroup ();

	void clear ();

	void swap (FilterGroup &);

	bool operator () (const DiaryEntry &) const;
};


} // namespace tiary

#endif // include guard
