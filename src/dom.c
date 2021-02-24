/*
  cxproc - Configurable Xml PROCessor

  Copyright (C) 2006..2020 by Alexander Tenbusch

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License 
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

*/


/* 
 */
#include <libxml/HTMLtree.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

#include <libxslt/xslt.h>
    
/* 
 */
#include "basics.h"
#include "utils.h"
#include <res_node/res_node_io.h>
#include <cxp/cxp.h>

#ifdef HAVE_PIE
#include <pie/pie_text.h>
#else
#include <pie/pie_dtd.h>
#endif

#include <cxp/cxp_dir.h>
#include "dom.h"

#ifdef HAVE_PETRINET
#include <petrinet/petrinet.h>
#endif


/*!\todo initialize global xmlNsPtr for XSL*/
xmlNsPtr pnsXsl = NULL;

const xmlChar *pucXsl = BAD_CAST "http://www.w3.org/1999/XSL/Transform";


static int
IncrementWeightProp(xmlNodePtr pndArg, int iArg);


/*! cleanup this module
*/
void
domCleanup(void)
{
  if (pnsXsl) {
    xmlFreeNs(pnsXsl);
  }
} /* end of domCleanup() */


/*! searchs under the nexts of 'pndArg' for an element named 'pucNameElement'

\param pndArg is the current node (for recursion)
\param pucNameElement string for name element

\return the next node with name 'pucNameElement'
*/
xmlNodePtr
domGetNextNode(xmlNodePtr pndArg, xmlChar *pucNameElement)
{
  xmlNodePtr pndCurrent;

  if (pndArg) {
    if (pucNameElement==NULL) {
      pucNameElement = BAD_CAST pndArg->name;
    }
    for (pndCurrent = pndArg->next; pndCurrent; pndCurrent = pndCurrent->next) {
      if (pndCurrent->type == XML_ELEMENT_NODE
	&& !xmlStrcasecmp(pndCurrent->name, pucNameElement)) {
	return pndCurrent;
      }
    }
  }
  return NULL;
}
/* End of domGetNextNode() */


/*! \return a pointer to xmlXPathObject according to XPath 'pucArg' in pdocArg was found
\param pdocArg source DOM
\param pucArg pointer to XPath string

 http://xmlsoft.org/tutorial/apd.html "D. Code for XPath Example"
*/
xmlXPathObjectPtr
domGetXPathNodeset(xmlDocPtr pdocArg, xmlChar *pucArg)
{
  xmlXPathObjectPtr result = NULL;

  if (pdocArg != NULL && STR_IS_NOT_EMPTY(pucArg)) {
    xmlXPathContextPtr context;

    if ((context = xmlXPathNewContext(pdocArg))) {
      if ((result = xmlXPathEvalExpression(pucArg, context))) {
	if (xmlXPathNodeSetIsEmpty(result->nodesetval)){
	  PrintFormatLog(2, "XPath: no nodes for '%s' found", pucArg);
	  xmlXPathFreeObject(result);
	  result = NULL;
	}
	else { /* result is OK */
	  assert(result->nodesetval);
	  PrintFormatLog(2, "XPath: %i nodes for '%s' found", result->nodesetval->nodeNr, pucArg);
	}
      }
      else {
	PrintFormatLog(1, "XPath: Error in xmlXPathEvalExpression '%s'", pucArg);
      }
      xmlXPathFreeContext(context);
      /* Error in xmlXPathNewContext */
    }
  }
  return result;
} /* end of domGetXPathNodeset() */


/*! increments value of property "w" by iArg numerically
\param pndArg node for attribute
\param iArg default integer value
*/
int
IncrementWeightProp(xmlNodePtr pndArg, int iArg)
{
  int iResult = 0;

  if ((pndArg != NULL) && (pndArg->type == XML_ELEMENT_NODE) && iArg != 0) {
    int iCurrent;
    xmlAttrPtr patT;

    if ((patT = xmlHasProp(pndArg, BAD_CAST"w")) == NULL
      || patT->children == NULL || STR_IS_EMPTY(patT->children->content)) {
      /* there is no attribute value yet, initial value '1' */
      iResult = 1;
    }
    else if ((iCurrent = atoi((const char *)patT->children->content)) != 0) {
      iResult = iCurrent + iArg;
    }
    else {
      /*\todo remove property if iArg == 0? */
    }

    if (iResult) {
      xmlChar mucCount[32];

      xmlStrPrintf(mucCount, sizeof(mucCount), "%i", iResult);
      xmlSetProp(pndArg, BAD_CAST"w", mucCount);
    }
  }
  return iResult;
} /* end of IncrementWeightProp() */


/*!
 */
BOOL_T
IncrementWeightPropRecursive(xmlNodePtr pndArg)
{
  BOOL_T fResult = FALSE;

  if (pndArg) {
    xmlNodePtr pndT;

    IncrementWeightProp(pndArg, 1);

    for (pndT = pndArg->children; pndT; pndT = pndT->next) {
      IncrementWeightPropRecursive(pndT);
    }

    fResult = TRUE;
  }

  return fResult;
} /* end of IncrementWeightPropRecursive() */


/*! \return TRUE if a node according to XPath 'pucArg' in pdocArg was found
\param pdocArg source DOM
\param pucArg pointer to XPath string
*/
BOOL_T
domWeightXPathInDoc(xmlDocPtr pdocArg, xmlChar *pucArg)
{
  BOOL_T fResult = FALSE;

  if (pdocArg != NULL && STR_IS_NOT_EMPTY(pucArg)) {
    xmlNodePtr pndRoot;

    if ((pndRoot = xmlDocGetRootElement(pdocArg))) {
      xmlXPathObjectPtr result;

      if ((result = domGetXPathNodeset(pdocArg, pucArg)) != NULL) {
	int i;
	xmlNodeSetPtr nodeset;

	nodeset = result->nodesetval;
	if (nodeset->nodeNr > 0) {
	  for (i=0; i < nodeset->nodeNr; i++) {
	    xmlNodePtr pndT;

	    if (IS_NODE_PIE_SECTION(nodeset->nodeTab[i])) {
	      /* weight the tree of this section when element matches */
	      IncrementWeightPropRecursive(nodeset->nodeTab[i]);
	      /* weight all ancestors of this section */
	      for (pndT=nodeset->nodeTab[i]->parent; pndT; pndT=pndT->parent) {
		IncrementWeightProp(pndT, 1);
	      }
	    }
	    else {
	      /* weight all ancestors and this element */
	      for (pndT=nodeset->nodeTab[i]; pndT; pndT=pndT->parent) {
		IncrementWeightProp(pndT, 1);
	      }
	    }
	  }
	  fResult = TRUE;
	}
	xmlXPathFreeObject(result);
      }
    }
  }
  return fResult;
} /* end of domWeightXPathInDoc() */


