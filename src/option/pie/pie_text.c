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

/*!\todo remove XML-only features (@effort, @assignee,  ...), in XSL too */

/*!\todo autodetection line or paragraph input */

/*!\todo autodetection encoding (BOM) */

/*!\todo autodetection input language */

/*!\todo autodetection dates containing offsets, ranges etc */

/*!\todo define an id markup for referencing, anchor for link target, local and global anchors: |#abc|TEST| */

/*!\todo cite markup [] to <cite></cite> */

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
#include <pie/pie_calendar.h>
#include "utils.h"
#include <vcf/vcf.h>

#ifdef HAVE_PETRINET
#include <petrinet/petrinet.h>
#endif

#ifdef HAVE_JS
#include <script/script.h>
#endif

static void
TraverseImportNodes(xmlNodePtr pndArg, cxpContextPtr pccArg);

static xmlNodePtr
TraverseScriptNodes(xmlNodePtr pndCurrent, cxpContextPtr pccArg);

static BOOL_T
ProcessPieDoc(xmlNodePtr pndArgResult, xmlDocPtr pdocArgPie, cxpContextPtr pccArg);

static BOOL_T
ProcessImportNode(xmlNodePtr pndArgImport, cxpContextPtr pccArg);

static BOOL_T
ImportNodeFile(xmlNodePtr pndArgImport, cxpContextPtr pccArg);

static BOOL_T
_ImportNodeArg(xmlNodePtr pndArgResult, xmlNodePtr pndArgImport, cxpContextPtr pccArg);

static BOOL_T
ImportNodeContent(xmlNodePtr pndArgImport, cxpContextPtr pccArg);

static BOOL_T
ImportNodeCxp(xmlNodePtr pndArgImport, cxpContextPtr pccArg);

static BOOL_T
ImportNodeStdin(xmlNodePtr pndArgImport, cxpContextPtr pccArg);


/*! exit procedure for this module
*/
void
pieTextCleanup(void)
{
  pieTextTagsCleanup();
  pieTextBlocksCleanup();
} /* end of pieTextCleanup() */


/*! \param pndArg node to search for attribute 'context' or 'prefix'

\return pointer to a new allocated buffer according to attribute content or to DOM URL or NULL
*/
xmlChar *
pieGetAncestorContextStr(xmlNodePtr pndArg)
{
  xmlChar *pucResult = NULL;

  if (pndArg) {
    xmlNodePtr pndT;

    for (pndT=pndArg; pndT; pndT=pndT->parent) {
      if ((pucResult = xmlGetProp(pndT, BAD_CAST"context"))
	|| (pucResult = xmlGetProp(pndT, BAD_CAST"prefix"))) {
	pucResult = xmlStrdup(pucResult);
	break;
      }
    }

    if (STR_IS_EMPTY(pucResult) &&  pndArg != NULL && pndArg->doc != NULL && STR_IS_NOT_EMPTY(pndArg->doc->URL)) {
      pucResult = resPathGetBasedir(BAD_CAST pndArg->doc->URL);
    }
  }
  return pucResult;
} /* end of pieGetAncestorContextStr() */


/*! process the pie tree of pdocArgPie and append result to pndArgParent

\param pndArgParent node to append processing result
\param pdocArgPie DOM to process a pie tree
\param pccArg the processing context

\return new allocated pie node if successful, else NULL
*/
BOOL_T
ProcessPieDoc(xmlNodePtr pndArgResult, xmlDocPtr pdocArgPie, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;
  xmlNodePtr pndRoot;

  assert(pndArgResult);

  if ((pndRoot = xmlDocGetRootElement(pdocArgPie)) != NULL && IS_NODE_PIE(pndRoot) && pndRoot->children != NULL) {
    xmlDocPtr pdocT;

    pdocT = pieProcessPieNode(pndRoot, pccArg); /* because of substitutions and pie options */
    if (pdocT != NULL && (pndRoot = xmlDocGetRootElement(pdocT)) != NULL && IS_NODE_PIE(pndRoot)) {
      xmlNodePtr pndT;

      //cxpCtxtLogPrintDoc(pccInput, 2, "read result", pdocArgPie);
      pndT = xmlCopyNodeList(pndRoot->children);
      /*!\todo unlink meta and error nodes */
      xmlAddChildList(pndArgResult, pndT);
      fResult = TRUE;
    }
    else {
    }
    xmlFreeDoc(pdocT);
  }
  else {
    cxpCtxtLogPrint(pccArg, 1, "This is not a pie node");
  }

  return fResult;
}
/* end of ProcessPieDoc() */


/*!
\param pndArg node
\return TRUE if pndArg has only one text child
*/
BOOL_T
NodeHasSingleText(xmlNodePtr pndArg)
{
  return (pndArg != NULL && pndArg->children != NULL && pndArg->children == pndArg->last && pndArg->children->type == XML_TEXT_NODE);
}
/* End of NodeHasSingleText() */


/*! process the PIE child instructions of pndArgPie

\param pdocArgPie DOM to process a pie tree
\param pccArg the processing context

\return new allocated pie DOM if successful, else NULL
*/
xmlDocPtr
pieParseFile(xmlChar *pucArg, cxpContextPtr pccArg)
{
  xmlDocPtr pdocResult = NULL;
  xmlNodePtr pndPieRoot = NULL;

#ifdef DEBUG
  cxpCtxtLogPrint(pccArg, 4, "pieParseFile(pucArg=%0x,pccArg=%0x)", pucArg, pccArg);
#endif

  if ((pndPieRoot = xmlNewNode(NULL, NAME_PIE_PIE))) {
    xmlNodePtr pndT = NULL;

    pndT = xmlNewChild(pndPieRoot, NULL, NAME_PIE_IMPORT, NULL);
    xmlSetProp(pndT, BAD_CAST"name", pucArg);
    xmlSetProp(pndT, BAD_CAST"tags", BAD_CAST"yes");

    //domPutNodeString(stderr, BAD_CAST "split result", pndPieRoot);
    if ((pdocResult = pieProcessPieNode(pndPieRoot, pccArg))) {
    }
    else {
      /* no pie make instructions */
    }
    xmlFreeNode(pndPieRoot);
    //domPutDocString(stderr, BAD_CAST "split result", pdocResult);
  }
  return pdocResult;
} /* end of pieParseFile() */


