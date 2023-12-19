/*
  cxproc - Configurable Xml PROCessor

  Copyright (C) 2006..2022 by Alexander Tenbusch

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

#include <libxml/xpath.h>

#include <res_node/res_node.h>

#define NAME_META BAD_CAST "meta"

#define NAME_ERROR BAD_CAST "error"

#define NAME_INFO BAD_CAST "info"

#define IS_NODE_META(NODE) (IS_NODE(NODE,NAME_META))

#define IS_NODE_ERROR(NODE) (IS_NODE(NODE,NAME_ERROR))

#define IS_NODE_INFO(NODE) (IS_NODE(NODE,NAME_INFO))

#define IS_NODE__XSL(NODE,NAME) (NODE != NULL && NODE->type == XML_ELEMENT_NODE && NODE->ns != NULL && NODE->ns->prefix != NULL && xmlStrEqual(NODE->ns->prefix,BAD_CAST "xsl") && NODE->name != NULL && (NAME==NULL || xmlStrEqual(NODE->name,BAD_CAST NAME)))

#define IS_NODE_XSL_STYLESHEET(NODE) (IS_NODE__XSL(NODE,"stylesheet"))

#define IS_NODE_XSL_TEMPLATE(NODE) (IS_NODE__XSL(NODE,"template"))

#define IS_NODE_XSL_VARIABLE(NODE) (IS_NODE__XSL(NODE,"variable"))


extern void
domFreeNodeByName(xmlNodePtr pndArg, xmlChar* pucArgName);

extern xmlDocPtr
domDocFromNodeNew(xmlNodePtr pndArg);

extern RN_MIME_TYPE
domMimeType(xmlDocPtr pdocArg);

extern xmlXPathObjectPtr
domGetXPathNodeset(xmlDocPtr pdocArg, xmlChar *pucArg);

extern xmlDocPtr
domGetXPathDoc(xmlDocPtr pdocArg, xmlChar *pucArg);

extern void
domUnsetPropFileLocator(xmlNodePtr pndArg);

extern void
domSetPropFileLocator(xmlNodePtr pndArg, xmlChar *pucArg);

extern void
domSetPropFileXpath(xmlNodePtr pndArg, xmlChar *pucArgName, xmlChar *pucArgPrefix);

#ifdef DEBUG
extern int
domPutNodeString(FILE *out, xmlChar *pucArgMessage, xmlNodePtr pndArg);

extern int
domPutDocString(FILE *out, xmlChar *pucArgMessage, xmlDocPtr pdocArg);

extern BOOL_T
domPutNodeGraphvizString(char *pchNameFile, xmlNodePtr pndArg, int iArgDepth);

extern void
domPutNodeGraphvizStringRecursive(FILE *out, xmlNodePtr pndArg, int iArgDepth);
#endif

extern index_t 
domNumberOf(xmlNodePtr pndArg, xmlChar *pucNameElement, index_t n);

extern index_t
domNumberOfChild(xmlNodePtr pndArg, xmlChar *pucNameElement);

extern xmlNodePtr
domGetFirstChild(xmlNodePtr pndArg, xmlChar *pucNameElement);

extern xmlChar *
domGetPropValuePtr(xmlNodePtr pndArg, xmlChar *pucNameAttr);

extern void
domUnsetPropAll(xmlNodePtr pndArg);

extern xmlChar *
domNodeEatContent(xmlNodePtr pndArg);

extern xmlChar *
domNodeGetContentPtr(xmlNodePtr cur);

extern BOOL_T
domNodeIsDocRoot(xmlNodePtr pndArg);

extern BOOL_T
domNodeIsAttribute(xmlNodePtr pndArg);

extern BOOL_T
domNodeIsDescendant(xmlNodePtr pndArgTop, xmlNodePtr pndArg);

extern xmlChar*
domNodeListGetString(xmlNodePtr pndArg, xmlChar* pucArg);

extern BOOL_T
domGetPropFlag(xmlNodePtr pndArg, xmlChar *pucNameAttr, BOOL_T fDefault);

extern BOOL_T
domGetAncestorsPropFlag(xmlNodePtr pndArg, xmlChar *pucNameAttr, BOOL_T fDefault);

extern BOOL_T
domPropIsEqual(xmlNodePtr pndArg, xmlChar *pucNameAttr, xmlChar *pucValueAttr);

extern xmlAttrPtr
domCopyPropList(xmlNodePtr target, xmlNodePtr cur);

extern xmlNodePtr
domGetNextNode(xmlNodePtr pndArg, xmlChar *pucNameElement);

extern xmlNsPtr
domGetNsXsl(void);

extern BOOL_T
domSetNsRecursive(xmlNodePtr pndArg, xmlNsPtr ns);

extern void
domUnsetNs(xmlNodePtr pndArg);

extern BOOL_T
domNodesAreEqual(xmlNodePtr pndA, xmlNodePtr pndB);

extern xmlNodePtr
domReplaceNodeList(xmlNodePtr old, xmlNodePtr cur);

extern void
domUnlinkNodeList(xmlNodePtr cur);

extern xmlAttrPtr
domSetPropEat(xmlNodePtr pndArg, xmlChar *pucArg, xmlChar *pucValue);

extern void
domIncrProp(xmlNodePtr pndArg, xmlChar *pucArg, int iValue);

extern xmlChar *
domGetXslOutputMethod(xmlDocPtr pdocArg);

#ifdef DEBUG

extern BOOL_T
domIsNodeInTree(xmlNodePtr pndArgHaystack, xmlNodePtr pndArgNeedle);

extern BOOL_T
domIsTreeOverlapping(xmlNodePtr pndArgA, xmlNodePtr pndArgB);

#endif

extern BOOL_T
domNodeHasAncestor(xmlNodePtr pndArg, xmlChar* pucArg);

extern BOOL_T
domNodeHasChild(xmlNodePtr pndArg, xmlChar* pucArg);

extern xmlNodePtr
domValidateTree(xmlNodePtr pndArg);

extern xmlNodePtr
domAddNodeToError(xmlDocPtr pdocArg, xmlNodePtr pndArg);

extern BOOL_T
domDocIsHtml(xmlDocPtr pdocArg);

extern xmlNodePtr
domNodeGrepNew(xmlNodePtr pndArg, xmlChar *pucArgGrep);

extern BOOL_T
domGrepRegExpInTree(xmlNodePtr pndResultArg, xmlNodePtr pndArg, const pcre2_code *re_grep);

extern void
domCleanup(void);

#ifdef TESTCODE
extern int
domTest(void);
#endif
