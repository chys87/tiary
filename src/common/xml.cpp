// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the so-called 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#include "common/xml.h"
#include <stack>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <stdlib.h>
#include <string.h>

void tiary::xml_free (XMLNode *root)
{
	if (root == 0)
		return;

	std::stack<XMLNode *> stk;

	XMLNode *p = root; // Current node

	for (;;) {
		// Push right sibling into stack
		if (p->next)
			stk.push (p->next);
		XMLNode *first_child = 0;
		if (XMLNodeTree *q = dynamic_cast<XMLNodeTree *>(p))
			first_child = q->children;
		delete p;
		if (first_child)
			p = first_child;
		else if (!stk.empty ()) {
			p = stk.top ();
			stk.pop ();
		} else
			break;
	}
}

tiary::XMLNode *tiary::xml_parse (const std::string &s)
{
	return xml_parse (s.data (), s.length ());
}

namespace {

using namespace tiary;

// Copy the attribute name and attributes, without children
XMLNode *shallow_copy (xmlNodePtr iptr)
{
	if (xmlNodeIsText (iptr)) { // Text node
		const char *text = (const char *)xmlNodeGetContent (iptr);
		// If a text node is empty or completely consists of space (tab, newline) etc.
		// Eliminate it!
		if (strspn (text, " \t\r\n\v")[text] == '\0')
			return 0;
		return new XMLNodeText (text);
	} else { // Normal node
		XMLNodeTree *optr = new XMLNodeTree ((const char *)iptr->name);
		// Attributes. Not ordered.
		for (xmlAttrPtr aptr=iptr->properties; aptr; aptr=aptr->next)
			optr->properties[(const char *)aptr->name] = (const char *)xmlNodeGetContent (aptr->children);
		return optr;
	}
}

// The other way
xmlNodePtr shallow_copy (const XMLNode *iptr)
{
	if (const XMLNodeText *p = dynamic_cast<const XMLNodeText *>(iptr))
		return xmlNewText (BAD_CAST (p->text.c_str ()));
	else if (const XMLNodeTree *p = dynamic_cast<const XMLNodeTree *>(iptr)) {
		xmlNodePtr optr = xmlNewNode (0, BAD_CAST (p->name.c_str()));
		// Copy attributes. Not ordered.
		for (XMLNodeTree::PropertyList::const_iterator it=p->properties.begin(); it!=p->properties.end(); ++it)
			xmlNewProp (optr, BAD_CAST(it->first.c_str()), BAD_CAST(it->second.c_str()));
		return optr;
	} else
		return 0;
}

void generic_error_silent (void *, const char *, ...) {}
void structured_error_silent (void *, xmlErrorPtr) {}

void libxml2_init ()
{
	static bool called = false;
	if (!called) {
		xmlSetGenericErrorFunc (0, generic_error_silent);
		xmlSetStructuredErrorFunc (0, structured_error_silent);
		called = true;
	}
}

} // anonymous namespace

tiary::XMLNode *tiary::xml_parse (const char *str, size_t len)
{
	xmlDocPtr doc;
	xmlNodePtr iptr;

	libxml2_init ();

	if (!(doc = xmlReadMemory (str, len, 0, 0, 0)))
		return 0;
	if (!(iptr = xmlDocGetRootElement (doc))) {
		xmlFreeDoc (doc);
		return 0;
	}

	// Successfully parsed. Now we need to construct our own XML tree

	// "(a,b) in stk" means "b's children should be copied as a's children"
	std::stack<std::pair<XMLNodeTree *, xmlNodePtr> > stk;

	XMLNode *root = shallow_copy (iptr);
	if (!dynamic_cast<XMLNodeTree *>(root)) {
		// Root is a text node. Error.
		delete root;
		return 0;
	}
	XMLNodeTree *optr = static_cast<XMLNodeTree *>(root);
				// Current working output node
				// Current working input node is iptr
	XMLNode virtual_node;
	for (;;) {
		// Shallow copy all children of current node
		virtual_node.next = 0;
		XMLNode *last = &virtual_node;
		for (xmlNodePtr child_ptr = iptr->xmlChildrenNode; child_ptr; child_ptr = child_ptr->next)
			if (XMLNode *newnode = shallow_copy (child_ptr)) {
				last = last->next = newnode;
				if (child_ptr->xmlChildrenNode)
					stk.push (std::make_pair (static_cast<XMLNodeTree *>(newnode), child_ptr));
			}
		optr->children = virtual_node.next;
		if (stk.empty ())
			break;
		optr = stk.top().first;
		iptr = stk.top().second;
		stk.pop ();
	}
	xmlFreeDoc (doc);
	return root;
}

std::string tiary::xml_make (const XMLNode *root)
{
	/**
	 * I have considered directly generating the XML text, which should
	 * not be difficult to implement and obviously more efficient than
	 * using libxml2.
	 *
	 * However, considering there are many details in XML that I may
	 * easily forget, (say, always remember to replace special characters
	 * with escape sequences starting with "&")
	 * I decide to stick to libxml2.
	 */
	std::string ret;

	if (const XMLNodeTree *iroot = dynamic_cast <const XMLNodeTree *> (root)) {

		libxml2_init ();

		xmlDocPtr doc = xmlNewDoc (BAD_CAST "1.0"); // "1.0" - XML version
		xmlNodePtr oroot = xmlNewNode(0, BAD_CAST (iroot->name.c_str()));
		xmlDocSetRootElement(doc, oroot);

		// "(a,b) in stk" means "a's children should be copied as b's children"
		std::stack<std::pair<const XMLNodeTree *, xmlNodePtr> > stk;
		xmlNodePtr optr = oroot;         // Current working output node
		const XMLNodeTree *iptr = iroot; // Current working input node

		for (;;) {
			// Shallow copy all children of current node
			for (XMLNode *child_ptr = iptr->children; child_ptr; child_ptr = child_ptr->next)
				if (xmlNodePtr nptr = shallow_copy (child_ptr)) {
					xmlAddChild (optr, nptr);
					if (const XMLNodeTree *ip = dynamic_cast <const XMLNodeTree *> (child_ptr))
						stk.push (std::make_pair (ip, nptr));
				}
			if (stk.empty ())
				break;
			iptr = stk.top().first;
			optr = stk.top().second;
			stk.pop ();
		}

		xmlChar *str;
		int len;
		xmlDocDumpMemoryEnc (doc, &str, &len, "UTF-8");
		xmlFreeDoc (doc);

		ret.assign ((const char*)str, len);
		xmlFree (str);
	}
	return ret;
}
