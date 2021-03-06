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
#include <pie/pie_calendar.h>
#include <petrinet/petrinet.h>
#endif
#ifdef HAVE_LIBSQLITE3
#include <database/database.h>
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
    pccResult->iCountArgv = argc;
    pccResult->ppcArgv = argv;

    cxpCtxtEnvDup(pccResult,envp);

    pucT = cxpCtxtEnvGetValueByName(pccResult, BAD_CAST "CXP_LOG");
    if (cxpCtxtLogSetLevelStr(pccResult, pucT) == 0) {
      /* level value from environment */
    }
    else {
#if defined(DEBUG) || defined(TESTCODE)
      cxpCtxtLogSetLevel(pccResult, 3);
#else
      cxpCtxtLogSetLevel(pccResult, 4);
#endif
    }
    xmlFree(pucT);

    cxpCtxtLogPrint(pccResult,1,"Based on %s %s",CXP_VER_FILE_VERSION_STR,CXP_VER_FILE_BRANCH_STR);

    if (cxpCtxtRootSet(pccResult,NULL)) {
      cxpCtxtSearchSet(pccResult, NULL);
      cxpCtxtLocationSet(pccResult, cxpCtxtRootGet(pccResult));
      cxpCtxtSetReadonly(pccResult, cxpCtxtEnvGetBoolByName(pccResult, BAD_CAST "CXP_READONLY", FALSE));
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
  resNodePtr prnContent = NULL;
  resNodePtr prnPathTranslated = NULL;
  xmlChar *pucNameNormalizedDir = NULL;
  xmlChar *pucNameNormalizedFile = NULL;
  xmlChar mpucNameFile[BUFFER_LENGTH];
  xmlChar *pucCgiCxp = NULL;
  xmlChar *pucCgiAjax = NULL;
  xmlChar *pucCgiEncoding = NULL;
  xmlChar *pucCgiDir = NULL;
  xmlChar *pucCgiFile = NULL;
  xmlChar *pucCgiPath = NULL;
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
    xmlStrPrintf(mpucNameFile, BUFFER_LENGTH, (const char *)pucCgiCxp);
  }

  pucCgiEncoding = cxpCtxtCgiGetValueByName(pccArg,BAD_CAST"encoding");

  pucCgiAjax = cxpCtxtCgiGetValueByName(pccArg,BAD_CAST"ajax");

  pucCgiXsl = cxpCtxtCgiGetValueByName(pccArg,BAD_CAST"xsl");

  pucCgiXpath = cxpCtxtCgiGetValueByName(pccArg,BAD_CAST"xpath");

  pucCgiPath = cxpCtxtCgiGetValueByName(pccArg,BAD_CAST"path");
  if (pucCgiPath) {
    /*!
    map pucCgiPath either to pucCgiDir OR pucCgiFile
    */
    resNodePtr prnTest = NULL;

    prnTest = resNodeRootNew(cxpCtxtRootGet(pccArg),pucCgiPath);
    if (resNodeReadStatus(prnTest) && resNodeIsDir(prnTest)) {
      cxpCtxtLogPrint(pccArg, 2, "Copy value '%s' of 'path' to 'dir'", resNodeGetNameNormalized(prnTest));
      pucCgiDir = xmlStrdup(pucCgiPath);
      pucCgiFile = NULL;
    }
    else {
      cxpCtxtLogPrint(pccArg, 2, "Copy value '%s' of 'path' to 'file'", pucCgiPath);
      pucCgiDir = NULL;
      pucCgiFile = xmlStrdup(pucCgiPath);
    }
    resNodeFree(prnTest);
  }
  else {
    pucCgiDir = cxpCtxtCgiGetValueByName(pccArg,BAD_CAST"dir");
    if (pucCgiDir) {
      /* ignoring "file" */
    }
    else {
      pucCgiFile = cxpCtxtCgiGetValueByName(pccArg,BAD_CAST"file");
    }
  }

