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

#define pieElementGetBeginPtr(N) ((N != NULL) ? N->data : NULL)

#define pieElementGetDepth(N) ((N != NULL) ? N->as.heading.level : 0)


#define pieElementIsHeader(N) (N != NULL && N->type == CMARK_NODE_HEADER)

#define pieElementIsEnum(N) (N != NULL && N->type == CMARK_NODE_ITEM)

#define pieElementIsList(N) (N != NULL && N->type == CMARK_NODE_LIST)

#define pieElementIsListItem(N) (N != NULL && N->type == CMARK_NODE_ITEM)

#define pieElementIsRuler(N) (N != NULL && N->type == CMARK_NODE_HRULE)

#define pieElementIsPre(N) (N != NULL && N->type == CMARK_NODE_CODE_BLOCK)



static xmlNodePtr
GetParentElement(cmark_node* pcmnArg, xmlNodePtr pndArgParent);

#if 0
unsigned char cmark_strbuf__initbuf[1];

struct render_state {
  cmark_strbuf* xml;
  int indent;
};


/*! derived from "cmark/src/xml.c" */

#define BUFFER_SIZE 100
#define MAX_INDENT 40

// Functions to convert cmark_nodes to XML strings.

// C0 control characters, U+FFFE and U+FFF aren't allowed in XML.
static const char XML_ESCAPE_TABLE[256] = {
    /* 0x00 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1,
    /* 0x10 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /* 0x20 */ 0, 0, 2, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 0x30 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 5, 0,
    /* 0x40 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 0x50 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 0x60 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 0x70 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 0x80 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 0x90 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xA0 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xB0 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 9,
    /* 0xC0 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xD0 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xE0 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xF0 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

// U+FFFD Replacement Character encoded in UTF-8
#define UTF8_REPL "\xEF\xBF\xBD"

static const char *XML_ESCAPES[] = {
  "", UTF8_REPL, "&quot;", "&amp;", "&lt;", "&gt;"
};

static void escape_xml(cmark_strbuf *ob, const unsigned char *src,
                       bufsize_t size) {
  bufsize_t i = 0, org, esc = 0;

  while (i < size) {
    org = i;
    while (i < size && (esc = XML_ESCAPE_TABLE[src[i]]) == 0)
      i++;

    if (i > org)
      cmark_strbuf_put(ob, src + org, i - org);

    if (i >= size)
      break;

    if (esc == 9) {
      // To replace U+FFFE and U+FFFF with U+FFFD, only the last byte has to
      // be changed.
      // We know that src[i] is 0xBE or 0xBF.
      if (i >= 2 && src[i-2] == 0xEF && src[i-1] == 0xBF) {
        cmark_strbuf_putc(ob, 0xBD);
      } else {
        cmark_strbuf_putc(ob, src[i]);
      }
    } else {
      cmark_strbuf_puts(ob, XML_ESCAPES[esc]);
    }

    i++;
  }
}

static void escape_xml_str(cmark_strbuf *dest, const unsigned char *source) {
  if (source)
    escape_xml(dest, source, strlen((char *)source));
}

static CMARK_INLINE void indent(struct render_state *state) {
  int i;
  for (i = 0; i < state->indent && i < MAX_INDENT; i++) {
    cmark_strbuf_putc(state->xml, ' ');
  }
}

#endif


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

    if (pieElementIsHeader(pcmnArg)) {
      /* no new pointer needed */
      int iDepth;

      for (iDepth = 0, pndLast = pndArgParent; pndLast; pndLast = pndLast->last) {
	if (IS_NODE_PIE_PIE(pndLast)) {

	  if (IS_NODE_PIE_BLOCK(pndLast->last)) {
	    pndLast = pndLast->last;
	  }

	  if (pieElementGetDepth(pcmnArg) == 1) { /* top section pcmnArg */
	    pndResult = pndLast;
	    break;
	  }
	}
	else if (IS_NODE_PIE_BLOCK(pndLast)) {
	  if (pieElementGetDepth(pcmnArg) == 1) { /* top section pcmnArg */
	    pndResult = pndLast;
	    break;
	  }
	}
	else if (IS_NODE_PIE_SECTION(pndLast)) {
	  iDepth++;
	  pndResult = pndLast;
	  if (iDepth == pieElementGetDepth(pcmnArg) - 1) {
	    /* section pcmnArg with parent depth found */
	    break;
	  }
	}
	else {
	  /* last pcmnArg is not a section */
	  break;
	}
      }

      for (; iDepth < (pieElementGetDepth(pcmnArg) - 1); iDepth++) {
	/* create empty SECTION elements */
	pndResult = xmlNewChild(pndResult, NULL, NAME_PIE_SECTION, NULL);
      }

      if (pndResult) {
      }
      else {
	int i;
	pndResult = pndArgParent;
	for (i = 1; i < pieElementGetDepth(pcmnArg); i++) {
	  /* create empty SECTION elements */
	  pndResult = xmlNewChild(pndResult, NULL, NAME_PIE_SECTION, NULL);
	}
      }
    }
