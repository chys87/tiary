// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009-2023, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


/*
 * ~/.tiary format:
 * <?xml ... ?>
 * <tiary>
 *  <option name=".." value=".." />
 *  <option name=".." value=".." />
 *  ...
 * </tiary>
 *
 *
 * Diary file format (unencrypted file):
 *
 * The file is compressed with bzip2.
 * When decompressed, the format is an XML:
 *
 * <?xml version="1.0" encoding="UTF-8" ?>
 * <tiary>
 *  <option name=".." value=".." />
 *  <option name=".." value=".." />
 *  ....
 *  <entry>
 *    <time local="%Y-%m-%d %H:%M:%S" />
 *    <label name="..." />
 *    <label name="..." />
 *    ....
 *    <title>.....</title>
 *    <text>.....</text>
 *  </entry>
 *  ...
 * </tiary>
 *
 *
 * Diary file format (encrypted file 2009, obsolete):
 * 0000~000F Signature
 * 0010~001F MD5(passwoord+salt1)
 * 0020~     encrypt (bzip2 (XML), password)
 *
 * Diary file format (encrypted file 2018):
 * 0000~000F Signature
 * 0010~004F SHA512(salt_2018a + password + salt_2018b)
 * 0050~     evp_aes_encrypt(bzip2(XML), password)
 */

#include "diary/file.h"
#include "diary/config.h"
#include "diary/diary.h"
#include "common/aes.h"
#include "common/algorithm.h"
#include "common/xml.h"
#include "common/bzip2.h"
#include "common/misc.h"
#include "common/dir.h"
#include "common/unicode.h"
#include "common/digest.h"
#include "common/format.h"
#include "common/string.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


