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


#include "main/stat.h"
#include "diary/diary.h"
#include "ui/dialog_richtext.h"
#include "ui/paletteid.h"
#include "common/containers.h"
#include "common/format.h"
#include "common/unicode.h"
#include <wctype.h>
#include <math.h>

namespace tiary {

namespace {

struct Stat
{
	unsigned bytes;      // Number of bytes when represented in UTF-8
	unsigned characters; // Number of characters (excluding newlines and spaces)
	unsigned words;      // Number of non-CJK words
	unsigned cjks;       // Number of CJK characters
	unsigned paragraphs; // Number of paragraphs (excluding title)

	Stat &operator += (const Stat &other)
	{
		bytes += other.bytes;
		characters += other.characters;
		words += other.words;
		cjks += other.cjks;
		paragraphs += other.paragraphs;
		return *this;
	}
};

// Average value, obtained by deviding Stat by an integer
struct AStat
{
	double bytes;
	double characters;
	double words;
	double cjks;
	double paragraphs;
};

AStat operator / (const Stat &x, unsigned n)
{
	AStat r;
	double y = 1. / n;
	r.bytes = x.bytes * y;
	r.characters = x.characters * y;
	r.words = x.words * y;
	r.cjks = x.cjks * y;
	r.paragraphs = x.paragraphs * y;
	return r;
}

Stat stat_string (const std::wstring &text)
{
	Stat ret = {};

	bool last_alpha = false;
	wchar_t lastc = L'\n';
	for (const wchar_t *p = text.c_str (); *p; ++p) {
		wchar_t c = *p;

		if (c!=L'\n' && lastc==L'\n') {
			++ret.paragraphs;
		}
		lastc = c;

		bool this_alpha;
		if (iswprint (c)) {
			++ret.characters;
			if (ucs_iscjk (c)) {
				++ret.cjks;
				last_alpha = false;
			}
			else {
				this_alpha = ucs_isalpha (c);
			}
		}
		else {
			this_alpha = false;
		}
		if (!last_alpha && this_alpha) {
			++ret.words;
		}
		last_alpha = this_alpha;
		ret.bytes += utf8_len_by_wchar (c);
	}
	return ret;
}

Stat stat_entry (const DiaryEntry &entry)
{
	Stat ret = stat_string (entry.title);
	ret.paragraphs = 0;
	ret += stat_string (entry.text);
	return ret;
}

struct TimeSpan
{
	uint64_t min;
	uint64_t max;
};

// Returns the minimum and maximum time
TimeSpan get_span (const DiaryEntryList &lst)
{
	uint64_t min = uint64_t(0) - 1;
	uint64_t max = 0;
	for (DiaryEntryList::const_iterator it=lst.begin(), e=lst.end();
			it != e; ++it) {
		uint64_t v = (*it)->local_time.v;
		if (v < min) {
			min = v;
		}
		if (v > max) {
			max = v;
		}
	}
	TimeSpan ret = { min, max };
	return ret;
}

void append_stat (std::wstring &text, ui::RichTextLineList &lst, const Stat &info)
{
	ui::append_richtext_line (text, lst, ui::PALETTE_ID_SHOW_NORMAL,
			L"Characters          " + format_dec (info.characters, 8));
	ui::append_richtext_line (text, lst, ui::PALETTE_ID_SHOW_NORMAL,
			L"Non-CJK words       " + format_dec (info.words, 8));
	ui::append_richtext_line (text, lst, ui::PALETTE_ID_SHOW_NORMAL,
			L"CJK characters      " + format_dec (info.cjks, 8));
	ui::append_richtext_line (text, lst, ui::PALETTE_ID_SHOW_NORMAL,
			L"Paragraphs          " + format_dec (info.paragraphs, 8));
	ui::append_richtext_line (text, lst, ui::PALETTE_ID_SHOW_NORMAL,
			L"UTF-8 bytes         " + format_dec (info.bytes, 8));
}

void append_stat (std::wstring &text, ui::RichTextLineList &lst, const AStat &info)
{
	ui::append_richtext_line (text, lst, ui::PALETTE_ID_SHOW_NORMAL,
			L"Characters          " + format_double (info.characters, 8, 4));
	ui::append_richtext_line (text, lst, ui::PALETTE_ID_SHOW_NORMAL,
			L"Non-CJK words       " + format_double (info.words, 8, 4));
	ui::append_richtext_line (text, lst, ui::PALETTE_ID_SHOW_NORMAL,
			L"CJK characters      " + format_double (info.cjks, 8, 4));
	ui::append_richtext_line (text, lst, ui::PALETTE_ID_SHOW_NORMAL,
			L"Paragraphs          " + format_double (info.paragraphs, 8, 4));
	ui::append_richtext_line (text, lst, ui::PALETTE_ID_SHOW_NORMAL,
			L"UTF-8 bytes         " + format_double (info.bytes, 8, 4));
}

} // anonymous namespace

void display_statistics (const DiaryEntryList &all_entries,
		const DiaryEntryList *filtered_entries,
		const DiaryEntry *current_entry)
{
	std::wstring rich_text;
	ui::RichTextLineList rich_lines;

	Stat info = {};
	if (current_entry) {
		info = stat_entry (*current_entry);
		ui::append_richtext_line (rich_text, rich_lines, ui::PALETTE_ID_SHOW_BOLD, L"Current entry");
		append_stat (rich_text, rich_lines, info);
		ui::append_richtext_line (rich_text, rich_lines, ui::PALETTE_ID_SHOW_NORMAL, std::wstring ());
	}

	if (filtered_entries) {
		for (DiaryEntryList::const_iterator it=filtered_entries->begin(), e=filtered_entries->end();
				it != e; ++it) {
			if (*it != current_entry) {
				info += stat_entry (**it);
			}
		}
		ui::append_richtext_line (rich_text, rich_lines, ui::PALETTE_ID_SHOW_BOLD,
			L"Displayed entries   " + format_dec (filtered_entries->size (), 8));
		append_stat (rich_text, rich_lines, info);
		ui::append_richtext_line (rich_text, rich_lines, ui::PALETTE_ID_SHOW_NORMAL, std::wstring ());
	}

	{
		tiary::unordered_set<const DiaryEntry*> filtered_set;
		if (filtered_entries) {
			filtered_set.insert (filtered_entries->begin (), filtered_entries->end ());
		}
		else {
			filtered_set.insert (current_entry);
		}
		for (DiaryEntryList::const_iterator it=all_entries.begin(), e=all_entries.end();
				it != e; ++it) {
			if (filtered_set.find (*it) == filtered_set.end ()) {
				info += stat_entry (**it);
			}
		}
	}

	ui::append_richtext_line (rich_text, rich_lines, ui::PALETTE_ID_SHOW_BOLD,
		L"All entries         " + format_dec (all_entries.size (), 8));
	append_stat (rich_text, rich_lines, info);
	ui::append_richtext_line (rich_text, rich_lines, ui::PALETTE_ID_SHOW_NORMAL, std::wstring ());

	TimeSpan span = get_span (all_entries);
	unsigned days = extract_date_from_datetime (span.max) - extract_date_from_datetime (span.min) + 1;
	ui::append_richtext_line (rich_text, rich_lines, ui::PALETTE_ID_SHOW_NORMAL,
			Format (L"Date span           %8a days (%b - %c)")
			<< days
			<< format_datetime (span.min, L"%m/%d/%Y")
			<< format_datetime (span.max, L"%m/%d/%Y")
		);
	ui::append_richtext_line (rich_text, rich_lines, ui::PALETTE_ID_SHOW_NORMAL,
		L"Entries per day     " + format_double (double (all_entries.size ()) / days, 8, 4));
	ui::append_richtext_line (rich_text, rich_lines, ui::PALETTE_ID_SHOW_NORMAL, std::wstring ());

	ui::append_richtext_line (rich_text, rich_lines, ui::PALETTE_ID_SHOW_BOLD, L"Average entry");
	AStat ainfo = info / all_entries.size ();
	append_stat (rich_text, rich_lines, ainfo);
	ui::append_richtext_line (rich_text, rich_lines, ui::PALETTE_ID_SHOW_NORMAL, std::wstring ());

	ui::dialog_richtext (L"Statistics", rich_text, rich_lines);
}

} // namespace tiary