/*! process the PIE child instructions of pndArgPie

\param pdocArgPie DOM to process a pie tree
\param pccArg the processing context

\return new allocated pie DOM if successful, else NULL
*/
xmlDocPtr
pieProcessPieNode(xmlNodePtr pndArgPie, cxpContextPtr pccArg)
{
  xmlNodePtr pndMeta;
  xmlNodePtr pndError;
  xmlDocPtr pdocResult = NULL;
  xmlNodePtr pndPieRoot = NULL;
  xmlNodePtr pndBlock = NULL;
  xmlChar *pucAttr;

#ifdef DEBUG
  cxpCtxtLogPrint(pccArg, 4, "pieProcessPieNode(pndArg=%0x,pccArg=%0x)", pndArgPie, pccArg);
#endif

  CompileRegExpDefaults();

  if (IS_NODE_PIE(pndArgPie) && pndArgPie->children != NULL && (pdocResult = xmlNewDoc(BAD_CAST "1.0")) != NULL) {
    xmlNodePtr pndMakePieCopy;
    xmlNodePtr pndTags;

    pdocResult->encoding = xmlStrdup(BAD_CAST"UTF-8");
    pndPieRoot = xmlNewNode(NULL,NAME_PIE_PIE);

    if (NodeHasSingleText(pndArgPie)) {
      pndBlock = xmlNewNode(NULL, NAME_PIE_BLOCK);
      xmlNewChild(pndBlock, NULL, NAME_PIE_IMPORT, domNodeGetContentPtr(pndArgPie));
    }
    else {
      pndBlock = xmlCopyNode(pndArgPie, 1); /* first copy for import processing */
      xmlNodeSetName(pndBlock, NAME_PIE_BLOCK);
      xmlSetNs(pndBlock,NULL);
    }

    if (pndArgPie->doc != NULL && STR_IS_NOT_EMPTY(pndArgPie->doc->URL)) {
      resNodePtr prnDoc; 	/* because of libxml2 handling of "file:/C:/TMP/" */

      prnDoc = resNodeDirNew(BAD_CAST pndArgPie->doc->URL);
      if (prnDoc) {
	xmlSetProp(pndBlock, BAD_CAST"context", resNodeGetURI(prnDoc));
	resNodeFree(prnDoc);
      }
    }
    xmlAddChild(pndPieRoot, pndBlock);
    xmlSetTreeDoc(pndPieRoot, pdocResult);
    xmlDocSetRootElement(pdocResult, pndPieRoot);

    if (domGetPropFlag(pndPieRoot, NAME_PIE_IMPORT, TRUE)) {
      cxpContextPtr pccImport;

      cxpCtxtLogPrint(pccArg, 2, "Importing from node");
      pccImport = cxpCtxtFromAttr(pccArg, pndArgPie);
      // TODO: set top element if @root = 'yes'

      //xmlSetProp(pndPieRoot, BAD_CAST "class", BAD_CAST "article");
      /*! \todo xmlSetNs(pndPieRoot,pnsPie); */
      TraverseImportNodes(pndBlock, pccImport);

      if (pccImport != pccArg) {
	cxpCtxtIncrExitCode(pccArg, cxpCtxtGetExitCode(pccImport));
      }
    }
    else {
    }
    pndMeta = xmlNewChild(pndPieRoot, NULL, NAME_META, NULL);
    xmlSetTreeDoc(pndMeta, pdocResult);
    cxpInfoProgram(pndMeta, pccArg);
    pndMakePieCopy = xmlCopyNode(pndArgPie, 1); /* second copy for meta element */
    /*! \todo avoid copy of complete pie tree here */
    xmlAddChild(pndMeta, pndMakePieCopy);
    /* Get the current time. */
    domSetPropEat(pndMeta, BAD_CAST "ctime", GetNowFormatStr(BAD_CAST "%s"));
    domSetPropEat(pndMeta, BAD_CAST "ctime2", GetDateIsoString(0));

    /*! \todo add error logs to DOM */
    //pndError = xmlNewChild(pndPieRoot, NULL, NAME_ERROR, NULL);

    if (domGetPropFlag(pndArgPie, BAD_CAST "script", TRUE)) {
      cxpCtxtLogPrint(pccArg, 2, "Recognize scripts");
      RecognizeScripts(pndPieRoot);
      TraverseScriptNodes(pndPieRoot, pccArg);
    }
    else {
      cxpCtxtLogPrint(pccArg, 3, "Ignoring scripts");
    }

    if (domGetPropFlag(pndArgPie, BAD_CAST "figure", TRUE)) {
      cxpCtxtLogPrint(pccArg, 2, "Recognize Figures");
      RecognizeFigures(pndPieRoot);
    }
    else {
      cxpCtxtLogPrint(pccArg, 3, "Ignoring Figures markup");
    }

    if (domGetPropFlag(pndArgPie, BAD_CAST "url", TRUE)) {
      cxpCtxtLogPrint(pccArg, 2, "Recognize URLs");
      /*! \todo use an attribute for regexp?? */
      RecognizeUrls(pndPieRoot);
    }
    else {
      cxpCtxtLogPrint(pccArg, 3, "Ignoring URLs");
    }

    /* process all child subst nodes */
    cxpSubstInChildNodes(pndBlock, NULL, pccArg);

    /* replace all subst nodes in tree by its result */
    cxpSubstReplaceNodes(pndBlock, pccArg);

    RecognizeInlines(pndPieRoot);
    RecognizeDates(pndPieRoot);

    if (domGetPropFlag(pndArgPie, BAD_CAST "todo", TRUE)) {
      cxpCtxtLogPrint(pccArg, 2, "Recognize tasks markup");
      RecognizeTasks(pndPieRoot);
    }
    else {
      cxpCtxtLogPrint(pccArg, 3, "Ignoring tasks markup");
    }

    //xmlSetProp(pndArgPie, BAD_CAST "xpath",BAD_CAST "/*/*[1]/*[1]/*[1]/*");

    pucAttr = xmlGetProp(pndArgPie, BAD_CAST "xpath"); /*  */
    if (STR_IS_NOT_EMPTY(pucAttr) && xmlStrEqual(pucAttr,BAD_CAST"/*") == FALSE) {
      xmlDocPtr pdocResultXPath;

      cxpCtxtLogPrint(pccArg, 2, "Locator XPath for '%s'", pucAttr);
      if ((pdocResultXPath = domGetXPathDoc(pdocResult, pucAttr)) != NULL) {
	xmlFreeDoc(pdocResult);
	pdocResult = pdocResultXPath;
	pndPieRoot = xmlDocGetRootElement(pdocResult);
      }
    }

    pucAttr = xmlGetProp(pndArgPie, BAD_CAST "tags"); /* domGetPropFlag() not used because of (empty|yes|no|regexp) */
    if (STR_IS_NOT_EMPTY(pucAttr) && xmlStrcasecmp(pucAttr, BAD_CAST "no") == 0) {
      /* explicit: no */
    }
    else {
      cxpCtxtLogPrint(pccArg, 2, "Recognize tags '%s'", pucAttr);
      ProcessTags(pdocResult, pucAttr);
    }

    //xmlSetProp(pndArgPie, BAD_CAST "pattern", BAD_CAST "t[contains(text(),'Year')]");

    pucAttr = xmlGetProp(pndArgPie, BAD_CAST "pattern"); /*  */
    if (STR_IS_NOT_EMPTY(pucAttr)) {
      xmlChar *pucTT;

      pucTT = xmlStrdup(BAD_CAST"//*[");
      /*\bug replace quotes by apostrophs */
      pucTT = xmlStrcat(pucTT,pucAttr);
      pucTT = xmlStrcat(pucTT, BAD_CAST"]");
      /*\todo check XPAth format of pucTT */
      cxpCtxtLogPrint(pccArg, 2, "Filter XPath for '%s'", pucTT);
      if (domWeightXPathInDoc(pdocResult, pucTT)) {
	CleanUpTree(pndPieRoot);
      }
      xmlFree(pucTT);
    }

    /*! \todo global cite recognition in scientific text */

    if (domGetPropFlag(pndArgPie, BAD_CAST "offset", FALSE)) {
      calAddAttributeDayDiff(pdocResult);
    }
  }
  else {
    /* no pie make instructions */
    //xmlSetProp(pndPieRoot, BAD_CAST "class", BAD_CAST "empty");
  }
  //domPutDocString(stderr, BAD_CAST "split result", pdocResult);

  return pdocResult;
}
/* end of pieProcessPieNode() */


