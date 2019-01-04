// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2018, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#include "common/xml.h"
#include <stack>
#include <vector>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <stdlib.h>
#include <string.h>

namespace tiary {

void xml_free(XMLNode *root) {
	if (root == 0) {
		return;
	}

	// deque is too complicated; vector is fine
	std::stack<XMLNode *, std::vector <XMLNode *> > stk;

	XMLNode *p = root; // Current node

	for (;;) {
		XMLNode *right_sibling = p->next;
		XMLNode *first_child = 0;
		if (p->type == XMLNodeType::kTree) {
			first_child = p->children;
		}
		delete p;
		if (first_child) {
			// Proceed to children
			if (right_sibling) {
				stk.push (right_sibling);
			}
			p = first_child;
		}
		else if (right_sibling) {
			// Only right sibling
			p = right_sibling;
		}
		else if (!stk.empty ()) {
			// Neither right sibling nor children
			// Pop one from stack
			p = stk.top ();
			stk.pop ();
		}
		else {
			// Neither right sibling nor children
			// And stack is empty.
			// Everything is done.
			break;
		}
	}
}

namespace {

// Copy the attribute name and attributes, without children
XMLNode *shallow_copy (xmlNodePtr iptr)
{
	if (iptr->type == XML_TEXT_NODE || iptr->type == XML_CDATA_SECTION_NODE) { // Text node
		XMLNode *optr = nullptr;
		if (const char *text = (const char *)iptr->content) {
			// If a text node is empty or completely consists of space (tab, newline) etc.
			// Eliminate it!
			if (strspn (text, " \t\r\n\v")[text] != '\0') {
				optr = new XMLNode(XMLNode::TextTag(), text);
			}
		}
		return optr;
	}
	else if (iptr->type == XML_ELEMENT_NODE) { // Element node
		XMLNode *optr = new XMLNode(XMLNode::TreeTag(), (const char *)iptr->name);
		// Attributes. Not ordered.
		for (xmlAttrPtr aptr=iptr->properties; aptr; aptr=aptr->next) {
			if (aptr->name && aptr->children) {
				optr->properties[(const char *)aptr->name] = (const char *)aptr->children->content;
			}
		}
		return optr;
	}
	else { // Other kinds of nodes. Not supported.
		return 0;
	}
}

// The other way
xmlNodePtr shallow_copy (const XMLNode *iptr)
{
	if (iptr->type == XMLNodeType::kText) {
		return xmlNewText(BAD_CAST(iptr->text().c_str()));
	} else if (iptr->type == XMLNodeType::kTree) {
		xmlNodePtr optr = xmlNewNode(0, BAD_CAST(iptr->name().c_str()));
		// Copy attributes. Not ordered.
		for (auto it = iptr->properties.begin(); it != iptr->properties.end(); ++it) {
			xmlNewProp (optr, BAD_CAST(it->first.c_str()), BAD_CAST(it->second.c_str()));
		}
		return optr;
	}
	else {
		return 0;
	}
}

extern "C" void generic_error_silent (void *, const char *, ...) {}
extern "C" void structured_error_silent (void *, xmlErrorPtr) {}

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

XMLNode *xml_parse(const char *str, size_t len) {
	xmlDocPtr doc;
	xmlNodePtr iptr;

	libxml2_init ();

	if (!(doc = xmlReadMemory (str, len, 0, 0, 0))) {
		return 0;
	}
	if (!(iptr = xmlDocGetRootElement (doc))) {
		xmlFreeDoc (doc);
		return 0;
	}

	// Successfully parsed. Now we need to construct our own XML tree

	// "(a,b) in stk" means "b's children should be copied as a's children"
	std::stack<std::pair<XMLNode *, xmlNodePtr>, std::vector<std::pair<XMLNode *, xmlNodePtr> > > stk;

	XMLNode *root = shallow_copy (iptr);
				// Current working output node
				// Current working input node is iptr
	if (root->type != XMLNodeType::kTree) {
		// Root is a text node. Error.
		delete root;
		return 0;
	}
	XMLNode *optr = root;
	XMLNode virtual_node;
	for (;;) {
		// Shallow copy all children of current node
		virtual_node.next = 0;
		XMLNode *last = &virtual_node;
		for (xmlNodePtr child_ptr = iptr->xmlChildrenNode; child_ptr; child_ptr = child_ptr->next) {
			if (XMLNode *newnode = shallow_copy (child_ptr)) {
				last = last->next = newnode;
				if (child_ptr->xmlChildrenNode) {
					stk.push(std::make_pair(newnode, child_ptr));
				}
			}
		}
		optr->children = virtual_node.next;
		if (stk.empty ()) {
			break;
		}
		optr = stk.top().first;
		iptr = stk.top().second;
		stk.pop ();
	}
	xmlFreeDoc (doc);
	return root;
}

std::string xml_make(const XMLNode *root) {
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

	if (root->type == XMLNodeType::kTree) {

		libxml2_init ();

		xmlDocPtr doc = xmlNewDoc (BAD_CAST "1.0"); // "1.0" - XML version
		xmlNodePtr oroot = xmlNewNode(0, BAD_CAST(root->name().c_str()));
		xmlDocSetRootElement(doc, oroot);

		// "(a,b) in stk" means "a's children should be copied as b's children"
		std::stack<std::pair<const XMLNode *, xmlNodePtr>, std::vector<std::pair<const XMLNode *, xmlNodePtr>>> stk;
		xmlNodePtr optr = oroot;         // Current working output node
		const XMLNode *iptr = root; // Current working input node

		for (;;) {
			// Shallow copy all children of current node
			for (XMLNode *child_ptr = iptr->children; child_ptr; child_ptr = child_ptr->next) {
				if (xmlNodePtr nptr = shallow_copy (child_ptr)) {
					xmlAddChild (optr, nptr);
					if (child_ptr->type == XMLNodeType::kTree && child_ptr->children) {
						stk.push(std::make_pair(child_ptr, nptr));
					}
				}
			}
			if (stk.empty ()) {
				break;
			}
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

} // namespace tiary
