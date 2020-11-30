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
#include <libxml/uri.h>

#include "basics.h"
#include <res_node/res_node_ops.h>
#include <cxp/cxp.h>
#include "dom.h"
#include <cxp/cxp_dir.h>
#include "utils.h"
#include "plain_text.h"

#ifdef HAVE_LIBSQLITE3
#include <database/cxp_database.h>
#endif
#ifdef HAVE_PIE
#include <pie/pie_text.h>
#include <ics/ics.h>
#include <vcf/vcf.h>
#endif
#ifdef HAVE_RP
#include <rp/rp.h>
#endif
#ifdef HAVE_JSON
#include <json/json.h>
#endif
#ifdef HAVE_LIBARCHIVE
#include <archive/archive.h>
#endif

static BOOL_T
AddNameSpaces(xmlNodePtr pndPie);

static void
addUrlEncoding(xmlNodePtr pndArg);

static BOOL_T
SetTopPrefix(xmlNodePtr pndArg, resNodePtr prnArg, cxpContextPtr pccArg);

static xmlNodePtr
GrepDirNew(xmlNodePtr pndArg, const pcre2_code *re_grep, cxpContextPtr pccArg);


/*!
*/
BOOL_T
AddNameSpaces(xmlNodePtr pndPie)
{
  if (pndPie) {
    /* these are namespace declarations only */
    xmlSetProp(pndPie, BAD_CAST "xmlns:exif", BAD_CAST "http://www.w3.org/2003/12/exif/ns");
    xmlSetProp(pndPie, BAD_CAST "xmlns:jpeg", BAD_CAST CXP_VER_URL "/jpeg");
    xmlSetProp(pndPie, BAD_CAST "xmlns:svg",  BAD_CAST "http://www.w3.org/2000/svg");
    xmlSetProp(pndPie, BAD_CAST "xmlns:tiff", BAD_CAST CXP_VER_URL "/tiff");
    /*!\todo add other namespaces TIFF etc. */
    xmlSetProp(pndPie, BAD_CAST "xmlns:xapMM", BAD_CAST "http://ns.adobe.com/xap/1.0/mm/");

    return TRUE;
  }
  return FALSE;
}
/* end of AddNameSpaces() */


/*! \return enum value according to attribute "verbosity" of pndArgFile

- "0" -- directory elements only

- "1" -- directory elements and file elements (default)

- "2" -- directory elements, file elements and file system attributes (size,mtime,...)

- "3" -- directory elements, file elements, file system attributes, file owner and MIME attributes

- "4" -- directory elements, file elements, file system attributes, file owner, MIME attributes, parsed meta information (structure, id3 tags, image information) and parsed XML content

 */
int
dirMapInfoVerbosity(xmlNodePtr pndArgFile, cxpContextPtr pccArg)
{
  int iResult = RN_INFO_INFO;
  xmlChar *pucAttrVerbosity;

  /* map integer attribute to info level */
  pucAttrVerbosity = domGetAttributePtr(pndArgFile, BAD_CAST "verbosity");

  if (domGetAttributePtr(pndArgFile, BAD_CAST "igrep") != NULL
      || domGetAttributePtr(pndArgFile, BAD_CAST "grep") != NULL) {
    iResult |= RN_INFO_CONTENT;
  }
  else if (STR_IS_EMPTY(pucAttrVerbosity)) {
    /* keep default */
  }
  else if (xmlStrEqual(pucAttrVerbosity, BAD_CAST "5")) {
    iResult = RN_INFO_MAX;
  }
  else if (xmlStrEqual(pucAttrVerbosity, BAD_CAST "4")) {
    iResult =  RN_INFO_META | RN_INFO_STRUCT | RN_INFO_XML | RN_INFO_PIE | RN_INFO_CONTENT;
  }
  else if (xmlStrEqual(pucAttrVerbosity, BAD_CAST "3")) {
    iResult =  RN_INFO_META | RN_INFO_STRUCT | RN_INFO_XML | RN_INFO_INDEX;
  }
  else if (xmlStrEqual(pucAttrVerbosity, BAD_CAST "2")) {
    iResult = RN_INFO_META | RN_INFO_STRUCT;
  }
  else {
    /* keep default */
  }

  cxpCtxtLogPrint(pccArg,2,"Using '%s' verbosity '%i'", pndArgFile->name, iResult);

  return iResult;
} /* end of dirMapInfoVerbosity() */


