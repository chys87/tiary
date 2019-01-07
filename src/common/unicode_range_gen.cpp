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

#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace {

struct Range {
	// [lo, hi]
	uint32_t lo;
	uint32_t hi;
	std::string_view value;

	constexpr Range(uint32_t l, uint32_t h, std::string_view v) : lo(l), hi(h), value(v) {}
	constexpr Range(uint32_t l, std::string_view v) : lo(l), hi(l), value(v) {}
};

inline bool operator < (const Range &a, const Range &b) {
	return (a.lo < b.lo) || (a.lo == b.lo && a.hi < b.hi);
}

std::vector<Range> ranges_from_string(std::u32string_view string, std::string_view value) {
	std::vector<Range> res;
	for (char32_t c : string) {
		res.push_back({c, c, value});
	}
	return res;
}

struct FunctionMeta {
	std::string_view ret_type;
	std::string_view function_name;
	std::string_view default_value;
};

void merge_ranges(std::vector<Range> &ranges) {
	std::sort(ranges.begin(), ranges.end());
	for (auto it = ranges.begin(); it != ranges.end(); ) {
		auto jt = it + 1;
		while (jt != ranges.end() && jt->lo == it->hi + 1) {
			it->hi = jt->hi;
			// lo > hi marks an empty range
			jt->lo = 1;
			jt->hi = 0;
			++jt;
		}
		it = jt;
	}

	ranges.erase(std::remove_if(ranges.begin(), ranges.end(), [](const Range &range) { return range.lo > range.hi; }), ranges.end());
}

std::string make_function_header(const FunctionMeta &meta) {
	return std::string(meta.ret_type) + " " + std::string(meta.function_name) + "(char32_t c) {\n";
}

std::string make_function_footer(const FunctionMeta &meta) {
	return "}\n\n";
}

std::string make_function_body_if_else(const FunctionMeta &meta, const std::vector<Range> &ranges) {
	std::ostringstream oss;
	oss << std::hex;
	for (const Range &range : ranges) {
		if (range.lo == range.hi) {
			oss << "\tif (c == 0x" << range.lo << ")\n";
		} else {
			oss << "\tif (c >= 0x" << range.lo << " && c <= 0x" << range.hi << ")\n";
		}
		oss << "\t\treturn " << range.value << ";\n";
	}
	oss << "\treturn " << meta.default_value << ";\n";
	return oss.str();
}

std::string make_function_body_case_ranges(const FunctionMeta &meta, const std::vector<Range> &ranges) {
	std::ostringstream oss;
	oss << std::hex;

	oss << "\tswitch (c) {\n";

	for (auto it = ranges.begin(); it != ranges.end(); ) {
		auto jt = it;
		for (; jt != ranges.end() && jt->value == it->value; ++jt) {
			if (jt->lo == jt->hi) {
				oss << "\tcase 0x" << jt->lo << ":\n";
			} else {
				oss << "\tcase 0x" << jt->lo << " ... 0x" << jt->hi << ":\n";
			}
		}
		oss << "\t\treturn " << it->value << ";\n";
		it = jt;
	}

	oss << "\tdefault:\n";
	oss << "\t\treturn " << meta.default_value << ";\n";
	oss << "\t}\n";

	return oss.str();
}

std::string gen_function(const FunctionMeta &meta, const std::vector<Range> &ranges_in) {

	// Return value ==> Range list
	std::map<std::string_view, std::vector<Range>> range_groups;
	for (const Range &range : ranges_in) {
		range_groups[range.value].push_back(range);
	}

	std::vector<Range> final_ranges;
	for (auto &group_pair : range_groups) {
		merge_ranges(group_pair.second);
		final_ranges.insert(final_ranges.end(), group_pair.second.begin(), group_pair.second.end());
	}

	std::sort(final_ranges.begin(), final_ranges.end());

	std::ostringstream oss;

	oss << make_function_header(meta);
	oss << "#ifdef HAVE_CASE_RANGES\n";
	oss << make_function_body_case_ranges(meta, final_ranges);
	oss << "#else\n";
	oss << make_function_body_if_else(meta, final_ranges);
	oss << "#endif\n";
	oss << make_function_footer(meta);

	return oss.str();
}


} // namespace