/*! process the import instructions of pndArgImport in directory context of pccArg

\param pndArgResult node to append processing result
\param pndArgImport node to test for import, else traversing childs
\param pccArg the processing context

\return TRUE if import was successful, else FALSE
*/
BOOL_T
ImportNodeCxp(xmlNodePtr pndArgImport, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;
  xmlNodePtr pndChild;

#ifdef DEBUG
  cxpCtxtLogPrint(pccArg, 3, "ImportNodeCxp(pndArgImport=%0x,pccArg=%0x)", pndArgImport, pccArg);
#endif
  assert(IS_NODE_PIE_IMPORT(pndArgImport));

  if ((pndChild = domGetFirstChild(pndArgImport, NAME_XML)) != NULL) {
    xmlNodePtr pndPieRoot;
    xmlDocPtr pdocPie;
    /*! build an import result pndArgResult */

    cxpCtxtLogPrint(pccArg, 2, "Importing via cxp XML");
    if (pndChild->next) {
      cxpCtxtLogPrint(pccArg, 2, "Next '%s' element will be ignored", pndChild->next->name);
    }

    if ((pdocPie = cxpProcessXmlNode(pndChild, pccArg)) != NULL) { /* first child only !!! */

      // cxpCtxtLogPrintDoc(pccArg, 4, "import result", pdocPie);
      if ((pndPieRoot = xmlDocGetRootElement(pdocPie)) != NULL && IS_NODE_PIE_PIE(pndPieRoot)) {
	xmlNodePtr pndPieProcRoot;
	xmlDocPtr pdocPieProc;

	if ((pdocPieProc = pieProcessPieNode(pndPieRoot, pccArg)) != NULL) {
	  if ((pndPieProcRoot = xmlDocGetRootElement(pdocPieProc)) != NULL && IS_NODE_PIE_PIE(pndPieProcRoot) && pndPieProcRoot->children != NULL) {
	    //domPutNodeString(stderr,BAD_CAST "ImportNodeCxp()",pndPieProcRoot);
	    xmlUnlinkNode(pndPieProcRoot);
	    xmlNodeSetName(pndPieProcRoot, NAME_PIE_BLOCK);
	    xmlSetNs(pndPieProcRoot,NULL);
	    xmlReplaceNode(pndArgImport, pndPieProcRoot);
	    xmlFreeNode(pndArgImport);
	    TraverseImportNodes(pndPieProcRoot, pccArg); /* parse result recursively */
	    fResult = TRUE;
	  }
	  else {
	    //xmlSetProp(pndBlock, BAD_CAST"error", BAD_CAST"schema");
	  }
	  xmlFreeDoc(pdocPieProc);
	}
	xmlFreeDoc(pdocPie);
      }
    }
    else {
      //xmlSetProp(pndBlock, BAD_CAST"error", BAD_CAST"schema");
    }
  }
  else if ((pndChild = domGetFirstChild(pndArgImport, NAME_PLAIN)) != NULL) {
    xmlDocPtr pdocT;
    xmlChar *pucContent = NULL;
    int iLen;

    pucContent = cxpProcessPlainNode(pndChild, pccArg);
    if (STR_IS_NOT_EMPTY(pucContent)) {
      xmlNodePtr pndBlock;

      fResult = TRUE;
      pndBlock = pndArgImport;
      xmlSetNs(pndBlock,NULL);
      xmlNodeSetName(pndBlock, NAME_PIE_BLOCK);
      //xmlSetProp(pndBlock, BAD_CAST "context", resNodeGetURI(prnInput));

      if (ParsePlainBuffer(pndBlock, pucContent, GetModeByAttr(pndArgImport))) {
	TraverseImportNodes(pndBlock, pccArg); /* parse result recursively */
      }
      else {
	xmlSetProp(pndBlock, BAD_CAST"error", BAD_CAST"parse");
      }
      //domPutNodeString(stderr,BAD_CAST "ImportNodeFile()",pndBlock);
    }
    else {
      //cxpCtxtLogPrint(pccArg, 1, "Cant read from '%s'", resNodeGetNameNormalized(prnInput));
      //xmlSetProp(pndBlock, BAD_CAST"error", BAD_CAST"empty");
    }
    xmlFree(pucContent);
  }
  else {
    //cxpCtxtLogPrint(pccArg, 1, "Cant read from '%s'", BAD_CAST"cxp");
    //xmlSetProp(pndBlock, BAD_CAST"error", BAD_CAST"schema");
  }
  return fResult;
}
/* end of ImportNodeCxp() */


