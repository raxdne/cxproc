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


/* 
 */
#include <libxml/tree.h>

/* 
 */
#include "basics.h"
#include "dom.h"
#include <pie/pie_text_blocks.h>
#include "utils.h"

static xmlChar
AutoDetectSeparatorChar(xmlChar *pucArg);

static xmlChar *
GetNextSeparatorPtr(xmlChar *pucArg, xmlChar ucSep);

static int
AddTableCellsEmpty(xmlNodePtr pndArg);

static int
AddTableColumnNames(xmlNodePtr pndArg);


/*! Append the parsed plain text to the given pndArgTop

\param pndArgTop parent node to append import result nodes OR NULL if pndArgImport must be replaced by result
\param pucArg pointer to an UTF-8 encoded buffer (not XML-conformant!)

\return pointer to result node "block" or NULL in case of errors
*/
xmlNodePtr
ParseCsvBuffer(xmlNodePtr pndArgTop, xmlChar *pucArg)
{
  index_t k;
  index_t iMax = -1;
  pieTextElementPtr ppeT = NULL;
  xmlNodePtr pndParent; /*! */
  xmlNodePtr pndResult = NULL;
  xmlChar *pucT;
  xmlChar *pucText = NULL;

  if ((ppeT = pieElementNew(pucArg, RMODE_TABLE))) {
    /*\todo iMax = domGetPropValuePtr(pndArgImport, BAD_CAST "max"); */
    iMax = 512 * 1024;
  }

  /*!
  main loop for reading pie text elements and building of DOM
  */
  for (k = 0, pndParent = pndArgTop; k < iMax && pieElementHasNext(ppeT); k++) {
    xmlNodePtr pndNew = NULL;

    pieElementParse(ppeT);

    if ((pndNew = pieElementToDOM(ppeT))) {
      xmlNodePtr pndList = NULL;

      /* append to result DOM */
      if (IS_NODE_PIE_SECTION(pndParent) && xmlStrEqual(domGetPropValuePtr(pndParent, BAD_CAST "type"), BAD_CAST "table")) {
	/* there is a table parent already */
      }
      else if (IS_NODE(pndParent, NULL) && IS_NODE_PIE_TD(pndParent->parent)) {
	/* there is a table cell parent already */

	xmlNodePtr pndT;

	assert(pndParent->children == NULL);
	/* remove 'pndParent' as empty block element */
	pndT = pndParent->parent;
	xmlUnlinkNode(pndParent);
	xmlFree(pndParent);
	pndParent = pndT;

	if (pndNew->children == pndNew->last) {
	  /* result is single node, remove 'pndNew' as empty block element */
	  pndT = pndNew;
	  pndNew = pndNew->children;
	  xmlUnlinkNode(pndNew->children);
	  xmlFree(pndT);
	}
      }
      else {
	xmlChar *pucSep;
	xmlChar *pucTT;

	pndParent = xmlNewChild(pndParent, NULL, BAD_CAST NAME_PIE_SECTION, NULL);
	xmlSetProp(pndParent, BAD_CAST "type", BAD_CAST "table");

	pucSep = xmlStrdup(BAD_CAST " ");

	if (((pucTT = domGetPropValuePtr(pndArgTop, BAD_CAST "sep")) != NULL) && xmlStrlen(pucTT) > 0) {
	  pucSep[0] = pucTT[0];
	}
	else {
	  pucSep[0] = AutoDetectSeparatorChar(pucArg);
	}
	xmlSetProp(pndParent, BAD_CAST "sep", pucSep);
	xmlFree(pucSep);
      }
      xmlAddChild(pndParent, pndNew);
    }
  }
  xmlFree(pucText);
  // domPutNodeString(stderr, BAD_CAST"", pndArgTop);

  TransformToTable(pndArgTop, pndParent, domGetPropValuePtr(pndParent, BAD_CAST "sep"));
  // CompressTable(pndParent);
  AddTableCellsEmpty(pndParent);
  AddTableColumnNames(pndParent);
  RecognizeScripts(pndParent);
  xmlUnsetProp(pndParent, BAD_CAST "sep");
  xmlUnsetProp(pndParent, BAD_CAST "type");

  pieElementFree(ppeT);
  pndResult = pndArgTop;

  return pndResult;
} /* end of ParseCsvBuffer() */


