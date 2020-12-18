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
#include <libxml/xmlversion.h>
#include <libxml/HTMLtree.h>
#include <libxml/parser.h>
#include <libxml/relaxng.h>
#include <libxml/uri.h>

#ifdef LIBXML_THREAD_ENABLED
#include <libxml/globals.h>
#include <libxml/threads.h>
#ifdef HAVE_LIBPTHREAD
#include <pthread.h>
#elif WITH_THREAD
#endif
#endif

#ifdef HAVE_PCRE2
#include <pcre2.h>
#endif

/* 
 */
#include <libxslt/xslt.h>
#include <libxslt/xsltutils.h>
#include <libxslt/transform.h>
#include <libxslt/variables.h>

#ifdef HAVE_LIBMAGICK
#include <magick/ImageMagick.h>
#endif

#ifdef HAVE_LIBID3TAG
#include <id3tag.h>
#endif

#ifdef HAVE_LIBVORBIS
#include <vorbis/codec.h>
#endif

/*
 */
#include "basics.h"
#include "utils.h"
#include <res_node/res_node_ops.h>
#include <cxp/cxp.h>
#include <cxp/cxp_threadp.h>
#include "dom.h"
#include <cxp/cxp_dir.h>
#include <pie/pie_text.h>
#ifdef HAVE_PIE
#include <pie/pie_calendar.h>
#endif
#ifdef HAVE_PETRINET
#include <petrinet/petrinet.h>
#endif
#ifdef HAVE_LIBSQLITE3
#include <database/cxp_database.h>
#endif

#ifdef HAVE_LIBEXIF
#include <image/image_exif.h>
#endif
#ifdef HAVE_LIBMAGICK
#include <image/image.h>
#endif
#ifdef HAVE_JSON
#include <json/json.h>
#endif
#ifdef HAVE_JS
#include <script/script.h>
#endif
#ifdef HAVE_LIBARCHIVE
#include <archive/cxp_archive.h>
#endif
#ifdef HAVE_CGI
#include <cxp/cxp_context_cgi.h>
#endif

#ifdef HAVE_PCRE2
/*! regular expression string for separation of 'each' values */
#define RE_EACH "([^ \\,\\;\\r\\n]+)"
#endif

xmlNsPtr pnsCxp = NULL; /*!\todo use defined namespace "cxp" */

/* all nodes for source content */
#define IS_NODE_SOURCE(NODE) (IS_NODE_XML(NODE) || IS_NODE_XSL(NODE) || IS_NODE_INFO(NODE) || IS_NODE_XHTML(NODE) || IS_NODE_PLAIN(NODE) || IS_NODE_ZIP(NODE) || IS_NODE_IMAGE(NODE) || IS_NODE_DIR(NODE) || IS_NODE_FILE(NODE) || IS_NODE_PIE(NODE))

static BOOL_T
ValidateCxpTree(xmlNodePtr pndArg, cxpContextPtr pccArg);

static BOOL_T
ValidateSchema(const xmlDocPtr pdocArgXml, const xmlChar *pucArg, cxpContextPtr pccArg);

static BOOL_T
cxpChangeXslParam(xmlDocPtr pdocResult, char **param, BOOL_T fInsertvars, cxpContextPtr pccArg);

static xmlDocPtr
cxpXslTransformToDom(const xmlDocPtr pdocArgXml, const xmlDocPtr pdocArgXsl, char **ppchArgParam, cxpContextPtr pccArg);

static xmlChar *
cxpXslTransformToText(const xmlDocPtr pdocArgXml, const xmlDocPtr pdocArgXsl, char **ppchArgParam, cxpContextPtr pccArg);

static xmlNodePtr
cxpProcessXmlNodeEmbedded(xmlNodePtr pndArg, cxpContextPtr pccArg);

static xmlDocPtr
cxpProcessXmlChildNodes(xmlNodePtr pndArg,cxpContextPtr pccArg);

static xmlDocPtr
cxpProcessEachNode(xmlNodePtr pndArg, cxpContextPtr pccArg);

static void
cxpProcessEachSubstitute(xmlNodePtr pndArg, xmlChar *pucName, xmlChar *pucValue, cxpContextPtr pccArg);

static BOOL_T
cxpIsStorageNode(xmlNodePtr pndArg);

static xmlNodePtr
cxpGetXmlSourceNode(xmlNodePtr pndArg, cxpContextPtr pccArg);

static BOOL_T
cxpProcessSystemNode(xmlNodePtr pndArg, cxpContextPtr pccArg);

static BOOL_T
cxpViewNodeResult(xmlNodePtr pndArg, cxpContextPtr pccArg);

#ifndef HAVE_PIE

extern xmlDocPtr
pieProcessPieNode(xmlNodePtr pndMakePie, cxpContextPtr pccArg);

#endif

/*! \return TRUE if pndArg and descendants are valid against relaxng

  s. http://stackoverflow.com/questions/6436456/validating-xml-against-relax-ng-in-ansi-c

  \todo handle URL values
*/
BOOL_T
ValidateSchema(const xmlDocPtr pdocArgXml, const xmlChar *pucArg, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;
  resNodePtr prnFile = NULL; /* resource node according to pucAttrFile */

#ifdef DEBUG
  PrintFormatLog(3, "ValidateSchema(pdocArgXml=%0x,pucArg=\"%s\",pccArg=%0x);", pdocArgXml, pucArg, pccArg);
#endif

  /*! find according relaxng file */
  if (pccArg) {
    prnFile = cxpResNodeResolveNew(pccArg, xmlDocGetRootElement(pdocArgXml), BAD_CAST pucArg, CXP_O_READ);
    if (prnFile) {
      xmlRelaxNGParserCtxtPtr rngparser;

      /*! parse relaxng file */
      rngparser = xmlRelaxNGNewParserCtxt(resNodeGetNameNormalizedNative(prnFile));
      if (rngparser) {
	xmlRelaxNGPtr schema;

	schema = xmlRelaxNGParse(rngparser);
	if (schema) {
	  xmlRelaxNGValidCtxtPtr validctxt;

	  validctxt = xmlRelaxNGNewValidCtxt(schema);
	  if (validctxt) {
	    /*! validate DOM */
	    fResult = (xmlRelaxNGValidateDoc(validctxt, pdocArgXml) == 0);
	    if (fResult) {
	      PrintFormatLog(2, "Validation against '%s' successful", resNodeGetNameNormalizedNative(prnFile));
	    }
	    else {
	      PrintFormatLog(2, "Validation against '%s' failed", resNodeGetNameNormalizedNative(prnFile));
	    }
	    /*!\todo cache schema? */
	    xmlRelaxNGFreeValidCtxt(validctxt);
	  }
	  xmlRelaxNGFree(schema);
	}
	xmlRelaxNGFreeParserCtxt(rngparser);
      }
    }
    resNodeFree(prnFile);
  }
  return fResult;
}
/* end of ValidateSchema() */


/*! substitutes each occurrence of 'name' in XML_ATTRIBUTE_NODE with value
 */
void
cxpProcessEachSubstitute(xmlNodePtr pndArg, xmlChar *pucName, xmlChar *pucValue, cxpContextPtr pccArg)
{
  while (pndArg) {
    if (pndArg->type == XML_ELEMENT_NODE) {
      cxpProcessEachSubstitute((xmlNodePtr)pndArg->properties, pucName, pucValue, pccArg);
      cxpProcessEachSubstitute(pndArg->children, pucName, pucValue, pccArg);
    }
    else if (pndArg->type == XML_TEXT_NODE) {
      /*! substitution in text nodes */
      xmlChar *pucA;
      if (pndArg->content!=NULL
	  && (pucA = ReplaceStr(pndArg->content,pucName,pucValue))!=NULL) {
	cxpCtxtLogPrint(pccArg,3,"Pattern '%s' found in '%s' to '%s'", pucName, pndArg->content, pucA);
	xmlNodeSetContent(pndArg,pucA);
	xmlFree(pucA);
      }
    }
    else if (pndArg->type == XML_ATTRIBUTE_NODE) {
      /*! substitution in attribute nodes */
      xmlChar *pucA;
      if ((pucA = ReplaceStr(domNodeGetContentPtr(pndArg),pucName,pucValue))) {
	cxpCtxtLogPrint(pccArg,3,"Pattern '%s' found in '%s' '%s' to '%s'", pucName, pndArg->name, domNodeGetContentPtr(pndArg), pucA);
	xmlNodeSetContent(pndArg->children,pucA);
	xmlFree(pucA);
      }
    }
    else {
      /* ignore */
    }
    cxpProcessEachSubstitute(pndArg->next, pucName, pucValue, pccArg);
    pndArg = pndArg->next;
  }
}
/* end of cxpProcessEachSubstitute() */


/*! iteration loop

build and returns a tree of MAKE including all single steps as node copies

\param pndArg a xmlNodePtr to append data
\param pccArg the resource node

\return
*/
xmlDocPtr
cxpProcessEachNode(xmlNodePtr pndArg, cxpContextPtr pccArg)
{
  xmlChar *pucAttrName;
  xmlChar *pucAttrThread;
  xmlChar *pucI;
  xmlDocPtr pdocResult = NULL;
  xmlNodePtr pndCurrent;
  xmlNodePtr pndFrom;
  xmlNodePtr pndMake;

  if ( ! IS_NODE_EACH(pndArg)) {
    cxpCtxtLogPrint(pccArg,1,"No EACH element");
  }
  else if (IS_VALID_NODE(pndArg) == FALSE) {
    cxpCtxtLogPrint(pccArg,4,"No valid iteration EACH");
  }
  else if ((pucAttrName = domGetPropValuePtr(pndArg,BAD_CAST "name"))!=NULL
      && (xmlStrlen(pucAttrName) > 0)
      && (pndFrom = domGetFirstChild(pndArg,NAME_FROM))!=NULL ) {
    if (pndFrom->children) {
      /*
	FROM element
      */
      int i;
      xmlChar *pucSubstr;
      xmlChar *pucValues;

#ifdef HAVE_PCRE2
      if (pccArg->re_each==NULL) {
	int errornumber;
	size_t erroroffset;

	/* compile regexp first */
	cxpCtxtLogPrint(pccArg,2,"Initialize Iterator regexp '%s'", RE_EACH);
	pccArg->re_each = pcre2_compile(
	  (PCRE2_SPTR8)RE_EACH, /* the pattern */
	  PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
	  0,                     /* default options */
	  &errornumber,          /* for error number */
	  &erroroffset,          /* for error offset */
	  NULL);                 /* use default compile context */

	if (pccArg->re_each == NULL) {
	  /* regexp error handling */
	  cxpCtxtLogPrint(pccArg,1, "Splitting regexp '%s' error: '%i'", RE_EACH, errornumber);
	  return NULL;
	}
      }
#endif

      pdocResult = xmlNewDoc(BAD_CAST "1.0");
      pndMake = xmlNewDocNode(pdocResult, NULL, NAME_MAKE, NULL); 
      xmlDocSetRootElement(pdocResult,pndMake);
      /* pdocResult->encoding = xmlStrdup(BAD_CAST "ISO-8859-1"); */
      pdocResult->encoding = xmlStrdup(BAD_CAST "UTF-8"); /* according to conversion in ParseImportNodePlainContent() */
      /* set thread attribute according to pndArg */
      if ((pucAttrThread = domGetPropValuePtr(pndArg,BAD_CAST "thread"))) {
	xmlSetProp(pndMake,BAD_CAST "thread",pucAttrThread);
      }

      /* with simple text node */
      pucValues = cxpProcessPlainNode(pndFrom,pccArg);
      if (pucValues == NULL) {
	cxpCtxtLogPrint(pccArg,1,"No iteration keys for '%s'", pucAttrName);
	xmlFreeDoc(pdocResult);
	return NULL;
      }

      //cxpCtxtLogPrint(pccArg,2,"Iteration with '%s' over '%s'", pucAttrName, pucValues);

      /* copy next value for iterator */

      for (pucSubstr=pucValues, i=0; pucSubstr; i++) {
	int rc;
#ifdef HAVE_PCRE2
	PCRE2_SIZE *ovector;
	pcre2_match_data *match_data;
#else
	xmlChar *pucT;
	size_t ovector[3];
#endif

#ifdef HAVE_PCRE2
	match_data = pcre2_match_data_create_from_pattern(pccArg->re_each, NULL);
	rc = pcre2_match(
	  pccArg->re_each,        /* result of pcre2_compile() */
	  (PCRE2_SPTR8)pucSubstr,  /* the subject string */
	  xmlStrlen(pucSubstr),             /* the length of the subject string */
	  0,              /* start at offset 0 in the subject */
	  0,              /* default options */
	  match_data,        /* vector of integers for substring information */
	  NULL);            /* number of elements (NOT size in bytes) */
#else
	for (pucT = pucSubstr; isspace(*pucT); pucT++);
	ovector[0] = (pucT - pucSubstr);
	for (; isalnum(*pucT); pucT++);
	ovector[1] = (pucT - pucSubstr);

	rc = (ovector[1] - ovector[0] > 0) ? 0 : -1;
#endif

	if (rc > -1) {
#ifdef HAVE_PCRE2
	  ovector = pcre2_get_ovector_pointer(match_data);
#endif
	  cxpCtxtLogPrint(pccArg,3, "each %i..%i in '%s'", ovector[0], ovector[1], pucSubstr);
	  pucI = xmlStrndup(pucSubstr + ovector[0], (int) (ovector[1] - ovector[0]));
	  if (pucI == NULL) {
	    cxpCtxtLogPutsExit(pccArg,45,"Cant duplicate iterator");
	  }
	  
	  cxpCtxtLogPrint(pccArg,2,"Step '%s' with '%s'", pucAttrName, pucI);
	  for (pndCurrent = pndFrom->next; pndCurrent; pndCurrent = pndCurrent->next) {
	    /* copy all element nodes after FROM with childs */
	    if (pndCurrent->type == XML_ELEMENT_NODE) {
	      xmlNodePtr pndCopy = xmlCopyNode(pndCurrent,1);
	      if (pndCopy) {
		cxpProcessEachSubstitute(pndCopy, pucAttrName, pucI, pccArg);
		xmlAddChildList(pndMake,pndCopy);
	      }
	    }
	  }
	  xmlFree(pucI);
	  if (isend(*(pucSubstr + ovector[1]))) {
	    /* there are no trailing chars */
	    pucSubstr = NULL;
	  }
	  else {
	    pucSubstr += ovector[1];
	  }
	}
	else {
	  pucSubstr = NULL;
	}
#ifdef HAVE_PCRE2
	pcre2_match_data_free(match_data);   /* Release memory used for the match */
#endif
      }
      if (domGetPropFlag(pndArg,BAD_CAST "dump",FALSE)) {
	cxpCtxtLogPrintDoc(pccArg, 1, NULL, pdocResult);
      }
      xmlFree(pucValues);
    }
    else {
      cxpCtxtLogPrint(pccArg,1,"No valid iteration with '%s'", pucAttrName);
    }
  }
  else {
    cxpCtxtLogPrint(pccArg,1,"No valid attributes at EACH");
  }
  return pdocResult;
}
/* end of cxpProcessEachNode() */


/*! processes the embedded XML elements and its childs recursively

\param pndArg a xmlNodePtr to process directly (no Copy!!)
\param pccArg the resource node

DON'T change the pccArg!!!

\todo process other nodes "script" etc

\return pointer to new allocated tree, pndArg if nothing have to be replaced or NULL if pndArg have to be released
*/
xmlNodePtr
cxpProcessXmlNodeEmbedded(xmlNodePtr pndArg, cxpContextPtr pccArg)
{
  xmlNodePtr pndResult = NULL;

#ifdef DEBUG
  cxpCtxtLogPrint(pccArg,4,"cxpProcessXmlNodeEmbedded(pndArg=%0x,pccArg=%0x) in '%s'",
      pndArg,pccArg,cxpCtxtLocationGetStr(pccArg));
#endif

  assert(pndArg);
  
  if (IS_NODE_XML(pndArg)) {
    if (IS_VALID_NODE(pndArg)
      && domGetPropFlag(pndArg, BAD_CAST "eval", TRUE) == TRUE) {
      /*! 'pndArg' is a processing node 'xml', not marked as eval="no"  */
      xmlDocPtr pdocT;

      /*! process this node and register the result DOM as 'pdocT' */
      pdocT = cxpProcessXmlNode(pndArg,pccArg);
      if (pdocT) {
	xmlNodePtr pndRoot;

	pndRoot = xmlDocGetRootElement(pdocT);
	if (pndRoot) {
	  xmlNodePtr pndNew = NULL;

	  if (domDocIsHtml(pdocT)) {
	    xmlNodePtr pndToCopy;

	    if ((pndToCopy = domGetFirstChild(pndRoot,BAD_CAST"body")) != NULL && pndToCopy->children != NULL) {
	      pndNew = xmlCopyNode(pndToCopy,1);
	      xmlNodeSetName(pndNew,BAD_CAST"div"); /* append this tree as "div" element */
	    }
	    else if ((pndToCopy = domGetFirstChild(pndRoot,BAD_CAST"head")) != NULL && pndToCopy->children != NULL) {
	      pndNew = xmlCopyNode(pndToCopy,1);
	    }
	    else {
	      cxpCtxtLogPrint(pccArg,1,"No usable 'html/head/*' or 'html/body/*' result");
	    }
	  }
	  else {
	    /*!\todo recursive call of cxpProcessEmbeddedNodes(pndRoot,pccArg) */

	    /*!\todo processing without copying of tree (DANGER!!!) */

	    pndNew = xmlCopyNode(pndRoot,1);
	  }
	  /*! keep the inserted node 'pndNew' as the result */
	  pndResult = pndNew;
	}
	xmlFreeDoc(pdocT);
      }
    }
  }
  else if (pndArg != NULL && pndArg->children != NULL) {
    xmlNodePtr pndChild;

    for (pndChild = pndArg->children; pndChild;) {
      xmlNodePtr pndT;
      xmlNodePtr pndChildNext;

      pndChildNext = pndChild->next;
      pndT = cxpProcessXmlNodeEmbedded(pndChild,pccArg);
      if (pndT == NULL) {
	cxpCtxtLogPrint(pccArg,2,"Removing '%s' element",pndChild->name);
	xmlUnlinkNode(pndChild);
	xmlFreeNode(pndChild);
      }
      else if (pndT != pndChild) {
	/*! if there is a new result tree, replace the old processing node 'pndChild' with tree of resulting 'pndT' */
	cxpCtxtLogPrint(pccArg,2,"Replacing tree of '%s'",pndChild->name);

	assert(pndT->next == NULL);
	/*!\todo compare type compatibility of nodes */

	xmlReplaceNode(pndChild,pndT);
	/*! free the unlinked processing node 'pndArg': Side effect!! */
	xmlFreeNode(pndChild);
      }
      /* WARNING: pndChild was released eventually in cxpProcessEmbeddedNodes() */
      pndChild = pndChildNext;
    }
    pndResult = pndArg;
  }
  else {
    pndResult = pndArg;
  }
  return pndResult;
}
/* end of cxpProcessXmlNodeEmbedded() */


