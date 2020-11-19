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
#include <libxml/parser.h>

#include "basics.h"
#include <res_node/res_node_io.h>
#include <pie/calendar_element.h>
#include <cxp/cxp.h>
#include "dom.h"
#include <cxp/cxp_dir.h>
#include "plain_text.h"
#include "utils.h"

#ifdef HAVE_LIBARCHIVE
#include <archive/cxp_archive.h>
#endif


static BOOL_T
arcNodeTreeAdd(resNodePtr prnArgZip, xmlNodePtr pndArgAdd, cxpContextPtr pccArg);

static BOOL_T
arcNodeTextListAdd(resNodePtr prnArgZip, xmlChar* pucArgAdd, cxpContextPtr pccArg);

static BOOL_T
arcResNodeAdd(resNodePtr prnArgZip, resNodePtr prnArgAdd, xmlChar* pucArg, cxpContextPtr pccArg);


/*! process the DIR child instructions of pndMakePie

\todo additional argument depth_to_go for recursion limit
*/
xmlDocPtr
arcProcessZipNode(xmlNodePtr pndArgZip, cxpContextPtr pccArg)
{
  xmlDocPtr pdocResult = NULL;
  xmlChar *pucAttr;
  xmlNodePtr pndChildZip = NULL;
  resNodePtr prnZip;
  cxpContextPtr pccHere;

#ifdef DEBUG
  cxpCtxtLogPrint(pccArg,3, "arcProcessZipNode()");
#endif

  if (IS_VALID_NODE(pndArgZip) == FALSE) {
    return NULL;
  }

  pccHere = pccArg;

  prnZip = cxpResNodeResolveNew(pccArg, pndArgZip, NULL, CXP_O_NONE);
  if (prnZip) {
    if (pndArgZip->children == NULL) {
      xmlNodePtr pndRoot = NULL;

      //pndRoot = resNodeToDOM(prnZip, RN_INFO_MAX);
      if (pndRoot) {
	pdocResult = xmlNewDoc(BAD_CAST "1.0");
	if (pdocResult) {
	  xmlSetTreeDoc(pndRoot, pdocResult);
	  xmlDocSetRootElement(pdocResult, pndRoot);
	}
	else {
	  xmlFreeNode(pndRoot);
	}
      }
    }
    else if (resNodeOpen(prnZip, "wa")) {
      int iLevelCompress;

      /*!\todo set depth attribute per single dir element, instead of global */
      if ((pucAttr = domGetAttributePtr(pndArgZip, BAD_CAST "level")) != NULL
	&& ((iLevelCompress = atoi((char *)pucAttr)) > 0)) {
	cxpCtxtLogPrint(pccArg, 3, "Set compress level to '%i'", iLevelCompress);
      }
      else {
	iLevelCompress = CXP_COMRESSION_DEFAULT;
      }

      for (pndChildZip = pndArgZip->children; pndChildZip; pndChildZip = pndChildZip->next) {

	if (IS_NODE_XML(pndChildZip)) {
	  /*! we must handle the result of cxpProcessXml(), run evaluation and free */
	  xmlDocPtr pdocResultT;
	  xmlNodePtr pndDir = NULL;

	  pdocResultT = cxpProcessXmlNode(pndChildZip, pccHere);
	  if (pdocResultT != NULL
	    && (pndDir = xmlDocGetRootElement(pdocResultT)) != NULL
	    && (pndDir = domGetFirstChild(pndDir, NAME_DIR)) != NULL) {
	    /*! DOM of files to compress */
	    arcNodeTreeAdd(prnZip, pndDir, pccHere);
	    xmlFreeDoc(pdocResultT);
	  }
	}
	else if (IS_NODE_PLAIN(pndChildZip)) {
	  xmlChar *pucResultT;
	  pucResultT = cxpProcessPlainNode(pndChildZip, pccHere);
	  if (pucResultT) {
	    /*! plain list of files to compress */
	    arcNodeTextListAdd(prnZip, pucResultT, pccHere);
	    xmlFree(pucResultT);
	  }
	}
	else if (IS_NODE_DIR(pndChildZip) || IS_NODE_FILE(pndChildZip)) {
	  arcNodeTreeAdd(prnZip, pndChildZip, pccHere);
	}
	else if (IS_NODE_PIE(pndChildZip)) {
	  arcNodeTreeAdd(prnZip, pndChildZip, pccHere);
	}
	else {
	  cxpCtxtLogPrint(pccArg, 1, "Element '%s' ignored", pndChildZip->name);
	}
      }
      resNodeClose(prnZip);
    }
    resNodeFree(prnZip);
  }
  else {
    cxpCtxtLogPrint(pccArg, 1, "No archive name found");
  }

  if (pccHere != pccArg) {
    cxpCtxtIncrExitCode(pccArg, cxpCtxtGetExitCode(pccHere));
    //cxpCtxtFree(pccHere);
  }

  return pdocResult;
} /* end of arcProcessZipNode() */


