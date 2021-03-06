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


/**
 * @file	diary/config.cpp
 * @author	chys <admin@chys.info>
 * @brief	Defines default value for preferences and implements tiary::OptionGroupBase
 */

#include "diary/config.h"
#include "common/format.h"
#include "common/unicode.h"
#include <string.h>
#include <stdlib.h>

namespace tiary {

extern const OptionDescription g_global_option_descriptions[] = {
	{ GLOBAL_OPTION_DEFAULT_FILE      , "" },
	{ GLOBAL_OPTION_EXPAND_LINES      , "4" },
	{ GLOBAL_OPTION_EDITOR            , "rvim|vim|emacs -nw|nano|gedit|kwrite" },
	{ GLOBAL_OPTION_DATETIME_FORMAT   , "%m/%d/%Y" },
	{ GLOBAL_OPTION_LONGTIME_FORMAT   , "%W %B %d, %Y  %h:%M:%S %P" },
	{ GLOBAL_OPTION_RECENT_FILES      , "4" },
	{ 0, 0 }
};

extern const OptionDescription g_perfile_option_descriptions[] = {
	{ PERFILE_OPTION_MODTIME          , "0" },
	{ 0, 0 }
};


void OptionGroupBase::reset (const OptionDescription *descriptions)
{
	for (const OptionDescription *desc = descriptions; desc->name; ++desc) {
		data[desc->name] = desc->default_value;
	}
}

void OptionGroupBase::set (const char *name, const char *value)
{
	data[name] = value;
}

void OptionGroupBase::set(const char *name, std::string_view value) {
	data[name] = value;
}

void OptionGroupBase::set(const char *name, std::wstring_view value) {
	data[name] = wstring_to_utf8(value);
}

void OptionGroupBase::set(const char *name, unsigned value) {
	data[name] = format_dec_narrow(value);
}

void OptionGroupBase::set (const char *name, bool value)
{
	data[name].assign(1, value ? '1' : '0');
}

void OptionGroupBase::set (const std::string &name, const char *value)
{
	data[name] = value;
}

void OptionGroupBase::set(const std::string &name, std::string_view value) {
	data[name] = value;
}

void OptionGroupBase::set(const std::string &name, std::wstring_view value) {
	data[name] = wstring_to_utf8(value);
}

void OptionGroupBase::set (const std::string &name, unsigned value)
{
	data[name] = format_dec_narrow(value);
}

void OptionGroupBase::set (const std::string &name, bool value)
{
	data[name].assign (1, value ? '1' : '0');
}

const std::string &OptionGroupBase::get(const char *name) const {
	DataType::const_iterator it = data.find(name);
	if (it == data.end ()) { // Not found. But we must return a reference
		return error_return_;
	}
	return it->second;
}

unsigned OptionGroupBase::get_num (const char *name) const
{
	return strtoul(get(name).c_str(), 0, 10);
}

bool OptionGroupBase::get_bool (const char *name) const
{
	return (get(name)[0] == '1');
}

std::wstring OptionGroupBase::get_wstring (const char *name) const
{
	return utf8_to_wstring(get(name));
}

const std::string &OptionGroupBase::get (const std::string &name) const
{
	DataType::const_iterator it = data.find (name);
	if (it == data.end ()) { // Not found. But we must return a reference
		return error_return_;
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

const std::string OptionGroupBase::error_return_;

} // namespace tiary