/*! transform a dir tree containing content DOMs into an according tree containing grep results

\param pndArg node pointer to all parser options

 */
xmlNodePtr
GrepDirNew(xmlNodePtr pndArg, const pcre2_code *re_grep, cxpContextPtr pccArg)
{
  xmlNodePtr pndResult = pndArg;

  if (IS_NODE_DIR(pndArg)) {
    xmlNodePtr pndI;

    for(pndI=pndArg->children; pndI; pndI=pndI->next) {
      GrepDirNew(pndI,re_grep,pccArg);
    }
  }
  else if (IS_NODE_FILE(pndArg)) {
    xmlNodePtr pndContent;
    xmlNodePtr pndGrep;

    pndContent = pndArg->children;
    xmlUnlinkNode(pndContent);

    pndGrep = xmlNewNode(NULL,NAME_GREP);
    if (pndGrep) {
#if 1
      if (domGrepRegExpInTree(pndGrep,pndContent,re_grep)) {
	xmlAddChild(pndArg,pndGrep);
      }
      else {
	xmlFreeNode(pndGrep);
      }
      xmlFreeNode(pndContent);
#else
      if (domGrepRegExpInTree(pndGrep,pndContent,re_grep)) {
	xmlAddChild(pndArg,pndContent);
	xmlAddChild(pndArg,pndGrep);
      }
      else {
	xmlFreeNode(pndContent);
	xmlFreeNode(pndGrep);
      }
#endif
    }
  }
  else {
    /* ignore */
  }

  return pndResult;
} /* end of GrepDirNew() */


/*! process the "dir" and "file" childs of pndArgDir

\param pndArgDir node pointer to all parser options
\param prnArg filesystem context

\todo additional argument depth_to_go for recursion limit
 */
