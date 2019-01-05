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


#ifndef TIARY_DIARY_FILTER_H
#define TIARY_DIARY_FILTER_H

#include "common/string_match.h"
#include <memory>
#include <string>
#include <vector>

namespace tiary {

struct DiaryEntry;

class Filter {
public:
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
struct FilterByLabel final : public Filter {
public:
	explicit FilterByLabel(const std::wstring &label) : label_(label) {}
	explicit FilterByLabel(std::wstring &&label) : label_(std::move(label)) {}
	const std::wstring &label() const { return label_; }
	bool operator()(const DiaryEntry &) const override;
	~FilterByLabel ();

private:
	std::wstring label_;
};


/**
 * @brief	Filter by text
 *
 * Displays only entries contain some specific text
 */
class FilterByText final : public Filter {
public:
	bool operator () (const DiaryEntry &) const;

	FilterByText(const std::wstring &pattern, bool use_regex = false) : matcher_(pattern, use_regex) {}
	~FilterByText ();

	const std::wstring &get_pattern() const { return matcher_.get_pattern(); }
	bool get_use_regex() const { return matcher_.get_use_regex(); }
	explicit operator bool() const { return static_cast<bool>(matcher_); }

private:
	StringMatch matcher_;
};

/**
 * @brief	Filter by title
 *
 * Same as tiary::FilterByText, except that only the title is matched against
 */
class FilterByTitle final : public Filter {
public:
	bool operator () (const DiaryEntry &) const;

	FilterByTitle(const std::wstring &pattern, bool use_regex = false) : matcher_(pattern, use_regex) {}
	~FilterByTitle ();

	const std::wstring &get_pattern() const { return matcher_.get_pattern(); }
	bool get_use_regex() const { return matcher_.get_use_regex(); }
	explicit operator bool() const { return static_cast<bool>(matcher_); }

private:
	StringMatch matcher_;
};



/**
 * @brief	Complex filter
 *
 * Use AND or OR to combine a number of other filters
 */
struct FilterGroup final : public Filter {
public:

	enum Relation { AND, OR };

	bool operator () (const DiaryEntry &) const;

	Relation relation() const { return relation_; }
	void relation(Relation relation) { relation_ = relation; }

	auto begin() const { return filters_.begin(); }
	auto end() const { return filters_.end(); }
	void add(Filter *filter) { filters_.emplace_back(filter); }
	bool empty() const { return filters_.empty(); }
	void clear() { filters_.clear(); }

private:
	std::vector<std::unique_ptr<Filter>> filters_;
	Relation relation_ = AND;
};


} // namespace tiary

#endif // include guard