/*! \return a new DOM according to XPath 'pucArg' in pdocArg
\param pdocArg source DOM
\param pucArg pointer to XPath string
*/
xmlDocPtr
domGetXPathDoc(xmlDocPtr pdocArg, xmlChar *pucArg)
{
  xmlDocPtr pdocResult = NULL;

  if (pdocArg != NULL && STR_IS_NOT_EMPTY(pucArg)) {
    xmlNodePtr pndRoot;

    if ((pndRoot = xmlDocGetRootElement(pdocArg))) {
      xmlChar *pucRootName;

      if ((pucRootName = BAD_CAST pndRoot->name) == NULL) {
	pucRootName = NAME_PIE_PIE;
      }

      if ((pdocResult = xmlNewDoc(BAD_CAST "1.0")) != NULL) {
	xmlNodePtr pndPieXPathRoot;
	xmlXPathObjectPtr result;

	pdocResult->encoding = xmlStrdup(BAD_CAST"UTF-8");
	pndPieXPathRoot = xmlNewNode(NULL, pucRootName);

	if ((result = domGetXPathNodeset(pdocArg, pucArg)) != NULL) {
	  int i;
	  xmlNodeSetPtr nodeset;

	  nodeset = result->nodesetval;
	  for (i=0; i < nodeset->nodeNr; i++) {
	    xmlNodePtr pndCopy;

	    if ((pndCopy = xmlCopyNode(nodeset->nodeTab[i], 1))) {
	      xmlAddChild(pndPieXPathRoot, pndCopy);
	    }
	  }
	  xmlXPathFreeObject(result);
	}

	xmlSetTreeDoc(pndPieXPathRoot, pdocResult);
	xmlDocSetRootElement(pdocResult, pndPieXPathRoot);
      }
    }
  }
  return pdocResult;
} /* end of domGetXPathNodeset() */


/*!
\param pndArg is the current node to set

\return a pointer to private XSL namespace
*/
xmlNsPtr
domGetNsXsl(void)
{
  if (pnsXsl == NULL) {
    pnsXsl = xmlNewNs(NULL, pucXsl, BAD_CAST "xsl");
  }
  return pnsXsl;
} /* End of domGetNsXsl() */


/*!
 */
BOOL_T
domSetNsRecursive(xmlNodePtr pndArg, xmlNsPtr ns)
{
  BOOL_T fResult = FALSE;

  if (pndArg) {
    if (pndArg->ns==NULL) {
      pndArg->ns = ns;
      //domSetNsRecursive((xmlNodePtr)pndArg->properties,ns);
    }
    domSetNsRecursive(pndArg->children, ns);
    domSetNsRecursive(pndArg->next, ns);
    fResult = TRUE;
  }

  return fResult;
} /* end of domSetNsRecursive() */


/*!
*/
void
domUnsetNs(xmlNodePtr pndArg)
{
  if (pndArg) {
    xmlSetNs(pndArg,NULL);

    domUnsetNs((xmlNodePtr)pndArg->properties);
    domUnsetNs(pndArg->children);
    domUnsetNs(pndArg->next);
  }
  return;
} /* end of domUnsetNs() */


/*! counts the number of elements with right name

  \param pndArg is the current node
  \param pucNameElement string for name element
  \param n counter for recursive call

  \return the number of elements with name 'pucNameElement'
*/
index_t
domNumberOf(xmlNodePtr pndArg, xmlChar *pucNameElement, index_t n)
{
  while (pndArg) {
    if (pndArg->type == XML_ELEMENT_NODE && !xmlStrcasecmp(pndArg->name, pucNameElement))
      n++;

    n = domNumberOf(pndArg->children, pucNameElement, n);

    pndArg = pndArg->next;
  }
  return n;
} /* end of domNumberOf() */


/*! counts the number of child elements with right name

  \param pndArg is the current node
  \param pucNameElement string for name element (with pucNameElement==NULL count childs only)

  \return the number of child elements with name 'pucNameElement'
*/
index_t
domNumberOfChild(xmlNodePtr pndArg, xmlChar *pucNameElement)
{
  index_t result = 0;

  if (pndArg) {
    xmlNodePtr pndCurrent;

    for (pndCurrent=pndArg->children; pndCurrent; pndCurrent=pndCurrent->next) {
      if (IS_NODE(pndCurrent,pucNameElement)) {
	result++;
      }
    }
  }
  return result;
} /* end of domNumberOfChild() */


/*! searchs under the childs of 'pndArg' for an element named 'pucNameElement'

  \param pndArg is the current node (for recursion)
  \param pucNameElement string for name element

  \return the child node with name 'pucNameElement'
*/
xmlNodePtr
domGetFirstChild(xmlNodePtr pndArg, xmlChar *pucNameElement)
{
  xmlNodePtr pndResult = NULL;

  if (pndArg) {
    xmlNode *pndCurrent;

    for (pndCurrent = pndArg->children; pndCurrent; pndCurrent = pndCurrent->next) {
      if (IS_NODE(pndCurrent,pucNameElement)) {
	pndResult = pndCurrent;
	break;
      }
    }
  }
  return pndResult;
}
/* End of domGetFirstChild() */


/*! domGetPropFlag
\param pndArg parent node for attributes
\param pucNameAttr name of wanted attribute
\param fDefault default return value if attribute not found
\return a flag value according to attribute value
*/
BOOL_T
domGetPropFlag(xmlNodePtr pndArg, xmlChar *pucNameAttr, BOOL_T fDefault)
{
  xmlChar *pucAttr = domGetPropValuePtr(pndArg,pucNameAttr);

  if (pucAttr) {
    if (xmlStrcasecmp(pucAttr,BAD_CAST "yes")==0) {
      return TRUE;
    }
    if (xmlStrcasecmp(pucAttr,BAD_CAST "no")==0) {
      return FALSE;
    }
  }
  return fDefault;
}
/* End of domGetPropFlag() */


