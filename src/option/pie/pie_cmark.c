/*
  cxproc - Configurable Xml PROCessor

  Copyright (C) 2006..2024 by Alexander Tenbusch

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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/*
 */
#include <libxml/tree.h>

 /*
  */
#include "basics.h"
#include <res_node/res_node_io.h>
#include <cxp/cxp.h>
#include "dom.h"
#include <cxp/cxp_dir.h>
#include <pie/pie_text_blocks.h>

#include "utils.h"

#include <config.h>
#include <cmark.h>
#include <node.h>


#define cmarkNodeGetBeginPtr(N) ((N != NULL) ? N->data : NULL)

#define cmarkNodeGetDepth(N) ((N != NULL) ? N->as.heading.level : 0)


#define cmarkNodeIsHeader(N) (N != NULL && N->type == CMARK_NODE_HEADER)

#define cmarkNodeIsEnum(N) (N != NULL && N->type == CMARK_NODE_ITEM)

#define cmarkNodeIsList(N) (N != NULL && N->type == CMARK_NODE_LIST)

#define cmarkNodeIsListItem(N) (N != NULL && N->type == CMARK_NODE_ITEM)

#define cmarkNodeIsRuler(N) (N != NULL && N->type == CMARK_NODE_HRULE)


static xmlNodePtr
cmarkTreeToDOM(xmlNodePtr pndArgBlock, xmlNodePtr pndArg, cmark_node* pcmnArg);

static xmlNodePtr
GetParentElement(cmark_node* pcmnArg, xmlNodePtr pndArgParent);


/*! \return the xmlNodePtr to an according section or block pcmnArg
* 
* similar to GetParentElement() in pie_text_blocks.c
*/
xmlNodePtr
GetParentElement(cmark_node* pcmnArg, xmlNodePtr pndArgParent)
{
  xmlNodePtr pndLast = NULL;
  xmlNodePtr pndResult = pndArgParent;

  if (pcmnArg != NULL && pndArgParent != NULL) {

    if (cmarkNodeIsHeader(pcmnArg)) {
      int iDepth;

      for (iDepth = 0, pndLast = pndArgParent; pndLast; pndLast = pndLast->last) {
	if (IS_NODE_PIE_PIE(pndLast)) {

	  if (IS_NODE_PIE_BLOCK(pndLast->last)) {
	    pndLast = pndLast->last;
	  }

	  if (cmarkNodeGetDepth(pcmnArg) == 1) { /* top section pcmnArg */
	    pndResult = pndLast;
	    break;
	  }
	}
	else if (IS_NODE_PIE_BLOCK(pndLast)) {
	  if (cmarkNodeGetDepth(pcmnArg) == 1) { /* top section pcmnArg */
	    pndResult = pndLast;
	    break;
	  }
	}
	else if (IS_NODE_PIE_SECTION(pndLast)) {
	  iDepth++;
	  pndResult = pndLast;
	  if (iDepth == cmarkNodeGetDepth(pcmnArg) - 1) {
	    /* section pcmnArg with parent depth found */
	    break;
	  }
	}
	else {
	  /* last pcmnArg is not a section */
	  break;
	}
      }

      for (; iDepth < (cmarkNodeGetDepth(pcmnArg) - 1); iDepth++) {
	/* create empty SECTION elements */
	pndResult = xmlNewChild(pndResult, NULL, BAD_CAST NAME_PIE_SECTION, NULL);
      }

      if (pndResult) {
      }
      else {
	int i;
	pndResult = pndArgParent;
	for (i = 1; i < cmarkNodeGetDepth(pcmnArg); i++) {
	  /* create empty SECTION elements */
	  pndResult = xmlNewChild(pndResult, NULL, BAD_CAST NAME_PIE_SECTION, NULL);
	}
      }
    }
    else {
      /* find very last node of a pie/block */
      for (pndResult = pndLast = pndArgParent; pndLast; pndLast = pndLast->last) {
	if (IS_NODE_PIE_PIE(pndLast)) {
	  if (IS_NODE_PIE_BLOCK(pndLast->last)) {
	    pndLast = pndLast->last;
	  }
	  pndResult = pndLast;
	}
	else if (IS_NODE_PIE_BLOCK(pndLast)) {
	  pndResult = pndLast;
	}
	else if (IS_NODE_PIE_SECTION(pndLast)) {
	  pndResult = pndLast;
	}
	else {
	  break;
	}
      }
    }
  }
  return pndResult;
} /* end of GetParentElement() */


