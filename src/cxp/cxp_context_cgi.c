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
draft-thomson-hybi-http-timeout-01 <https://tools.ietf.org/id/draft-thomson-hybi-http-timeout-01.html#rfc.section.2.1>
    Keep-Alive: timeout=600
    Connection: Keep-Alive

proper-use-of-keepalive-in-apache-htaccess <https://stackoverflow.com/questions/12095533/proper-use-of-keepalive-in-apache-htaccess>
    Apache:
    KeepAliveTimeout 1

RFC3875 CGI 1.1 <http://tools.ietf.org/html/rfc3875#page-10>
    "AUTH_TYPE"
    "CONTENT_LENGTH"
    "CONTENT_TYPE"
    "GATEWAY_INTERFACE"
    "PATH_INFO"
    "PATH_TRANSLATED"
    "QUERY_STRING"
    "REMOTE_ADDR"
    "REMOTE_HOST"
    "REMOTE_IDENT"
    "REMOTE_USER"
    "REQUEST_METHOD"
    "SCRIPT_NAME"
    "SERVER_NAME"
    "SERVER_PORT"
    "SERVER_PROTOCOL"
    "SERVER_SOFTWARE"
    scheme
    protocol-var-name
    extension-var-name
*/

/*
*/
#include <libxml/xmlversion.h>
#include <libxml/HTMLtree.h>
#include <libxml/parser.h>
#include <libxml/uri.h>

#ifdef LIBXML_THREAD_ENABLED
#include <libxml/globals.h>
#include <libxml/threads.h>
#ifdef HAVE_LIBPTHREAD
#include <pthread.h>
#elif defined(WITH_THREAD)
#endif
#endif

/*
*/
#include "basics.h"
#include "utils.h"
#include <res_node/res_node_ops.h>
#include <cxp/cxp.h>
#include <cxp/cxp_context_cgi.h>
#include <cxp/cxp_dir.h>
#include "dom.h"

#ifdef HAVE_PIE
#include <pie/pie_text.h>
#include <cxp/cxp_calendar.h>
#include <petrinet/petrinet.h>
#endif
#ifdef HAVE_LIBSQLITE3
#include <database/database.h>
#endif

#ifdef HAVE_JS
#include <script/script.h>
#endif
#ifdef HAVE_LIBARCHIVE
#include <archive/cxp_archive.h>
#endif


/*! creates a new cxproc Context for a CGI request

\param argc -- count of program arguments
\param **argv -- pointer to array of program arguments
\param **envp -- pointer to NULL terminated array of program environment
\return pointer to new allocated context
*/
cxpContextPtr
cxpCtxtCgiNew(int argc, char *argv[], char *envp[])
{
  xmlChar* pucT = NULL;
  cxpContextPtr pccResult = NULL;

  pccResult = cxpCtxtNew();
  if (pccResult) {
    xmlChar* pucPathValue = NULL;
    resNodePtr prnT = NULL;

    pccResult->iCountArgv = argc;
    pccResult->ppcArgv = argv;

    cxpCtxtEnvDup(pccResult,envp);

    cxpCtxtSetReadonly(pccResult, cxpCtxtEnvGetBoolByName(pccResult, BAD_CAST "CXP_READONLY", FALSE));

    pucT = cxpCtxtEnvGetValueByName(pccResult, BAD_CAST "CXP_LOG");
    if (cxpCtxtLogSetLevelStr(pccResult, pucT) == 0) {
      /* level value from environment */
    }
    else {
#if defined(DEBUG) || defined(TESTCODE)
      cxpCtxtLogSetLevel(pccResult, LEVEL_MAX);
#else
      cxpCtxtLogSetLevel(pccResult, 2);
#endif
    }
    xmlFree(pucT);

    cxpCtxtLogPrint(pccResult,1,"Based on %s %s",CXP_VER_FILE_VERSION_STR,CXP_VER_FILE_BRANCH_STR);

    if (cxpCtxtRootSet(pccResult, NULL)) {
      cxpCtxtLocationSet(pccResult, cxpCtxtRootGet(pccResult));

      if ((pucPathValue = cxpCtxtEnvGetValueByName(pccResult, BAD_CAST "CXP_PATH")) != NULL) {
	cxpCtxtLogPrint(pccResult, 2, "Use value '%s' of environment variable 'CXP_PATH'", pucPathValue);
	if ((prnT = resNodeStrNew(pucPathValue))) {
	  cxpCtxtSearchSet(pccResult, prnT);
	  resNodeListFree(prnT);
	}
	xmlFree(pucPathValue);
      }
    }
    else {
      cxpCtxtFree(pccResult);
      pccResult = NULL;
    }
  }
  return pccResult;
} /* end of cxpCtxtCgiNew() */


