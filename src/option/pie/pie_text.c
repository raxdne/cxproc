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

/*!\todo remove calls of cxpCtxtLogPrint() */

/*!\todo autodetection encoding (BOM) */

/*!\todo autodetection input language */

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
#include "utils.h"
#include <vcf/vcf.h>

#ifdef HAVE_PETRINET
#include <petrinet/petrinet.h>
#endif

#ifdef HAVE_JS
#include <script/script.h>
#endif

static void
TraverseIncludeNodes(xmlNodePtr pndArg, cxpContextPtr pccArg);

static void
TraverseImportNodes(xmlNodePtr pndArg, cxpContextPtr pccArg);

static xmlNodePtr
TraverseScriptNodes(xmlNodePtr pndCurrent, cxpContextPtr pccArg);

static BOOL_T
ProcessPieDoc(xmlNodePtr pndArgResult, xmlDocPtr pdocArgPie, cxpContextPtr pccArg);

static BOOL_T
ProcessIncludeNode(xmlNodePtr pndArgInclude, cxpContextPtr pccArg);

static BOOL_T
IncludeNodeFile(xmlNodePtr pndArgInclude, cxpContextPtr pccArg);

static BOOL_T
ProcessImportOptions(xmlNodePtr pndArgPie, xmlNodePtr pndArgImport, cxpContextPtr pccArg);

static BOOL_T
ProcessImportNode(xmlNodePtr pndArgImport, cxpContextPtr pccArg);

static BOOL_T
ImportNodeFile(xmlNodePtr pndArgImport, cxpContextPtr pccArg);

static BOOL_T
ImportNodeContent(xmlNodePtr pndArgImport, cxpContextPtr pccArg);

static BOOL_T
ImportNodeCxp(xmlNodePtr pndArgImport, cxpContextPtr pccArg);

static BOOL_T
ImportNodeStdin(xmlNodePtr pndArgImport, cxpContextPtr pccArg);

static void
SetPropXpathInBlock(xmlNodePtr pndArg, xmlChar* pucArgPrefix);

static lang_t
GetPieNodeLang(xmlNodePtr pndArg, cxpContextPtr pccArg);

static BOOL_T
IncrementWeightPropRecursive(xmlNodePtr pndArg);

static int
IncrementWeightProp(xmlNodePtr pndArg, int iArg);

static BOOL_T
pieSubstSkip(xmlNodePtr pndArg);