#if 0
    else if (pieElementIsListItem(pcmnArg)) {
      int iDepth;
      xmlNodePtr pndT;
      xmlNodePtr pndList;

      for (iDepth = 0, pndList = NULL, pndLast = pndT = pndArgParent;
	IS_ENODE(pndT) && iDepth < pieElementGetDepth(pcmnArg);
	pndT = pndT->last) {

	if (IS_NODE_PIE_LIST(pndT)) {
	  pndLast = pndList = pndT;
	  iDepth++;
	}
	else if (IS_NODE_PIE_PAR(pndT)) {
	  pndLast = pndList = pndT;
	}
	else {
	  pndLast = pndT;
	}
      }

      for (; iDepth < pieElementGetDepth(pcmnArg); iDepth++) {
	if (pndList) {
	  /*
	  last element is a list, create a child list element
	  */
	  pndList = xmlNewChild(pndList, NULL, NAME_PIE_LIST, NULL);
	}
	else if (pndLast == pndArgParent) {
	  /*
	  last element is parent element, create a child list element
	  */
	  pndList = xmlNewChild(pndArgParent, NULL, NAME_PIE_LIST, NULL);
	}
	else {
	  /*
	  last element is not a list, create a sibling list element
	  */
	  assert(pndLast != NULL);

	  pndList = xmlNewNode(NULL, NAME_PIE_LIST);

	  if (IS_NODE_PIE_PAR(pndLast)) {
	    xmlAddChild(pndLast, pndList);
	  }
	  else {
	    xmlAddNextSibling(pndLast, pndList);
	  }
	}
      }

      if (pieElementIsEnum(pcmnArg)) {
	xmlSetProp(pndList, BAD_CAST "enum", BAD_CAST "yes");
      }
      pndResult = pndList;
    }