int main() {
	std::cout << "#include \"common/unicode.h\"" << std::endl;
	std::cout << std::endl;
	std::cout << "namespace tiary {" << std::endl;
	std::cout << std::endl;

	std::cout << gen_function({"bool", "ucs_isalpha", "false"}, {
		{L'A', L'Z', "true"}, {L'a', L'z', "true"}, // Basic latin
		{0x00c0, 0x00d6, "true"}, {0x00d8, 0x00f6, "true"}, {0x00f8, 0x00FF, "true"}, // Latin-1 supplement
		{0x0100, 0x017F, "true"}, // Latin Extended-A
		{0x0180, 0x024F, "true"}, // Latin Extended-B
		{0x0250, 0x02AE, "true"}, // IPA
		{0x02B0, 0x02fe, "true"}, // Spacing modifier letters
		{0x0385, 0x03ff, "true"}, // Greek
		{0x0400, 0x04ff, "true"}, // Cyrillic
		{0x0500, 0x0513, "true"}, // Cyrillic supplement
		{0x0531, 0x0558, "true"}, {0x0561, 0x0588, "true"}, // Armenian
		{0x05d0, 0x05ef, "true"}, // Hebrew
		{0x0621, 0x0655, "true"}, {0x066e, 0x06d3, "true"}, {0x06fa, 0x06fc, "true"}, {0x06ff, 0x06ff, "true"}, // Arabic/Urdu/Farsi
		{0x0710, 0x074f, "true"}, // Syriac
		{0x0750, 0x076d, "true"}, // Arabic supplement
		{0x0780, 0x07a5, "true"}, // Thaana
		{0x07ca, 0x07f3, "true"}, // N'Ko
		{0x0800, 0x083f, "true"}, // Samaritan
		{0x0840, 0x085f, "true"}, // Mandaic
		{0x0860, 0x086f, "true"}, // Syriac Supplement
		{0x2c60, 0x2c77, "true"}, // Latin Extended-C
	});

	std::cout << gen_function({"bool", "ucs_iscjk", "false"}, {
		{0, 0x33FF, "false"}, // Quickly return false for ASCII
		{0x4E00, 0x9fff, "true"}, // CJK Unified Ideographs
		{0x3400, 0x4dbf, "true"}, // CJK Unified Ideographs Extension A (Unicode 3.0, 1999)
		{0x20000, 0x2A6DF, "true"}, // CJK Unified Ideographs Extension B (Unicode 3.1, 2001): 20000–2A6DF
		{0x2A700, 0x2B73F, "true"}, // CJK Unified Ideographs Extension C (Unicode 5.2, 2009): 2A700–2B73F
		{0x2B740, 0x2B81F, "true"}, // CJK Unified Ideographs Extension D (Unicode 6.0, 2010): 2B740–2B81F
		{0x2B820, 0x2CEAF, "true"}, // CJK Unified Ideographs Extension E (Unicode 8.0, 2015): 2B820–2CEAF
		{0x2CEB0, 0x2EBEF, "true"}, // CJK Unified Ideographs Extension F (Unicode 10.0, 2017): 2CEB0–2EBEF
		{0xF900, 0xFAFF, "true"}, // CJK Compatibility Ideographs: F900–FAFF
	});

	std::cout << gen_function({"bool", "allow_line_beginning", "true"},
			ranges_from_string(U"!),.:;?]}¨·ˇˉ―‖’”…∶、。〃々〉》」』】〕〗！＂＇），．：；？］｀｜｝～￠", "false"));

	std::cout << gen_function({"bool", "allow_line_end", "true"},
			ranges_from_string(U"([{·‘“〈《「『【〔〖（．［｛￡￥", "false"));

	std::cout << "} // namespace tiary" << std::endl;
}