/*! domPropIsEqual
\param pndArg parent node for attributes
\param pucNameAttr name of wanted attribute
\param pucValueAttr compare value of wanted attribute
\return true if value of named argument matches given value
*/
BOOL_T
domPropIsEqual(xmlNodePtr pndArg, xmlChar *pucNameAttr, xmlChar *pucValueAttr)
{
  BOOL_T fResult = FALSE;
  xmlChar *pucAttr = domGetPropValuePtr(pndArg,pucNameAttr);

  if (STR_IS_NOT_EMPTY(pucAttr) && STR_IS_NOT_EMPTY(pucValueAttr)
      && xmlStrcasecmp(pucAttr,pucValueAttr)==0) {
    fResult = TRUE;
  }
  return fResult;
}
/* End of domPropIsEqual() */


/*! similar to xmlGetProp() but returns only a pointer
* 
\param pndArg parent node for attributes
\param pucNameAttr name of wanted attribute
\return the content pointer of the attribute named 'pucNameAttr'
of 'pndArg' OR NULL if no attribute found
*/
xmlChar *
domGetPropValuePtr(xmlNodePtr pndArg, xmlChar *pucNameAttr)
{
  xmlChar *pucResult = NULL;

  if (pndArg != NULL && STR_IS_NOT_EMPTY(pucNameAttr)) {
    xmlAttrPtr patAttr;

    if ((patAttr = xmlHasProp(pndArg, pucNameAttr)) != NULL && patAttr->children != NULL && patAttr->children->type == XML_TEXT_NODE) {
      pucResult = patAttr->children->content;
    }
  }
  return pucResult;
} /* End of domGetPropValuePtr() */


/*!
\param pndArg node for content
\return the content pointer of pndArg (and unlink from node) OR NULL if no content > 0 found
*/
xmlChar *
domNodeEatContent(xmlNodePtr pndArg)
{
  xmlChar *pucResult;

  pucResult = domNodeGetContentPtr(pndArg);
  if (pucResult) {
    if (xmlStrlen(pucResult) > 0) {
      pndArg->children->content = NULL;
    }
    else {
      pucResult = NULL; /* ignore empty node content */
    }
  }
  return pucResult;
}
/* End of domNodeEatContent() */


/*!
\param pndArg node
\return TRUE if pndArg is the root node of his Doc
*/
BOOL_T
domNodeIsDocRoot(xmlNodePtr pndArg)
{
  return (pndArg != NULL && pndArg->parent != NULL && pndArg->parent->type == XML_DOCUMENT_NODE);
}
/* End of domNodeIsDocRoot() */


/*!
\param pndArg node
\return TRUE if pndArg is a ttribute node
*/
BOOL_T
domNodeIsAttribute(xmlNodePtr pndArg)
{
  return (pndArg != NULL && pndArg->type == XML_ATTRIBUTE_NODE);
}
/* End of domNodeIsAttribute() */


/*! return TRUE if there is a parent path from pndArg to pndArgTop
*/
BOOL_T
domNodeIsDescendant(xmlNodePtr pndArgTop, xmlNodePtr pndArg)
{
  if (pndArgTop != NULL && pndArgTop->children != NULL && pndArg != NULL && pndArg->parent) {
    xmlNodePtr pndUp;

    for (pndUp=pndArg->parent; pndUp; pndUp=pndUp->parent) {
      if (IS_ENODE(pndUp) && pndUp == pndArgTop) {
	return TRUE;
      }
    }
  }
  return FALSE;
}
/* end of domNodeIsDescendant() */


/**
 * copy from libxml2-2.9.1/tree.c xmlNodeGetContent():
 *
 * @cur:  the node being read
 *
 * Read the value of a node, this can be either the text carried
 * directly by this node if it's a TEXT node or the aggregate string
 * of the values carried by this node child's (TEXT and ENTITY_REF).
 * Entity references are substituted.
 * Returns the #xmlChar * or NULL if no content is available.
 */
xmlChar *
domNodeGetContentPtr(xmlNodePtr cur)
{
    if (cur == NULL)
        return (NULL);
    switch (cur->type) {
        case XML_DOCUMENT_FRAG_NODE:
        case XML_ATTRIBUTE_NODE:
        case XML_ELEMENT_NODE:{
          if (cur->children != NULL && cur->children->content != NULL) {
            return (cur->children->content);
          }
          return (NULL);
        }
        case XML_COMMENT_NODE:
        case XML_PI_NODE:
                return (cur->content);
        case XML_ENTITY_REF_NODE:
        case XML_ENTITY_NODE:
        case XML_DOCUMENT_TYPE_NODE:
        case XML_NOTATION_NODE:
        case XML_DTD_NODE:
        case XML_XINCLUDE_START:
        case XML_XINCLUDE_END:
            return (NULL);
        case XML_DOCUMENT_NODE:
#ifdef LIBXML_DOCB_ENABLED
        case XML_DOCB_DOCUMENT_NODE:
#endif
        case XML_HTML_DOCUMENT_NODE: {
	    return (NULL);
	}
        case XML_NAMESPACE_DECL: {
            return BAD_CAST (((xmlNsPtr) cur)->href);
	}
        case XML_ELEMENT_DECL:
        case XML_ATTRIBUTE_DECL:
        case XML_ENTITY_DECL:
            return (NULL);
        case XML_CDATA_SECTION_NODE:
        case XML_TEXT_NODE:
                return (cur->content);
    }
    return (NULL);
}
/* End of domNodeGetContentPtr() */


/*!
\param pndArg node to delete attributes
*/
void
domUnsetPropAll(xmlNodePtr pndArg)
{
  if (pndArg) {
    xmlFreeNodeList((xmlNodePtr)pndArg->properties);
    pndArg->properties = NULL;
  }
} /* End of domUnsetPropAll() */


/*! Adds locator attribute to all descendant element nodes.

\param pndArg pointer to node to add attribute
\param pucArg pointer to locator string for childs
 */
void
domUnsetPropFileLocator(xmlNodePtr pndArg)
{
  if (IS_NODE_META(pndArg) || IS_NODE_PIE_ERROR(pndArg)) {
  }
  else if (IS_ENODE(pndArg)) {
    xmlNodePtr pndChild;
    for (pndChild = pndArg->children; pndChild != NULL; pndChild = pndChild->next) {
      xmlUnsetProp(pndChild,BAD_CAST"flocator");
      xmlUnsetProp(pndChild,BAD_CAST"fxpath");
      domUnsetPropFileLocator(pndChild);
    }
  }
}
/* End of domUnsetPropFileLocator() */


/*! Adds locator attribute to all descendant element nodes.

\param pndArg pointer to node to add attribute
\param pucArg pointer to locator string for childs
 */