/*! return TRUE if pucArg ends at postion iArg with an XML entity like '&amp;'
*/
BOOL_T
StringEndsWithEntity(xmlChar *pucArg, int iArg)
{
  BOOL_T fResult = FALSE;

  if (pucArg != NULL) {

    /*!\bug check numerically encoded entities '&#xF3F5;' */

    if (iArg > 4 && pucArg[iArg] == (xmlChar)';' && pucArg[iArg - 1] == (xmlChar)'p' && pucArg[iArg - 2] == (xmlChar)'m' && pucArg[iArg - 3] == (xmlChar)'a' &&
	pucArg[iArg - 4] == (xmlChar)'&') {
      fResult = TRUE;
    }
    else if (iArg > 3 && pucArg[iArg] == (xmlChar)';' && pucArg[iArg - 1] == (xmlChar)'t' &&
	     (pucArg[iArg - 2] == (xmlChar)'g' || pucArg[iArg - 2] == (xmlChar)'l') && pucArg[iArg - 3] == (xmlChar)'&') {
      fResult = TRUE;
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


/*! \return pointer to detected separator, else DEFAULT_SEP_STR
 */
xmlChar
AutoDetectSeparatorChar(xmlChar *pucArg)
{
  xmlChar ucResult = DEFAULT_SEP_STR[0];

  if (pucArg != NULL) {

    if (StringBeginsWith((char *)pucArg, "sep=")) {
      if (isend(pucArg[4])) {
	/* content too short */
      }
      else {
	ucResult = pucArg[4];
      }
    }
    else {
      int i;
      int n[256];

      memset(n, 0, sizeof(n));
      for (i = 0; i < 1e4 && pucArg[i] != '\0'; i++) { n[pucArg[i]]++; }

      if (n['\t'] > n[';'] && n['\t'] > n[',']) {
	ucResult = '\t';
      }
      else if (n[','] > n[';']) {
	ucResult = ',';
      }
      else if (n[';'] > 0) {
	ucResult = ';';
      }
    }
  }
  return ucResult;
} /* end of AutoDetectSeparatorChar() */

/*! \return pointer to next separator, else NULL
 */
xmlChar *
GetNextSeparatorPtr(xmlChar *pucArg, xmlChar ucSep)
{
  xmlChar *pucResult = NULL;

  if (pucArg != NULL) {
    int i;
    BOOL_T fInQuotes;

    for (i = 0, fInQuotes = FALSE; pucArg[i] != '\0'; i++) {
      if (pucArg[i] == '\"') { /*!\todo process single quotes too */
	fInQuotes = !fInQuotes;
      }
      else if (pucArg[i] == ucSep && fInQuotes == FALSE) {
	pucResult = &pucArg[i];
	break;
      }
    }
  }
  return pucResult;
} /* end of GetNextSeparatorPtr() */


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
    xmlNodeSetName(pndArgParent, BAD_CAST NAME_PIE_TR);
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

    for (pucBegin = pucSep = pndChild->content;;) {
      xmlNodePtr pndT;

      if ((pucSep = GetNextSeparatorPtr(pucSep, pucPatternSep[0])) != NULL) {

	if (fMix && StringEndsWithEntity(pucBegin, (int)(pucSep - pucBegin))) {
	  /* its a XML entity like '&amp;' dont use this as separator */
	  pucSep = BAD_CAST xmlStrstr(pucSep + 1, pucPatternSep); /*!\bug to be repeated */
	}

	if ((pucCell = xmlStrndup(pucBegin, (int)(pucSep - pucBegin))) != NULL) {
	  StringRemovePairQuotes(pucCell);
	  StringRemoveDoubleDoubleQuotes(pucCell);
	  if ((pucT = StringGetEndOfHeaderMarker(pucCell))) {
	    pndT = xmlNewChild(pndArgParent, NULL, BAD_CAST NAME_PIE_TH, NULL);
	    if (STR_IS_NOT_EMPTY(pucT)) {
	      xmlAddChild(pndT, xmlNewText(pucT));
	    }
	  }
	  else {
	    pndT = xmlNewChild(pndArgParent, NULL, BAD_CAST NAME_PIE_TD, NULL);
	    if (STR_IS_NOT_EMPTY(pucCell)) {
	      xmlAddChild(pndT, xmlNewText(pucCell));
	    }
	  }
	  xmlFree(pucCell);
	}

	pucBegin = pucSep + xmlStrlen(pucPatternSep);
	pucSep = pucBegin;
      }
      else if ((pucCell = xmlStrdup(pucBegin)) != NULL) { /* no more separator */
	/*!\bug StringEndsWithEntity() required */
	StringRemovePairQuotes(pucCell);
	if (STR_IS_EMPTY(pucCell)) {
	  /* ignoring trailing empty cell */
	}
	else if ((pucT = StringGetEndOfHeaderMarker(pucCell))) {
	  pndT = xmlNewChild(pndArgParent, NULL, BAD_CAST NAME_PIE_TH, NULL);
	  if (STR_IS_NOT_EMPTY(pucT)) {
	    xmlAddChild(pndT, xmlNewText(pucT));
	  }
	}
	else {
	  pndT = xmlNewChild(pndArgParent, NULL, BAD_CAST NAME_PIE_TD, NULL);
	  if (STR_IS_NOT_EMPTY(pucCell)) {
	    xmlAddChild(pndT, xmlNewText(pucCell));
	  }
	}
	xmlFree(pucCell);
	break;
      }
    }

    xmlFreeNode(pndChild);
    xmlNodeSetName(pndArgParent,BAD_CAST NAME_PIE_TR);
    RecognizeSymbols(pndArgParent, LANG_DEFAULT);
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
  int c;
  xmlNodePtr pndI;

  if (IS_NODE_PIE_TABLE(pndArg) || IS_NODE_PIE_THEAD(pndArg) || IS_NODE_PIE_TBODY(pndArg)) {
    for (pndI = pndArg->children; pndI != NULL; pndI = pndI->next) {
      c = GetTableColumns(pndI);
      if (c > iResult) {
	iResult = c;
      }
    }
  }
  else if (IS_NODE_PIE_TR(pndArg)) {
    for (c = 0, pndI = pndArg->children; pndI != NULL; pndI = pndI->next) {
      if (IS_NODE_PIE_TH(pndI) || IS_NODE_PIE_TD(pndI)) {
	if (pndI->next == NULL && xmlStrlen(domNodeGetContentPtr(pndI)) < 1) {
	  /* last cell in row is empty */
	}
	else {
	  c++;
	}
      }
    }
    if (c > iResult) {
      iResult = c;
    }
  }
  return iResult;
} /* end of GetTableColumns() */


/*! \return number of columns in widest row in table
*/
int
AddTableColumnNames(xmlNodePtr pndArg)
{
  int iResult = 0;
  int i;

  i = GetTableColumns(pndArg);
  if (i > 0) {
    xmlNodePtr pndT;
    xmlNodePtr pndTableHeader;
    xmlNodePtr pndTableBody;

    pndTableHeader = domGetFirstChild(pndArg, NAME_PIE_THEAD);
    if (pndTableHeader) {
      domUnlinkNodeList(pndTableHeader);
    }
    else {
      /* table header does not yet exist */
      int j;

      pndTableHeader = xmlNewNode(NULL, NAME_PIE_THEAD);
      pndT = xmlNewChild(pndTableHeader, NULL, NAME_PIE_TR, NULL);
#if 1
      for (j = 0; j < i; j++) {
	xmlChar mucT[128];

	xmlStrPrintf(mucT, sizeof(mucT), "%i", j+1);
	xmlNewChild(pndT, NULL, NAME_PIE_TH, mucT);
      }
#else
      for (j = 0; j < i; j++) {
	int k = 0;
	xmlChar mucT[128];

	if (j < 26) {
	  mucT[0] = 'A' + j % 26;
	  k = 1;
	}
	else if (j < (26 * 27)) {
	  mucT[0] = (xmlChar) ('A' + j / 26 - 1);
	  mucT[1] = (xmlChar) ('A' + j % 26);
	  k = 2;
	}
	else {}
	mucT[k] = '\0';
	xmlNewChild(pndT, NULL, NAME_PIE_TH, mucT);
      }
#endif
    }

    pndTableBody = domGetFirstChild(pndArg, NAME_PIE_TBODY);
    if (pndTableBody) {
      domUnlinkNodeList(pndTableBody);
    }
    else {
      pndTableBody = xmlNewNode(NULL, NAME_PIE_TBODY);
      pndT = pndArg->children;
      domUnlinkNodeList(pndT);
      xmlAddChildList(pndTableBody, pndT);
    }

    /* correct order */
    xmlAddChild(pndArg, pndTableHeader);
    xmlAddChild(pndArg, pndTableBody);
    iResult = i;
  }
  return iResult;
} /* end of AddTableColumnNames() */


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
		xmlNodeSetName(pndCell, BAD_CAST NAME_PIE_TH);
	      }
	      j++;
	      pndCell = pndCell->next;
	    }
	    else if (IS_ENODE(pndCell)) {
	      pndCell = pndCell->next;
	    }
	    else {
	      xmlNewChild(pndArgRow, NULL, (fHeader ? BAD_CAST NAME_PIE_TH : BAD_CAST NAME_PIE_TD), BAD_CAST" ");
	      j++;
	    }
	  }
	}
      }
    }
    /* add some meta data to table */
    xmlSetProp(pndArg, BAD_CAST "cols", mucT);
    xmlStrPrintf(mucT,128,"%i",r-1);
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
      xmlNodeSetName(pndArg, BAD_CAST NAME_PIE_TABLE);
      xmlNewTextChild(pndArg, NULL, BAD_CAST NAME_PIE_TTAG, BAD_CAST"#table");

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
	xmlNodeSetName(pndChild,BAD_CAST NAME_PIE_TH);
      }
    }
    else if (IS_NODE_PIE_TD(pndArg) || IS_NODE_PIE_TH(pndArg) || IS_NODE_PIE_TABLE(pndArg)) {
      /* already transformed */
    }
  }
  
  return pndResult;
}
/* end of TransformToTable() */


#ifdef TESTCODE
#include "test/test_pie_csv.c"
#endif
