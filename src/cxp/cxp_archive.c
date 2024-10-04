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


/*
*/
#include <libxml/parser.h>

#include "basics.h"
#include <res_node/res_node_io.h>
#include "calendar_element.h"
#include <cxp/cxp.h>
#include "dom.h"
#include <cxp/cxp_dir.h>
#include "plain_text.h"
#include "utils.h"

#ifdef HAVE_LIBARCHIVE
#include <cxp/cxp_archive.h>
#endif


static BOOL_T
AddNodeList(resNodePtr prnArgZip, xmlNodePtr pndArgAdd, cxpContextPtr pccArg);

static BOOL_T
AddTextList(resNodePtr prnArgZip, xmlChar* pucArgAdd, cxpContextPtr pccArg);


/*! process the DIR child instructions of pndMakePie

\todo remove DOM result
*/
xmlDocPtr
arcProcessZipNode(xmlNodePtr pndArgZip, cxpContextPtr pccArg)
{
  xmlDocPtr pdocResult = NULL;

  if (IS_VALID_NODE(pndArgZip)) {
    resNodePtr prnZip;

#ifdef DEBUG
    cxpCtxtLogPrint(pccArg, 3, "arcProcessZipNode()");
#endif

    prnZip = cxpResNodeResolveNew(pccArg, pndArgZip, NULL, CXP_O_NONE);
    if (prnZip) {
      xmlChar* pucT;
      cxpContextPtr pccHere;

      resNodeSetMimeType(prnZip,MIME_APPLICATION_ZIP);
#if 0
      if ((pucT = domGetPropValuePtr(pndArgZip, BAD_CAST"context"))) {
	pccHere = cxpCtxtFromAttr(pccArg, pndArgZip);
      }
      else {
      }
#endif
	pccHere = pccArg;

      if (pndArgZip->children == NULL) { /* zip element has no childs, parse content only */
	if (resNodeIsReadable(prnZip) && resNodeUpdate(prnZip, RN_INFO_CONTENT, NULL, NULL)) { /*! read Resource Node as list of childs */
	  xmlNodePtr pndRoot;

	  pndRoot = xmlNewNode(NULL, NAME_ARCHIVE);
	  if (pndRoot) {
	    pdocResult = xmlNewDoc(BAD_CAST "1.0");
	    if (pdocResult) {
	      resNodeContentToDOM(pndRoot, prnZip);
	      xmlSetTreeDoc(pndRoot, pdocResult);
	      xmlDocSetRootElement(pdocResult, pndRoot);
	    }
	    else {
	      xmlFreeNode(pndRoot);
	    }
	  }
	}
      }
      else if (resNodeOpen(prnZip, "wa")) { /* zip element has childs, pack their resource nodes */
	int iLevelCompress;
	xmlChar* pucAttr;
	xmlNodePtr pndChildZip = NULL;

	/*!\todo set depth attribute per single dir element, instead of global */
	if ((pucAttr = domGetPropValuePtr(pndArgZip, BAD_CAST "level")) != NULL
	  && ((iLevelCompress = atoi((char*)pucAttr)) > 0)) {
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
	      AddNodeList(prnZip, pndDir, pccHere);
	      xmlFreeDoc(pdocResultT);
	    }
	  }
	  else if (IS_NODE_PLAIN(pndChildZip)) {
	    xmlChar* pucResultT;
	    pucResultT = cxpProcessPlainNode(pndChildZip, pccHere);
	    if (pucResultT) {
	      /*! plain list of files to compress */
	      AddTextList(prnZip, pucResultT, pccHere);
	      xmlFree(pucResultT);
	    }
	  }
	  else if (xmlNodeIsText(pndChildZip) && STR_IS_NOT_EMPTY(pndChildZip->content)) {
	    /*! plain list of files to compress */
	    AddTextList(prnZip, pndChildZip->content, pccHere);
	  }
	  else if (IS_NODE_PIE(pndChildZip) || IS_NODE_DIR(pndChildZip) || IS_NODE_FILE(pndChildZip)) {
	    AddNodeList(prnZip, pndChildZip, pccHere);
	  }
	  else {
	    cxpCtxtLogPrint(pccArg, 1, "Element '%s' ignored", pndChildZip->name);
	  }
	}
	resNodeClose(prnZip);
	/*!\todo define resulting pdocResult */
      }
      resNodeFree(prnZip);

      if (pccHere != pccArg) {
	cxpCtxtIncrExitCode(pccArg, cxpCtxtGetExitCode(pccHere));
	//cxpCtxtFree(pccHere);
      }
    }
    else {
      cxpCtxtLogPrint(pccArg, 1, "No archive name found");
    }
  }
  return pdocResult;
} /* end of arcProcessZipNode() */


/*! \return
*/
BOOL_T
AddTextList(resNodePtr prnArgZip, xmlChar *pucArg, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;

  if (resNodeIsError(prnArgZip) == FALSE) {
    resNodePtr prnTree = NULL;

    if ((prnTree = resNodeSplitLineBufferNew(pucArg))) {
      if (resNodeGetNameBaseDir(prnTree) == NULL) {
	resNodeSetNameBaseDir(prnTree, cxpCtxtLocationGetStr(pccArg));
      }
      fResult = arcAddResNode(prnArgZip, prnTree, NULL, resNodeGetNameNormalized(cxpCtxtLocationGet(pccArg)));
      resNodeFree(prnTree);
    }
  }
  return fResult;
} /* end of AddTextList() */


/*! \return
*/
BOOL_T
AddNodeList(resNodePtr prnArgZip, xmlNodePtr pndArg, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;

  if (resNodeIsError(prnArgZip) == FALSE) {
    resNodePtr prnTree;

    if ((prnTree = dirNodeToResNodeList(pndArg))) {
//       if (resNodeGetNameBaseDir(prnTree) == NULL) {
// 	resNodeSetNameBaseDir(prnTree, cxpCtxtLocationGetStr(pccArg));
//       }
//      fResult = arcAddResNode(prnArgZip, prnTree, NULL, resNodeGetNameNormalized(cxpCtxtLocationGet(pccArg)));

resNodeAddChild(prnArgZip, prnTree);
fResult = arcFileWrite(prnArgZip);
resNodeFree(prnTree);
    }
  }
  return fResult;
} /* end of AddNodeList() */


#ifdef TESTCODE
#include "test/test_cxp_archive.c"
#endif