/*! exit procedure for this module
*/
void
pieTextCleanup(void)
{
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
      if ((pucResult = domGetPropValuePtr(pndT, BAD_CAST"context"))
	|| (pucResult = domGetPropValuePtr(pndT, BAD_CAST"prefix"))) {
	pucResult = xmlStrdup(pucResult);
	break;
      }
    }

    if (STR_IS_EMPTY(pucResult) &&  pndArg != NULL && pndArg->doc != NULL && STR_IS_NOT_EMPTY(pndArg->doc->URL)) {
      pucResult = resPathGetBasedirStr(BAD_CAST pndArg->doc->URL);
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


/*!
\param pndArg node
\return enum value of attribute "lang" or environment variable or LANG_DEFAULT
*/
lang_t
GetPieNodeLang(xmlNodePtr pndArg, cxpContextPtr pccArg)
{
  lang_t eLangResult = LANG_DEFAULT;
  xmlChar *pucT = NULL;

  if ((pucT = domGetPropValuePtr(pndArg, BAD_CAST "lang")) == NULL
      && (pucT = cxpCtxtEnvGetValueByName(pccArg, BAD_CAST "CXP_LANG")) == NULL
      && (pucT = cxpCtxtEnvGetValueByName(pccArg, BAD_CAST "LANG")) == NULL) {
    /* neither node attribute nor environment variable defined */
  }
  else if (xmlStrEqual(pucT, BAD_CAST"de")) {
    eLangResult = LANG_DE;
  }
  else if (xmlStrEqual(pucT, BAD_CAST"fr")) {
    eLangResult = LANG_FR;
  }
  xmlFree(pucT);
  
  return eLangResult;
} /* End of GetPieNodeLang() */


/*! \return TRUE if subtree of pndArg has to be skipped
 */
BOOL_T
pieSubstSkip(xmlNodePtr pndArg)
{
  return (pndArg == NULL || IS_NODE_PIE_META(pndArg) || IS_NODE_PIE_LINK(pndArg) || IS_NODE_PIE_HTAG(pndArg) || IS_NODE_PIE_TAGS(pndArg) || IS_NODE_PIE_PRE(pndArg) || IS_NODE_PIE_TT(pndArg));
} /* End of pieSubstSkip() */


/*! apply the substitution of 'pndArgSubst' on 'pndArgTop' or all child substitutions of 'pndArgTop' if 'pndArgSubst' is NULL

\param pndArgTop a xmlNodePtr to apply substitutions
\param pndArgSubst a xmlNodePtr to a subst node to apply
\param pccArg the context

\return TRUE if successful
 */
BOOL_T
pieSubstInChildNodes(xmlNodePtr pndArgTop, xmlNodePtr pndArgSubst, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;

  if (IS_VALID_NODE(pndArgTop) == FALSE) {
    /* ignore non-valid elements */
  }
  else if (pndArgSubst == NULL) {
    /* subst nodes not yet detected */
    xmlNodePtr pndT;
    xmlNodePtr pndChild;
    xmlNodePtr pndNextChild;

    for (pndChild = pndArgTop->children; pndChild; pndChild = pndNextChild) {
      pndNextChild = pndChild->next;

      if (IS_NODE_SUBST(pndChild)) {
	cxpCtxtLogPrint(pccArg, 2, "New substitution found ''");
	fResult |= pieSubstInChildNodes(pndArgTop, pndChild, pccArg);
      }
      else if (IS_NODE_PIE_BLOCK(pndChild)
	|| IS_NODE_PIE_SECTION(pndChild)
	) {
	fResult |= pieSubstInChildNodes(pndChild, NULL, pccArg);
      }
    }
  }
  else if (IS_NODE_SUBST(pndArgSubst) && IS_VALID_NODE(pndArgSubst)) {
    cxpSubstPtr pcxpSubstT;

    if ((pcxpSubstT = cxpSubstDetect(pndArgSubst, pccArg))) {
      fResult = TRUE;
      pcxpSubstT->pPredicateSkip = pieSubstSkip;

      if (
#ifdef HAVE_PCRE2
	cxpSubstGetRegExp(pcxpSubstT) != NULL ||
#endif
	cxpSubstGetNamePtr(pcxpSubstT) != NULL
	) {

	if (pndArgTop->parent != NULL && pndArgTop->parent->type == XML_DOCUMENT_NODE) {
	  if (IS_VALID_NODE(pndArgTop)) {
	    cxpSubstApply(pndArgTop, pcxpSubstT, pccArg);
	  }
	}
	else {
	  if (IS_VALID_NODE(pndArgSubst->next)) {
	    cxpSubstApply(pndArgSubst->next, pcxpSubstT, pccArg);
	  }
	}
	xmlUnlinkNode(pndArgSubst);
	xmlFreeNode(pndArgSubst);
      }
      cxpSubstFree(pcxpSubstT);
    }
  }
  return fResult;
} /* end of pieSubstInChildNodes() */


/*! process the PIE child instructions of pndArgPie

\param pdocArgPie DOM to process a pie tree
\param pccArg the processing context

\return new allocated pie DOM if successful, else NULL
*/
xmlDocPtr
pieProcessPieNode(xmlNodePtr pndArgPie, cxpContextPtr pccArg)
{
  xmlNodePtr pndMeta;
  //xmlNodePtr pndError;
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
#ifdef HAVE_CGI
	xmlSetProp(pndBlock, BAD_CAST"context", resNodeGetNameRelative(cxpCtxtRootGet(pccArg), prnDoc));
#else
	xmlSetProp(pndBlock, BAD_CAST"context", resNodeGetURI(prnDoc));
#endif
	resNodeFree(prnDoc);
      }
    }
    xmlAddChild(pndPieRoot, pndBlock);
    xmlSetTreeDoc(pndPieRoot, pdocResult);
    xmlDocSetRootElement(pdocResult, pndPieRoot);

    if (domGetPropFlag(pndPieRoot, NAME_PIE_IMPORT, TRUE)) {
      cxpContextPtr pccImport = pccArg;

      RecognizeIncludes(pndBlock);
      TraverseIncludeNodes(pndBlock, pccImport); /* #inlude: parse and insert only, no recursion, no own subst, no imports, no block, no locators */
      RecognizeSubsts(pndBlock);
      RecognizeImports(pndBlock);

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
      cxpCtxtLogPrint(pccArg, 3, "Ignoring import markup");
    }
  
    pndMeta = xmlNewChild(pndPieRoot, NULL, NAME_META, NULL);
    xmlSetTreeDoc(pndMeta, pdocResult);
    cxpInfoProgram(pndMeta, pccArg);
    pndMakePieCopy = xmlCopyNode(pndArgPie, 1); /* second copy for meta element */
    /*! \todo avoid copy of complete pie tree here */
    xmlAddChild(pndMeta, pndMakePieCopy);
    /* Get the current time. */
    //xmlSetProp(pndMeta, BAD_CAST"tzname", tzGetId(0));    
    domSetPropEat(pndMeta, BAD_CAST "ctime", GetNowFormatStr(BAD_CAST "%s"));
    domSetPropEat(pndMeta, BAD_CAST "ctime2", GetDateIsoString(0));

    /*! \todo add error logs to DOM */
    //pndError = xmlNewChild(pndPieRoot, NULL, NAME_ERROR, NULL);

    if (domGetPropFlag(pndArgPie, BAD_CAST  NAME_PIE_SCRIPT, TRUE)) {
      cxpCtxtLogPrint(pccArg, 2, "Recognize scripts");
      RecognizeScripts(pndPieRoot);
#ifdef HAVE_JS
      TraverseScriptNodes(pndPieRoot, pccArg);
#endif
    }
    else {
      cxpCtxtLogPrint(pccArg, 3, "Ignoring scripts");
    }

    /* process all child subst nodes */
    cxpCtxtLogPrint(pccArg, 2, "Start substitution");
    pieSubstInChildNodes(pndBlock, NULL, pccArg);

    /* replace all subst nodes in tree by its result */
    cxpCtxtLogPrint(pccArg, 2, "Start node substitution");
    cxpSubstReplaceNodes(pndBlock, pccArg);

    ProcessImportOptions(pndPieRoot,pndArgPie, pccArg);
  
    pucAttr = domGetPropValuePtr(pndArgPie, BAD_CAST "xpath"); /*  */
    if (STR_IS_NOT_EMPTY(pucAttr) && xmlStrEqual(pucAttr,BAD_CAST"/*") == FALSE) {
      xmlDocPtr pdocResultXPath;
      
      cxpCtxtLogPrint(pccArg, 2, "Locator XPath for '%s'", pucAttr);
      if ((pdocResultXPath = domGetXPathDoc(pdocResult, pucAttr)) != NULL) {
	xmlChar* pucRegExpTag = NULL;

	/* collect all NAME_PIE_PI_TAG in current DOM and add result to sub-DOM pdocResultXPath */
	if ((pndPieRoot = xmlDocGetRootElement(pdocResultXPath)) != NULL
	  && (pucRegExpTag = GetBlockTagRegExpStr(xmlDocGetRootElement(pdocResult), NULL, TRUE)) != NULL) {
	  xmlAddChild(pndPieRoot, xmlNewPI(NAME_PIE_PI_TAG, pucRegExpTag));
	  xmlFree(pucRegExpTag);
	}

	xmlFreeDoc(pdocResult);
	pdocResult = pdocResultXPath;
      }
    }

    /*!\todo update meta element */
    
    if (domGetPropFlag(pndArgPie, BAD_CAST "tags", TRUE)) {
      cxpCtxtLogPrint(pccArg, 2, "Recognize tags");
      ProcessTags(pdocResult, NULL);
    }
    else {
      cxpCtxtLogPrint(pccArg, 3, "Ignoring tags");
    }

    pucAttr = domGetPropValuePtr(pndArgPie, BAD_CAST "pattern"); /*  */
    if (STR_IS_NOT_EMPTY(pucAttr)) {
      xmlChar *pucTT;

      pucTT = xmlStrdup(BAD_CAST"//*[");
      /*\bug replace quotes by apostrophs */
      pucTT = xmlStrcat(pucTT,pucAttr);
      pucTT = xmlStrcat(pucTT, BAD_CAST"]");
      /*\todo check XPAth format of pucTT */
      cxpCtxtLogPrint(pccArg, 2, "Filter XPath for '%s'", pucTT);
      if (pieWeightXPathInDoc(pdocResult, pucTT)) {
	/* some matching nodes found, remove others */
	CleanUpTree(pndPieRoot);
      }
      else {
	/* no matching nodes found */
	xmlFreeDoc(pdocResult);
	pdocResult = NULL;
      }
      xmlFree(pucTT);
    }
    else {
      pucAttr = domGetPropValuePtr(pndArgPie, BAD_CAST "regexp"); /*  */
      if (STR_IS_NOT_EMPTY(pucAttr)) {
	cxpCtxtLogPrint(pccArg, 2, "Filter RegExp for '%s'", pucAttr);
	if (pieWeightRegExpInDoc(pdocResult, pucAttr)) {
	  /* some matching nodes found, remove others */
	  CleanUpTree(pndPieRoot);
	}
	else {
	  /* no matching nodes found */
	  xmlFreeDoc(pdocResult);
	  pdocResult = NULL;
	}
      }
      else {
	pucAttr = domGetPropValuePtr(pndArgPie, BAD_CAST "words"); /*  */
	if (STR_IS_NOT_EMPTY(pucAttr)) {
	  cxpCtxtLogPrint(pccArg, 2, "Filter RegExp for '%s'", pucAttr);
	  if (pieWeightWordsInBlocks(pdocResult, pucAttr)) {
	    /* some matching nodes found, remove others */
	    CleanUpTree(pndPieRoot);
	  }
	  else {
	    /* no matching nodes found */
	    xmlFreeDoc(pdocResult);
	    pdocResult = NULL;
	  }
	}
      }
    }

    pieRemoveInvalidsFromTree(pndPieRoot);
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
	    ProcessImportOptions(pndPieProcRoot,pndPieRoot,pccArg);
	    RecognizeIncludes(pndPieProcRoot);
	    TraverseIncludeNodes(pndPieProcRoot, pccArg);
	    RecognizeSubsts(pndPieProcRoot);
	    RecognizeImports(pndPieProcRoot);
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
    xmlChar *pucContent = NULL;

    pucContent = cxpProcessPlainNode(pndChild, pccArg);
    if (STR_IS_NOT_EMPTY(pucContent)) {
      xmlNodePtr pndBlock;
      xmlNodePtr pndRelease;

      fResult = TRUE;

      pndRelease = pndArgImport->children;
      domUnlinkNodeList(pndRelease);
      xmlFreeNodeList(pndRelease);

      pndBlock = pndArgImport;
      xmlSetNs(pndBlock,NULL);
      xmlNodeSetName(pndBlock, NAME_PIE_BLOCK);
      //xmlSetProp(pndBlock, BAD_CAST "context", resNodeGetURI(prnInput));

      if (ParsePlainBuffer(pndBlock, pucContent, GetModeByAttr(pndArgImport))) {
	ProcessImportOptions(pndBlock,pndChild,pccArg);
	RecognizeIncludes(pndBlock);
	TraverseIncludeNodes(pndBlock, pccArg);
	RecognizeSubsts(pndBlock);
	RecognizeImports(pndBlock);
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
#ifdef HAVE_CGI
    xmlSetProp(pndBlock, BAD_CAST"context", resNodeGetNameRelative(cxpCtxtRootGet(pccArg), prnInput));
#else
    xmlSetProp(pndBlock, BAD_CAST"context", resNodeGetURI(prnInput));
#endif

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
      pucAttrType = domGetPropValuePtr(pndBlock, BAD_CAST"type");

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
	|| xmlStrEqual(pucAttrType, BAD_CAST"par")))
	|| iMimeType == MIME_TEXT_CSV
	|| iMimeType == MIME_TEXT_PLAIN
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
	    ProcessImportOptions(pndBlock,pndArgImport,pccArg);
	    RecognizeIncludes(pndBlock);
	    TraverseIncludeNodes(pndBlock, pccInput);

	    if (domGetPropFlag(pndArgImport, BAD_CAST "subst", TRUE)) {
	      RecognizeSubsts(pndBlock);
	    }

	    RecognizeImports(pndBlock);
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
	       && (pucAttrType != NULL && (xmlStrEqual(pucAttrType, BAD_CAST NAME_PIE_SCRIPT)
					   || iMimeType == MIME_APPLICATION_X_JAVASCRIPT))) {

	cxpCtxtLogPrint(pccInput, 2, "Importing '%s' Javascript result as PIE '%s'", pucAttrType, resNodeGetNameNormalized(prnInput));

	if ((pucContent = cxpCtxtCacheGetBuffer(pccInput, resNodeGetNameNormalized(prnInput))) == NULL) {
	  /* there is no cached Buffer */
	  pucContent = plainGetContextTextEat(prnInput, 1024);
	}

	/*!\todo process MIME_APPLICATION_JSON too */

	if (STR_IS_NOT_EMPTY(pucContent)) {
	  xmlChar *pucScriptResult = NULL;

	  xmlSetProp(pndBlock, BAD_CAST"type", BAD_CAST NAME_PIE_SCRIPT);
#ifdef HAVE_JS
	  pucScriptResult = scriptProcessScriptNode(pndBlock, pccInput);
#endif
	  if (pucScriptResult == NULL) {
	    xmlSetProp(pndBlock, BAD_CAST"result", BAD_CAST"empty");
	  }
	  else if (ParsePlainBuffer(pndBlock, pucScriptResult, GetModeByAttr(pndBlock))) {
	    RecognizeIncludes(pndBlock);
	    TraverseIncludeNodes(pndBlock, pccInput);
	    RecognizeSubsts(pndBlock);
	    RecognizeImports(pndBlock);
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
	  xmlSetProp(pndBlock, BAD_CAST"error", BAD_CAST NAME_PIE_SCRIPT);
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
	  ProcessImportOptions(pndBlock,pndArgImport,pccArg);
	  RecognizeIncludes(pndBlock);
	  TraverseIncludeNodes(pndBlock, pccInput);
	  RecognizeSubsts(pndBlock);
	  RecognizeImports(pndBlock);
	  TraverseImportNodes(pndBlock, pccInput); /* parse result recursively */
	}
	else {
	  cxpCtxtLogPrint(pccInput, 1, "Cant read from '%s'", resNodeGetNameNormalized(prnInput));
	  xmlSetProp(pndBlock, BAD_CAST"error", BAD_CAST"parse");
	}
      }

      xmlUnsetProp(pndBlock, BAD_CAST "name");
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
  BOOL_T fResult = TRUE;
  xmlChar *pucContent = NULL;
  xmlNodePtr pndBlock = NULL;

#ifdef DEBUG
  cxpCtxtLogPrint(pccArg, 3, "ImportNodeContent(pndArgImport=%0x,pccArg=%0x)", pndArgImport, pccArg);
#endif
  assert(IS_NODE_PIE_IMPORT(pndArgImport));
  assert(NodeHasSingleText(pndArgImport));

  pndBlock = pndArgImport;
  xmlSetNs(pndBlock, NULL);

  if (xmlStrEqual(domGetPropValuePtr(pndBlock, BAD_CAST "type"), BAD_CAST NAME_PIE_SCRIPT) &&
      domGetAncestorsPropFlag(pndBlock, BAD_CAST "script", TRUE) == FALSE) {
    xmlNodeSetName(pndBlock, NAME_PIE_PRE);
  }
  else if (xmlStrEqual(domGetPropValuePtr(pndBlock, BAD_CAST "type"), BAD_CAST NAME_PIE_SCRIPT)) {
#ifdef HAVE_JS
    xmlNodeSetName(pndBlock, NAME_PIE_BLOCK);
    pucContent = scriptProcessScriptNode(pndBlock, pccArg);
    xmlFreeNode(pndArgImport->children);
    pndArgImport->children = pndArgImport->last = NULL; /* unlink node content */
#else
    xmlNodeSetName(pndBlock, NAME_PIE_PRE);
#endif
  }
  else {
    pucContent = domNodeEatContent(pndArgImport);
    xmlNodeSetName(pndBlock, NAME_PIE_BLOCK);
    xmlFreeNode(pndArgImport->children);
    pndArgImport->children = pndArgImport->last = NULL; /* unlink node content */
  }

  if (STR_IS_NOT_EMPTY(pucContent)) {
    if (ParsePlainBuffer(pndBlock, pucContent, GetModeByAttr(pndBlock))) {
      ProcessImportOptions(pndBlock,pndArgImport,pccArg);
      RecognizeIncludes(pndBlock);
      TraverseIncludeNodes(pndBlock, pccArg);
      RecognizeSubsts(pndBlock);
      RecognizeImports(pndBlock);
      TraverseImportNodes(pndBlock, pccArg); /* parse result recursively */
    }
    else {
      xmlSetProp(pndBlock, BAD_CAST"error", BAD_CAST"parse");
    }
  }
  else {
    xmlSetProp(pndBlock, BAD_CAST"result", BAD_CAST"empty");
  }
  xmlFree(pucContent);

  return fResult;
}
/* end of ImportNodeContent() */


