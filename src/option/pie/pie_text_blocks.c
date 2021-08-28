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
#include <libxml/tree.h>
#include <libxml/parserInternals.h>

/* 
 */
#include "basics.h"
#include <res_node/res_node_io.h>
#include "dom.h"
#include <pie/pie_text_blocks.h>
#include "utils.h"
#include <vcf/vcf.h>

#ifdef HAVE_PETRINET
#include <cxp/cxp.h>
#include <petrinet/petrinet.h>
#endif

/*! \todo German Umlaute in UNC */
/* s. "Regular Expressions Cookbook" by Jan Goyvaerts and Steven Levithan, Chapter "Validate Windows Paths"
   Published by O’Reilly Media, Inc.

   s. https://www.regular-expressions.info/
*/
#define RE_UNC "(?:\\b[a-z]:\\\\|\\\\\\\\[a-zäÄöÖüÜß0-9_.$\\-]+\\\\[a-zäÄöÖüÜß0-9_.$\\-]+)\\\\*(?:[^\\\\/:*?\"<>|\\r\\n]+\\\\)*[^\\\\/:*?\"<>|\\r\\n]*"
#define RE_URL "(tel|onenote|file|http|https|ftp|ftps|mailto)(://+|%%3A%%2F%%2F|:|%%3A)([a-zäÄöÖüÜß0-9\\.\\-\\&\\#\\;\\:\\,\\+\\_%%\\~\\?\\!=\\@]+|[a-zäÄöÖüÜß0-9\\.\\-]+@)([/a-zäÄöÖüÜß0-9\\(\\)\\.\\-\\&\\#\\;\\,\\+\\:\\_%%\\~\\*\\?\\!=\\@]+)*"
#define RE_LINK "([^\\|]*)\\| *([^\\|]+) *\\|([^\\|]*)\\|"
#define RE_LINK_MD "!*\\[([^\\]]+)\\]\\(([^\\)]+)\\)"
#define RE_LINK_MD_AUTO "(<|&lt;|\\xE2\\x80\\x99)([^<> \\t]+)(>|&gt;|\\xE2\\x80\\x98)"
#define RE_FIG "^[ \\t]*(Fig|Abb)\\.[ \\t]*([^ \\t]+)[ \\t]*(.+)*$"
#define RE_SCRIPT "script=\\\"([^\\\"]+)\\\""

#define RE_DATE_YEAR   "([12][90][0-9]{2}|0{4})"
#define RE_DATE_EASTER "\\@e([\\-+][0-9]+)*"
#define RE_DATE_WEEK   "\\*+w[0-59]*[0-9](mon|tue|wed|thu|fri|sat|sun)"
#define RE_DATE_MONTH  "[01][0-9]"
#define RE_DATE_DAY    "[0123][0-9]"
#define RE_DATE_HOUR   "[\t ]+[012]*[0-9][\\.:][0-5][0-9](-[012]*[0-9][\\.:][0-5][0-9])*"

#define RE_DATE_ISO_TIME "T[012][0-9]:*[0-5][0-9]:*[0-5][0-9]((\\+|" STR_UTF8_MINUS ")[0-9]{2}:*[0-9]{2}|Z|[A-Z]{3})"
#define RE_DATE_MODS   "[\\,\\+\\:\\#\\.x][0-9]+"
#define RE_DATE_TIME   "[\t ]+[012]*[0-9].[0-5][0-9](-[0-9]{1,2}.[0-5][0-9])*"
#define RE_DATE_GERMAN RE_DATE_DAY "\\." RE_DATE_MONTH "\\." RE_DATE_YEAR

#define RE_DATE "\\b("							\
  "(" RE_DATE_YEAR "-" RE_DATE_MONTH "-" RE_DATE_DAY "(" RE_DATE_ISO_TIME "|" RE_DATE_HOUR ")*" ")" \
  "|"									\
  "(" RE_DATE_YEAR RE_DATE_MONTH RE_DATE_DAY "(" RE_DATE_MODS ")*" "(" RE_DATE_HOUR ")*" ")"	\
  "|"									\
  "(" RE_DATE_YEAR RE_DATE_MONTH RE_DATE_DAY "(" RE_DATE_ISO_TIME "|" RE_DATE_HOUR ")*" ")"	\
  "|"									\
  "(" RE_DATE_YEAR RE_DATE_EASTER ")"					\
  "|"									\
  "(" RE_DATE_GERMAN "(" RE_DATE_HOUR ")*" ")"				\
  "|"									\
  "(" RE_DATE_YEAR RE_DATE_WEEK ")"					\
  ")\\b"

#define RE_INLINE "_{2,}[^_]+_{2,}|\\*{2,}[^\\*]+\\*{2,}|`[^`]+`"
#define RE_IMPORT "^#import\\(\\\"(.+)\\\"\\,*([a-z]+)*\\,*([a-z]+)*\\,*([a-z]+)*\\)$"
#define RE_TASK "^(TODO|DONE|REQ|BUG|TARGET|TEST)(:[ \\t]*)"


static pcre2_code *re_unc    = NULL;
static pcre2_code *re_link = NULL;
static pcre2_code *re_link_md = NULL;
static pcre2_code *re_link_md_auto = NULL;
static pcre2_code *re_url = NULL;
static pcre2_code *re_fig = NULL;
static pcre2_code *re_script = NULL;
static pcre2_code *re_date = NULL;
static pcre2_code *re_inline = NULL;
static pcre2_code *re_task = NULL;
static pcre2_code *re_import = NULL;


xmlNsPtr pnsPie = NULL;

static int
GetTableColumns(xmlNodePtr pndArg);

static int
AddTableCellsEmpty(xmlNodePtr pndArg);

static int
CompressTable(xmlNodePtr pndArg);

static xmlChar *
StringDecodeCharMarkupNew(xmlChar *pucArg, lang_t eLangArg);

static xmlChar*
StringGetEndOfHeaderMarker(xmlChar* pucArg);

static xmlNodePtr
SplitStringToDateNodes(const xmlChar *pucArg, RN_MIME_TYPE eMimeTypeArg);

static xmlNodePtr
SplitStringToInlineNodes(const xmlChar *pucArg);

static xmlNodePtr
TaskNodeNew(xmlNodePtr pndArg);


/*! exit procedure for this module
*/
void
pieTextBlocksCleanup(void)
{
  if (pnsPie) {
    xmlFreeNs(pnsPie);
    pnsPie = NULL;
  }

  if (re_unc) {
    pcre2_code_free(re_unc);
    re_unc = NULL;
  }

  if (re_inline) {
    pcre2_code_free(re_inline);
    re_inline = NULL;
  }

  if (re_date) {
    pcre2_code_free(re_date);
    re_date = NULL;
  }

  if (re_url) {
    pcre2_code_free(re_url);
    re_url = NULL;
  }

  if (re_link_md) {
    pcre2_code_free(re_link_md);
    re_link_md = NULL;
  }

   if (re_link_md_auto) {
    pcre2_code_free(re_link_md_auto);
    re_link_md_auto = NULL;
  }

 if (re_link) {
    pcre2_code_free(re_link);
    re_link = NULL;
  }

  if (re_fig) {
    pcre2_code_free(re_fig);
    re_fig = NULL;
  }

  if (re_script) {
    pcre2_code_free(re_script);
    re_script = NULL;
  }

  if (re_task) {
    pcre2_code_free(re_task);
    re_task = NULL;
  }

  if (re_import) {
    pcre2_code_free(re_import);
    re_import = NULL;
  }
}
/* end of pieTextBlocksCleanup() */


/*! init procedure for this module

\return TRUE if successful
*/
BOOL_T
CompileRegExpDefaults(void)
{
  BOOL_T fResult = TRUE;
  size_t erroroffset;
  int errornumber;

  if (re_unc == NULL) {
    PrintFormatLog(2, "Initialize UNC regexp '%s'", RE_UNC);
    re_unc = pcre2_compile(
      (PCRE2_SPTR8)RE_UNC, /* the pattern */
      PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
      PCRE2_CASELESS,        /* default options */
      &errornumber,          /* for error number */
      &erroroffset,          /* for error offset */
      NULL);                 /* use default compile context */

    if (re_unc == NULL) {
      /* regexp error handling */
      PrintFormatLog(1, "UNC regexp '%s' error: '%i'", RE_UNC, errornumber);
      fResult = FALSE;
    }
  }

  if (re_link == NULL) {
    PrintFormatLog(2, "Initialize URL block regexp '%s'", RE_LINK);
    re_link = pcre2_compile(
      (PCRE2_SPTR8)RE_LINK, /* the pattern */
      PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
      PCRE2_CASELESS,        /* default options */
      &errornumber,          /* for error number */
      &erroroffset,          /* for error offset */
      NULL);                 /* use default compile context */

    if (re_link == NULL) {
      /* regexp error handling */
      PrintFormatLog(1, "URL regexp '%s' error: '%i'", RE_LINK, errornumber);
      fResult = FALSE;
    }
  }

  if (re_link_md == NULL) {
    PrintFormatLog(2, "Initialize Markdown URL block regexp '%s'", RE_LINK_MD);
    re_link_md = pcre2_compile(
      (PCRE2_SPTR8)RE_LINK_MD, /* the pattern */
      PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
      PCRE2_CASELESS,        /* default options */
      &errornumber,          /* for error number */
      &erroroffset,          /* for error offset */
      NULL);                 /* use default compile context */

    if (re_link_md == NULL) {
      /* regexp error handling */
      PrintFormatLog(1, "URL regexp '%s' error: '%i'", RE_LINK_MD, errornumber);
      fResult = FALSE;
    }
  }

  if (re_link_md_auto == NULL) {
    PrintFormatLog(2, "Initialize Markdown URL block regexp '%s'", RE_LINK_MD_AUTO);
    re_link_md_auto = pcre2_compile(
      (PCRE2_SPTR8)RE_LINK_MD_AUTO, /* the pattern */
      PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
      PCRE2_CASELESS,        /* default options */
      &errornumber,          /* for error number */
      &erroroffset,          /* for error offset */
      NULL);                 /* use default compile context */

    if (re_link_md_auto == NULL) {
      /* regexp error handling */
      PrintFormatLog(1, "URL regexp '%s' error: '%i'", RE_LINK_MD_AUTO, errornumber);
      fResult = FALSE;
    }
  }

  if (re_url == NULL) {
    PrintFormatLog(2, "Initialize URL regexp '%s'", RE_URL);
    re_url = pcre2_compile(
      (PCRE2_SPTR8)RE_URL, /* the pattern */
      PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
      PCRE2_CASELESS,        /* default options */
      &errornumber,          /* for error number */
      &erroroffset,          /* for error offset */
      NULL);                 /* use default compile context */

    if (re_url == NULL) {
      /* regexp error handling */
      PrintFormatLog(1, "URL regexp '%s' error: '%i'", RE_URL, errornumber);
      fResult = FALSE;
    }
  }

  if (re_date == NULL) {
    PrintFormatLog(2, "Initialize DATE regexp '%s'", RE_DATE);
    re_date = pcre2_compile(
      (PCRE2_SPTR8)RE_DATE, /* the pattern */
      PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
      PCRE2_CASELESS,        /* default options */
      &errornumber,          /* for error number */
      &erroroffset,          /* for error offset */
      NULL);                 /* use default compile context */

    if (re_date == NULL) {
      /* regexp error handling */
      PrintFormatLog(1, "DATE regexp '%s' error: '%i'", RE_DATE, errornumber);
      fResult = FALSE;
    }
  }

  if (re_inline == NULL) {
    PrintFormatLog(2, "Initialize INLINE regexp '%s'", RE_DATE);
    re_inline = pcre2_compile(
      (PCRE2_SPTR8)RE_INLINE, /* the pattern */
      PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
      PCRE2_CASELESS,        /* default options */
      &errornumber,          /* for error number */
      &erroroffset,          /* for error offset */
      NULL);                 /* use default compile context */

    if (re_inline == NULL) {
      /* regexp error handling */
      PrintFormatLog(1, "INLINE regexp '%s' error: '%i'", RE_INLINE, errornumber);
      fResult = FALSE;
    }
  }

  if (re_fig == NULL) {
    /* compile regexp first */
    PrintFormatLog(2, "Initialize Figure regexp '%s'", RE_FIG);
    re_fig = pcre2_compile(
      (PCRE2_SPTR8)RE_FIG, /* the pattern */
      PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
      PCRE2_CASELESS,        /* default options */
      &errornumber,          /* for error number */
      &erroroffset,          /* for error offset */
      NULL);                 /* use default compile context */

    if (re_fig == NULL) {
      /* regexp error handling */
      PrintFormatLog(1, "Figure regexp '%s' error: '%i'", RE_FIG, errornumber);
      fResult = FALSE;
    }
  }

  if (re_script == NULL) {
    /* compile regexp first */
    PrintFormatLog(2, "Initialize Script regexp '%s'", RE_SCRIPT);
    re_script = pcre2_compile(
      (PCRE2_SPTR8)RE_SCRIPT, /* the pattern */
      PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
      0,		        /* default options */
      &errornumber,          /* for error number */
      &erroroffset,          /* for error offset */
      NULL);                 /* use default compile context */

    if (re_script == NULL) {
      /* regexp error handling */
      PrintFormatLog(1, "Script regexp '%s' error: '%i'", RE_SCRIPT, errornumber);
      fResult = FALSE;
    }
  }

  if (re_import == NULL) {
    PrintFormatLog(2, "Initialize import regexp '%s'", RE_IMPORT);
    re_import = pcre2_compile(
      (PCRE2_SPTR8)RE_IMPORT, /* the pattern */
      PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
      0,        /* default options */
      &errornumber,          /* for error number */
      &erroroffset,          /* for error offset */
      NULL);                 /* use default compile context */

    if (re_import == NULL) {
      /* regexp error handling */
      PrintFormatLog(1, "File import regexp '%s' error: '%i'", RE_IMPORT, errornumber);
      fResult = FALSE;
    }
  }

  if (re_task == NULL) {
    PrintFormatLog(2, "Initialize task regexp '%s'", RE_TASK);
    re_task = pcre2_compile(
      (PCRE2_SPTR8)RE_TASK, /* the pattern */
      PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
      0,        /* default options */
      &errornumber,          /* for error number */
      &erroroffset,          /* for error offset */
      NULL);                 /* use default compile context */

    if (re_task == NULL) {
      /* regexp error handling */
      PrintFormatLog(1, "File task regexp '%s' error: '%i'", RE_TASK, errornumber);
      fResult = FALSE;
    }
  }

  if (pnsPie == NULL) {
    pnsPie = xmlNewNs(NULL, BAD_CAST CXP_PIE_URL, BAD_CAST"pie");
  }

  return fResult;
}
/* End of CompileRegExpDefaults() */