/*! compress prnArgAdd and all descendants to archive prnArgZip

\return TRUE in case of success
*/
BOOL_T
arcResNodeAdd(resNodePtr prnArgZip, resNodePtr prnArgAdd, xmlChar* pucArg, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;

  if (resNodeIsError(prnArgZip) == FALSE && resNodeGetHandleIO(prnArgZip) != NULL) {
    char* pcPathInZip;
    xmlChar* pucT = NULL;
    arcEntryPtr pArcEntryAdd;

    if (STR_IS_NOT_EMPTY(pucArg)) {
      /* use mapped name */
      pucT = xmlStrdup(pucArg);
    }
    else if (resNodeGetNameAlias(prnArgAdd)) {
      pucT = xmlStrdup(resNodeGetNameAlias(prnArgAdd));
    }
    else if ((pucT = resPathDiffPtr(resNodeGetNameNormalized(prnArgAdd), resNodeGetNameNormalized(cxpCtxtLocationGet(pccArg))))) {
      pucT = xmlStrdup(pucT);
    }
    else {
      pucT = resNodeGetAncestorPathStr(prnArgAdd);
    }

    if (resNodeIsDir(prnArgAdd)) {
      pucT = xmlStrcat(pucT, BAD_CAST"/");
    }
    resPathChangeToSlashes(pucT);

    if (arcGetFileNameEncoded(pucT, &pcPathInZip) == FALSE || pcPathInZip == NULL) {
      PrintFormatLog(1, "Skipping file because of context encoding error");
    }
    else {

#ifdef DEBUG
      PrintFormatLog(1, "Compress '%s' as '%s' to '%s'",
	resNodeGetNameNormalizedNative(prnArgAdd),
	pucT,
	resNodeGetNameNormalizedNative(prnArgZip));
#else
      //PrintStatusDot(2);
#endif

      pArcEntryAdd = archive_entry_new();
      if (pArcEntryAdd) {
	if (prnArgAdd) {
	  archive_entry_set_mtime(pArcEntryAdd, resNodeGetMtime(prnArgAdd), 0);
	}
	archive_entry_set_mode(pArcEntryAdd, AE_IFREG | 0755);
	//archive_write_set_filter_option((arcPtr)resNodeGetHandleIO(prnArgZip), NULL, "compression-level", "1");

	if (resNodeIsDir(prnArgAdd)) {
	  int iErr;

	  archive_entry_copy_pathname(pArcEntryAdd, pcPathInZip);
	  archive_entry_set_size(pArcEntryAdd, 0);
	  iErr = archive_write_header((arcPtr)resNodeGetHandleIO(prnArgZip), pArcEntryAdd);
	  if (iErr == ARCHIVE_OK) {
	    resNodePtr prnT;

	    for (prnT = resNodeGetChild(prnArgAdd); prnT; prnT = resNodeGetNext(prnT)) {
	      arcResNodeAdd(prnArgZip, prnT, pucArg, pccArg);
	    }
	  }
	  else {
	    PrintFormatLog(1, "Error writing header file '%s' to '%s': %s",
	      resNodeGetNameNormalizedNative(prnArgAdd), resNodeGetNameNormalizedNative(prnArgZip), archive_error_string((arcPtr)resNodeGetHandleIO(prnArgZip)));
	  }
	}
	else {
	  int iErr;
	  unsigned int l = 0;

	  if (prnArgAdd != NULL && resNodeGetContent(prnArgAdd,1024) != NULL) {
	    l = resNodeGetSize(prnArgAdd);
	  }
	  archive_entry_copy_pathname(pArcEntryAdd, pcPathInZip);
	  archive_entry_set_size(pArcEntryAdd, l);
	  iErr = archive_write_header((arcPtr)resNodeGetHandleIO(prnArgZip), pArcEntryAdd);
	  if (iErr == ARCHIVE_OK) {
	    if (l > 0) {
	      unsigned int m = 0;

	      /*!\todo update content to existing archive file */

	      m = archive_write_data((arcPtr)resNodeGetHandleIO(prnArgZip), resNodeGetContent(prnArgAdd, 1024), l);
	      if (m != l) {
		PrintFormatLog(1, "Error writing file '%s' to '%s'",
		  resNodeGetNameNormalizedNative(prnArgAdd), resNodeGetNameNormalizedNative(prnArgZip));
	      }
	    }
	  }
	  else {
	    PrintFormatLog(1, "Error writing header file '%s' to '%s': %s",
	      resNodeGetNameNormalizedNative(prnArgAdd), resNodeGetNameNormalizedNative(prnArgZip), archive_error_string((arcPtr)resNodeGetHandleIO(prnArgZip)));
	  }
	}
	archive_entry_free(pArcEntryAdd);
	fResult = TRUE;
      }
    }
    xmlFree(pcPathInZip);
    xmlFree(pucT);
  }
  return fResult;
} /* end of arcResNodeAdd() */