/*! process the import instructions of pndArgImport in directory context of pccArg

\param pndArgResult node to append processing result
\param pndArgImport node to test for import, else traversing childs
\param pccArg the processing context

\return TRUE if import was successful, else FALSE
*/
BOOL_T
ImportNodeStdin(xmlNodePtr pndArgImport, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;
  /*! build an import result pndArgResult */
  xmlChar *pucContent = NULL;
  xmlNodePtr pndBlock;
  resNodePtr prnInput; /*! context for import of new document (avoid side effect) */

#ifdef DEBUG
  cxpCtxtLogPrint(pccArg, 3, "ImportNodeStdin(pndArgImport=%0x,pccArg=%0x)", pndArgImport, pccArg);
#endif
  assert(IS_NODE_PIE_IMPORT(pndArgImport));

  pndBlock = pndArgImport;
  xmlSetNs(pndBlock,NULL);
  xmlNodeSetName(pndBlock, NAME_PIE_BLOCK);
  xmlSetProp(pndBlock, BAD_CAST "context", BAD_CAST"stdin");

  prnInput = cxpResNodeResolveNew(NULL, pndArgImport, NULL, CXP_O_NONE);
  if (prnInput) {
    pucContent = plainGetContextTextEat(prnInput, 1024);
    if (pucContent) {
      int iLen;
      xmlDocPtr pdocPie;

      fResult = TRUE;

      /* check for pie XML input first */
      if ((iLen = xmlStrlen(pucContent)) > 10 && (pdocPie = xmlParseMemory((const char *)pucContent, iLen))) {
	cxpCtxtLogPrint(pccArg, 2, "Importing %i byte PIE/XML from stdin", iLen);
	if (ProcessPieDoc(pndBlock, pdocPie, pccArg)) {
	  //cxpCtxtLogPrintDoc(pccArg, 4, "import result", pdocPie);
	}
	else {
	}
	xmlFreeDoc(pdocPie);
      }
      else {
	cxpCtxtLogPrint(pccArg, 2, "Importing %i byte PIE/Plain from stdin", iLen);
	if (ParsePlainBuffer(pndBlock, pucContent, GetModeByAttr(pndBlock))) {
	}
	else {
	  xmlSetProp(pndBlock, BAD_CAST "error", BAD_CAST"parse");
	}
      }
    }
    else {
      cxpCtxtLogPrint(pccArg, 1, "Empty input");
      xmlSetProp(pndBlock, BAD_CAST "error", BAD_CAST"empty");
    }
    resNodeFree(prnInput);
  }

  return fResult;
} /* end of ImportNodeStdin() */


