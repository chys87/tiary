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
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace tiary {

struct DiaryEntry;

struct Filter {
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



typedef std::vector<std::unique_ptr<Filter>> FilterList;

/**
 * @brief	Complex filter
 *
 * Use AND or OR to combine a number of other filters
 */
struct FilterGroup : public Filter, private FilterList
{
public:

	using FilterList::const_iterator;
	using FilterList::begin;
	using FilterList::end;
	using FilterList::emplace_back;
	using FilterList::push_back;
	using FilterList::empty;
	using FilterList::clear;

	enum Relation { AND, OR };
	Relation relation = AND;

	bool operator () (const DiaryEntry &) const;
};


} // namespace tiary

#endif // include guard
