
/*! derived from "cmark/src/xml.c" */

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
#include <pie/pie_text.h>
#include "utils.h"

#include <config.h>
#include <cmark.h>
#include <node.h>

unsigned char cmark_strbuf__initbuf[1];

#if 0
struct render_state {
  cmark_strbuf* xml;
  int indent;
};



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

/*! \return the xmlNodePtr to an according section or block node

similar to GetParentElement() in pie_text_blocks.c

*/
xmlNodePtr
GetParentElement(cmark_node *node, xmlNodePtr pndArgParent)
{
  xmlNodePtr pndResult = NULL;

#if 0
  if (ppeArg != NULL && pndArgParent != NULL) {
  xmlNodePtr pndLast = NULL;
    if (pieElementIsHeader(ppeArg)) {
      /* no new pointer needed */
      int iDepth;

      for (iDepth=0, pndLast=pndArgParent; pndLast; pndLast = pndLast->last) {
	if (IS_NODE_PIE_PIE(pndLast)) {

	  if (IS_NODE_PIE_BLOCK(pndLast->last)) {
	    pndLast = pndLast->last;
	  }

	  if (pieElementGetDepth(ppeArg) == 1) { /* top section node */
	    pndResult = pndLast;
	    break;
	  }
	}
	else if (IS_NODE_PIE_BLOCK(pndLast)) {
	  if (pieElementGetDepth(ppeArg) == 1) { /* top section node */
	    pndResult = pndLast;
	    break;
	  }
	}
	else if (IS_NODE_PIE_SECTION(pndLast)) {
	  iDepth++;
	  pndResult = pndLast;
	  if (iDepth == pieElementGetDepth(ppeArg) - 1) {
	    /* section node with parent depth found */
	    break;
	  }
	}
	else {
	  /* last node is not a section */
	  break;
	}
      }

      for (; iDepth < (pieElementGetDepth(ppeArg) - 1); iDepth++) {
	/* create empty SECTION elements */
	pndResult = xmlNewChild(pndResult, NULL, NAME_PIE_SECTION, NULL);
      }

      if (pndResult) {
      }
      else {
	int i;
	pndResult = pndArgParent;
	for (i=1; i < pieElementGetDepth(ppeArg); i++) {
	  /* create empty SECTION elements */
	  pndResult = xmlNewChild(pndResult, NULL, NAME_PIE_SECTION, NULL);
	}
      }
    }
    else if (pieElementIsListItem(ppeArg)) {
      int iDepth;
      xmlNodePtr pndT;
      xmlNodePtr pndList;

      for (iDepth = 0, pndList = NULL, pndLast = pndT = pndArgParent;
	IS_ENODE(pndT) && iDepth < pieElementGetDepth(ppeArg);
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

      for ( ; iDepth < pieElementGetDepth(ppeArg); iDepth++) {
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

      if (pieElementIsEnum(ppeArg)) {
	xmlSetProp(pndList, BAD_CAST "enum", BAD_CAST "yes");
      }
      pndResult = pndList;
    }
    else {

      for (pndResult = pndLast=pndArgParent; pndLast; pndLast = pndLast->last) {
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
#endif
  
  return pndResult;
} /* end of GetParentElement() */


static xmlNodePtr
PieCmarkAddToPieNode(xmlNodePtr pndArgParent, cmark_node *node, cmark_event_type ev_type, int options)
{
  xmlNodePtr pndResult = NULL;
  
  if (pndArgParent == NULL || node == NULL) {
  }
  else if (ev_type == CMARK_EVENT_ENTER) {
    xmlNodePtr pndT = NULL;
    xmlNodePtr pndParent = NULL;

    pndParent = GetParentElement(node,pndArgParent);
    
    switch (node->type) {
    case CMARK_NODE_DOCUMENT:
      pndT = xmlNewChild(pndArgParent, NULL, NAME_PIE_BLOCK, NULL);
      break;
    case CMARK_NODE_HEADING:
      pndT = xmlNewChild(xmlNewChild(pndArgParent, NULL, NAME_PIE_SECTION, NULL), NULL, NAME_PIE_HEADER, BAD_CAST node->first_child->data);
      break;
    case CMARK_NODE_PARAGRAPH:
      pndT = xmlNewChild(pndArgParent, NULL, NAME_PIE_PAR, BAD_CAST node->first_child->data);
      break;
    case CMARK_NODE_TEXT:
      pndT = xmlNewTextChild(pndArgParent, NULL, NULL, BAD_CAST node->data);
      break;
    case CMARK_NODE_CODE:
    case CMARK_NODE_HTML_BLOCK:
    case CMARK_NODE_HTML_INLINE:
      pndT = xmlNewTextChild(pndArgParent, NULL, NAME_PIE_TT, BAD_CAST node->data);
      break;
    case CMARK_NODE_LIST:
      switch (cmark_node_get_list_type(node)) {
      case CMARK_ORDERED_LIST:
        pndT = xmlNewChild(pndArgParent, NULL, NAME_PIE_LIST, NULL);
        break;
      case CMARK_BULLET_LIST:
        pndT = xmlNewChild(pndArgParent, NULL, NAME_PIE_LIST, NULL);
        break;
      default:
        break;
      }
      break;
    case CMARK_NODE_CODE_BLOCK:
    case CMARK_NODE_BLOCK_QUOTE:
      pndT = xmlNewChild(pndArgParent, NULL, NAME_PIE_PRE, BAD_CAST node->data);
      break;

    case CMARK_NODE_CUSTOM_BLOCK:
    case CMARK_NODE_CUSTOM_INLINE:
      pndT = xmlNewChild(pndArgParent, NULL, NAME_PIE_PRE, BAD_CAST node->data);
      break;
    case CMARK_NODE_LINK:
      pndT = xmlNewChild(pndArgParent, NULL, NAME_PIE_LINK, BAD_CAST node->data);
      //cmark_strbuf_puts(xml, " destination=\""); escape_xml_str(xml, node->as.link.url);
      if (node->as.link.title) {
        //cmark_strbuf_puts(xml, " title=\""); escape_xml_str(xml, node->as.link.title);
      }
      break;
    case CMARK_NODE_IMAGE:
      break;

    default:
      pndT = xmlNewChild(pndArgParent, NULL, BAD_CAST cmark_node_get_type_string(node), BAD_CAST node->data);
      break;
    }

    pndResult = pndT;
  }

  return 1;
}

xmlDocPtr
_cmark_render_pie(xmlDocPtr pdocArg, cmark_node *root, int options)
{
  //cmark_strbuf xml = CMARK_BUF_INIT(root->mem);
  cmark_event_type ev_type;
  cmark_node *cur;
  //struct render_state state = {&xml, 0};

  cmark_iter *iter = cmark_iter_new(root);

  while ((ev_type = cmark_iter_next(iter)) != CMARK_EVENT_DONE) {
    cur = cmark_iter_get_node(iter);
    PieCmarkAddToPieNode(xmlDocGetRootElement(pdocArg), cur, ev_type, options);
  }

  cmark_iter_free(iter);

  return pdocArg;
}


/*!
derived from cmark_markdown_to_html()
*/
xmlDocPtr
cmark_markdown_to_pie(const char* text, size_t len, int options)
{
  xmlDocPtr pdocResult = NULL;
  xmlNodePtr pndRoot;
  xmlNodePtr pndI;
  cmark_node* doc;
  cmark_event_type ev_type;
  cmark_iter* iter;

  pdocResult= xmlNewDoc(BAD_CAST "1.0");
  pndRoot = xmlNewDocNode(pdocResult, NULL, NAME_PIE, NULL);
  xmlDocSetRootElement(pdocResult, pndRoot);

  doc = cmark_parse_document(text, len, options);

  for (pndI = pndRoot, iter = cmark_iter_new(doc); (ev_type = cmark_iter_next(iter)) != CMARK_EVENT_DONE;) {
    cmark_node* cur;

    cur = cmark_iter_get_node(iter);
    pndI = PieCmarkAddToPieNode(pndI, cur, ev_type, options);
  }

  cmark_iter_free(iter);

  cmark_node_free(doc);

  return pdocResult;
}


/*! Append the parsed plain text to the given pndArgTop

\param pndArgTop parent node to append import result nodes OR NULL if pndArgImport must be replaced by result
\param pucArg pointer to an UTF-8 encoded buffer (not XML-conformant!)

\return pointer to result node "block" or NULL in case of errors

similar to ParsePlainBuffer()

*/
xmlNodePtr
ParseMarkdownBuffer(xmlNodePtr pndArgTop, xmlChar* pucArg)
{
  xmlNodePtr pndResult = NULL;

  CompileRegExpDefaults();

  if (STR_IS_NOT_EMPTY(pucArg)) {
    //index_t k;
    //index_t iMax = -1;
    //pieTextElementPtr ppeT = NULL;
    xmlNodePtr pndParent; /*! */
    xmlNodePtr pndBlock; /*! */
    xmlChar* pucT;
    xmlChar* pucText = NULL;

    if (IS_NODE_PIE_BLOCK(pndArgTop)) { /* block element exists already */
      pndBlock = pndArgTop;
    }
    else if (pndArgTop) { /* use as parent node for new block element */
      pndBlock = xmlNewChild(pndArgTop, NULL, NAME_PIE_BLOCK, NULL);
    }
    else { /* there is nor parent element, return new block element */
      pndBlock = xmlNewNode(NULL, NAME_PIE_BLOCK);
    }
    SetTypeAttr(pndBlock, RMODE_MD);

#if 0
    if ((ppeT = pieElementNew(pucArg, eArgMode))) {
      /*\todo iMax = domGetPropValuePtr(pndArgImport, BAD_CAST "max"); */
      iMax = 512 * 1024;
    }

    /*!\todo handling of date-leading formats */

    /*!
    main loop for reading pie text elements and building of DOM
    */
    for (k = 0, pndParent = pndBlock; k < iMax && pieElementHasNext(ppeT); k++) {
      xmlNodePtr pndNew = NULL;

      if (pieElementIsQuote(ppeT)) {
	pndNew = xmlNewChild(pndParent, NULL, NAME_PIE_BLOCK, NULL);
	xmlSetProp(pndNew, BAD_CAST "type", BAD_CAST"quote");
	pndNew = ParsePlainBuffer(pndNew, pieElementGetBeginPtr(ppeT), eArgMode);
      }
      else if (pieElementIsMetaTags(ppeT)) {
	if ((pucT = pieElementGetBeginPtr(ppeT)) != NULL) {
	  pndNew = xmlNewPI(NAME_PIE_PI_TAG, &pucT[6]);
	  /*! insert all PI nodes at the begin of block */
	  if (pndBlock->children) {
	    pndNew->parent = pndBlock;
	    pndNew->next = pndBlock->children;
	    pndNew->next->prev = pndNew;
	    pndBlock->children = pndNew;
	  }
	  else {
	    xmlAddChild(pndParent, pndNew);
	  }
	}
      }
      else if (pieElementIsMetaOrigin(ppeT)) {
	if ((pucT = pieElementGetBeginPtr(ppeT)) != NULL) {
	  xmlSetProp(pndBlock, BAD_CAST "context", &pucT[8]);
	}
      }
      else {
	pieElementParse(ppeT);

	if ((pndNew = pieElementToDOM(ppeT))) {
	  xmlNodePtr pndList = NULL;

	  /* append to result DOM */
	  if (pieElementGetMode(ppeT) == RMODE_TABLE) {
	    if (IS_NODE_PIE_SECTION(pndParent) && xmlStrEqual(domGetPropValuePtr(pndParent, BAD_CAST"type"), BAD_CAST "table")) {
	      /* there is a table parent already */
	    }
	    else {
	      pndParent = xmlNewChild(pndParent, NULL, NAME_PIE_SECTION, NULL);
	      xmlSetProp(pndParent, BAD_CAST "type", BAD_CAST "table");
	      xmlSetProp(pndParent, BAD_CAST "sep", (pieElementGetSepPtr(ppeT) ? pieElementGetSepPtr(ppeT) : BAD_CAST ";"));
	    }
	    xmlAddChild(pndParent, pndNew);
	  }
	  else if (pieElementIsHeader(ppeT) && (pndParent = GetParentElement(ppeT, pndBlock)) != NULL) {
	    pndParent = xmlAddChild(pndParent, pndNew);
	  }
	  else if (pieElementIsListItem(ppeT) && (pndList = GetParentElement(ppeT, pndParent)) != NULL) {
	    xmlAddChild(pndList, pndNew);
	  }
	  else {
	    xmlAddChild(pndParent, pndNew);
	  }
	}
      }
    }
    xmlFree(pucText);
    //domPutNodeString(stderr, BAD_CAST"", pndArgTop);

    if (pieElementGetMode(ppeT) == RMODE_TABLE) {
      TransformToTable(pndArgTop, pndParent, domGetPropValuePtr(pndParent, BAD_CAST"sep"));
      CompressTable(pndParent);
      AddTableCellsEmpty(pndParent);
      xmlUnsetProp(pndParent, BAD_CAST "sep");
      xmlUnsetProp(pndParent, BAD_CAST "type");
    }
    pieElementFree(ppeT);
#endif

    pndResult = pndBlock;
  }

  return pndResult;
} /* end of ParseMarkdownBuffer() */


#ifdef TESTCODE
#include "test/test_pie_cmark.c"
#endif