#endif
    else {

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
    xmlChar* pucC;
    xmlNodePtr pndT = NULL;
    cmark_node* pcmnI;

    pucC = pieElementGetBeginPtr(pcmnArg); /* shortcut */

    /* Error status */
    if (pcmnArg->type == CMARK_NODE_NONE) {
      xmlAddChild(pndArg, xmlNewComment(pucC));
    }
    /* Block */
    else if (pcmnArg->type == CMARK_NODE_DOCUMENT) {
      pndResult = xmlNewChild(pndArg, NULL, NAME_PIE_BLOCK, NULL);
      SetTypeAttr(pndResult, RMODE_MD);
      for (pndT = pndResult,  pcmnI = pcmnArg->first_child; pcmnI; pcmnI = pcmnI->next) {
	pndT = cmarkTreeToDOM(pndArgBlock, pndT, pcmnI);
      }
    }
    else if (pcmnArg->type == CMARK_NODE_FIRST_BLOCK) {
      xmlAddChild(pndArg, xmlNewComment(BAD_CAST"CMARK_NODE_FIRST_BLOCK"));
    }
    else if (pcmnArg->type == CMARK_NODE_BLOCK_QUOTE) {
      pndT = xmlNewChild(pndArg, NULL, NAME_PIE_PRE, NULL);
      for (pcmnI = pcmnArg->first_child; pcmnI; pcmnI = pcmnI->next) {
	cmarkTreeToDOM(pndArgBlock, pndT, pcmnI);
      }
    }
    else if (pcmnArg->type == CMARK_NODE_LIST) {
      pndT = xmlNewChild(pndArg, NULL, NAME_PIE_LIST, NULL);
      for (pcmnI = pcmnArg->first_child; pcmnI; pcmnI = pcmnI->next) {
	cmarkTreeToDOM(pndArgBlock, pndT, pcmnI);
      }
    }
    else if (pcmnArg->type == CMARK_NODE_ITEM) {
      for (pcmnI = pcmnArg->first_child; pcmnI; pcmnI = pcmnI->next) {
	cmarkTreeToDOM(pndArgBlock, pndArg, pcmnI);
      }
    }
    else if (pcmnArg->type == CMARK_NODE_CODE_BLOCK) {
      pndT = xmlNewChild(pndArg, NULL, NAME_PIE_PRE, pcmnArg->data);
    }
    else if (pcmnArg->type == CMARK_NODE_HTML_BLOCK) {
      xmlAddChild(pndArg, xmlNewComment(BAD_CAST"CMARK_NODE_HTML_BLOCK"));
    }
    else if (pcmnArg->type == CMARK_NODE_CUSTOM_BLOCK) {
      xmlAddChild(pndArg, xmlNewComment(BAD_CAST"CMARK_NODE_CUSTOM_BLOCK"));
    }
    else if (pcmnArg->type == CMARK_NODE_PARAGRAPH) {
      pndT = xmlNewChild(pndArg, NULL, NAME_PIE_PAR, NULL);
      for (pcmnI = pcmnArg->first_child; pcmnI; pcmnI = pcmnI->next) {
	cmarkTreeToDOM(pndArgBlock, pndT, pcmnI);
      }
    }
    else if (pcmnArg->type == CMARK_NODE_HEADING) {
      pndArg = GetParentElement(pcmnArg, pndArgBlock);

      pndResult = xmlNewChild(pndArg, NULL, NAME_PIE_SECTION, NULL);
      if (pcmnArg->first_child) {
	pndT = xmlNewChild(pndResult, NULL, NAME_PIE_HEADER, NULL);
	for (pcmnI = pcmnArg->first_child; pcmnI; pcmnI = pcmnI->next) {
	  cmarkTreeToDOM(pndArgBlock, pndT, pcmnI);
	}
      }
    }
    else if (pcmnArg->type == CMARK_NODE_THEMATIC_BREAK || pcmnArg->type == CMARK_NODE_LAST_BLOCK) {
    }
    /* Inline */
    else if (pcmnArg->type == CMARK_NODE_TEXT || pcmnArg->type == CMARK_NODE_FIRST_INLINE) {
      if (STR_IS_NOT_EMPTY(pucC)) {
	xmlAddChild(pndArg, xmlNewText(pucC));
      }
    }
    else if (pcmnArg->type == CMARK_NODE_SOFTBREAK) {
    }
    else if (pcmnArg->type == CMARK_NODE_LINEBREAK) {
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
      xmlAddChild(pndArg, xmlNewComment(BAD_CAST"CMARK_NODE_IMAGE"));
    }
    else if (pcmnArg->type == CMARK_NODE_LAST_INLINE) {
      xmlAddChild(pndArg, xmlNewComment(BAD_CAST"CMARK_NODE_LAST_INLINE"));
    }
    else {
      //cmarkTreeToDOM(pndArgBlock, pndResult, pcmnArg->first_child);
    }
    //cmarkTreeToDOM(pndArgBlock, pndArg, pcmnArg->next);
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

  CompileRegExpDefaults();

  if (STR_IS_NOT_EMPTY(pucArg)) {
    cmark_node* doc;

    doc = cmark_parse_document(pucArg, strlen(pucArg), CMARK_OPT_DEFAULT | CMARK_OPT_SMART);

    cmarkTreeToDOM(pndArgTop, pndArgTop, doc);

    cmark_node_free(doc);

    pndResult = pndArgTop;
  }

  return pndResult;
} /* end of ParseMarkdownBuffer() */


#ifdef TESTCODE
#include "test/test_pie_cmark.c"
#endif