void
domSetPropFileLocator(xmlNodePtr pndArg, xmlChar *pucArg)
{
  if (pucArg == NULL || isend(*pucArg)) {
    /* no usable value, dont set the attribute, no recursion */
  }
  else if (IS_NODE_META(pndArg) || IS_NODE_PIE_ERROR(pndArg)) {
  }
  else if (IS_ENODE(pndArg)) {
    xmlNodePtr pndChild;
    for (pndChild = pndArg->children; pndChild != NULL; pndChild = pndChild->next) {
      if (IS_NODE_PIE_SECTION(pndChild) || IS_NODE(pndChild,BAD_CAST"node")) {
	xmlSetProp(pndChild,BAD_CAST"flocator",pucArg);
	domSetPropFileLocator(pndChild,pucArg);
      }
      else if (IS_NODE_PIE_TASK(pndChild)
	|| IS_NODE_PIE_TARGET(pndChild)
	|| IS_NODE_PIE_PRE(pndChild)
#ifdef HAVE_PETRINET
	|| IS_NODE_PKG2_STATE(pndChild)
	|| IS_NODE_PKG2_TRANSITION(pndChild)
	|| IS_NODE_PKG2_REQUIREMENT(pndChild)
#endif
	) {
	xmlSetProp(pndChild,BAD_CAST"flocator",pucArg);
      }
      else {
	domSetPropFileLocator(pndChild,pucArg);
      }
    }
  }
}
/* End of domSetPropFileLocator() */


/*! Adds XPath attribute to all descendant element nodes.

\param pndArg pointer to node to add attribute
\param pucArgPrefix pointer to XPath prefix for childs
 */
void
domSetPropFileXpath(xmlNodePtr pndArg, xmlChar* pucArgName, xmlChar* pucArgPrefix)
{
  if (IS_NODE_META(pndArg) || IS_NODE_PIE_ERROR(pndArg)) {
  }
  else if (IS_ENODE(pndArg)) {
    xmlNodePtr pndChild;
    int i=0;

    for (pndChild = pndArg->children; pndChild != NULL; pndChild = pndChild->next) {

      if (IS_ENODE(pndChild)) {
#if 1
	xmlChar mucT[BUFFER_LENGTH];

	i++;
	xmlStrPrintf(mucT, BUFFER_LENGTH, "%s/*[%i]", (pucArgPrefix==NULL ? BAD_CAST "/*" : pucArgPrefix), i);
	xmlSetProp(pndChild, pucArgName, mucT);
	domSetPropFileXpath(pndChild, pucArgName, mucT);
#else
	xmlChar* pucT = xmlGetNodePath(pndChild);

	xmlSetProp(pndChild, pucArgName, pucT);
	xmlFree(pucT);
	domSetPropFileXpath(pndChild, pucArgName, pucT);
#endif
      }
    }
  }
  /*!\todo exclude import text nodes and existing attributes also */
}
/* End of domSetPropFileXpath() */


/*! Delete all descendant element nodes named pucArgName.

\param pndArg pointer to node
\param pucArgName pointer to name string for nodes to delete
 */
void
domFreeNodeByName(xmlNodePtr pndArg, xmlChar *pucArgName)
{
  if (IS_ENODE(pndArg)) {
    xmlNodePtr pndChild;
    xmlNodePtr pndChildNext = NULL;

    for (pndChild = pndArg->children; pndChild != NULL; pndChild = pndChildNext) {
      pndChildNext = pndChild->next;
      if (xmlStrEqual(pndChild->name, pucArgName)) {
	xmlUnlinkNode(pndChild);
	xmlFreeNode(pndChild);
      }
      else {
	domFreeNodeByName(pndChild, pucArgName);
      }
    }
  }
} /* End of domFreeNodeByName() */


/*! \return a new Doc with a copy of pndArg
*/
xmlDocPtr
domDocFromNodeNew(xmlNodePtr pndArg)
{
  xmlDocPtr pdocResult = NULL;

  if (pndArg) {
    pdocResult = xmlNewDoc(BAD_CAST "1.0");
    if (pdocResult) {
      xmlNodePtr pndCopy;

      pdocResult->encoding = xmlStrdup(BAD_CAST "UTF-8");
      //pdocResult->charset = XML_CHAR_ENCODING_NONE;
      pndCopy = xmlCopyNode(pndArg,1);
      if (pndCopy) {
	xmlSetTreeDoc(pndCopy, pdocResult);
	xmlDocSetRootElement(pdocResult, pndCopy);
	if (pndArg->doc != NULL && STR_IS_NOT_EMPTY(pndArg->doc->URL)) {
	  pdocResult->URL = xmlStrdup(pndArg->doc->URL);
	}
      }
      else {
	xmlFreeDoc(pdocResult);
	pdocResult = NULL;
      }
    }
  }
  return pdocResult;
}
/* end of domDocFromNodeNew() */


/*! \return MIME type of this DOM
  \param pdocArg source DOM
 */
RN_MIME_TYPE
domMimeType(xmlDocPtr pdocArg)
{
  RN_MIME_TYPE eTypeResult = MIME_UNDEFINED;
  
  if (pdocArg) {
    xmlNodePtr pndRoot;

    if ((pndRoot = xmlDocGetRootElement(pdocArg)) != NULL) {
      if (xmlStrEqual(pndRoot->name,BAD_CAST "map")) {
	eTypeResult = MIME_APPLICATION_MM_XML;
      }
      else if (xmlStrEqual(pndRoot->name,BAD_CAST "make")) {
	eTypeResult = MIME_APPLICATION_CXP_XML;
      }
      else if (xmlStrEqual(pndRoot->name,BAD_CAST "html")) {
	eTypeResult = MIME_TEXT_HTML;
      }
#ifdef HAVE_PIE
      else if (xmlStrEqual(pndRoot->name,BAD_CAST "pie")) {
	eTypeResult = MIME_APPLICATION_PIE_XML;
      }
#endif
      else {
	eTypeResult = MIME_TEXT_XML;
      }
    }
  }
  return eTypeResult;
} /* end of domMimeType() */


/*! \return TRUE if DOM pdocArg is HTML
 */
BOOL_T
domDocIsHtml(xmlDocPtr pdocArg)
{
  xmlNodePtr pndRoot;

  return (pdocArg != NULL && (pndRoot = xmlDocGetRootElement(pdocArg)) != NULL
      && xmlStrcasecmp(pndRoot->name,BAD_CAST "html") == 0 && pndRoot->children != NULL
      && (domGetFirstChild(pndRoot,BAD_CAST "head") != NULL || domGetFirstChild(pndRoot,BAD_CAST "body") != NULL));
}
/* end of domDocIsHtml() */