/*! \return resource node for a locator according to pccArg and pndArg
* 
* \bug handle attribute "context"!
*/
resNodePtr
cxpAttributeLocatorResNodeNew(cxpContextPtr pccArg, xmlNodePtr pndArg, xmlChar *pucArg)
{
  resNodePtr prnResult = NULL;

  if (pccArg != NULL || pndArg != NULL || pucArg != NULL) {
    xmlChar *pucAttrValue = NULL;
    resNodePtr prnT = NULL;

    if (IS_NODE_MAKE(pndArg)) {
      pucAttrValue = domGetPropValuePtr(pndArg, BAD_CAST "dir");
      if (STR_IS_EMPTY(pucAttrValue) || xmlStrEqual(pucAttrValue,BAD_CAST"pwd")) {
	/* keep location of pccArg */
	prnT = cxpCtxtLocationDup(pccArg);
      }
      else if (resPathIsAbsolute(pucAttrValue)) {
	prnT = resNodeDirNew(pucAttrValue);
	cxpCtxtLogPrint(pccArg, 3, "Get Context '%s' from attribute 'dir' absolute", resNodeGetNameNormalized(prnT));
      }
      else {
	prnT = resNodeConcatNew(cxpCtxtLocationGetStr(pccArg), pucAttrValue);
	cxpCtxtLogPrint(pccArg, 3, "Get Context '%s' from attribute 'dir' relative", resNodeGetNameNormalized(prnT));
      }
    }
    
#ifdef HAVE_PIE
    if (prnT == NULL && (pucAttrValue = pieGetAncestorContextStr(pndArg)) != NULL) {
      prnT = resNodeDirNew(pucAttrValue);
      cxpCtxtLogPrint(pccArg, 3, "Get Context '%s' from parent context", resNodeGetNameNormalized(prnT));
      xmlFree(pucAttrValue);
      pucAttrValue = NULL;
    }
#endif
    
    if (prnT == NULL && pccArg == NULL && pndArg != NULL && pndArg->doc != NULL && STR_IS_NOT_EMPTY(pndArg->doc->URL)) {
      cxpCtxtLogPrint(pccArg, 3, "Get Context from '%s' node DOM", pndArg->name);
      prnT = resNodeDirNew(BAD_CAST pndArg->doc->URL);
      resNodeSetToParent(prnT);
    }

    if (prnT) {
      if (resPathIsEquivalent(cxpCtxtLocationGetStr(pccArg), resNodeGetNameNormalized(prnT))) {
	/* to be ignore */
	resNodeFree(prnT);
	prnT = NULL;
      }
      else {
	resNodeReadStatus(prnT); /* test existence of node */
	if (resNodeIsFile(prnT)) {
	  resNodeSetToParent(prnT); /* cxp context locator is on directory level */
	}
      }
    }
    prnResult = prnT;
  }
  return prnResult;
} /* end of cxpAttributeLocatorResNodeNew() */


/*! \return resource node for a ressource according to pccArg and attributes of pndArg if access is permitted else NULL
*/
resNodePtr
cxpResNodeResolveNew(cxpContextPtr pccArg, xmlNodePtr pndArg, xmlChar *pucArg, int iArgOptions)
{
  resNodePtr prnResult = NULL;

  if (pccArg != NULL || pndArg != NULL || pucArg != NULL) {
    xmlChar *pucShortcut = pucArg;
    xmlChar *pucDocUrlDir = NULL;
    xmlChar *pucLocation = NULL;
    xmlChar *pucRootPath = NULL;
    xmlChar *pucParentPath = NULL;
    xmlChar *pucAttrName = NULL;

    if (pndArg != NULL) {
      pucAttrName = domGetPropValuePtr(pndArg, BAD_CAST "name");
      if (pucAttrName == NULL) { /* no attribute 'name' */
      }
      else if (STR_IS_EMPTY(pucAttrName) || xmlStrEqual(pucAttrName, BAD_CAST".")) { /* valid but empty attribute 'name' */
#ifdef HAVE_CGI
	pucAttrName = resNodeGetNameNormalized(cxpCtxtRootGet(pccArg));
#else
	/*\todo TBD */
#endif	
      }
      else if (iArgOptions == CXP_O_NONE) {
	/* no special focus */
      }
      else {
	/* valid attribute 'name' */
	if (cxpIsStorageNode(pndArg)) { /* => target node, must be writable */
	  iArgOptions = CXP_O_WRITE;
	  assert((iArgOptions & CXP_O_SEARCH) == 0); /* searching is not allowed */
	}
	else { /* no child elements => source node, read and search if neccessary, readable */
	  iArgOptions |= CXP_O_READ;

	  if (domGetPropFlag(pndArg, BAD_CAST"search", FALSE)) {
	    /* explicit search flag at pndArg */
	    iArgOptions |= CXP_O_SEARCH;
	  }
	}
      }

      if (pndArg->doc != NULL && STR_IS_NOT_EMPTY(pndArg->doc->URL)) {
	/* derive path prefix from DOM */
	pucDocUrlDir = resPathGetBasedir(BAD_CAST pndArg->doc->URL);
      }
    }

    if (STR_IS_EMPTY(pucShortcut)) {
      pucShortcut = pucAttrName;
    }

    if (cxpCtxtLocationGet(pccArg)) {
      pucLocation = resNodeGetNameNormalized(cxpCtxtLocationGet(pccArg));
    }

    if (cxpCtxtRootGet(pccArg)) {
      pucRootPath = resNodeGetNameNormalized(cxpCtxtRootGet(pccArg));
    }

#ifdef HAVE_PIE
    if ((pucParentPath = pieGetAncestorContextStr(pndArg)) != NULL) {
#ifdef HAVE_CGI
#elif 0
      if (resPathIsAbsolute(pucShortcut)) {
      }
      else if (resPathIsAbsolute(pucParentPath)) {
	pucShortcut = resPathConcat(pucParentPath, pucShortcut);
      }
      else {
	xmlChar *pucT;

	pucT = resPathConcat(pucLocation, pucParentPath);
	pucShortcut = resPathConcat(pucT, pucShortcut);
	xmlFree(pucT);
      }
#endif
    }
#endif

    // resPathIsInArchive(pucShortcut)

    if (STR_IS_EMPTY(pucShortcut)) {
      /* no attribute value found */
    }
    else if (resPathIsStd(pucShortcut)) {
      prnResult = resNodeDirNew(pucShortcut);
      if ((iArgOptions & CXP_O_READ)) {
	resNodeSetType(prnResult,rn_type_stdin);
	resNodeSetWrite(prnResult,FALSE);
      }
    }
    else if (resPathIsInMemory(pucShortcut)) {
      prnResult = resNodeInMemoryNew();
    }
    else if (resPathIsInArchive(pucShortcut)) {
      xmlChar *pucRelease;

      pucRelease = resPathGetPathOfArchive(pucShortcut);
      if (STR_IS_NOT_EMPTY(pucRelease)) {
	xmlChar *pucT;

	pucT = resPathGetPathInNextArchivePtr(pucShortcut);
	prnResult = cxpResNodeResolveNew(pccArg, pndArg, pucRelease, iArgOptions);
	if (STR_IS_NOT_EMPTY(pucT) && prnResult != NULL) {
	  resNodeAddChildNew(prnResult, pucT);
	}
	xmlFree(pucRelease);
      }
    }
    else if (resPathIsURL(pucShortcut)) {
      assert((iArgOptions & CXP_O_WRITE) == 0);
      prnResult = resNodeDirNew(pucShortcut);
    }
    else if (resPathIsAbsolute(pucShortcut)) {
      prnResult = resNodeDirNew(pucShortcut);
      if (IS_NODE_DIR(pndArg)) {
	resNodeSetType(prnResult, rn_type_dir);
      }
    }
    else if ((iArgOptions & CXP_O_READ)) { /* find a readable ressource */

      /*\todo use context attribute too */

      if (STR_IS_NOT_EMPTY(pucParentPath)) {
	if (resPathIsDescendant(pucParentPath, pucShortcut)) {
	  prnResult = resNodeDirNew(pucShortcut);
	}
	else {
	  prnResult = resNodeConcatNew(pucParentPath, pucShortcut);
	}
      }

      if (resNodeIsReadable(prnResult) == FALSE && STR_IS_NOT_EMPTY(pucRootPath)) {
	resNodeFree(prnResult);
	if (resPathIsDescendant(pucRootPath, pucShortcut)) {
	  prnResult = resNodeDirNew(pucShortcut);
	}
	else {
	  prnResult = resNodeConcatNew(pucRootPath, pucShortcut);
	}
      }

      if (resNodeIsReadable(prnResult) == FALSE && STR_IS_NOT_EMPTY(pucDocUrlDir)) {
	resNodeFree(prnResult);
	if (resPathIsDescendant(pucDocUrlDir, pucShortcut)) {
	  prnResult = resNodeDirNew(pucShortcut);
	}
	else {
	  prnResult = resNodeConcatNew(pucDocUrlDir, pucShortcut);
	}
      }

      if (resNodeIsReadable(prnResult) == FALSE && STR_IS_NOT_EMPTY(pucLocation)) {
	resNodeFree(prnResult);
	if (resPathIsDescendant(pucLocation, pucShortcut)) {
	  prnResult = resNodeDirNew(pucShortcut);
	}
	else {
	  prnResult = resNodeConcatNew(pucLocation, pucShortcut);
	}
      }

      if (resNodeIsReadable(prnResult) == FALSE && (iArgOptions & CXP_O_SEARCH)) {
	resNodePtr prnI;

	resNodeFree(prnResult);
	prnResult = NULL;

	for (prnI = cxpCtxtSearchGet(pccArg); prnI; prnI = resNodeGetNext(prnI)) {
	  resNodePtr prnT;

	  if ((prnT = resNodeListFindPath(prnI, pucShortcut, (RN_FIND_FILE | RN_FIND_IN_SUBDIR))) != NULL) {
	    prnResult = resNodeDup(prnT, RN_DUP_THIS); /* found in search DOM */
	    break;
	  }
	}
      }

      if (resNodeIsReadable(prnResult) && (iArgOptions & CXP_O_DIR) && resNodeIsFile(prnResult)) {
	/*! set to parent directory if file was found but directory was demanded */
	resNodeSetToParent(prnResult);
	cxpCtxtLogPrint(pccArg, 3, "Change to parent directory '%s'", resNodeGetNameNormalized(prnResult));
      }

      if (resNodeIsReadable(prnResult) == FALSE) {
	resNodeFree(prnResult);
	prnResult = NULL;
      }
    }
    else if ((iArgOptions & CXP_O_WRITE)) { /* find a writeable ressource */
#ifdef HAVE_CGI
      if (STR_IS_NOT_EMPTY(pucDocUrlDir)) {
	if (resPathIsDescendant(pucDocUrlDir, pucShortcut)) {
	  prnResult = resNodeDirNew(pucShortcut);
	}
	else {
	  prnResult = resNodeConcatNew(pucDocUrlDir, pucShortcut);
	}
      }
      else if ((pucRootPath = resNodeGetNameNormalized(cxpCtxtRootGet(pccArg)))) {
	if (resPathIsDescendant(pucRootPath, pucShortcut)) {
	  prnResult = resNodeDirNew(pucShortcut);
	}
	else {
	  prnResult = resNodeConcatNew(pucRootPath, pucShortcut);
	}
      }
      else {
	prnResult = resNodeDirNew(pucShortcut);
      }
#else
      if (pucLocation) { /* derive path prefix from pccArg */
	prnResult = resNodeConcatNew(pucLocation, pucShortcut);
      }
#endif
    }
    else {
#ifdef HAVE_CGI
    prnResult = resNodeDup(cxpCtxtRootGet(pccArg), RN_DUP_THIS);
#else
    if (pucLocation) { /* derive path prefix from pccArg */
      prnResult = resNodeConcatNew(pucLocation, pucShortcut);
    }
#endif
    }

    if (IS_NODE_DIR(pndArg)) {
      resNodeSetType(prnResult, rn_type_dir);
    }

    // resNodeGetMimeType(prnResult)
    resNodeReadStatus(prnResult);
    
    if (prnResult == NULL) {
      cxpCtxtLogPrint(pccArg, 1, "No valid ressource for '%s'", STR_IS_NOT_EMPTY(pucShortcut) ? pucShortcut : BAD_CAST"(anonymous)");
    }
    else if (cxpCtxtAccessIsPermitted(pccArg, prnResult) == FALSE) { /* check permission and status */
      cxpCtxtLogPrint(pccArg, 1, "No permitted ressource '%s'", resNodeGetNameNormalized(prnResult));
      resNodeFree(prnResult);
      prnResult = NULL;
    }
    else if (((iArgOptions & CXP_O_DIR) && resNodeIsDir(prnResult) == FALSE)) { /* check type */
      cxpCtxtLogPrint(pccArg, 1, "Ressource '%s' is not of type directory (%i)", resNodeGetNameNormalized(prnResult),resNodeGetType(prnResult));
      resNodeFree(prnResult);
      prnResult = NULL;
    }
    else if (((iArgOptions & CXP_O_FILE) && resNodeIsFile(prnResult) == FALSE && resNodeIsURL(prnResult) == FALSE)) { /* check type */
      cxpCtxtLogPrint(pccArg, 1, "Ressource '%s' is not of type file (%i)", resNodeGetNameNormalized(prnResult),resNodeGetType(prnResult));
      resNodeFree(prnResult);
      prnResult = NULL;
    }
    else if ((iArgOptions & CXP_O_WRITE) && resNodeIsWriteable(prnResult)) {
      cxpCtxtLogPrint(pccArg, 2, "Valid writeable ressource '%s'", resNodeGetNameNormalized(prnResult));
    }
    else if ((iArgOptions & CXP_O_READ) && resNodeIsReadable(prnResult)) {
      cxpCtxtLogPrint(pccArg, 2, "Valid readable ressource '%s'", resNodeGetNameNormalized(prnResult));
    }
    else if (iArgOptions == CXP_O_NONE) {
      cxpCtxtLogPrint(pccArg, 2, "Valid ressource '%s'", resNodeGetNameNormalized(prnResult));
    }
    else {
      cxpCtxtLogPrint(pccArg, 2, "No valid ressource '%s'", resNodeGetNameNormalized(prnResult));
      resNodeFree(prnResult);
      prnResult = NULL;
    }

    if (prnResult) {
      /* check attribute "cacheas" */
      xmlChar *pucAttrNameCacheAs;

      pucAttrNameCacheAs = domGetPropValuePtr(pndArg, BAD_CAST "cacheas");
      if (STR_IS_NOT_EMPTY(pucAttrNameCacheAs)) {
	resNodeSetNameAlias(prnResult, pucAttrNameCacheAs);
      }
    }
    xmlFree(pucDocUrlDir);
    xmlFree(pucParentPath);
  }
  return prnResult;
} /* end of cxpResNodeResolveNew() */


/*! process a PLAIN instruction

\todo handle plain/@append="yes"

 */
xmlChar *
cxpProcessPlainNode(xmlNodePtr pndArg, cxpContextPtr pccArg)
{
  xmlChar *pucResult = NULL;
  xmlChar *pucAttrName;
  //xmlChar *pucAttrNameCacheAs;

#ifdef DEBUG
  cxpCtxtLogPrint(pccArg,1,"pieProcessPlainNode(pndArg=%0x,pccArg=%0x)",pndArg,pccArg);
#endif

  if (IS_VALID_NODE(pndArg) == FALSE) {
    /* ignore NULL and invalid elements */
  }
  else if (IS_NODE_PLAIN(pndArg) || IS_NODE_FROM(pndArg)) {
    BOOL_T fCache = FALSE;
    BOOL_T fSearch;
    cxpContextPtr pccHere = pccArg;

    pucAttrName        = domGetPropValuePtr(pndArg,BAD_CAST "name");
    //pucAttrNameCacheAs = domGetPropValuePtr(pndArg,BAD_CAST "cacheas");
    fCache             = domGetPropFlag(pndArg,BAD_CAST "cache",  FALSE);
    fSearch            = domGetPropFlag(pndArg,BAD_CAST "search",FALSE);

    //pccHere = cxpCtxtFromAttr(pccArg,pndArg);

    if (pndArg->children) {
      xmlNodePtr pndChild;
      xmlNodePtr pndChildNext;

      for (pucResult = NULL, pndChild = pndArg->children; pndChild != NULL; pndChild = pndChildNext) {
	xmlChar *pucChildResult;
	
	pucChildResult = NULL;	
	pndChildNext = pndChild->next;
	
	if (xmlNodeIsText(pndChild)) {
	  /* keep this node */
	  pucChildResult = xmlNodeGetContent(pndChild);
	}
	else if (IS_NODE_PLAIN(pndChild)) {
	  pucChildResult = cxpProcessPlainNode(pndChild,pccHere);
	}
	else if (IS_NODE_XML(pndChild)) {
	  /* there is a XML instruction, only first XML!! */
	  xmlDocPtr pdocXml;

	  if ((pdocXml = cxpProcessXmlNode(pndChild, pccArg))) {
	    cxpProcessTransformations(pdocXml,pndArg,NULL,&pucChildResult,pccHere);
	    xmlFreeDoc(pdocXml); /* release DOM */
	  }
	}
#ifdef HAVE_LIBSQLITE3
	else if (IS_NODE_DB(pndChild)) {
	  pucChildResult = dbProcessDbNodeToPlain(pndChild,pccHere);
	}
#endif
#ifdef HAVE_JS
	else if (IS_NODE_SCRIPT(pndChild)) {
	  pucChildResult = scriptProcessScriptNode(pndChild,pccHere);
	}
#endif
	else if (IS_NODE_IMAGE(pndChild)) {
	  /*
	    there is a number of child elements of IMAGE
	  */
	  resNodePtr prnComment;
	  resNodePtr prnSrc;

	  prnSrc = resNodeFromNodeNew(cxpCtxtLocationGet(pccArg),domGetPropValuePtr(pndChild,BAD_CAST "src"));
	  if (prnSrc) {
	    prnComment = resNodeCommentNew(prnSrc);
	    if (prnComment != NULL && resNodeIsFile(prnComment)) {
	      if ((pucChildResult = plainGetContextTextEat(prnComment,16)) != NULL) {
		/* OK */
		NormalizeStringSpaces((char *)pucChildResult);
	      }
	      else {
		cxpCtxtLogPrint(pccArg,3,"No valid comment file '%s' found", resNodeGetNameNormalized(prnComment));
		pucChildResult = NULL;
	      }
	      resNodeFree(prnComment);
	    }
	    else {
	      cxpCtxtLogPrint(pccArg,1,"Error resNodeCommentNew()\n");
	    }
	    resNodeFree(prnSrc);
	  }
	}
	else if (IS_NODE_XSL(pndChild)) {
	  /* OK, but will be processed later */
	}
	else {
	  cxpCtxtLogPrint(pccArg,1,"No valid element '%s'",pndChild->name);
	}

	if (pucChildResult) {
	  if (pucResult) {
	    pucResult = xmlStrcat(pucResult,pucChildResult);
	    xmlFree(pucChildResult);
	  }
	  else {
	    pucResult = pucChildResult;
	  }
	}
      }
      /* all child nodes are processed, save if required */
      cxpCtxtSaveFileNode(pccArg,pndArg,NULL,pucResult);
    }
    else if (STR_IS_NOT_EMPTY(pucAttrName)) {
      /*
      element without children, read the attribute named file
       */
      resNodePtr prnFile = NULL;

      if ((pucResult = cxpCtxtCacheGetBuffer(pccArg, pucAttrName)) != NULL) {
	/* there is a cached Buffer */
	pucResult = xmlStrdup(pucResult);
      }
      else if ((prnFile = cxpResNodeResolveNew(pccArg, pndArg, NULL, CXP_O_READ)) != NULL) {
	pucResult = xmlStrdup(BAD_CAST resNodeGetContent(prnFile,1024));
	cxpCtxtCacheAppendResNodeEat(pccArg,prnFile);
      }
    }
  }
  else {
  }
  return pucResult;
} /* end of cxpProcessPlainNode() */


