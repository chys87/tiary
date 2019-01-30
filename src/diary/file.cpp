// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2010, 2016, 2018, 2019, chys <admin@CHYS.INFO>
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
 *    <time local="%Y-%m-%d %H:%M:%S" utc=".." />
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
 * Diary file format (encrypted file 2009):
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
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


namespace tiary {

namespace {

// For convenience, the null terminators are also part of the salts
const char password_salt1[] = "Tiary, written by chys <admin@chys.info>";
const char password_salt2[] = "tIARY, WRITTEN BY CHYS <ADMIN@CHYS.INFO>";
const char password_salt3[] = "TiArY, WrItTeN By ChYs <AdMiN@ChYs.InFo>";
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
	return wstring_to_utf8(date_time.format(L"%Y-%m-%d %H:%M:%S"sv));
}

bool is_legal_label_name (const std::wstring &name)
{
	if (name.empty ()) {
		return false;
	}
	if (name.find (L',') != std::wstring::npos) {
		return false;
	}
	wchar_t c = name[0];
	if (c==L' ' || c==L'\t' || c==L'\v' || c==L'\r' || c==L'\n') {
		return false;
	}
	c = *name.rbegin ();
	if (c==L' ' || c==L'\t' || c==L'\v' || c==L'\r' || c==L'\n') {
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
//	uint64_t utc_time = 0;
	const char *title = 0;
	const char *text = 0;
	DiaryEntry::LabelList labels;

	if (entry_node->type != XMLNodeType::kTree) {
		return nullptr;
	}

	for (const XMLNode *ptr = entry_node->children; ptr; ptr = ptr->next) {
		if (ptr->type != XMLNodeType::kTree) {
			// Wild text directly within <entry> - Never allowed
			return 0;
		}
		if (ptr->name() == "time") { // <time local="...." utc="...." />

			if (local_time/* | utc_time*/) {
				// More than one <time> tags.
				return 0;
			}

			const char *local = map_query (ptr->properties, "local");
//			const char *utc = map_query (ptr->properties, "utc");
			if (!local/* || !utc*/) {
				return 0;
			}

			if (!(local_time = parse_time (local))/* || !(utc_time = parse_time (utc))*/) {
				return 0;
			}

		} else if (ptr->name() == "label") {
			const char *name = map_query (ptr->properties, "name");
			if (!name) {
				return 0;
			}
			// Convert name from UTF-8 to UCS-4
			std::wstring wname = utf8_to_wstring (name);
			if (!is_legal_label_name (wname)) {
				return 0;
			}
			labels.insert (wname);

		} else if (ptr->name() == "title") {

			if (title != 0) { // More than one <title> tags
				return 0;
			}

			if (ptr->children == 0) { // Empty
				title = "";
			}
			else if (ptr->children->type == XMLNodeType::kText) {
				if (ptr->children->next != 0) {
					return 0;
				}
				title = ptr->children->text().c_str ();
			}

		} else if (ptr->name() == "text") {

			if (text != 0) { // More than one <text> tags
				return 0;
			}

			if (ptr->children == 0) { // Empty
				text = "";
			} else if (ptr->children->type == XMLNodeType::kText) {
				if (ptr->children->next != 0) {
					return 0;
				}
				text = ptr->children->text().c_str ();
			}
			else {
				return 0;
			}

		}
		else {
			// Unknown child tag within <entry>
			return 0;
		}
	}

	// Almost finished! But are we missing any required tags?
	if (local_time==0 || /*utc_time==0 ||*/ title==0 || text==0) {
		return 0;
	}

	// Finally successful
	DiaryEntry *entry = new DiaryEntry;
	entry->local_time = DateTime (local_time);
//	entry->utc_time = DateTime (utc_time);
	entry->title = utf8_to_wstring (title);
	entry->text = utf8_to_wstring (text);
	entry->labels = std::move (labels);
	return entry;
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
	if (root->name() != "tiary") { // Root node must be <tiary>
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

		if (main_child->name() == "option") { // An option
			if (const char *option_name = map_query (main_child->properties, "name")) {
				if (const char *option_value = map_query (main_child->properties, "value")) {
					opts.set (option_name, option_value);
				}
				else if (strictest) {
					// <option> without "name" - Disallowed in strict mode
					return false;
				}
			}
			else if (strictest) {
				// <option> without "name" - Disallowed in strict mode
				return false;
			}
		} else if (entries && main_child->name() == "entry") {

			DiaryEntry *entry = analyze_entry_xml (main_child);
			if (entry == 0) {
				return false;
			}
			entries->push_back (entry);
			
		} else if (recent_files && main_child->name() == "recent") {
			if (const char *file_name = map_query (main_child->properties, "file")) {
				recent_files->emplace_back();
				RecentFile &item = recent_files->back();
				item.filename = utf8_to_wstring (file_name);
				if (const char *line_number = map_query (main_child->properties, "line")) {
					item.focus_entry = strtoul (line_number, 0, 10);
				}
				else {
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


void decrypt_2009(void *data, size_t datalen, std::string_view pass) {
	union {
		uint64_t xor_u64[32]; // For alignment only
		uint8_t xor_byte[256];
	};
	union {
		uint64_t xor_u64_2[32]; // For alignment only
		uint8_t xor_byte2[256];
	};

	// Supress "unused" warnings
	((void) xor_u64);
	((void) xor_u64_2);

	{
		MD5 md5_tmp{pass};
		(MD5(md5_tmp))(password_salt2, sizeof password_salt2).result(xor_byte + 240);
		md5_tmp(password_salt3, sizeof password_salt3).result(xor_byte2 + 240);
	}
	for (int i=28; i>=0; i-=2) {
		MD5(xor_byte + (i + 2) * 8, (30 - i) * 8)(password_salt2, sizeof password_salt2).result(xor_byte + i * 8);
		MD5(xor_byte2 + (i + 2) * 8, (30 - i) * 8)(password_salt3, sizeof password_salt3).result(xor_byte2 + i * 8);
	}

	/*
	 * Let's leave the optimization to the compiler.
	 * A recent compiler, when optimization is fully enabled,
	 * may do some vectorization.
	 *
	 * The loops are written in a way which most current
	 * vectorization-supporting compiler recognize, as opposed
	 * to something like this (harder for optimizer):
	 *
	 * for (size_t i=0; i<datalen; ++i) {
	 *     byte[i] ^= xor_byte[i%256];
	 * }
	 */
	uint8_t *byte = reinterpret_cast<uint8_t *>(data);

	for (size_t i=0; i<datalen/256; ++i) {
		for (int j=0; j<256; ++j) {
			byte[j] ^= xor_byte[j];
		}
		byte += 256;
	}
	for (size_t i=0; i<datalen%256; ++i) {
		byte[i] ^= xor_byte[i];
	}

	byte = reinterpret_cast<uint8_t *>(data);
	for (size_t i=0; i<datalen/255; ++i) {
		for (int j=0; j<255; ++j) {
			byte[j] ^= xor_byte2[j];
		}
		byte += 255;
	}
	for (size_t i=0; i<datalen%255; ++i) {
		byte[i] ^= xor_byte2[i];
	}
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
		bool ret = read_whole_file (fp, data, 128*1024);
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
	bool bool_ret = read_whole_file (fp, everything);
	fclose (fp);
	if (!bool_ret) {
		return LOAD_FILE_READ_ERROR;
	}

	LoadFileRet success_ret = LOAD_FILE_SUCCESS;

	password.clear ();

	// Encrypted?
	if (everything.size() >= 32 && !memcmp(&everything[0], new_format_signature_2009, 16)) {
		// Second 16 bytes: MD5(password+salt1)
		password = enter_password ();
		if (password.empty ()) { // User cancelation
			return LOAD_FILE_PASSWORD;
		}
		if (memcmp(MD5(password)(password_salt1, sizeof password_salt1).result().data(),
					&everything[16], 16) != 0) { // Password incorrect
			password.clear ();
			return LOAD_FILE_PASSWORD;
		}

		// Password correct. Decrypt now
		decrypt_2009(&everything[32], everything.size() - 32, password);
		// Decompress
		everything = bunzip2 (&everything[32], everything.size () - 32);

		success_ret = LOAD_FILE_DEPRECATED;
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
	return success_ret;
}


namespace {

XMLNode *make_xml_tree_from_options(const OptionGroupBase &opts, const OptionGroupBase &default_options) {
	XMLNode node_dummy;
	XMLNode *p = &node_dummy;

	for (OptionGroupBase::const_iterator it=opts.begin (); it!=opts.end(); ++it) {
		// If the option is the same as default, do not save it

		if (default_options.get (it->first) == it->second) {
			continue;
		}

		// For readability, insert "\n\t" before each option
		p = p->next = new XMLNode(XMLNode::TextTag(), "\n\t");

		XMLNode *newnode = new XMLNode(XMLNode::TreeTag(), "option");
		p = p->next = newnode;
		newnode->properties["name"] = it->first;
		newnode->properties["value"] = it->second;
	}

	// Finally, insert an "\n" before closing tag </tiary>
	p = p->next = new XMLNode(XMLNode::TextTag(), "\n");

	// Create the <tiary> node last
	XMLNode *root = new XMLNode(XMLNode::TreeTag(), "tiary");
	root->children = node_dummy.next;
	return root;
}

} // anonymous namespace


bool save_global_options (const GlobalOptionGroup &options, const RecentFileList &recent_files)
{
	// First create the XML tree
	XMLNode *root = make_xml_tree_from_options (options, GlobalOptionGroup ());
	assert(root->type == XMLNodeType::kTree);

	// Find the last child of root node <tiary>
	XMLNode *ptr = root->children;
	// ptr is never null. Let's loop until we find the last entry
	while (ptr->next) {
		ptr = ptr->next;
	}

	// Loop thru recent files
	for (RecentFileList::const_iterator it = recent_files.begin (); it != recent_files.end (); ++it) {
		// Insert a text node "\n\t" before each <recent>
		// At the first loop, ptr should point to a text node "\n"
		// Change that!
		if (ptr->type == XMLNodeType::kText) {
			ptr->text() = "\n\t";
		} else {
			ptr = ptr->next = new XMLNode(XMLNode::TextTag(), "\n\t");
		}

		XMLNode *recent_node = new XMLNode(XMLNode::TreeTag(), "recent");
		ptr = ptr->next = recent_node;
		recent_node->properties["file"] = wstring_to_utf8 (it->filename);
		recent_node->properties["line"] = format_dec_narrow (it->focus_entry);
	}
	// Insert a node "\n" at the end
	if (ptr->type != XMLNodeType::kText) {
		ptr = ptr->next = new XMLNode(XMLNode::TextTag(), "\n");
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
	XMLNode *ptr = root->children;
	// ptr is never null. Let's loop until we find the last entry
	while (ptr->next) {
		ptr = ptr->next;
	}

	// Loop thru entries
	for (DiaryEntryList::const_iterator it = entries.begin (); it != entries.end (); ++it) {
		// Insert a text node "\n\t" before each <entry>
		// At the first loop, ptr should point to a text node "\n"
		// Change that!
		if (ptr->type == XMLNodeType::kText) {
			ptr->text() = "\n\t";
		} else {
			ptr = ptr->next = new XMLNode(XMLNode::TextTag(), "\n\t");
		}

		DiaryEntry *entry = *it; // Never null
		XMLNode *entry_node = new XMLNode(XMLNode::TreeTag(), "entry");
		ptr = ptr->next = entry_node;

		XMLNode *sub_ptr; // Point to <entry>'s children

		// <time local="..." utc="..." />
		sub_ptr = entry_node->children = new XMLNode(XMLNode::TextTag(), "\n\t\t");
		XMLNode *time_node = new XMLNode(XMLNode::TreeTag(), "time");
		sub_ptr = sub_ptr->next = time_node;
		time_node->properties["local"] = format_time (entry->local_time);
//		time_node->properties["utc"] = format_time (entry->utc_time);

		// Labels
		for (DiaryEntry::LabelList::const_iterator it = entry->labels.begin (); it != entry->labels.end (); ++it) {
			sub_ptr = sub_ptr->next = new XMLNode(XMLNode::TextTag(), "\n\t\t");
			XMLNode *label_node = new XMLNode(XMLNode::TreeTag(), "label");
			sub_ptr = sub_ptr->next = label_node;
			// Convert from UCS-4 to UTF-8
			label_node->properties["name"] = wstring_to_utf8 (*it);
		}

		// Text
		sub_ptr = sub_ptr->next = new XMLNode(XMLNode::TextTag(), "\n\t\t");
		XMLNode *title_tag_node = new XMLNode(XMLNode::TreeTag(), "title");
		sub_ptr = sub_ptr->next = title_tag_node;
		XMLNode *diary_title_node = new XMLNode(XMLNode::TextTag(), wstring_to_utf8 (entry->title));
		title_tag_node->children = diary_title_node;

		// Text
		sub_ptr = sub_ptr->next = new XMLNode(XMLNode::TextTag(), "\n\t\t");
		XMLNode *text_tag_node = new XMLNode(XMLNode::TreeTag(), "text");
		sub_ptr = sub_ptr->next = text_tag_node;
		XMLNode *diary_text_node = new XMLNode(XMLNode::TextTag(), wstring_to_utf8 (entry->text));
		text_tag_node->children = diary_text_node;

		// Insert a text node "\n\t" before closing tag </entry>
		sub_ptr = sub_ptr->next = new XMLNode(XMLNode::TextTag(), "\n\t");
	}
	// Insert a text node "\n" before closing tag </tiary>
	ptr = ptr->next = new XMLNode(XMLNode::TextTag(), "\n");



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