#ifdef DEBUG

/*! 
*/
int
domPutNodeString(FILE *out, xmlChar *pucArgMessage, xmlNodePtr pndArg)
{
  int iResult = EOF;
  xmlDocPtr pdocT = NULL;
  
  if (pndArg) {
    pdocT = domDocFromNodeNew(pndArg);
  }
  
  iResult = domPutDocString(out,pucArgMessage,pdocT);

  xmlFreeDoc(pdocT);
  
  return iResult;
} /* end of domPutNodeString() */


/*! 
*/
int
domPutDocString(FILE *out, xmlChar *pucArgMessage, xmlDocPtr pdocArg)
{
  int iResult = EOF;
  
#if 0
  out = fopen("cxproc.log","w");
#else
  if (out == NULL) {
    out = stderr;		/* default */
  }
#endif

  if (STR_IS_NOT_EMPTY(pucArgMessage)) {
    fputs((const char *)pucArgMessage, out);
    fputs("\n",out);
  }
  
  if (pdocArg != NULL && STR_IS_NOT_EMPTY(pdocArg->URL)) {
    fputs((const char *)pdocArg->URL,out);
    fputs("\n",out);
  }

  if (pdocArg) {
    //iResult = xmlDocFormatDump(out, pdocArg, 1);
    iResult = xmlSaveFormatFile("-", pdocArg, 1);
  }

#if 0
  fclose(out);
#endif

  return iResult;
} /* end of domPutDocString() */


/*! 
*/
BOOL_T
domPutNodeGraphvizString(char *pchNameFile, xmlNodePtr pndArg, int iArgDepth)
{
  if (pndArg) {
    resNodePtr prnT;

    prnT = resNodeDirNew(BAD_CAST pchNameFile);
    if (prnT) {
      if (resNodeOpen(prnT,"wb")) {
        PrintFormatLog(1,"Write Dump to '%s'", resNodeGetNameNormalized(prnT));
        fprintf((FILE *)resNodeGetHandleIO(prnT),"digraph dump {\nnode [shape=record];\n");
        fprintf((FILE *)resNodeGetHandleIO(prnT),"//rankdir=\"LR\";\n");
        domPutNodeGraphvizStringRecursive((FILE *)resNodeGetHandleIO(prnT),pndArg,iArgDepth);
        fprintf((FILE *)resNodeGetHandleIO(prnT),"\n}\n\n");
        resNodeClose(prnT);
        return TRUE;
      }
      else {
        PrintFormatLog(1,"Error resNodeOpen()");
      }
      resNodeFree(prnT);
    }
    else {
      PrintFormatLog(1,"Error resNodeDirNew()");
    }
  }
  return FALSE;
}
/* end of domPutNodeGraphvizString() */


/*! 
*/
void
domPutNodeGraphvizStringRecursive(FILE *out, xmlNodePtr pndArg, int iArgDepth)
{
  if (pndArg && iArgDepth > 0) {
#if 1
    if (pndArg->type == XML_ELEMENT_NODE) {
      fprintf(out,
	      "\"%p\" [label = \"ELEMENT|\\\"%s\\\"\"];\n",
	      (void *) pndArg,
//	      ((pndArg->ns == NULL) ? "null" : (char *)pndArg->ns->prefix),
	      (char *)pndArg->name);
    }
    else if (pndArg->type == XML_TEXT_NODE) {
      xmlChar *pucA = BAD_CAST xmlStrchr(pndArg->content,'\n');
      int iLengthMax = (pucA ? pucA - pndArg->content : 8);
      fprintf(out,
	      "\"%p\" [label = \"TEXT|\\\"%s\\\"\"];\n",
	      (void *) pndArg,
	      (char *)xmlStrsub(pndArg->content,0,iLengthMax)
	      );
    }
#if 0
    else if (pndArg->type == XML_ATTRIBUTE_NODE) {
      fprintf(out,"\"%p\" [label = \"ATTRIBUTE|\\\"%s\\\"\"];\n", (void *) pndArg, (char *)pndArg->name);
    }
#endif
    else if (pndArg->type == XML_PI_NODE) {
      fprintf(out,"\"%p\" [label = \"PI|\\\"%s\\\"\"];\n", (void *) pndArg, (char *)pndArg->name);
    }
    else if (pndArg->type == XML_ENTITY_REF_NODE) {
      fprintf(out,"\"%p\" [label = \"ENTITY_REF|\\\"%s\\\"\"];\n", (void *) pndArg, (char *)pndArg->name);
    }
#if 0
    if (pndArg->parent)
      fprintf(out,"\"%p\" -> \"%p\" [label = \"%s\"];\n", (void *) pndArg, (void *) (pndArg->parent ? pndArg->parent : pndArg), "parent");
#endif
    if (pndArg->next)
      fprintf(out,"\"%p\" -> \"%p\" [label = \"%s\"];\n", (void *) pndArg, (void *) (pndArg->next ? pndArg->next : pndArg), "next");
    if (pndArg->prev)
      fprintf(out,"\"%p\" -> \"%p\" [label = \"%s\"];\n", (void *) pndArg, (void *) (pndArg->prev ? pndArg->prev : pndArg), "prev");
    if (pndArg->children)
      fprintf(out,"\"%p\" -> \"%p\" [label = \"%s\"];\n", (void *) pndArg, (void *) (pndArg->children ? pndArg->children : pndArg), "children");
    if (pndArg->last)
      fprintf(out,"\"%p\" -> \"%p\" [label = \"%s\"];\n", (void *) pndArg, (void *) (pndArg->last ? pndArg->last : pndArg), "last");
#if 0
    if (pndArg->properties)
      fprintf(out,"\"%p\" -> \"%p\" [label = \"%s\"];\n", (void *) pndArg, (void *) (pndArg->properties ? pndArg->properties : pndArg), "properties");
#endif
#else
    if (pndArg->type == XML_ELEMENT_NODE) {
      fprintf(out,"ELEMENT|%s", (char *)pndArg->name);
    }
    else if (pndArg->type == XML_TEXT_NODE) {
      fprintf(out,"TEXT|%s", (char *)xmlStrsub(pndArg->content,0,8));
    }
    else if (pndArg->type == XML_ATTRIBUTE_NODE) {
      fprintf(out,"ATTRIBUTE|%s", (char *)pndArg->name);
    }
    else if (pndArg->type == XML_ENTITY_REF_NODE) {
      fprintf(out,"ENTITY_REF|%s", (char *)pndArg->name);
    }
#endif
    fprintf(out,"\n");
    domPutNodeGraphvizStringRecursive(out,(xmlNodePtr)pndArg->properties,iArgDepth);
    domPutNodeGraphvizStringRecursive(out,pndArg->children,iArgDepth-1);
    domPutNodeGraphvizStringRecursive(out,pndArg->next,iArgDepth-1);
  }
  return;
}
/* end of domPutNodeGraphvizStringRecursive() */

