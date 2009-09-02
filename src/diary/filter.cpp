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


#include "diary/filter.h"
#include "diary/diary.h"
#include "common/algorithm.h"


namespace tiary {

DiaryEntryList Filter::filter (const DiaryEntryList &lst) const
{
	DiaryEntryList new_lst (lst.size ());
	DiaryEntryList::iterator iw = new_lst.begin ();
	for (DiaryEntryList::const_iterator it = lst.begin ();
			it != lst.end (); ++it)
		if ((*this)(**it))
			*iw++ = *it;
	new_lst.erase (iw, new_lst.end ());
	return new_lst;
}

bool FilterByLabel::operator () (const DiaryEntry &entry) const
{
	return (entry.labels.find (label) != entry.labels.end ());
}

FilterByLabel::~FilterByLabel ()
{
}



bool FilterByText::operator () (const DiaryEntry &entry) const
{
	return basic_match (entry.title) || basic_match (entry.text);
}

FilterByText::~FilterByText ()
{
}



bool FilterByTitle::operator () (const DiaryEntry &entry) const
{
	return basic_match (entry.title);
}

FilterByTitle::~FilterByTitle ()
{
}




FilterGroup::FilterGroup ()
	: FilterList ()
	, relation (AND)
{
}

FilterGroup::~FilterGroup ()
{
	clear ();
}

void FilterGroup::clear ()
{
	std::for_each (begin (), end (), delete_fun <Filter> ());
	FilterList::clear ();
}

void FilterGroup::swap (FilterGroup &other)
{
	FilterList::swap (other);
	std::swap (relation, other.relation);
}

bool FilterGroup::operator () (const DiaryEntry &entry) const
{
	if (relation == AND) {
		for (const_iterator it = FilterList::begin (); it != FilterList::end (); ++it) {
			if (!(**it)(entry))
				return false;
		}
		return true;
	} else {
		for (const_iterator it = FilterList::begin (); it != FilterList::end (); ++it) {
			if ((**it)(entry))
				return true;
		}
		return false;
	}
}



} // namespace tiary