/*! traverse DOM of pndArg searching for include nodes in context of pccArg and
* replaces include node by his processing result if pndArgTop is NULL, else append result to pndArgTop

\param pndArg node to test for include, else traversing childs
\param pccArg the processing context
*/
void
TraverseIncludeNodes(xmlNodePtr pndArg, cxpContextPtr pccArg)
{
  if (IS_VALID_NODE(pndArg) == FALSE) {
    /* ignore NULL and invalid elements */
  }
  else if (IS_NODE_PIE_SCRIPT(pndArg) || IS_NODE_PIE_PRE(pndArg) ) {
    /* skip */
  }
  else if (IS_NODE_PIE_INCLUDE(pndArg)) {
    if (ProcessIncludeNode(pndArg, pccArg)) {
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
      if (IS_NODE_PIE_INCLUDE(pndArgChildren)
	|| IS_NODE_PIE_SECTION(pndArgChildren)
	|| IS_NODE_PIE_TASK(pndArgChildren)
	|| IS_NODE_PIE_PIE(pndArgChildren)
	|| IS_NODE_PIE_BLOCK(pndArgChildren)) {
	TraverseIncludeNodes(pndArgChildren, pccArg);
      }
      pndArgChildren = pndNext;
    }
  }
}
/* end of TraverseIncludeNodes() */


