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


#ifndef TIARY_DIARY_CONFIG_H
#define TIARY_DIARY_CONFIG_H

#include "common/containers.h"
#include <string>

namespace tiary {

#define GLOBAL_OPTION_FILE ".tiary"

#define GLOBAL_OPTION_DEFAULT_FILE     "default_file"
#define GLOBAL_OPTION_EXPAND_LINES     "expand_lines"
#define GLOBAL_OPTION_EDITOR           "editor"
#define GLOBAL_OPTION_DATETIME_FORMAT  "time_format"
#define GLOBAL_OPTION_LONGTIME_FORMAT  "long_time_format"




struct OptionDescription {
	const char *name;
	const char *default_value; ///< Always use UTF-8
};

extern const OptionDescription g_global_option_descriptions[];
extern const OptionDescription g_perfile_option_descriptions[];

class OptionGroupBase
{
	typedef StringUnorderedMap DataType;
	DataType data;

protected:

	void reset (const OptionDescription *); // Does not affect unknown options

public:
	void set (const char *, const char *);
	void set (const char *, const std::string &);
	void set (const char *, const std::wstring &);
	void set (const char *, unsigned);
	void set (const char *, bool);

	void set (const std::string &, const char *);
	void set (const std::string &, const std::string &);
	void set (const std::string &, const std::wstring &);
	void set (const std::string &, unsigned);
	void set (const std::string &, bool);

	const std::string &get (const char *) const;
	unsigned get_num (const char *) const;
	bool get_bool (const char *) const;
	std::wstring get_wstring (const char *) const;

	const std::string &get (const std::string &) const;
	unsigned get_num (const std::string &) const;
	bool get_bool (const std::string &) const;
	std::wstring get_wstring (const std::string &) const;

	// Sometimes we may need to visit everyone. Force const
	typedef DataType::const_iterator const_iterator;
	typedef const_iterator iterator;
	iterator begin () const { return data.begin (); }
	iterator end () const { return data.end (); }
};


template <const OptionDescription *Descriptions>
class OptionGroup : public OptionGroupBase
{
public:
	static const OptionDescription *get_descriptions ()
	{
		return Descriptions;
	}

	OptionGroup ()
	{
		reset ();
	}

	void reset ()
	{
		OptionGroupBase::reset (Descriptions);
	}
};

typedef OptionGroup <g_global_option_descriptions> GlobalOptionGroup;
typedef OptionGroup <g_perfile_option_descriptions> PerFileOptionGroup;


} // namespace tiary

#endif // Include guard