/*! makes elements content XML-conformant and
  \param pcmnArg element to use
  \return a new node pointer or NULL if failed
*/
xmlNodePtr
cmarkTreeToDOM(xmlNodePtr pndArgBlock, xmlNodePtr pndArg, cmark_node* pcmnArg)
{
  xmlNodePtr pndResult = pndArg;

  if (pcmnArg) {
    xmlChar* pucT;
    xmlNodePtr pndT = NULL;
    cmark_node* pcmnIter;

    /* Error status */
    if (pcmnArg->type == CMARK_NODE_NONE) {
      if (STR_IS_NOT_EMPTY(pcmnArg->data)) {
      pucT = StringEncodeXmlDefaultEntitiesNew(BAD_CAST pcmnArg->data);
      xmlAddChild(pndArg, xmlNewComment(pucT));
      xmlFree(pucT);
      }
    }
    /* Block */
    else if (pcmnArg->type == CMARK_NODE_DOCUMENT) {
      pndResult = xmlNewChild(pndArg, NULL, BAD_CAST NAME_PIE_BLOCK, NULL);
      SetTypeAttr(pndResult, RMODE_MD);
      for (pndT = pndResult,  pcmnIter = pcmnArg->first_child; pcmnIter; pcmnIter = pcmnIter->next) {
	pndT = cmarkTreeToDOM(pndResult, pndT, pcmnIter);
      }
    }
    else if (pcmnArg->type == CMARK_NODE_BLOCK_QUOTE) {
      pndT = xmlNewChild(pndArg, NULL, BAD_CAST NAME_PIE_BLOCKQUOTE, NULL);
      xmlAddChild(pndT, xmlNewComment(BAD_CAST"TODO: CMARK_NODE_BLOCK_QUOTE"));
      for (pcmnIter = pcmnArg->first_child; pcmnIter; pcmnIter = pcmnIter->next) {
	cmarkTreeToDOM(pndArgBlock, pndT, pcmnIter);
      }
    }
    else if (pcmnArg->type == CMARK_NODE_LIST) {
      if (IS_NODE_PIE_PAR(pndArg->last)) {
	/* append this list to its preceeding paragraph */
	pndT = xmlNewChild(pndArg->last, NULL, BAD_CAST NAME_PIE_LIST, NULL);
      }
      else {
	pndT = xmlNewChild(pndArg, NULL, BAD_CAST NAME_PIE_LIST, NULL);
      }

      if (pcmnArg->as.list.list_type == 2) {
	xmlSetProp(pndT, BAD_CAST "enum", BAD_CAST"yes");
      }
      
      for (pcmnIter = pcmnArg->first_child; pcmnIter; pcmnIter = pcmnIter->next) {
	cmarkTreeToDOM(pndT, pndT, pcmnIter);
      }
    }
    else if (pcmnArg->type == CMARK_NODE_ITEM) {
      for (pcmnIter = pcmnArg->first_child; pcmnIter; pcmnIter = pcmnIter->next) {
	cmarkTreeToDOM(pndArgBlock, pndArg, pcmnIter);
      }
    }
    else if (pcmnArg->type == CMARK_NODE_CODE_BLOCK) {
      pucT = StringEncodeXmlDefaultEntitiesNew(BAD_CAST pcmnArg->data);
      pndT = xmlNewChild(pndArg, NULL, BAD_CAST NAME_PIE_PRE, pucT);
      xmlFree(pucT);
    }
    else if (pcmnArg->type == CMARK_NODE_HTML_BLOCK) {
      xmlNodePtr pndNew = NULL;

      /*!\bug block is interrupted by empty lines */

      if (StringBeginsWith((char *)pcmnArg->data, "<" NAME_PIE_CSV ">")) {
	xmlChar *puc0;
	xmlChar *puc1 = NULL;

	pndNew = xmlNewChild(pndArg, NULL, BAD_CAST NAME_PIE_IMPORT, NULL);
	xmlSetProp(pndNew, BAD_CAST "type", BAD_CAST NAME_PIE_CSV);
	
	if ((puc0 = BAD_CAST xmlStrstr(BAD_CAST pcmnArg->data, BAD_CAST "<" NAME_PIE_CSV ">")) != NULL) {
	  puc0 += xmlStrlen(BAD_CAST "<" NAME_PIE_CSV ">");
	  while (isspace(*puc0)) { puc0++; }
	  if ((puc1 = BAD_CAST xmlStrstr(BAD_CAST pcmnArg->data, BAD_CAST "</" NAME_PIE_CSV ">")) != NULL) {
	    xmlChar *pucContent;

	    if (puc1 > puc0 && (pucContent = xmlStrndup(puc0, (int)(puc1 - puc0))) != NULL) {
	      xmlAddChild(pndNew, xmlNewText(pucContent));
	      xmlFree(pucContent);
	    }
	  }
	}
      }
      else if (StringBeginsWith((char *)pcmnArg->data, "<" NAME_PIE_SCRIPT ">")) {
	xmlChar *puc0;
	xmlChar *puc1 = NULL;

	pndNew = xmlNewChild(pndArg, NULL, BAD_CAST NAME_PIE_IMPORT, NULL);
	xmlSetProp(pndNew, BAD_CAST "type", BAD_CAST NAME_PIE_SCRIPT);
	
	if ((puc0 = BAD_CAST xmlStrstr(BAD_CAST pcmnArg->data, BAD_CAST "<" NAME_PIE_SCRIPT ">")) != NULL) {
	  puc0 += xmlStrlen(BAD_CAST "<" NAME_PIE_SCRIPT ">");
	  if ((puc1 = BAD_CAST xmlStrstr(BAD_CAST pcmnArg->data, BAD_CAST "</" NAME_PIE_SCRIPT ">")) != NULL) {
	    xmlChar *pucContent;

	    if (puc1 > puc0 && (pucContent = xmlStrndup(puc0, (int)(puc1 - puc0))) != NULL) {
	      pucT = StringEncodeXmlDefaultEntitiesNew(pucContent);
	      xmlNodeSetContent(pndNew, pucT);
	      //xmlAddChild(pndArg, xmlNewComment(pucT));
	      xmlFree(pucT);
	      xmlFree(pucContent);
	    }
	  }
	}
      }
      else {
	xmlDocPtr pdocHtml;

	pdocHtml = xmlParseMemory((const char *)pcmnArg->data, xmlStrlen(BAD_CAST pcmnArg->data)); /* XHTML only */
	if ((pndNew = xmlDocGetRootElement(pdocHtml)) != NULL && IS_NODE_PIE_HTML(pndNew) && pndNew->children != NULL) {
	  xmlUnlinkNode(pndNew);
	  xmlNodeSetName(pndNew, BAD_CAST"block");
	  xmlSetProp(pndNew, BAD_CAST "type", BAD_CAST"text/html");
	  xmlAddChild(pndArg, pndNew);
	}
	else {
	  xmlAddChild(pndArg, xmlNewComment(BAD_CAST"HTML parser error"));
	}
	xmlFreeDoc(pdocHtml);
      }
    }
    else if (pcmnArg->type == CMARK_NODE_CUSTOM_BLOCK) {
      xmlAddChild(pndArg, xmlNewComment(BAD_CAST"CMARK_NODE_CUSTOM_BLOCK"));
    }
    else if (pcmnArg->type == CMARK_NODE_PARAGRAPH) {
      RN_MIME_TYPE t;

      if (pcmnArg->first_child != NULL && pcmnArg->first_child == pcmnArg->last_child && pcmnArg->first_child->data != NULL && resMimeIsPicture((t = resMimeGetTypeFromDataBase64(pcmnArg->first_child->data)))) {
	xmlChar *pucTT;
	xmlNodePtr pndImage;

	pndImage = xmlNewChild(pndArg, NULL, BAD_CAST NAME_PIE_IMG, NULL);
	xmlSetProp(pndImage, BAD_CAST "type", BAD_CAST resMimeGetTypeStr(t));

	if ((pucTT = BAD_CAST xmlStrchr(pcmnArg->first_child->data, ',')) != NULL && pucTT++) {
	  xmlNewChild(pndImage, NULL, BAD_CAST NAME_BASE64, pucTT);
	}
      }
      else {
        pndT = xmlNewChild(pndArg, NULL, BAD_CAST NAME_PIE_PAR, NULL);
        for (pcmnIter = pcmnArg->first_child; pcmnIter; pcmnIter = pcmnIter->next) {
	  cmarkTreeToDOM(pndArgBlock, pndT, pcmnIter);
        }
      }
    }
    else if (pcmnArg->type == CMARK_NODE_HEADING) {
      pndArg = GetParentElement(pcmnArg, pndArgBlock);

      pndResult = xmlNewChild(pndArg, NULL, BAD_CAST NAME_PIE_SECTION, NULL);
      if (pcmnArg->first_child) {
	pndT = xmlNewChild(pndResult, NULL, BAD_CAST NAME_PIE_HEADER, NULL);
	for (pcmnIter = pcmnArg->first_child; pcmnIter; pcmnIter = pcmnIter->next) {
	  cmarkTreeToDOM(pndArgBlock, pndT, pcmnIter);
	}
      }
    }
    else if (pcmnArg->type == CMARK_NODE_THEMATIC_BREAK) {
      pndT = xmlNewChild(pndArg, NULL, BAD_CAST NAME_PIE_RULER, NULL);
    }
    /* Inline */
    else if (pcmnArg->type == CMARK_NODE_TEXT) {
      if (STR_IS_NOT_EMPTY(pcmnArg->data)) {
	if (IS_NODE_PIE_PAR(pndArg) && xmlNodeIsText(pndArg->last)) {
	  xmlAddChild(pndArg, xmlNewText(BAD_CAST " ")); /* concatenation of two text lines */
	}
	xmlAddChild(pndArg, xmlNewText(BAD_CAST pcmnArg->data));
      }
#ifdef DEBUG
      else {
	xmlAddChild(pndArg, xmlNewComment(BAD_CAST"empty paragraph"));
      }
#endif
    }
    else if (pcmnArg->type == CMARK_NODE_FIRST_INLINE) {
      cmarkTreeToDOM(pndArg, pndArg, pcmnArg->first_child);
    }
    else if (pcmnArg->type == CMARK_NODE_SOFTBREAK) {
      cmarkTreeToDOM(pndArg, pndArg, pcmnArg->first_child);
    }
    else if (pcmnArg->type == CMARK_NODE_LINEBREAK) {
      cmarkTreeToDOM(pndArg, pndArg, pcmnArg->first_child);
    }
    else if (pcmnArg->type == CMARK_NODE_CODE) {
      pucT = StringEncodeXmlDefaultEntitiesNew(BAD_CAST pcmnArg->data);
      pndT = xmlNewChild(pndArg, NULL, BAD_CAST NAME_PIE_TT, pucT);
      xmlFree(pucT);
    }
    else if (pcmnArg->type == CMARK_NODE_HTML_INLINE) {
    }
    else if (pcmnArg->type == CMARK_NODE_CUSTOM_INLINE) {
      xmlAddChild(pndArg, xmlNewComment(BAD_CAST"CMARK_NODE_CUSTOM_INLINE"));
    }
    else if (pcmnArg->type == CMARK_NODE_EMPH) {
      pndT = xmlNewChild(pndArg, NULL, BAD_CAST NAME_PIE_EM, NULL);
      cmarkTreeToDOM(pndArgBlock, pndT, pcmnArg->first_child);
    }
    else if (pcmnArg->type == CMARK_NODE_STRONG) {
      pndT = xmlNewChild(pndArg, NULL, BAD_CAST NAME_PIE_STRONG, NULL);
      cmarkTreeToDOM(pndArgBlock, pndT, pcmnArg->first_child);
    }
    else if (pcmnArg->type == CMARK_NODE_LINK) {
      pndT = xmlNewChild(pndArg, NULL, BAD_CAST NAME_PIE_LINK, NULL);
      if (pcmnArg->first_child != NULL && pcmnArg->first_child == pcmnArg->last_child) {
	if (StringBeginsWith((char *) pcmnArg->first_child->data, "mailto:")) {
	  xmlAddChild(pndT, xmlNewText(BAD_CAST & (pcmnArg->first_child->data[7])));
	  xmlSetProp(pndT, BAD_CAST "href", pcmnArg->as.link.url);
	}
	else if (xmlStrEqual(pcmnArg->first_child->data, pcmnArg->as.link.url)) {
	  /* attribute href not required if it's same like display value */
	  xmlAddChild(pndT, xmlNewText(BAD_CAST pcmnArg->first_child->data));
	}
	else {
	  xmlAddChild(pndT, xmlNewText(BAD_CAST pcmnArg->first_child->data));
	  xmlSetProp(pndT, BAD_CAST "href", pcmnArg->as.link.url);
	}
      }
    }
    else if (pcmnArg->type == CMARK_NODE_IMAGE) {
      RN_MIME_TYPE t;
      xmlNodePtr pndImage;

      pndImage = xmlNewChild(pndArg, NULL, BAD_CAST NAME_PIE_IMG, NULL);

      t = resMimeGetTypeFromDataBase64(pcmnArg->as.link.url);
      if (resMimeIsPicture(t)) {
	xmlChar *pucTT;

	xmlSetProp(pndImage, BAD_CAST "type", BAD_CAST resMimeGetTypeStr(t));

	if ((pucTT = BAD_CAST xmlStrchr(pcmnArg->as.link.url, ',')) != NULL && pucTT++) {
	  xmlNewChild(pndImage, NULL, BAD_CAST NAME_BASE64, pucTT);
	}
      }
      else {
	xmlSetProp(pndImage, BAD_CAST "src", pcmnArg->as.link.url);
      }

      if (pcmnArg->first_child != NULL && STR_IS_NOT_EMPTY(pcmnArg->first_child->data)) {
	xmlSetProp(pndImage, BAD_CAST "alt", pcmnArg->first_child->data);
      }

      if (pcmnArg->next == NULL && pcmnArg->prev == NULL && !IS_NODE_PIE_LIST(pndArg->parent)) {
	/* its an image in a single paragraph */
	xmlNodeSetName(pndArg, BAD_CAST NAME_PIE_FIG);
	xmlAddChild(pndArg, pndImage);

	if (pcmnArg->first_child != NULL && STR_IS_NOT_EMPTY(pcmnArg->first_child->data)) {
	  pndT = xmlNewChild(pndArg, NULL, BAD_CAST NAME_PIE_HEADER, pcmnArg->first_child->data);
	}

	//cmarkTreeToDOM(pndArg, pndImage, pcmnArg->first_child);
	xmlNewTextChild(pndArg, NULL, BAD_CAST NAME_PIE_TTAG, BAD_CAST "#fig");
      }
      else {
	/* the image is inline (prefix and/or postfix text) */
	xmlAddChild(pndArg, pndImage);
      }
    }
    else if (pcmnArg->type == CMARK_NODE_LAST_INLINE) {
      xmlAddChild(pndArg, xmlNewComment(BAD_CAST"CMARK_NODE_LAST_INLINE"));
    }
    else if (pcmnArg->type == CMARK_NODE_HRULE) {
      pndT = xmlNewChild(pndArg, NULL, BAD_CAST NAME_PIE_RULER, NULL);
    }
    else {
      /* recursion without any footprint of pcmnArg itself */
      cmarkTreeToDOM(pndArgBlock, pndArg, pcmnArg->first_child);
    }
  }
  return pndResult;
} /* end of cmarkTreeToDOM() */


/*! Append the parsed plain text to the given pndArgTop

\param pndArgTop parent pcmnArg to append import result nodes OR NULL if pndArgImport must be replaced by result
\param pucArg pointer to an UTF-8 encoded buffer (not XML-conformant!)

\return pointer to result pcmnArg "block" or NULL in case of errors

similar to ParsePlainBuffer()

*/
xmlNodePtr
ParseMarkdownBuffer(xmlNodePtr pndArgTop, xmlChar* pucArg)
{
  xmlNodePtr pndResult = NULL;

  if (STR_IS_NOT_EMPTY(pucArg)) {
    cmark_node* doc;

    if ((doc = cmark_parse_document((const char *)pucArg, strlen((const char *)pucArg), CMARK_OPT_DEFAULT)) != NULL) {
      cmarkTreeToDOM(pndArgTop, pndArgTop, doc);
      cmark_node_free(doc);
      pndResult = pndArgTop;
    }
  }
  return pndResult;
} /* end of ParseMarkdownBuffer() */


#ifdef TESTCODE
#include "test/test_pie_cmark.c"
#endif