/*! process the single include node pndArgInclude in context of pccArg and replace it

\param pndArgInclude node to test for include, else traversing childs
\param pccArg the processing context

\return TRUE if include was successful, else FALSE
*/
BOOL_T
ProcessIncludeNode(xmlNodePtr pndArgInclude, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;

#ifdef DEBUG
  cxpCtxtLogPrint(pccArg, 3, "ProcessIncludeNode(pndArgInclude=%0x,pccArg=%0x)", pndArgInclude, pccArg);
#endif

  if (pndArgInclude != NULL
    && (pndArgInclude->parent || pndArgInclude->prev || pndArgInclude->next) /* because of replacing pndArgInclude by include result */
    && IS_NODE_PIE_INCLUDE(pndArgInclude)) {
    cxpContextPtr pccHere = pccArg;	      /*! process context for include */
    cxpContextPtr pccDoc = NULL;	      /*! process context for include (derived from DOM) */
    xmlChar *pucAttrName = NULL;
    xmlNodePtr pndT;
    xmlDocPtr pdocT = NULL;

    //pccHere = cxpCtxtFromAttr(pccArg, pndArgInclude);
    //pccDoc = cxpCtxtFromAttr(NULL, pndArgInclude);
    pucAttrName = domGetPropValuePtr(pndArgInclude, BAD_CAST"name");

    if (resPathIsStd(pucAttrName)) {

    }
#if 0
    else if ((pdocT = cxpCtxtCacheGetDoc(pccHere, pucAttrName)) != NULL) {
      /* matching DOM in cache found */
      if ((pndT = xmlDocGetRootElement(pdocT)) != NULL && pndT->children != NULL && (pndT = xmlCopyNodeList(pndT->children)) != NULL) {
	fResult = (xmlAddChildList(pndArgInclude, pndT) != NULL);
	xmlNodeSetName(pndArgInclude, NAME_PIE_BLOCK);
      }
    }
#endif
    else if (STR_IS_NOT_EMPTY(pucAttrName)) {
      fResult = IncludeNodeFile(pndArgInclude, pccHere);
    }
    else {
      xmlAddChild(pndArgInclude, xmlNewComment(BAD_CAST"unknown content type"));
    }

    cxpCtxtFree(pccDoc);
    if (pccHere != pccArg) {
      cxpCtxtIncrExitCode(pccArg, cxpCtxtGetExitCode(pccHere));
    }
  }
  return fResult;
}
/* end of ProcessIncludeNode() */


