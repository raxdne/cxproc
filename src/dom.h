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

#include <libxml/xpath.h>

#include <res_node/res_node.h>

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

extern xmlNodePtr
domGetFollowingNode(xmlNodePtr pndArg, xmlChar *pucName);

extern xmlXPathObjectPtr
domGetXPathNodeset(xmlDocPtr pdocArg, xmlChar *pucArg);

extern xmlDocPtr
domGetXPathDoc(xmlDocPtr pdocArg, xmlChar *pucArg);

extern BOOL_T
domWeightXPathInDoc(xmlDocPtr pdocArg, xmlChar *pucArg);

extern void
domRemoveFileLocator(xmlNodePtr pndArg);

extern void
domAddFileLocator(xmlNodePtr pndArg, xmlChar *pucArg);

extern void
domAddFileXpath(xmlNodePtr pndArg, xmlChar *pucArgName, xmlChar *pucArgPrefix);

extern xmlChar *
domNodeGetXpathStr(xmlNodePtr pndArg);

extern int
domPutNodeString(FILE *out, xmlChar *pucArgMessage, xmlNodePtr pndArg);

extern int
domPutDocString(FILE *out, xmlChar *pucArgMessage, xmlDocPtr pdocArg);

#ifdef DEBUG
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
domGetAttributePtr(xmlNodePtr pndArg, xmlChar *pucNameAttr);

extern void
domRemoveAttributes(xmlNodePtr pndArg);

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

extern xmlAttrPtr
domGetAttributeNode(xmlNodePtr pndArg, xmlChar *pucNameAttr);

extern BOOL_T
domGetAttributeFlag(xmlNodePtr pndArg, xmlChar *pucNameAttr, BOOL_T fDefault);

extern BOOL_T
domGetAttributeEqual(xmlNodePtr pndArg, xmlChar *pucNameAttr, xmlChar *pucValueAttr);

extern xmlNodePtr
domGetNextNode(xmlNodePtr pndArg, xmlChar *pucNameElement);

extern xmlNsPtr
domGetNsXsl(void);

extern BOOL_T
domSetNsRecursive(xmlNodePtr pndArg, xmlNsPtr ns);

extern void
domUnsetNs(xmlNodePtr pndArg);

extern BOOL_T
_domTransferNsTo(xmlNodePtr pndArg, xmlDocPtr pdocArg);

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

extern void
domIncrPropRecursive(xmlNodePtr pndArg, xmlChar *pucArg, int iArg);

extern xmlChar *
domGetXslOutputMethod(xmlDocPtr pdocArg);

#ifdef DEBUG

extern BOOL_T
domIsNodeInTree(xmlNodePtr pndArgHaystack, xmlNodePtr pndArgNeedle);

extern BOOL_T
domIsTreeOverlapping(xmlNodePtr pndArgA, xmlNodePtr pndArgB);

#endif

extern xmlNodePtr
domValidateTree(xmlNodePtr pndArg);

extern xmlNodePtr
domAddNodeToError(xmlDocPtr pdocArg, xmlNodePtr pndArg);

extern void
domChangeURL(xmlDocPtr pdocArg, resNodePtr pccArg);

extern BOOL_T
domDocIsHtml(xmlDocPtr pdocArg);

extern BOOL_T
isEmptyTextNode(xmlNodePtr pndArg);

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
