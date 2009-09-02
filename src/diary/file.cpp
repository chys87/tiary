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
 * Diary file format:
 *
 * The file is compressed with bzip2.
 * When decompressed, the format is as follows:
 *
 * (1) If there's no password, it's simply an XML;
 * (2) If there is a password, the first few bytes are:
 *     0000 ~ 000F  All zerores. (Reserved, and in order to differ from "<?xml ...")
 *     0010 ~ 001F  MD5(password + salt1)
 *     0020 ~ ....  encrypt (XML, password)
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
 */

#include "diary/file.h"
#include "diary/config.h"
#include "diary/diary.h"
#include "common/xml.h"
#include "common/bzip2.h"
#include "common/misc.h"
#include "common/dir.h"
#include "common/unicode.h"
#include "common/md5.h"
#include "common/format.h"
#include <stdio.h>
#include <string.h>


namespace tiary {

namespace {

// For convenience, the null terminators are also part of the salts
const char password_salt1[] = "Tiary, written by chys <admin@chys.info>";
const char password_salt2[] = "tIARY, WRITTEN BY CHYS <ADMIN@CHYS.INFO>";
const char password_salt3[] = "TiArY, WrItTeN By ChYs <AdMiN@ChYs.InFo>";

/**
 * Parse the time format used in the <time> tag
 * (%Y-%m-%d %H:%M:%S)
 * Returns 0 on error
 */
uint64_t parse_time (const char *s)
{
	ReadableDateTime t;
	// strptime is not in standard C or C++
	if (sscanf (s, "%u-%u-%u%u:%u:%u", &t.y, &t.m, &t.d, &t.H, &t.M, &t.S) != 6)
		return 0;
	return make_datetime_strict (t);
}

/**
 * Format the time as used in the <time> tag
 * (%Y-%m-%d %H:%M:%S)
 */
std::string format_time (const DateTime &date_time)
{
	return wstring_to_utf8 (date_time.format (L"%Y-%m-%d %H:%M:%S"));
}

/**
 * Given an <entry> node, construct and return the corresponding DiaryEntry
 * If any error is encountered, returns 0
 */
DiaryEntry *analyze_entry_xml (const XMLNodeTree *entry_node)
{
	uint64_t local_time = 0;
//	uint64_t utc_time = 0;
	const char *title = 0;
	const char *text = 0;
	DiaryEntry::LabelList labels;

	for (const XMLNode *xptr = entry_node->children; xptr; xptr = xptr->next)
		if (const XMLNodeTree *ptr = dynamic_cast <const XMLNodeTree *>(xptr)) {
			if (ptr->name == "time") { // <time local="...." utc="...." />

				if (local_time/* | utc_time*/) // More than one <time> tags.
					return 0;

				const char *local = map_query (ptr->properties, "local");
//				const char *utc = map_query (ptr->properties, "utc");
				if (!local/* || !utc*/)
					return 0;

				if (!(local_time = parse_time (local))/* || !(utc_time = parse_time (utc))*/)
					return 0;

			} else if (ptr->name == "tag" || ptr->name == "label") {
				// A "label" used to be called a "tag".
				// Older formats use "tag" instead of "label"

				const char *name = map_query (ptr->properties, "name");
				if (!name)
					return 0;
				// Convert name from UTF-8 to UCS-4
				std::wstring wname = utf8_to_wstring (name);
				if (wname.empty ()) // Labels cannot be empty
					return 0;
				labels.insert (wname);

			} else if (ptr->name == "title") {

				if (title != 0) // More than one <title> tags
					return 0;

				if (ptr->children == 0) // Empty
					title = "";
				else if (const XMLNodeText *title_node = dynamic_cast<const XMLNodeText *>(ptr->children)) {
					if (title_node->next != 0)
						return 0;
					title = title_node->text.c_str ();
				}

			} else if (ptr->name == "text") {

				if (text != 0) // More than one <text> tags
					return 0;

				if (ptr->children == 0) // Empty
					text = "";
				else if (const XMLNodeText *text_node = dynamic_cast<const XMLNodeText *>(ptr->children)) {
					if (text_node->next != 0)
						return 0;
					text = text_node->text.c_str ();
				} else
					return 0;

			} else {
				// Unknown child tag within <entry>
				return 0;
			}

		} else {
			// Error - wild text directly within <entry>
			return 0;
		}

	// Almost finished! But are we missing any required tags?
	if (local_time==0 || /*utc_time==0 ||*/ title==0 || text==0)
		return 0;

	// Finally successful
	DiaryEntry *entry = new DiaryEntry;
	entry->local_time = DateTime (local_time);
//	entry->utc_time.assign (utc_time);
	utf8_to_wstring (title).swap (entry->title);
	utf8_to_wstring (text).swap (entry->text);
	entry->labels.swap (labels);
	return entry;
}


/**
 * Assuming we have successfully parsed the XML, analyzes the XML and extracts useful info.
 * Applicable for both ~/.tiary and diary files
 */
bool general_analyze_xml (const XMLNode *root, OptionGroupBase &opts, DiaryEntryList *entries = 0)
{
	const XMLNodeTree *root_diary = dynamic_cast<const XMLNodeTree *>(root);
	if (root_diary == 0)
		return false;
	if (root_diary->name != "tiary") // Root node must be <tiary>
		return false;
	if (entries)
		entries->clear ();
	// OK. Now loop thru its children
	for (const XMLNode *main_childx = root_diary->children; main_childx; main_childx = main_childx->next) {
		if (const XMLNodeTree *main_child = dynamic_cast <const XMLNodeTree *>(main_childx)) {

			if (main_child->name == "option") { // An option
				if (const char *option_name = map_query (main_child->properties, "name"))
					if (const char *option_value = map_query (main_child->properties, "value"))
						opts.set (option_name, option_value);
			} else if (entries && main_child->name == "entry") {

				DiaryEntry *entry = analyze_entry_xml (main_child);
				if (entry == 0)
					return false;
				entries->push_back (entry);
				
			} // else: Ignored for forward compatibility
		} else {
			// It must be an error - wild text directly within <tiary>
			return false;
		}
	}

	return true;
}


// (dst == src) is allowed, but should not be overlapped in any other way
void encrypt (void *dst, const char *src, size_t datalen, const void *pass, size_t passlen)
{
	/*
	 * The data always begins with "<?xml verison="1.0" encoding="UTF-8"?>\n<tiary>\n\t"
	 * We must make sure one cannot crack our file using this.
	 * (At least, cannot easily crack the whole file)
	 */
	union {
		uint64_t xor_data[32];
		uint8_t xor_byte[256];
	};
	union {
		uint64_t xor_data2[32];
		uint8_t xor_byte2[255]; // 255: Not a mistake
	};
	MD5 (pass, passlen).append (password_salt2, sizeof password_salt2).result (xor_data+30);
	for (int i=28; i>=0; i-=2)
		MD5 (xor_data+i+2, (30-i)*8).append (password_salt2, sizeof password_salt2).result (xor_data+i);
	MD5 (pass, passlen).append (password_salt3, sizeof password_salt3).result (xor_data2+30);
	for (int i=28; i>=0; i-=2)
		MD5 (xor_data2+i+2, (30-i)*8).append (password_salt3, sizeof password_salt3).result (xor_data2+i);

	// In our program, data should be well-aligned, but it seems there's no guarantee.
	// Hopefully there will be better solutions.
	// (It absolutely is not a good idea to do the XOR byte by byte.)
	uint8_t *dstbyte = reinterpret_cast<uint8_t *>(dst);
	const uint8_t *srcbyte = reinterpret_cast<const uint8_t *>(src);

	for (size_t i=0; i<datalen/256; ++i) {
		for (int j=0; j<256; ++j)
			dstbyte[j] = srcbyte[j] ^ xor_byte[j];
		dstbyte += 256;
		srcbyte += 256;
	}
	for (size_t i=0; i<datalen%256; ++i)
		dstbyte[i] = srcbyte[i] ^ xor_byte[i];

	dstbyte = reinterpret_cast<uint8_t *>(dst);
	for (size_t i=0; i<datalen/255; ++i) {
		for (int j=0; j<255; ++j)
			dstbyte[j] ^= xor_byte2[j];
		dstbyte += 255;
	}
	for (size_t i=0; i<datalen%255; ++i)
		dstbyte[i] ^= xor_byte2[i];
}

inline void decrypt (void *dst, const char *src, size_t datalen, const void *pass, size_t passlen)
{
	// The same
	encrypt (dst, src, datalen, pass, passlen);
}


} // Anonymous namespace

LoadFileRet load_global_options (GlobalOptionGroup &options)
{
	FILE *fp = fopen (make_home_dirname (GLOBAL_OPTION_FILE).c_str(), "r");
	if (fp == NULL)
		return LOAD_FILE_NOT_FOUND;
	std::vector<char> data;
	bool ret = read_whole_file (fp, data);
	fclose (fp);
	if (!ret)
		return LOAD_FILE_READ_ERROR;
	XMLNode *root = xml_parse (&data[0], data.size ());
	std::vector<char>().swap(data);
	if (root == 0)
		return LOAD_FILE_XML;
	ret = general_analyze_xml (root, options);
	xml_free (root);
	if (!ret)
		return LOAD_FILE_CONTENT;
	return LOAD_FILE_SUCCESS;
}

LoadFileRet load_file (
		const char *filename,
		const NoArgCallback<std::wstring> &enter_password,
		DiaryEntryList &entries,
		PerFileOptionGroup &options,
		std::wstring &password)
{
	FILE *fp = fopen (filename, "rb");
	if (fp == 0)
		return LOAD_FILE_NOT_FOUND;

	std::vector<char> everything;

	// Read everything out of file
	bool bool_ret = read_whole_file (fp, everything);
	fclose (fp);
	if (!bool_ret)
		return LOAD_FILE_READ_ERROR;

	// Decompress: everything = bunzip2 (everything)
	bunzip2 (&everything[0], everything.size ()).swap (everything);
	if (everything.empty ())
		return LOAD_FILE_BUNZIP2;


	size_t offset = 0; // For efficiency

	password.clear ();

	// Is there a password?
	if (everything[0] != '<') {
		// We have a password.
		if (everything.size () < 32)
			return LOAD_FILE_CONTENT;

		uint64_t zerores[] = { 0, 0 };

		// First 16 bytes must be zeroes
		if (memcmp (&everything[0], zerores, 16) != 0)
			return LOAD_FILE_CONTENT;

		// Second 16 bytes: MD5(filename + salt1)
		password = enter_password ();
		if (password.empty ()) { // User cancelation
			return LOAD_FILE_PASSWORD;
		}
		if (memcmp (MD5 (wstring_to_mbs (password)).append (password_salt1, sizeof password_salt1).result (),
					&everything[16], 16) != 0) { // Password incorrect
			password.clear ();
			return LOAD_FILE_PASSWORD;
		}

		// Password correct. Decrypt now
		decrypt (&everything[32], &everything[32], everything.size()-32, password.data(), password.length());
		offset = 32;
	}

	// Parse XML
	XMLNode *root = xml_parse (&everything[offset], everything.size() - offset);
	if (root == 0)
		return LOAD_FILE_XML;
	options.reset ();
	bool_ret = general_analyze_xml (root, options, &entries);
	xml_free (root);
	if (!bool_ret)
		return LOAD_FILE_CONTENT;
	return LOAD_FILE_SUCCESS;
}


namespace {

XMLNodeTree *make_xml_tree_from_options (const OptionGroupBase &opts, const OptionGroupBase &default_options)
{
	XMLNode node_dummy;
	XMLNode *p = &node_dummy;

	for (OptionGroupBase::const_iterator it=opts.begin (); it!=opts.end(); ++it) {
		// If the option is the same as default, do not save it

		if (default_options.get (it->first) == it->second)
			continue;

		// For readability, insert "\n\t" before each option
		p = p->next = new XMLNodeText ("\n\t");

		XMLNodeTree *newnode = new XMLNodeTree ("option");
		p = p->next = newnode;
		newnode->properties["name"] = it->first;
		newnode->properties["value"] = it->second;
	}

	// Finally, insert an "\n" before closing tag </tiary>
	p = p->next = new XMLNodeText ("\n");

	// Create the <tiary> node last
	XMLNodeTree *root = new XMLNodeTree ("tiary");
	root->children = node_dummy.next;
	return root;
}

} // anonymous namespace


bool save_global_options (const GlobalOptionGroup &options)
{
	// First create the XML tree
	XMLNodeTree *root = make_xml_tree_from_options (options, GlobalOptionGroup ());
	std::string xml = xml_make (root);
	xml_free (root);

	// Now write to file
	return safe_write_file (make_home_dirname (GLOBAL_OPTION_FILE).c_str(), xml.data(), xml.length());
}

bool save_file (const char *filename,
		const DiaryEntryList &entries,
		const PerFileOptionGroup &options,
		const std::wstring &password)
{
	// First create the XML tree
	XMLNodeTree *root = make_xml_tree_from_options (options, PerFileOptionGroup ());

	// Find the last child of root node <tiary>
	XMLNode *ptr = root->children;
	// ptr is never null. Let's loop until we find the last entry
	while (ptr->next)
		ptr = ptr->next;

	// Loop thru entries
	for (DiaryEntryList::const_iterator it = entries.begin (); it != entries.end (); ++it) {
		// Insert a text node "\n\t" before each <entry>
		// At the first loop, ptr should point to a text node "\n"
		// Change that!
		if (XMLNodeText *text_node = dynamic_cast <XMLNodeText *>(ptr))
			text_node->text = "\n\t";
		else
			ptr = ptr->next = new XMLNodeText ("\n\t");

		DiaryEntry *entry = *it; // Never null
		XMLNodeTree *entry_node = new XMLNodeTree ("entry");
		ptr = ptr->next = entry_node;

		XMLNode *sub_ptr; // Point to <entry>'s children

		// <time local="..." utc="..." />
		sub_ptr = entry_node->children = new XMLNodeText ("\n\t\t");
		XMLNodeTree *time_node = new XMLNodeTree ("time");
		sub_ptr = sub_ptr->next = time_node;
		time_node->properties["local"] = format_time (entry->local_time);
//		time_node->properties["utc"] = format_time (entry->utc_time);

		// Labels
		for (DiaryEntry::LabelList::const_iterator it = entry->labels.begin (); it != entry->labels.end (); ++it) {
			sub_ptr = sub_ptr->next = new XMLNodeText ("\n\t\t");
			XMLNodeTree *label_node = new XMLNodeTree ("label");
			sub_ptr = sub_ptr->next = label_node;
			// Convert from UCS-4 to UTF-8
			wstring_to_utf8 (*it).swap (label_node->properties["name"]);
		}

		// Text
		sub_ptr = sub_ptr->next = new XMLNodeText ("\n\t\t");
		XMLNodeTree *title_tag_node = new XMLNodeTree ("title");
		sub_ptr = sub_ptr->next = title_tag_node;
		XMLNodeText *diary_title_node = new XMLNodeText (entry->title);
		title_tag_node->children = diary_title_node;

		// Text
		sub_ptr = sub_ptr->next = new XMLNodeText ("\n\t\t");
		XMLNodeTree *text_tag_node = new XMLNodeTree ("text");
		sub_ptr = sub_ptr->next = text_tag_node;
		XMLNodeText *diary_text_node = new XMLNodeText (entry->text);
		text_tag_node->children = diary_text_node;

		// Insert a text node "\n\t" before closing tag </entry>
		sub_ptr = sub_ptr->next = new XMLNodeText ("\n\t");
	}
	// Insert a text node "\n" before closing tag </tiary>
	ptr = ptr->next = new XMLNodeText ("\n");



	// Finished building the XML tree. Now make the XML text
	std::string xml_text = xml_make (root);
	xml_free (root);

	// The next task is to make the data to everything
	// that will finally be written to file
	std::vector<char> everything;

	// Is there a password?
	if (password.empty ()) { // No password.

		bzip2 (xml_text.data (), xml_text.length ()).swap (everything);

	} else { // Password. Encrypt first

		everything.resize (32 + xml_text.length());
		MD5 (wstring_to_mbs (password)).append (password_salt1, sizeof password_salt1).result (&everything[16]);
		encrypt (&everything[32], xml_text.data(), xml_text.length(), password.data(), password.length());
		bzip2 (&everything[0], everything.size ()).swap (everything); // everything = bzip2 (everything)
	}

	// Write to file
	return safe_write_file (filename, &everything[0], everything.size());
}


} // namespace tiary