/*! process the XML element childs nodes to one resulting xmlDoc, ignoring text nodes

\param pndArg a parent xmlNodePtr which child elements are processed
\param pccArg the resource node

\return a new independant DOM pointer as result of all processing steps
*/
xmlDocPtr
cxpProcessXmlChildNodes(xmlNodePtr pndArg,cxpContextPtr pccArg)
{
  xmlDocPtr pdocResult = NULL;

#ifdef DEBUG
  cxpCtxtLogPrint(pccArg,3,"cxpProcessXmlChildNodes(pndArg=%0x,pccArg=%0x)",pndArg,pccArg);
#endif

  assert(IS_VALID_NODE(pndArg));
  assert(IS_NODE_XML(pndArg));

  pdocResult = xmlNewDoc(BAD_CAST "1.0");
  if (pdocResult) {
    xmlNodePtr pndNew;
    xmlNodePtr pndRootNew;
    xmlNodePtr pndChild;
    xmlNodePtr pndT;

    pndRootNew  = xmlCopyNode(pndArg,0);

    for (pndChild = pndArg->children; pndChild;) {
      cxpContextPtr pccHere = pccArg;
      xmlNodePtr pndChildNext;

      //pccHere = cxpCtxtFromAttr(pccArg,pndArg);

      pndChildNext = pndChild->next;

      pndNew = cxpProcessXmlNodeEmbedded(pndChild,pccHere);
      if (pndNew == NULL) {
	cxpCtxtLogPrint(pccArg,2,"Removing '%s' element",pndChild->name);
	xmlUnlinkNode(pndChild);
	xmlFreeNode(pndChild);
      }
      else if (pndNew != pndChild) {
	/*! if there is a new result tree, replace the old processing node 'pndChild' with tree of resulting 'pndT' */
	cxpCtxtLogPrint(pccArg,2,"Replacing tree of '%s'",pndChild->name);
	assert(pndNew->next == NULL);
	/*!\todo compare type compatibility of nodes */
	xmlAddChild(pndRootNew,pndNew);
      }
      else {
	xmlAddChild(pndRootNew,xmlCopyNode(pndChild,1));
      }

      /* WARNING: pndChild was released eventually in cxpProcessEmbeddedNodes() */
      pndChild = pndChildNext;
    }

    if (pndArg->children == pndArg->last) {
      /* one single child only, remove current top node pndRootNew */
      pndT = pndRootNew->children;
      xmlUnlinkNode(pndT);
      xmlFreeNode(pndRootNew);
      pndRootNew = pndT;
    }
    xmlDocSetRootElement(pdocResult,pndRootNew);
  }
  return pdocResult;
}
/* end of cxpProcessXmlChildNodes() */


/*! \return TRUE if pndArg is a node to store content
*/
BOOL_T
cxpIsStorageNode(xmlNodePtr pndArg)
{
  BOOL_T fResult = FALSE;

  if (xmlHasProp(pndArg, BAD_CAST "name")) {
    xmlNodePtr pndChild;

    for (pndChild=pndArg->children; fResult == FALSE && pndChild != NULL; pndChild=pndChild->next) {
      if (IS_NODE_SOURCE(pndChild)) {
	fResult = TRUE;
      }
    }
  }
  return fResult;
} /* end of cxpIsStorageNode() */


/*! \return TRUE if pndArg is a node in cxp namespace
*/
BOOL_T
cxpIsNs(xmlNodePtr pndArg)
{
  BOOL_T fResult = FALSE;

  /*!\todo implement */

  assert(FALSE);

#if 0
  if (pndArg != NULL && pndArg->ns != NULL && pndArg->ns) {

    if (pnsCxp == NULL) {
      pnsCxp = xmlNewNs(NULL, BAD_CAST CXP_PIE_URL, BAD_CAST"cxp");
    }

	fResult = TRUE;
  }
#endif

  return fResult;
} /* end of cxpIsNs() */


/*! \return TRUE if pndArg is a node in cxp namespace
*/
xmlNsPtr
cxpGetNs(void)
{
  if (pnsCxp == NULL) {
    pnsCxp = xmlNewNs(NULL, BAD_CAST CXP_NAMESPACE_URL, BAD_CAST"cxp");
  }
  return pnsCxp;
} /* end of cxpGetNs() */


/*! \return a node pointer to XML source child node of pndArg
*/
xmlNodePtr
cxpGetXmlSourceNode(xmlNodePtr pndArg, cxpContextPtr pccArg)
{
  xmlNodePtr pndChild;
  xmlNodePtr pndResult = NULL;

  for (pndChild=pndArg->children; pndChild != NULL; pndChild=pndChild->next) {

    if (IS_NODE_XML(pndChild)) {
      if (pndResult) {
	cxpCtxtLogPrint(pccArg, 1, "Ignoring redundant XML source: '%s/%s[@name='%s']'",
	  pndArg->name, pndChild->name, domGetPropValuePtr(pndChild, BAD_CAST"name"));
	break;
      }
      pndResult = pndChild;
    }
    else if (IS_NODE_XSL(pndChild) || IS_NODE(pndChild, BAD_CAST"stylesheet")) {
      if (pndResult) {
	cxpCtxtLogPrint(pccArg, 1, "Accepting: '%s/%s[@name='%s']'",
	  pndArg->name, pndChild->name, domGetPropValuePtr(pndChild, BAD_CAST"name"));
      }
      else {
	pndResult = pndChild;
      }
    }
    else if (IS_NODE_DIR(pndChild) || IS_NODE_FILE(pndChild) || IS_NODE_ZIP(pndChild)) {
      if (pndResult) {
	cxpCtxtLogPrint(pccArg,1,"Ignoring redundant XML source: '%s/%s[@name='%s']'",
	    pndArg->name,pndChild->name,domGetPropValuePtr(pndChild,BAD_CAST"name"));
	break;
      }
      pndResult = pndChild;
    }
    else if (IS_NODE_JSON(pndChild) || IS_NODE_PIE(pndChild) || IS_NODE_CALENDAR(pndChild)) {
      if (pndResult) {
	cxpCtxtLogPrint(pccArg,1,"Ignoring redundant XML source: '%s/%s[@name='%s']'",
	    pndArg->name,pndChild->name,domGetPropValuePtr(pndChild,BAD_CAST"name"));
	break;
      }
      pndResult = pndChild;
    }
    else if (IS_NODE_DB(pndChild)) {
      pndResult = pndChild;
    }
    else if (IS_NODE_PATHNET(pndChild) || IS_NODE_PATHTABLE(pndChild)) {
      if (pndResult) {
	cxpCtxtLogPrint(pccArg,1,"Ignoring redundant XML source: '%s/%s[@name='%s']'",
	    pndArg->name,pndChild->name,domGetPropValuePtr(pndChild,BAD_CAST"name"));
	break;
      }
      pndResult = pndChild;
    }
    else if (IS_NODE_INFO(pndChild)) {
      if (pndResult) {
	cxpCtxtLogPrint(pccArg,1,"Ignoring redundant XML source: '%s/%s[@name='%s']'",
	    pndArg->name,pndChild->name,domGetPropValuePtr(pndChild,BAD_CAST"name"));
	break;
      }
      pndResult = pndChild;
    }
    else if (IS_ENODE(pndChild)) { /* every other element node */
      if (pndResult) {
	cxpCtxtLogPrint(pccArg,1,"Ignoring redundant XML source: '%s/%s[@name='%s']'",
	    pndArg->name,pndChild->name,domGetPropValuePtr(pndChild,BAD_CAST"name"));
	break;
      }
      pndResult = pndChild;
    }
  }

  return pndResult;
}
/* end of cxpGetXmlSourceNode() */


/*! process the XML element and its childs

\param pndArg a xmlNodePtr to append data
\param pccArg the resource node

\return a new independant DOM pointer as result of transformations
*/
xmlDocPtr
cxpProcessXmlNode(xmlNodePtr pndArg, cxpContextPtr pccArg)
{
  xmlDocPtr pdocResult = NULL;

#ifdef DEBUG
  cxpCtxtLogPrint(pccArg,3,"cxpProcessXmlNode(pndArg=%0x,pccArg=%0x)",pndArg,pccArg);
#endif

  if (IS_VALID_NODE(pndArg) == FALSE) {
    /* ignore NULL and invalid elements */
  }
  else if (IS_NODE_XML(pndArg)) {
    xmlChar *pucSchema;
    xmlChar *pucT = NULL;
    xmlChar *pucAttrName;
    xmlDocPtr pdocT = NULL; /* preliminary result DOM */
    xmlNodePtr pndChildSource;

    pucAttrName = domGetPropValuePtr(pndArg, BAD_CAST "name");

    if ((pndChildSource = cxpGetXmlSourceNode(pndArg, pccArg))) {
      /*
      this is a XML instruction
      */
      cxpContextPtr pccHere = pccArg;

      //pccHere = cxpCtxtFromAttr(pccArg, pndArg);

      if (IS_NODE_XML(pndChildSource) || IS_NODE_XHTML(pndChildSource)) {
	pdocT = cxpProcessXmlNode(pndChildSource,pccHere);
      }
      else if (IS_NODE_DIR(pndChildSource) || IS_NODE_FILE(pndChildSource)) {
	pdocT = dirProcessDirNode(pndChildSource,NULL,pccHere);
      }
      else if (IS_NODE_PIE(pndChildSource)) {
	pdocT = pieProcessPieNode(pndChildSource,pccHere);
      }
#ifdef HAVE_PIE
      else if (IS_NODE_CALENDAR(pndChildSource)) {
	pdocT = calProcessCalendarNode(pndChildSource,pccHere);
      }
#endif
#ifdef HAVE_PETRINET
      else if (IS_NODE_PATHNET(pndChildSource) || IS_NODE_PATHTABLE(pndChildSource)) {
	pdocT = pnetProcessNode(pndChildSource,pccHere);
      }
#endif
#ifdef HAVE_JSON
      else if (IS_NODE_JSON(pndChildSource)) {
	pdocT = jsonProcessJsonNode(pndChildSource, pccHere);
      }
#endif
      else if (IS_NODE_INFO(pndChildSource)) {
	pdocT = cxpProcessInfoNode(pndChildSource,pccHere);
      }
#ifdef HAVE_LIBSQLITE3
      else if (IS_NODE_DB(pndChildSource)) {
	pdocT = dbProcessDbNodeToDoc(pndChildSource,pccHere);
      }
#endif
#ifdef HAVE_LIBARCHIVE
      else if (IS_NODE_ZIP(pndChildSource)) {
	pdocT = arcProcessZipNode(pndChildSource,pccHere);
      }
#endif
      else if (IS_NODE_PLAIN(pndChildSource)) {
	int iLen;

	pucT = cxpProcessPlainNode(pndChildSource,pccHere);
	if (pucT!=NULL && (iLen = xmlStrlen(pucT)) > 10) {
	  /*!\todo apply text substitutions */
	  pdocT = xmlParseMemory((const char *)pucT,iLen);
	}
	xmlFree(pucT);
      }
#ifdef HAVE_JS
      else if (IS_NODE_SCRIPT(pndChildSource)) {
	int iLen;

	pucT = scriptProcessScriptNode(pndChildSource,pccHere);
	if (pucT!=NULL && (iLen = xmlStrlen(pucT)) > 10) {
	  pdocT = xmlParseMemory((const char *)pucT,iLen);
	}
	xmlFree(pucT);
      }
#endif
      else {
	/*
	 * this is no single instruction element, copy first node tree into a new DOM
	 */
	pdocT = cxpProcessXmlChildNodes(pndArg,pccHere);
      }

      /*!\todo cxpSubstitute(pndArg); */

      if (pdocT) {
	/*! transform DOM by XSL if required */
	if (cxpProcessTransformations(pdocT, pndArg, &pdocResult, NULL, pccHere) && pdocResult != NULL) {

	  /*! save DOM if required */
	  cxpCtxtSaveFileNode(pccArg, pndArg, pdocResult, NULL);

	  /*! evaluate DOM if required */
	  if (IS_NODE_XML(pndArg) && domGetPropFlag(pndArg, BAD_CAST "eval", FALSE)) {
	    xmlNodePtr pndRoot;

	    pndRoot = xmlDocGetRootElement(pdocResult);
	    if (IS_NODE_MAKE(pndRoot)) {
	      /* process only but no result DOM expected */
	      cxpCtxtLogPrint(pccArg, 2, "MAKE evaluation");
	      cxpProcessMakeNode(pndRoot, pccHere);
	      xmlFreeDoc(pdocResult);
	      pdocResult = NULL;
	    }
	    else if (IS_NODE_XML(pndRoot)) {
	      /* process and a result DOM expected */
	      cxpCtxtLogPrint(pccArg, 2, "'%s' evaluation", pndRoot->name);
	      pdocResult = cxpProcessXmlNode(pndRoot, pccHere);
	    }
	  }
	}

	if (pdocResult != pdocT) {
	  xmlFreeDoc(pdocT);
	}
      }
    }
    else if (STR_IS_NOT_EMPTY(pucAttrName)) {
      /*
      this is a simple input XML element (no child's, but name attribute)
       */

      /*!\todo validate="yes" for XML validation on demand */

      if ((pdocResult = cxpCtxtCacheGetDoc(pccArg, pucAttrName)) != NULL) {
	/* there is a cached DOM */
	pdocResult = xmlCopyDoc(pdocResult,1);
      }
      else {
	resNodePtr prnFile; /* resource node according to pucAttrName */

	prnFile = cxpResNodeResolveNew(pccArg, pndArg, NULL, CXP_O_READ);
	if (prnFile) {
	  cxpCtxtLogPrint(pccArg,2,"Reading XML '%s'", resNodeGetNameNormalized(prnFile));
	  
	  if ((pdocResult = resNodeReadDoc(prnFile)) == NULL) {
	  }
	  else if (resPathIsDescendant(resNodeGetNameNormalized(cxpCtxtSearchGet(pccArg)), resNodeGetNameNormalized(prnFile))) {
	    /* remove locator of this dom, because it comes from a search path */
	    if (STR_IS_NOT_EMPTY(pdocResult->URL)) {
	      xmlFree((void *)pdocResult->URL);
	      pdocResult->URL = NULL;
	    }
	  }
	  else if (domGetPropFlag(pndArg, BAD_CAST "locator", FALSE)) {
	    /* add additional attributes for navigation */
	    xmlNodePtr pndRoot;

	    pndRoot = xmlDocGetRootElement(pdocResult);
	    domSetPropFileLocator(pndRoot,resNodeGetNameRelative(cxpCtxtRootGet(pccArg),prnFile));
	    domSetPropFileXpath(pndRoot,BAD_CAST"fxpath",NULL);
	  }
	  cxpCtxtCacheAppendResNodeEat(pccArg,prnFile);
	}
	else {
	  /*!\bug 'https://', use sscanf_next_url() ? */
	  cxpCtxtLogPrint(pccArg,1,"XML source not readable '%s'", pucAttrName);
	}
      }

      if (domGetPropFlag(pndArg,BAD_CAST "dump",FALSE)) {
	cxpCtxtLogPrintDoc(pccArg, 1, NULL, pdocResult);
      }
    }
    else {
      cxpCtxtLogPrint(pccArg,1,"Ignoring empty element '%s'",pndArg->name);
    }

    if (pdocResult) {
      xmlNodePtr pndRoot;

      pucSchema = domGetPropValuePtr(pndArg, BAD_CAST "schema");
      if (pucSchema != NULL && xmlStrlen(pucSchema) > 4) {
	if (ValidateSchema(pdocResult, pucSchema, pccArg) == FALSE) {
	  /*!\todo append validation error messages to existing meta element? */
	}
      }
      else {
	cxpCtxtLogPrint(pccArg,4, "No Schema Validation requested");
      }

      if (domGetPropFlag(pndArg,BAD_CAST "xpath",FALSE)) {
	pndRoot = xmlDocGetRootElement(pdocResult);
	if (pndRoot) {
	  cxpCtxtLogPrint(pccArg,2,"Add XPath attribute to every element");
	  /*!\todo redundant to "domSetPropFileXpath(pndRoot,BAD_CAST"fxpath",NULL);" */
	  domSetPropFileXpath(pndRoot,BAD_CAST"xpath",NULL);
	}
      }
    }
  }
  else {
    cxpCtxtLogPrint(pccArg,1,"Ignoring non XML element '%s'",pndArg->name);
  }
//  xmlAddChild(xmlDocGetRootElement(pdocResult),cxpGetLog());

  return pdocResult;
}
/* end of cxpProcessXml() */


/*! \return a pointer to a changed context directory

\todo handle platform differences win32|linux
\todo remove handling of attributes  "chdir", "mkdir" and "rmdir"
 */