/*! process the import instructions of pndArgImport in directory context of pccArg

\param pndArgTop node to append processing result
\param pndArgImport node to test for import, else traversing childs
\param pccArg the processing context

\return TRUE if import was successful, else FALSE
*/
BOOL_T
ImportNodeFile(xmlNodePtr pndArgImport, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;
  resNodePtr prnInput = NULL; /*! context for import of new document (avoid side effect) */

#ifdef DEBUG
  cxpCtxtLogPrint(pccArg, 3, "ImportNodeFile(pndArgImport=%0x,pccArg=%0x)", pndArgImport, pccArg);
#endif
  assert(IS_NODE_PIE_IMPORT(pndArgImport));

  prnInput = cxpResNodeResolveNew(pccArg, pndArgImport, NULL, CXP_O_READ);
  if (prnInput) {
    BOOL_T fLocator;
    xmlNodePtr pndBlock = NULL;

#ifdef HAVE_CGI
    fLocator = TRUE;
#else
    fLocator = FALSE;
#endif

    resNodeResetMimeType(prnInput);

    /*! \todo add cache handling for import */

    pndBlock = pndArgImport;
    xmlSetNs(pndBlock,NULL);
    xmlNodeSetName(pndBlock,NAME_PIE_BLOCK);
    xmlSetProp(pndBlock, BAD_CAST "context", resNodeGetURI(prnInput));

    if (IsImportCircular(pndBlock, prnInput)) { /*! check circular reference */
      xmlSetProp(pndBlock, BAD_CAST"error", BAD_CAST"circular");
      /*! \bug circular check for cache too */
    }
    else if (resNodeIsReadable(prnInput) == FALSE && cxpCtxtCacheGetResNode(pccArg, resNodeGetNameNormalized(prnInput)) == NULL) {
      xmlSetProp(pndBlock, BAD_CAST"error", BAD_CAST"read");
    }
    else {
      xmlChar *pucAttrCache = NULL;
      xmlChar *pucAttrType;
      xmlChar *pucContent = NULL;
      RN_MIME_TYPE iMimeType;
      resNodePtr prnT;
      cxpContextPtr pccInput;

      if (resNodeGetChild(prnInput)) {
	iMimeType = resNodeGetMimeType(resNodeGetChild(prnInput));
      }
      else {
	iMimeType = resNodeGetMimeType(prnInput);
      }
      pucAttrType = xmlGetProp(pndBlock, BAD_CAST"type");

      /* set a new cxpContext with file-based location */
      pccInput = cxpCtxtNew();
      prnT = resNodeDup(prnInput, RN_DUP_THIS);
      resNodeSetToParent(prnT);
      cxpCtxtLocationSet(pccInput, prnT);
      cxpCtxtLogSetLevel(pccInput, 2);
      if (pccArg) {
	cxpCtxtAddChild(pccArg, pccInput);
      }
      resNodeFree(prnT);

      if (resNodeGetNameNormalized(prnInput) != NULL
	&& pucAttrType != NULL && xmlStrEqual(pucAttrType, BAD_CAST"pre")) {
	cxpCtxtLogPrint(pccInput, 2, "Importing '%s' PIE '%s'", pucAttrType, resNodeGetNameNormalized(prnInput));

	if ((pucContent = cxpCtxtCacheGetBuffer(pccInput, resNodeGetNameNormalized(prnInput))) == NULL) {
	  /* there is no cached Buffer */
	  pucContent = plainGetContextTextEat(prnInput, 1024);
	}

	if (STR_IS_NOT_EMPTY(pucContent)) {
	  xmlNewChild(pndBlock, NULL, NAME_PIE_PRE, pucContent);
	}
	else {
	  cxpCtxtLogPrint(pccInput, 1, "Cant read from '%s'", resNodeGetNameNormalized(prnInput));
	  xmlSetProp(pndBlock, BAD_CAST"error", BAD_CAST"empty");
	}
	xmlFree(pucContent);
      }
      else if (resNodeGetNameNormalized(prnInput) != NULL
	&& ((pucAttrType != NULL
	&& (xmlStrEqual(pucAttrType, BAD_CAST"line")
	|| xmlStrEqual(pucAttrType, BAD_CAST"csv")
	|| xmlStrEqual(pucAttrType, BAD_CAST"log")
	|| xmlStrEqual(pucAttrType, BAD_CAST"markdown")
	|| xmlStrEqual(pucAttrType, BAD_CAST"cal")
	|| xmlStrEqual(pucAttrType, BAD_CAST"par")))
	|| iMimeType == MIME_TEXT_CSV
	|| iMimeType == MIME_TEXT_PLAIN
	|| iMimeType == MIME_TEXT_PLAIN_CALENDAR
#ifdef WITH_MARKDOWN
	|| iMimeType == MIME_TEXT_MARKDOWN
#endif
	|| iMimeType == MIME_TEXT_CALENDAR)) {
	rmode_t m;

	cxpCtxtLogPrint(pccInput, 2, "Importing '%s' PIE '%s'", pucAttrType, resNodeGetNameNormalized(prnInput));

	if ((pucContent = cxpCtxtCacheGetBuffer(pccInput, resNodeGetNameNormalized(prnInput))) == NULL) {
	  /* there is no cached Buffer */
	  pucContent = plainGetContextTextEat(prnInput, 1024);
	}

	if (iMimeType == MIME_TEXT_PLAIN) {
	  m = GetModeByExtension(resNodeGetExtension(prnInput));
	}
	else if (iMimeType != MIME_UNDEFINED) {
	  m = GetModeByMimeType(iMimeType);
	}
	else {
	  m = GetModeByAttr(pndBlock);
	}

	if (STR_IS_NOT_EMPTY(pucContent)) {
	  if (ParsePlainBuffer(pndBlock, pucContent, m)) {
	    domSetPropFileXpath(pndBlock, BAD_CAST"fxpath", NULL);
	    domSetPropFileLocator(pndBlock, resNodeGetNameRelative(cxpCtxtRootGet(pccInput), prnInput));
	    TraverseImportNodes(pndBlock, pccInput); /* parse result recursively */
	  }
	  else {
	    xmlSetProp(pndBlock, BAD_CAST"error", BAD_CAST"parse");
	  }
	  //domPutNodeString(stderr,BAD_CAST "ImportNodeFile()",pndArgResult);
	}
	else {
	  cxpCtxtLogPrint(pccInput, 1, "Cant read from '%s'", resNodeGetNameNormalized(prnInput));
	  xmlSetProp(pndBlock, BAD_CAST"error", BAD_CAST"empty");
	}
	xmlFree(pucContent);
      }
      else if (resNodeGetNameNormalized(prnInput) != NULL
	       && (pucAttrType != NULL && (xmlStrEqual(pucAttrType, BAD_CAST"script")
					   || iMimeType == MIME_APPLICATION_X_JAVASCRIPT))) {

	cxpCtxtLogPrint(pccInput, 2, "Importing '%s' Javascript result as PIE '%s'", pucAttrType, resNodeGetNameNormalized(prnInput));

	if ((pucContent = cxpCtxtCacheGetBuffer(pccInput, resNodeGetNameNormalized(prnInput))) == NULL) {
	  /* there is no cached Buffer */
	  pucContent = plainGetContextTextEat(prnInput, 1024);
	}

	/*!\todo process MIME_APPLICATION_JSON too */

	if (STR_IS_NOT_EMPTY(pucContent)) {
	  xmlChar *pucScriptResult = NULL;

	  xmlSetProp(pndBlock, BAD_CAST"type", BAD_CAST"script");
#ifdef HAVE_JS
	  pucScriptResult = scriptProcessScriptNode(pndBlock, pccInput);
#endif
	  if (pucScriptResult == NULL) {
	    xmlSetProp(pndBlock, BAD_CAST"result", BAD_CAST"empty");
	  }
	  else if (ParsePlainBuffer(pndBlock, pucScriptResult, GetModeByAttr(pndBlock))) {
	    TraverseImportNodes(pndBlock, pccInput); /* parse result recursively */
	  }
	  else {
	    xmlSetProp(pndBlock, BAD_CAST"result", BAD_CAST"parse");
	  }
	  xmlFree(pucScriptResult);
	  //domPutNodeString(stderr,BAD_CAST "ImportNodeFile()",pndArgResult);
	}
	else {
	  cxpCtxtLogPrint(pccInput, 1, "Cant read from '%s'", resNodeGetNameNormalized(prnInput));
	  xmlSetProp(pndBlock, BAD_CAST"error", BAD_CAST"script");
	}
	xmlFree(pucContent);
      }
      else if ((pucAttrType != NULL && xmlStrEqual(pucAttrType, BAD_CAST"xml"))
	|| iMimeType == MIME_TEXT_XML
	|| iMimeType == MIME_APPLICATION_PIE_XML) {
	xmlDocPtr pdocPie;

	cxpCtxtLogPrint(pccInput, 2, "Importing XML PIE '%s'", (pucAttrCache ? pucAttrCache : resNodeGetNameNormalized(prnInput)));

	if ((((pdocPie = cxpCtxtCacheGetDoc(pccInput, resNodeGetNameNormalized(prnInput))) != NULL) && (pdocPie = xmlCopyDoc(pdocPie,1)) != NULL)
	  ||
	  ((pdocPie = resNodeReadDoc(prnInput)) != NULL)) { /*! \todo remove redundant cache lookup */
	  xmlNodePtr pndT;

	  if ((pndT = xmlDocGetRootElement(pdocPie)) != NULL && IS_NODE_PIE_PIE(pndT) && pndT->children != NULL) {
#if 1
	    /* domUnlinkNodeList(pndT); is not yet usable when there is a mix of namspaces */
	    pndT = xmlCopyNodeList(pndT->children);
#else
	    pndT = pndT->children;
	    domUnlinkNodeList(pndT);
#endif
	    xmlAddChildList(pndBlock, pndT);
	  }
	  xmlFreeDoc(pdocPie);
	  domSetPropFileXpath(pndBlock, BAD_CAST"fxpath", NULL);
	  domSetPropFileLocator(pndBlock, resNodeGetNameRelative(cxpCtxtRootGet(pccInput), prnInput));
	  TraverseImportNodes(pndBlock, pccInput); /* parse result recursively */
	}
	else {
	  cxpCtxtLogPrint(pccInput, 1, "Cant read from '%s'", resNodeGetNameNormalized(prnInput));
	  xmlSetProp(pndBlock, BAD_CAST"error", BAD_CAST"parse");
	}
      }

      if (domGetPropFlag(pndBlock, BAD_CAST "locator", TRUE) == FALSE) {
	/* explicit NO */
	domUnsetPropFileLocator(pndBlock);
      }
      else if (domGetPropFlag(pndBlock, BAD_CAST "locator", fLocator)) {
	/* add additional attributes for navigation */
	//domSetPropFileLocator(pndBlock, resNodeGetNameRelative(cxpCtxtRootGet(pccInput), prnInput));
	//domSetPropFileXpath(pndBlock, BAD_CAST"fxpath", NULL);
      }

      xmlFree(pucAttrCache);
      if (pccArg == NULL) {
	cxpCtxtFree(pccInput);
      }
    }

    resNodeFree(prnInput);
    fResult = TRUE;
  }
  return fResult;
}
/* end of ImportNodeFile() */


