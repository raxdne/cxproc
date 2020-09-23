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
arcNodeAdd(resNodePtr prnArgZip,xmlNodePtr pndArgAdd,cxpContextPtr pccArg, cxpContextPtr pccArgBasedir);

static BOOL_T
arcNodeTextAdd(resNodePtr prnArgZip,xmlChar *pucArgAdd,cxpContextPtr pccArg, cxpContextPtr pccArgBasedir);

static BOOL_T
arcResNodeAdd(resNodePtr prnArgZip,resNodePtr prnArgAdd,xmlChar *pucArg, cxpContextPtr pccArg);

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

  pccHere = cxpCtxtFromAttr(pccArg,pndArgZip);

  prnZip = cxpResNodeResolveNew(pccArg, pndArgZip, NULL, CXP_O_READ);
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

	  pdocResultT = cxpProcessXmlNode(pndChildZip, pccHere);
	  if (pdocResultT) {
	    arcNodeAdd(prnZip, xmlDocGetRootElement(pdocResultT), pccHere, pccHere);
	    xmlFreeDoc(pdocResultT);
	  }
	}
	else if (IS_NODE_PLAIN(pndChildZip)) {
	  xmlChar *pucResultT;
	  pucResultT = cxpProcessPlainNode(pndChildZip, pccHere);
	  if (pucResultT) {
	    /*! plain list of files to compress */
	    arcNodeTextAdd(prnZip, pucResultT, pccHere, pccHere);
	    xmlFree(pucResultT);
	  }
	}
	else if (IS_NODE_DIR(pndChildZip) || IS_NODE_FILE(pndChildZip)) {
	  arcNodeAdd(prnZip, pndChildZip, pccHere, pccHere);
	}
	else if (IS_NODE_PIE(pndChildZip)) {
	  arcNodeAdd(prnZip, pndChildZip, pccHere, pccHere);
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


/*! \return 0 in case of success
*/
BOOL_T
arcResNodeAdd(resNodePtr prnArgZip, resNodePtr prnArgAdd, xmlChar *pucArg, cxpContextPtr pccArg)
{
  if (resNodeIsError(prnArgZip) == FALSE && resNodeGetHandleIO(prnArgZip) != NULL) {
    if (resNodeIsError(prnArgAdd) == FALSE) {
      char *pcPathInZip;
      xmlChar *pucT;
      arcEntryPtr pArcEntryAdd;

      if (pucArg != NULL && xmlStrlen(pucArg) > 0) {
	/* use mapped name */
	pucT = xmlStrdup(pucArg);
      }
      else {
	pucT = xmlStrdup(_resNodeGetNameDescendant(prnArgAdd));
      }

      if (resNodeIsDir(prnArgAdd)) {
	pucT = xmlStrcat(pucT, BAD_CAST"/");
      }
      resPathChangeToSlashes(pucT);

      if (arcGetFileNameEncoded(pucT,&pcPathInZip) == FALSE || pcPathInZip == NULL) {
	xmlFree(pucT);
	PrintFormatLog(1, "Skipping file because of context encoding error");
	return FALSE;
      }

#ifdef DEBUG
      PrintFormatLog(3, "Compress '%s' as '%s' to '%s'",
	resNodeGetNameNormalizedNative(prnArgAdd),
	pucT,
	resNodeGetNameNormalizedNative(prnArgZip));
#else
      //PrintStatusDot(2);
#endif

      pArcEntryAdd = archive_entry_new();
      if (pArcEntryAdd) {
	archive_entry_set_mtime(pArcEntryAdd, resNodeGetMtime(prnArgAdd), 0);
	archive_entry_set_mode(pArcEntryAdd, AE_IFREG | 0755);
	//archive_write_set_filter_option((arcPtr)resNodeGetHandleIO(prnArgZip), NULL, "compression-level", "1");

	if (resNodeIsDir(prnArgAdd)) {
	  int iErr;

	  archive_entry_copy_pathname(pArcEntryAdd, pcPathInZip);
	  archive_entry_set_size(pArcEntryAdd, 0);
	  iErr = archive_write_header((arcPtr)resNodeGetHandleIO(prnArgZip), pArcEntryAdd);
	  if (iErr == ARCHIVE_OK) {
	  }
	  else {
	    PrintFormatLog(1, "Error writing header file '%s' to '%s': %s",
	      resNodeGetNameNormalizedNative(prnArgAdd), resNodeGetNameNormalizedNative(prnArgZip), archive_error_string((arcPtr)resNodeGetHandleIO(prnArgZip)));
	  }
	}
	else if (resNodeGetContent(prnArgAdd, 1024)) {
	  unsigned int l;
	  unsigned int m;

	  l = resNodeGetSize(prnArgAdd);
	  if (l > 0) {
	    int iErr;

	    // TODO: update content to existing archive file
	    archive_entry_copy_pathname(pArcEntryAdd, pcPathInZip);
	    archive_entry_set_size(pArcEntryAdd, l);
	    iErr = archive_write_header((arcPtr)resNodeGetHandleIO(prnArgZip), pArcEntryAdd);
	    if (iErr == ARCHIVE_OK) {
	      m = archive_write_data((arcPtr)resNodeGetHandleIO(prnArgZip), resNodeGetContent(prnArgAdd, 1024), l);
	      if (m != l) {
		PrintFormatLog(1, "Error writing file '%s' to '%s'",
		    resNodeGetNameNormalizedNative(prnArgAdd), resNodeGetNameNormalizedNative(prnArgZip));
	      }
	    }
	    else {
	      PrintFormatLog(1, "Error writing header file '%s' to '%s': %s",
		  resNodeGetNameNormalizedNative(prnArgAdd), resNodeGetNameNormalizedNative(prnArgZip), archive_error_string((arcPtr)resNodeGetHandleIO(prnArgZip)));
	    }
	  }
	  else {
	    PrintFormatLog(1, "Error reading '%s'", resNodeGetNameNormalizedNative(prnArgAdd));
	  }
	}
	archive_entry_free(pArcEntryAdd);
      }
      xmlFree(pcPathInZip);
      xmlFree(pucT);
    }
    else {
      PrintFormatLog(1, "Skipping file because of previous context error");
    }
  }
  return FALSE;
} /* end of arcResNodeAdd() */


/*! \return
*/
BOOL_T
arcNodeTextAdd(resNodePtr prnArgZip,xmlChar *pucArgAdd,cxpContextPtr pccArg, cxpContextPtr pccArgBasedir)
{
  int i;
  int j;
  BOOL_T fResult = FALSE;

  for (i=0, j=0; ; ) {
    if (isend(pucArgAdd[i]) || pucArgAdd[i] == '\n' || pucArgAdd[i] == '\r') {
      xmlChar *pucPath;
      resNodePtr prnAdd;

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
      prnAdd = resNodeConcatNew(cxpCtxtLocationGetStr(pccArg),pucPath);
      if (prnAdd == NULL) {
	PrintFormatLog(2,"Cant create context for '%s'",pucPath);
      }
      else if (_resNodeSetNameAncestor(prnAdd,cxpCtxtLocationGetStr(pccArgBasedir)) == FALSE) {
	PrintFormatLog(2,"No valid ancestor path '%s'",cxpCtxtLocationGetStr(pccArgBasedir));
      }
      else if (resNodeReadStatus(prnAdd) == FALSE) {
	PrintFormatLog(2,"Directory or file '%s' does not exist",resNodeGetNameNormalized(prnAdd));
      }
      else if (resNodeIsDir(prnAdd) || resNodeIsFile(prnAdd) || resNodeIsURL(prnAdd)) {
	fResult |= (arcResNodeAdd(prnArgZip,prnAdd,NULL,pccArg) == 0);
      }
      else {
	PrintFormatLog(2,"File entry '%s' neither file nor directory",pucPath);
      }
      resNodeFree(prnAdd);
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

  return fResult;
} /* end of arcNodeTextAdd() */


/*! \return
*/
BOOL_T
arcNodeAdd(resNodePtr prnArgZip, xmlNodePtr pndArgAdd, cxpContextPtr pccArg, cxpContextPtr pccArgBasedir)
{
  BOOL_T fResult = FALSE;

  if (resNodeIsError(prnArgZip) == FALSE) {
    xmlChar *pucAttrName;

    pucAttrName = domGetAttributePtr(pndArgAdd,BAD_CAST "name");
    if (pucAttrName == NULL && (IS_NODE_PIE(pndArgAdd) || IS_NODE_XML(pndArgAdd) || IS_NODE_FILE(pndArgAdd) || IS_NODE_DIR(pndArgAdd))) {
      xmlNodePtr pndChild;

      for (pndChild = pndArgAdd->children; pndChild; pndChild = pndChild->next) {
	if (IS_NODE(pndChild, NULL)) {
	  fResult |= arcNodeAdd(prnArgZip, pndChild, pccArg, pccArgBasedir); /* recursion */
	}
      }
    }
    else if (IS_NODE_FILE(pndArgAdd) || IS_NODE_DIR(pndArgAdd)) {
      resNodePtr prnAdd = NULL;
      xmlChar *pucAttrMap;

      pucAttrMap = domGetAttributePtr(pndArgAdd,BAD_CAST "map"); /* alternative name */
      if (pucAttrMap) {
	if (xmlStrlen(pucAttrMap) > 0 && resPathIsRelative(pucAttrMap)) {
	  /* OK */
	}
	else {
	  PrintFormatLog(2,"Mapping name '%s' not usable",pucAttrMap);
	  pucAttrMap = NULL;
	}
      }
      /*!\todo mapping via script or xpath attribute? */

      prnAdd = cxpResNodeResolveNew(pccArg, pndArgAdd, pucAttrName, CXP_O_READ);
      if (prnAdd == NULL) {
	PrintFormatLog(2,"Cant create context for '%s'",pucAttrName);
      }
      else if (_resNodeSetNameAncestor(prnAdd,cxpCtxtLocationGetStr(pccArgBasedir)) == FALSE) {
	PrintFormatLog(2,"No valid ancestor path '%s'",cxpCtxtLocationGetStr(pccArgBasedir));
      }
      else if (resNodeReadStatus(prnAdd) == FALSE) {
	PrintFormatLog(2,"Directory or file '%s' does not exist",resNodeGetNameNormalized(prnAdd));
      }
      else if (resNodeIsDir(prnAdd)) {
	// add directory entry itself
	fResult |= (arcResNodeAdd(prnArgZip,prnAdd,pucAttrMap,pccArg) == 0);

	if (pndArgAdd->children) {
	  cxpContextPtr pccDir;

	  pccDir = cxpCtxtFromAttr(pccArg,pndArgAdd);
	  if (pccDir) {
	    //cxpContextSetCurrent(pccDir);
	    cxpCtxtLocationUpdate(pccDir,pucAttrName);

	    if (resNodeIsFile(cxpCtxtLocationGet(pccDir))) {
	      PrintFormatLog(2,"Context '%s' is a file, not a directory",cxpCtxtLocationGetStr(pccDir));
	    }
	    else {
	      xmlNodePtr pndChild;

	      PrintFormatLog(2,"Context '%s' is an existing directory",cxpCtxtLocationGetStr(pccDir));
	      for (pndChild = pndArgAdd->children; pndChild; pndChild = pndChild->next) {
		if (IS_NODE(pndChild, NULL)) {
		  fResult |= arcNodeAdd(prnArgZip, pndChild, pccDir, pccArgBasedir); /* recursion */
		}
	      }
	    }
	    if (pccDir != pccArg) {
	      cxpCtxtIncrExitCode(pccArg, cxpCtxtGetExitCode(pccDir));
	      //cxpCtxtFree(pccDir);
	    }
	  }
	}
      }
      else if (resNodeIsFile(prnAdd) || resNodeIsURL(prnAdd)) {
	fResult |= (arcResNodeAdd(prnArgZip,prnAdd,pucAttrMap,pccArg) == 0);
      }
      else {
	PrintFormatLog(2,"File entry '%s' neither file nor directory",pucAttrName);
      }
      resNodeFree(prnAdd);
    }
    else {
    }
  }
  else {
    //PrintFormatLog(1,"zipFileOpen('%s') failed",resNodeGetNameNormalizedNative(prnArg));
  }
  return fResult;
} /* end of arcNodeAdd() */


#ifdef TESTCODE
#include "test/test_cxp_archive.c"
#endif
