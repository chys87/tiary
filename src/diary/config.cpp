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


#include "diary/config.h"
#include "common/misc.h"
#include "common/format.h"
#include "common/unicode.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

namespace tiary {

extern const OptionDescription g_global_option_descriptions[] = {
	{ GLOBAL_OPTION_DEFAULT_FILE      , "" },
	{ GLOBAL_OPTION_EXPAND_LINES      , "4" },
	{ GLOBAL_OPTION_EDITOR            , "$EDITOR|rvim|vim|emacs -nw|nano|gedit|kwrite" },
	{ GLOBAL_OPTION_PAGER             , "$PAGER|less -icM|more" },
	{ GLOBAL_OPTION_DATETIME_FORMAT   , "%m/%d/%Y" },
	{ 0, 0 }
};

extern const OptionDescription g_perfile_option_descriptions[] = {
	{ 0, 0 }
};


void OptionGroupBase::reset (const OptionDescription *descriptions)
{
	for (const OptionDescription *desc = descriptions; desc->name; ++desc)
		data[desc->name] = desc->default_value;
}

void OptionGroupBase::set (const char *name, const char *value)
{
	set (std::string (name), value);
}

void OptionGroupBase::set (const char *name, const std::string &value)
{
	set (std::string (name), value);
}

void OptionGroupBase::set (const char *name, const std::wstring &value)
{
	set (name, wstring_to_utf8 (value));
}

void OptionGroupBase::set (const char *name, unsigned value)
{
	set (std::string (name), value);
}

void OptionGroupBase::set (const char *name, bool value)
{
	set (std::string (name), value);
}

void OptionGroupBase::set (const std::string &name, const char *value)
{
	data[name] = value;
}

void OptionGroupBase::set (const std::string &name, const std::string &value)
{
	data[name] = value;
}

void OptionGroupBase::set (const std::string &name, const std::wstring &value)
{
	set (name, wstring_to_utf8 (value));
}

void OptionGroupBase::set (const std::string &name, unsigned value)
{
	data[name] = format_utf8("%a") << value;
}

void OptionGroupBase::set (const std::string &name, bool value)
{
	data[name].assign (1, value ? '1' : '0');
}

const std::string &OptionGroupBase::get (const char *name) const
{
	return get (std::string (name));
}

unsigned OptionGroupBase::get_num (const char *name) const
{
	return get_num (std::string (name));
}

bool OptionGroupBase::get_bool (const char *name) const
{
	return get_bool (std::string (name));
}

std::wstring OptionGroupBase::get_wstring (const char *name) const
{
	return get_wstring (std::string (name));
}

const std::string &OptionGroupBase::get (const std::string &name) const
{
	DataType::const_iterator it = data.find (name);
	if (it == data.end ()) { // Not found. But we must return a reference
		static const std::string error_return;
		return error_return;
	}
	return it->second;
}

unsigned OptionGroupBase::get_num (const std::string &name) const
{
	return strtoul (get (name).c_str (), 0, 10);
}

bool OptionGroupBase::get_bool (const std::string &name) const
{
	return (get (name)[0] == '1');
}

std::wstring OptionGroupBase::get_wstring (const std::string &name) const
{
	return utf8_to_wstring (get (name));
}

} // namespace tiary