#ifdef HAVE_CGI
  if (pucCgiAjax != NULL && xmlStrlen(pucCgiAjax) > 0) {
    /*!\bug security risk? */
    if (pucCgiXsl) {
      xmlFree(pucCgiXsl);
      pucCgiXsl = NULL;
    }
    if (pucCgiCxp) {
      xmlFree(pucCgiCxp);
      pucCgiCxp = NULL;
    }
  }
  else if (pucCgiCxp != NULL && xmlStrlen(pucCgiCxp) > 0) {
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

  if (pucCgiDir) {
    prnDir = resNodeRootNew(cxpCtxtRootGet(pccArg), pucCgiDir);
    pucNameNormalizedDir = resNodeGetNameNormalized(prnDir);
  }
  else if (pucCgiFile) {
    prnFile = resNodeRootNew(cxpCtxtRootGet(pccArg), pucCgiFile);
    if (resNodeReadStatus(prnFile)) {
      prnContent = resNodeGetLastDescendant(prnFile);
      iMimeType = resNodeGetMimeType(prnContent);
      pucNameNormalizedFile = resNodeGetNameNormalized(prnFile);
    }
  }

  if (pucCgiAjax) {
    /*
    deliver the result as plain text via AJAX
    */
    xmlChar *pucRelease;
    xmlNodePtr pndT;
    xmlDocPtr pdocT;

#if WITH_AJAX
    pucRelease = xmlStrdup(BAD_CAST"<make log='3'><plain name='-'>");
    pucRelease = xmlStrcat(pucRelease, pucCgiAjax);
    pucRelease = xmlStrcat(pucRelease, BAD_CAST"</plain>");
    pucRelease = xmlStrcat(pucRelease, BAD_CAST"<plain name='-'>OK</plain></make>");
    cxpCtxtLogPrint(pccArg, 3, "Parse AJAX '%s'", pucRelease);

    pdocT = xmlParseMemory((const char *)pucRelease, xmlStrlen(pucRelease));
    if (pdocT) {
      xmlNodePtr pndCopy;

      pndT = xmlDocGetRootElement(pdocT);
      if (pndT) {
	pndCopy = xmlCopyNode(pndT->children, 1);
	if (pndCopy) {
	  xmlAddChild(pndMake, pndCopy);
	}
	else {
	  cxpCtxtLogPrint(pccArg, 1, "No usable children");
	}
      }
      xmlFreeDoc(pdocT);

      cxpCtxtLogPrintDoc(pccArg, 4, "cxpParseCgi()", pccArg->pdocContextNode);
    }
    else {
      cxpCtxtLogPrint(pccArg, 1, "Cant parse AJAX '%s'", pucRelease);
    }
    xmlFree(pucRelease);
#else
    cxpCtxtLogPrint(pccArg, 1, "Compiled without AJAX, ignoring");
#endif
  }
  else if (prnPathTranslated) {
    /* deliver the file content via CXP configuration */
    pndXml = xmlNewChild(pndMake, NULL, NAME_XML, NULL);
    xmlSetProp(pndXml, BAD_CAST "name", resNodeGetNameNormalized(prnPathTranslated));
    xmlSetProp(pndXml, BAD_CAST "schema", BAD_CAST "cxp.rng");
    xmlSetProp(pndXml, BAD_CAST "eval", BAD_CAST "yes");
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

    if ((prnCgiXsl = cxpResNodeResolveNew(pccArg, NULL, mpucNameFile, CXP_O_SEARCH)) != NULL) {
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
	if (pucCgiFile) {
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
	else if (pucCgiDir) {
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
  else if (pucCgiDir) {
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
      xmlSetProp(pndDir, BAD_CAST "depth", BAD_CAST "1");
      xmlSetProp(pndDir, BAD_CAST "urlencode", BAD_CAST "yes");
      pndDir = xmlNewChild(pndDir, NULL, NAME_DIR, NULL);
      xmlSetProp(pndDir, BAD_CAST "name", pucNameNormalizedDir);
    }
  }
  else if (pucCgiFile) {
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
	  xmlSetProp(pndImport, BAD_CAST "name", pucNameNormalizedFile);
#endif
	}
	else if (iMimeType == MIME_TEXT_HTML) {
	  xmlNodePtr pndXhtml;

	  pndXhtml = xmlNewChild(pndXml, NULL, NAME_XHTML, NULL);
	  xmlSetProp(pndXhtml, BAD_CAST "name", pucNameNormalizedFile);
	}
	else {
	  xmlNodePtr pndXmlChild;

	  pndXmlChild = xmlNewChild(pndXml, NULL, NAME_XML, NULL);
	  xmlSetProp(pndXmlChild, BAD_CAST "name", pucNameNormalizedFile);
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
	/*
	deliver the file content without CXP or XSL
	*/
	xmlNodePtr pndCopy;
	pndCopy = xmlNewChild(pndMake, NULL, NAME_FILECOPY, NULL);
	if (resNodeIsFileInArchive(prnContent)) {
	  xmlSetProp(pndCopy, BAD_CAST "from", resNodeGetNameNormalized(prnContent));
	}
	else {
	  xmlSetProp(pndCopy, BAD_CAST "from", pucNameNormalizedFile);
	}
	xmlSetProp(pndCopy, BAD_CAST "to", BAD_CAST "-");
	xmlSetProp(pndCopy, BAD_CAST "encoding", pucCgiEncoding);
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
  resNodeFree(prnDir);
  resNodeFree(prnFile);
  /*
  release the allocated CGI values
  */
  xmlFree(pucCgiAjax);
  xmlFree(pucCgiCxp);
  xmlFree(pucCgiXsl);
  xmlFree(pucCgiXpath);
  xmlFree(pucCgiDir);
  xmlFree(pucCgiFile);
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
  xmlChar *pucT;

#ifdef DEBUG
  cxpCtxtLogPrint(pccArg,2, "cxpCtxtCgiProcess(cxpContextPtr pccArg)");
#endif

  cxpProcessMakeNode(xmlDocGetRootElement(pccArg->pdocContextNode), pccArg);

  return fResult;
} /* end of cxpCtxtCgiProcess() */


#ifdef TESTCODE
#include "test/test_cxp_context_cgi.c"
#endif