xmlDocPtr
dirProcessDirNode(xmlNodePtr pndArgDir, resNodePtr prnArgContext, cxpContextPtr pccArg)
{
  xmlChar *pucAttrWrite;
  xmlChar *pucAttrGrep;
  xmlChar *pucAttrRoot;
  xmlChar *pucAttrDepth;
  xmlChar *pucAttrMatch;
  int iDepth;
  int iVerbosity;
  int iOptions = FS_PARSE_DEFAULT;

  xmlChar mpucT[BUFFER_LENGTH];
  xmlDocPtr pdocResult = NULL;
  xmlNodePtr pndPie = NULL;
  xmlNodePtr pndMeta = NULL;
  xmlNodePtr pndEntry = NULL;
  pcre2_code *re_grep  = NULL;
  pcre2_code *re_match = NULL;
#ifdef HAVE_PCRE2
  int erroffset = 0;
  int opt_grep_pcre = 0;
  int opt_match_pcre = PCRE2_UTF;
#endif
  BOOL_T fLocator;
  
#ifdef HAVE_CGI
  fLocator = TRUE;
#else
  fLocator = FALSE;
#endif

  if (IS_VALID_NODE(pndArgDir) == FALSE) {
    return NULL;
  }

  iVerbosity = dirMapInfoVerbosity(pndArgDir, pccArg);

  /* get default depth from attribute */
  if ((pucAttrDepth = domGetAttributePtr(pndArgDir,BAD_CAST "depth"))!=NULL
      && ((iDepth = atoi((char *)pucAttrDepth)) > -1)) {
  }
  else {
    iDepth = 999;
  }
  cxpCtxtLogPrint(pccArg,2,"Set default DIR depth to '%i'", iDepth);
  
#ifdef HAVE_PCRE2
  if (((pucAttrGrep = domGetAttributePtr(pndArgDir, BAD_CAST "igrep")) != NULL
    && xmlStrlen(pucAttrGrep) > 0)
    ||
    ((pucAttrGrep = domGetAttributePtr(pndArgDir, BAD_CAST "grep")) != NULL
    && xmlStrlen(pucAttrGrep) > 0)) {
    size_t erroroffset;
    int errornumber;

    if (domGetAttributeNode(pndArgDir, BAD_CAST "igrep")) {
      cxpCtxtLogPrint(pccArg,2, "Use caseless file grep '%s' in %s with depth '%i'", pucAttrGrep, NAME_DIR, iDepth);
      opt_grep_pcre |= PCRE2_CASELESS;
    }
    else {
      cxpCtxtLogPrint(pccArg,2, "Use file grep '%s' in %s with depth '%i'", pucAttrGrep, NAME_DIR, iDepth);
    }

    re_grep = pcre2_compile(
      (PCRE2_SPTR8)pucAttrGrep, /* the pattern */
      PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
      opt_grep_pcre,         /* default options */
      &errornumber,          /* for error number */
      &erroroffset,          /* for error offset */
      NULL);                 /* use default compile context */

    if (re_grep == NULL) {
      /* regexp error handling */
      cxpCtxtLogPrint(pccArg,1, "File greping regexp '%s' error: '%i'", &pucAttrGrep[erroffset], errornumber);
      return NULL;
    }
  }

  if (((pucAttrMatch = domGetAttributePtr(pndArgDir, BAD_CAST "imatch")) != NULL
    && xmlStrlen(pucAttrMatch) > 0)
    ||
    ((pucAttrMatch = domGetAttributePtr(pndArgDir, BAD_CAST "match")) != NULL
    && xmlStrlen(pucAttrMatch) > 0)) {
    size_t erroroffset;
    int errornumber;

    if (domGetAttributeNode(pndArgDir, BAD_CAST "imatch")) {
      cxpCtxtLogPrint(pccArg,2, "Use caseless file match '%s' in %s with depth '%i'", pucAttrMatch, NAME_DIR, iDepth);
      opt_match_pcre |= PCRE2_CASELESS;
    }
    else {
      cxpCtxtLogPrint(pccArg,2, "Use file match '%s' in %s with depth '%i'", pucAttrMatch, NAME_DIR, iDepth);
    }

    re_match = pcre2_compile(
      (PCRE2_SPTR8)pucAttrMatch, /* the pattern */
      PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
      opt_match_pcre,         /* default options */
      &errornumber,          /* for error number */
      &erroroffset,          /* for error offset */
      NULL);                 /* use default compile context */

    if (re_match == NULL) {
      /* regexp error handling */
      cxpCtxtLogPrint(pccArg,1, "File matching regexp '%s' error: '%i'", &pucAttrMatch[erroffset], errornumber);
      return NULL;
    }
  }
#endif

  if (domGetAttributeFlag(pndArgDir,BAD_CAST "hidden",FALSE)) {
    cxpCtxtLogPrint(pccArg,2,"Read hidden directories");
    iOptions |= FS_PARSE_HIDDEN;
  }
  else {
    cxpCtxtLogPrint(pccArg,2,"Ignore hidden directories");
  }

  /*!\todo use exclude="(~|)" (or: mach="" type="exclude") */

  pdocResult = xmlNewDoc(BAD_CAST "1.0");
  /*!\todo use dir as root node */
  pndPie = xmlNewDocNode(pdocResult, NULL, NAME_PIE, NULL);
  xmlSetProp(pndPie, BAD_CAST "class", BAD_CAST "directory");
  xmlDocSetRootElement(pdocResult, pndPie);
  /* this is default namespace */
  /*!\todo xmlNewNs(pndPie, "http://", NULL); */ /* no prefix string for this ns */

#ifdef HAVE_LIBMAGICK
  if (iLevelVerbose > 2) {
    AddNameSpaces(pndPie);
  }
#endif

  pndMeta = xmlNewChild(pndPie, NULL, NAME_META, NULL);
  cxpInfoProgram(pndMeta, pccArg);
  xmlAddChild(pndMeta, xmlCopyNode(pndArgDir, 1));
  /* Get the current time. */
  domSetPropEat(pndMeta, BAD_CAST "ctime", GetNowFormatStr(BAD_CAST "%s"));
  domSetPropEat(pndMeta,BAD_CAST "ctime2", GetDateIsoString(0));

  for (pndEntry = ((pndArgDir->children) ? pndArgDir->children : pndArgDir); pndEntry; pndEntry = pndEntry->next) {
    int iVerbosityChild = iVerbosity;
    int iDepthChild;
    resNodePtr prnT;
    xmlNodePtr pndT;

    if (domGetAttributePtr(pndEntry, BAD_CAST "verbosity")) {
      iVerbosityChild = dirMapInfoVerbosity(pndArgDir, pccArg);
    }

    /*! get depth attribute per single dir element, instead of global */
    if ((pucAttrDepth = domGetAttributePtr(pndEntry,BAD_CAST "depth"))!=NULL
	&& ((iDepthChild = atoi((char *)pucAttrDepth)) > -1)) {
    }
    else {
      iDepthChild = iDepth;
    }
    cxpCtxtLogPrint(pccArg,2,"Set DIR depth to '%i'", iDepthChild);
  
    prnT = NULL;
    
    if (IS_VALID_NODE(pndEntry) == FALSE) {
      /* skip invalid nodes */
    }
    else if (prnT == NULL && IS_NODE_DIR(pndEntry) && (prnT = cxpResNodeResolveNew(pccArg, pndEntry, NULL, (CXP_O_READ | CXP_O_DIR))) == NULL) {
    }
    else if (prnT == NULL && IS_NODE_FILE(pndEntry) && (prnT = cxpResNodeResolveNew(pccArg, pndEntry, NULL, (CXP_O_READ | CXP_O_FILE))) == NULL) {
    }
    else if (prnT == NULL) {
      /* ignore nodes other than "cxp:dir" or "cxp:file" */
    }
    else if (resNodeListParse(prnT, iDepthChild, re_match) == FALSE) { /*! read Resource Node as list of childs */
      xmlNewChild(pndPie, NULL, BAD_CAST"error", BAD_CAST"parse");
    }
    else if (resNodeUpdate(prnT, iVerbosityChild, NULL, NULL) == FALSE) { /*! read Resource Node as list of childs */
      xmlNewChild(pndPie, NULL, BAD_CAST"error", BAD_CAST"parse");
    }
    else if ((pndT = resNodeListToDOM(prnT, iVerbosityChild)) != NULL) {
      if (re_grep) {
	GrepDirNew(pndT,re_grep,pccArg); /*! replace content DOM of pndT by grep result DOM */
      }
      SetTopPrefix(pndT, prnT, pccArg);
      xmlAddChild(pndPie, pndT);
    }
    else {
      xmlNewChild(pndPie, NULL, BAD_CAST"error", BAD_CAST"access");
    }    
    resNodeFree(prnT);
  }
  
  if (domGetAttributeFlag(pndArgDir,BAD_CAST "urlencode",fLocator)) {
    /* add RFC1738 escaped attributes */
    addUrlEncoding(pndPie);
  }

#ifdef HAVE_PCRE2
  if (re_match)
    pcre2_code_free(re_match);
  if (re_grep)
    pcre2_code_free(re_grep);
#endif

  return pdocResult;
} /* end of dirProcessDirNode() */