/*! process the import instructions of pndArgImport in directory context of pccArg

\param pndArgImport node to test for import, else traversing childs
\param pccArg the processing context

\return TRUE if import was successful, else FALSE
*/
BOOL_T
ImportNodeContent(xmlNodePtr pndArgImport, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;
  xmlChar *pucContent = NULL;
  xmlNodePtr pndBlock = NULL;
  BOOL_T fLocator;

#ifdef HAVE_CGI
  fLocator = TRUE;
#else
  fLocator = FALSE;
#endif

#ifdef DEBUG
  cxpCtxtLogPrint(pccArg, 3, "ImportNodeContent(pndArgImport=%0x,pccArg=%0x)", pndArgImport, pccArg);
#endif
  assert(IS_NODE_PIE_IMPORT(pndArgImport));
  assert(NodeHasSingleText(pndArgImport));

  fResult = TRUE;
  pndBlock = pndArgImport;
  xmlSetNs(pndBlock,NULL);
  xmlNodeSetName(pndBlock, NAME_PIE_BLOCK);
  //xmlSetProp(pndBlock, BAD_CAST "context", resNodeGetURI(prnInput));

  if (xmlStrEqual(xmlGetProp(pndBlock, BAD_CAST "type"), BAD_CAST"script")) {
#ifdef HAVE_JS
    pucContent = scriptProcessScriptNode(pndBlock, pccArg);
#else
    /*\todo define fallback */
#endif
  }
  else {
    pucContent = domNodeEatContent(pndArgImport);
  }
  xmlFreeNode(pndArgImport->children);
  pndArgImport->children = pndArgImport->last = NULL; /* unlink node content */

  /*!\todo change pndArgImport->children to a CDATA node? */

  if (STR_IS_NOT_EMPTY(pucContent)) {
    if (ParsePlainBuffer(pndBlock, pucContent, GetModeByAttr(pndBlock))) {
      TraverseImportNodes(pndBlock, pccArg); /* parse result recursively */
      if (domGetPropFlag(pndArgImport, BAD_CAST "locator", fLocator)) {
	/*!\todo domSetPropFileLocator(pndBlock, resNodeGetNameRelative(cxpCtxtRootGet(pccArg), pndArgImport->doc->URL)); */
      }
    }
    else {
      xmlSetProp(pndBlock, BAD_CAST"error", BAD_CAST"parse");
    }
    //domPutNodeString(stderr,BAD_CAST "ImportNodeFile()",pndArgResult);
  }
  else {
    //cxpCtxtLogPrint(pccInput, 1, "Cant read from '%s'", resNodeGetNameNormalized(prnInput));
    xmlSetProp(pndBlock, BAD_CAST"result", BAD_CAST"empty");
  }
  xmlFree(pucContent);

  /*! \todo parse CDATA content */

  return fResult;
}
/* end of ImportNodeContent() */


