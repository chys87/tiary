// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2018, 2019, chys <admin@CHYS.INFO>
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
#include "common/algorithm.h"
#include <wctype.h>
#include <math.h>
#include <unordered_set>

namespace tiary {

namespace {

struct Stat
{
	unsigned bytes;      // Number of bytes when represented in UTF-8
	unsigned characters; // Number of characters (including newlines and spaces)
	unsigned char_graph; // Number of characters (excluding newlines and spaces)
	unsigned words;      // Number of non-CJK words
	unsigned cjks;       // Number of CJK characters
	unsigned paragraphs; // Number of paragraphs (excluding title)
};

// The result is _added_ to ret
void stat_string(Stat *ret, const std::wstring &text) {
	ret->characters += text.length();

	bool last_alpha = false;
	wchar_t lastc = L'\n';
	for (wchar_t c : text) {

		if (c != L'\n' && lastc == L'\n') {
			++ret->paragraphs;
		}
		lastc = c;

		bool this_alpha = false;
		if (iswgraph (c)) {
			++ret->char_graph;
			if (ucs_iscjk (c)) {
				++ret->cjks;
			} else {
				this_alpha = ucs_isalpha (c);
			}
		}
		if (!last_alpha && this_alpha) {
			++ret->words;
		}
		last_alpha = this_alpha;
		ret->bytes += utf8_len_by_wchar (c);
	}
}

// The result is _added_ to ret
void stat_entry(Stat *ret, const DiaryEntry &entry) {
	unsigned old_paragraphs = ret->paragraphs;
	stat_string (ret, entry.title);
	ret->paragraphs = old_paragraphs;
	stat_string (ret, entry.text);
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
	for (const DiaryEntry *entry : lst) {
		uint64_t v = entry->local_time.get_value();
		if (v < min) {
			min = v;
		}
		if (v > max) {
			max = v;
		}
	}
	return {min, max};
}

void append_stat (std::wstring &text, ui::RichTextLineList &lst, const Stat &info)
{
	ui::append_richtext_line (text, lst, ui::PALETTE_ID_SHOW_NORMAL,
			L"Characters          ", format_dec (info.characters, 8));
	ui::append_richtext_line (text, lst, ui::PALETTE_ID_SHOW_NORMAL,
			L"Printable characters", format_dec (info.char_graph, 8));
	ui::append_richtext_line (text, lst, ui::PALETTE_ID_SHOW_NORMAL,
			L"Non-CJK words       ", format_dec (info.words, 8));
	ui::append_richtext_line (text, lst, ui::PALETTE_ID_SHOW_NORMAL,
			L"CJK characters      ", format_dec (info.cjks, 8));
	ui::append_richtext_line (text, lst, ui::PALETTE_ID_SHOW_NORMAL,
			L"Paragraphs          ", format_dec (info.paragraphs, 8));
	ui::append_richtext_line (text, lst, ui::PALETTE_ID_SHOW_NORMAL,
			L"UTF-8 bytes         ", format_dec (info.bytes, 8));
}

void append_average_stat (std::wstring &text, ui::RichTextLineList &lst, const Stat &info, unsigned n)
{
	double x = 1. / n;
	ui::append_richtext_line (text, lst, ui::PALETTE_ID_SHOW_NORMAL,
			L"Characters          ", format_double (info.characters * x, 8, 4));
	ui::append_richtext_line (text, lst, ui::PALETTE_ID_SHOW_NORMAL,
			L"Printable characters", format_double (info.char_graph * x, 8, 4));
	ui::append_richtext_line (text, lst, ui::PALETTE_ID_SHOW_NORMAL,
			L"Non-CJK words       ", format_double (info.words * x, 8, 4));
	ui::append_richtext_line (text, lst, ui::PALETTE_ID_SHOW_NORMAL,
			L"CJK characters      ", format_double (info.cjks * x, 8, 4));
	ui::append_richtext_line (text, lst, ui::PALETTE_ID_SHOW_NORMAL,
			L"Paragraphs          ", format_double (info.paragraphs * x, 8, 4));
	ui::append_richtext_line (text, lst, ui::PALETTE_ID_SHOW_NORMAL,
			L"UTF-8 bytes         ", format_double (info.bytes * x, 8, 4));
}

} // anonymous namespace

void display_statistics (const DiaryEntryList &all_entries,
		const DiaryEntryList *filtered_entries,
		const DiaryEntry *current_entry)
{
	std::wstring rich_text;
	rich_text.reserve (4096); // Just a rough guess
	ui::RichTextLineList rich_lines;
	rich_lines.reserve (128); // Just a rough guess

	Stat info = {};
	if (current_entry) {
		stat_entry(&info, *current_entry);
		ui::append_richtext_line (rich_text, rich_lines, ui::PALETTE_ID_SHOW_BOLD, L"Current entry");
		append_stat (rich_text, rich_lines, info);
		ui::append_richtext_line (rich_text, rich_lines, ui::PALETTE_ID_SHOW_NORMAL);
	}

	// Show filtered entries
	if (filtered_entries) {
		for (const DiaryEntry *entry : *filtered_entries) {
			if (entry != current_entry) {
				stat_entry(&info, *entry);
			}
		}
		ui::append_richtext_line (rich_text, rich_lines, ui::PALETTE_ID_SHOW_BOLD,
			L"Displayed entries   ", format_dec (filtered_entries->size (), 8));
		append_stat (rich_text, rich_lines, info);
		ui::append_richtext_line (rich_text, rich_lines, ui::PALETTE_ID_SHOW_NORMAL);
	}

	// Add the remaining entries
	{
		std::set<const DiaryEntry*> visited_set;
		if (filtered_entries) {
			visited_set.insert(filtered_entries->begin(), filtered_entries->end());
		}
		visited_set.insert(current_entry);
		for (const DiaryEntry *entry : all_entries) {
			if (visited_set.find(entry) == visited_set.end()) {
				stat_entry(&info, *entry);
			}
		}
	}

	ui::append_richtext_line (rich_text, rich_lines, ui::PALETTE_ID_SHOW_BOLD,
		L"All entries         ", format_dec (all_entries.size (), 8));
	append_stat (rich_text, rich_lines, info);
	ui::append_richtext_line (rich_text, rich_lines, ui::PALETTE_ID_SHOW_NORMAL);

	TimeSpan span = get_span (all_entries);
	unsigned days = extract_date_from_datetime (span.max) - extract_date_from_datetime (span.min) + 1;
	ui::append_richtext_line (rich_text, rich_lines, ui::PALETTE_ID_SHOW_NORMAL,
			std::wstring(Format(L"Date span           %8a days (%b - %c)")
			<< days
			<< format_datetime(span.min, L"%m/%d/%Y")
			<< format_datetime(span.max, L"%m/%d/%Y"))
		);
	ui::append_richtext_line (rich_text, rich_lines, ui::PALETTE_ID_SHOW_NORMAL,
		L"Entries per day     ", format_double (double (all_entries.size ()) / days, 8, 4));

	unsigned n_labels = 0;
	unsigned n_distinct_labels;
	// Count the number of distinct labels
	{
		std::unordered_set<std::wstring_view> all_labels;
		for (const DiaryEntry *entry : all_entries) {
			n_labels += entry->labels.size ();
			for (const auto &s: entry->labels)
				all_labels.insert(s);
		}
		n_distinct_labels = all_labels.size ();
	}
	ui::append_richtext_line (rich_text, rich_lines, ui::PALETTE_ID_SHOW_NORMAL,
			L"Labels              ", format_dec (n_distinct_labels, 8));
	ui::append_richtext_line (rich_text, rich_lines, ui::PALETTE_ID_SHOW_NORMAL,
			L"Labels per entry    ", format_double (double (n_labels) / all_entries.size (), 8, 4));

	ui::append_richtext_line (rich_text, rich_lines, ui::PALETTE_ID_SHOW_NORMAL);

	ui::append_richtext_line (rich_text, rich_lines, ui::PALETTE_ID_SHOW_BOLD, L"Average entry");
	append_average_stat (rich_text, rich_lines, info, all_entries.size ());
	ui::append_richtext_line (rich_text, rich_lines, ui::PALETTE_ID_SHOW_NORMAL);

	ui::dialog_richtext (L"Statistics", rich_text, rich_lines);
}

} // namespace tiary
