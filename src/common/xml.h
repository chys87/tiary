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


#ifndef TIARY_COMMON_XML_H
#define TIARY_COMMON_XML_H

#include "common/containers.h"
#include <stddef.h>
#include <string>

/**
 * @file	common/xml.h
 * @author	chys <admin@chys.info>
 * @brief	Parses and creates XML
 *
 * Remember everything is in UTF-8
 */

namespace tiary {

struct XMLNode
{
	virtual ~XMLNode () {}
	XMLNode () : next(0) {}

	XMLNode *next;     // Pointer to right sibling

private:
	XMLNode (const XMLNode &);
	void operator = (const XMLNode &);
};

struct XMLNodeTree : XMLNode
{
	explicit XMLNodeTree (const char *name_);
	~XMLNodeTree ();

	XMLNode *children; // Pointer to first child
	const std::string name;

	typedef StringOrderedMap PropertyList;

	PropertyList properties;
};

struct XMLNodeText : XMLNode
{
	explicit XMLNodeText (const char *text_);
	explicit XMLNodeText (const std::string &text_);
	XMLNodeText () {}
	~XMLNodeText ();

	std::string text;
};

// Parses an XML string and returns the corresponding tree
// Returns NULL on error
XMLNode *xml_parse (const char *, size_t);

inline XMLNode *xml_parse (const std::string &s)
{
	return xml_parse (s.data (), s.length ());
}

// Recursively deletes an XML tree
void xml_free (XMLNode *);

std::string xml_make (const XMLNode *);

} // namespace tiary


#endif // Include guard