/*! creates a new cxproc Context for a new CGI request

\param pccArg -- pointer to context
\return a config DOM according to program arguments
*/
xmlDocPtr
cxpCtxtCgiParse(cxpContextPtr pccArg)
{
  int iMimeType = 0;
  resNodePtr prnExecutable;
  resNodePtr prnFile = NULL;
  resNodePtr prnDir = NULL;
  resNodePtr prnFound = NULL;
  resNodePtr prnContent = NULL;
  resNodePtr prnPathTranslated = NULL;
  xmlChar mpucNameFile[BUFFER_LENGTH];
  xmlChar *pucCgiCxp = NULL;
  xmlChar *pucCgiEncoding = NULL;
  xmlChar *pucCgiPath = NULL;
  xmlChar *pucCgiYear = NULL;
  xmlChar *pucCgiPathTranslated = NULL;
  xmlChar *pucCgiXpath = NULL;
  xmlChar *pucCgiXsl = NULL;
  xmlChar *pucT;
  xmlNodePtr pndMake = NULL;
  xmlNodePtr pndPlain = NULL;
  xmlNodePtr pndXml = NULL;
  xmlNsPtr pnsCxp;

  mpucNameFile[0] = (xmlChar)'\0';

  pucT = cxpCtxtCliGetValue(pccArg,0);
  prnExecutable = resNodeDirNew(pucT);
  xmlFree(pucT);

  cxpCtxtLogPrint(pccArg, 1, "Its me '%s' %s", resNodeGetNameNormalized(prnExecutable), CXP_VER_FILE_VERSION_STR);
  resNodeFree(prnExecutable);

  pccArg->pdocContextNode = xmlNewDoc(BAD_CAST "1.0");
  pndMake = xmlNewDocNode(pccArg->pdocContextNode, NULL, NAME_MAKE, NULL);
  xmlDocSetRootElement(pccArg->pdocContextNode, pndMake);
  pnsCxp = xmlNewNs(pndMake, BAD_CAST CXP_VER_URL, BAD_CAST "cxp");
  xmlSetNs(pndMake, pnsCxp);

  /*
  detect all CGI variables
  */
  pucCgiPathTranslated = cxpCtxtEnvGetValueByName(pccArg, BAD_CAST"PATH_TRANSLATED");
  if (pucCgiPathTranslated) {
    prnPathTranslated = resNodeRootNew(cxpCtxtRootGet(pccArg),pucCgiPathTranslated);
    if (resNodeIsReadable(prnPathTranslated)) {
      cxpCtxtLogPrint(pccArg, 1, "Use value of CGI 'PATH_TRANSLATED' '%s'", pucCgiPathTranslated);
    }
    else {
      resNodeFree(prnPathTranslated);
      prnPathTranslated = NULL;
    }
  }

  pucCgiCxp = cxpCtxtCgiGetValueByName(pccArg,BAD_CAST"cxp");
  if (pucCgiCxp == NULL && resNodeGetMimeType(prnPathTranslated) == MIME_APPLICATION_CXP_XML) {
    /* in this case use the translated path */
    pucCgiCxp = xmlStrdup(pucCgiPathTranslated);
    xmlStrPrintf(mpucNameFile, BUFFER_LENGTH, "%s", (char *)pucCgiCxp);
  }

  pucCgiEncoding = cxpCtxtCgiGetValueByName(pccArg,BAD_CAST"encoding");

  pucCgiXsl = cxpCtxtCgiGetValueByName(pccArg,BAD_CAST"xsl");

  pucCgiXpath = cxpCtxtCgiGetValueByName(pccArg,BAD_CAST"xpath");

  if ((pucCgiPath = cxpCtxtCgiGetValueByName(pccArg, BAD_CAST"search")) != NULL) {
    /*!
      search for this file name in CXP_ROOT
    */
    if ((prnFile = resNodeRootNew(cxpCtxtRootGet(pccArg), pucCgiPath)) != NULL && resNodeReadStatus(prnFile) && resNodeIsFile(prnFile)) {
    }
    else if (resNodeReadStatus(prnFile) && resNodeIsDir(prnFile)) {
      /* ignoring searches for directories */
      resNodeFree(prnFile);
      prnFile = NULL;
    }
    else if ((prnFile = resNodeListFindPath(cxpCtxtRootGet(pccArg), pucCgiPath, (RN_FIND_FILE | RN_FIND_IN_SUBDIR))) != NULL) {
      prnFile = resNodeDup(prnFile, RN_DUP_THIS);
    }
    else if ((prnFile = cxpResNodeResolveNew(pccArg, NULL, pucCgiPath, (CXP_O_FILE | CXP_O_READ | CXP_O_SEARCH))) != NULL) {
    }
    else {
    }
  }
  else if ((pucCgiPath = cxpCtxtCgiGetValueByName(pccArg, BAD_CAST"spath")) != NULL) {
    /*!
      search for this file name in CXP_PATH
    */
    prnFound = cxpCtxtSearchFind(pccArg, pucCgiPath);
    if (resNodeReadStatus(prnFound) == FALSE) {
      cxpCtxtLogPrint(pccArg, 1, "File '%s' not found", pucCgiPath);
      resNodeFree(prnFound);
      prnFound = NULL;
    }
  }
  else if ((pucCgiPath = cxpCtxtCgiGetValueByName(pccArg, BAD_CAST"path")) != NULL
    || (pucCgiPath = cxpCtxtCgiGetValueByName(pccArg, BAD_CAST"dir")) != NULL
    || (pucCgiPath = cxpCtxtCgiGetValueByName(pccArg, BAD_CAST"file")) != NULL) {
    /*!
      map pucCgiPath either to pucCgiDir OR pucCgiFile
    */
    resNodePtr prnTest = NULL;

    if ((prnTest = resNodeRootNew(cxpCtxtRootGet(pccArg), pucCgiPath)) == NULL) {
    }
    else if (resNodeReadStatus(prnTest) && resNodeIsDir(prnTest)) {
      prnDir = prnTest;
    }
    else if (resNodeReadStatus(prnTest) && resNodeIsFile(prnTest)) {
      prnFile = prnTest;
    }
    else {
      resNodeFree(prnTest);
    }
  }
  else if ((pucCgiYear = cxpCtxtCgiGetValueByName(pccArg, BAD_CAST"year")) != NULL) {
    /*! output of XML calendar */
  }

  if (prnFile) {
    prnContent = resNodeGetLastDescendant(prnFile);
    iMimeType = resNodeGetMimeType(prnContent);
  }

#ifdef HAVE_CGI
  if (pucCgiCxp != NULL && xmlStrlen(pucCgiCxp) > 0) {
    /*
    for security reasons add prefix "Cgi" to filename
    */

    if (xmlStrlen(mpucNameFile) > 0) {
      /* the CGI translated path is used */
    }
    else {
      xmlStrPrintf(mpucNameFile, BUFFER_LENGTH, "Cgi%s.cxp", pucCgiCxp);
    }

    if (pucCgiXsl) {
      xmlFree(pucCgiXsl);
      pucCgiXsl = NULL;
    }
  }
  else if (pucCgiXsl != NULL && xmlStrlen(pucCgiXsl) > 0) {
    /*!\todo for security add prefix "Cgi" to XSL filename */
    xmlStrPrintf(mpucNameFile, BUFFER_LENGTH, "%s.xsl", pucCgiXsl);
    if (pucCgiCxp) {
      xmlFree(pucCgiCxp);
      pucCgiCxp = NULL;
    }
  }
#else
#ifdef DEBUG
  xmlStrPrintf(mpucNameFile, BUFFER_LENGTH, "Cgiinfo.cxp");
#else
  xmlStrPrintf(mpucNameFile, BUFFER_LENGTH, "CgiPieUiPowered.cxp");
#endif
  pucCgiCxp = xmlStrdup(mpucNameFile);
  if (pucCgiXsl) {
    xmlFree(pucCgiXsl);
    pucCgiXsl = NULL;
  }
#endif

  if (STR_IS_NOT_EMPTY(pucCgiYear)) {
    xmlNodePtr pndCalendar;
    xmlNodePtr pndT;

    pndXml = xmlNewChild(pndMake, NULL, NAME_XML, NULL);
    xmlSetProp(pndXml, BAD_CAST "name", BAD_CAST "-");
    pndCalendar = xmlNewChild(pndXml, NULL, NAME_CALENDAR, NULL);
    domSetPropEat(pndCalendar, BAD_CAST "year", pucCgiYear);
    //cxpCtxtCliAddXsl(pndXml, pccArg);
  }
  else if (prnPathTranslated) {
    /* deliver the file content via CXP configuration */
    pndXml = xmlNewChild(pndMake, NULL, NAME_XML, NULL);
    xmlSetProp(pndXml, BAD_CAST "name", resNodeGetNameNormalized(prnPathTranslated));
    xmlSetProp(pndXml, BAD_CAST "schema", BAD_CAST "cxp.rng");
    xmlSetProp(pndXml, BAD_CAST "eval", BAD_CAST "yes");
    xmlSetProp(pndMake, BAD_CAST "dir", resNodeGetNameBaseDir(prnPathTranslated));
  }
  else if (prnFound) {
    /* deliver the plain file content via CXP configuration */
    pndPlain = xmlNewChild(pndMake, NULL, NAME_FILECOPY, NULL);
    xmlSetProp(pndPlain, BAD_CAST "from", resNodeGetNameNormalized(prnFound));
    xmlSetProp(pndPlain, BAD_CAST "to", BAD_CAST"-");
  }
  else if (pucCgiCxp) {
    /* deliver the file content via CXP configuration */
    resNodePtr prnCgiCxp;

    if ((prnCgiCxp = cxpResNodeResolveNew(pccArg, NULL, mpucNameFile, CXP_O_SEARCH | CXP_O_READ)) != NULL) {
      /* found */
      pndXml = xmlNewChild(pndMake, NULL, NAME_XML, NULL);
      xmlSetProp(pndXml, BAD_CAST "name", resNodeGetNameNormalized(prnCgiCxp));
      //xmlSetProp(pndXml, BAD_CAST "schema", BAD_CAST "cxp.rng");
      xmlSetProp(pndXml, BAD_CAST "eval", BAD_CAST "yes");
      resNodeFree(prnCgiCxp);
    }
    else {
      pndXml = xmlNewChild(pndMake, NULL, NAME_PLAIN, BAD_CAST"File not found");
      xmlSetProp(pndXml, BAD_CAST "name", BAD_CAST"-");
    }
  }
  else if (pucCgiXsl) {
    /*
    deliver the file content with XSL transformation
    */
    xmlNodePtr pndXsl;
    xmlNodePtr pndFile;
    xmlNodePtr pndDir;
    resNodePtr prnCgiXsl;

    /* detect output type of XSL */

    if ((prnCgiXsl = cxpResNodeResolveNew(pccArg, NULL, mpucNameFile, CXP_O_READ | CXP_O_SEARCH)) != NULL) {
      xmlDocPtr pdocXsl;

      pdocXsl = resNodeReadDoc(prnCgiXsl);
      if (pdocXsl) {
	xmlNodePtr pndRoot = xmlDocGetRootElement(pdocXsl);
	xmlNodePtr pndOutput = domGetFirstChild(pndRoot, BAD_CAST "output");
	xmlChar *pucAttrMethod = domGetPropValuePtr(pndOutput, BAD_CAST "method");

	if (xmlStrEqual(pucAttrMethod, BAD_CAST "html")) {
	  pndOutput = xmlNewChild(pndMake, NULL, NAME_XHTML, NULL);
	}
	else if (xmlStrEqual(pucAttrMethod, BAD_CAST "text")) {
	  pndOutput = xmlNewChild(pndMake, NULL, NAME_PLAIN, NULL);
	}
	else {
	  pndOutput = xmlNewChild(pndMake, NULL, NAME_XML, NULL);
	}

	xmlSetProp(pndOutput, BAD_CAST "name", BAD_CAST "-");
	pndXml = xmlNewChild(pndOutput, NULL, NAME_XML, NULL);
	if (prnFile) {
	  if (iMimeType == MIME_TEXT_PLAIN) {
#ifdef HAVE_PIE
	    pndFile = xmlNewChild(pndXml, NULL, NAME_PIE, NULL);
	    pndFile = xmlNewChild(pndFile, NULL, NAME_PIE_IMPORT, NULL);
	    xmlSetProp(pndFile, BAD_CAST "name", resNodeGetNameNormalized(prnContent));
#endif
	  }
	  else if (iMimeType == MIME_TEXT_HTML) {
	    pndFile = xmlNewChild(pndXml, NULL, NAME_XHTML, NULL);
	    xmlSetProp(pndFile, BAD_CAST "name", resNodeGetNameNormalized(prnContent));
	  }
	  else {
	    pndFile = xmlNewChild(pndXml, NULL, NAME_XML, NULL);
	    xmlSetProp(pndFile, BAD_CAST "name", resNodeGetNameNormalized(prnContent));
	  }
	}
	else if (prnDir) {
	  pndDir = xmlNewChild(pndXml, NULL, NAME_DIR, NULL);
	  xmlSetProp(pndDir, BAD_CAST "name", resNodeGetNameNormalized(prnDir));
	  xmlSetProp(pndDir, BAD_CAST "verbosity", BAD_CAST "3");
	  xmlSetProp(pndDir, BAD_CAST "depth", BAD_CAST "1");
	  xmlSetProp(pndDir, BAD_CAST "urlencode", BAD_CAST "yes");
	}
	else {
	  /* this results in a empty PIE DOM */
	  xmlNewChild(pndXml, NULL, NAME_PIE, NULL);
	}
	pndXsl = xmlNewChild(pndOutput, NULL, NAME_XSL, NULL);
	xmlSetProp(pndXsl, BAD_CAST "name", resNodeGetNameNormalized(prnCgiXsl));

	cxpCtxtCacheAppendDoc(pccArg, pdocXsl, resNodeGetNameNormalized(prnCgiXsl));
	xmlFreeDoc(pdocXsl);
      }
    }
    else {
      cxpCtxtLogPrint(pccArg, 1, "Stylesheet '%s' not found in current search DOM", mpucNameFile);
      pndPlain = xmlNewChild(pndMake, NULL, NAME_PLAIN, BAD_CAST "\n\n\nUnknown XML Stylesheet!");
      xmlSetProp(pndPlain, BAD_CAST "name", BAD_CAST "-");
    }
  }
  else if (prnDir) {
    if (cxpCtxtAccessIsPermitted(pccArg,prnDir) == FALSE) {
      // access error
    }
    else {
      /*
      deliver the directory XML listing
      */
      xmlNodePtr pndDir;

      pndXml = xmlNewChild(pndMake, NULL, NAME_XML, NULL);
      xmlSetProp(pndXml, BAD_CAST "name", BAD_CAST "-");
      pndDir = xmlNewChild(pndXml, NULL, NAME_DIR, NULL);
      xmlSetProp(pndDir, BAD_CAST "verbosity", BAD_CAST "3");
      xmlSetProp(pndDir, BAD_CAST "depth", BAD_CAST (resNodeIsRecursive(prnDir) ? "99" : "1"));
      xmlSetProp(pndDir, BAD_CAST "urlencode", BAD_CAST "yes");
      pndDir = xmlNewChild(pndDir, NULL, NAME_DIR, NULL);
      xmlSetProp(pndDir, BAD_CAST "name", resNodeGetNameNormalized(prnDir));
    }
  }
  else if (prnFile) {
    if (cxpCtxtAccessIsPermitted(pccArg,prnFile) == FALSE) {
      // access error
    }
    else {
      if (pucCgiXpath) {
	/*
	deliver the XML file content of Xpath
	*/
	xmlNodePtr pndXsl;
	xmlNodePtr pndImport;

	pndXml = xmlNewChild(pndMake, NULL, NAME_XML, NULL);
	xmlSetProp(pndXml, BAD_CAST "name", BAD_CAST "-");
	xmlSetProp(pndXml, BAD_CAST "encoding", pucCgiEncoding);

	if (iMimeType == MIME_TEXT_PLAIN) {
	  xmlNodePtr pndPie;

#ifdef HAVE_PIE
	  pndPie = xmlNewChild(pndXml, NULL, NAME_PIE, NULL);
	  pndImport = xmlNewChild(pndPie, NULL, NAME_PIE_IMPORT, NULL);
	  xmlSetProp(pndImport, BAD_CAST "locator", BAD_CAST "yes");
	  xmlSetProp(pndImport, BAD_CAST "name", resNodeGetNameNormalized(prnFile));
#endif
	}
	else if (iMimeType == MIME_TEXT_HTML) {
	  xmlNodePtr pndXhtml;

	  pndXhtml = xmlNewChild(pndXml, NULL, NAME_XHTML, NULL);
	  xmlSetProp(pndXhtml, BAD_CAST "name", resNodeGetNameNormalized(prnFile));
	}
	else {
	  xmlNodePtr pndXmlChild;

	  pndXmlChild = xmlNewChild(pndXml, NULL, NAME_XML, NULL);
	  xmlSetProp(pndXmlChild, BAD_CAST "name", resNodeGetNameNormalized(prnFile));
	}
	pndXsl = xmlNewChild(pndXml, NULL, NAME_XSL, NULL);
	xmlSetProp(pndXsl, BAD_CAST "xpath", pucCgiXpath);
      }
#ifdef HAVE_JS
      else if (iMimeType == MIME_APPLICATION_X_JAVASCRIPT) {
	/* path is a name of a script */
	xmlNodePtr pndScript;

	pndPlain = xmlNewChild(pndMake, NULL, NAME_PLAIN, NULL);
	xmlSetProp(pndPlain, BAD_CAST "name", BAD_CAST"-");
	xmlSetProp(pndPlain, BAD_CAST "encoding", pucCgiEncoding);
	pndScript = xmlNewChild(pndPlain, NULL, NAME_SCRIPT, NULL);

	if (resNodeIsFile(prnFile)) {
	  xmlSetProp(pndScript, BAD_CAST "name", pucCgiPath);
	}
	else {
	  xmlSetProp(pndScript, BAD_CAST "name", pucCgiPath);
	  xmlSetProp(pndScript, BAD_CAST "search", BAD_CAST "yes");
	  cxpCtxtLogPrint(pccArg, 2, "Search later for '%s'", pucCgiPath);
	}
	/*!\todo append additional arguments */
      }
#endif
      else {
#ifdef EXPERIMENTAL
	/*
	deliver the file XML
	*/
	xmlNodePtr pndFile;

	pndXml = xmlNewChild(pndMake, NULL, NAME_XML, NULL);
	xmlSetProp(pndXml, BAD_CAST "name", BAD_CAST "-");
	pndFile = xmlNewChild(pndXml, NULL, NAME_FILE, NULL);
	xmlSetProp(pndFile, BAD_CAST "verbosity", BAD_CAST "4");
	//xmlSetProp(pndFile, BAD_CAST "search", BAD_CAST "yes");
	xmlSetProp(pndFile, BAD_CAST "name", resNodeGetNameNormalized(prnFile));
#else
	/*
	deliver the file content without CXP or XSL
	*/
	xmlNodePtr pndCopy;
	pndCopy = xmlNewChild(pndMake, NULL, NAME_FILECOPY, NULL);
	if (resNodeIsFileInArchive(prnContent)) {
	  xmlSetProp(pndCopy, BAD_CAST "from", resNodeGetNameNormalized(prnContent));
	}
	else {
	  xmlSetProp(pndCopy, BAD_CAST "from", resNodeGetNameNormalized(prnFile));
	}
	xmlSetProp(pndCopy, BAD_CAST "to", BAD_CAST "-");
	xmlSetProp(pndCopy, BAD_CAST "encoding", pucCgiEncoding);
#endif
      }
    }
  }
  else {
#if 1
    pndXml = xmlNewChild(pndMake, NULL, NAME_XML, NULL);
    xmlSetProp(pndXml, BAD_CAST "name", BAD_CAST "-");
    xmlNewChild(pndXml, NULL, BAD_CAST "info", NULL);
#else
    /*
    show a plain error message only
    */
    pndPlain = xmlNewChild(pndMake, NULL, NAME_PLAIN, BAD_CAST "\n\n\nThe requested action ist not yet supported!");
    xmlSetProp(pndPlain, BAD_CAST "name", BAD_CAST "-");
#endif
  }

  /*
  release the allocated resNode's
  */
  resNodeFree(prnPathTranslated);
  resNodeFree(prnFound);
  resNodeFree(prnDir);
  resNodeFree(prnFile);
  /*
  release the allocated CGI values
  */
  xmlFree(pucCgiCxp);
  xmlFree(pucCgiXsl);
  xmlFree(pucCgiXpath);
  xmlFree(pucCgiPathTranslated);
  xmlFree(pucCgiPath);

  return pccArg->pdocContextNode;
  //return pcxpContextResult->;
} /* end of cxpCtxtCgiParse() */


/*! cxp Ctxt Cgi Process

\param pccArg -- pointer to context
\return TRUE if , FALSE in case of
*/
BOOL_T
cxpCtxtCgiProcess(cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;
  xmlNodePtr pndRoot;

  if (pccArg != NULL 
    && pccArg->pdocContextNode != NULL
    && (pndRoot = xmlDocGetRootElement(pccArg->pdocContextNode)) != NULL) {
      cxpProcessMakeNode(pndRoot, pccArg);
      fResult = TRUE;
  }
  return fResult;
} /* end of cxpCtxtCgiProcess() */


#ifdef TESTCODE
#include "test/test_cxp_context_cgi.c"
#endif