/*! Adds an RFC1738 encoded attributes 'urlname' and 'urlprefix' to dir and file nodes
*/
void
addUrlEncoding(xmlNodePtr pndArg)
{
  xmlChar *pucAttrName;
  xmlNodePtr pndChild;

  if (IS_NODE_PIE(pndArg) || IS_NODE_ARCHIVE(pndArg)) {
    for (pndChild=pndArg->children; pndChild; pndChild=pndChild->next) {
      addUrlEncoding(pndChild);
    }
  }
  else if (IS_NODE_DIR(pndArg)) {
    if ((pucAttrName = domGetAttributePtr(pndArg,BAD_CAST "name")) != NULL) {
      domSetPropEat(pndArg, BAD_CAST "urlname", EncodeRFC1738(pucAttrName));
    }

    if ((pucAttrName = domGetAttributePtr(pndArg,BAD_CAST "prefix")) != NULL) {
      domSetPropEat(pndArg, BAD_CAST "urlprefix", EncodeRFC1738(pucAttrName));
    }

    for (pndChild=pndArg->children; pndChild; pndChild=pndChild->next) {
      addUrlEncoding(pndChild);
    }
  }
  else if (IS_NODE_FILE(pndArg)) {
    if ((pucAttrName = domGetAttributePtr(pndArg, BAD_CAST "name")) != NULL) {
      domSetPropEat(pndArg, BAD_CAST "urlname", EncodeRFC1738(pucAttrName));
    }

    if ((pucAttrName = domGetAttributePtr(pndArg,BAD_CAST "prefix")) != NULL) {
      domSetPropEat(pndArg, BAD_CAST "urlprefix", EncodeRFC1738(pucAttrName));
    }

    for (pndChild=pndArg->children; pndChild; pndChild=pndChild->next) {
      if (IS_NODE_ARCHIVE(pndChild)) {
	addUrlEncoding(pndChild);
      }
    }
  }
  else {
    /* ignore */
  }
}
/* end of addUrlEncoding() */


