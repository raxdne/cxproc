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
	pndResult = xmlNewChild(pndResult, NULL, NAME_PIE_SECTION, NULL);
      }

      if (pndResult) {
      }
      else {
	int i;
	pndResult = pndArgParent;
	for (i = 1; i < cmarkNodeGetDepth(pcmnArg); i++) {
	  /* create empty SECTION elements */
	  pndResult = xmlNewChild(pndResult, NULL, NAME_PIE_SECTION, NULL);
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
	xmlAddChild(pndArg, xmlNewComment(BAD_CAST pcmnArg->data));
      }
    }
    /* Block */
    else if (pcmnArg->type == CMARK_NODE_DOCUMENT) {
      pndResult = xmlNewChild(pndArg, NULL, NAME_PIE_BLOCK, NULL);
      SetTypeAttr(pndResult, RMODE_MD);
      for (pndT = pndResult,  pcmnIter = pcmnArg->first_child; pcmnIter; pcmnIter = pcmnIter->next) {
	pndT = cmarkTreeToDOM(pndResult, pndT, pcmnIter);
      }
    }
    else if (pcmnArg->type == CMARK_NODE_BLOCK_QUOTE) {
      pndT = xmlNewChild(pndArg, NULL, NAME_PIE_BLOCKQUOTE, NULL);
      xmlAddChild(pndT, xmlNewComment(BAD_CAST"TODO: CMARK_NODE_BLOCK_QUOTE"));
      for (pcmnIter = pcmnArg->first_child; pcmnIter; pcmnIter = pcmnIter->next) {
	cmarkTreeToDOM(pndArgBlock, pndT, pcmnIter);
      }
    }
    else if (pcmnArg->type == CMARK_NODE_LIST) {
      if (IS_NODE_PIE_PAR(pndArg->last)) {
	/* append this list to its preceeding paragraph */
	pndT = xmlNewChild(pndArg->last, NULL, NAME_PIE_LIST, NULL);
      }
      else {
	pndT = xmlNewChild(pndArg, NULL, NAME_PIE_LIST, NULL);
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
      pndT = xmlNewChild(pndArg, NULL, NAME_PIE_PRE, pcmnArg->data);
    }
    else if (pcmnArg->type == CMARK_NODE_HTML_BLOCK) {
      xmlNodePtr pndNew = NULL;

      if (StringBeginsWith(pcmnArg->data,BAD_CAST"<csv>")) {
	xmlChar *puc0;
	xmlChar *puc1 = NULL;

	pndNew = xmlNewChild(pndArg, NULL, NAME_PIE_IMPORT, NULL);
	xmlSetProp(pndNew, BAD_CAST "type", BAD_CAST"csv");
	
	if ((puc0 = xmlStrstr(pcmnArg->data,BAD_CAST"<csv>")) != NULL) {
	  puc0 += xmlStrlen(BAD_CAST"<csv>");
	  if ((puc1 = xmlStrstr(pcmnArg->data,BAD_CAST"</csv>")) != NULL) {
	    xmlChar *pucContent;

	    if (puc1 > puc0 && (pucContent = xmlStrndup(puc0, puc1 - puc0)) != NULL) {
	      xmlNodeSetContent(pndNew,pucContent);
	      xmlFree(pucContent);
	    }
	  }
	}
	/*!\todo handle <script> in a similar way */
      }
      else {
	xmlDocPtr pdocHtml;

	pucT = xmlStrdup(BAD_CAST"<html>");
	pucT = xmlStrcat(pucT, BAD_CAST pcmnArg->data);
	pucT = xmlStrcat(pucT, BAD_CAST"</html>");

	pdocHtml = xmlParseMemory(pucT,xmlStrlen(pucT));
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
      pndT = xmlNewChild(pndArg, NULL, NAME_PIE_PAR, NULL);
      for (pcmnIter = pcmnArg->first_child; pcmnIter; pcmnIter = pcmnIter->next) {
	cmarkTreeToDOM(pndArgBlock, pndT, pcmnIter);
      }
    }
    else if (pcmnArg->type == CMARK_NODE_HEADING) {
      pndArg = GetParentElement(pcmnArg, pndArgBlock);

      pndResult = xmlNewChild(pndArg, NULL, NAME_PIE_SECTION, NULL);
      if (pcmnArg->first_child) {
	pndT = xmlNewChild(pndResult, NULL, NAME_PIE_HEADER, NULL);
	for (pcmnIter = pcmnArg->first_child; pcmnIter; pcmnIter = pcmnIter->next) {
	  cmarkTreeToDOM(pndArgBlock, pndT, pcmnIter);
	}
      }
    }
    else if (pcmnArg->type == CMARK_NODE_THEMATIC_BREAK) {
      pndT = xmlNewChild(pndArg, NULL, NAME_PIE_RULER, NULL);
    }
    /* Inline */
    else if (pcmnArg->type == CMARK_NODE_TEXT) {
      if (STR_IS_NOT_EMPTY(pcmnArg->data)) {
	xmlAddChild(pndArg, xmlNewText(BAD_CAST pcmnArg->data));
      }
      else {
	xmlAddChild(pndArg, xmlNewComment(BAD_CAST"empty paragraph"));
      }
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
      pndT = xmlNewChild(pndArg, NULL, NAME_PIE_TT, pcmnArg->data);
    }
    else if (pcmnArg->type == CMARK_NODE_HTML_INLINE) {
    }
    else if (pcmnArg->type == CMARK_NODE_CUSTOM_INLINE) {
      xmlAddChild(pndArg, xmlNewComment(BAD_CAST"CMARK_NODE_CUSTOM_INLINE"));
    }
    else if (pcmnArg->type == CMARK_NODE_EMPH) {
      pndT = xmlNewChild(pndArg, NULL, NAME_PIE_EM, NULL);
      cmarkTreeToDOM(pndArgBlock, pndT, pcmnArg->first_child);
    }
    else if (pcmnArg->type == CMARK_NODE_STRONG) {
      pndT = xmlNewChild(pndArg, NULL, NAME_PIE_STRONG, NULL);
      cmarkTreeToDOM(pndArgBlock, pndT, pcmnArg->first_child);
    }
    else if (pcmnArg->type == CMARK_NODE_LINK) {
      pndT = xmlNewChild(pndArg, NULL, NAME_PIE_LINK, NULL);
      xmlSetProp(pndT,BAD_CAST"href", pcmnArg->as.link.url);
      cmarkTreeToDOM(pndT, pndT, pcmnArg->first_child);
    }
    else if (pcmnArg->type == CMARK_NODE_IMAGE) {
      pndT = xmlNewChild(pndArg, NULL, NAME_PIE_IMG, NULL);
      xmlSetProp(pndT,BAD_CAST"src", pcmnArg->as.link.url);
      cmarkTreeToDOM(pndT, pndT, pcmnArg->first_child);
    }
    else if (pcmnArg->type == CMARK_NODE_LAST_INLINE) {
      xmlAddChild(pndArg, xmlNewComment(BAD_CAST"CMARK_NODE_LAST_INLINE"));
    }
    else if (pcmnArg->type == CMARK_NODE_HRULE) {
      pndT = xmlNewChild(pndArg, NULL, NAME_PIE_RULER, NULL);
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

    if ((doc = cmark_parse_document(pucArg, strlen(pucArg), CMARK_OPT_DEFAULT | CMARK_OPT_SMART)) != NULL) {
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