#endif


/*! \return TRUE if two node with all childrens are equal
\param pndA first candidate
\param pndB second candidate
*/
BOOL_T
domNodesAreEqual(xmlNodePtr pndA, xmlNodePtr pndB)
{
  xmlNodePtr pndChildA;
  xmlNodePtr pndChildB;

  if ( pndA==NULL || pndB==NULL) {
    /* pndA or pndB is NULL */
    return FALSE;
  }

  if (pndA == pndB) {
    /* pointers to same nodes */
    return TRUE;
  }

  /* pointers to different nodes, compare node element properties
   */
  if ( pndA->type != pndB->type
       || !xmlStrEqual(pndA->name,pndB->name)
       || !xmlStrEqual(pndA->content,pndB->content)) {
    return FALSE;
  }

  /* compare all childrens */
  for (pndChildA = pndA->children, pndChildB = pndB->children;
       pndChildA && pndChildB;
       pndChildA = pndChildA->next, pndChildB = pndChildB->next ) {
    if (!domNodesAreEqual(pndChildA, pndChildB)) {
      return FALSE;
    }
  }
  /* compare all properties */
  for (pndChildA = (xmlNodePtr)pndA->properties, pndChildB = (xmlNodePtr)pndB->properties;
       pndChildA && pndChildB;
       pndChildA = pndChildA->next, pndChildB = pndChildB->next ) {
    if (!domNodesAreEqual(pndChildA, pndChildB)) {
      return FALSE;
    }
  }
  return TRUE;
} /* end of domNodesAreEqual() */


/*! derived from xmlReplaceNode()

replace element tree old by node list cur
 */
xmlNodePtr
domReplaceNodeList(xmlNodePtr old, xmlNodePtr cur)
{
  xmlNodePtr last = cur;
  xmlNodePtr pndT;

  assert(cur != NULL);
  assert(old != NULL);
  assert(old != cur);
  //assert(cur->parent == NULL);
  //assert(old->parent != NULL);

  if (old == NULL || old == cur) return(NULL);
#if 0
  if ((old == NULL) || (old->parent == NULL)) {
#ifdef DEBUG_TREE
	xmlGenericError(xmlGenericErrorContext,
		"xmlReplaceNode : old == NULL or without parent\n");
#endif
	return(NULL);
    }
#endif
    if (cur == NULL) {
	xmlUnlinkNode(old);
	return(old);
    }
    if (cur == old) {
	return(old);
    }
    if ((old->type==XML_ATTRIBUTE_NODE) && (cur->type!=XML_ATTRIBUTE_NODE)) {
#ifdef DEBUG_TREE
	xmlGenericError(xmlGenericErrorContext,
		"xmlReplaceNode : Trying to replace attribute node with other node type\n");
#endif
	return(old);
    }
    if ((cur->type==XML_ATTRIBUTE_NODE) && (old->type!=XML_ATTRIBUTE_NODE)) {
#ifdef DEBUG_TREE
	xmlGenericError(xmlGenericErrorContext,
		"xmlReplaceNode : Trying to replace a non-attribute node with attribute node\n");
#endif
	return(old);
    }
    domUnlinkNodeList(cur);	/* unlink new node list from previous context */

    while (cur != NULL) {
      xmlNodePtr next;

      last = cur;
      next = cur->next;

      xmlSetTreeDoc(cur, old->doc);
      cur->parent = old->parent;
      if (cur->parent != NULL) {
	if (cur->type == XML_ATTRIBUTE_NODE) {
	  if (cur->parent->properties == (xmlAttrPtr)old)
	    cur->parent->properties = ((xmlAttrPtr)cur);
	}
	else {
	  if (cur->parent->children == old)
	    cur->parent->children = cur;
	  if (cur->parent->last == old)
	    cur->parent->last = cur;
	}
      }

      if (cur->prev==NULL) {
	/* cur is first node */
	cur->prev = old->prev;
	if (cur->prev != NULL)
	  cur->prev->next = cur;
      }

      if (cur->next==NULL) {
	/* cur is last node in list */
	cur->next = old->next;
	if (cur->next != NULL)
	  cur->next->prev = cur;
      }
      cur = next;
    }

#if 0
    if (old->parent != NULL && old->parent->last == old)
      old->parent->last = last;
#else
    if (old->parent != NULL) {
      for (pndT=old->parent->children; pndT; pndT = pndT->next) {
	old->parent->last = pndT; /*\todo cleanup this workaround */
      }
    }
#endif

    old->next = NULL;
    old->prev = NULL;
    old->parent = NULL;
    old->doc = NULL;

    return(old);
}
/* end of domReplaceNodeList() */


/* derived from from xmlUnlinkNode() in libxml2-2.6.26/tree.c */
/**
 * domUnlinkNodeList:
 * @cur:  the node
 *
 * Unlink a node from it's current context, the node is not freed
 * 
 * \bug handling of used namespaces
 */
void
domUnlinkNodeList(xmlNodePtr cur) {

  if (cur) {

    if (cur->type == XML_ELEMENT_NODE && cur == xmlDocGetRootElement(cur->doc)) {
      xmlUnlinkNode(cur); 	/* unlink root node */
    }

    if (cur->type == XML_DTD_NODE) {
      xmlUnlinkNode(cur);
    }

    if (cur->parent != NULL && cur->parent != (xmlNodePtr)cur->doc) {
      /*
	 regular node
      */
      xmlNodePtr parent;
      xmlNodePtr pndNext;

      parent = cur->parent;
      if (cur->type == XML_ATTRIBUTE_NODE) {
	if (parent->properties == (xmlAttrPtr)cur)
	  parent->properties = NULL;
      }
      else {
	/* unlink references from parents */
	if (parent->children == cur)
	  parent->children = cur->prev;
	parent->last = cur->prev;
      }

      /* unlink references to parents */
      for (pndNext=cur; pndNext; pndNext=pndNext->next) {
	//if (pndNext->type == XML_ELEMENT_NODE) {
	xmlSetTreeDoc(pndNext, NULL);
	pndNext->parent = NULL;
	//}
      }
    }

    /* unlink references from previous */
    if (cur->prev != NULL) {
      cur->prev->next = NULL;
      cur->prev = NULL;
    }
  }
} /* end of domUnlinkNodeList() */