BOOL_T
cxpProcessSystemNode(xmlNodePtr pndArg, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;

  if (IS_VALID_NODE(pndArg) == FALSE) {
    /* ignore NULL and invalid elements */
  }
  else if (IS_NODE_SYSTEM(pndArg)) {
    xmlChar *pucAttrT;
    cxpContextPtr pccHere = pccArg;

    //pccHere = cxpCtxtFromAttr(pccArg,pndArg);

    if ((pucAttrT = domGetPropValuePtr(pndArg,BAD_CAST "message"))) {
      /*!\todo convert pucAttrT to isolat1? */
      xmlChar *pucT;
      xmlChar *pucMessage;

      pucMessage = xmlStrdup(pucAttrT);
      for (pucT=pucMessage; ! isend(*pucT); pucT++) {
	if (*pucT == (xmlChar)'%') {
	  *pucT = (xmlChar)' '; /* set '%' to space character because use as format string */
	}
      }

      if (domGetPropFlag(pndArg,BAD_CAST "timer",FALSE)) {
	/* append timer string to message */
	xmlChar mpucT[BUFFER_LENGTH];

	xmlStrPrintf(mpucT, BUFFER_LENGTH, " (%is)", cxpCtxtGetRunningTime(pccArg));
	pucMessage = xmlStrcat(pucMessage,mpucT);
      }

      cxpCtxtLogPrint(pccArg,1,(char *)pucMessage);
#ifdef HAVE_CGI
      // no pause at all
#else
      if (domGetPropFlag(pndArg, BAD_CAST "pause", FALSE)) {
	/* pause with prompt and dummy input when not in CGI mode */
	char mcT[10];
	(void)fgets(mcT,sizeof(mcT)-1,stdin);
      }
#endif
      xmlFree(pucMessage);
      fResult = TRUE;
    }
#ifdef HAVE_CGI
      // no dialog, exec or call at all
#else
    else if ((pucAttrT = domGetPropValuePtr(pndArg,BAD_CAST "dialog"))) {
      cxpCtxtLogPrint(pccArg,3,"Using dialog value '%s'",pucAttrT);
      fResult = cxpCtxtCliMessageBox(pucAttrT,pccHere);
    }
    else if ((pucAttrT = domGetPropValuePtr(pndArg, BAD_CAST "exec"))) { /* this is a system shell exec */
      xmlChar* pucAttrCommand;

      if ((pucAttrCommand = xmlStrdup(pucAttrT))) { /*!\bug convert pucAttrT to isolat1 */
	int iError;

	/*!\todo check commands tbefore execution */
	/*!\todo set environment with all env child's */
	/*!\bug Windows: "You must explicitly flush (using fflush or _flushall) or close any stream before calling system. " */
	iError = system((const char*)pucAttrCommand);
	if (iError == 0) {
	  fResult = TRUE;
	  /*!\todo more error handling */
	}

	if (pucAttrCommand != pucAttrT) {
	  xmlFree(pucAttrCommand);
	}
      }
      else {
	PrintFormatLog(1, "Command encoding error '%s'", pucAttrT);
      }
    }
    else if ((pucAttrT = domGetPropValuePtr(pndArg,BAD_CAST "call"))) { /* this is a system shell call */
      resNodePtr prnFile;

      prnFile = cxpResNodeResolveNew(pccHere, NULL, pucAttrT, (CXP_O_FILE | CXP_O_READ));
      // prnFile = resNodeConcatNew(cxpCtxtLocationGetStr(pccHere), pucAttrT);
      if (resNodeIsFile(prnFile) && resNodeIsExecuteable(prnFile)) {
	int iError;

	/*!\todo set environment with all env childs */
	/*!\bug Windows: "You must explicitly flush (using fflush or _flushall) or close any stream before calling system. " */
	iError = system((const char*)resNodeGetNameNormalizedNative(prnFile));
	if (iError == 0) {
	  fResult = TRUE;
	  /*!\todo more error handling */
	}
      }
      else {
	PrintFormatLog(1,"Only existing files are callable.");
      }
      resNodeFree(prnFile);
    }
#endif
    else if ((pucAttrT = domGetPropValuePtr(pndArg,BAD_CAST "chdir"))) { /* this is a system chdir call */
      resNodeReset(cxpCtxtLocationGet(pccHere),pucAttrT); /*! avoid global chdir */
      fResult = cxpViewNodeResult(pndArg, pccHere);
    }
    else if ((pucAttrT = domGetPropValuePtr(pndArg,BAD_CAST "mkdir"))) {
      /* this is a system mkdir call */
      resNodePtr prnMkdir;

      if ((prnMkdir = resNodeConcatNew(cxpCtxtLocationGetStr(pccHere),pucAttrT))) {
        /*!\todo use umask for MODE_DIR_CREATE */
        fResult = resNodeMakeDirectoryStr(resNodeGetNameNormalized(prnMkdir),MODE_DIR_CREATE);
        resNodeFree(prnMkdir);
        cxpViewNodeResult(pndArg, pccHere);
      }
    }
    else if ((pucAttrT = domGetPropValuePtr(pndArg,BAD_CAST "rmdir"))) {
      /* this is a system mkdir call */
      resNodePtr prnRmdir;

      prnRmdir = resNodeConcatNew(cxpCtxtLocationGetStr(pccHere),pucAttrT);
      /*!\todo test if cxpCtxtLocationGet(pccHere) != prnRmdir */
      fResult = resNodeUnlink(prnRmdir,domGetPropFlag(pndArg,BAD_CAST "recursive",FALSE));
      resNodeFree(prnRmdir);
    }
    else {
      cxpCtxtLogPrint(pccArg,1,"Unknown system attribute");
    }
  }
  return fResult;
} /* end of cxpProcessSystemNode() */


/*! process the child's of top element MAKE

\param pndArg a xmlNodePtr to append data
\param pccArg the resource node

\todo Validate against DTD if readable locally
\todo configurable Processing-Instructions?
*/
void
cxpProcessMakeNode(xmlNodePtr pndArg,cxpContextPtr pccArg)
{
#ifdef DEBUG
  cxpCtxtLogPrint(pccArg,2,"cxpProcessMakeNode(pndArg=%0x,pccArg=%0x)",pndArg,pccArg);
#endif

  if (IS_VALID_NODE(pndArg) == FALSE) {
    /* ignore NULL and invalid elements */
  }
  else {
    BOOL_T fValidation = FALSE;
    xmlNodePtr pndChild;
    cxpContextPtr pccHere = pccArg;

    fValidation = domGetPropFlag(pndArg, BAD_CAST "validation", TRUE);
    if (fValidation == FALSE) {
      cxpCtxtLogPrint(pccArg, 3, "Skipping XML pre-substitution validation");
    }
    else if (ValidateCxpTree(pndArg, pccArg)) {
      cxpCtxtLogPrint(pccArg, 3, "pre-substitution validation of '%s' successful", pndArg->name);
    }
    else {
      cxpCtxtLogPrint(pccArg, 1, "pre-substitution validation failed");
      return;
    }

    pccHere = cxpCtxtFromAttr(pccArg, pndArg);

    if (IS_NODE_MAKE(pndArg)) {
      /*
	MAKE level, there is no result DOM
	*/
#if defined(HAVE_LIBPTHREAD) || defined(WITH_THREAD)
      xmlChar *pucAttrThread = NULL;
      BOOL_T fThread = FALSE;
      cxpThreadPoolPtr pcxpThreadPool = NULL;

      /*!\todo set the max number of threads via attribute threads="11" */

      if ((fThread = domGetPropFlag(pndArg,BAD_CAST "thread",FALSE))) {
	cxpCtxtLogPrint(pccArg,2,"Threads enabled");
      }
      else {
	cxpCtxtLogPrint(pccArg,4,"Threads disabled");
      }
#else
      cxpCtxtLogPrint(pccArg, 2, "Compiled without threads");
#endif

      cxpSubstIncludeNodes(pndArg, pccHere);
      /*!\todo substitute format strings in text nodes */
      cxpSubstInChildNodes(pndArg, NULL, pccHere);
      //cxpSubstReplaceNodes(pndArg, pccHere);

      /* post include and subst validation */
      if (fValidation == FALSE) {
	cxpCtxtLogPrint(pccArg, 4, "Skipping XML post-substitution validation");
      }
      else if (ValidateCxpTree(pndArg, pccArg)) {
	cxpCtxtLogPrint(pccArg, 3, "post-substitution validation of '%s' successful", pndArg->name);
      }
      else {
	cxpCtxtLogPrint(pccArg, 1, "post-substitution validation failed after includes and substs");
	return;
      }

      if (domGetPropFlag(pndArg, BAD_CAST "dump", FALSE)) {
	cxpCtxtLogPrintDoc(pccArg, 1, NULL, pndArg->doc);
      }

#if defined(HAVE_LIBPTHREAD) || defined(WITH_THREAD)
      if (fThread) {
	pcxpThreadPool = cxpThreadPoolNew();
      }
#endif

      for (pndChild=pndArg->children; pndChild != NULL; pndChild=pndChild->next) {

	if (IS_VALID_NODE(pndChild) == FALSE) {
	  continue; /* ignore all non-valid element nodes */
	}
	else if (IS_NODE_FILECOPY(pndChild)) {
	  cxpProcessCopyNode(pndChild, pccHere);
	}
	else if (IS_NODE_EACH(pndChild)) {
	  xmlDocPtr pdocMake;
#if defined(HAVE_LIBPTHREAD) || defined(WITH_THREAD)
	  if (pucAttrThread) {
	    xmlSetProp(pndChild,BAD_CAST "thread",pucAttrThread);
	  }
#endif
	  pdocMake = cxpProcessEachNode(pndChild, pccHere);
	  if (pdocMake) {
	    cxpProcessMakeNode(xmlDocGetRootElement(pdocMake), pccHere);
	    xmlFreeDoc(pdocMake);
	  }
	}
	else if (IS_NODE_MAKE(pndChild)) {
#if defined(HAVE_LIBPTHREAD) || defined(WITH_THREAD)
	  if (pcxpThreadPool) {
	    cxpContextPtr pccThread;

	    pccThread = cxpCtxtDup(pccHere);
	    cxpCtxtAddChild(pccHere, pccThread);
	    cxpCtxtProcessSetNodeCopy(pccThread,pndChild);
	    cxpThreadPoolAppend(pcxpThreadPool,pccThread);
	  }
	  else {
	    /* simple recursive call if no threads are available */
	    cxpProcessMakeNode(pndChild,pccHere);
	  }
#else
	  cxpProcessMakeNode(pndChild, pccHere);
#endif
	}
	else if (IS_NODE_XML(pndChild)) {
	  /*! we must handle the result of cxpProcessXml(), run evaluation and free */
	  xmlDocPtr pdocResultT;

	  pdocResultT = cxpProcessXmlNode(pndChild, pccHere);
	  if (pdocResultT) {
	    if (domGetPropFlag(pndChild, BAD_CAST "eval", FALSE)) {
	      /*! */
	      xmlNodePtr pndEval;

	      cxpCtxtLogPrint(pccArg, 2, "Self evaluating");

	      pndEval = xmlDocGetRootElement(pdocResultT);
	      if (pndEval) {
		cxpContextPtr pccEval;

		pccEval = cxpCtxtFromAttr(pccHere, pndEval); /* use context of Doc URL */
		cxpProcessMakeNode(pndEval, pccEval);
	      }
	    }
	    xmlFreeDoc(pdocResultT);
	    cxpViewNodeResult(pndChild, pccHere);
	  }
	}
#ifdef HAVE_LIBARCHIVE
	else if (IS_NODE_ZIP(pndChild)) {
	  arcProcessZipNode(pndChild, pccHere);
	}
#endif
#ifdef HAVE_LIBEXIF
#endif
#ifdef HAVE_LIBMAGICK
	else if (IS_NODE_IMAGE(pndChild)) {
	  imgMain(pndChild,pccHere);
	}
#endif
#ifdef HAVE_LIBSQLITE3
	else if (IS_NODE_DB(pndChild)) {
	  dbProcessDbNode(pndChild,pccHere);
	}
#endif
	else if (IS_NODE_PLAIN(pndChild)) {
	  xmlChar *pucT;

	  pucT = cxpProcessPlainNode(pndChild, pccHere);
	  if (pucT) {
	    /*!\todo apply text substitutions */
	    xmlFree(pucT);
	    cxpViewNodeResult(pndChild, pccHere);
	  }
	}
	else if (IS_NODE_SYSTEM(pndChild)) {
	  if (cxpProcessSystemNode(pndChild, pccHere) == FALSE) { /* change of local resource node possible */
	    break; /* in case of canceling or errors */
	  }
	}
      }

#if defined(HAVE_LIBPTHREAD) || defined(WITH_THREAD)
      if (pcxpThreadPool) {
	cxpThreadPoolJoin(pcxpThreadPool); /* join all created threads of this MAKE element */
      }
#endif
    }
    else if (IS_NODE_XML(pndArg)) {
      xmlDocPtr pdocResultT;

      pdocResultT = cxpProcessXmlNode(pndArg, pccHere);
      if (pdocResultT) {
	/* we are not interested in any result at MAKE level */
	xmlFreeDoc(pdocResultT);
      }
    }
#ifdef HAVE_LIBMAGICK
    else if (IS_NODE_IMAGE(pndArg)) {
      imgMain(pndArg,pccHere);
    }
#endif
    else if (IS_NODE_PLAIN(pndArg)) {
      xmlChar *pucT;

      pucT = cxpProcessPlainNode(pndArg, pccHere);
      if (pucT) {
	/*!\todo apply text substitutions */
	xmlFree(pucT);
      }
    }
    else {
      cxpCtxtLogPrint(pccArg, 1, "Unusable Element '%s'", pndArg->name);
    }
  }
}
/* end of cxpProcessMakeNode() */


/*! apply all sibling PARAM elements, non recursive!!

\param pndArg a xmlNodePtr to append data
\param pccArg the resource node

\return
*/
xmlChar **
cxpXslParamProcess(xmlNodePtr pndArg, cxpContextPtr pccArg)
{
  cxpContextPtr pccHere = pccArg;
  xmlChar **ppucResult = NULL;
  size_t ciNodeVariable;

  if (pndArg == NULL) {
    return ppucResult;
  }

  //pccHere = cxpCtxtFromAttr(pccArg,pndArg);

  if (domGetPropFlag(pndArg, BAD_CAST "appendcgi", FALSE)) {
#ifdef HAVE_CGI
    /* CGI environment */
    index_t i;

    cxpCtxtLogPrint(pccArg, 2, "Appending %i CGI values", cxpCtxtCgiGetCount(pccArg));

    for (i = 0; i < cxpCtxtCgiGetCount(pccArg); i++) {
      xmlNodePtr pndVariable;
      pndVariable = xmlNewChild(pndArg, NULL, BAD_CAST"variable", NULL);
      domSetPropEat(pndVariable, BAD_CAST "name", cxpCtxtCgiGetName(pccArg, i));
      domSetPropEat(pndVariable, BAD_CAST "select", cxpCtxtCgiGetValue(pccArg, i));
    }
#else
  cxpCtxtLogPrint(pccArg, 2, "Can append CGI values in CGI mode only");
#endif
  }

  ciNodeVariable = domNumberOf(pndArg->children, BAD_CAST "variable", 0);
  if (ciNodeVariable > 0) {
    xmlNodePtr pndChildParam;
    xmlNodePtr pndPlain;
    xmlNodePtr pndXml;
    xmlChar *pucAttrSelect;
    index_t i;

    ppucResult = (xmlChar **)xmlMemMalloc((ciNodeVariable * 2 + 1) * sizeof(xmlChar *));
    ppucResult[0] = NULL;		/* end markup */
    for (i=0,pndChildParam=pndArg->children; ppucResult!=NULL && pndChildParam!=NULL; pndChildParam=pndChildParam->next) {
      /* all PARAM siblings */
      if (IS_NODE_VARIABLE(pndChildParam)) {
	/* this is an PARAM */
	xmlChar *pucName = domGetPropValuePtr(pndChildParam,BAD_CAST "name");
	xmlChar *pucSelect = NULL;
	xmlChar *pucSelectTest = NULL;
	cxpSubstPtr pcxpSubstT;

	if (STR_IS_NOT_EMPTY(pucName)) {
	  if ((pndPlain = domGetFirstChild(pndChildParam,NAME_PLAIN))) {
	    /* there are child nodes (cxp:plain only!!), read as value */
	    cxpCtxtLogPrint(pccArg,3,"VARIABLE '%s' from plain",pucName);
	    pucSelect = cxpProcessPlainNode(pndPlain,pccHere);
	  }
	  else if ((pndXml = domGetFirstChild(pndChildParam,NAME_XML))) {
	    /* there are child nodes, read as value */
	    xmlDocPtr pdocT;
	    cxpCtxtLogPrint(pccArg,3,"VARIABLE '%s' from XML",pucName);
	    pdocT = cxpProcessXmlNode(pndXml,pccHere);
	    if (pdocT) {
	      int iLength = 0;
	      cxpCtxtLogPrint(pccArg,2,"Replacing");
	      xmlDocDumpMemory(pdocT,&pucSelect,&iLength);
	      xmlFreeDoc(pdocT);
	    }
	  }
	  else if ((pucAttrSelect = domGetPropValuePtr(pndChildParam,BAD_CAST "select"))) {
	    /* use attribute value */
	    cxpCtxtLogPrint(pccArg,3,"VARIABLE '%s' from attribute 'select'",pucName);
	    pucSelect = xmlStrdup(pucAttrSelect);
	  }
	  else if ((pucSelectTest = domNodeGetContentPtr(pndChildParam))) {
	    /* use text node */
	    cxpCtxtLogPrint(pccArg,3,"VARIABLE '%s' from content",pucName);
	    pucSelect = xmlStrdup(pucSelectTest);
	  }
	  else if ((pcxpSubstT = cxpSubstDetect(pndChildParam,pccArg))) {
	    if ((pucSelectTest = cxpSubstGetPtr(pcxpSubstT))) {
	      /* use attribute value */
	      cxpCtxtLogPrint(pccArg,3,"VARIABLE value '%s' from attribute",pucSelectTest);
	      pucSelect = xmlStrdup(pucSelectTest);
	    }
	    cxpSubstFree(pcxpSubstT);
	  }

	  if (pucSelect) {
	    ppucResult[i] = xmlStrdup(pucName);
	    i++;
	    ppucResult[i] = pucSelect;
	    i++;
	  }
	  else {
	    cxpCtxtLogPrint(pccArg,1,"Ignoring empty VARIABLE '%s'",pucName);
	  }
	}
      }
      ppucResult[i] = NULL;		/* end markup */
    }
  }
  return ppucResult;
}
/* end of cxpXslParamProcess() */


/*! release all entries in param array
*/
void
cxpXslParamFree(char **ppchArg)
{
  if (ppchArg) {
    int i;
    for (i=0; ppchArg[i]; i++) {
      xmlFree(ppchArg[i]);
    }
    xmlMemFree(ppchArg);
  }
  return;
}
/* end of cxpXslParamFree() */


/*! cxp Ctxt Search Set

\param pccArg -- pointer to context
\param prnArg -- new resNode to set as location
\return TRUE if , FALSE in case of
*/
resNodePtr
cxpCtxtSearchGet(cxpContextPtr pccArg)
{
  resNodePtr prnResult = NULL;

  if (pccArg) {
    cxpContextPtr pccParent;

    if (pccArg->prnSearch) {
      prnResult = pccArg->prnSearch;
    }
    else if ((pccParent = cxpCtxtGetParent(pccArg))) {
      prnResult = cxpCtxtSearchGet(pccParent);
    }
    else {
    }
  }
  return prnResult;
} /* end of cxpCtxtSearchGet() */


/*! cxp Ctxt Search Set

search path is not a single context, because of symbolic path '...//'
and multiple directories (like env PATH). Reading of files and
directories is permitted here!

\param pccArg -- pointer to context
\param prnArg -- new resNode to set as location
\return TRUE if , FALSE in case of

\todo cleanup code
*/
BOOL_T
cxpCtxtSearchSet(cxpContextPtr pccArg, resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (pccArg) {
    xmlChar *pucPathValue = NULL;

#ifdef DEBUG
    cxpCtxtLogPrint(pccArg, 2, "cxpCtxtSearchSet(cxpContextPtr pccArg, resNodePtr prnArg)");
#endif

    pucPathValue = cxpCtxtEnvGetValueByName(pccArg, BAD_CAST "CXP_PATH");
    if (STR_IS_NOT_EMPTY(pucPathValue)) {
      cxpCtxtLogPrint(pccArg, 2, "Use value '%s' of environment variable 'CXP_PATH'", pucPathValue);
    }
#ifdef HAVE_CGI
    /* the only way in CGI mode!! */
#else
    else {
      /*
      some additional tricks to set the default search path
      */
      xmlChar *pucRelease;
      xmlChar *pucExecutablePath;

      pucRelease = cxpCtxtCliGetValue(pccArg, 0);
      pucExecutablePath = resPathNormalize(pucRelease);
      xmlFree(pucRelease);

#ifdef _MSC_VER
      /* find "cxproc" in argv[0] */
      pucPathValue = resPathGetDirFind(pucExecutablePath, BAD_CAST"bin");
      if (pucPathValue) {
	cxpCtxtLogPrint(pccArg, 2, "Use executable directory '%s' in '%s'", pucPathValue, pucExecutablePath);
	pucRelease = resPathGetBasedir(pucPathValue);
	xmlFree(pucPathValue);
	pucPathValue = xmlStrncatNew(pucRelease, BAD_CAST"//", -1);
	xmlFree(pucRelease);
      }
      else {
	pucPathValue = resPathGetDirFind(pucExecutablePath, BAD_CAST"cxproc");
	if (pucPathValue) {
	  cxpCtxtLogPrint(pccArg, 2, "Use executable directory '%s' in '%s'", pucPathValue, pucExecutablePath);
	  pucRelease = pucPathValue;
	  pucPathValue = xmlStrncatNew(pucRelease, BAD_CAST"//", -1);
	  xmlFree(pucRelease);
	}
	else {
	}
      }
#endif
      xmlFree(pucExecutablePath);
    }
#endif
    /*!\todo improve speed by using index files 'NAME_FILE_INDEX' */

    /*! release old search list first */
    resNodeListFree(pccArg->prnSearch);

    /*! check search path context */
    if (STR_IS_NOT_EMPTY(pucPathValue)) {
      pccArg->prnSearch = resNodeStrNew(pucPathValue);
      resNodeSetRecursion(pccArg->prnSearch,resPathIsDirRecursive(pucPathValue));
      xmlFree(pucPathValue);
    }
    else if (prnArg) {
      pccArg->prnSearch = resNodeDup(prnArg, (RN_DUP_CHILDS | RN_DUP_NEXT));
    }
    else {
      pccArg->prnSearch = NULL;
    }
    if (resPathIsDescendant(resNodeGetNameNormalized(cxpCtxtRootGet(pccArg)), resNodeGetNameNormalized(cxpCtxtSearchGet(pccArg)))) {
      cxpCtxtLogPrint(pccArg, 1, "WARNING: Value '%s' is a descendant directory of search path '%s'",
	resNodeGetNameNormalized(cxpCtxtRootGet(pccArg)), resNodeGetNameNormalized(cxpCtxtSearchGet(pccArg)));
    }
    else if (resPathIsDescendant(resNodeGetNameNormalized(cxpCtxtSearchGet(pccArg)), resNodeGetNameNormalized(cxpCtxtRootGet(pccArg)))) {
      cxpCtxtLogPrint(pccArg, 1, "WARNING: Value '%s' is a descendant directory of root path '%s'",
	resNodeGetNameNormalized(cxpCtxtSearchGet(pccArg)), resNodeGetNameNormalized(cxpCtxtRootGet(pccArg)));
    }

    fResult = TRUE;
  }

  return fResult;
} /* end of cxpCtxtSearchSet() */