namespace tiary {

namespace {

// For convenience, the null terminators are also part of the salts
const char password_salt2018a[] = "tIaRy, wRiTtEn bY cHyS <aDmIn@cHyS.iNfO>";
const char password_salt2018b[] = "TIARY, WRITTEN BY CHYS <ADMIN@CHYS.INFO>";

const char new_format_signature_2009[16] = "TiaryEncrypted\0";
const char new_format_signature_2018[16] = "TiaryEncrypted2";

/**
 * Parse the time format used in the @c <time> tag
 * (%Y-%m-%d %H:%M:%S)
 * Returns 0 on error
 */
uint64_t parse_time (const char *s)
{
	ReadableDateTime t;
	// strptime is not in standard C or C++
	if (sscanf (s, "%u-%u-%u%u:%u:%u", &t.y, &t.m, &t.d, &t.H, &t.M, &t.S) != 6) {
		return 0;
	}
	return make_datetime_strict (t);
}

/**
 * Format the time as used in the @c <time> tag
 * (%Y-%m-%d %H:%M:%S)
 */
std::string format_time(const DateTime &date_time) {
	return date_time.format("%Y-%m-%d %H:%M:%S"sv);
}

bool is_trimmable_space(wchar_t c) {
	switch (c) {
	case L' ':
	case L'\t':
	case L'\v':
	case L'\r':
	case L'\n':
		return true;
	default:
		return false;
	}
}

bool is_legal_label_name(std::wstring_view name) {
	if (name.empty ()) {
		return false;
	}
	if (name.find (L',') != std::wstring_view::npos) {
		return false;
	}
	if (is_trimmable_space(name[0]) || is_trimmable_space(*name.rbegin())) {
		return false;
	}
	return true;
}

/**
 * Given an @c <entry> node, construct and return the corresponding DiaryEntry
 * If any error is encountered, returns 0
 */
DiaryEntry *analyze_entry_xml(const XMLNode *entry_node) {
	uint64_t local_time = 0;
	std::string_view title;
	std::string_view text;
	DiaryEntry::LabelList labels;

	if (entry_node->type != XMLNodeType::kTree) {
		return nullptr;
	}

	for (const XMLNode *ptr = entry_node->children; ptr; ptr = ptr->next) {
		if (ptr->type != XMLNodeType::kTree) {
			// Wild text directly within <entry> - Never allowed
			return 0;
		}
		if (ptr->name() == "time"sv) { // <time local="...." />

			if (local_time) {
				// More than one <time> tags.
				return 0;
			}

			auto it_local = ptr->properties.find("local");
			if (it_local == ptr->properties.end()) {
				return nullptr;
			}

			if (!(local_time = parse_time(it_local->second.c_str()))) {
				return 0;
			}

		} else if (ptr->name() == "label"sv) {
			auto it_name = ptr->properties.find("name");
			if (it_name == ptr->properties.end()) {
				return nullptr;
			}
			// Convert name from UTF-8 to UCS-4
			std::wstring wname = utf8_to_wstring(it_name->second);
			if (!is_legal_label_name (wname)) {
				return 0;
			}
			labels.insert (wname);

		} else if (ptr->name() == "title"sv) {

			if (title.data() != nullptr) { // More than one <title> tags
				return 0;
			}

			if (ptr->children == 0) { // Empty
				title = ""sv;
			} else if (ptr->children->type == XMLNodeType::kText) {
				if (ptr->children->next != 0) {
					return 0;
				}
				title = ptr->children->text();
			}

		} else if (ptr->name() == "text"sv) {

			if (text.data() != nullptr) { // More than one <text> tags
				return 0;
			}

			if (ptr->children == 0) { // Empty
				text = ""sv;
			} else if (ptr->children->type == XMLNodeType::kText) {
				if (ptr->children->next != 0) {
					return 0;
				}
				text = ptr->children->text();
			} else {
				return 0;
			}

		}
		else {
			// Unknown child tag within <entry>
			return 0;
		}
	}

	// Almost finished! But are we missing any required tags?
	if (local_time == 0 || title.data() == nullptr || text.data() == nullptr) {
		return 0;
	}

	// Finally successful
	return new DiaryEntry{
		DateTime(local_time),
		utf8_to_wstring(title),
		utf8_to_wstring(text),
		std::move(labels)
	};
}


/**
 * Assuming we have successfully parsed the XML, analyzes the XML and extracts useful info.
 * Applicable for both ~/.tiary and diary files
 */
bool general_analyze_xml (const XMLNode *root,
		OptionGroupBase &opts,
		DiaryEntryList *entries,
		RecentFileList *recent_files,
		bool strictest ///< Should be enabled for data file, and disabled for config files
		)
{
	if (root->type != XMLNodeType::kTree) {
		return false;
	}
	if (root->name() != "tiary"sv) { // Root node must be <tiary>
		return false;
	}
	if (entries) {
		entries->clear ();
	}
	if (recent_files) {
		recent_files->clear ();
	}
	// OK. Now loop thru its children
	for (const XMLNode *main_child = root->children; main_child; main_child = main_child->next) {
		if (main_child->type != XMLNodeType::kTree) {
			// Wild text directly within <tiary> - must be an error
			// But we choose to be as tolerant as possible in non-strict mode
			if (strictest) {
				return false;
			}
			else {
				continue;
			}
		}

		if (main_child->name() == "option"sv) { // An option
			auto it_option_name = main_child->properties.find("name");
			auto it_option_value = main_child->properties.find("value");
			if (it_option_name != main_child->properties.end() && it_option_value != main_child->properties.end()) {
				opts.set(it_option_name->second, it_option_value->second);
			} else if (strictest) {
				// <option> without "name" or "value" - Disallowed in strict mode
				return false;
			}
		} else if (entries && main_child->name() == "entry"sv) {

			DiaryEntry *entry = analyze_entry_xml (main_child);
			if (entry == 0) {
				return false;
			}
			entries->push_back (entry);
			
		} else if (recent_files && main_child->name() == "recent"sv) {
			auto it_file_name = main_child->properties.find("file");
			if (it_file_name != main_child->properties.end()) {
				recent_files->emplace_back();
				RecentFile &item = recent_files->back();
				item.filename = utf8_to_wstring(it_file_name->second);
				auto it_line_number = main_child->properties.find("line");
				if (it_line_number != main_child->properties.end()) {
					item.focus_entry = strtoul(it_line_number->second.c_str(), 0, 10);
				} else {
					// <option> without "line"
					if (strictest) {
						// Disallowed in strict mode
						return false;
					}
					else {
						// Default to 0 in non-strict mode
						item.focus_entry = 0;
					}
				}
			}
			else if (strictest) {
				// <option> without "file" - Disallowed in strict mode
				return false;
			}
		}
		else {
			// Ignored for forward compatibility - even in strict mode
		}
	}

	return true;
}


std::array<unsigned char, SHA512::DIGEST_LENGTH> format_2018_password_digest(std::string_view password) {
	SHA512 h;
	h(password_salt2018a, sizeof(password_salt2018a));
	h(password);
	h(password_salt2018b, sizeof(password_salt2018b));
	return h.result();
}

} // Anonymous namespace

LoadFileRet load_global_options (GlobalOptionGroup &options, RecentFileList &recent_files)
{
	FILE *fp = fopen (make_home_dirname (GLOBAL_OPTION_FILE).c_str(), "r");
	if (fp == 0) {
		return LOAD_FILE_NOT_FOUND;
	}
	XMLNode *root;
	{
		std::string data;
		bool ret = read_whole_file(fp, &data, 128 * 1024);
		fclose (fp);
		if (!ret) {
			return LOAD_FILE_READ_ERROR;
		}
		root = xml_parse(data);
	}
	if (root == 0) {
		return LOAD_FILE_XML;
	}
	bool ret = general_analyze_xml (root, options, 0, &recent_files, false);
	xml_free (root);
	if (!ret) {
		return LOAD_FILE_CONTENT;
	}
	return LOAD_FILE_SUCCESS;
}

LoadFileRet load_file (
		const char *filename,
		const std::function<std::string()> &enter_password,
		DiaryEntryList &entries,
		PerFileOptionGroup &options,
		std::string &password)
{
	FILE *fp = fopen (filename, "rb");
	if (fp == 0) {
		return LOAD_FILE_NOT_FOUND;
	}

	std::string everything;

	// Read everything out of file
	bool bool_ret = read_whole_file(fp, &everything);
	fclose (fp);
	if (!bool_ret) {
		return LOAD_FILE_READ_ERROR;
	}

	password.clear ();

	// Encrypted?
	if (everything.size() >= 32 && !memcmp(&everything[0], new_format_signature_2009, 16)) {
		// Obsolete encryption format (insecure, prior to 2018)
		return LOAD_FILE_OBSOLETE;
	} else if (everything.size() >= 16 + SHA512::DIGEST_LENGTH && !memcmp(&everything[0], new_format_signature_2018, 16)) {
		// Second 64 bytes: SHA(salt_2018a + password + salt_2018b)
		password = enter_password();
		if (password.empty ()) { // User cancelation
			return LOAD_FILE_PASSWORD;
		}

		if (memcmp(format_2018_password_digest(password).data(), &everything[16], SHA512::DIGEST_LENGTH) != 0) { // Password incorrect
			password.clear ();
			return LOAD_FILE_PASSWORD;
		}

		// Password correct. Decrypt now
		everything = evp_aes_decrypt({&everything[16 + SHA512::DIGEST_LENGTH], everything.size() - 16 - SHA512::DIGEST_LENGTH}, password);
		if (everything.empty()) {
			return LOAD_FILE_DECRYPTION;
		}
		// Decompress
		everything = bunzip2(everything);
	} else {
		// Not encrypted
		everything = bunzip2 (&everything[0], everything.size ());
		if (everything.empty ()) {
			return LOAD_FILE_BUNZIP2;
		}
	}

	// Parse XML
	XMLNode *root = xml_parse(everything.data(), everything.size());
	if (root == 0) {
		return LOAD_FILE_XML;
	}
	options.reset ();
	bool_ret = general_analyze_xml (root, options, &entries, 0, true);
	xml_free (root);
	if (!bool_ret) {
		return LOAD_FILE_CONTENT;
	}
	return LOAD_FILE_SUCCESS;
}


namespace {

XMLNode *make_xml_tree_from_options(const OptionGroupBase &opts, const OptionGroupBase &default_options) {
	XMLNode *root = new XMLNode(XMLNode::TreeTag(), "tiary");
	root->children = nullptr;
	XMLNode **tail = &root->children;

	for (const auto &opt_pair: opts) {
		// If the option is the same as default, do not save it

		if (default_options.get(opt_pair.first) == opt_pair.second) {
			continue;
		}

		XMLNode *newnode = new XMLNode(XMLNode::TreeTag(), "option");
		*tail = newnode;
		tail = &newnode->next;
		newnode->properties.emplace("name", opt_pair.first);
		newnode->properties.emplace("value", opt_pair.second);
	}

	return root;
}

} // anonymous namespace


bool save_global_options (const GlobalOptionGroup &options, const RecentFileList &recent_files)
{
	// First create the XML tree
	XMLNode *root = make_xml_tree_from_options (options, GlobalOptionGroup ());

	// Find the last child of root node <tiary>
	XMLNode **tail = &root->children;
	while (*tail) {
		tail = &(*tail)->next;
	}

	// Loop thru recent files
	for (const auto &rf: recent_files) {
		XMLNode *recent_node = new XMLNode(XMLNode::TreeTag(), "recent");
		*tail = recent_node;
		tail = &recent_node->next;
		recent_node->properties.emplace("file", wstring_to_utf8(rf.filename));
		recent_node->properties.emplace("line", format_dec_narrow(rf.focus_entry));
	}

	std::string xml = xml_make (root);
	xml_free (root);

	// Now write to file
	return safe_write_file(make_home_dirname(GLOBAL_OPTION_FILE).c_str(), xml);
}

bool save_file (const char *filename,
		const DiaryEntryList &entries,
		const PerFileOptionGroup &options,
		std::string_view password) {
	// First create the XML tree
	XMLNode *root = make_xml_tree_from_options(options, PerFileOptionGroup());

	// Find the last child of root node <tiary>
	XMLNode **tail = &root->children;
	while (*tail) {
		tail = &(*tail)->next;
	}

	// Loop thru entries
	for (DiaryEntry *entry: entries) {
		XMLNode *entry_node = new XMLNode(XMLNode::TreeTag(), "entry");
		*tail = entry_node;
		tail = &entry_node->next;

		// <time local="..." />
		XMLNode *time_node = new XMLNode(XMLNode::TreeTag(), "time");
		XMLNode *sub_ptr = entry_node->children = time_node;
		time_node->properties.emplace("local", format_time(entry->local_time));

		// Labels
		for (const std::wstring &label: entry->labels) {
			XMLNode *label_node = new XMLNode(XMLNode::TreeTag(), "label");
			sub_ptr = sub_ptr->next = label_node;
			// Convert from UCS-4 to UTF-8
			label_node->properties.emplace("name", wstring_to_utf8(label));
		}

		// Text
		XMLNode *title_tag_node = new XMLNode(XMLNode::TreeTag(), "title");
		sub_ptr = sub_ptr->next = title_tag_node;
		XMLNode *diary_title_node = new XMLNode(XMLNode::TextTag(), wstring_to_utf8 (entry->title));
		title_tag_node->children = diary_title_node;

		// Text
		XMLNode *text_tag_node = new XMLNode(XMLNode::TreeTag(), "text");
		sub_ptr = sub_ptr->next = text_tag_node;
		XMLNode *diary_text_node = new XMLNode(XMLNode::TextTag(), wstring_to_utf8 (entry->text));
		text_tag_node->children = diary_text_node;
	}

	// Finished building the XML tree. Now make the XML text
	// Make the data to everything that will finally be written to file
	std::string everything = bzip2(xml_make(root));
	xml_free (root);

	// Is there a password?
	if (!password.empty ()) {
		// Yes. Encrypt
		everything = evp_aes_encrypt(everything, password);

		// Encrypted file header
		char header[16 + SHA512::DIGEST_LENGTH];
		memcpy(header, new_format_signature_2018, 16);
		memcpy(header + 16, format_2018_password_digest(password).data(), SHA512::DIGEST_LENGTH);

		// Write to file
		return safe_write_file(filename, {header, sizeof(header)}, everything);
	}
	else {
		// Write to file
		return safe_write_file(filename, everything);
	}
}


} // namespace tiary
