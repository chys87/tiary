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


#include "common/xml.h"
#include <stack>
#include <vector>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <stdlib.h>
#include <string.h>
#include "common/string.h"

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
XMLNode *shallow_copy(const xmlNode *iptr) {
	switch (iptr->type) {
	case XML_TEXT_NODE:
	case XML_CDATA_SECTION_NODE: { // Text node
		XMLNode *optr = nullptr;
		if (const char *text = (const char *)iptr->content) {
			// If a text node is empty or completely consists of space (tab, newline) etc.
			// Eliminate it!
			std::string_view text_sv = text;
			if (text_sv.find_first_not_of(" \t\r\n\v"sv) != text_sv.npos) {
				optr = new XMLNode(XMLNode::TextTag(), text_sv);
			}
		}
		return optr;
	}
	case XML_ELEMENT_NODE: { // Element node
		XMLNode *optr = new XMLNode(XMLNode::TreeTag(), (const char *)iptr->name);
		// Attributes. Not ordered.
		for (xmlAttrPtr aptr=iptr->properties; aptr; aptr=aptr->next) {
			if (aptr->name && aptr->children && aptr->children->content) {
				optr->properties.emplace((const char *)aptr->name, (const char *)aptr->children->content);
			}
		}
		return optr;
	}
	default: // Other kinds of nodes. Not supported.
		return nullptr;
	}
}

// The other way
xmlNodePtr shallow_copy(const XMLNode *iptr) {
	switch (iptr->type) {
	case XMLNodeType::kText:
		return xmlNewTextLen(BAD_CAST(iptr->text().c_str()), iptr->text().length());
	case XMLNodeType::kTree: {
		xmlNodePtr optr = xmlNewNode(0, BAD_CAST(iptr->name().c_str()));
		// Copy attributes.
		for (const auto &prop_pair: iptr->properties) {
			xmlNewProp(optr, BAD_CAST(prop_pair.first.c_str()), BAD_CAST(prop_pair.second.c_str()));
		}
		return optr;
	}
	default:
		return nullptr;
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
		xmlKeepBlanksDefault(0);
		xmlIndentTreeOutput = 1;
		called = true;
	}
}

} // anonymous namespace

XMLNode *xml_parse(const char *str, size_t len) {
	libxml2_init ();

	xmlDocPtr doc = xmlReadMemory(str, len, 0, 0, 0);
	if (doc == nullptr) {
		return 0;
	}

	const xmlNode *iptr = xmlDocGetRootElement(doc);
	if (iptr == nullptr) {
		xmlFreeDoc (doc);
		return 0;
	}

	// Successfully parsed. Now we need to construct our own XML tree

	// "(a,b) in stk" means "b's children should be copied as a's children"
	std::stack<std::pair<XMLNode *, const xmlNode *>, std::vector<std::pair<XMLNode *, const xmlNode *>>> stk;

	XMLNode *root = shallow_copy (iptr);
				// Current working output node
				// Current working input node is iptr
	if (root->type != XMLNodeType::kTree) {
		// Root is a text node. Error.
		delete root;
		return 0;
	}
	XMLNode *optr = root;
	for (;;) {
		// Shallow copy all children of current node
		optr->children = nullptr;
		XMLNode **tail = &optr->children;
		for (const xmlNode *child_ptr = iptr->xmlChildrenNode; child_ptr; child_ptr = child_ptr->next) {
			if (XMLNode *newnode = shallow_copy (child_ptr)) {
				*tail = newnode;
				tail = &newnode->next;
				if (child_ptr->xmlChildrenNode) {
					stk.push(std::make_pair(newnode, child_ptr));
				}
			}
		}
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
		xmlDocDumpFormatMemoryEnc(doc, &str, &len, "UTF-8", 1);
		xmlFreeDoc (doc);

		ret.assign ((const char*)str, len);
		xmlFree (str);
	}
	return ret;
}

} // namespace tiary