/*! global XSL variables only (between RootElement and first xsl:template element)
*/
BOOL_T
cxpUpdateXslVariable(xmlNodePtr pndArg, char *pcValue, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;

  if (pndArg != NULL && STR_IS_NOT_EMPTY(pcValue)) {
    xmlChar *pucAttrSelect;

    pucAttrSelect = domGetPropValuePtr(pndArg, BAD_CAST "select");
    if ((STR_IS_NOT_EMPTY(pucAttrSelect) && pucAttrSelect[0]==(xmlChar)'\'') || pcValue[0]=='\'') {
      /* probably it's a string variable */
      xmlChar *pucValueNew = NULL;

      if (STR_IS_NOT_EMPTY(BAD_CAST pcValue) && (pucValueNew = xmlStrdup(BAD_CAST pcValue)) != NULL && StringRemovePairQuotes(pucValueNew)) {
	xmlUnsetProp(pndArg, BAD_CAST "select");
	/*!\todo xmlUnlink(pndCurrent->children); */
	xmlNewTextChild(pndArg, domGetNsXsl(), BAD_CAST"text", pucValueNew);
      }
      xmlFree(pucValueNew);
    }
    else {
      xmlSetProp(pndArg, BAD_CAST "select", BAD_CAST pcValue);
    }

    fResult = TRUE;
  }

  return fResult;
} /* end of cxpUpdateXslVariable() */


/*! global XSL variables only (between RootElement and first xsl:template element)
*/
BOOL_T
cxpChangeXslParam(xmlDocPtr pdocResult, char **param, BOOL_T fInsertvars, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;

  if (pdocResult != NULL && param != NULL && param[0] != NULL) {
    xmlNodePtr pndCurrent;
    xmlNodePtr pndRoot = xmlDocGetRootElement(pdocResult);
    index_t i;

    for (i=0; param[i]; i+=2) {	/* all name/value pairs */
      for (pndCurrent=pndRoot->children; pndCurrent; pndCurrent=pndCurrent->next) {
	if (IS_NODE_XSL_VARIABLE(pndCurrent)) {
	  xmlChar *pucAttrName;

	  if ((pucAttrName = domGetPropValuePtr(pndCurrent, BAD_CAST "name")) != NULL
	    && xmlStrEqual(pucAttrName, BAD_CAST param[i])
	    && cxpUpdateXslVariable(pndCurrent, param[i+1], pccArg)) {
	    /* variable exists already */
	    break;
	  }
	}
	else if (IS_NODE_XSL_TEMPLATE(pndCurrent)) {
	  /*
	  the first xsl:template element is reached, insert this new variable
	  */
	  xmlNodePtr pndVariable;

	  if (fInsertvars) {
	    cxpCtxtLogPrint(pccArg, 2, "Inserting VARIABLE '%s' with value '%s'", param[i], param[i+1]);
	    if ((pndVariable = xmlNewNode(pndCurrent->ns, BAD_CAST "variable")) != NULL
	      && xmlSetProp(pndVariable, BAD_CAST "name", BAD_CAST param[i]) != NULL
	      && cxpUpdateXslVariable(pndVariable, param[i+1], pccArg)) {
	      xmlAddPrevSibling(pndCurrent, pndVariable);
	    }
	    /* insert the xsl:variable after xsl:import and xsl:output
	    -> before first xsl:template
	    */
	  }
	  else {
	    cxpCtxtLogPrint(pccArg, 3, "Ignoring VARIABLE '%s'", param[i]);
	  }
	  break;
	}
      }
    }
    fResult = TRUE;
  }
  return fResult;
} /* end of cxpChangeXslParam() */


/*! apply all sibling XSL elements of pndParent to pdocArgXml, non recursive!!

\param pndArgParent a xmlNodePtr to append data
\param ppdocArgResult
\param ppucArgResult
\param pccArg the resource node

\return TRUE if the transformation was successful, else FALSE

\todo use DOM caching for XSL
*/
BOOL_T
cxpProcessTransformations(const xmlDocPtr pdocArgXml, const xmlNodePtr pndArgParent, xmlDocPtr *ppdocArgResult, xmlChar **ppucArgResult, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;

#ifdef DEBUG
  cxpCtxtLogPrint(pccArg, 3, "cxpProcessTransformations(pdocArgXml=%0x,pndParent=%0x,pccArg=%0x)", pdocArgXml, pndArgParent, pccArg);
#endif

  if (ppdocArgResult != NULL || ppucArgResult != NULL) {

    if (pndArgParent) {
      cxpContextPtr pccHere = pccArg;
      xmlNodePtr pndChild;
      xmlNodePtr pndChildNext = NULL;
      xmlDocPtr pdocResult = NULL;
      xmlChar *pucResult = NULL;

      //pccHere = cxpCtxtFromAttr(pccArg,pndArgParent);

      if (pdocArgXml == NULL) {
	pdocResult = xmlNewDoc(BAD_CAST "1.0");
	if (pdocResult) {
	  xmlNodePtr pndT;

	  pndT = xmlNewDocNode(pdocResult, NULL, BAD_CAST"dummy", NULL);
	  xmlDocSetRootElement(pdocResult, pndT);
	  pdocResult->encoding = xmlStrdup(BAD_CAST "UTF-8"); /* according to conversion in ParseImportNodePlainContent() */
	  cxpCtxtLogPrint(pccArg, 1, "Use a new created dummy DOM");
	}
	else {
	  cxpCtxtLogPrint(pccArg, 1, "Cant create new DOM");
	}
      }
      else {
	pdocResult = xmlCopyDoc(pdocArgXml,1);
	if (pdocResult) {
	  cxpCtxtLogPrint(pccArg, 1, "Use a fresh copy of DOM");
	}
	else {
	  cxpCtxtLogPrint(pccArg, 1, "Cant copy DOM");
	}
      }

      /*! step all transformation nodes (subst, xsl), ending with DOM or plain result
       */
      for (pndChild=pndArgParent->children; pndChild != NULL; pndChild=pndChildNext) {

	pndChildNext = pndChild->next; /* because of node removal after substitution */

	if (IS_VALID_NODE(pndChild) == FALSE) {
	  /* node to ignored */
	}
	else if (IS_NODE_SUBST(pndChild)) {

	  if (pucResult) {
	    xmlChar *pucT;
	    cxpSubstPtr pcxpSubstT;

	    pcxpSubstT = cxpSubstDetect(pndChild, pccHere);
	    pucT = cxpSubstInStringNew(pucResult, pcxpSubstT, pccArg);
	    if (pucT) {
	      xmlFree(pucResult);
	      pucResult = pucT;
	    }
	    cxpSubstFree(pcxpSubstT);
	  }
	  else if (pdocResult) {
	    xmlDocPtr pdocT;	/* preliminary result DOM */

	    pdocT = xmlCopyDoc(pdocResult, 1);
	    if (pdocT) {
	      cxpSubstInChildNodes(xmlDocGetRootElement(pdocT), pndChild, pccArg);
	      xmlFreeDoc(pdocResult);
	      pdocResult = pdocT;
	    }
	    else {
	      cxpCtxtLogPrint(pccArg, 1, "Copy error 'XML'");
	    }
	  }
	  else {
	  }
	}
	else if (IS_NODE_XPATH(pndChild)) {
	  xmlChar* pucXpath;
	  xmlDocPtr pdocT;	/* preliminary result DOM */

	  if (pucResult) {
	    break;
	  }
	  else if (pdocResult == NULL) {
	    break;
	  }
	  else if ((pucXpath = domGetPropValuePtr(pndChild, BAD_CAST"select")) == NULL) {
	  }
	  else if (xmlStrEqual(pucXpath, BAD_CAST"/*")) {
	  }
	  /*!\todo check XPath syntax of pucXpath */
	  else if ((pdocT = domGetXPathDoc(pdocResult, pucXpath)) == NULL) {
	    cxpCtxtLogPrint(pccArg, 1, "XPath error 'XML'");
	  }
	  else {
	    xmlFreeDoc(pdocResult);
	    pdocResult = pdocT;
	  }
	}
	else if (IS_NODE_XSL(pndChild)) {

	  if (pucResult) { /* there is an intermediate plain text result */
	    break;
	  }
	  else if (pdocResult) {
	    char **ppchParam;
	    xmlChar *pucNameFileXsl = NULL;
	    xmlDocPtr pdocXsl;

	    pdocXsl = cxpXslRetrieve(pndChild, pccHere);
	    if (pdocXsl == NULL) {
	      cxpCtxtLogPrint(pccArg, 1, "No DOM retrieved");
	      xmlFreeDoc(pdocResult);
	      pdocResult = NULL;
	      break;
	    }

	    ppchParam = (char **)cxpXslParamProcess(pndChild, pccHere);
	    if (ppchParam) {
	      /*!\todo handle ppchParam[] as Global Vars ? (but see comment at libxslt/variables.c:947) */
	      cxpChangeXslParam(pdocXsl, ppchParam, domGetPropFlag(pndChild, BAD_CAST "insertvars", FALSE), pccArg);
	    }

	    //cxpCtxtLogPrintDoc(pccArg,1,NULL,pdocArgXml);
	    //cxpCtxtLogPrintDoc(pccArg,1,NULL,pdocXsl);

	    if (xmlStrEqual(domGetXslOutputMethod(pdocXsl), BAD_CAST"xml")
	      || xmlStrEqual(domGetXslOutputMethod(pdocXsl), BAD_CAST"html")) {
	      xmlDocPtr pdocT;	/* preliminary result DOM */

	      pdocT = cxpXslTransformToDom(pdocResult, pdocXsl, ppchParam, pccHere);
	      if (pdocT != NULL && pdocT != pdocResult) {
		xmlFreeDoc(pdocResult);
		pdocResult = pdocT; /* new result DOM */
	      }
	      else {
		cxpCtxtLogPrint(pccArg, 1, "No result with Stylesheet '%s'", pucNameFileXsl);
		xmlFreeDoc(pdocResult);
		pdocResult = NULL;
	      }
	    }
	    else if (xmlStrEqual(domGetXslOutputMethod(pdocXsl), BAD_CAST"text")) {
	      /*!\todo test on following XSL nodes */
	      pucResult = cxpXslTransformToText(pdocResult, pdocXsl, ppchParam, pccHere);
	      xmlFreeDoc(pdocResult);
	      pdocResult = NULL; /* loop ends when pdocResult is NULL */
	    }
	    else {
	      xmlFreeDoc(pdocResult);
	      pdocResult = NULL;
	      cxpCtxtLogPrint(pccArg, 1, "No <xsl:output> defined");
	    }

	    cxpXslParamFree(ppchParam);
	    xmlFreeDoc(pdocXsl);
	  }
	  else {
	  }
	}
      }

      if (ppdocArgResult != NULL) { /* DOM result expected */
	assert(ppucArgResult == NULL);
	*ppdocArgResult = pdocResult;
	fResult = TRUE;
      }
      else if (ppucArgResult != NULL) { /* plain text result expected */
	int l = 0;

	assert(ppdocArgResult == NULL);
	if (pucResult) {
	  *ppucArgResult = pucResult;
	}
	else {
	  xmlDocDumpFormatMemoryEnc(pdocResult, ppucArgResult, &l, "UTF-8", 1);
	  xmlFreeDoc(pdocResult);
	}
	fResult = TRUE;
      }
      else {
      }
    }
  }
  return fResult;
} /* end of cxpProcessTransformations() */


/*! try to retrieve a validated XSL DOM

\param pndArg a xmlNodePtr to append data
\param pccArg the resource node

\return
*/
xmlDocPtr
cxpXslRetrieve(const xmlNodePtr pndArgXsl, cxpContextPtr pccArg)
{
  xmlDocPtr pdocResult = NULL;
  xmlNodePtr pndChild;
  xmlNodePtr pndRootXsl;

#ifdef DEBUG
  cxpCtxtLogPrint(pccArg,3,"cxpXslRetrieve(pndArgXsl=%0x,pccArg=%0x)",pndArgXsl,pccArg);
#endif

  if (pndArgXsl == NULL
      || pndArgXsl->type != XML_ELEMENT_NODE
      || IS_VALID_NODE(pndArgXsl) == FALSE) {
    /* ignrore pndArgXsl */
  }
  else if (xmlStrcasecmp(pndArgXsl->name,BAD_CAST "stylesheet")==0
	   && pndArgXsl->ns!=NULL
	   && pndArgXsl->ns->prefix!=NULL
	   && xmlStrcasecmp(pndArgXsl->ns->prefix,BAD_CAST "xsl")==0) {
    /* this is a xsl:stylesheet tree */
    pdocResult = domDocFromNodeNew(pndArgXsl);
    if (pdocResult) {
      //cxpCtxtLogPrintDoc(pccArg,3,"Applying MAKE internal xsl:stylesheet",pdocResult);
    }
    else {
      cxpCtxtLogPrint(pccArg,1,"xsl:stylesheet not usable");
    }
  }
  else if (IS_NODE_XSL(pndArgXsl)) {
    //cxpContextPtr pccHere = pccArg;
    xmlChar *pucAttrFile = domGetPropValuePtr(pndArgXsl,BAD_CAST "name");
    xmlChar *pucAttrFileNorm = NULL;

    //pccHere = cxpCtxtFromAttr(pccArg,pndArgXsl);

    if ((pndChild = domGetFirstChild(pndArgXsl,NAME_XML))) {
      /* the XSL DOM has to be transformed first */
      pdocResult = cxpProcessXmlNode(pndChild,pccArg);
      if (pdocResult != NULL && (pndRootXsl = xmlDocGetRootElement(pdocResult)) != NULL && IS_NODE(pndRootXsl,BAD_CAST"stylesheet")) {
	xmlChar *prop;
	xmlNsPtr pnsXsl;

	prop = xmlGetNsProp(pndRootXsl, (const xmlChar *)"version", XSLT_NAMESPACE);
	if (prop == NULL) {
	  cxpCtxtLogPrint(pccArg,2,"create local namespace for XSL");
	  pnsXsl = xmlNewNs(pndRootXsl,XSLT_NAMESPACE,BAD_CAST "xsl");
	  domSetNsRecursive(pndRootXsl,pnsXsl);
	}
      }
      else {
	cxpCtxtLogPutsExit(pccArg,81,"No stylesheet");
      }
    }
    else if (pucAttrFile) {
      BOOL_T fCache = FALSE;
      xmlChar *pucAttrNameCacheAs;

      if ((pdocResult = cxpCtxtCacheGetDoc(pccArg, pucAttrFile)) != NULL) {
	/* there is a cached DOM */
	pdocResult = xmlCopyDoc(pdocResult, 1);
      }
      else {
	resNodePtr prnFile; /* resource node according to pucAttrFile */

	prnFile = cxpResNodeResolveNew(pccArg, pndArgXsl, NULL, CXP_O_SEARCH);
	if (prnFile) {
	  if (resNodeGetMimeType(prnFile) == MIME_TEXT_XSL) { /* this is a XSL file */
	    xmlChar *pucAttrContext;

	    cxpCtxtLogPrint(pccArg,2, "Reading XSL '%s'", resNodeGetNameNormalized(prnFile));
	    pdocResult = resNodeReadDoc(prnFile);

	    pucAttrContext = domGetPropValuePtr(pndArgXsl,BAD_CAST "context");
	    if (STR_IS_NOT_EMPTY(pucAttrContext)) {
	      resNodePtr prnContext; /* resource node according to pucAttrContext */

	      prnContext = resNodeConcatNew(cxpCtxtLocationGetStr(pccArg),pucAttrContext);
	      if (prnContext != NULL && resNodeIsReadable(prnContext)) {
		/* relocate the DOM URI to */
		domChangeURL(pdocResult,prnContext);
	      }
	      resNodeFree(prnContext);
	    }

	    pucAttrNameCacheAs = domGetPropValuePtr(pndArgXsl,BAD_CAST "cacheas");
	    fCache             = domGetPropFlag(pndArgXsl,BAD_CAST "cache",  FALSE);
	    if (STR_IS_NOT_EMPTY(pucAttrNameCacheAs)) {
	      cxpCtxtCacheAppendDoc(pccArg, pdocResult, pucAttrNameCacheAs);
	    }
	    else if (fCache && xmlStrlen(pucAttrFile) > 0) {
	      cxpCtxtCacheAppendDoc(pccArg, pdocResult, pucAttrFile);
	    }
	  }
	  else {
	  }
	  resNodeFree(prnFile);
	}
	else {
	  /*!\bug 'https://', use sscanf_next_url() ? */
	  cxpCtxtLogPrint(pccArg,1,"XSL source not readable '%s'", pucAttrFile);
	}
      }
      xmlFree(pucAttrFileNorm);
    }
    else {
      cxpCtxtLogPrint(pccArg,1,"No file attribute");
      return NULL;
    }
  }

  if (domGetXslOutputMethod(pdocResult)==NULL) {
    cxpCtxtLogPrint(pccArg, 1, "This XSL DOM has no 'xsl:output' element, ignoring");
    xmlFreeDoc(pdocResult);
    pdocResult = NULL;
  }

  return pdocResult;
}
/* end of cxpXslRetrieve() */