/*! process the import instructions of pndArgImport in directory context of pccArg

\param pndArgResult node to append processing result
\param pndArgImport node to test for import, else traversing childs
\param pccArg the processing context

\return TRUE if import was successful, else FALSE
*/
BOOL_T
_ImportNodeArg(xmlNodePtr pndArgResult, xmlNodePtr pndArgImport, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;

  return fResult;

#ifdef DEBUG
  cxpCtxtLogPrint(pccArg, 1, "ImportNodeArg(pndArgImport=%0x,pccArg=%0x)", pndArgImport, pccArg);
#endif

  if (pndArgImport) {
    cxpSubstPtr pcxpSubstT;

    pcxpSubstT = cxpSubstDetect(pndArgImport, pccArg);
    if (pcxpSubstT) {
      xmlChar *pucContent;

      if ((pucContent = cxpSubstGetPtr(pcxpSubstT)) != NULL && xmlStrlen(pucContent) > 0) {

	/*!\todo build an import result pndPieResult */

	fResult = TRUE;
      }
      cxpSubstFree(pcxpSubstT);
    }
  }
  return fResult;
}
/* end of ImportNodeArg() */


/*! traverse DOM of pndArg searching for import nodes in context of pccArg and
* replaces import node by his processing result if pndArgTop is NULL, else append result to pndArgTop

\param pndArg node to test for import, else traversing childs
\param pccArg the processing context
*/
void
TraverseImportNodes(xmlNodePtr pndArg, cxpContextPtr pccArg)
{
  if (IS_VALID_NODE(pndArg) == FALSE) {
    /* ignore NULL and invalid elements */
  }
  else if (IS_NODE_PIE_IMPORT(pndArg)) {
    if (ProcessImportNode(pndArg, pccArg)) {
    }
    //    xmlFreeNode(pndArg);
  }
  else {
    /*
    recursion for all child nodes
    */
    xmlNodePtr pndArgChildren;

    for (pndArgChildren=pndArg->children; pndArgChildren;) {
      xmlNodePtr pndNext;

      assert(pndArgChildren->parent == pndArg);

      pndNext = pndArgChildren->next;
      if (IS_NODE_PIE_IMPORT(pndArgChildren)
	|| IS_NODE_PIE_SECTION(pndArgChildren)
	|| IS_NODE_PIE_TASK(pndArgChildren)
#ifdef HAVE_PETRINET
	|| IS_NODE_PKG2_STELLE(pndArgChildren)
	|| IS_NODE_PKG2_TRANSITION(pndArgChildren)
	|| IS_NODE_PKG2_REQUIREMENT(pndArgChildren)
#endif
	|| IS_NODE_PIE_PIE(pndArgChildren)
	|| IS_NODE_PIE_BLOCK(pndArgChildren)) {
	TraverseImportNodes(pndArgChildren, pccArg);
      }
      else {
	/* ignore this subtree */
      }
      pndArgChildren = pndNext;
    }
  }
}
/* end of TraverseImportNodes() */


/*! process the single import node pndArgImport in context of pccArg and replace it

\param pndArgImport node to test for import, else traversing childs
\param pccArg the processing context

\return TRUE if import was successful, else FALSE
*/
BOOL_T
ProcessImportNode(xmlNodePtr pndArgImport, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;

#ifdef DEBUG
  cxpCtxtLogPrint(pccArg, 3, "ProcessImportNode(pndArgImport=%0x,pccArg=%0x)", pndArgImport, pccArg);
#endif

  if (pndArgImport != NULL
    && (pndArgImport->parent || pndArgImport->prev || pndArgImport->next) /* because of replacing pndArgImport by import result */
    && IS_NODE_PIE_IMPORT(pndArgImport)) {
    cxpContextPtr pccHere = pccArg;	      /*! process context for import */
    cxpContextPtr pccDoc = NULL;	      /*! process context for import (derived from DOM) */
    xmlChar *pucAttrName = NULL;
    xmlNodePtr pndT;
    xmlDocPtr pdocT = NULL;

    //pccHere = cxpCtxtFromAttr(pccArg, pndArgImport);
    //pccDoc = cxpCtxtFromAttr(NULL, pndArgImport);
    pucAttrName = xmlGetProp(pndArgImport, BAD_CAST"name");

    if (resPathIsStd(pucAttrName)) {
      fResult = ImportNodeStdin(pndArgImport, pccHere);
    }
    else if ((pdocT = cxpCtxtCacheGetDoc(pccHere, pucAttrName)) != NULL) {
      /* matching DOM in cache found */
      if ((pndT = xmlDocGetRootElement(pdocT)) != NULL && pndT->children != NULL && (pndT = xmlCopyNodeList(pndT->children)) != NULL) {
	fResult = (xmlAddChildList(pndArgImport, pndT) != NULL);
	xmlNodeSetName(pndArgImport, NAME_PIE_BLOCK);
	TraverseImportNodes(pndArgImport, pccHere); /* parse result recursively */
      }
    }
    else if (domGetFirstChild(pndArgImport, NAME_XML) != NULL || domGetFirstChild(pndArgImport, NAME_PLAIN) != NULL) {
      fResult = ImportNodeCxp(pndArgImport, (cxpCtxtLocationGet(pccDoc) ? pccDoc : pccHere));
    }
    else if (STR_IS_NOT_EMPTY(pucAttrName)) {
      fResult = ImportNodeFile(pndArgImport, pccHere);
    }
    else if (NodeHasSingleText(pndArgImport)) {
      fResult = ImportNodeContent(pndArgImport, (cxpCtxtLocationGet(pccDoc) ? pccDoc : pccHere));
    }
    else {
      xmlAddChild(pndArgImport, xmlNewComment(BAD_CAST"unknown content type"));
    }

    cxpCtxtFree(pccDoc);
    if (pccHere != pccArg) {
      cxpCtxtIncrExitCode(pccArg, cxpCtxtGetExitCode(pccHere));
    }
  }
  return fResult;
}
/* end of ProcessImportNode() */