/*! 
  \param 
  \return TRUE if successful
*/
BOOL_T
SetTopPrefix(xmlNodePtr pndArg, resNodePtr prnArg, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;

  if (pndArg != NULL) {

    fResult = TRUE;

#if HAVE_CGI
    if (pccArg != NULL) {
      xmlChar* pucAttrRoot;

      /* cut a named root path from prefix attribute values */
      pucAttrRoot = resNodeGetNameNormalized(cxpCtxtRootGet(pccArg));
      if (STR_IS_EMPTY(pucAttrRoot)) {
	pucAttrRoot = resNodeGetNameNormalized(cxpCtxtLocationGet(pccArg));
	if (STR_IS_EMPTY(pucAttrRoot)) {
	  pucAttrRoot = domGetAttributePtr(pndArg, BAD_CAST "root");
	}
      }

      if (STR_IS_NOT_EMPTY(pucAttrRoot)) {
	xmlChar* pucAttrPrefix;

	pucAttrPrefix = domGetAttributePtr(pndArg, BAD_CAST "prefix");
	if (STR_IS_NOT_EMPTY(pucAttrPrefix)) {
	  xmlChar* pucT;

	  resPathCutTrailingChars(pucAttrPrefix);
	  pucT = resPathDiffPtr(pucAttrRoot, pucAttrPrefix);
	  if (pucT == NULL) {
	    xmlSetProp(pndArg, BAD_CAST "name", BAD_CAST".");
	    xmlUnsetProp(pndArg, BAD_CAST "prefix");
	  }
	  else if (xmlStrlen(pucT) == 0) { /* prefix and root are equal */
	    xmlUnsetProp(pndArg, BAD_CAST "prefix");
	  }
	  else {
	    pucT = xmlStrdup(pucT);
	    resPathChangeToSlashes(pucT);
	    domSetPropEat(pndArg, BAD_CAST "prefix", pucT);
	  }
	}
	else {
	  /* no prefix required */
	}
      }
  }
#else
    xmlSetProp(pndArg, BAD_CAST "prefix", resNodeGetNameBaseDir(prnArg));
#endif
  }
  return fResult;
} /* end of SetTopPrefix() */


/*! Resource Node List from DOM

\param prnArg -- resNode tree to build as DOM
\param iArgOptions
\return DOM tree representing prnArg
*/
resNodePtr
dirNodeToResNodeList(xmlNodePtr pndArg)
{
  resNodePtr prnResult = NULL;
  xmlChar* pucName;
  resNodePtr prnNew;
  xmlNodePtr pndI;
  xmlChar* pucAttrMap;

  pucAttrMap = domGetAttributePtr(pndArg, BAD_CAST "map"); /* alternative name */
  pucName = domGetAttributePtr(pndArg, BAD_CAST"name");

  if (IS_NODE_DIR(pndArg) && STR_IS_NOT_EMPTY(pucName)) {
    xmlChar* pucPrefix;

    prnNew = resNodeSplitStrNew(pucName);
    resNodeSetType(prnNew, rn_type_dir);
    if (resPathIsRelative(pucAttrMap)) {
      resNodeSetNameAlias(prnNew, pucAttrMap);
    }
    for (pndI=pndArg->children; pndI; pndI=pndI->next) {
      resNodeAddChild(prnNew, dirNodeToResNodeList(pndI));
    }

    if ((pucPrefix = domGetAttributePtr(pndArg, BAD_CAST"prefix"))) {
      //prnResult = resNodeSplitStrNew(pucPrefix);
      //resNodeAddChild(resNodeGetLastDescendant(prnResult), prnNew);
      if (resNodeSetNameBaseDir(prnNew, pucPrefix)) {
      }
    }
    prnResult = prnNew;
  }
  else if (IS_NODE_FILE(pndArg) && STR_IS_NOT_EMPTY(pucName)) {
    /*!\bug handle URL etc */
    prnNew = resNodeSplitStrNew(pucName);
    resNodeSetType(prnNew, rn_type_file);
    if (resPathIsRelative(pucAttrMap)) {
      resNodeSetNameAlias(prnNew, pucAttrMap);
    }
    prnResult = prnNew;
  }
  else if (IS_NODE_PIE(pndArg) || IS_NODE_DIR(pndArg)) {
    for (pndI=pndArg->children; pndI; pndI=pndI->next) {
      if ((prnNew = dirNodeToResNodeList(pndI))) {
	if (prnResult) {
	  resNodeAddSibling(prnResult,prnNew);
	}
	else {
	  prnResult = prnNew;
	}
      }
    }
  }
  return prnResult;
} /* end of dirNodeToResNodeList() */


#ifdef TESTCODE
#include "test/test_cxp_dir.c"
#endif