/*! apply XSL element pdocArgXsl to pdocArgXml, recursive

  \param pdocArgXml pointer to DOM to transform
  \param pdocArgXsl pointer to DOM of XSL
  \param ppchArgParam pointer to XSL parameter array
  \param pccArg pointer to current resource node

  \return pdocArgXml if no transformations are done, else a new created xmlDocPtr

  \todo use DOM caching for XSL
  \todo define result DOM encoding
*/
xmlDocPtr
cxpXslTransformToDom(const xmlDocPtr pdocArgXml, const xmlDocPtr pdocArgXsl, char **ppchArgParam, cxpContextPtr pccArg)
{
  xmlDocPtr pdocResult = pdocArgXml;
  xmlDocPtr pdocXslCopy;
  xsltStylesheetPtr pxslT = NULL;

  assert(pdocArgXml != NULL);
  assert(pdocArgXsl != NULL);

#ifdef DEBUG
  cxpCtxtLogPrintDoc(pccArg, 4, "cxpXslTransformToDom() XML:", pdocArgXml);
  cxpCtxtLogPrintDoc(pccArg, 4, "cxpXslTransformToDom() XSL:", pdocArgXsl);
#endif

  pdocXslCopy = xmlCopyDoc(pdocArgXsl,1); /* because of xsltFreeStylesheet() */
  if (pdocXslCopy) {
    pxslT = xsltParseStylesheetDoc(pdocXslCopy);
    if (pxslT) {
      /* apply the stylesheet, check the result
       */
      /*!\bug xsltParseStylesheetDoc() doesnt set pxslT->omitXmlDeclaration correct */
      xmlDocPtr pdocT;

      pdocT = xsltApplyStylesheet(pxslT, pdocArgXml, (const char **)ppchArgParam);
      if (pdocT == NULL) {
	cxpCtxtLogPrint(pccArg,1,"No result with this Stylesheet");
      }
      else if (xmlDocGetRootElement(pdocT) == NULL) {
	cxpCtxtLogPrint(pccArg,1,"No content with this Stylesheet");
	xmlFreeDoc(pdocT);
	pdocT = NULL;
      }
      else {
	pdocResult = pdocT;
      }
      xsltFreeStylesheet(pxslT);  /* xsltFreeStylesheet() releases the DOM also */
#ifdef DEBUG
      //cxpCtxtLogPrintDoc(pccArg, 4, "cxpXslTransformToDom() returns:", pdocResult);
#endif
    }
    else {
      cxpCtxtLogPrint(pccArg,1,"Cant parse this Stylesheet");
      xmlFreeDoc(pdocXslCopy);
    }
  }
  else {
    cxpCtxtLogPrint(pccArg,1,"No copy of Stylesheet DOM possible");
  }

  return pdocResult;
}
/* end of cxpXslTransformToDom() */


/*! apply all sibling XSL elements of pdocArgXsl to pdocArgXml, non recursive!!

  \param pdocArgXml pointer to DOM to transform
  \param pdocArgXsl pointer to DOM of XSL
  \param ppchArgParam pointer to XSL parameter array
  \param pccArg pointer to current resource node

  \return pointer to result string

  \todo use DOM caching for XSL
*/
xmlChar *
cxpXslTransformToText(const xmlDocPtr pdocArgXml, const xmlDocPtr pdocArgXsl, char **ppchArgParam, cxpContextPtr pccArg)
{
  int l;
  xmlDocPtr pdocXslCopy;
  xmlChar *pucResult = NULL;
  xmlDocPtr pdocT;	/* preliminary result DOM */
  xsltStylesheetPtr pxslT = NULL;

  assert(pdocArgXml != NULL);
  assert(pdocArgXsl != NULL);

#ifdef DEBUG
  cxpCtxtLogPrintDoc(pccArg, 4, "cxpXslTransformToDom() XML:", pdocArgXml);
  cxpCtxtLogPrintDoc(pccArg, 4, "cxpXslTransformToDom() XSL:", pdocArgXsl);
#endif

  pdocXslCopy = xmlCopyDoc(pdocArgXsl,1); /* because of xsltFreeStylesheet() */
  if (pdocXslCopy==NULL) {
    cxpCtxtLogPrint(pccArg,1,"No copy of Stylesheet DOM possible");
    return NULL;
  }

  pxslT = xsltParseStylesheetDoc(pdocXslCopy);
  /*!\bug xsltParseStylesheetDoc() doesnt set pxslT->omitXmlDeclaration correct */
  if (pxslT==NULL) {
    cxpCtxtLogPutsExit(pccArg,45,"Cant parse this copied Stylesheet");
  }

  /* apply the stylesheet, check the result
   */
  pdocT = xsltApplyStylesheet(pxslT, pdocArgXml, (const char **)ppchArgParam);
  if (pdocT) {
    /* dump the result into a buffer */
    xsltSaveResultToString(&pucResult,&l,pdocT,pxslT);
    if (l < 1) {
      cxpCtxtLogPrint(pccArg,1,"Empty content with this Stylesheet");
      xmlFree(pucResult);
      pucResult = NULL;
    }
  }
  else {
    cxpCtxtLogPrint(pccArg,1,"No content with this Stylesheet");
  }
  xmlFreeDoc(pdocT);

  if (pxslT) {
    /* release former stylesheets */
    xsltFreeStylesheet(pxslT);
  }

  return pucResult;
}
/* end of cxpXslTransformToText() */


/*! \return TRUE if pndArg and descendants are valid cxp elements

  neither XML Schema nor DTD used because of fast tree validation, not DOM

  for complete DOM see ValidateSchema()
  */
BOOL_T
ValidateCxpTree(xmlNodePtr pndArg, cxpContextPtr pccArg)
{
  BOOL_T fResult = TRUE;
  xmlNodePtr pndChild;
  xmlAttrPtr pndAttr;

  if (IS_VALID_NODE(pndArg) == FALSE) {
    cxpCtxtLogPrint(pccArg,3,"Skipping XML Validation of invalid element");
    return fResult;
  }

  if (domGetPropFlag(pndArg,BAD_CAST "validation",TRUE) == FALSE) {
    cxpCtxtLogPrint(pccArg,2,"Skipping XML Validation");
    return fResult;
  }

  /* return FALSE only if one of childs is not allowed */
  if (IS_NODE_MAKE(pndArg)) {

    for (pndAttr = pndArg->properties; pndAttr;) {
      if (xmlStrEqual(pndAttr->name,BAD_CAST"validation") || xmlStrEqual(pndAttr->name,BAD_CAST"valid") || xmlStrEqual(pndAttr->name,BAD_CAST"log")
	|| xmlStrEqual(pndAttr->name,BAD_CAST"thread") || xmlStrEqual(pndAttr->name,BAD_CAST"context") 
	|| xmlStrEqual(pndAttr->name,BAD_CAST"readonly") || xmlStrEqual(pndAttr->name,BAD_CAST"dir") || xmlStrEqual(pndAttr->name,BAD_CAST"text")) {
      }
      else {
	cxpCtxtLogPrint(pccArg,1,"Validation warning: '%s/@%s' is not used",pndArg->name,pndAttr->name);
      }
      pndAttr = pndAttr->next;
    }

    for (pndChild = pndArg->children; pndChild;) {
      if (xmlNodeIsText(pndChild)) {
	/* e.g. placeholder for substitution */
      }
      else if (IS_NODE_MAKE_CHILD(pndChild)) {
	fResult &= ValidateCxpTree(pndChild, pccArg);
      }
      else {
	cxpCtxtLogPrint(pccArg,1,"Validation error: '%s/%s[@name=\"%s\"]' is not allowed",
	    pndArg->name, pndChild->name,domGetPropValuePtr(pndChild,BAD_CAST"name"));
	fResult = FALSE;
      }
      pndChild = pndChild->next;
    }
  }
  else if (IS_NODE_XML(pndArg)) {    /* main format nodes (with foreign content) */
    int iChildCount;

    for (pndAttr = pndArg->properties; pndAttr;) {
      if (xmlStrEqual(pndAttr->name,BAD_CAST"eval") || xmlStrEqual(pndAttr->name,BAD_CAST"valid") || xmlStrEqual(pndAttr->name,BAD_CAST"context")
	|| xmlStrEqual(pndAttr->name,BAD_CAST"name") || xmlStrEqual(pndAttr->name,BAD_CAST"mode") || xmlStrEqual(pndAttr->name,BAD_CAST"schema")
	|| xmlStrEqual(pndAttr->name,BAD_CAST"type") || xmlStrEqual(pndAttr->name,BAD_CAST"disposition")
	|| xmlStrEqual(pndAttr->name,BAD_CAST"xpath") || xmlStrEqual(pndAttr->name,BAD_CAST"locator")
	|| xmlStrEqual(pndAttr->name,BAD_CAST"encoding") || xmlStrEqual(pndAttr->name,BAD_CAST"cache") || xmlStrEqual(pndAttr->name,BAD_CAST"cacheas")
	|| xmlStrEqual(pndAttr->name,BAD_CAST"search") || xmlStrEqual(pndAttr->name,BAD_CAST"view") || xmlStrEqual(pndAttr->name,BAD_CAST"text")) {
      }
      else {
	cxpCtxtLogPrint(pccArg,1,"Validation warning: '%s/@%s' is not used",pndArg->name,pndAttr->name);
      }
      pndAttr = pndAttr->next;
    }

#if 0
#elif 0
    if (pndArg->children != NULL && cxpGetXmlSourceNode(pndArg) == NULL) {
      cxpCtxtLogPrint(pccArg,1,"Validation error: can't find the source child of xml node");
      fResult = FALSE;
    }

    for (pndChild = pndArg->children; pndChild; pndChild = pndChild->next) {

      if (pndChild->type == XML_ELEMENT_NODE) {

	if (IS_NODE_XSL(pndChild)) {
	  fResult &= ValidateCxpTree(pndChild);
	}
	else if (IS_NODE_MAKE(pndChild) || IS_NODE_THREAD(pndChild)
	  || IS_NODE_FROM(pndChild) || IS_NODE_EACH(pndChild)) {
	  cxpCtxtLogPrint(pccArg,1,"Validation error: '%s/%s[@name=\"%s\"]' is not allowed",
	      pndArg->name, pndChild->name,domGetPropValuePtr(pndChild,BAD_CAST"name"));
	  fResult = FALSE;
	}
	else {
	  fResult &= ValidateCxpTree(pndChild);
	}
      }
    }
#else
    for (iChildCount=0,pndChild = pndArg->children; pndChild;) {

      if (pndChild->type == XML_ELEMENT_NODE) {

	if (iChildCount > 1) {
	  cxpCtxtLogPrint(pccArg,1,"Validation error: only one source child of xml node allowed");
	  //fResult = FALSE;
	}

	if (IS_NODE_XSL(pndChild) || IS_NODE_SUBST(pndChild)) {
	  fResult &= ValidateCxpTree(pndChild, pccArg);
	}
	else if (IS_NODE_MAKE(pndChild)
	  || IS_NODE_FROM(pndChild) || IS_NODE_EACH(pndChild)) {
		cxpCtxtLogPrint(pccArg,1,"Validation error: '%s/%s[@name=\"%s\"]' is not allowed",
		    pndArg->name, pndChild->name,domGetPropValuePtr(pndChild,BAD_CAST"name"));
	  iChildCount++;
	  fResult = FALSE;
	}
	else {
	  iChildCount++;
	  fResult &= ValidateCxpTree(pndChild, pccArg);
	}
      }
      pndChild = pndChild->next;
    }
#endif
  }
  else if (IS_NODE_XHTML(pndArg)) {
    /* main format nodes (with foreign content) */

    for (pndAttr = pndArg->properties; pndAttr;) {
      if (xmlStrEqual(pndAttr->name,BAD_CAST"valid") || xmlStrEqual(pndAttr->name,BAD_CAST"context")
	|| xmlStrEqual(pndAttr->name,BAD_CAST"name") || xmlStrEqual(pndAttr->name,BAD_CAST"mode")
	|| xmlStrEqual(pndAttr->name,BAD_CAST"type") || xmlStrEqual(pndAttr->name,BAD_CAST"disposition")
	|| xmlStrEqual(pndAttr->name, BAD_CAST"xpath")
	|| xmlStrEqual(pndAttr->name,BAD_CAST"encoding") || xmlStrEqual(pndAttr->name,BAD_CAST"cache") || xmlStrEqual(pndAttr->name,BAD_CAST"cacheas")
	|| xmlStrEqual(pndAttr->name,BAD_CAST"search") || xmlStrEqual(pndAttr->name,BAD_CAST"view") || xmlStrEqual(pndAttr->name,BAD_CAST"text")) {
      }
      else {
	cxpCtxtLogPrint(pccArg,1,"Validation warning: '%s/@%s' is not used",pndArg->name,pndAttr->name);
      }
      pndAttr = pndAttr->next;
    }

#if 0
#elif 0
    if (pndArg->children != NULL && cxpGetXmlSourceNode(pndArg) == NULL) {
      cxpCtxtLogPrint(pccArg,1,"Validation error: cant find the source child of xml node");
      fResult = FALSE;
    }
#else
    for (pndChild = pndArg->children; pndChild;) {
      if (IS_NODE_MAKE(pndChild)
	|| IS_NODE_FROM(pndChild) || IS_NODE_EACH(pndChild)) {
	cxpCtxtLogPrint(pccArg,1,"Validation error: '%s/%s[@name=\"%s\"]' is not allowed",
	    pndArg->name, pndChild->name,domGetPropValuePtr(pndChild,BAD_CAST"name"));
	fResult = FALSE;
      }
      else {
	fResult &= ValidateCxpTree(pndChild, pccArg);
      }
      pndChild = pndChild->next;
    }
#endif
  }
  else if (IS_NODE_PLAIN(pndArg)) {
    /* main format nodes (with foreign content) */

    for (pndAttr = pndArg->properties; pndAttr;) {
      if (xmlStrEqual(pndAttr->name,BAD_CAST"valid") || xmlStrEqual(pndAttr->name,BAD_CAST"context")
	|| xmlStrEqual(pndAttr->name,BAD_CAST"name") || xmlStrEqual(pndAttr->name,BAD_CAST"mode")
	|| xmlStrEqual(pndAttr->name,BAD_CAST"type") || xmlStrEqual(pndAttr->name,BAD_CAST"disposition")
	|| xmlStrEqual(pndAttr->name, BAD_CAST"xpath")
	|| xmlStrEqual(pndAttr->name,BAD_CAST"encoding") || xmlStrEqual(pndAttr->name,BAD_CAST"cache") || xmlStrEqual(pndAttr->name,BAD_CAST"cacheas")
	|| xmlStrEqual(pndAttr->name,BAD_CAST"search") || xmlStrEqual(pndAttr->name,BAD_CAST"view") || xmlStrEqual(pndAttr->name,BAD_CAST"text")) {
      }
      else {
	cxpCtxtLogPrint(pccArg,1,"Validation warning: '%s/@%s' is not used",pndArg->name,pndAttr->name);
      }
      pndAttr = pndAttr->next;
    }

    for (pndChild = pndArg->children; pndChild;) {
      if (pndChild->type == XML_ELEMENT_NODE) {
	if (IS_NODE_XML(pndChild) || IS_NODE_XSL(pndChild) || IS_NODE_PLAIN(pndChild) || IS_NODE_IMAGE(pndChild) || IS_NODE_SCRIPT(pndChild)
	  || IS_NODE_DB(pndChild) || IS_NODE_QUERY(pndChild)) {
	  fResult &= ValidateCxpTree(pndChild, pccArg);
	}
	else {
		cxpCtxtLogPrint(pccArg,1,"Validation error: '%s/%s[@name=\"%s\"]' is not allowed",
		    pndArg->name, pndChild->name,domGetPropValuePtr(pndChild,BAD_CAST"name"));
	//  fResult = FALSE;
	}
      }
      pndChild = pndChild->next;
    }
  }
  else if (IS_NODE_JSON(pndArg)) {
  }
  else if (IS_NODE(pndArg,BAD_CAST"stylesheet")) {
    /* trust XSLT nodes */
    cxpCtxtLogPrint(pccArg,2,"Skipping XSLT tree Validation");
  }
  else if (IS_NODE_XSL(pndArg)) {
    /*  nodes */

    for (pndAttr = pndArg->properties; pndAttr;) {
      if (xmlStrEqual(pndAttr->name,BAD_CAST"valid") || xmlStrEqual(pndAttr->name,BAD_CAST"context") || xmlStrEqual(pndAttr->name,BAD_CAST"xpath")
	|| xmlStrEqual(pndAttr->name,BAD_CAST"name") || xmlStrEqual(pndAttr->name,BAD_CAST"cache") || xmlStrEqual(pndAttr->name,BAD_CAST"cacheas")
	|| xmlStrEqual(pndAttr->name,BAD_CAST"appendcgi") || xmlStrEqual(pndAttr->name,BAD_CAST"insertvars") || xmlStrEqual(pndAttr->name,BAD_CAST"search")) {
      }
      else {
	cxpCtxtLogPrint(pccArg,1,"Validation warning: '%s/@%s' is not used",pndArg->name,pndAttr->name);
      }
      pndAttr = pndAttr->next;
    }

    for (pndChild = pndArg->children; pndChild;) {
      if (IS_NODE_XML(pndChild) || IS_NODE_XSL(pndChild) || IS_NODE_VARIABLE(pndChild) || xmlNodeIsText(pndChild) || IS_COMMENT(pndChild)) {
	/* valid */
	fResult &= ValidateCxpTree(pndChild, pccArg);
      }
      else if (pndChild->type == XML_TEXT_NODE) {
	/* valid */
      }
      else {
	cxpCtxtLogPrint(pccArg,1,"Validation error: '%s/%s[@name=\"%s\"]' is not allowed",
	    pndArg->name, pndChild->name,domGetPropValuePtr(pndChild,BAD_CAST"name"));
	fResult = FALSE;
      }
      pndChild = pndChild->next;
    }
  }
  else if (IS_NODE_VARIABLE(pndArg)) {
    /*  nodes */
    for (pndAttr = pndArg->properties; pndAttr;) {
      if (xmlStrEqual(pndAttr->name,BAD_CAST"valid") || xmlStrEqual(pndAttr->name,BAD_CAST"argv") || xmlStrEqual(pndAttr->name,BAD_CAST"env")
	|| xmlStrEqual(pndAttr->name,BAD_CAST"name") || xmlStrEqual(pndAttr->name,BAD_CAST"select") || xmlStrEqual(pndAttr->name,BAD_CAST"cgi")
	|| xmlStrEqual(pndAttr->name,BAD_CAST"now") || xmlStrEqual(pndAttr->name,BAD_CAST"default")) {
      }
      else {
	cxpCtxtLogPrint(pccArg,1,"Validation warning: '%s/@%s' is not used",pndArg->name,pndAttr->name);
      }
      pndAttr = pndAttr->next;
    }
  }
  else if (IS_NODE_FILE(pndArg) || IS_NODE_DIR(pndArg)) {
    /* main format nodes (with foreign content) */

    for (pndAttr = pndArg->properties; pndAttr;) {
      if (xmlStrEqual(pndAttr->name,BAD_CAST"valid") || xmlStrEqual(pndAttr->name,BAD_CAST"verbosity") || xmlStrEqual(pndAttr->name,BAD_CAST"name") 
	|| xmlStrEqual(pndAttr->name,BAD_CAST"type") || xmlStrEqual(pndAttr->name,BAD_CAST"disposition")
	|| xmlStrEqual(pndAttr->name,BAD_CAST"index") || xmlStrEqual(pndAttr->name,BAD_CAST"depth") || xmlStrEqual(pndAttr->name,BAD_CAST"maxdepth") 
	|| xmlStrEqual(pndAttr->name,BAD_CAST"imatch") || xmlStrEqual(pndAttr->name,BAD_CAST"match") || xmlStrEqual(pndAttr->name,BAD_CAST"grep")
	|| xmlStrEqual(pndAttr->name,BAD_CAST"hidden") || xmlStrEqual(pndAttr->name,BAD_CAST"map") || xmlStrEqual(pndAttr->name,BAD_CAST"urlencode")) {
      }
      else {
	cxpCtxtLogPrint(pccArg,1,"Validation warning: '%s/@%s' is not used",pndArg->name,pndAttr->name);
      }
      pndAttr = pndAttr->next;
    }

    for (pndChild = pndArg->children; pndChild;) {
      if (IS_NODE_FILE(pndChild) || IS_NODE_DIR(pndChild) || IS_COMMENT(pndChild)) {
	/* valid */
	fResult &= ValidateCxpTree(pndChild, pccArg);
      }
      else if (pndChild->type == XML_TEXT_NODE) {
	/* valid */
      }
      else {
	cxpCtxtLogPrint(pccArg,1,"Validation error: '%s/%s[@name=\"%s\"]' is not allowed",
	    pndArg->name, pndChild->name,domGetPropValuePtr(pndChild,BAD_CAST"name"));
	fResult = FALSE;
      }
      pndChild = pndChild->next;
    }
  }
  else if (IS_NODE_EACH(pndArg)) {
    /*  nodes */

    for (pndAttr = pndArg->properties; pndAttr;) {
      if (xmlStrEqual(pndAttr->name,BAD_CAST"valid") || xmlStrEqual(pndAttr->name,BAD_CAST"name")) {
      }
      else {
	cxpCtxtLogPrint(pccArg,1,"Validation warning: '%s/@%s' is not used",pndArg->name,pndAttr->name);
      }
      pndAttr = pndAttr->next;
    }

    for (pndChild = pndArg->children; pndChild;) {
      if (IS_NODE_FROM(pndChild) || IS_NODE_MAKE_CHILD(pndChild) || IS_COMMENT(pndChild)) {
	/* valid */
	fResult &= ValidateCxpTree(pndChild, pccArg);
      }
      else if (pndChild->type == XML_TEXT_NODE) {
	/* valid */
      }
      else {
	cxpCtxtLogPrint(pccArg,1,"Validation error: '%s/%s[@name=\"%s\"]' is not allowed",
	    pndArg->name, pndChild->name,domGetPropValuePtr(pndChild,BAD_CAST"name"));
	fResult = FALSE;
      }
      pndChild = pndChild->next;
    }
  }
  else if (IS_NODE_DB(pndArg)) {
    /*  nodes */

    for (pndAttr = pndArg->properties; pndAttr;) {
      if (xmlStrEqual(pndAttr->name,BAD_CAST"valid") || xmlStrEqual(pndAttr->name,BAD_CAST"name") || xmlStrEqual(pndAttr->name,BAD_CAST"append")
	|| xmlStrEqual(pndAttr->name,BAD_CAST"write") || xmlStrEqual(pndAttr->name,BAD_CAST"dump")) {
      }
      else {
	cxpCtxtLogPrint(pccArg,1,"Validation warning: '%s/@%s' is not used",pndArg->name,pndAttr->name);
      }
      pndAttr = pndAttr->next;
    }

    for (pndChild = pndArg->children; NULL && pndChild;) {
      if (IS_NODE_PLAIN(pndChild) || IS_NODE_FILE(pndChild) || IS_NODE_DIR(pndChild) || IS_NODE_SCRIPT(pndChild) || IS_COMMENT(pndChild)) {
	/* valid */
	fResult &= ValidateCxpTree(pndChild, pccArg);
      }
      else if (pndChild->type == XML_TEXT_NODE) {
	/* valid */
      }
      else {
	cxpCtxtLogPrint(pccArg,1,"Validation error: '%s/%s[@name=\"%s\"]' is not allowed",
	    pndArg->name, pndChild->name,domGetPropValuePtr(pndChild,BAD_CAST"name"));
	fResult = FALSE;
      }
      pndChild = pndChild->next;
    }
  }
  else if (IS_NODE_ZIP(pndArg)) {
    /*  nodes */

    for (pndAttr = pndArg->properties; pndAttr;) {
      if (xmlStrEqual(pndAttr->name,BAD_CAST"valid") || xmlStrEqual(pndAttr->name,BAD_CAST"name") || xmlStrEqual(pndAttr->name,BAD_CAST"append")
	|| xmlStrEqual(pndAttr->name,BAD_CAST"level") || xmlStrEqual(pndAttr->name,BAD_CAST"context") || xmlStrEqual(pndAttr->name,BAD_CAST"map")) {
      }
      else {
	cxpCtxtLogPrint(pccArg,1,"Validation warning: '%s/@%s' is not used",pndArg->name,pndAttr->name);
      }
      pndAttr = pndAttr->next;
    }

    for (pndChild = pndArg->children; pndChild;) {
      if (IS_NODE_XML(pndChild) || IS_NODE_PLAIN(pndChild) || IS_NODE_FILE(pndChild) || IS_NODE_DIR(pndChild) || IS_NODE_SCRIPT(pndChild) || IS_COMMENT(pndChild)) {
	/* valid */
	fResult &= ValidateCxpTree(pndChild, pccArg);
      }
      else if(pndChild->type == XML_TEXT_NODE) {
	/* valid */
      }
      else {
	cxpCtxtLogPrint(pccArg,1,"Validation error: '%s/%s[@name=\"%s\"]' is not allowed",
	    pndArg->name, pndChild->name,domGetPropValuePtr(pndChild,BAD_CAST"name"));
	fResult = FALSE;
      }
      pndChild = pndChild->next;
    }
  }
  else if (IS_NODE_QUERY(pndArg)) {
    /*  nodes */

    for (pndAttr = pndArg->properties; pndAttr;) {
      if (xmlStrEqual(pndAttr->name,BAD_CAST"valid") || xmlStrEqual(pndAttr->name,BAD_CAST"declaration")) {
      }
      else {
	cxpCtxtLogPrint(pccArg,1,"Validation warning: '%s/@%s' is not used",pndArg->name,pndAttr->name);
      }
      pndAttr = pndAttr->next;
    }

    for (pndChild = pndArg->children; pndChild;) {
      if (xmlNodeIsText(pndChild) || IS_COMMENT(pndChild) || IS_NODE_PLAIN(pndChild) || IS_NODE_QUERY(pndChild)) {
	/* valid */
	fResult &= ValidateCxpTree(pndChild, pccArg);
      }
      else if (pndChild->type == XML_TEXT_NODE) {
	/* valid */
      }
      else {
	cxpCtxtLogPrint(pccArg,1,"Validation error: '%s/%s[@name=\"%s\"]' is not allowed",
	    pndArg->name, pndChild->name,domGetPropValuePtr(pndChild,BAD_CAST"name"));
	fResult = FALSE;
      }
      pndChild = pndChild->next;
    }
  }
  else if (IS_NODE_CALENDAR(pndArg)) {
    /*  nodes */

    for (pndAttr = pndArg->properties; pndAttr;) {
      if (xmlStrEqual(pndAttr->name,BAD_CAST"valid") || xmlStrEqual(pndAttr->name,BAD_CAST"subst")
	  || xmlStrEqual(pndAttr->name,BAD_CAST"type") || xmlStrEqual(pndAttr->name,BAD_CAST"columns")
	  || xmlStrEqual(pndAttr->name,BAD_CAST"year") || xmlStrEqual(pndAttr->name,BAD_CAST"coordinate") || xmlStrEqual(pndAttr->name,BAD_CAST"timezone")) {
      }
      else {
	cxpCtxtLogPrint(pccArg,1,"Validation warning: '%s/@%s' is not used",pndArg->name,pndAttr->name);
      }
      pndAttr = pndAttr->next;
    }

    for (pndChild = pndArg->children; pndChild;) {
      if (IS_NODE_COL(pndChild) || IS_COMMENT(pndChild)) {
	/* valid */
	fResult &= ValidateCxpTree(pndChild, pccArg);
      }
      else if (pndChild->type == XML_TEXT_NODE) {
	/* valid */
      }
      else {
	cxpCtxtLogPrint(pccArg,1,"Validation error: '%s/%s[@name=\"%s\"]' is not allowed",
	    pndArg->name, pndChild->name,domGetPropValuePtr(pndChild,BAD_CAST"name"));
	fResult = FALSE;
      }
      pndChild = pndChild->next;
    }
  }
  else if (IS_NODE_COL(pndArg)) {
    /*  nodes */

    for (pndAttr = pndArg->properties; pndAttr;) {
      if (xmlStrEqual(pndAttr->name,BAD_CAST"valid") || xmlStrEqual(pndAttr->name,BAD_CAST"id") || xmlStrEqual(pndAttr->name,BAD_CAST"name")) {
      }
      else {
	cxpCtxtLogPrint(pccArg,1,"Validation warning: '%s/@%s' is not used",pndArg->name,pndAttr->name);
      }
      pndAttr = pndAttr->next;
    }

    for (pndChild = pndArg->children; pndChild;) {
      if (IS_NODE_XML(pndChild) || IS_NODE_PIE(pndChild) || IS_COMMENT(pndChild)) {
	/* valid */
	fResult &= ValidateCxpTree(pndChild, pccArg);
      }
      else if (pndChild->type == XML_TEXT_NODE) {
	/* valid */
      }
      else {
	cxpCtxtLogPrint(pccArg,1,"Validation error: '%s/%s[@name=\"%s\"]' is not allowed",
	    pndArg->name, pndChild->name,domGetPropValuePtr(pndChild,BAD_CAST"name"));
	fResult = FALSE;
      }
      pndChild = pndChild->next;
    }
  }
  return fResult;
}
/* end of ValidateCxpTree() */