/*!
\param pndArg node to test for script, else traversing childs
\param pccArg the processing context

\return new allocated pie DOM if successful, else NULL
*/
xmlNodePtr
TraverseScriptNodes(xmlNodePtr pndCurrent, cxpContextPtr pccArg)
{
  xmlNodePtr pndResult = NULL;

#ifdef HAVE_JS
  if (IS_ENODE(pndCurrent)) {
    xmlNodePtr pndChild;
    int iLengthStr;

    if (IS_NODE_PIE_IMPORT(pndCurrent) && xmlStrEqual(xmlGetProp(pndCurrent,BAD_CAST"type"),BAD_CAST"script")) {
      if ((pndChild = pndCurrent->children) != NULL
	&& xmlNodeIsText(pndChild)
	&& pndChild->content != NULL
	&& (iLengthStr = xmlStrlen(pndChild->content)) > 0) {
	xmlChar *pucContent = NULL;
	xmlNodePtr pndReplace = NULL;

	pucContent = scriptProcessScriptNode(pndCurrent, pccArg);
	if (pucContent == NULL) {
	  /*  */
	}
	else if (IS_NODE_PIE(pndCurrent->parent) || IS_NODE_PIE_SECTION(pndCurrent->parent)) {
	  pndReplace = xmlNewNode(NULL, NAME_PIE_IMPORT);
	  xmlAddChild(pndReplace, xmlNewText(pucContent));
	}
	else {
	  pndReplace = xmlNewText(pucContent);
	}
	xmlFree(pucContent);

	if (pndReplace) {
	  xmlNodePtr pndT;

	  pndT = pndCurrent->next;
	  if (domReplaceNodeList(pndCurrent, pndReplace) == pndCurrent) {
	    xmlFreeNodeList(pndCurrent);
	  }
	  /*  */
	  if (pndT != NULL && pndT->prev != NULL) {
	    pndCurrent = pndT->prev;
	  }
	  else {
	    pndCurrent = NULL;
	  }
	  pndResult = pndReplace;
	}
      }
    }
    else {
      for (pndChild = pndCurrent->children; pndChild;) {
	xmlNodePtr pndT;

	pndT = TraverseScriptNodes(pndChild, pccArg);
	if (pndT) {
	  pndChild=pndT->next;
	}
	else {
	  pndChild=pndChild->next;
	}
      }
    }
  }
#endif
  return pndResult;
}
/* End of TraverseScriptNodes() */


/*! \return a string of ancestor section/h nodes
*/
xmlChar *
pieGetParentHeaderStr(xmlNodePtr pndN)
{
  xmlChar *pucResult;
  xmlNodePtr pndI;

  for ( pucResult=NULL,pndI=pndN; pndI; pndI = pndI->parent) {
    if (IS_NODE_PIE_SECTION(pndI)) {
      if (pndI==pndN) {
	/* ignore first section node itself */
      }
      else {
	xmlNodePtr pndHeader;

	pndHeader = domGetFirstChild(pndI, NAME_PIE_HEADER);
	if (pndHeader==NULL) {
	  /* skip */
	}
	else if (pndHeader==pndN) {
	  /* ignore first section node itself */
	}
	else {
	  xmlChar *pucT = NULL;
	  xmlChar *pucHeader = NULL;
	  xmlNodePtr pndHeaderChild;

	  for (pndHeaderChild=pndHeader->children; pndHeaderChild; pndHeaderChild=pndHeaderChild->next) {
	    if (IS_NODE_PIE_LINK(pndHeaderChild) || IS_NODE_PIE_DATE(pndHeaderChild) || IS_NODE_PIE_HTAG(pndHeaderChild)) {
	      if ((pucT = domNodeGetContentPtr(pndHeaderChild))) {
		/* append first text node content of link node */
		pucHeader = xmlStrcat(pucHeader, pucT);
	      }
	    }
	    else if (pndHeaderChild->type == XML_TEXT_NODE && xmlStrlen(pndHeaderChild->content) > 0) {
	      pucHeader = xmlStrcat(pucHeader, pndHeaderChild->content);
	    }
	  }

	  if (pucHeader) {
	    if (xmlStrlen(pucHeader) > 0) {
	      xmlChar *pucRelease;

	      pucHeader = xmlStrcat(pucHeader, BAD_CAST " :: ");
	      pucRelease = pucResult;
	      pucResult = xmlStrncatNew(pucHeader, pucResult, -1);
	      xmlFree(pucRelease);
	    }
	    xmlFree(pucHeader);
	  }

	  if (xmlStrlen(pucResult) > PARENTSTRING_LENGTH_MAX) {
	    break;
	  }
	}
      }
    }
    else if (IS_NODE(pndI,BAD_CAST"node")) {
      if (pndI==pndN) {
	/* ignore first section node itself */
      }
      else {
	xmlChar *pucHeader = NULL;
	xmlChar *pucAttrText;
	if ((pucAttrText = xmlGetProp(pndI,BAD_CAST"TEXT"))
	    && xmlStrlen(pucAttrText) > 0) {
	  xmlChar *pucRelease;
	  pucHeader = xmlStrncatNew(pucAttrText, BAD_CAST " :: ", -1);
	  pucRelease = pucResult;
	  pucResult = xmlStrncatNew(pucHeader, pucResult, -1);
	  xmlFree(pucRelease);
	  xmlFree(pucHeader);
	  if (xmlStrlen(pucResult) > PARENTSTRING_LENGTH_MAX) {
	    break;
	  }
	}
      }
    }
    else if (IS_NODE_DIR(pndI)) {
      if (pndI==pndN) {
        /* ignore first section node itself */
      }
      else {
        xmlChar *pucHeader = NULL;
        xmlChar *pucAttrText;
        if ((pucAttrText = xmlGetProp(pndI,BAD_CAST"name"))
            && xmlStrlen(pucAttrText) > 0) {
          xmlChar *pucRelease;
          pucHeader = xmlStrncatNew(pucAttrText, BAD_CAST " :: ", -1);
          pucRelease = pucResult;
          pucResult = xmlStrncatNew(pucHeader, pucResult, -1);
          xmlFree(pucRelease);
          xmlFree(pucHeader);
	  if (xmlStrlen(pucResult) > PARENTSTRING_LENGTH_MAX) {
            break;
          }
        }
      }
    }
  }

  if (pucResult) {
    if (xmlStrlen(pucResult) > 0) {
      return pucResult;
    }
    else {
      xmlFree(pucResult);
    }
  }
  return NULL;
}
/* end of pieGetParentHeaderStr() */



#ifdef TESTCODE
#include "test/test_pie_text.c"
#endif