/*! process the include instructions of pndArgInclude in directory context of pccArg

\param pndArgTop node to append processing result
\param pndArgInclude node to test for include, else traversing childs
\param pccArg the processing context

\return TRUE if include was successful, else FALSE
*/
BOOL_T
IncludeNodeFile(xmlNodePtr pndArgInclude, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;
  resNodePtr prnInput = NULL; /*! context for include of new document (avoid side effect) */

#ifdef DEBUG
  cxpCtxtLogPrint(pccArg, 1, "IncludeNodeFile(pndArgInclude=%0x,pccArg=%0x)", pndArgInclude, pccArg);
#endif
  assert(pndArgInclude->parent != NULL);
  assert(IS_NODE_PIE_INCLUDE(pndArgInclude));

  prnInput = cxpResNodeResolveNew(pccArg, pndArgInclude, NULL, CXP_O_READ);
  if (prnInput) {
    BOOL_T fLocator;
    xmlNodePtr pndBlock = NULL;

#ifdef HAVE_CGI
    fLocator = TRUE;
#else
    fLocator = FALSE;
#endif

    resNodeResetMimeType(prnInput);

    /*! \todo add cache handling for include */

    pndBlock = xmlCopyNode(pndArgInclude,0);
    xmlSetNs(pndBlock,NULL);
    xmlNodeSetName(pndBlock,NAME_PIE_BLOCK);
#ifdef HAVE_CGI
    xmlSetProp(pndBlock, BAD_CAST"context", resNodeGetNameRelative(cxpCtxtRootGet(pccArg), prnInput));
#else
    xmlSetProp(pndBlock, BAD_CAST"context", resNodeGetURI(prnInput));
#endif

    //xmlSetProp(pndBlock, BAD_CAST "tags", BAD_CAST "no");

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
      pucAttrType = domGetPropValuePtr(pndBlock, BAD_CAST"type");

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
	&& ((pucAttrType != NULL
	&& (xmlStrEqual(pucAttrType, BAD_CAST"line")
	|| xmlStrEqual(pucAttrType, BAD_CAST"csv")
	|| xmlStrEqual(pucAttrType, BAD_CAST"log")
	|| xmlStrEqual(pucAttrType, BAD_CAST"markdown")
	|| xmlStrEqual(pucAttrType, BAD_CAST"par")))
	|| iMimeType == MIME_TEXT_CSV
	|| iMimeType == MIME_TEXT_PLAIN
#ifdef WITH_MARKDOWN
	|| iMimeType == MIME_TEXT_MARKDOWN
#endif
	|| iMimeType == MIME_TEXT_CALENDAR)) {
	rmode_t m;

	cxpCtxtLogPrint(pccInput, 2, "Including '%s' PIE '%s'", pucAttrType, resNodeGetNameNormalized(prnInput));

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
	    //TraverseIncludeNodes(pndBlock, pccInput); /* parse result recursively */
	  }
	  else {
	    xmlSetProp(pndBlock, BAD_CAST"error", BAD_CAST"parse");
	  }
	  //domPutNodeString(stderr,BAD_CAST "IncludeNodeFile()",pndArgResult);
	}
	else {
	  cxpCtxtLogPrint(pccInput, 1, "Cant read from '%s'", resNodeGetNameNormalized(prnInput));
	  xmlSetProp(pndBlock, BAD_CAST"error", BAD_CAST"empty");
	}
	xmlFree(pucContent);
      }
      else if ((pucAttrType != NULL && xmlStrEqual(pucAttrType, BAD_CAST"xml"))
	|| iMimeType == MIME_TEXT_XML
	|| iMimeType == MIME_APPLICATION_PIE_XML) {
	xmlDocPtr pdocPie;

	cxpCtxtLogPrint(pccInput, 2, "Including XML PIE '%s'", (pucAttrCache ? pucAttrCache : resNodeGetNameNormalized(prnInput)));

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
	  //TraverseIncludeNodes(pndBlock, pccInput); /* parse result recursively */
	}
	else {
	  cxpCtxtLogPrint(pccInput, 1, "Cant read from '%s'", resNodeGetNameNormalized(prnInput));
	  xmlSetProp(pndBlock, BAD_CAST"error", BAD_CAST"parse");
	}
      }

      //domPutNodeString(stderr, BAD_CAST "IncludeNodeFile()", pndBlock);
      if (pndBlock != NULL && pndBlock->children != NULL) {
	RecognizeSubsts(pndBlock);
	domReplaceNodeList(pndArgInclude, pndBlock->children);
	xmlFreeNodeList(pndArgInclude);
	xmlFreeNode(pndBlock);
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
/* end of IncludeNodeFile() */


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
  else if (IS_NODE_PIE_SCRIPT(pndArg) || IS_NODE_PIE_PRE(pndArg) ) {
    /* skip */
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
	|| IS_NODE_PKG2_STATE(pndArgChildren)
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


/*! process the import node pndArgImport attributes in context of pccArg

\param pndArgImport node for import, else traversing childs
\param pccArg the processing context

\return TRUE if import was successful, else FALSE
*/
BOOL_T
ProcessImportOptions(xmlNodePtr pndArgPie, xmlNodePtr pndArgImport, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;

#ifdef DEBUG
  cxpCtxtLogPrint(pccArg, 3, "ProcessImportOptions(pndArgImport=%0x,pccArg=%0x)", pndArgImport, pccArg);
#endif

  if (pndArgPie) {
    
    if (domGetPropFlag(pndArgImport, BAD_CAST "figure", TRUE)) {
      cxpCtxtLogPrint(pccArg, 2, "Recognize Figures");
      RecognizeFigures(pndArgPie);
    }
    else {
      cxpCtxtLogPrint(pccArg, 3, "Ignoring Figures markup");
    }

    RecognizeInlines(pndArgPie);

    if (domGetPropFlag(pndArgImport, BAD_CAST "url", TRUE)) {
      cxpCtxtLogPrint(pccArg, 2, "Recognize URLs");
      /*! \todo use an attribute for regexp?? */
      RecognizeUrls(pndArgPie);
    }
    else {
      cxpCtxtLogPrint(pccArg, 3, "Ignoring URLs");
    }

    RecognizeSymbols(pndArgPie, GetPieNodeLang(pndArgPie, pccArg));
    RecognizeDates(pndArgPie,MIME_TEXT_PLAIN);

    /*! \todo global cite recognition in scientific text */

    if (domGetPropFlag(pndArgImport, BAD_CAST "todo", TRUE)) {
      cxpCtxtLogPrint(pccArg, 2, "Recognize tasks markup");
      RecognizeTasks(pndArgPie);
    }
    else {
      cxpCtxtLogPrint(pccArg, 3, "Ignoring tasks markup");
    }

    if (domGetPropFlag(pndArgImport, BAD_CAST "locators", TRUE)) {
      resNodePtr prnDoc;

      cxpCtxtLogPrint(pccArg, 2, "Add locator attribute");
      if (pndArgImport->doc != NULL && (prnDoc = resNodeDirNew(BAD_CAST pndArgImport->doc->URL)) != NULL) {
	SetPropXpathInBlock(pndArgPie, NULL);
	resNodeFree(prnDoc);
      }
      else {
	SetPropXpathInBlock(pndArgPie, NULL);
      }
    }
    else {
      cxpCtxtLogPrint(pccArg, 3, "Skipping locators");
    }
  }
  return fResult;
} /* end of ProcessImportOptions() */


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
    pucAttrName = domGetPropValuePtr(pndArgImport, BAD_CAST"name");

    if (resPathIsStd(pucAttrName)) {
      fResult = ImportNodeStdin(pndArgImport, pccHere);
    }
    else if ((pdocT = cxpCtxtCacheGetDoc(pccHere, pucAttrName)) != NULL) {
      /* matching DOM in cache found */
      if ((pndT = xmlDocGetRootElement(pdocT)) != NULL && pndT->children != NULL && (pndT = xmlCopyNodeList(pndT->children)) != NULL) {
	fResult = (xmlAddChildList(pndArgImport, pndT) != NULL);
	xmlNodeSetName(pndArgImport, NAME_PIE_BLOCK);
	RecognizeIncludes(pndArgImport);
	TraverseIncludeNodes(pndArgImport, pccHere);
	RecognizeSubsts(pndArgImport);
	RecognizeImports(pndArgImport);
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
    xmlChar* pucT;
    xmlNodePtr pndChild;
    int iLengthStr;

    if (IS_NODE_PIE_IMPORT(pndCurrent) && xmlStrEqual(domGetPropValuePtr(pndCurrent,BAD_CAST"type"),BAD_CAST NAME_PIE_SCRIPT)) {
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
    else if (IS_NODE_SCRIPT(pndCurrent)) {
      if ((pucT = scriptProcessScriptNode(pndCurrent, pccArg)) != NULL) {
	if ((pndResult = xmlNewText(pucT)) != NULL) {
	  xmlReplaceNode(pndCurrent, pndResult);
	  xmlFreeNode(pndCurrent);
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


/*! \return a readable/identifiable sub-tree copy for pndArg
*/
xmlNodePtr
pieGetSelfAncestorNodeList(xmlNodePtr pndArg, xmlChar *pucArgId)
{
  xmlNodePtr pndResult = NULL;

  if (IS_NODE_PIE_DATE(pndArg)) {
    xmlNodePtr pndA = pndArg; /* ancestor axis */

    if ((pndResult = xmlNewNode(NULL, NAME_PIE_BLOCK))) {

      xmlSetProp(pndResult, BAD_CAST"idref", pucArgId);

      pndA = pndA->parent;
      if (IS_NODE_PIE_PAR(pndA)) { /* p/date */
	xmlNodePtr pndPar;

	pndPar = xmlCopyNode(pndA, 1);

	for ( ; IS_NODE_PIE_PAR(pndA->parent) || IS_NODE_PIE_LIST(pndA->parent); pndA = pndA->parent) {
	  /* skip all list parents */
	  /*!\todo handle parent p for context */
	}

	pndA = pndA->parent;
	if (IS_NODE_PIE_SECTION(pndA) || IS_NODE_PIE_TASK(pndA)) { /* section/p/date or task/p/date */
	  xmlNodePtr pndI, pndT;

	  pndI = xmlNewNode(NULL, pndA->name);
	  domCopyPropList(pndI, pndA);

	  if ((pndT = domGetFirstChild(pndA, NAME_PIE_HEADER)) != NULL) {
	    xmlAddChild(pndI, xmlCopyNode(pndT, 1));
	  }
	  xmlAddChild(pndI, pndPar);

	  pndA = pndA->parent;
	  if (IS_NODE_PIE_SECTION(pndA)) { /* section/section/header/date */
	    xmlNodePtr pndII;

	    pndII = xmlNewNode(NULL, pndA->name);
	    domCopyPropList(pndII, pndA);

	    if ((pndT = domGetFirstChild(pndA, NAME_PIE_HEADER)) != NULL) {
	      xmlAddChild(pndII, xmlCopyNode(pndT, 1));
	    }
	    xmlAddChild(pndII, pndI);
	    xmlAddChild(pndResult, pndII);
	  }
	  else {
	    xmlAddChild(pndResult, pndI);
	  }
	}
	else { /* p */
	  xmlAddChild(pndResult, pndPar);
	}
      }
      else if (IS_NODE_PIE_HEADER(pndA)) { /* header/date */
	xmlNodePtr pndH;

	pndH = xmlCopyNode(pndA, 1);

	pndA = pndA->parent;
	if (IS_NODE_PIE_SECTION(pndA) || IS_NODE_PIE_TASK(pndA)) { /* section/header/date */
	  xmlNodePtr pndI, pndT;

	  xmlFreeNode(pndH);
	  pndI = xmlCopyNode(pndA, 1);

	  pndA = pndA->parent;
	  if (IS_NODE_PIE_SECTION(pndA)) { /* section/section/header/date */
	    xmlNodePtr pndII;

	    pndII = xmlNewNode(NULL, pndA->name);
	    domCopyPropList(pndII, pndA);

	    if ((pndT = domGetFirstChild(pndA, NAME_PIE_HEADER)) != NULL) {
	      xmlAddChild(pndII, xmlCopyNode(pndT, 1));
	    }
	    xmlAddChild(pndII, pndI);
	    xmlAddChild(pndResult, pndII);
	  }
	  else {
	    xmlAddChild(pndResult, pndI);
	  }
	}
	else { /* p */
	  xmlAddChild(pndResult, pndH);
	}
      }
    }
    else {
    }

    if (pndResult->children) {
      /* OK */
    }
    else {
      xmlFreeNode(pndResult);
      pndResult = NULL;
    }
  }
  else if (IS_NODE_PIE_TR(pndArg)) { /*! tr/td/date */
    if ((pndResult = xmlCopyNode(pndArg, 1))) {
      xmlNodePtr pndIter;

      xmlNodeSetName(pndResult, NAME_PIE_BLOCK);
      xmlSetProp(pndResult, BAD_CAST"idref", pucArgId);

      for (pndIter=pndResult->children; pndIter; ) {
	xmlNodePtr pndIterNext;
	
	pndIterNext = pndIter->next;
	if (IS_NODE_PIE_TD(pndIter) || IS_NODE_PIE_TH(pndIter)) {
	  if (pndIter->children == NULL) {
	    xmlUnlinkNode(pndIter);
	    xmlFreeNodeList(pndIter);
	  }
	  else {
	    xmlNodeSetName(pndIter, NAME_PIE_PAR);
	  }
	}
	else {
	}
	pndIter = pndIterNext;
      }
    }
  }
  
  return pndResult;
} /* end of pieGetSelfAncestorNodeList() */


/*! Adds attributes bxpath to all descendant element nodes.
\todo restrict recursion depth
\param pndArg pointer to node to add attribute
\param pucArgPrefix pointer to XPath prefix for childs
 */
void
SetPropXpathInBlock(xmlNodePtr pndArg, xmlChar* pucArgPrefix)
{
  if (IS_NODE_META(pndArg) || IS_NODE_ERROR(pndArg)) {
  }
  else if (IS_ENODE(pndArg)) {
    xmlNodePtr pndChild;
    int i=0;

    for (pndChild = pndArg->children; pndChild != NULL; pndChild = pndChild->next) {
      i++;
      if ( ! IS_ENODE(pndChild) || xmlHasProp(pndChild, BAD_CAST"bxpath")) {
      }
      else if (IS_NODE_PIE_TTAG(pndChild) || IS_NODE_PIE_ETAG(pndChild) || IS_NODE_PIE_HTAG(pndChild) || IS_NODE_PIE_META(pndChild) || IS_NODE_ERROR(pndChild)) {
	/* dont set xpath attribute here */
      }
      else {
	xmlChar mucT[BUFFER_LENGTH];

	xmlStrPrintf(mucT, BUFFER_LENGTH, "%s/*[%i]", (pucArgPrefix==NULL ? BAD_CAST "/*" : pucArgPrefix), i);
	xmlSetProp(pndChild, BAD_CAST"bxpath", mucT);
	  
	SetPropXpathInBlock(pndChild, mucT);
      }
    }
  }
} /* end of SetPropXpathInBlock() */


/*! increments value of property "w" by iArg numerically
\param pndArg node for attribute
\param iArg default integer value
*/
int
IncrementWeightProp(xmlNodePtr pndArg, int iArg)
{
  int iResult = 0;

  if ((pndArg != NULL) && (pndArg->type == XML_ELEMENT_NODE) && iArg != 0) {
    int iCurrent;
    xmlAttrPtr patT;

    if ((patT = xmlHasProp(pndArg, BAD_CAST"w")) == NULL
      || patT->children == NULL || STR_IS_EMPTY(patT->children->content)) {
      /* there is no attribute value yet, initial value '1' */
      iResult = 1;
    }
    else if ((iCurrent = atoi((const char *)patT->children->content)) != 0) {
      iResult = iCurrent + iArg;
    }
    else {
      /*\todo remove property if iArg == 0? */
    }

    if (iResult) {
      xmlChar mucCount[32];

      xmlStrPrintf(mucCount, sizeof(mucCount), "%i", iResult);
      xmlSetProp(pndArg, BAD_CAST"w", mucCount);
    }
  }
  return iResult;
} /* end of IncrementWeightProp() */


/*!
 */
BOOL_T
IncrementWeightPropRecursive(xmlNodePtr pndArg)
{
  BOOL_T fResult = FALSE;

  if (pndArg) {
    xmlNodePtr pndT;

    IncrementWeightProp(pndArg, 1);

    for (pndT = pndArg->children; pndT; pndT = pndT->next) {
      IncrementWeightPropRecursive(pndT);
    }

    fResult = TRUE;
  }

  return fResult;
} /* end of IncrementWeightPropRecursive() */


/*! \return TRUE if a node according to XPath 'pucArg' in pdocArg was found
\param pdocArg source DOM
\param pucArg pointer to XPath string
*/
BOOL_T
pieWeightXPathInDoc(xmlDocPtr pdocArg, xmlChar *pucArg)
{
  BOOL_T fResult = FALSE;

  if (pdocArg != NULL && STR_IS_NOT_EMPTY(pucArg)) {
    xmlNodePtr pndRoot;

    if ((pndRoot = xmlDocGetRootElement(pdocArg))) {
      xmlXPathObjectPtr result;

      if ((result = domGetXPathNodeset(pdocArg, pucArg)) != NULL) {
	int i;
	xmlNodeSetPtr nodeset;

	nodeset = result->nodesetval;
	if (nodeset->nodeNr > 0) {
	  for (i=0; i < nodeset->nodeNr; i++) {
	    xmlNodePtr pndT;

	    if (IS_NODE_PIE_HEADER(nodeset->nodeTab[i])) {
	      /* weight the tree of this section when element matches */
	      IncrementWeightPropRecursive(nodeset->nodeTab[i]->parent);
	      /* weight all ancestors of this section */
	      for (pndT=nodeset->nodeTab[i]->parent->parent; pndT; pndT=pndT->parent) {
		IncrementWeightProp(pndT, 1);
	      }
	    }
	    else {
	      /* weight all ancestors and this element */
	      for (pndT=nodeset->nodeTab[i]; pndT; pndT=pndT->parent) {
		IncrementWeightProp(pndT, 1);
	      }
	    }
	  }
	  fResult = TRUE;
	}
	xmlXPathFreeObject(result);
      }
    }
  }
  return fResult;
} /* end of pieWeightXPathInDoc() */


/*! increments the weight property of pndArg if its text block matches preArg

\return TRUE by
*/
BOOL_T
RecognizeRegExps(xmlNodePtr pndArg, pcre2_code* preArg)
{
  BOOL_T fResult = FALSE;
  xmlNodePtr pndIter;
  int errornumber = 0;
  size_t erroroffset;

  if (preArg == NULL) {
    /* regexp error handling */
    //PrintFormatLog(1, "hashtag regexp '%s' error: '%i'", RE_HASHTAG, errornumber);
  }
  else if (IS_VALID_NODE(pndArg) == FALSE || xmlHasProp(pndArg,BAD_CAST"hidden") != NULL) {
    /* skip */
  }
  else if (IS_NODE_PIE_HEADER(pndArg) || IS_NODE_PIE_PAR(pndArg) || IS_NODE_PIE_LIST(pndArg)
	   || IS_NODE_PIE_FIG(pndArg) || IS_NODE_PIE_TR(pndArg) || IS_NODE_PIE_IMG(pndArg)
	   || IS_NODE_PIE_BLOCKQUOTE(pndArg) || IS_NODE_PIE_PRE(pndArg)) {
    xmlChar* pucT;

    pucT = domNodeListGetString(pndArg,NULL);
    if (STR_IS_NOT_EMPTY(pucT)) { /* pndIter is a text node */
      int rc;
      pcre2_match_data* match_data;

      match_data = pcre2_match_data_create_from_pattern(preArg, NULL);
      rc = pcre2_match(
		       preArg,        /* result of pcre2_compile() */
		       (PCRE2_SPTR8)pucT,  /* the subject string */
		       xmlStrlen(pucT),             /* the length of the subject string */
		       0,              /* start at offset 0 in the subject */
		       0,              /* default options */
		       match_data,        /* vector of integers for substring information */
		       NULL);            /* number of elements (NOT size in bytes) */

      if (rc > -1) {
	xmlNodePtr pndT;

	if (IS_NODE_PIE_HEADER(pndArg)) {
	  /* weight the tree of this section when element matches */
	  IncrementWeightPropRecursive(pndArg->parent);
	  /* weight all ancestors of this section */
	  for (pndT = pndArg->parent->parent; pndT; pndT = pndT->parent) {
	    IncrementWeightProp(pndT, 1);
	  }
	}
	else {
	  /* weight all ancestors and this element */
	  for (pndT = pndArg; pndT; pndT = pndT->parent) {
	    IncrementWeightProp(pndT, 1);
	  }
	}
	fResult = TRUE;
      }
      else {
      }
      pcre2_match_data_free(match_data);   /* Release memory used for the match */
    }
    xmlFree(pucT);
  }
  else {
    for (pndIter = pndArg->children; pndIter != NULL; pndIter = pndIter->next) {
      fResult |= RecognizeRegExps(pndIter, preArg);
    }
  }
  return fResult;
} /* End of RecognizeRegExps() */


/*! \return Regexp 'pucArg'
\param pucArg pointer to Regexp string

CASELESS, ignoring space, minus, underscore
*/
pcre2_code* 
_GetFuzzyRegExp(xmlChar *pucArg)
{
  pcre2_code* preResult = NULL;

  if (STR_IS_NOT_EMPTY(pucArg)) {
    size_t l;
    size_t erroroffset;
    int errornumber = 0;
    xmlChar *pucFuzzy = NULL;

    l = xmlStrlen(pucArg) * 4;
    if (l > 1) {

      pucFuzzy = (xmlChar *) xmlMalloc(l);
      if (pucFuzzy) {
	size_t i, j;
	
	for (i=j=0; j < l; i++, j++) {
	  if (isend(pucArg[i])) {
	    pucFuzzy[j] = '\0';
	    break;
	  }
	  else if (pucArg[i]==' ' || pucArg[i]=='-' || pucArg[i]=='_') {
	    /*!\bug if it's a complex regexp "a[a-z]+" etc */
	    pucFuzzy[j++] = '.';
	    pucFuzzy[j] = '*';
	    for ( ; pucArg[i+1]==' ' || pucArg[i+1]=='-' || pucArg[i+1]=='_'; i++) ;
	  }
	  else {
	    //pucFuzzy[j++] = '.';
	    //pucFuzzy[j++] = '*';
	    pucFuzzy[j] = pucArg[i];
	  }
	  assert(i<l);
	}
      }
    }
    
    preResult = pcre2_compile(
			      (PCRE2_SPTR8)pucFuzzy, /* the pattern */
			      PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
			      PCRE2_UTF | PCRE2_CASELESS | PCRE2_MULTILINE,        /* default options */
			      &errornumber,          /* for error number */
			      &erroroffset,          /* for error offset */
			      NULL);                 /* use default compile context */
  }

  return preResult;
} /* End of _GetFuzzyRegExp() */


/*! \return TRUE if a node according to Regexp 'pucArg' in pdocArg was found
\param pdocArg source DOM
\param pucArg pointer to Regexp string
*/
BOOL_T
pieWeightRegExpInDoc(xmlDocPtr pdocArg, xmlChar *pucArg)
{
  BOOL_T fResult = FALSE;

  if (pdocArg != NULL && STR_IS_NOT_EMPTY(pucArg)) {
    xmlNodePtr pndRoot;    
    if ((pndRoot = xmlDocGetRootElement(pdocArg))) {
      pcre2_code* re_pattern = NULL;
      
      if ((re_pattern = _GetFuzzyRegExp(pucArg)) != NULL) {
	PrintFormatLog(2, "Initialized filter regexp with '%s'", pucArg);
	RecognizeRegExps(pndRoot, re_pattern);
	pcre2_code_free(re_pattern);
	fResult = TRUE;
      }
      else {
	/* regexp error handling */
	PrintFormatLog(1, "Filter regexp '%s' error", pucArg);
      }
    }
  }
  return fResult;
} /* end of _pieWeightRegExpInDoc() */


/*! \return Regexp 'pucArg'
\param pucArg pointer to Regexp string

CASELESS, ignoring space, minus, underscore

\todo order of terms
 https://stackoverflow.com/questions/19896324/match-string-in-any-word-order-regex
 https://www.regular-expressions.info/lookaround.html
*/
pcre2_code* 
GetPositiveLookaheadRegExp(xmlChar *pucArg)
{
  pcre2_code* preResult = NULL;

  if (STR_IS_NOT_EMPTY(pucArg)) {
    size_t i, j;
    size_t erroroffset;
    int errornumber = 0;
    xmlChar *pucLookahead = NULL;
	
    for (i=j=0; ; i++) {

      if (pucArg[i]==' ' || pucArg[i]==',' || isend(pucArg[i])) {
	xmlChar *pucT = NULL;
	
	pucT = xmlStrndup(&pucArg[j],i-j);

	if (pucLookahead) {
	  pucLookahead = xmlStrcat(pucLookahead,BAD_CAST"(?=.*");
	}
	else {
	  pucLookahead = xmlStrdup(BAD_CAST"(?=.*");
	}
	pucLookahead = xmlStrcat(pucLookahead,pucT);
	pucLookahead = xmlStrcat(pucLookahead,BAD_CAST")");

	xmlFree(pucT);
	
	for (; pucArg[i] == ' ' || pucArg[i] == ','; i++);

	if (isend(pucArg[i])) {
	  break;
	}

	j = i;
      }
    }
    
    preResult = pcre2_compile(
			      (PCRE2_SPTR8)(pucLookahead ? pucLookahead : pucArg), /* the pattern */
			      PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
			      PCRE2_UTF | PCRE2_CASELESS | PCRE2_MULTILINE,        /* default options */
			      &errornumber,          /* for error number */
			      &erroroffset,          /* for error offset */
			      NULL);                 /* use default compile context */

    xmlFree(pucLookahead);
  }

  return preResult;
} /* End of GetPositiveLookaheadRegExp() */


/*! \return TRUE if a node according to Regexp 'pucArg' in pdocArg was found
\param pdocArg source DOM
\param pucArg pointer to string of words
*/
BOOL_T
pieWeightWordsInBlocks(xmlDocPtr pdocArg, xmlChar *pucArg)
{
  BOOL_T fResult = FALSE;

  if (pdocArg != NULL && STR_IS_NOT_EMPTY(pucArg)) {
    xmlNodePtr pndRoot;    
    if ((pndRoot = xmlDocGetRootElement(pdocArg))) {
      pcre2_code* re_pattern = NULL;
      
      if ((re_pattern = GetPositiveLookaheadRegExp(pucArg)) != NULL) {
	PrintFormatLog(2, "Initialized filter regexp with '%s'", pucArg);
	RecognizeRegExps(pndRoot, re_pattern);
	pcre2_code_free(re_pattern);
	fResult = TRUE;
      }
      else {
	/* regexp error handling */
	PrintFormatLog(1, "Filter regexp '%s' error", pucArg);
      }
    }
  }
  return fResult;
} /* end of pieWeightWordsInBlocks() */


#ifdef TESTCODE
#include "test/test_pie_text.c"
#endif