/*! increments value of named property by iArg numerically
 */
void
domIncrProp(xmlNodePtr pndArg, xmlChar *pucArg, int iArg) 
{
  if ((pndArg != NULL) && (pndArg->type == XML_ELEMENT_NODE) && STR_IS_NOT_EMPTY(pucArg)) {
    xmlAttrPtr patT;
    xmlChar mucCount[32];

    patT = xmlHasProp(pndArg, pucArg);
    if (patT) {
      if (iArg != 0 && patT->children != NULL && STR_IS_NOT_EMPTY(patT->children->content)) {
	int iCurrent;

	iCurrent = atoi((const char *)patT->children->content);
	xmlStrPrintf(mucCount,sizeof(mucCount),"%i", iCurrent + iArg);
	xmlSetProp(pndArg, pucArg, mucCount);
      }
    }
    else {
      /* there is no attribute yet, implicit value '1' */
      xmlStrPrintf(mucCount, sizeof(mucCount), "%i", iArg + 1);
      xmlSetProp(pndArg, pucArg, mucCount);
    }
  }
} /* end of domIncrProp() */


/*! same result like xmlSetProp(), but pucValue is freed at last
*/
xmlAttrPtr
domSetPropEat(xmlNodePtr pndArg, xmlChar *pucArg, xmlChar *pucValue) 
{
  xmlAttrPtr patResult = NULL;

  if ((patResult = xmlSetProp(pndArg, pucArg, pucValue))) {
  }
  xmlFree(pucValue);

  return patResult;
} /* end of domSetPropEat() */


/*! 
*/
xmlChar *
domGetXslOutputMethod(xmlDocPtr pdocArg)
{
  xmlChar *pucMethod = NULL;
  xmlNodePtr pndArgXslOutput;
  xmlNodePtr pndRootXsl;

  if ((pndRootXsl = xmlDocGetRootElement(pdocArg)) == NULL
      || ! IS_NODE(pndRootXsl,BAD_CAST"stylesheet")) {
    //domPutDocString(stderr, "DOM contains no XML stylesheet", pdocArg);
  }
  else if ((pndArgXslOutput = domGetFirstChild(pndRootXsl,BAD_CAST"output")) == NULL
      || (pucMethod = domGetPropValuePtr(pndArgXslOutput,BAD_CAST"method")) == NULL) {
    PrintFormatLog(1,"XML stylesheet contains no output method");
  }

  return pucMethod;
}
/* end of domGetXslOutputMethod() */


/*! returns TRUE if pndArgNeedle is a node in tree pndArgHaystack
 */
BOOL_T
domIsNodeInTree(xmlNodePtr pndArgHaystack, xmlNodePtr pndArgNeedle)
{
  if (pndArgHaystack != NULL && pndArgNeedle != NULL) {
    xmlNodePtr pndT;

    if (pndArgHaystack == pndArgNeedle) {
      return TRUE;
    }

    /* check node properties */
    if (domIsNodeInTree((xmlNodePtr)pndArgHaystack->properties, pndArgNeedle)) {
      return TRUE;
    }

    /* check node childs */
    if (domIsNodeInTree(pndArgHaystack->children, pndArgNeedle)) {
      return TRUE;
    }

    /* check node nexts */
    for (pndT = pndArgHaystack->next; pndT != NULL; pndT = pndT->next) {
      if (domIsNodeInTree(pndT, pndArgNeedle)) {
	return TRUE;
      }
    }
  }

  return FALSE;
}
/* end of domIsNodeInTree() */


/*! \return TRUE if 'pndArg' has an ancestor node with name 'pucArg'
 */
BOOL_T
domNodeHasAncestor(xmlNodePtr pndArg, xmlChar* pucArg)
{
  BOOL_T fResult = FALSE;

  if (pndArg != NULL && STR_IS_NOT_EMPTY(pucArg)) {
    xmlNodePtr pndT;

    /* check node ancestors */
    for (pndT = pndArg->parent; fResult == FALSE && pndT != NULL; pndT = pndT->parent) {
      if (IS_NODE(pndT, pucArg)) {
	fResult = TRUE;
      }
    }
  }
  return fResult;
} /* end of domNodeHasAncestor() */


/*! returns TRUE if there is an overlapping between pndArgA and pndArgB
*/
BOOL_T
domIsTreeOverlapping(xmlNodePtr pndArgA, xmlNodePtr pndArgB)
{
  if (pndArgA != NULL && pndArgB != NULL) {
    xmlNodePtr pndT;

    if (domIsNodeInTree(pndArgA, pndArgB)) {
      return TRUE;
    }

    /* check node properties */
    if (domIsTreeOverlapping(pndArgA, (xmlNodePtr)pndArgB->properties)) {
      return TRUE;
    }

    /* check node childs */
    if (domIsTreeOverlapping(pndArgA, pndArgB->children)) {
      return TRUE;
    }

    /* check node nexts */
    for (pndT = pndArgB->next; pndT != NULL; pndT = pndT->next) {
      if (domIsTreeOverlapping(pndArgA, pndT)) {
	return TRUE;
      }
    }
  }
  return FALSE;
}
/* end of domIsTreeOverlapping() */


/*! unlinks all element trees containing attribute valid="no"
 */
xmlNodePtr
domValidateTree(xmlNodePtr pndArg)
{
  if (IS_ENODE(pndArg)) {

    if (IS_VALID_NODE(pndArg) == FALSE) {
      xmlNodePtr pndRelease = pndArg;
      xmlUnlinkNode(pndRelease);
      xmlFreeNode(pndRelease);
    }
    else {
      xmlNodePtr pndChild;
      xmlNodePtr pndNext = NULL;
      for (pndChild = pndArg->children; pndChild != NULL; pndChild = pndNext) {
	pndNext = pndChild->next;
	domValidateTree(pndChild);
      }
    }
  }
  return NULL;
}
/* end of domValidateTree() */


/*! unlinks all element trees containing attribute valid="no"
 */