/*! \return
*/
BOOL_T
arcNodeTextListAdd(resNodePtr prnArgZip, xmlChar *pucArgAdd, cxpContextPtr pccArg)
{
  int i;
  int j;
  int l;
  BOOL_T fResult = FALSE;
  resNodePtr prnTree = NULL;

  for (i=0, j=0, l=0; ; ) {
    if (isend(pucArgAdd[i]) || pucArgAdd[i] == '\n' || pucArgAdd[i] == '\r') {
      xmlChar *pucPath;

      if (i == j) {
	if (isend(pucArgAdd[i])) {
	  break;
	}

	while (pucArgAdd[i] == '\n' || pucArgAdd[i] == '\r') {
	  i++; /* skip multiple line breaks */
	}
	j = i;
	continue;
      }
      
      pucPath = xmlStrndup(&(pucArgAdd[j]), i - j);
      if (STR_IS_NOT_EMPTY(pucPath)) {
	if (l > 0) {
	  assert(prnTree);
	  resNodeInsertStrNew(prnTree, pucPath);
	}
	else {
	  prnTree = resNodeSplitStrNew(pucPath);
	}
	l++;
      }
      xmlFree(pucPath);

      if (isend(pucArgAdd[i])) {
	break;
      }
      
      while (pucArgAdd[i] == '\n' || pucArgAdd[i] == '\r') {
	i++; /* skip multiple line breaks */
      }
      j = i;
    }
    else {
      i++;
    }
  }

  if (prnTree) {
    fResult = arcResNodeAdd(prnArgZip, prnTree, NULL, pccArg);
    resNodeFree(prnTree);
  }

  return fResult;
} /* end of arcNodeTextAdd() */


/*! \return
*/
BOOL_T
arcNodeTreeAdd(resNodePtr prnArgZip, xmlNodePtr pndArgAdd, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;

  if (resNodeIsError(prnArgZip) == FALSE) {
    resNodePtr prnTree;

    prnTree = dirNodeToResNodeList(pndArgAdd);
    //resNodeUpdate(prnTree,RN_INFO_CONTENT,NULL,NULL);
    fResult = arcResNodeAdd(prnArgZip,prnTree,NULL,pccArg);
    resNodeFree(prnTree);
  }
  return fResult;
} /* end of arcNodeTreeAdd() */


#ifdef TESTCODE
#include "test/test_cxp_archive.c"
#endif