/*! test a node if \@view is set to "yes" and call the named file or directory
*/
BOOL_T
cxpViewNodeResult(xmlNodePtr pndArg, cxpContextPtr pccArg)
{
#ifdef HAVE_CGI
    /* ignore anything here in CGI mode*/
#else
  xmlChar *pucAttrName;
  xmlChar mpucCall[BUFFER_LENGTH];
    resNodePtr prnTarget;

  if (domGetPropFlag(pndArg, BAD_CAST "view", FALSE)
    &&
    (((pucAttrName = domGetPropValuePtr(pndArg, BAD_CAST "name")) != NULL && resPathIsStd(pucAttrName) == FALSE)
      || (pucAttrName = domGetPropValuePtr(pndArg, BAD_CAST "to")) != NULL
      || (pucAttrName = domGetPropValuePtr(pndArg, BAD_CAST "chdir")) != NULL
      || (pucAttrName = domGetPropValuePtr(pndArg, BAD_CAST "mkdir")) != NULL
      )
    && (prnTarget = cxpResNodeResolveNew(pccArg, NULL, pucAttrName, CXP_O_READ)) != NULL) {
    /*
    asynchronous call for viewing
    */
    char* pchNativeCall;
    xmlChar *pucAttrPath;
    xmlChar *pucPathCall;

    pucPathCall = xmlStrdup(BAD_CAST resNodeGetNameNormalizedNative(prnTarget));

    if ((pucAttrPath = domGetPropValuePtr(pndArg, BAD_CAST "path")) != NULL && xmlStrlen(pucAttrPath) > 0) {
      /* append the anchor/path to the filename */
      pucPathCall = xmlStrcat(pucPathCall, BAD_CAST "#");
      pucPathCall = xmlStrcat(pucPathCall, pucAttrPath);
      cxpCtxtLogPrint(pccArg,3,"Concat Path '%s'",pucPathCall);
    }

    xmlStrPrintf(mpucCall, BUFFER_LENGTH,
#ifdef _MSC_VER
      "explorer.exe \"%s\" & ",
#else
      "firefox -remote 'openFile(%s, new-tab)'",
#endif
      pucPathCall);
    xmlFree(pucPathCall);

    cxpCtxtLogPrint(pccArg,2,"Call '%s' for VIEW",mpucCall);
    /*!\bug shell in system() encode '#' to '%23' */

    if ((pchNativeCall = cxpCtxtEncShellCommand(pccArg, mpucCall)) != NULL) {
      assert(strlen(pchNativeCall) > 0);
      system(pchNativeCall);
      xmlFree(pchNativeCall);
    }
  }
#endif
  return TRUE;
} /* end of cxpViewNodeResult() */


/*! \return 
*/
BOOL_T
cxpProcessCopyNode(xmlNodePtr pndArgCopy, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;

  if (IS_NODE_FILECOPY(pndArgCopy)) {
    xmlChar *pucFrom = NULL;
    xmlChar *pucTo = NULL;
    xmlChar *pucAttrResponse = NULL;
    resNodePtr prnFrom = NULL;
    resNodePtr prnContent = NULL;
    resNodePtr prnTo = NULL;

    if ((pucTo = domGetPropValuePtr(pndArgCopy, BAD_CAST "to")) == NULL || STR_IS_EMPTY(pucTo)
      || (prnTo = cxpResNodeResolveNew(pccArg, pndArgCopy, pucTo, CXP_O_WRITE)) == NULL) {
#ifdef HAVE_CGI
      printf("Status: 501\r\n"
	"Content-Type: text/plain\r\n\r\n"
	"Cxproc write error '%s'\r\n", pucTo);
#endif
      cxpCtxtLogPrint(pccArg, 1, "No valid target name");
    }
    else if ((pucFrom = domGetPropValuePtr(pndArgCopy, BAD_CAST "from")) == NULL || STR_IS_EMPTY(pucFrom)
      || (prnFrom = cxpResNodeResolveNew(pccArg, pndArgCopy, pucFrom, ((domGetPropFlag(pndArgCopy, BAD_CAST"search", FALSE)) ? CXP_O_SEARCH | CXP_O_READ : CXP_O_READ))) == NULL) {
#ifdef HAVE_CGI
      printf("Status: 501\r\n"
	"Content-Type: text/plain\r\n\r\n"
	"Cxproc read error '%s'\r\n", pucFrom);
#endif
      cxpCtxtLogPrint(pccArg, 1, "No valid source name '%s'", pucFrom);
    }
    else if (cxpCtxtAccessIsPermitted(pccArg, prnFrom) == FALSE
      || cxpCtxtAccessIsPermitted(pccArg, prnTo) == FALSE) {
#ifdef HAVE_CGI
      printf("Status: 503\r\n"
	"Content-Type: text/plain\r\n\r\n"
	"Cxproc access error '%s' '%s'\r\n", pucFrom, pucTo);
#endif
      cxpCtxtLogPrint(pccArg, 1, "Cxproc access error '%s' '%s'\r\n", pucFrom, pucTo);
    }
    else if ((prnContent = resNodeGetLastDescendant(prnFrom)) == NULL
      || resNodeIsDir(prnContent) || resNodeIsDirInArchive(prnContent)) {
#ifdef HAVE_CGI
      printf("Status: 503\r\n"
	"Content-Type: text/plain\r\n\r\n"
	"Cxproc access error '%s' '%s'\r\n", pucFrom, pucTo);
#endif
      cxpCtxtLogPrint(pccArg, 1, "Cxproc access error '%s' '%s'\r\n", pucFrom, pucTo);
    }
    else if (resNodeIsStd(prnTo)) {
#ifdef HAVE_CGI
      /* s. RFC 1806 */
      xmlChar *pucAttrType = domGetPropValuePtr(pndArgCopy, BAD_CAST "type");
      xmlChar *pucAttrDisposition = domGetPropValuePtr(pndArgCopy, BAD_CAST "disposition");

      printf("Content-Type: %s\n", (pucAttrType && xmlStrlen(pucAttrType)>5) ? pucAttrType : BAD_CAST resNodeGetMimeTypeStr(prnContent));
      printf("Content-Disposition: attachment; filename=%s\n", (pucAttrDisposition && xmlStrlen(pucAttrDisposition)>5) ? pucAttrDisposition : resNodeGetNameBase(prnContent));
      printf("Content-Description: Dynamic cxproc content\n\n");
#endif
      if (resNodeTransfer(prnContent, prnTo, FALSE) == FALSE) {
#ifdef HAVE_CGI
	printf("Status: 503\r\n"
	  "Content-Type: text/plain\r\n\r\n"
	  "Cxproc copy error '%s' '%s'\r\n", pucFrom, pucTo);
#endif
      }
      else {
	fResult = TRUE;
      }
    }
    else if (resNodeTransfer(prnContent, prnTo, domGetPropFlag(pndArgCopy, BAD_CAST "delete", FALSE)) == FALSE) {
#ifdef HAVE_CGI
      printf("Status: 503\r\n"
	"Content-Type: text/plain\r\n\r\n"
	"Cxproc copy error '%s' '%s'\r\n", pucFrom, pucTo);
#endif
    }
    else {
      fResult = TRUE;
    }

#ifdef HAVE_CGI
    if ((pucAttrResponse = domGetPropValuePtr(pndArgCopy, BAD_CAST "response"))) {
      printf("Status: 200 OK\r\n"
	"Content-Type: text/plain;\r\n\r\n"
	"Cxproc '%s' '%s' = '%s' OK\r\n", pucFrom, pucTo, pucAttrResponse);
    }
#endif
    
    fflush(stdout); /*! because problems with VC++ (reverse order in stdout) */
    
    resNodeFree(prnTo);
    resNodeFree(prnFrom);
    cxpViewNodeResult(pndArgCopy, pccArg);
  }
  return fResult;
} /* end of cxpProcessCopyNode() */


/*! \return a DOM containing all program information (meta, libs, options, environment)
*/
xmlNodePtr
cxpInfoProgram(xmlNodePtr pndArg, cxpContextPtr pccArg)
{
  xmlNodePtr pndResult;

  if (pndArg) {
    pndResult = xmlNewChild(pndArg, NULL, BAD_CAST"program", NULL);
  }
  else {
    pndResult = xmlNewNode(NULL, BAD_CAST"program");
  }

  if (pndResult) {
    xmlNodePtr nodeOption;
    xmlNodePtr nodeSource;

    xmlSetProp(pndResult, BAD_CAST "name", BAD_CAST CXP_VER_PRODUCTNAME_STR);

    nodeSource = xmlNewChild(pndResult, NULL, BAD_CAST"source", NULL);
    xmlSetProp(nodeSource, BAD_CAST "url", BAD_CAST CXP_VERSION_URL);
    xmlSetProp(nodeSource, BAD_CAST "version", BAD_CAST CXP_VERSION_STR);
    xmlSetProp(nodeSource, BAD_CAST "revision", BAD_CAST CXP_REVISION_STR);

    nodeOption = xmlNewChild(pndResult, NULL, BAD_CAST"compilation", NULL);
    xmlSetProp(nodeOption, BAD_CAST "build", BAD_CAST CXP_VERSION_BUILD_STR);
    xmlSetProp(nodeOption, BAD_CAST "platform", BAD_CAST
#ifdef _MSC_VER
#ifdef _WIN64
      "Windows 64bit"
#elif defined _WIN32
      "Windows 32bit"
#else
      "Win???"
#endif
#elif defined _WIN32
      "Minimal GNU/Windows 32Bit"
#elif defined (__i386__)
      "GNU/Linux 32bit"
#elif defined (__x86_64__)
      "GNU/Linux 64bit"
#elif defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7S__)
      "GNU/Linux ARM7 32bit"
#else
      "???"
#endif
      );

#ifdef __TIMESTAMP__
    xmlSetProp(nodeOption, BAD_CAST "date", BAD_CAST __TIMESTAMP__);
#endif

#ifdef __cplusplus
    xmlSetProp(nodeOption, BAD_CAST "lang", BAD_CAST"C++");
#else
    xmlSetProp(nodeOption, BAD_CAST "lang", BAD_CAST"C");
#endif

#ifdef EXPERIMENTAL
    xmlSetProp(nodeOption, BAD_CAST "experimental", BAD_CAST"yes");
#else
    xmlSetProp(nodeOption, BAD_CAST "experimental", BAD_CAST"no");
#endif

#ifdef DEBUG
    xmlSetProp(nodeOption, BAD_CAST "debug", BAD_CAST"yes");
#else
    xmlSetProp(nodeOption, BAD_CAST "debug", BAD_CAST"no");
#endif

  }
  return pndResult;
} /* end of cxpInfoProgram() */