xmlNodePtr
domAddNodeToError(xmlDocPtr pdocArg, xmlNodePtr pndArg)
{
  xmlNodePtr pndError;
  xmlNodePtr pndMeta;
  xmlNodePtr pndRoot;

  assert(pdocArg);
  pndRoot = xmlDocGetRootElement(pdocArg);
  assert(pndRoot);
  pndMeta = domGetFirstChild(pndRoot,NAME_META);
  if (pndMeta == NULL) {
      pndMeta = xmlNewChild(pndRoot,NULL,NAME_META,NULL);
  }
  pndError = domGetFirstChild(pndMeta,NAME_ERROR);
  if (pndError == NULL) {
    pndError = xmlNewChild(pndMeta,NULL,NAME_ERROR,NULL);
  }
  return xmlAddChild(pndError,pndArg);
}
/* End of domAddNodeToError() */


#ifdef HAVE_PCRE2

/*! greps for a regexp in all descendant element nodes.

\param pndArg pointer to node to add attribute
 */
BOOL_T
domGrepRegExpInTree(xmlNodePtr pndResultArg, xmlNodePtr pndArg, const pcre2_code *re_grep)
{
  BOOL_T fResult = FALSE;
  
  if (pndResultArg == NULL || pndArg == NULL || re_grep == NULL) {
  }
  else if (IS_ENODE(pndArg)) {
    int rc;
    xmlChar *pucText;
    xmlNodePtr pndChild;
    xmlNodePtr pndMatch = NULL;
    xmlNodePtr pndT;
    xmlAttrPtr pndAttr;
    pcre2_match_data *match_data;

    /*!
      search in all node attributes
    */
    for (pndAttr = pndArg->properties; pndAttr != NULL; pndAttr = pndAttr->next) {
    
	pucText = pndAttr->children->content;
	match_data = pcre2_match_data_create_from_pattern(re_grep, NULL);
	rc = pcre2_match(
			 re_grep,        /* result of pcre2_compile() */
			 (PCRE2_SPTR8)pucText,  /* the subject string */
			 xmlStrlen(pucText),   /* the length of the subject string */
			 0,              /* start at offset 0 in the subject */
			 0,              /* default options */
			 match_data,        /* vector of integers for substring information */
			 NULL);            /* number of elements (NOT size in bytes) */

	if (rc > -1) {
	  pndMatch = xmlNewChild(pndResultArg,NULL,NAME_MATCH,NULL);
	  domSetPropEat(pndMatch, BAD_CAST"xpath", xmlGetNodePath(pndArg));
	  pndT = xmlNewChild(pndMatch,NULL,pndArg->name,NULL);
	  //pndT = xmlNewChild(pndT,NULL,pndAttr->name,pucText);
	  xmlSetProp(pndT, pndAttr->name, pucText);
	  fResult = TRUE;
	}
    
	pcre2_match_data_free(match_data);   /* Release memory used for the match */
    }

    /*!
      search in all node childs
    */
    for (pndChild = pndArg->children; pndChild != NULL; pndChild = pndChild->next) {

      if (IS_ENODE(pndChild)) {
	fResult |= domGrepRegExpInTree(pndResultArg,pndChild,re_grep);
      }
      else if (IS_TEXT(pndChild)) {
	pucText = pndChild->content;
    
	match_data = pcre2_match_data_create_from_pattern(re_grep, NULL);
	rc = pcre2_match(
			 re_grep,        /* result of pcre2_compile() */
			 (PCRE2_SPTR8)pucText,  /* the subject string */
			 xmlStrlen(pucText),   /* the length of the subject string */
			 0,              /* start at offset 0 in the subject */
			 0,              /* default options */
			 match_data,        /* vector of integers for substring information */
			 NULL);            /* number of elements (NOT size in bytes) */

	if (rc > -1) {
	  pndMatch = xmlNewChild(pndResultArg,NULL,NAME_MATCH,NULL);
	  domSetPropEat(pndMatch, BAD_CAST"xpath", xmlGetNodePath(pndChild->parent));
	  xmlNewChild(pndMatch,NULL,pndChild->parent->name,pucText);
	  /*!\todo split text() into resulting substrings */
	  /*!\todo xmlSetProp(pndMatch, BAD_CAST"n", BAD_CAST"3"); */
	  fResult = TRUE;
	}
    
	pcre2_match_data_free(match_data);   /* Release memory used for the match */
      }
    }

  }
  return fResult;
} /* End of domGrepRegExpInTree() */


/*! 
\param pndArg pointer to a node to start grep
\param pucArgGrep pointer to a regexp string

\return a pointer to a grep node with all matching text nodes as childs
 */
xmlNodePtr
domNodeGrepNew(xmlNodePtr pndArg, xmlChar *pucArgGrep)
{
  xmlNodePtr pndResult = NULL;

  if (pndArg != NULL && STR_IS_NOT_EMPTY(pucArgGrep)) {
    pcre2_code *re_grep = NULL;
    size_t erroroffset = 0;
    int errornumber = 0;
    int opt_match_pcre = PCRE2_UTF | PCRE2_CASELESS;

    re_grep = pcre2_compile(
			    (PCRE2_SPTR8)pucArgGrep, /* the pattern */
			    PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
			    opt_match_pcre,         /* default options */
			    &errornumber,          /* for error number */
			    &erroroffset,          /* for error offset */
			    NULL);                 /* use default compile context */

    if (re_grep != NULL) {
    
      pndResult = xmlNewNode(NULL,NAME_GREP);
      if (pndResult != NULL) {
	xmlSetProp(pndResult, BAD_CAST ((opt_match_pcre & PCRE2_CASELESS) ? "imatch" : "match"), pucArgGrep);
	if (domGrepRegExpInTree(pndResult,pndArg,re_grep)) {
	}
	else {
	  xmlFreeNode(pndResult);
	  pndResult = NULL;
	}
      }
      pcre2_code_free(re_grep);
    }
    else {
      printf("Error 1\n");
    }
  }
  return pndResult;
} /* End of domNodeGrepNew() */

#endif


/*! change the URL of DOM pdocArg to URI of pccArg

\param pdocArg pointer to DOM
\param pccArg the filesystem context

 */
void
domChangeURL(xmlDocPtr pdocArg, resNodePtr prnArg)
{
#ifdef DEBUG
  PrintFormatLog(3,"domChangeURL(pdocArg=%0x,prnArg=%0x) to '%s'",pdocArg,prnArg,resNodeGetURI(prnArg));
#endif

  if (pdocArg != NULL && prnArg != NULL) {
    xmlChar *pucUri;

    pucUri = resNodeGetURI(prnArg);
    if (pucUri) {
      xmlFree((void *) pdocArg->URL);
      pdocArg->URL = xmlStrdup(pucUri);
    }
  }
}
/* end of domChangeURL() */



#ifdef TESTCODE
#include "test/test_dom.c"
#endif
