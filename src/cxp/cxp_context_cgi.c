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
  resNodePtr prnExecutable;
  xmlChar mpucNameFile[BUFFER_LENGTH];
  xmlChar *pucCgiCxp = NULL;
  xmlChar *pucCgiRedir = NULL;
  xmlChar *pucCgiPathTranslated = NULL;
  xmlChar *pucT;
  xmlChar *pucCgiPath = NULL;
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
  if ((pucCgiPathTranslated = cxpCtxtEnvGetValueByName(pccArg, BAD_CAST"PATH_TRANSLATED")) != NULL) {
    resNodePtr prnPathTranslated = NULL;

    prnPathTranslated = resNodeRootNew(cxpCtxtRootGet(pccArg),pucCgiPathTranslated);
    if (resNodeIsReadable(prnPathTranslated) && resNodeGetMimeType(prnPathTranslated) == MIME_APPLICATION_CXP_XML) {
      cxpCtxtLogPrint(pccArg, 1, "Use value of CGI 'PATH_TRANSLATED' '%s'", pucCgiPathTranslated);
      /* in this case use the translated path */
      
      pndXml = xmlNewChild(pndMake,pnsCxp,NAME_XML,NULL);
      xmlSetProp(pndXml, BAD_CAST "name", BAD_CAST "-");

      /* deliver the file content via CXP configuration */
      xmlSetProp(pndXml, BAD_CAST "name", resNodeGetNameNormalized(prnPathTranslated));
      xmlSetProp(pndXml, BAD_CAST "schema", BAD_CAST "cxp.rng");
      xmlSetProp(pndXml, BAD_CAST "eval", BAD_CAST "yes");
      xmlSetProp(pndMake, BAD_CAST "dir", resNodeGetNameBaseDir(prnPathTranslated));
    }
    else {
    }
    resNodeFree(prnPathTranslated);
  }
  else if ((pucCgiCxp = cxpCtxtCgiGetValueByName(pccArg,BAD_CAST"cxp")) != NULL && STR_IS_NOT_EMPTY(pucCgiCxp)) {
    /* deliver the file content via CXP configuration */
    resNodePtr prnCgiCxp;

    pndXml = xmlNewChild(pndMake, pnsCxp, NAME_XML, NULL);

    /* for security reasons add prefix "Cgi" to filename */
    xmlStrPrintf(mpucNameFile, BUFFER_LENGTH, "Cgi%s.cxp", pucCgiCxp);

    if ((prnCgiCxp = cxpResNodeResolveNew(pccArg, NULL, mpucNameFile, CXP_O_SEARCH | CXP_O_READ)) != NULL) {
      /* found */
      xmlSetProp(pndXml, BAD_CAST "name", resNodeGetNameNormalized(prnCgiCxp));
      //xmlSetProp(pndXml, BAD_CAST "schema", BAD_CAST "cxp.rng");
      xmlSetProp(pndXml, BAD_CAST "eval", BAD_CAST "yes");
      resNodeFree(prnCgiCxp);
    }
    else {
      //pndXml = xmlNewChild(pndMake, NULL, NAME_PLAIN, BAD_CAST"File not found");
    }
  }
  else if ((pucCgiRedir = cxpCtxtCgiGetValueByName(pccArg, BAD_CAST"redir")) != NULL) {
    /*!
      search for this file name in CXP_ROOT using 'pucCgiRedir' as regexp and redirect client to this URI
    */
    resNodePtr prnTest = NULL;

    while ((prnTest = resNodeListFindPath(cxpCtxtRootGet(pccArg), pucCgiRedir, (RN_FIND_FILE | RN_FIND_IN_SUBDIR | RN_FIND_REGEXP))) != NULL) {
      xmlChar *pucRedir = NULL;

      if (cxpCtxtAccessIsPermitted(pccArg,prnTest) == FALSE) {
	// access error, continue
      }
      else if ((pucRedir = resNodeGetNameRelative(cxpCtxtLocationGet(pccArg),prnTest)) != NULL && STR_IS_NOT_EMPTY(pucRedir)) {
	xmlStrPrintf(mpucNameFile, BUFFER_LENGTH, "Location: /%s\r\n\r\n", (char *)pucRedir);
	pndPlain = xmlNewChild(pndMake, NULL, NAME_PLAIN, mpucNameFile);
	xmlSetProp(pndPlain, BAD_CAST "name", BAD_CAST "-");
	xmlSetProp(pndPlain, BAD_CAST "status", BAD_CAST "302 Found");
	break;
      }
      xmlFree(pucRedir);
    }
    resNodeFree(prnTest);
  }
  else if ((pucCgiPath = cxpCtxtCgiGetValueByName(pccArg, BAD_CAST"spath")) != NULL) {
    /*!
      search for this file name in CXP_PATH
    */
    resNodePtr prnFound = NULL;

    prnFound = cxpCtxtSearchFind(pccArg, pucCgiPath);
    if (resNodeReadStatus(prnFound)) {
      /* deliver the file content via CXP configuration */
      pndPlain = xmlNewChild(pndMake, NULL, NAME_FILECOPY, NULL);
      xmlSetProp(pndPlain, BAD_CAST "from", resNodeGetNameNormalized(prnFound));
      xmlSetProp(pndPlain, BAD_CAST "to", BAD_CAST"-");
    }
    else {
      cxpCtxtLogPrint(pccArg, 1, "File '%s' not found", pucCgiPath);
    }
    resNodeFree(prnFound);
  }
  else {
    xmlChar *pucCgiEncoding = NULL;
    xmlChar *pucCgiXsl = NULL;
    xmlChar *pucCgiXpath = NULL;
    xmlChar *pucCgiYear = NULL;
    xmlNodePtr pndSource = NULL;

    pucCgiEncoding = cxpCtxtCgiGetValueByName(pccArg,BAD_CAST"encoding");
    pucCgiXpath = cxpCtxtCgiGetValueByName(pccArg,BAD_CAST"xpath");
    pucCgiXsl = cxpCtxtCgiGetValueByName(pccArg,BAD_CAST"xsl");

    pndXml = xmlNewNode(pnsCxp, NAME_XML);

    if ((pucCgiPath = cxpCtxtCgiGetValueByName(pccArg, BAD_CAST"path")) != NULL
	|| (pucCgiPath = cxpCtxtCgiGetValueByName(pccArg, BAD_CAST"dir")) != NULL
	|| (pucCgiPath = cxpCtxtCgiGetValueByName(pccArg, BAD_CAST"file")) != NULL) {
      /*!
	map pucCgiPath either to pucCgiDir OR pucCgiFile
      */
      resNodePtr prnTest = NULL;

      if ((prnTest = resNodeRootNew(cxpCtxtRootGet(pccArg), pucCgiPath)) == NULL || resNodeIsReadable(prnTest) == FALSE) {
	prnTest = resNodeListFindPath(cxpCtxtRootGet(pccArg), pucCgiPath, (RN_FIND_FILE | RN_FIND_IN_SUBDIR | RN_FIND_REGEXP));
      }
	  
      if (resNodeReadStatus(prnTest) && resNodeIsDir(prnTest)) {
	if (cxpCtxtAccessIsPermitted(pccArg,prnTest) == FALSE) {
	  // access error
	}
	else {
	  /*
	    deliver the directory XML listing
	  */
	  xmlNodePtr pndDir;

	  pndDir = xmlNewChild(pndXml, NULL, NAME_DIR, NULL);
	  xmlSetProp(pndDir, BAD_CAST "verbosity", BAD_CAST "3");
	  xmlSetProp(pndDir, BAD_CAST "depth", BAD_CAST (resNodeIsRecursive(prnTest) ? "99" : "1"));
	  xmlSetProp(pndDir, BAD_CAST "urlencode", BAD_CAST "yes");
	  pndDir = xmlNewChild(pndDir, NULL, NAME_DIR, NULL);
	  xmlSetProp(pndDir, BAD_CAST "name", resNodeGetNameRelative(cxpCtxtRootGet(pccArg),prnTest));
	}
      }
      else if (resNodeReadStatus(prnTest) && resNodeIsFile(prnTest)) {
	resNodePtr prnContent = NULL;
	RN_MIME_TYPE iMimeType = 0;

	prnContent = resNodeGetLastDescendant(prnTest);
	iMimeType = resNodeGetMimeType(prnContent);

	if (cxpCtxtAccessIsPermitted(pccArg,prnTest) == FALSE) {
	  // access error
	}
#ifdef HAVE_JS
	else if (iMimeType == MIME_APPLICATION_X_JAVASCRIPT) {
	  /* path is a name of a script */
	  xmlNodePtr pndScript;

	  pndScript = xmlNewChild(pndXml, NULL, NAME_SCRIPT, NULL);
	  xmlNodeSetName(pndXml, NAME_PLAIN);
	  if (STR_IS_NOT_EMPTY(pucCgiEncoding)) {
	    xmlSetProp(pndXml, BAD_CAST "encoding", pucCgiEncoding);
	  }

	  if (resNodeIsFile(prnTest)) {
	    xmlSetProp(pndScript, BAD_CAST "name", resNodeGetNameRelative(cxpCtxtRootGet(pccArg),prnTest));
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
	  /*
	    deliver the file XML
	  */
	  xmlNodePtr pndFile;

	  pndFile = xmlNewChild(pndXml, NULL, NAME_FILE, NULL);
	  xmlSetProp(pndFile, BAD_CAST "verbosity", BAD_CAST "4");
	  xmlSetProp(pndFile, BAD_CAST "name", resNodeGetNameRelative(cxpCtxtRootGet(pccArg),prnTest));
	}

      }
      else {
      }
      resNodeFree(prnTest);
    }
    else if ((pucCgiYear = cxpCtxtCgiGetValueByName(pccArg, BAD_CAST"year")) != NULL) {
      /*! output of XML calendar */
      xmlNodePtr pndCalendar;

      pndCalendar = xmlNewChild(pndXml, NULL, NAME_CALENDAR, NULL);
      domSetPropEat(pndCalendar, BAD_CAST "year", pucCgiYear);
    }
    else {
#if 1
      xmlNewChild(pndXml, NULL, BAD_CAST "info", NULL);
#else
      /*
	show a plain error message only
      */
      pndPlain = xmlNewChild(pndMake, NULL, NAME_PLAIN, BAD_CAST "\n\n\nThe requested action ist not yet supported!");
      xmlSetProp(pndPlain, BAD_CAST "name", BAD_CAST "-");
#endif
    }

    if (pucCgiXpath) {
      /*
	deliver the file content with XSL transformation
      */
      xmlNodePtr pndT;
      xmlNodePtr pndXsl;

      pndT = xmlNewNode(pnsCxp, NAME_XML);
      xmlAddChild(pndT,pndXml);
      pndXsl = xmlNewChild(pndT, NULL, NAME_XSL, NULL);
      xmlSetProp(pndXsl, BAD_CAST "xpath", pucCgiXpath);
      pndXml = pndT;
    }

    if (pucCgiXsl) {
      /*
	deliver the file content with XSL transformation
      */
      xmlNodePtr pndXsl;
      resNodePtr prnCgiXsl;

      /*!\todo for security add prefix "Cgi" to XSL filename */
      xmlStrPrintf(mpucNameFile, BUFFER_LENGTH, "%s.xsl", pucCgiXsl);

      /* detect output type of XSL */

      if ((prnCgiXsl = cxpResNodeResolveNew(pccArg, NULL, mpucNameFile, CXP_O_READ | CXP_O_SEARCH)) != NULL) {
	xmlDocPtr pdocXsl;

	if ((pdocXsl = resNodeReadDoc(prnCgiXsl)) != NULL) {
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
	  xmlAddChild(pndOutput,pndXml);

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
    else {
      xmlAddChild(pndMake,pndXml);
      xmlSetProp(pndXml, BAD_CAST "name", BAD_CAST "-");
    }

    /*
      release the allocated CGI values
    */
    xmlFree(pucCgiCxp);
    xmlFree(pucCgiXsl);
    xmlFree(pucCgiXpath);
    xmlFree(pucCgiPathTranslated);
    xmlFree(pucCgiPath);
  }

  return pccArg->pdocContextNode;
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
