// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2019, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#include "diary/filter.h"
#include "diary/diary.h"
#include "common/algorithm.h"
#include "common/string.h"


namespace tiary {

DiaryEntryList Filter::filter (const DiaryEntryList &lst) const
{
	DiaryEntryList new_lst;
	for (DiaryEntry *entry: lst) {
		if ((*this)(*entry)) {
			new_lst.push_back(entry);
		}
	}
	return new_lst;
}

FilterByLabel::FilterByLabel(std::wstring_view label) {
	for (auto& label: split_string_view(label, L',')) {
		labels_.emplace_back(label.data(), label.size());
	}
}

bool FilterByLabel::operator () (const DiaryEntry &entry) const
{
	for (auto& label: labels_) {
		if (entry.labels.find(label) == entry.labels.end())
			return false;
	}
	return true;
}

std::wstring FilterByLabel::label() const {
	std::wstring res;
	for (auto& label: labels_) {
		if (!res.empty())
			res += L',';
		res += label;
	}
	return res;
}

bool FilterByText::operator () (const DiaryEntry &entry) const
{
	return matcher_.basic_match(entry.title) || matcher_.basic_match(entry.text);
}

FilterByText::~FilterByText ()
{
}



bool FilterByTitle::operator () (const DiaryEntry &entry) const
{
	return matcher_.basic_match(entry.title);
}

FilterByTitle::~FilterByTitle ()
{
}




bool FilterGroup::operator () (const DiaryEntry &entry) const
{
	if (relation_ == AND) {
		for (const auto &filter_ptr : filters_) {
			if (!(*filter_ptr)(entry)) {
				return false;
			}
		}
		return true;
	}
	else {
		for (const auto &filter_ptr : filters_) {
			if ((*filter_ptr)(entry)) {
				return true;
			}
		}
		return false;
	}
}



} // namespace tiary