/*! \return the xmlNodePtr to an according section or block node
*/
xmlNodePtr
GetParentElement(pieTextElementPtr ppeArg, xmlNodePtr pndArgParent)
{
  xmlNodePtr pndLast = NULL;
  xmlNodePtr pndResult = NULL;

  if (ppeArg != NULL && pndArgParent != NULL) {
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

      for (iDepth = 0, pndLast = pndList = NULL, pndT = pndArgParent;
	IS_ENODE(pndT) && iDepth < pieElementGetDepth(ppeArg);
	pndT = pndT->last) {

	if (IS_NODE_PIE_LIST(pndT)) {
	  pndList = pndT;
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
  return pndResult;
} /* end of GetParentElement() */


/*! Append the parsed plain text to the given pndArgTop

\param pndArgTop parent node to append import result nodes OR NULL if pndArgImport must be replaced by result
\param pucArg pointer to an UTF-8 encoded buffer (not XML-conformant!)

\return pointer to result node "block" or NULL in case of errors
*/
xmlNodePtr
ParsePlainBuffer(xmlNodePtr pndArgTop, xmlChar* pucArg, rmode_t eArgMode)
{
  xmlNodePtr pndResult = NULL;

  CompileRegExpDefaults();

  if (STR_IS_NOT_EMPTY(pucArg)) {
    index_t k;
    index_t iMax = -1;
    pieTextElementPtr ppeT = NULL;
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
    SetTypeAttr(pndBlock, eArgMode);

    if ((ppeT = pieElementNew(pucArg, eArgMode))) {
      /*\todo iMax = domGetPropValuePtr(pndArgImport, BAD_CAST "max"); */
      iMax = 512 * 1024;
    }

    /*!\todo handling of date-leading formats */

    /*!
    main loop for reading pie text elements and building of DOM
    */
    for (k=0, pndParent = pndBlock; k < iMax && pieElementHasNext(ppeT); k++) {
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

    pndResult = pndBlock;
  }

  return pndResult;
} /* end of ParsePlainBuffer() */


/*! return TRUE if pucArg ends at postion iArg with an XML entity like '&amp;'
*/
BOOL_T
StringEndsWithEntity(xmlChar*pucArg, int iArg)
{
  BOOL_T fResult = FALSE;

  if (pucArg != NULL && iArg > 1 && pucArg[iArg] == (xmlChar)';') {
    int i;

    for (i=iArg-1; i > 0; i--) {
      if (isalnum(pucArg[i])) {
      }
      else {
	fResult = (pucArg[i] == (xmlChar)'&' || (i > 0 && pucArg[i-1] == (xmlChar)'&' && pucArg[i] == (xmlChar)'#'));
	break;
      }
    }
  }
  return fResult;
} /* end of StringEndsWithEntity() */


/*! \return a pointer to first char after header markup in pucArg or NULL
*/
xmlChar* 
StringGetEndOfHeaderMarker(xmlChar* pucArg)
{
  xmlChar* pucResult = NULL;

  if (pucArg) {
    xmlChar* pucT;
    BOOL_T fHeader;

    for (pucT = pucArg, fHeader = FALSE; ; pucT++) {
      if (*pucT == (xmlChar)'*') {
	fHeader = TRUE;
      }
      else if (*pucT == (xmlChar)' ') {
	/* skip spaces */
      }
      else {
	break;
      }
    }

    if (fHeader) {
      pucResult = pucT;
    }
  }
  return pucResult;
} /* end of StringGetEndOfHeaderMarker() */


/*! \return TRUE if content string of pndArgParent is splitted into table data nodes using pucPatternSep, else FALSE

\todo compile 'pucPatternSep' as a regexp if '/[;|]/'
*/
BOOL_T
SplitNodeToTableDataNodes(xmlNodePtr pndArgParent, xmlChar* pucPatternSep)
{
  BOOL_T fResult = FALSE;
  
  if (pndArgParent == NULL) {
  }
  else if (pndArgParent->children == NULL || STR_IS_EMPTY(pndArgParent->children->content)) {
    xmlNodeSetName(pndArgParent, NAME_PIE_TR);
  }
  else if (pndArgParent->children != NULL && pndArgParent->children == pndArgParent->last
      && pndArgParent->children->content != NULL && xmlStrlen(pndArgParent->children->content) > 0) {
    BOOL_T fMix = FALSE; /* flag for ';' in pucPatternSep -> problems with included XML entities '&amp;' */
    xmlChar *pucBegin;
    xmlChar *pucSep;
    xmlChar* pucCell = NULL;
    xmlChar* pucT;
    xmlNodePtr pndChild;

    pndChild = pndArgParent->children;
    xmlUnlinkNode(pndChild);

    fMix = (xmlStrchr(pucPatternSep,(xmlChar)';') != NULL);

    for (pucBegin = pucSep = pndChild->content; (pucSep = BAD_CAST xmlStrstr(pucSep, pucPatternSep)); ) {

      if (fMix && StringEndsWithEntity(pucBegin, (int)(pucSep - pucBegin))) {
	/* its a XML entity like '&amp;' dont use this as separator */
	pucSep++;
      }
      else if ((pucCell = xmlStrndup(pucBegin, (int)(pucSep - pucBegin))) != NULL) {
	StringRemovePairQuotes(pucCell);
	if ((pucT = StringGetEndOfHeaderMarker(pucCell))) {
	  xmlNewChild(pndArgParent, NULL, NAME_PIE_TH, pucT);
	}
	else {
	  xmlNewChild(pndArgParent, NULL, NAME_PIE_TD, pucCell);
	}
	xmlFree(pucCell);
      }

      pucBegin = pucSep + xmlStrlen(pucPatternSep);
      pucSep = pucBegin;
    }

    if (STR_IS_NOT_EMPTY(pucBegin) && (pucCell = xmlStrdup(pucBegin)) != NULL) { /* append last cell in line */
      StringRemovePairQuotes(pucCell);
      if ((pucT = StringGetEndOfHeaderMarker(pucCell))) {
	xmlNewChild(pndArgParent, NULL, NAME_PIE_TH, pucT);
      }
      else {
	xmlNewChild(pndArgParent, NULL, NAME_PIE_TD, pucCell);
      }
      xmlFree(pucCell);
    }

    xmlFreeNode(pndChild);
    xmlNodeSetName(pndArgParent,NAME_PIE_TR);
    fResult = TRUE;
  }
  
  return fResult;
} /* end of SplitNodeToTableDataNodes() */


/*! \return number of columns in widest row in table
*/
int
GetTableColumns(xmlNodePtr pndArg)
{
  int iResult = 0;

  if (IS_NODE_PIE_TABLE(pndArg)) {
    xmlNodePtr pndArgRow;

    for (pndArgRow=pndArg->children; pndArgRow; pndArgRow = pndArgRow->next) {

      if (IS_NODE_PIE_TR(pndArgRow)) {
	int c = 0;
	xmlNodePtr pndCol;

	for (pndCol=pndArgRow->children; pndCol; pndCol = pndCol->next) {
	  if (IS_NODE_PIE_TH(pndCol) || IS_NODE_PIE_TD(pndCol)) {
	    c++;
	  }
	}

	if (c > iResult) {
	  iResult = c;
	}
      }
    }
  }
  return iResult;
} /* end of GetTableColumns() */


/*! \return number of columns in widest row in table
*/
int
AddTableCellsEmpty(xmlNodePtr pndArg)
{
  int iResult = 0;
  int i;

  i = GetTableColumns(pndArg);
  if (i > 0) {
    int r;
    xmlNodePtr pndArgRow;
    xmlChar mucT[128];

    xmlStrPrintf(mucT,128,"%i",i);
    
    for (r=0, pndArgRow=pndArg->children; pndArgRow; pndArgRow = pndArgRow->next, r++) {

      if (IS_NODE_PIE_TR(pndArgRow)) {
	int j;
	xmlNodePtr pndCell;
	BOOL_T fHeader = FALSE;

	/*! count cell elements and detect if header row
	 */
	for (pndCell = pndArgRow->children; pndCell; pndCell = pndCell->next) {
	  fHeader |= IS_NODE_PIE_TH(pndCell);
	}

	if (fHeader && pndArgRow->children == pndArgRow->last) { /* single header cell */
	  xmlSetProp(pndArgRow->children, BAD_CAST "colspan", mucT);
	}
	else {
	  /*!
	    fix element names and add missing cell elements
	   */
	  for (j=0, pndCell=pndArgRow->children; j < i; ) {
	    if (IS_NODE_PIE_TH(pndCell)) {
	      j++;
	      pndCell = pndCell->next;
	    }
	    else if (IS_NODE_PIE_TD(pndCell)) {
	      if (fHeader) {
		xmlNodeSetName(pndCell, NAME_PIE_TH);
	      }
	      j++;
	      pndCell = pndCell->next;
	    }
	    else if (IS_ENODE(pndCell)) {
	      pndCell = pndCell->next;
	    }
	    else {
	      xmlNewChild(pndArgRow, NULL, (fHeader ? NAME_PIE_TH : NAME_PIE_TD), BAD_CAST" ");
	      j++;
	    }
	  }
	}
      }
    }
    /* add some meta data to table */
    xmlSetProp(pndArg, BAD_CAST "cols", mucT);
    xmlStrPrintf(mucT,128,"%i",r);
    xmlSetProp(pndArg, BAD_CAST "rows", mucT);
  }
  return iResult;
} /* end of AddTableCellsEmpty() */


/*! return TRUE if there is a circular reference when importing prnArg into pndArg
*/
BOOL_T
IsTableCellEmpty(xmlNodePtr pndArg)
{
  BOOL_T fResult = FALSE;

#if 1
  if (pndArg) {
    xmlChar* pucT;

    pucT = xmlNodeListGetString(pndArg->doc, pndArg->children, 1);
    NormalizeStringSpaces((char*)pucT);
    fResult = xmlStrlen(pucT) < 1;
    xmlFree(pucT);
  }
#else
  if (IS_NODE_PIE_TD(pndArg) || IS_NODE_PIE_TH(pndArg)) {
    xmlNodePtr pndChild;

    for (pndChild = pndArg->children; fResult == FALSE && pndChild != NULL; pndChild = pndChild->next) {
      if (xmlNodeIsText(pndChild)) {
	fResult = STR_IS_NOT_EMPTY(pndChild->content);
      }
      else if (pndChild->children == NULL) {
	fResult = TRUE;
      }
      else if (STR_IS_EMPTY(pndChild->children->content)) {
	fResult = TRUE;
      }
      else if (xmlStrEqual(pndChild->children->content, BAD_CAST" ")) {
	fResult = TRUE;
      }
    }
  }
#endif
  return fResult;
} /* end of IsTableCellEmpty() */


/*! remove all empty columns and rows from table element
*/
int
CompressTable(xmlNodePtr pndArg)
{
  int iResult = 0;

  if (IS_NODE_PIE_TABLE(pndArg)) {
    int i;
    BOOL_T fEmpty;
    xmlNodePtr pndCell;
    xmlNodePtr pndTableRow;
    xmlNodePtr pndTableRowNext;

    for (pndTableRow=pndArg->children; pndTableRow; pndTableRow = pndTableRowNext) {
      pndTableRowNext = pndTableRow->next;
      if (IS_NODE_PIE_TR(pndTableRow)) {

	/*! count cell elements and detect if header row
	 */
	for (fEmpty = TRUE, pndCell = pndTableRow->children; fEmpty && pndCell != NULL; pndCell = pndCell->next) {
	  fEmpty = IsTableCellEmpty(pndCell);
	}

	if (fEmpty) { /* cells are empty */
	  xmlUnlinkNode(pndTableRow);
	  xmlFreeNode(pndTableRow);
	}
      }
    }

    for (i = GetTableColumns(pndArg) - 1; i > -1; i--) {
      int j;

      for (fEmpty = TRUE, pndTableRow=pndArg->children; fEmpty && pndTableRow != NULL; pndTableRow = pndTableRow->next) {
	if (IS_NODE_PIE_TR(pndTableRow)) {
	  for (j = 0, pndCell = pndTableRow->children; pndCell != NULL; j++, pndCell = pndCell->next) {
	    if (j == i) {
	      fEmpty = IsTableCellEmpty(pndCell);
	      break;
	    }
	  }
	}
      }

      /* all cells in column i are empty */
      for (pndTableRow=pndArg->children; fEmpty && pndTableRow != NULL; pndTableRow = pndTableRow->next) {
	if (IS_NODE_PIE_TR(pndTableRow)) {
	  xmlNodePtr pndTableCellNext;

	  for (j = 0, pndCell = pndTableRow->children; pndCell != NULL; j++, pndCell = pndTableCellNext) {
	    pndTableCellNext = pndCell->next;
	    if (j == i) {
	      xmlUnlinkNode(pndCell);
	      xmlFreeNode(pndCell);
	      break;
	    }
	  }
	}
      }
    }
  }
  return iResult;
} /* end of CompressTable() */


/*!

\param pndArgParent
\param pndArg node of
 */
xmlNodePtr
TransformToTable(xmlNodePtr pndArgParent, xmlNodePtr pndArg, xmlChar *pucPatternSep)
{
  xmlNodePtr pndResult = pndArgParent;

  // autodetect separator using "sep=" or statistics

  if (pndArg) {
    xmlNodePtr pndChild;

    if (IS_NODE_PIE_BLOCK(pndArg) || (IS_NODE_PIE_SECTION(pndArg) && domPropIsEqual(pndArg, BAD_CAST"type", BAD_CAST"table"))) {
      xmlNodeSetName(pndArg, NAME_PIE_TABLE);
      xmlNewTextChild(pndArg, NULL, NAME_PIE_TTAG, BAD_CAST"#table");

      for (pndChild = pndArg->children; pndChild;) {
	xmlNodePtr pndChildNext;

	pndChildNext = pndChild->next;
	TransformToTable(pndArgParent, pndChild, pucPatternSep);
	pndChild = pndChildNext;
      }
    }
    else if (IS_NODE_PIE_PAR(pndArg)) {
      SplitNodeToTableDataNodes(pndArg, pucPatternSep);
    }
    else if (IS_NODE_PIE_HEADER(pndArg)) {
      SplitNodeToTableDataNodes(pndArg, pucPatternSep);
      for (pndChild = pndArg->children; pndChild; pndChild = pndChild->next) {
	xmlNodeSetName(pndChild,NAME_PIE_TH);
      }
    }
    else if (IS_NODE_PIE_TD(pndArg) || IS_NODE_PIE_TH(pndArg) || IS_NODE_PIE_TABLE(pndArg)) {
      /* already transformed */
    }
  }
  
  return pndResult;
}
/* end of TransformToTable() */


/*! XPath ""

\param pndArg node of
*/
xmlNodePtr
FindElementNodeLast(xmlNodePtr pndArg)
{
  xmlNodePtr pndResult = pndArg;
  xmlNodePtr pndLast;

  for (pndLast = pndArg->last; pndLast != NULL; pndLast = pndLast->last) {
    pndResult = pndLast;
  }

  return pndResult;
}
/* end of FindElementNodeLast() */


/*! return TRUE if 
 */
BOOL_T
IsImportFromAttr(xmlNodePtr pndArg)
{
  if (pndArg) {
    xmlNodePtr pndT;

    for (pndT = (xmlNodePtr) pndArg->properties; pndT; pndT=pndT->next) {
      if (xmlStrEqual(pndT->name, BAD_CAST"valid") || xmlStrEqual(pndT->name, BAD_CAST"type") || xmlStrEqual(pndT->name, BAD_CAST"context") || xmlStrEqual(pndT->name, BAD_CAST"root")) {
	/* attribute name is common */
      }
      else {
	return TRUE;
      }
    }
  }
  return FALSE;
}
/* end of IsImportFromAttr() */


/*! return TRUE if there is a circular reference when importing prnArg into pndArg
*/
BOOL_T
IsImportCircularStr(xmlNodePtr pndArg, xmlChar *pucArgURI)
{
  BOOL_T fResult = FALSE;

  if (pndArg) {
    xmlNodePtr pndT;
    xmlChar *pucT;

    for (pndT=pndArg->parent; pndT != NULL && fResult == FALSE; pndT=pndT->parent) {
      if ((pucT = domGetPropValuePtr(pndT, BAD_CAST"context"))) {
	if (resPathIsEquivalent(pucT, pucArgURI)) {
	  PrintFormatLog(1, "Error circular for '%s' found", pucArgURI);
	  fResult = TRUE;
	}
	else {
	  PrintFormatLog(1, "Context '%s' found", pucT);
	}
      }
    }
  }
  return fResult;
} /* end of IsImportCircularStr() */


/*! return TRUE if there is a circular reference when importing prnArg into pndArg
*/
BOOL_T
IsImportCircular(xmlNodePtr pndArg, resNodePtr prnArg)
{
  if (pndArg != NULL
    && (resNodeGetMimeType(prnArg) == MIME_APPLICATION_PIE_XML || resNodeGetMimeType(prnArg) == MIME_TEXT_PLAIN)) {
    return IsImportCircularStr(pndArg, resNodeGetURI(prnArg));
  }
  return FALSE;
} /* end of IsImportCircular() */


#ifdef LEGACY

/*! translates UNC paths into URL notation first
*/
xmlChar *
TranslateUncToUrl(const xmlChar *pucArg)
{
  size_t ducOrigin;
  xmlChar *pucResult = xmlStrdup(pucArg);

  if (pucArg != NULL && (ducOrigin = xmlStrlen(pucArg)) > 0) {
    int rc;
    pcre2_match_data *match_data;

    match_data = pcre2_match_data_create_from_pattern(re_unc, NULL);
    rc = pcre2_match(
      re_unc,        /* result of pcre2_compile() */
      (PCRE2_SPTR8)pucArg,  /* the subject string */
      xmlStrlen(pucArg),             /* the length of the subject string */
      0,              /* start at offset 0 in the subject */
      0,              /* default options */
      match_data,        /* vector of integers for substring information */
      NULL);            /* number of elements (NOT size in bytes) */

    if (rc > -1) {
      PCRE2_SIZE *ovector;

      ovector = pcre2_get_ovector_pointer(match_data);
      if (ovector[1] - ovector[0] > 3) {
	/*
	   the regexp match, assemble a new string
	   */
	int i;
	xmlChar *pucUnc;
	xmlChar *pucA = (xmlChar *)pucArg + ovector[0];
	xmlChar *pucRelease;
	xmlChar *pucPre;
	xmlChar *pucT;
	xmlChar *pucPost;

	pucUnc = xmlStrndup(pucA, (int)(ovector[1] - ovector[0]));
	PrintFormatLog(3, "UNC '%s' (%i..%i) in '%s'", pucUnc, ovector[0], ovector[1], pucArg);

	if (ovector[0] > 6 && xmlStrncasecmp(BAD_CAST &pucArg[ovector[0] - 7], BAD_CAST"file://", 7) == 0) {
	  /* the content ends with "file://" already */
	  pucRelease = xmlStrndup(pucArg, (int)ovector[0]);
	  pucPre = xmlStrncatNew(pucRelease, BAD_CAST "/", -1);
	  xmlFree(pucRelease);
	}
	else if (ovector[0] > 0) {
	  /* the content starts with text	*/
	  pucRelease = xmlStrndup(pucArg, (int)ovector[0]);
	  pucPre = xmlStrncatNew(pucRelease, BAD_CAST "file:///", -1);
	  xmlFree(pucRelease);
	}
	else {
	  pucPre = xmlStrdup(BAD_CAST "file:///");
	}

	if (pucArg[ovector[0]] == '\\' && pucArg[ovector[0] + 1] == '\\') {
	  /* UNC noatation */
	  pucRelease = xmlStrndup(&(pucArg[ovector[0] + 1]), (int)(ovector[1] - ovector[0] - 1));
	}
	else {
	  pucRelease = xmlStrndup(&(pucArg[ovector[0]]), (int)(ovector[1] - ovector[0]));
	}

	for (i = 0; pucRelease[i] != '\0'; i++) {
	  if (pucRelease[i] == '\\') {
	    pucRelease[i] = '/';
	  }
	}

	if (pucPre) {
	  pucT = pucPre;
	  pucPre = xmlStrncatNew(pucPre, pucRelease, -1);
	  xmlFree(pucT);
	  xmlFree(pucRelease);
	}
	else {
	  pucPre = pucRelease;
	}

	if (ducOrigin > ovector[1]) {
	  /* there is a trailing text */
	  /* recursion */
	  pucPost = TranslateUncToUrl(&(pucArg[ovector[1]]));
	  pucRelease = pucPre;
	  pucPre = xmlStrncatNew(pucRelease, pucPost, -1);
	  xmlFree(pucRelease);
	  xmlFree(pucPost);
	}
	xmlFree(pucResult);
	pucResult = pucPre;
	PrintFormatLog(3, "UNC '%s'", pucResult);

	xmlFree(pucUnc);
      }
    }
    pcre2_match_data_free(match_data);   /* Release memory used for the match */
  }
  return pucResult;
}
/* end of TranslateUncToUrl() */


/*! detect longest word in URL and remove '-'
*/
xmlChar *
GetLinkDisplayNameNew(const xmlChar *pucArg)
{
  xmlChar *pucResult = NULL;

  if (pucArg) {
    size_t a,b,i,l;

    for (i=0,l=0,b=0,a=0; pucArg[i] != '\0'; i++) {
      if ((pucArg[i] == '/' || pucArg[i] == '=' || pucArg[i] == '&' || pucArg[i] == '?') && (i - b) > l) {
	b = a;
	a = i + 1;
	l = a - b - 1;
      }
    }

    if (b > 0 && l > 12) {
      pucResult = xmlStrndup(&pucArg[b], (int)l);
      for (i=0; pucResult[i] != '\0'; i++) {
        if ( ! isalnum(pucResult[i])) {
  	pucResult[i] = ' ';
        }
      }
    }
    else {
      pucResult = xmlStrdup(pucArg);
    }
  }
  return pucResult;
}
/* end of GetLinkDisplayNameNew() */


/*! splits an UTF-8 string into a list of text and link element nodes (s. "Test/TestGood.txt")

  https://en.wikipedia.org/wiki/Internationalized_Resource_Identifier

  \deprecated to be replaced by SplitTupelToLinkNodesMd()
*/
xmlNodePtr
SplitTupelToLinkNodes(const xmlChar *pucArg)
{
  int ducOrigin;
  xmlNodePtr pndResult = NULL;

  if (pucArg != NULL && (ducOrigin = xmlStrlen(pucArg)) > 0) {
    int rc;
    pcre2_match_data *match_data_link;
    xmlNodePtr pndLink;
    xmlNodePtr pndPostfix;

    /* http://www.pcre.org/current/doc/html/pcre2demo.html */

    match_data_link = pcre2_match_data_create_from_pattern(re_link, NULL);
    rc = pcre2_match(
	re_link,        /* result of pcre2_compile() */
	(PCRE2_SPTR8)pucArg,  /* the subject string */
	xmlStrlen(pucArg),             /* the length of the subject string */
	0,              /* start at offset 0 in the subject */
	0,              /* default options */
	match_data_link,        /* vector of integers for substring information */
	NULL);            /* number of elements (NOT size in bytes) */

    if (rc > -1) {
      /*
	 the regexp match, assemble node list with a common dummy
	 element node
       */
      xmlChar *pucPre;
      xmlChar *pucUrl = NULL;
      xmlChar *pucUrlDisplay = NULL;
      PCRE2_SIZE *ovector;
      int i = 0;

      ovector = pcre2_get_ovector_pointer(match_data_link);

      PrintFormatLog(3, "URL (%i..%i) in '%s'", ovector[i], ovector[i+1], pucArg);

      pndResult = xmlNewNode(NULL, BAD_CAST "dummy");

      i++;
      if (ovector[i*2+1] - ovector[i*2] > 0) {
	pucPre = xmlStrndup(&pucArg[ovector[0]], (int)(ovector[i*2+1] - ovector[i*2]));
	PrintFormatLog(3, "URL pre '%s' (%i..%i) in '%s'", pucPre, ovector[i*2], ovector[i*2+1], pucArg);
	xmlAddChild(pndResult, xmlNewText(pucPre));
	xmlFree(pucPre);
      }

      i++;
      if (ovector[i*2+1] - ovector[i*2] > 0) {
	/*! \todo Percent-encode non-ASCII chars (s. https://en.wikipedia.org/wiki/Percent-encoding) */
#if 1
	pucUrl = xmlStrndup(&pucArg[ovector[i*2]], (int)(ovector[i*2+1] - ovector[i*2]));
#else
	xmlChar *pucT;

	pucT = xmlStrndup(&pucArg[ovector[i*2]], ovector[i*2+1] - ovector[i*2]);
	if (pucT) {
	  if ((pucUrl = EncodeRFC1738(pucT))) {
	    PrintFormatLog(3, "URL '%s' (%i..%i) in '%s'", pucUrl, ovector[i*2], ovector[i*2+1], pucArg);
	  }
	  else {
	    PrintFormatLog(1, "EncodeRFC1738 error");
	    pucUrl = NULL;
	  }
	  xmlFree(pucT);
	}
#endif
      }

      i++;
      if (ovector[i*2+1] - ovector[i*2] > 0) {
	pucUrlDisplay = xmlStrndup(&pucArg[ovector[i*2]], (int)(ovector[i*2+1] - ovector[i*2]));
	PrintFormatLog(3, "URL display name '%s' (%i..%i) in '%s'", pucUrlDisplay, ovector[i*2], ovector[i*2+1], pucArg);
      }
      else {
	pucUrlDisplay = GetLinkDisplayNameNew(pucUrl);
      }
      pndLink = xmlNewTextChild(pndResult, NULL, NAME_PIE_LINK, pucUrlDisplay);
      xmlSetProp(pndLink, BAD_CAST "href", pucUrl);

      if (xmlStrlen(&pucArg[ovector[1]]) > 0) {
	PrintFormatLog(3, "URL post '%s' (%i..%i) in '%s'", &pucArg[ovector[1]], ovector[1], ovector[1] + xmlStrlen(&pucArg[ovector[1]]), pucArg);
	/* the content ends with text, recursion */
	pndPostfix = SplitTupelToLinkNodes(&pucArg[ovector[1]]);
	if (pndPostfix) {
	  xmlNodePtr pndT = pndPostfix->children;
	  domUnlinkNodeList(pndT);
	  xmlAddChildList(pndResult, pndT);
	  xmlFreeNode(pndPostfix);
	}
	else {
	  xmlChar *pucT = xmlStrdup(&pucArg[ovector[1]]);
	  xmlAddChild(pndResult, xmlNewText(pucT));
	  xmlFree(pucT);
	}
      }

      xmlFree(pucUrlDisplay);
      xmlFree(pucUrl);
    }

    pcre2_match_data_free(match_data_link);   /* Release memory used for the match */
  }
  return pndResult;
}
/* end of SplitTupelToLinkNodes() */

#endif


/*! splits an UTF-8 string into a list of text and link element nodes (s. "Test/TestGood.txt")

\return node tree containing all URL as link elements
*/
xmlNodePtr
SplitTupelToLinkNodesMd(const xmlChar *pucArg)
{
  int ducOrigin;
  xmlNodePtr pndResult = NULL;

  if (pucArg != NULL && (ducOrigin = xmlStrlen(pucArg)) > 0) {
    int rc;
    pcre2_match_data *match_data_link;
    xmlNodePtr pndLink = NULL;
    xmlNodePtr pndPostfix;

    /* http://www.pcre.org/current/doc/html/pcre2demo.html */

    match_data_link = pcre2_match_data_create_from_pattern(re_link_md, NULL);
    rc = pcre2_match(
      re_link_md,        /* result of pcre2_compile() */
	(PCRE2_SPTR8)pucArg,  /* the subject string */
	xmlStrlen(pucArg),             /* the length of the subject string */
	0,              /* start at offset 0 in the subject */
	0,              /* default options */
	match_data_link,        /* vector of integers for substring information */
	NULL);            /* number of elements (NOT size in bytes) */

    if (rc > -1) {
      /*
	 the regexp match, assemble node list with a common dummy
	 element node
       */
      xmlChar *pucUrl = NULL;
      PCRE2_SIZE *ovector;
      int i = 0;

      assert(pcre2_get_ovector_count(match_data_link) == 3);
      ovector = pcre2_get_ovector_pointer(match_data_link);
      /*
      0 - begin of match
      1 - end of match
      2 - begin of first group (display text)
      3 - end of first group
      4 - begin of second group (URL)
      5 - end of second group
      */

      PrintFormatLog(3, "URL (%i..%i) in '%s'", ovector[i], ovector[i+1], pucArg);
      pndResult = xmlNewNode(NULL, BAD_CAST "dummy");

      if (ovector[0] > 0) {
	xmlChar *pucPre;

	pucPre = xmlStrndup(pucArg, (int)ovector[0]);
	PrintFormatLog(3, "URL pre text '%s' (%i..%i) in '%s'", pucPre, 0, ovector[0], pucArg);
	xmlAddChild(pndResult, xmlNewText(pucPre));
	xmlFree(pucPre);
      }

      i++;
      if (ovector[i*2+1] - ovector[i*2] > 0) {
	xmlChar *pucUrlDisplay = NULL;

	pucUrlDisplay = xmlStrndup(&pucArg[ovector[i*2]], (int)(ovector[i*2+1] - ovector[i*2]));
	if (pucArg[ovector[0]] == '!') {
	  PrintFormatLog(3, "Image display text '%s' (%i..%i) in '%s'", pucUrlDisplay, ovector[i*2], ovector[i*2+1], pucArg);
	  pndLink = xmlNewChild(pndResult, NULL, NAME_PIE_IMG, NULL);
	  xmlSetProp(pndLink, BAD_CAST "title", pucUrlDisplay);
	}
	else {
	  PrintFormatLog(3, "URL display text '%s' (%i..%i) in '%s'", pucUrlDisplay, ovector[i*2], ovector[i*2+1], pucArg);
	  pndLink = xmlNewChild(pndResult, NULL, NAME_PIE_LINK, NULL);
	  xmlAddChild(pndLink, xmlNewText(pucUrlDisplay));
	}
	xmlFree(pucUrlDisplay);
      }

      i++;
      if (ovector[i*2+1] - ovector[i*2] > 0
	  && (pucUrl = xmlStrndup(&pucArg[ovector[i*2]], (int)(ovector[i*2+1] - ovector[i*2]))) != NULL) {

	if (IS_NODE_PIE_IMG(pndLink)) {
	  xmlSetProp(pndLink, BAD_CAST "src", pucUrl);
	}
	else if (StringBeginsWith((char *)pucUrl,"id:")) {
	  xmlSetProp(pndLink, BAD_CAST "id", &pucUrl[3]);
	}
	else {
	  xmlSetProp(pndLink, BAD_CAST "href", pucUrl);
	}
	xmlFree(pucUrl);
      }

      if (xmlStrlen(&pucArg[ovector[1]]) > 0) {
	PrintFormatLog(3, "URL post '%s' (%i..%i) in '%s'", &pucArg[ovector[1]], ovector[1], ovector[1] + xmlStrlen(&pucArg[ovector[1]]), pucArg);
	/* the content ends with text, recursion */
	pndPostfix = SplitTupelToLinkNodesMd(&pucArg[ovector[1]]);
	if (pndPostfix) {
	  xmlNodePtr pndT = pndPostfix->children;
	  domUnlinkNodeList(pndT);
	  xmlAddChildList(pndResult, pndT);
	  xmlFreeNode(pndPostfix);
	}
	else {
	  xmlChar *pucT = xmlStrdup(&pucArg[ovector[1]]);
	  xmlAddChild(pndResult, xmlNewText(pucT));
	  xmlFree(pucT);
	}
      }
    }

    pcre2_match_data_free(match_data_link);   /* Release memory used for the match */
  }
  return pndResult;
} /* end of SplitTupelToLinkNodesMd() */


/*! splits an UTF-8 string into a list of text and link element nodes
*/
xmlNodePtr
SplitStringToLinkNodes(const xmlChar *pucArg)
{
  size_t ducOrigin;
  xmlNodePtr pndResult = NULL;

  if (pucArg != NULL && (ducOrigin = xmlStrlen(pucArg)) > 0) {
    int rc;
    pcre2_match_data *match_data;
    xmlNodePtr pndLink;
    xmlNodePtr pndPostfix;

    match_data = pcre2_match_data_create_from_pattern(re_url, NULL);
    rc = pcre2_match(
      re_url,        /* result of pcre2_compile() */
      (PCRE2_SPTR8)pucArg,  /* the subject string */
      xmlStrlen(pucArg),             /* the length of the subject string */
      0,              /* start at offset 0 in the subject */
      0,              /* default options */
      match_data,        /* vector of integers for substring information */
      NULL);            /* number of elements (NOT size in bytes) */

    if (rc > -1) {
      PCRE2_SIZE *ovector;

      ovector = pcre2_get_ovector_pointer(match_data);
      if (ovector[1] - ovector[0] > 3) {
	/*
	 the regexp match, assemble node list with a common dummy
	 element node
	 */
	xmlChar *pucUrl;
	xmlChar *pucUrlDisplay;
	xmlChar *pucA = (xmlChar *)pucArg + ovector[0];

	/* check if its a relative link */
	if (xmlStrcasestr(pucA, BAD_CAST"file://.") == pucA) {
	  /* there is no information about file context, problems in CGI mode */
	  /* remove non-conformant file:// prefix from relative path */
	  pucUrl = xmlStrndup(pucA + 7, (int)(ovector[1] - ovector[0] - 7));
	}
	else {
	  pucUrl = xmlStrndup(pucA, (int)(ovector[1] - ovector[0]));
	}

	PrintFormatLog(3, "URL '%s' (%i..%i) in '%s'", pucUrl, ovector[0], ovector[1], pucArg);

	pndResult = xmlNewNode(NULL, BAD_CAST "dummy");

	if (ovector[0] > 0) {
	  /* the content starts with text	*/
	  xmlChar *pucT = xmlStrndup(pucArg, (int)ovector[0]);
	  xmlAddChild(pndResult, xmlNewText(pucT));
	  xmlFree(pucT);
	}

	if (xmlStrcasestr(pucUrl, BAD_CAST"mailto:") == pucUrl) {
	  pucUrlDisplay = xmlStrdup(&pucUrl[7]);
	}
	else {
	  pucUrlDisplay = xmlStrdup(pucUrl);
	}

	if (STR_IS_NOT_EMPTY(pucUrlDisplay)) {
	  /*! Percent-encode non-ASCII chars (s. https://en.wikipedia.org/wiki/Percent-encoding) */
	  DecodeRFC1738((char *)pucUrlDisplay);
	  if (xmlCheckUTF8(pucUrlDisplay)) {
	    /* OK */
	    pndLink = xmlNewTextChild(pndResult, NULL, NAME_PIE_LINK, pucUrlDisplay);
	  }
	  else {
	    pndLink = xmlNewTextChild(pndResult, NULL, NAME_PIE_LINK, pucUrl);
	  }
	}
	else {
	  pndLink = xmlNewTextChild(pndResult, NULL, NAME_PIE_LINK, pucUrl);
	}
	xmlSetProp(pndLink, BAD_CAST "href", pucUrl);

	if (ducOrigin > ovector[1]) {
	  /* the content ends with text, recursion */
	  pndPostfix = SplitStringToLinkNodes(pucArg + ovector[1]);
	  if (pndPostfix) {
	    xmlNodePtr pndT = pndPostfix->children;
	    domUnlinkNodeList(pndT);
	    xmlAddChildList(pndResult, pndT);
	    xmlFreeNode(pndPostfix);
	  }
	  else {
	    xmlChar *pucT = xmlStrdup(pucArg + ovector[1]);
	    xmlAddChild(pndResult, xmlNewText(pucT));
	    xmlFree(pucT);
	  }
	}

	xmlFree(pucUrlDisplay);
	xmlFree(pucUrl);
      }
    }

    pcre2_match_data_free(match_data);   /* Release memory used for the match */
  }
  return pndResult;
}
/* end of SplitStringToLinkNodes() */


/*! splits an UTF-8 string into a list of text and link element nodes
*/
xmlNodePtr
SplitStringToAutoLinkNodes(const xmlChar *pucArg)
{
  size_t ducOrigin;
  xmlNodePtr pndResult = NULL;

  if (pucArg != NULL && (ducOrigin = xmlStrlen(pucArg)) > 0) {
    int rc;
    pcre2_match_data *match_data;
    xmlNodePtr pndLink;
    xmlNodePtr pndPostfix;

    match_data = pcre2_match_data_create_from_pattern(re_url, NULL);
    rc = pcre2_match(
      re_link_md_auto,        /* result of pcre2_compile() */
      (PCRE2_SPTR8)pucArg,  /* the subject string */
      xmlStrlen(pucArg),             /* the length of the subject string */
      0,              /* start at offset 0 in the subject */
      0,              /* default options */
      match_data,        /* vector of integers for substring information */
      NULL);            /* number of elements (NOT size in bytes) */

    if (rc > -1) {
      PCRE2_SIZE *ovector;

      assert(pcre2_get_ovector_count(match_data) == 5);

      ovector = pcre2_get_ovector_pointer(match_data);
      if (ovector[1] - ovector[0] > 3) {
	/*
	 the regexp match, assemble node list with a common dummy
	 element node
	 */
	xmlChar *pucUrl;
	xmlChar *pucUrlDisplay;
	index_t i0, i1;

	pndResult = xmlNewNode(NULL, BAD_CAST "dummy");

	if (ovector[0] > 0) {
	  /* the content starts with text	*/
	  xmlChar *pucT = xmlStrndup(pucArg, (int)ovector[0]);
	  xmlAddChild(pndResult, xmlNewText(pucT));
	  xmlFree(pucT);
	}

	i0 = (index_t)ovector[0];
	if (pucArg[i0] == '<') {
	  i0++;
	}
	else if (pucArg[i0] == (xmlChar)'\xE2' && pucArg[i0+1] == (xmlChar)'\x80' && pucArg[i0+2] == (xmlChar)'\x99') {
	  /*!\bug if different typographic chars are used s. pieElementReplaceCharMarkup() */
	  i0 += 3;
	}
	else if (pucArg[i0] == '&' && pucArg[i0+1] == 'l' && pucArg[i0+2] == 't' && pucArg[i0+3] == ';') {
	  i0 += 4;
	}

	i1 = (index_t)ovector[1];
	if (i1 > 1 && pucArg[i1-1] == '>') {
	  i1 -= 1;
	}
	else if (i1 > 3 && pucArg[i1-3] == (xmlChar)'\xE2' && pucArg[i1-2] == (xmlChar)'\x80' && pucArg[i1-1] == (xmlChar)'\x98') {
	  /*!\bug see above */
	  i1 -= 3;
	}
	else if (i1 > 4 && pucArg[i1-4] == '&' && pucArg[i1-3] == 'g' && pucArg[i1-2] == 't' && pucArg[i1-1] == ';') {
	  i1 -= 4;
	}

	pucUrl = xmlStrndup(BAD_CAST pucArg + i0, i1 - i0);
	PrintFormatLog(3, "URL '%s' (%i..%i) in '%s'", pucUrl, ovector[0], ovector[1], pucArg);

	if (StringBeginsWith((char *)pucUrl, "mailto:")) {
	  pucUrlDisplay = xmlStrdup(&pucUrl[7]);
	}
	else {
	  pucUrlDisplay = xmlStrdup(pucUrl);
	}

	if (STR_IS_NOT_EMPTY(pucUrlDisplay)) {
	  /*! Percent-encode non-ASCII chars (s. https://en.wikipedia.org/wiki/Percent-encoding) */
	  DecodeRFC1738((char *)pucUrlDisplay);
	  if (xmlCheckUTF8(pucUrlDisplay)) {
	    /* OK */
	    pndLink = xmlNewTextChild(pndResult, NULL, NAME_PIE_LINK, pucUrlDisplay);
	  }
	  else {
	    pndLink = xmlNewTextChild(pndResult, NULL, NAME_PIE_LINK, pucUrl);
	  }
	}
	else {
	  pndLink = xmlNewTextChild(pndResult, NULL, NAME_PIE_LINK, pucUrl);
	}
	xmlSetProp(pndLink, BAD_CAST "href", pucUrl);

	if (ducOrigin > ovector[1]) {
	  /* the content ends with text, recursion */
	  pndPostfix = SplitStringToAutoLinkNodes(pucArg + ovector[1]);
	  if (pndPostfix) {
	    xmlNodePtr pndT = pndPostfix->children;
	    domUnlinkNodeList(pndT);
	    xmlAddChildList(pndResult, pndT);
	    xmlFreeNode(pndPostfix);
	  }
	  else {
	    xmlChar *pucT = xmlStrdup(pucArg + ovector[1]);
	    xmlAddChild(pndResult, xmlNewText(pucT));
	    xmlFree(pucT);
	  }
	}

	xmlFree(pucUrlDisplay);
	xmlFree(pucUrl);
      }
    }

    pcre2_match_data_free(match_data);   /* Release memory used for the match */
  }
  return pndResult;
}
/* end of SplitStringToAutoLinkNodes() */


/*! derive a sequence of text and LINK nodes from node

  find all URLs with AND without "| | |" markup
 */
xmlNodePtr
RecognizeUrls(xmlNodePtr pndArg)
{
  if (IS_NODE_META(pndArg) || IS_NODE_PIE_PRE(pndArg) || IS_NODE_PIE_TT(pndArg) || IS_NODE_PIE_TABLE(pndArg) || IS_NODE_PIE_LINK(pndArg)) {
    /* skip */
  }
  else if (IS_ENODE(pndArg) && (pndArg->ns==NULL || pndArg->ns==pnsPie)) {
    xmlNodePtr pndChild;

    for (pndChild = pndArg->children;
	 pndChild != NULL;
	 pndChild= (pndChild != NULL) ? pndChild->next : NULL) {

      if (xmlNodeIsText(pndChild)) { /* pndChild is a text node */
	xmlChar *pucRelease = pndChild->content;
	xmlNodePtr pndReplace;

#ifdef LEGACY
	pucRelease = TranslateUncToUrl(pndChild->content);
#endif

	if ((pndReplace = SplitStringToAutoLinkNodes(pucRelease))) {
	  RecognizeUrls(pndReplace);
	}
	else if ((pndReplace = SplitTupelToLinkNodesMd(pucRelease))) {
	  RecognizeUrls(pndReplace);
	}
#ifdef LEGACY
	else if ((pndReplace = SplitTupelToLinkNodes(pucRelease))) {
	  RecognizeUrls(pndReplace);
	}
#endif
	else if ((pndReplace = SplitStringToLinkNodes(pucRelease))) {
	}

	if (pndReplace == NULL) {
	  /* nothing found */
	}
	else if (IS_NODE_PIE_LINK(pndArg)) {
	  if (domGetPropValuePtr(pndArg,BAD_CAST"href")==NULL) {
	    /* URL inside a link with no href attribute */
	    xmlNodePtr pndFirstLink = domGetFirstChild(pndReplace,NAME_PIE_LINK);
	    assert(pndFirstLink != NULL);
	    xmlSetProp(pndArg, BAD_CAST "href", domGetPropValuePtr(pndFirstLink,BAD_CAST"href"));
	  }
	  xmlFreeNodeList(pndReplace);
	}
	else if (IS_NODE_PIE_PAR(pndArg) && pndArg->children == pndChild && pndArg->children == pndArg->last
	  && IS_NODE_PIE_IMG(pndReplace->children) && pndReplace->children == pndReplace->last) {
	  /*! 
	  use an image link as figure element if it's the only child of pndArg
	  */
	  xmlNodePtr pndT = pndReplace->children;

	  xmlNodeSetName(pndArg, NAME_PIE_FIG);
	  xmlNodeSetContent(pndArg,NULL);
	  xmlNewChild(pndArg, NULL, NAME_PIE_HEADER, domGetPropValuePtr(pndT, BAD_CAST"title"));
	  xmlUnlinkNode(pndT);
	  xmlAddChild(pndArg, pndT);
	  xmlFreeNode(pndReplace);
	  xmlNewTextChild(pndArg, NULL, NAME_PIE_TTAG, BAD_CAST"#fig");
	  pndChild = NULL; /* end the loop */
	}
	else {
	  /* there is a result list */
	  xmlNodePtr pndT;

	  pndT = pndChild->next;
	  if (domReplaceNodeList(pndChild,pndReplace->children) == pndChild) {
	    xmlFreeNodeList(pndChild);
	  }
	  xmlFreeNode(pndReplace);
	  /*  */
	  if (pndT != NULL && pndT->prev != NULL) {
	    pndChild = pndT->prev;
	  }
	  else {
	    pndChild = NULL;
	  }
	}
#ifdef LEGACY
	xmlFree(pucRelease);
#endif
      }
      else {
	RecognizeUrls(pndChild);
      }
    }
  }
  return NULL;
}
/* End of RecognizeUrls() */


/*! splits an UTF-8 string into a list of script element nodes
*/
xmlNodePtr
SplitStringToScriptNode(const xmlChar *pucArg)
{
  int ducOrigin;
  xmlNodePtr pndResult = NULL;

  if (pucArg != NULL && (ducOrigin = xmlStrlen(pucArg)) > 0) {
    int rc;
    pcre2_match_data *match_data_link;
    xmlNodePtr pndScript;
    xmlNodePtr pndPostfix;

    /* http://www.pcre.org/current/doc/html/pcre2demo.html */

    match_data_link = pcre2_match_data_create_from_pattern(re_script, NULL);
    
    rc = pcre2_match(
	re_script,        /* result of pcre2_compile() */
	(PCRE2_SPTR8)pucArg,  /* the subject string */
	xmlStrlen(pucArg),             /* the length of the subject string */
	0,              /* start at offset 0 in the subject */
	0,              /* default options */
	match_data_link,        /* vector of integers for substring information */
	NULL);            /* number of elements (NOT size in bytes) */

    if (rc > -1) {
      /*
	 the regexp match, assemble node list with a common dummy
	 element node
       */
      xmlChar *pucPre;
      xmlChar *pucScript = NULL;
      PCRE2_SIZE *ovector;
      int i = 0;

      ovector = pcre2_get_ovector_pointer(match_data_link);

      PrintFormatLog(3, "Script (%i..%i) in '%s'", ovector[i], ovector[i+1], pucArg);

      pndResult = xmlNewNode(NULL, BAD_CAST "dummy");

      if (ovector[i*2+1] - ovector[i*2] > 0) {
	if (ovector[i*2] > 0) {
	  pucPre = xmlStrndup(pucArg, (int)ovector[i*2]);
	  PrintFormatLog(3, "Script pre '%s' (%i..%i) in '%s'", pucPre, 0, ovector[0], pucArg);
	  xmlAddChild(pndResult, xmlNewText(pucPre));
	  xmlFree(pucPre);
	}
	else { /* string starts with 'script="' */
	}
      }

      i++;
      if (ovector[i*2+1] - ovector[i*2] > 0) {
	pndScript = xmlNewNode(NULL,NAME_PIE_IMPORT);
	xmlSetProp(pndScript, BAD_CAST "type", BAD_CAST "script");

	pucScript = xmlStrndup(&pucArg[ovector[i*2]], (int)(ovector[i*2+1] - ovector[i*2]));
	xmlAddChild(pndScript,xmlNewText(pucScript));
	xmlAddChild(pndResult,pndScript);
	xmlFree(pucScript);
      }

      if (xmlStrlen(&pucArg[ovector[1]]) > 0) {
	PrintFormatLog(3, "Script post '%s' (%i..%i) in '%s'", &pucArg[ovector[1]], ovector[1], ovector[1] + xmlStrlen(&pucArg[ovector[1]]), pucArg);
	/* the content ends with text, recursion */
	pndPostfix = SplitStringToScriptNode(&pucArg[ovector[1]]);
	if (pndPostfix) {
	  xmlNodePtr pndT = pndPostfix->children;
	  domUnlinkNodeList(pndT);
	  xmlAddChildList(pndResult, pndT);
	  xmlFreeNode(pndPostfix);
	}
	else {
	  xmlChar *pucT = xmlStrdup(&pucArg[ovector[1]]);
	  xmlAddChild(pndResult, xmlNewText(pucT));
	  xmlFree(pucT);
	}
      }
    }

    pcre2_match_data_free(match_data_link);   /* Release memory used for the match */
  }
  return pndResult;
}
/* end of SplitStringToScriptNode() */


/*! derive a sequence of text

  find all Scripts with script="" markup
 */
BOOL_T
RecognizeScripts(xmlNodePtr pndArg)
{
  BOOL_T fResult = FALSE;
  
  if (IS_NODE_META(pndArg) || IS_NODE_PIE_PRE(pndArg) || IS_NODE_PIE_LINK(pndArg) || IS_NODE_PIE_DATE(pndArg)) {
    /* skip */
  }
  else if (IS_ENODE(pndArg) && (pndArg->ns==NULL || pndArg->ns==pnsPie)) {
    xmlNodePtr pndChild;

    for (pndChild = pndArg->children;
	 pndChild != NULL;
	 pndChild= (pndChild != NULL) ? pndChild->next : NULL) {

      if (xmlNodeIsText(pndChild)) { /* pndChild is a text node */
	xmlNodePtr pndReplace;

	pndReplace = SplitStringToScriptNode(pndChild->content);
	if (pndReplace == NULL) {
	  /* nothing found */
	}
	else {
	  /* there is a result list */
	  xmlNodePtr pndT;

	  pndT = pndChild->next;
	  if (domReplaceNodeList(pndChild,pndReplace->children) == pndChild) {
	    xmlFreeNodeList(pndChild);
	  }
	  xmlFreeNode(pndReplace);
	  /*  */
	  if (pndT != NULL && pndT->prev != NULL) {
	    pndChild = pndT->prev;
	  }
	  else {
	    pndChild = NULL;
	  }
	  fResult = TRUE;
	}
      }
      else {
	fResult |= RecognizeScripts(pndChild);
      }
    }
  }
  return fResult;
}
/* End of RecognizeScripts() */


/*! splits an UTF-8 string into a list of text and inline element nodes

\todo nested emphasis
*/
xmlNodePtr
SplitStringToInlineNodes(const xmlChar *pucArg)
{
  size_t ducOrigin;
  xmlNodePtr pndResult = NULL;

  if (pucArg != NULL && (ducOrigin = xmlStrlen(pucArg)) > 0) {
    int rc;
    pcre2_match_data *match_data;
    xmlNodePtr pndIn;
    xmlNodePtr pndPostfix;

    match_data = pcre2_match_data_create_from_pattern(re_inline, NULL);
    rc = pcre2_match(
      re_inline,        /* result of pcre2_compile() */
      (PCRE2_SPTR8)pucArg,  /* the subject string */
      xmlStrlen(pucArg),             /* the length of the subject string */
      0,              /* start at offset 0 in the subject */
      0,              /* default options */
      match_data,        /* vector of integers for substring information */
      NULL);            /* number of elements (NOT size in bytes) */

    /* group matching http://pcre.org/current/doc/html/pcre2pattern.html#SEC23 
    http://pcre.org/current/doc/html/pcre2api.html#SEC33
    */

    //pcre2_get_ovector_count();

    //pcre2_get_startchar();


    if (rc > -1) {
      PCRE2_SIZE *ovector;

      ovector = pcre2_get_ovector_pointer(match_data);
      if (pcre2_get_ovector_count(match_data) == 1 && ovector[1] - ovector[0] > 0) { //
	/*
	the regexp match, assemble node list with a common dummy
	element node
	*/
	index_t i0;
	index_t i1;
	xmlChar *pucIn;

	for (i0=(index_t)ovector[0]; pucArg[i0] == pucArg[ovector[0]]; i0++);
	for (i1=(index_t)ovector[1]; i1 > 0 && pucArg[i1-1] == pucArg[ovector[0]]; i1--);
	pucIn = xmlStrndup((xmlChar *)pucArg + i0, i1 - i0);

	PrintFormatLog(3, "Inline '%s' in '%s'", pucIn, pucArg);

	pndResult = xmlNewNode(NULL, BAD_CAST "dummy");

	if (ovector[0] > 0) {
	  /* the content starts with text	*/
	  xmlChar *pucT = xmlStrndup(pucArg, (int)ovector[0]);
	  xmlAddChild(pndResult, xmlNewText(pucT));
	  xmlFree(pucT);
	}

	pndIn = xmlNewTextChild(pndResult, NULL, 
	  (pucArg[ovector[0]] == (xmlChar)'`') ? NAME_PIE_TT : ((i0 - ovector[0] > 2) ? NAME_PIE_STRONG : NAME_PIE_EM),
	  pucIn);

	if (ducOrigin > ovector[1]) {
	  /* the content ends with text, recursion */
	  pndPostfix = SplitStringToInlineNodes(pucArg + ovector[1]);
	  if (pndPostfix) {
	    xmlNodePtr pndT = pndPostfix->children;
	    domUnlinkNodeList(pndT);
	    xmlAddChildList(pndResult, pndT);
	    xmlFreeNode(pndPostfix);
	  }
	  else {
	    xmlChar *pucT = xmlStrdup(pucArg + ovector[1]);
	    xmlAddChild(pndResult, xmlNewText(pucT));
	    xmlFree(pucT);
	  }
	}
	xmlFree(pucIn);
      }
    }
    pcre2_match_data_free(match_data);   /* Release memory used for the match */
  }
  return pndResult;
}
/* end of SplitStringToInlineNodes() */


/*! splits an UTF-8 string into a list of text and date element nodes
*/
xmlNodePtr
SplitStringToDateNodes(const xmlChar *pucArg, RN_MIME_TYPE eMimeTypeArg)
{
  size_t ducOrigin;
  xmlNodePtr pndResult = NULL;

  if (pucArg != NULL && (ducOrigin = xmlStrlen(pucArg)) > 0) {
    int rc;
    pcre2_match_data *match_data;
    xmlNodePtr pndIn;
    xmlNodePtr pndPostfix;

    match_data = pcre2_match_data_create_from_pattern(re_date, NULL);
    rc = pcre2_match(
      re_date,        /* result of pcre2_compile() */
      (PCRE2_SPTR8)pucArg,  /* the subject string */
      xmlStrlen(pucArg),             /* the length of the subject string */
      0,              /* start at offset 0 in the subject */
      0,              /* default options */
      match_data,        /* vector of integers for substring information */
      NULL);            /* number of elements (NOT size in bytes) */

    if (rc > -1) {
      PCRE2_SIZE *ovector;

      ovector = pcre2_get_ovector_pointer(match_data);
      if (ovector[3] - ovector[2] > 0) {
	/*
	the regexp match, assemble node list with a common dummy
	element node
	*/
	xmlChar *pucDate;
	xmlChar *pucA = (xmlChar *)pucArg + ovector[2];

	if ((pucDate = xmlStrndup(pucA, (int)(ovector[3] - ovector[2]))) != NULL) {

	  PrintFormatLog(3, "Date '%s' (%i..%i) in '%s'", pucDate, ovector[2], ovector[3], pucArg);

	  pndResult = xmlNewNode(NULL, BAD_CAST "dummy");

	  if (ovector[0] > 0) {
	    /* the content starts with text	*/
	    xmlChar *pucT = xmlStrndup(pucArg, (int)ovector[0]);
	    xmlAddChild(pndResult, xmlNewText(pucT));
	    xmlFree(pucT);
	  }

	  if (eMimeTypeArg == MIME_TEXT_PLAIN_CALENDAR) {
	    pndIn = xmlNewChild(pndResult, NULL, NAME_PIE_DATE, NULL);
	    xmlSetProp(pndIn,BAD_CAST"ref",pucDate);
	  }
	  else {
	    pndIn = xmlNewTextChild(pndResult, NULL, NAME_PIE_DATE, pucDate);
	  }
	}

	if (ducOrigin > ovector[1]) {
	  /* the content ends with text, recursion */

	  if (eMimeTypeArg == MIME_TEXT_PLAIN_CALENDAR) {
	    while (*(pucArg + ovector[1]) == (xmlChar)' ') ovector[1]++;
	    if (*(pucArg + ovector[1]) == (xmlChar)'+') {
	      ovector[1]++;
	      xmlSetProp(pndIn,BAD_CAST"holiday",BAD_CAST"yes");
	      while (*(pucArg + ovector[1]) == (xmlChar)' ') ovector[1]++;
	    }
	  }

	  pndPostfix = SplitStringToDateNodes(pucArg + ovector[1], eMimeTypeArg);
	  if (pndPostfix) {
	    xmlNodePtr pndT = pndPostfix->children;
	    domUnlinkNodeList(pndT);
	    xmlAddChildList(pndResult, pndT);
	    xmlFreeNode(pndPostfix);
	  }
	  else {
	    xmlChar *pucT = xmlStrdup(pucArg + ovector[1]);
	    xmlAddChild(pndResult, xmlNewText(pucT));
	    xmlFree(pucT);
	  }
	}
	xmlFree(pucDate);
      }
    }
    pcre2_match_data_free(match_data);   /* Release memory used for the match */
  }
  return pndResult;
}
/* end of SplitStringToDateNodes() */


/*! \return number of matches
*/
int
GetNumberOfMatches(xmlNodePtr pndArg, xmlChar *pucArgPattern)
{
  int iResult = 0;
  xmlChar *pucT;

  assert(xmlNodeIsText(pndArg));

  for (pucT = domNodeGetContentPtr(pndArg); pucT != NULL; pucT = BAD_CAST xmlStrcasestr(pucT, pucArgPattern)) {
    /* pattern found */
    iResult++;
  }

  return iResult;
} /* End of GetNumberOfMatches() */


/*! unlinks all element trees containing attribute valid="no" or has no attribute "w"

\todo re-implement using XPath?
*/
xmlNodePtr
CleanUpTree(xmlNodePtr pndArg)
{
  if (IS_ENODE(pndArg) == FALSE) {
  }
  else if (IS_VALID_NODE(pndArg) == FALSE) {
    xmlUnlinkNode(pndArg);
    xmlFreeNode(pndArg);
  }
  else if (IS_NODE_PIE_META(pndArg) || IS_NODE_PIE_TTAG(pndArg)) {
    /* to be ignored */
  }
  else if (xmlHasProp(pndArg, BAD_CAST"w")) {
    xmlNodePtr pndChild;
    xmlNodePtr pndNext = NULL;

    if (IS_NODE_PIE_SECTION(pndArg) || IS_NODE_PIE_LIST(pndArg) || IS_NODE_PIE_TABLE(pndArg) || IS_NODE_PIE_FIG(pndArg)) { /* keep all descendants, if they have no attribute "w" */
      BOOL_T fT;
      
      for (fT = TRUE, pndChild = pndArg->children; fT && pndChild != NULL; pndChild = pndChild->next) {
	fT = (xmlHasProp(pndChild, BAD_CAST"w") == NULL);
      }

      if (fT) {
	/* none of the childs have attribute "w" */
      }
      else {
	for (pndChild = pndArg->children; pndChild != NULL; pndChild = pndNext) {
	  pndNext = pndChild->next;
	  CleanUpTree(pndChild);
	}
      }
      xmlUnsetProp(pndChild, BAD_CAST"w");
    }
    else if (IS_NODE_PIE_HEADER(pndArg)
      || IS_NODE_PIE_PAR(pndArg)
      || IS_NODE_PIE_TASK(pndArg)
      || IS_NODE_PIE_TH(pndArg)
      || IS_NODE_PIE_TR(pndArg)
#ifdef HAVE_PETRINET
      || IS_NODE_PKG2_STATE(pndArg)
      || IS_NODE_PKG2_TRANSITION(pndArg)
      || IS_NODE_PKG2_REQUIREMENT(pndArg)
#endif
      ) {
      /* keep all descendants */
      for (pndChild = pndArg->children; pndChild != NULL; pndChild = pndChild->next) {
	xmlUnsetProp(pndChild, BAD_CAST"w");
      }
    }
    else {
      for (pndChild = pndArg->children; pndChild != NULL; pndChild = pndNext) {
	pndNext = pndChild->next;
	CleanUpTree(pndChild);
      }
    }
    xmlUnsetProp(pndArg, BAD_CAST"w");
  }
  else if (IS_NODE_PIE_HEADER(pndArg) && xmlHasProp(pndArg->parent, BAD_CAST"w") != NULL) {
    /* keep header if parent section has @w */
  }
  else {
    xmlUnlinkNode(pndArg);
    xmlFreeNode(pndArg);
  }
  return NULL;
} /* end of CleanUpTree() */


/*! substitutions with accurate UTF-8/XML strings

https://en.wikipedia.org/wiki/UTF-8
https://www.duden.de/sprachwissen/rechtschreibregeln/anfuehrungszeichen

*/
xmlChar *
StringDecodeCharMarkupNew(xmlChar *pucArg, lang_t eLangArg)
{
  xmlChar* pucResult = NULL;
  
  if (STR_IS_NOT_EMPTY(pucArg)) {
    int i, k, l;

    pucResult = BAD_CAST xmlMalloc(xmlStrlen(pucArg) * 2);

    for (k=i=0; pucArg[i]; ) {
      int iCode;

      iCode = -1;
      l = 0;

      if ((pucArg[i + l] == (xmlChar)'<' && (++l))
	|| (pucArg[i + l] == (xmlChar)'&' && pucArg[i + l + 1] == (xmlChar)'l' && pucArg[i + l + 2] == (xmlChar)'t' && pucArg[i + l + 3] == (xmlChar)';' && (l += 4))) {

	if (pucArg[i + l] == (xmlChar)'=' && (++l)) {

	  if ((pucArg[i + l] == (xmlChar)'>' && (++l))
	    || (pucArg[i + l] == (xmlChar)'&' && pucArg[i + l + 1] == (xmlChar)'g' && pucArg[i + l + 2] == (xmlChar)'t' && pucArg[i + l + 3] == (xmlChar)';' && (l += 4))) { // STR_UTF8_LEFT_RIGHT_SINGLE_ARROW
	    iCode = 0x21D4;
	  }
	  else { // STR_UTF8_LEFTWARDS_SINGLE_ARROW
	    iCode = 0x21D0;
	  }
	}
	else if (pucArg[i + l] == (xmlChar)'-' && (++l)) {

	  if ((pucArg[i + l] == (xmlChar)'>' && (++l))
	    || (pucArg[i + l] == (xmlChar)'&' && pucArg[i + l + 1] == (xmlChar)'g' && pucArg[i + l + 2] == (xmlChar)'t' && pucArg[i + l + 3] == (xmlChar)';' && (l += 4))) { // STR_UTF8_LEFT_RIGHT_ARROW
	    iCode = 0x2194;
	  }
	  else { // STR_UTF8_LEFTWARDS_ARROW
	    iCode = 0x2190;
	  }
	}
	else if ((pucArg[i + l] == (xmlChar)'<' && (++l)) 
	|| (pucArg[i + l] == (xmlChar)'&' && pucArg[i + l + 1] == (xmlChar)'l' && pucArg[i + l + 2] == (xmlChar)'t' && pucArg[i + l + 3] == (xmlChar)';' && (l += 4))) {

	  switch (eLangArg) {
	  case LANG_DE: // STR_UTF8_LEFT_DOUBLE_QUOTATION_MARK
	    iCode = 0x201C;
	    break;
	  case LANG_FR: //  STR_UTF8_RIGHT_POINTING_DOUBLE_ANGLE_QUOTATIONMARK
	    iCode = 0x00BB;
	    break;
	  default:		/* is LANG_DEFAULT, STR_UTF8_RIGHT_DOUBLE_QUOTATION_MARK */
	    iCode = 0x201D;
	  }
	}
      }
      else if ((pucArg[i + l] == (xmlChar)'>' && (++l))
	|| (pucArg[i + l] == (xmlChar)'&' && pucArg[i + l + 1] == (xmlChar)'g' && pucArg[i + l + 2] == (xmlChar)'t' && pucArg[i + l + 3] == (xmlChar)';' && (l += 4))) {

	if ((pucArg[i + l] == (xmlChar)'>' && (++l))
	  || (pucArg[i + l] == (xmlChar)'&' && pucArg[i + l + 1] == (xmlChar)'g' && pucArg[i + l + 2] == (xmlChar)'t' && pucArg[i + l + 3] == (xmlChar)';' && (l += 4))) {

	  switch (eLangArg) {
	  case LANG_DE: // STR_UTF8_DOUBLE_LOW_9_QUOTATION_MARK
	    iCode = 0x201E;
	    break;
	  case LANG_FR: // STR_UTF8_LEFT_POINTING_DOUBLE_ANGLE_QUOTATIONMARK
	    iCode = 0x00AB;
	    break;
	  default:		/* is LANG_DEFAULT, STR_UTF8_LEFT_DOUBLE_QUOTATION_MARK */
	    iCode = 0x201C;
	  }
	}
      }
      else if (pucArg[i + l] == (xmlChar)'=' && (++l)) {

	if ((pucArg[i + l] == (xmlChar)'>' && (++l))
	  || (pucArg[i + l] == (xmlChar)'&' && pucArg[i + l + 1] == (xmlChar)'g' && pucArg[i + l + 2] == (xmlChar)'t' && pucArg[i + l + 3] == (xmlChar)';' && (l += 4))) { // STR_UTF8_RIGHTWARDS_SINGLE_ARROW

	  iCode = 0x21D2;
	}
      }
      else if (pucArg[i + l] == (xmlChar)'-' && (++l)) {

	if ((pucArg[i + l] == (xmlChar)'>' && (++l))
	  || (pucArg[i + l] == (xmlChar)'&' && pucArg[i + l + 1] == (xmlChar)'g' && pucArg[i + l + 2] == (xmlChar)'t' && pucArg[i + l + 3] == (xmlChar)';' && (l += 4))) { // STR_UTF8_RIGHTWARDS_ARROW

	  iCode = 0x2192;
	}
	else if (pucArg[i + l] == (xmlChar)'-' && (++l)) {

	  if (pucArg[i + l] == (xmlChar)'-' && (++l)) { // STR_UTF8_EM_DASH
	    iCode = 0x2014;
	  }
	  else { // STR_UTF8_EN_DASH
	    iCode = 0x2013;
	  }
	}
      }

      if (iCode > -1 && l > 0) {
	/* numeric character reference detected */
	int j;

	j = xmlCopyCharMultiByte(&pucResult[k], iCode);
	assert(j > 0 && j < 8);
	//assert(j <= l);

	k += j;
	i += l;
      }
      else {
	pucResult[k] = pucArg[i];
	k++;
	i++;
      }
    }
    pucResult[k] = (xmlChar)'\0';
  }
  return pucResult;
} /* end of StringDecodeCharMarkupNew() */


/*! 
*/
xmlNodePtr
RecognizeSymbols(xmlNodePtr pndArg, lang_t eLangArg)
{
  xmlNodePtr pndResult = NULL;

  if (pndArg) {
    pndResult = pndArg->next;
    if (IS_NODE_META(pndArg) || IS_NODE_PIE_PRE(pndArg) || IS_NODE_PIE_TT(pndArg) || IS_NODE_PIE_LINK(pndArg) || IS_NODE_PIE_DATE(pndArg) || IS_NODE_SCRIPT(pndArg)) {
      /* skip */
    }
    else if (IS_NODE_PIE_ETAG(pndArg) || IS_NODE_PIE_HTAG(pndArg) || IS_NODE_PIE_TTAG(pndArg) || IS_NODE_PIE_DATE(pndArg)) {
      /* skip existing tag elements */
    }
    else if (xmlNodeIsText(pndArg)) {
      /* pndChild is a text node */
      xmlChar* pucT = NULL;
      xmlChar* pucTT = NULL;

      if ((pucT = StringDecodeNumericCharsNew(pndArg->content)) != NULL
	  && (pucTT = StringDecodeCharMarkupNew(pucT, eLangArg)) != NULL) {

	xmlNodeSetContent(pndArg,pucTT);
      }
      xmlFree(pucTT);
      xmlFree(pucT);
    }
    else if (IS_ENODE(pndArg) && (pndArg->ns==NULL || pndArg->ns==pnsPie)) {
      xmlNodePtr pndChild;

      for (pndChild = pndArg->children; pndChild; pndChild = RecognizeSymbols(pndChild, eLangArg));
    }
  }
  return pndResult;
} /* End of RecognizeSymbols() */


/*! derive a sequence of text and Date nodes from node

https://spec.commonmark.org/0.29/#inlines
*/
xmlNodePtr
RecognizeInlines(xmlNodePtr pndArg)
{
  xmlNodePtr pndResult = NULL;

  if (pndArg) {
    pndResult = pndArg->next;
    if (IS_NODE_META(pndArg) || IS_NODE_PIE_PRE(pndArg) || IS_NODE_PIE_LINK(pndArg) || IS_NODE_PIE_DATE(pndArg) || IS_NODE_SCRIPT(pndArg)) {
      /* skip */
    }
    else if (IS_NODE_PIE_ETAG(pndArg) || IS_NODE_PIE_HTAG(pndArg) || IS_NODE_PIE_TTAG(pndArg) || IS_NODE_PIE_DATE(pndArg)) {
      /* skip existing tag elements */
    }
    else if (xmlNodeIsText(pndArg)) {
      /* pndChild is a text node */
      xmlNodePtr pndReplace;

      pndReplace = SplitStringToInlineNodes(pndArg->content);
      if (pndReplace) {
	/* there is a result list */
	if (domReplaceNodeList(pndArg, pndReplace->children) == pndArg) {
	  xmlFreeNodeList(pndArg);
	}
	/*  */
	xmlFreeNodeList(pndReplace);
      }
    }
    else if (IS_ENODE(pndArg) && (pndArg->ns==NULL || pndArg->ns==pnsPie)) {
      xmlNodePtr pndChild;

      for (pndChild = pndArg->children; pndChild; pndChild = RecognizeInlines(pndChild));
    }
  }
  return pndResult;
} /* End of RecognizeInlines() */


/*! derive a sequence of text and Date nodes from node
*/
xmlNodePtr
RecognizeDates(xmlNodePtr pndArg, RN_MIME_TYPE eMimeTypeArg)
{
  xmlNodePtr pndResult = NULL;

  if (pndArg) {
    pndResult = pndArg->next;
    if (IS_NODE_META(pndArg) || IS_NODE_PIE_PRE(pndArg) || IS_NODE_PIE_LINK(pndArg) || IS_NODE_PIE_DATE(pndArg) || IS_NODE_SCRIPT(pndArg)) {
      /* skip */
    }
    else if (IS_NODE_PIE_ETAG(pndArg) || IS_NODE_PIE_HTAG(pndArg) || IS_NODE_PIE_TTAG(pndArg)) {
      /* skip existing tag elements */
    }
    else if (xmlNodeIsText(pndArg)) {
      /* pndChild is a text node */
      xmlNodePtr pndReplace;

      pndReplace = SplitStringToDateNodes(pndArg->content, eMimeTypeArg);
      if (pndReplace) {
	/* there is a result list */
	if (domReplaceNodeList(pndArg, pndReplace->children) == pndArg) {
	  xmlFreeNodeList(pndArg);
	}
	/*  */
	xmlFreeNodeList(pndReplace);
      }
    }
    else if (IS_ENODE(pndArg) && (pndArg->ns==NULL || pndArg->ns==pnsPie)) {
      xmlChar* pucExt;
      xmlNodePtr pndChild;
      RN_MIME_TYPE eMimeTypeHere = eMimeTypeArg;

      if (IS_NODE_PIE_BLOCK(pndArg)
	&& (pndArg->ns == NULL || pndArg->ns == pnsPie)
	&& ((pucExt = resPathGetExtension(domGetPropValuePtr(pndArg, BAD_CAST"name"))) != NULL
	  || (pucExt = resPathGetExtension(domGetPropValuePtr(pndArg, BAD_CAST"context"))) != NULL)) {
	eMimeTypeHere = resMimeGetTypeFromExt(pucExt);
	xmlFree(pucExt);
      }

      for (pndChild = pndArg->children; pndChild; pndChild = RecognizeDates(pndChild, eMimeTypeHere));
    }
  }
  return pndResult;
} /* End of RecognizeDates() */


/*! splits an UTF-8 string into a list of script element nodes
derive
- a task/h node from node if content starts with "TODO:" or "DONE:"
- a p/@date if paragraph contains a date
*/
xmlNodePtr
TaskNodeNew(xmlNodePtr pndArg)
{
  xmlNodePtr pndResult = NULL;
  xmlChar* pucContent = NULL;

  if (IS_NODE_PIE_PAR(pndArg) && 
    (((xmlNodeIsText(pndArg->children) && (pucContent = pndArg->children->content) != NULL))
    || (IS_NODE_PIE_LINK(pndArg->children) && pndArg->children == pndArg->last && pndArg->children->children != NULL && (pucContent = pndArg->children->children->content) != NULL))) {
    int rc;
    pcre2_match_data *match_data;

    match_data = pcre2_match_data_create_from_pattern(re_task, NULL);
    rc = pcre2_match(
      re_task,        /* result of pcre2_compile() */
      (PCRE2_SPTR8)pucContent,  /* the subject string */
      xmlStrlen(pucContent),             /* the length of the subject string */
      0,              /* start at offset 0 in the subject */
      PCRE2_ANCHORED,              /* default options */
      match_data,        /* vector of integers for substring information */
      NULL);            /* number of elements (NOT size in bytes) */

    if (rc > -1) { /* pndChild is a text node beginning with according markup */
      xmlNodePtr pndT;
      xmlNodePtr pndTT;
      xmlNodePtr pndHeader;
      PCRE2_SIZE* ovector;

      PrintFormatLog(3, "Found '%s'", pucContent);

      ovector = pcre2_get_ovector_pointer(match_data);

      pndResult = xmlNewNode(NULL, NAME_PIE_TASK);
      pndHeader = xmlCopyNode(pndArg, 1); /* copy all child elements 'link' etc */
      if (pndHeader != NULL && pndHeader->children != NULL) {
	xmlChar* pucT;

	xmlNodeSetName(pndHeader, NAME_PIE_HEADER);
	xmlAddChild(pndResult, pndHeader);

	/*! remove leading markup
	*/
	assert(pndHeader != NULL);
	assert(pndHeader->children != NULL);

	/* pndHeader can have multiple childs */

	for (pndT = pndHeader->children; pndT; pndT = pndTT) { /* relocate all list childs to new task node */
	  pndTT = pndT->next;
	  if (IS_NODE_PIE_LIST(pndT)) {
	    xmlUnlinkNode(pndT);
	    xmlAddChild(pndResult, pndT);
	  }
	}

	if (pndHeader->children == pndHeader->last && xmlNodeIsText(pndHeader->children)) { /* header is a single string */
	  if (xmlStrlen(&pucContent[ovector[1]]) > 0) {
	    xmlFree(pndHeader->children->content);
	    pndHeader->children->content = xmlStrdup(&pucContent[ovector[1]]); /* without leading markup */
	  }
	}
	else if (pndHeader->children == pndHeader->last && IS_NODE_PIE_LINK(pndHeader->children)) { /* header is a single link node */
	  if (pndHeader->children->children != NULL && pndHeader->children->children->content != NULL) {
	    if (xmlStrlen(&pucContent[ovector[1]]) > 0) {
	      xmlFree(pndHeader->children->children->content);
	      pndHeader->children->children->content = xmlStrdup(&pucContent[ovector[1]]); /* without leading markup */
	    }
	  }
	}
	else if (xmlStrlen(&pucContent[ovector[1]]) > 0) {
	  xmlFree(pndHeader->children->content);
	  pndHeader->children->content = xmlStrdup(&pucContent[ovector[1]]); /* without leading markup */
	}
	else {
	  assert(pndHeader->children->content != NULL);
	  pndHeader->children->content[0] = (xmlChar)'\0';
	}

	/*! relocate properties from header to task
	*/
	if (pndHeader->properties) {
	  xmlAttrPtr patT;

	  for (patT = pndResult->properties = pndHeader->properties; patT; patT = patT->next) {
	    patT->parent = pndResult;
	  }
	  pndHeader->properties = NULL;
	}

	/*! derive class attribute from markup
	*/
	pucT = xmlStrndup(pucContent, (int)ovector[3]);
	if (pucT) {
	  xmlChar* pucTTT;
	  
	  pucTTT = xmlNodeListGetString(pndArg->doc,pndArg->children,0);
	  StringToLower((char*)pucT);
	  if (xmlStrEqual(pucT, BAD_CAST"done")
	      || (xmlStrEqual(pucT, BAD_CAST"todo") && xmlStrcasestr(pucTTT, BAD_CAST STR_PIE_OK) != NULL)) {
	    /* map 'DONE:' to a @class = 'todo' and @state='done' */
	    xmlSetProp(pndResult, BAD_CAST"class", BAD_CAST"todo");
	    xmlSetProp(pndResult, BAD_CAST"state", BAD_CAST"done");
	    xmlNewTextChild(pndResult, NULL, NAME_PIE_TTAG, BAD_CAST"#done");
	  }
	  else {
	    xmlChar* pucTT;

	    xmlSetProp(pndResult, BAD_CAST"class", pucT);
	    if (xmlStrcasestr(pucTTT, BAD_CAST STR_PIE_CANCEL) != NULL) {
	      xmlSetProp(pndResult, BAD_CAST"state", BAD_CAST"rejected");
	      xmlNewTextChild(pndResult, NULL, NAME_PIE_TTAG, BAD_CAST"#rejected");
	    }
	    else if (xmlStrcasestr(pucTTT, BAD_CAST STR_PIE_OK) != NULL) {
	      xmlSetProp(pndResult, BAD_CAST"state", BAD_CAST"done");
	      xmlNewTextChild(pndResult, NULL, NAME_PIE_TTAG, BAD_CAST"#done");
	    }
	    pucTT = xmlStrdup(BAD_CAST"#");
	    pucTT = xmlStrcat(pucTT, pucT);
	    xmlNewTextChild(pndResult, NULL, NAME_PIE_TTAG, pucTT);
	    xmlFree(pucTT);
	  }
	  xmlFree(pucTTT);
	  xmlFree(pucT);
	}
      }
      xmlNewTextChild(pndResult, NULL, NAME_PIE_TTAG, BAD_CAST"#task");
    }
    pcre2_match_data_free(match_data);   /* Release memory used for the match */
  }
  return pndResult;
} /* End of TaskNodeNew() */


/*!\return pointer to next node
 */
xmlNodePtr
RecognizeTasks(xmlNodePtr pndArg)
{
  xmlNodePtr pndResult = NULL;

  if (pndArg) {
    pndResult = pndArg->next;
    if (pndArg->ns != NULL && pndArg->ns != pnsPie) {
      /* skip nodes from other namespaces */
    }
    else if (IS_ENODE(pndArg)) {
      xmlNodePtr pndChild;
      xmlNodePtr pndTask;

      if ((pndTask = TaskNodeNew(pndArg)) != NULL) {
	xmlNodePtr pndList;

	/*! detect if following sibling is a list element
	*/
	if ((pndList = domGetFirstChild(pndTask, NAME_PIE_LIST)) != NULL) {
	  /* there is an list of details already */
	}
	else if (IS_NODE_PIE_LIST(pndArg->next) && IS_ENODE(pndArg->next->children)) {
	  /* relocate the following list element to task node */
	  pndList = pndArg->next;
	  //pndResult = pndList->next;
	  xmlUnlinkNode(pndList);
	  xmlAddChild(pndTask, pndList);
	}
	else {
	  /* there is no list of details */
	}

	for (pndChild = (pndList != NULL ? pndList->children : NULL); pndChild; pndChild = RecognizeTasks(pndChild));

	/*! replace pndArg by pndTask
	 */
	xmlReplaceNode(pndArg, pndTask);
	xmlFreeNode(pndArg);
      }
      else if (IS_NODE_PIE_PAR(pndArg) || IS_NODE_PIE_LIST(pndArg) || IS_NODE_PIE_SECTION(pndArg) || IS_NODE_PIE_BLOCK(pndArg) || IS_NODE_PIE_PIE(pndArg)) {
	for (pndChild = pndArg->children; pndChild; pndChild = RecognizeTasks(pndChild));
      }
      else {
	/* skip */
      }
    }
  }
  return pndResult;
} /* End of RecognizeTasks() */


/*! derive a sequence of text and LINK nodes from node
 */
xmlNodePtr
RecognizeFigures(xmlNodePtr pndArg)
{
  xmlNodePtr pndResult = NULL;

  if (pndArg) {
    pndResult = pndArg->next;

    if (IS_ENODE(pndArg)) {
      xmlNodePtr pndChild;
      xmlNodePtr pndForAppend;
      int iLengthStr;

      if (IS_NODE_PIE_PAR(pndArg)) {
	if ((pndChild = pndArg->children) != NULL
	    && xmlNodeIsText(pndChild)
	    && pndChild->content != NULL
	    && (iLengthStr = xmlStrlen(pndChild->content)) > 0) {
	  int rc;
	  pcre2_match_data *match_data;
	  xmlChar *pucSubstr = pndChild->content;

	  match_data = pcre2_match_data_create_from_pattern(re_fig, NULL);
	  rc = pcre2_match(
			   re_fig,        /* result of pcre2_compile() */
			   (PCRE2_SPTR8)pucSubstr,  /* the subject string */
			   xmlStrlen(pucSubstr),             /* the length of the subject string */
			   0,              /* start at offset 0 in the subject */
			   0,              /* default options */
			   match_data,        /* vector of integers for substring information */
			   NULL);            /* number of elements (NOT size in bytes) */

	  if (rc > -1) {
	    PCRE2_SIZE *ovector;

	    ovector = pcre2_get_ovector_pointer(match_data);
	    if (pcre2_get_ovector_count(match_data) > 1 && ovector[1] - ovector[0] > 3) {
	      xmlChar *pucT = NULL;
	      xmlChar *pucRelease;
	      xmlNodePtr pndRelease;

	      pucRelease = xmlStrndup(pucSubstr + ovector[4], (int)(ovector[5] - ovector[4]));
	      PrintFormatLog(3, "fig '%s' (%i..%i) in '%s'", pucRelease, ovector[0], ovector[1], pucSubstr);

	      pndRelease = pndArg->children;
	      xmlUnlinkNode(pndRelease);
	      xmlNodeSetName(pndArg, NAME_PIE_FIG);
	      pndForAppend = xmlNewChild(pndArg, NULL, NAME_PIE_IMG, NULL);
	      xmlSetProp(pndForAppend, BAD_CAST "src", pucRelease);

	      if (pcre2_get_ovector_count(match_data) > 2 && ovector[7] - ovector[6] > 0) {
		/* there exists an additional name */
		pucT = xmlStrndup(pucSubstr + ovector[6], (int)(ovector[7] - ovector[6]));
	      }
	      else {
		/* use filename as header */
		//pucT = resPathGetBasename(pucRelease);
	      }
	      if (pucT != NULL && xmlStrlen(pucT) > 0) {
		xmlNewChild(pndArg, NULL, NAME_PIE_HEADER, pucT);
	      }
	      xmlFree(pucT);
	      xmlFree(pucRelease);
	      xmlFreeNode(pndRelease);
	      xmlNewTextChild(pndArg, NULL, NAME_PIE_TTAG, BAD_CAST"#fig");
	    }
	  }
	  /* else ignore empty p elements */
	  pcre2_match_data_free(match_data);   /* Release memory used for the match */
	}
      }
      else {
	for (pndChild = pndArg->children; pndChild; pndChild = RecognizeFigures(pndChild));
      }
    }
  }
  return pndResult;
} /* End of RecognizeFigures() */


/*! set attribute "type" at pndArgImport according to eArgMode

\param pndArgImport node to set attribute

\return TRUE if successful, else FALSE
*/
BOOL_T
SetTypeAttr(xmlNodePtr pndArgImport, rmode_t eArgMode)
{
  BOOL_T fResult = FALSE;

  if (IS_NODE_PIE_IMPORT(pndArgImport) || IS_NODE_PIE_BLOCK(pndArgImport)) {
    xmlChar *pucAttrType;

    fResult = TRUE;
    if ((pucAttrType = domGetPropValuePtr(pndArgImport, BAD_CAST "type")) != NULL) {
      /* type is defined already, dont change it */
      fResult = (xmlStrEqual(pucAttrType, BAD_CAST "line") && eArgMode == RMODE_LINE)
	|| (xmlStrEqual(pucAttrType, BAD_CAST "log") && eArgMode == RMODE_LINE)
	|| (xmlStrEqual(pucAttrType, BAD_CAST "logpar") && eArgMode == RMODE_PAR)
	|| (xmlStrEqual(pucAttrType, BAD_CAST "pre") && eArgMode == RMODE_PRE)
	|| (xmlStrEqual(pucAttrType, BAD_CAST "script") && eArgMode == RMODE_PRE)
	|| (xmlStrEqual(pucAttrType, BAD_CAST "csv") && eArgMode == RMODE_TABLE)
	|| (xmlStrEqual(pucAttrType, BAD_CAST "par") && eArgMode == RMODE_PAR);
    }
    else if (eArgMode == RMODE_LINE) {
      xmlSetProp(pndArgImport, BAD_CAST "type", BAD_CAST "line");
    }
    else if (eArgMode == RMODE_PRE) {
      xmlSetProp(pndArgImport, BAD_CAST "type", BAD_CAST "pre");
    }
    else if (eArgMode == RMODE_MD) {
      xmlSetProp(pndArgImport, BAD_CAST "type", BAD_CAST "markdown");
    }
    else if (eArgMode == RMODE_TABLE) {
      xmlSetProp(pndArgImport, BAD_CAST "type", BAD_CAST "table");
    }
  }
  return fResult;
} /* end of SetTypeAttr() */


/*! 

\param pndArgImport node to get attribute

\return mode if successful, else RMODE_PAR
*/
rmode_t 
GetModeByMimeType(RN_MIME_TYPE tArg)
{
  rmode_t eResultMode = RMODE_PAR;

  switch (tArg) {

  case MIME_TEXT_PLAIN_CALENDAR:
    eResultMode = RMODE_LINE;
  break;

#ifdef WITH_MARKDOWN
  case MIME_TEXT_MARKDOWN:
    eResultMode = RMODE_MD;
  break;
#endif
  
  case MIME_TEXT_CSV:
    eResultMode = RMODE_TABLE;
  break;

  default:
    eResultMode = RMODE_PAR;
  }

  return eResultMode;
} /* end of GetModeByMimeType() */


/*! get attribute "type" at pndArgImport according to eArgMode

\param pndArgImport node to get attribute

\return mode if successful, else RMODE_PAR
*/
rmode_t 
GetModeByAttr(xmlNodePtr pndArgImport)
{
  rmode_t eResultMode = RMODE_PAR;

  if (IS_NODE_PIE_IMPORT(pndArgImport) || IS_NODE_PIE_BLOCK(pndArgImport)) {
    xmlChar *pucAttrType;
    
    if ((pucAttrType = domGetPropValuePtr(pndArgImport, BAD_CAST "type")) == NULL) {
      xmlChar *pucAttrName;
      xmlChar *pucAttrNameExt;
      /* no type is defined */
      
      if ((pucAttrName = domGetPropValuePtr(pndArgImport, BAD_CAST "name")) != NULL
	  && (pucAttrNameExt = resPathGetExtension(pucAttrName)) != NULL) {
	PrintFormatLog(1, "Ext '%s'", pucAttrNameExt);
	eResultMode = GetModeByExtension(pucAttrNameExt);
	xmlFree(pucAttrNameExt);
      }
    }
    else if (xmlStrEqual(pucAttrType, BAD_CAST "line") || xmlStrEqual(pucAttrType, BAD_CAST "cal")) {
      eResultMode = RMODE_LINE;
    }
    else if (xmlStrEqual(pucAttrType, BAD_CAST "log")) {
      eResultMode = RMODE_LINE;
    }
    else if (xmlStrEqual(pucAttrType, BAD_CAST "logpar")) {
    }
    else if (xmlStrEqual(pucAttrType, BAD_CAST "pre")) {
      eResultMode = RMODE_PRE;
    }
    else if (xmlStrEqual(pucAttrType, BAD_CAST "markdown")) {
      eResultMode = RMODE_MD;
    }
    else if (xmlStrEqual(pucAttrType, BAD_CAST "script")) {
      eResultMode = RMODE_PAR; /* result of script must be parsed paragraph-oriented */
    }
    else if (xmlStrEqual(pucAttrType, BAD_CAST "csv")) {
      eResultMode = RMODE_TABLE;
    }
    else if (xmlStrEqual(pucAttrType, BAD_CAST "par")) {
    }
    else {
      PrintFormatLog(2, "No valid import format '%s'", pucAttrType);
    }
  }

  return eResultMode;
} /* end of GetModeByAttr() */


/*! get attribute "type" at pndArgImport according to eArgMode

\param pndArgImport node to get attribute

\return mode if successful, else RMODE_PAR
*/
rmode_t 
GetModeByExtension(xmlChar *pucArgExt)
{
  rmode_t eResultMode = RMODE_PAR;

  if (STR_IS_EMPTY(pucArgExt)) {
    /* no type is defined */
  }
  else if (xmlStrEqual(pucArgExt, BAD_CAST "log") || xmlStrEqual(pucArgExt, BAD_CAST "cal")) {
    eResultMode = RMODE_LINE;
  }
  else if (xmlStrEqual(pucArgExt, BAD_CAST "csv")) {
    eResultMode = RMODE_TABLE;
  }
  else if (xmlStrEqual(pucArgExt, BAD_CAST "js")) {
    eResultMode = RMODE_PRE;
  }
  else if (xmlStrEqual(pucArgExt, BAD_CAST "md")) {
    eResultMode = RMODE_MD;
  }
  else {
    /* use default */
  }

  return eResultMode;
} /* end of GetModeByExtension() */


#ifdef TESTCODE
#include "test/test_pie_text_blocks.c"
#endif