/*! \return a DOM containing all program information (meta, libs, options, environment)
*/
xmlDocPtr
cxpProcessInfoNode(xmlNodePtr pndInfo, cxpContextPtr pccArg)
{
  int i;
  xmlChar mpucOut[BUFFER_LENGTH];
  xmlChar *pucArgv;
  xmlDocPtr pdocResult;
  xmlNodePtr nodeProgram;
  xmlNodePtr nodeRuntime;
  xmlNodePtr nodeOption;
  xmlNodePtr pndRoot;

  pdocResult = xmlNewDoc(BAD_CAST "1.0");
  pndRoot = xmlNewDocNode(pdocResult, NULL, NAME_INFO, NULL); 
  xmlDocSetRootElement(pdocResult, pndRoot);
  xmlSetNs(pndRoot,cxpGetNs());

  nodeProgram = cxpInfoProgram(pndRoot, pccArg);
  if (nodeProgram) {
#ifdef __GLIBC__
    nodeOption = xmlNewChild(nodeProgram, NULL, BAD_CAST"lib", NULL);
    xmlSetProp(nodeOption,BAD_CAST "name",BAD_CAST"libc");
    xmlSetProp(nodeOption,BAD_CAST "ns",BAD_CAST"http://www.gnu.org/software/libc/");
    xmlStrPrintf(mpucOut,BUFFER_LENGTH, "%i.%i", __GLIBC__, __GLIBC_MINOR__);
    xmlSetProp(nodeOption,BAD_CAST "version",mpucOut);
#endif

#ifdef _LIBICONV_VERSION
    nodeOption = xmlNewChild(nodeProgram, NULL, BAD_CAST"lib", NULL);
    xmlSetProp(nodeOption,BAD_CAST "name",BAD_CAST"libiconv");
    xmlSetProp(nodeOption,BAD_CAST "ns",BAD_CAST"http://www.gnu.org/software/libiconv/");
    xmlStrPrintf(mpucOut,BUFFER_LENGTH,"%i.%i", (_LIBICONV_VERSION >> 8), (_LIBICONV_VERSION & 0xFF));
    xmlSetProp(nodeOption,BAD_CAST "version",mpucOut);
#endif

    /* configured options */
    nodeOption = xmlNewChild(nodeProgram, NULL, BAD_CAST"lib", NULL);
    xmlSetProp(nodeOption,BAD_CAST "name",BAD_CAST"libxml2");
    xmlSetProp(nodeOption,BAD_CAST "ns",BAD_CAST"http://xmlsoft.org/");
    xmlSetProp(nodeOption,BAD_CAST "version",BAD_CAST LIBXML_DOTTED_VERSION);

    nodeOption = xmlNewChild(nodeProgram, NULL, BAD_CAST"lib", NULL);
    xmlSetProp(nodeOption,BAD_CAST "name",BAD_CAST"libxslt");
    xmlSetProp(nodeOption,BAD_CAST "ns",BAD_CAST"http://xmlsoft.org/XSLT/");
    xmlSetProp(nodeOption,BAD_CAST "version",BAD_CAST LIBXSLT_DOTTED_VERSION);

#ifdef HAVE_PCRE2
    nodeOption = xmlNewChild(nodeProgram, NULL, BAD_CAST"lib", NULL);
    xmlSetProp(nodeOption,BAD_CAST "name",BAD_CAST"libpcre2");
    xmlSetProp(nodeOption,BAD_CAST "ns",BAD_CAST"http://www.pcre.org/");
    xmlStrPrintf(mpucOut,BUFFER_LENGTH, "%i.%i",PCRE2_MAJOR,PCRE2_MINOR);
    xmlSetProp(nodeOption,BAD_CAST "version",mpucOut);
#endif

    nodeOption = xmlNewChild(nodeProgram,NULL,BAD_CAST"module",NULL);
    xmlSetProp(nodeOption,BAD_CAST "name",BAD_CAST"petrinet");
#ifdef HAVE_PETRINET
    xmlSetProp(nodeOption, BAD_CAST "ns", BAD_CAST CXP_PETRINET_URL);
    xmlSetProp(nodeOption,BAD_CAST "select",BAD_CAST "yes");
#else
    xmlSetProp(nodeOption,BAD_CAST "select",BAD_CAST "no");
#endif

    nodeOption = xmlNewChild(nodeProgram,NULL,BAD_CAST"module",NULL);
    xmlSetProp(nodeOption,BAD_CAST "name",BAD_CAST"pie");
#ifdef HAVE_PIE
    xmlSetProp(nodeOption, BAD_CAST "ns", BAD_CAST CXP_PIE_URL);
    xmlSetProp(nodeOption,BAD_CAST "select",BAD_CAST "yes");
#else
    xmlSetProp(nodeOption,BAD_CAST "select",BAD_CAST "no");
#endif

    nodeOption = xmlNewChild(nodeProgram, NULL, BAD_CAST"module", NULL);
    xmlSetProp(nodeOption,BAD_CAST "name",BAD_CAST"admesh");
    xmlSetProp(nodeOption,BAD_CAST "ns",BAD_CAST"http://www.varlog.com/");
#ifdef WITH_RP
    xmlSetProp(nodeOption,BAD_CAST "select", BAD_CAST "yes");
    //xmlSetProp(nodeOption,BAD_CAST "version",BAD_CAST SQLITE_VERSION);
#else
    xmlSetProp(nodeOption,BAD_CAST "select", BAD_CAST "no");
#endif

    nodeOption = xmlNewChild(nodeProgram, NULL, BAD_CAST"module", NULL);
    xmlSetProp(nodeOption, BAD_CAST "name", BAD_CAST"xmlzipio");
    xmlSetProp(nodeOption, BAD_CAST "ns", BAD_CAST"http://hal.iwr.uni-heidelberg.de/~christi/projects/xmlzipio.html");
#ifdef HAVE_ZLIB
    xmlSetProp(nodeOption,BAD_CAST "select",BAD_CAST "yes");
#else
    xmlSetProp(nodeOption, BAD_CAST "select", BAD_CAST "no");
#endif


    nodeOption = xmlNewChild(nodeProgram,NULL,BAD_CAST"module",NULL);
    xmlSetProp(nodeOption,BAD_CAST "name",BAD_CAST"jsmn");
    xmlSetProp(nodeOption,BAD_CAST "ns",BAD_CAST"https://github.com/zserge/jsmn/");
#ifdef HAVE_JSON
    xmlSetProp(nodeOption,BAD_CAST "select",BAD_CAST "yes");
    //xmlSetProp(nodeOption,BAD_CAST "version",BAD_CAST "");
#else
    xmlSetProp(nodeOption,BAD_CAST "select",BAD_CAST "no");
#endif

    nodeOption = xmlNewChild(nodeProgram,NULL,BAD_CAST"option",NULL);
    xmlSetProp(nodeOption,BAD_CAST "name",BAD_CAST"libarchive");
    xmlSetProp(nodeOption,BAD_CAST "ns",BAD_CAST"http://www.libarchive.org/");
#ifdef HAVE_LIBARCHIVE
    xmlSetProp(nodeOption,BAD_CAST "select", BAD_CAST "yes");
    xmlStrPrintf(mpucOut, BUFFER_LENGTH, "%i.%i.%i", 
      (int)ARCHIVE_VERSION_NUMBER / (int)1e6,
      ((int)ARCHIVE_VERSION_NUMBER % (int)1e6) / (int)1e3, 
      (int)ARCHIVE_VERSION_NUMBER % (int)1e3);
    xmlSetProp(nodeOption, BAD_CAST "version", BAD_CAST mpucOut);

#else
    xmlSetProp(nodeOption,BAD_CAST "select", BAD_CAST "no");
#endif

    nodeOption = xmlNewChild(nodeProgram, NULL, BAD_CAST"option", NULL);
    xmlSetProp(nodeOption, BAD_CAST "name", BAD_CAST"sqlite");
    xmlSetProp(nodeOption, BAD_CAST "ns", BAD_CAST"http://www.sqlite.org/");
#ifdef HAVE_LIBSQLITE3
    xmlSetProp(nodeOption, BAD_CAST "select", BAD_CAST "yes");
    xmlSetProp(nodeOption, BAD_CAST "version", BAD_CAST SQLITE_VERSION);
#else
    xmlSetProp(nodeOption, BAD_CAST "select", BAD_CAST "no");
#endif

    nodeOption = xmlNewChild(nodeProgram, NULL, BAD_CAST"option", NULL);
    xmlSetProp(nodeOption,BAD_CAST "name",BAD_CAST"libexif");
    xmlSetProp(nodeOption,BAD_CAST "ns",BAD_CAST"http://libexif.sourceforge.net/");
#ifdef HAVE_LIBEXIF
    xmlSetProp(nodeOption,BAD_CAST "select", BAD_CAST "yes");
    xmlSetProp(nodeOption,BAD_CAST "version",BAD_CAST "0.6.21");
#else
    xmlSetProp(nodeOption,BAD_CAST "select", BAD_CAST "no");
#endif

    nodeOption = xmlNewChild(nodeProgram, NULL, BAD_CAST"option", NULL);
    xmlSetProp(nodeOption,BAD_CAST "name",BAD_CAST"libmagick");
    xmlSetProp(nodeOption,BAD_CAST "ns",BAD_CAST"http://www.imagemagick.org/");
#ifdef HAVE_LIBMAGICK
    xmlSetProp(nodeOption,BAD_CAST "select", BAD_CAST "yes");
    xmlSetProp(nodeOption,BAD_CAST "version",BAD_CAST MagickLibVersionText);
#else
    xmlSetProp(nodeOption,BAD_CAST "select", BAD_CAST "no");
#endif

    nodeOption = xmlNewChild(nodeProgram, NULL, BAD_CAST"option", NULL);
    xmlSetProp(nodeOption,BAD_CAST "name",BAD_CAST"libvorbis");
    xmlSetProp(nodeOption,BAD_CAST "ns",BAD_CAST"http://www.xiph.org/");
#ifdef HAVE_LIBVORBIS
    xmlSetProp(nodeOption,BAD_CAST "select", BAD_CAST "yes");
    xmlSetProp(nodeOption,BAD_CAST "version", BAD_CAST vorbis_version_string());
#else
    xmlSetProp(nodeOption,BAD_CAST "select", BAD_CAST "no");
#endif

    nodeOption = xmlNewChild(nodeProgram, NULL, BAD_CAST"option", NULL);
    xmlSetProp(nodeOption,BAD_CAST "name",BAD_CAST"vorbiscomment");
    xmlSetProp(nodeOption,BAD_CAST "ns",BAD_CAST"http://www.vorbis.com/");
#ifdef HAVE_LIBVORBIS
    xmlSetProp(nodeOption,BAD_CAST "select", BAD_CAST "yes");
#else
    xmlSetProp(nodeOption,BAD_CAST "select", BAD_CAST "no");
#endif

    nodeOption = xmlNewChild(nodeProgram, NULL, BAD_CAST"option", NULL);
    xmlSetProp(nodeOption,BAD_CAST "name",BAD_CAST"libid3tag");
    xmlSetProp(nodeOption,BAD_CAST "ns",BAD_CAST"http://www.underbit.com/products/mad/");
#ifdef HAVE_LIBID3TAG
    xmlSetProp(nodeOption,BAD_CAST "select", BAD_CAST "yes");
    xmlSetProp(nodeOption,BAD_CAST "version", BAD_CAST ID3_VERSION);
#else
    xmlSetProp(nodeOption,BAD_CAST "select", BAD_CAST "no");
#endif

    nodeOption = xmlNewChild(nodeProgram, NULL, BAD_CAST"option", NULL);
    xmlSetProp(nodeOption,BAD_CAST "name",BAD_CAST"threading");
#if WITH_THREAD
    xmlSetProp(nodeOption,BAD_CAST "select", BAD_CAST "yes");
#else
    xmlSetProp(nodeOption,BAD_CAST "select", BAD_CAST "no");
#endif

#ifdef HAVE_JS
    nodeOption = xmlNewChild(nodeProgram, NULL, BAD_CAST"option", NULL);
    scriptInfo(nodeOption);
#endif

    nodeOption = xmlNewChild(nodeProgram,NULL,BAD_CAST"module",NULL);
    xmlSetProp(nodeOption,BAD_CAST "name",BAD_CAST"cgi");
    xmlSetProp(nodeOption,BAD_CAST "ns",BAD_CAST"http://www.newbreedsoftware.com/cgi-util/");
#ifdef HAVE_CGI
    xmlSetProp(nodeOption,BAD_CAST "select",BAD_CAST "yes");
    xmlSetProp(nodeOption,BAD_CAST "version",BAD_CAST CGIUTILVER);
#else
    xmlSetProp(nodeOption,BAD_CAST "select",BAD_CAST "no");
#endif
}

  nodeRuntime = xmlNewChild(pndRoot, NULL, BAD_CAST"runtime", NULL);
  if (nodeRuntime) {
    xmlChar *pucT;
    xmlNodePtr nodeDate;
    resNodePtr prnTest;

    //domSetPropEat(nodeRuntime,BAD_CAST "platform",GetHostValueNamed(BAD_CAST "os"));
    /* working dir */
    domSetPropEat(nodeRuntime,BAD_CAST "cwd",resPathGetCwd());
    xmlSetProp(nodeRuntime, BAD_CAST "context", cxpCtxtLocationGetStr(pccArg));

    pucT = GetDateIsoString(0);
    nodeDate = xmlNewChild(nodeRuntime, NULL, BAD_CAST"date", pucT);
    xmlFree(pucT);

    /*! program arguments */
    for (i = 0; (pucArgv = cxpCtxtCliGetValue(pccArg, i)); i++) {
      xmlChar mpucIndex[BUFFER_LENGTH];
      xmlNodePtr pndArgv;
      pndArgv = xmlNewChild(nodeRuntime, NULL, BAD_CAST"arg", NULL);
      xmlStrPrintf(mpucIndex,BUFFER_LENGTH-1,"%i",i);
      xmlSetProp(pndArgv,BAD_CAST "name",mpucIndex);
      domSetPropEat(pndArgv,BAD_CAST "select",pucArgv);
    }

    /*! program arguments interpreted as named variables */
    for (i = 0; i < cxpCtxtCliGetCount(pccArg); i++) {
      xmlNodePtr pndArgv;
      if ((pucArgv = cxpCtxtCliGetName(pccArg, i))) {
	pndArgv = xmlNewChild(nodeRuntime, NULL, BAD_CAST"arg", NULL);
	xmlSetProp(pndArgv,BAD_CAST "name", pucArgv);
	domSetPropEat(pndArgv, BAD_CAST "select", cxpCtxtCliGetValueByName(pccArg, pucArgv));
	xmlFree(pucArgv);
      }
    }

#ifdef HAVE_CGI
    /*! CGI arguments */
    for (i = 0; (pucArgv = cxpCtxtCgiGetName(pccArg, i)); i++) {
      xmlNodePtr pndCgi;
      pndCgi = xmlNewChild(nodeRuntime, NULL, BAD_CAST"cgi", NULL);
      domSetPropEat(pndCgi,BAD_CAST "name",pucArgv);
      domSetPropEat(pndCgi, BAD_CAST "select", cxpCtxtCgiGetValue(pccArg, i));
    }
#endif

    /*! Environment */
    for (i = 0; (pucArgv = cxpCtxtEnvGetName(pccArg, i)); i++) {
      xmlNodePtr pndEnv;
      pndEnv = xmlNewChild(nodeRuntime, NULL, NAME_ENV, NULL);
      domSetPropEat(pndEnv,BAD_CAST "name",pucArgv);
      domSetPropEat(pndEnv,BAD_CAST "select",cxpCtxtEnvGetValue(pccArg,i));
    }

    /*! Search path directories */
    for (prnTest = cxpCtxtSearchGet(pccArg); prnTest; prnTest = resNodeGetNext(prnTest)) {
      xmlNodePtr pndSearch;
      pndSearch = xmlNewChild(nodeRuntime, NULL, BAD_CAST "search", NULL);
      xmlSetProp(pndSearch,BAD_CAST "select",resNodeGetNameNormalized(prnTest));
      xmlSetProp(pndSearch,BAD_CAST "recursive", BAD_CAST (resNodeIsRecursive(prnTest) ? "yes" : "no"));
    }
  }

  return pdocResult;
}
/* end of cxpProcessInfoNode() */


#ifndef HAVE_PIE

/*! process the PIE child instructions of pndArgPie (!!! MINIMUM if pie module was excluded !!!)

\param pndArgPie pie tree to process
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
  xmlChar *pucAttr;

#ifdef DEBUG
  cxpCtxtLogPrint(pccArg, 2, "pieProcessPieNode(pndArg=%0x,pccArg=%0x)", pndArgPie, pccArg);
#endif

  if (IS_NODE_PIE(pndArgPie) && pndArgPie->children != NULL) {
    xmlNodePtr pndMakePieCopy;
    xmlNodePtr pndPiePre;

    pdocResult = xmlNewDoc(BAD_CAST "1.0");
    pdocResult->encoding = xmlStrdup(BAD_CAST "UTF-8");

    pndPieRoot = xmlNewNode(NULL,NAME_PIE_PIE);
    xmlSetTreeDoc(pndPieRoot,pdocResult);
    xmlDocSetRootElement(pdocResult, pndPieRoot);
      
    pndPiePre = xmlCopyNode(pndArgPie, 1); /* first copy for import processing */
    xmlNodeSetName(pndPiePre,NAME_PIE_PRE);
    xmlSetTreeDoc(pndPiePre, pdocResult);
    xmlAddChild(pndPieRoot, pndPiePre);
    
    pndMeta = xmlNewChild(pndPieRoot,NULL,NAME_PIE_META,NULL);
    /* Get the current time. */
    domSetPropEat(pndMeta, BAD_CAST "ctime", GetNowFormatStr(BAD_CAST "%s"));
    domSetPropEat(pndMeta, BAD_CAST "ctime2", GetDateIsoString(0));
    xmlAddChild(pndMeta, xmlNewPI(BAD_CAST"cxproc:", BAD_CAST"no PIE available"));
    cxpInfoProgram(pndMeta, pccArg);
  }
  else {
    /* no pie make instructions */
    //xmlSetProp(pndPieRoot, BAD_CAST "class", BAD_CAST "empty");
  }
  return pdocResult;
}
/* end of pieProcessPieNode() */

#endif

/*! exit procedure for this module
*/
void
cxpCleanup(void)
{
  xmlFreeNs(pnsCxp);
} /* end of cxpCleanup() */


#ifdef TESTCODE
#include "test/test_cxp.c"
#endif

