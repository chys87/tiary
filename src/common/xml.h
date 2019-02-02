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


#ifndef TIARY_COMMON_XML_H
#define TIARY_COMMON_XML_H

#include "common/containers.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <string>
#include <string_view>

/**
 * @file	common/xml.h
 * @author	chys <admin@chys.info>
 * @brief	Parses and creates XML
 *
 * Remember everything is in UTF-8
 */

namespace tiary {

enum struct XMLNodeType : uint8_t {
	kTree,
	kText,
	kDummy,
};

struct XMLNode
{
	struct TreeTag {};
	struct TextTag {};

	XMLNodeType type = XMLNodeType::kDummy;
	XMLNode *children = nullptr;
	XMLNode *next = nullptr; // Pointer to right sibling
	std::string name_or_text; ///< tag name (kTree) or content (kText)
	StringOrderedMap properties;

	const std::string &name() const { return name_or_text; }
	const std::string &text() const { return name_or_text; }
	std::string &text() { return name_or_text; }

	XMLNode() = default;
	XMLNode(TreeTag, const char *tag_name) : type(XMLNodeType::kTree), name_or_text(tag_name, strlen(tag_name)) {}
	XMLNode(TreeTag, std::string_view tag_name) : type(XMLNodeType::kTree), name_or_text(tag_name) {}
	XMLNode(TreeTag, std::string &&tag_name) : type(XMLNodeType::kTree), name_or_text(std::move(tag_name)) {}
	XMLNode(TextTag, const char *text) : type(XMLNodeType::kText), name_or_text(text, strlen(text)) {}
	XMLNode(TextTag, std::string_view text) : type(XMLNodeType::kText), name_or_text(text) {}
	XMLNode(TextTag, std::string &&text) : type(XMLNodeType::kText), name_or_text(std::move(text)) {}

	XMLNode(const XMLNode &) = delete;
	XMLNode &operator = (const XMLNode &) = delete;
};

// Parses an XML string and returns the corresponding tree
// Returns NULL on error
XMLNode *xml_parse (const char *, size_t);

inline XMLNode *xml_parse(std::string_view s) {
	return xml_parse (s.data (), s.length ());
}

// Recursively deletes an XML tree
void xml_free (XMLNode *);

std::string xml_make (const XMLNode *);

} // namespace tiary


#endif // Include guard
