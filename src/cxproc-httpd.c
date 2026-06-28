/* Feel free to use this example code in any way
   you see fit (Public Domain) */


/*
 */
#include <libxml/HTMLtree.h>
#include <libxml/parser.h>
#include <xmlzipio/xmlzipio.h>

/* 
 */
#include <libxslt/xslt.h>
#include <libxslt/xsltutils.h>
#include <libxslt/transform.h>
#include <libxslt/variables.h>

#include <libexslt/exslt.h>
#include <libexslt/exsltconfig.h>

/* 
 */
#include "basics.h"
#include "utils.h"
#include <res_node/res_node_ops.h>
#include <cxp/cxp.h>
#include <cxp/cxp_calendar.h>
#include <cxp/cxp_context_cgi.h>
#include "plain_text.h"
#include <cxp/cxp_dir.h>
#include <rp/rp.h>
#include "dom.h"

#ifdef HAVE_PIE
#include <pie/pie_text.h>
#endif

#ifdef HAVE_LIBARCHIVE
#include <cxp/cxp_archive.h>
#endif

#ifdef HAVE_LIBMAGICK
#include <magick/ImageMagick.h>
#endif

#ifndef _WIN32
#include <sys/select.h>
#include <sys/socket.h>
#else
#include <winsock2.h>
#endif
#include <microhttpd.h>

int iPort = 8888;

#define OFFLINE_NO


cxpContextPtr pccServer = NULL;


/*! cxp Ctxt Cgi Process

\param pccArg -- pointer to context
\return TRUE if , FALSE in case of
*/
BOOL_T
_cxpCtxtRequestProcess(cxpContextPtr pccArg)
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
} /* end of cxpCtxtRequestProcess() */


#define STR_ACTION_INFO "/info"

#define STR_ACTION_REDIR "/redir/"

#define STR_ACTION_XML "/xml/"

#define STR_ACTION_REFRESH "/refresh/"

/*! cxp Ctxt Cgi Process

\param pccArg -- pointer to context
\return TRUE if , FALSE in case of
*/
static int
cxpCtxtRequestCallback(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data,
		     size_t *upload_data_size, void **con_cls)
{
  int ret = -1;
  cxpContextPtr pccRequest = NULL;
  struct MHD_Response *response = NULL;

  if (strcmp("GET",method)) {
    cxpCtxtLogPrint(pccServer, 1, "Unknown method %s", method);
  }
  else if (StringBeginsWith(url, BAD_CAST STR_ACTION_INFO)) {

    pccRequest = cxpCtxtNew();
    if (pccRequest != NULL && cxpCtxtAddChild(pccServer, pccRequest)) {

      xmlChar *pucPage = "<html><body>Hello!</body></html>";
      xmlNodePtr pndMake;
      xmlNodePtr pndPlain;
      xmlNodePtr pndXml;

      pccRequest->pdocContextNode = xmlNewDoc(BAD_CAST "1.0");
      if (pccRequest->pdocContextNode == NULL) {
	cxpCtxtLogPutsExit(pccRequest, 1, "Mem cxpCtxtRequestCallback()");
      }

      pndMake = xmlNewDocNode(pccRequest->pdocContextNode, NULL, BAD_CAST NAME_MAKE, NULL);
      xmlDocSetRootElement(pccRequest->pdocContextNode, pndMake);
      pccRequest->pdocContextNode->encoding = xmlStrdup(BAD_CAST "UTF-8"); /* according to conversion in ParseImportNodePlainContent() */

      pndPlain = xmlNewChild(pndMake, NULL, BAD_CAST NAME_PLAIN, NULL);
      pndXml = xmlNewChild(pndPlain, NULL, BAD_CAST NAME_XML, NULL);
      // xmlSetProp(pndXml, BAD_CAST "name", BAD_CAST "-");
      xmlNewChild(pndXml, NULL, BAD_CAST NAME_INFO, NULL);

      // pucPage = cxpCtxtProcessDump(pccRequest);
      pucPage = cxpProcessPlainNode(pndPlain, pccRequest);
      if (pucPage) {
	/*!\todo apply text substitutions */

	response = MHD_create_response_from_buffer(strlen(pucPage), (void *)pucPage, MHD_RESPMEM_MUST_FREE);
	ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);
      }
    }
  }
  else if (StringBeginsWith(url, BAD_CAST STR_ACTION_REFRESH)) {
	// TODO:
  }
  else if (STR_IS_EMPTY(url)) {
    cxpCtxtLogPrint(pccServer, 1, "Empty URL");
  }
  else if (StringBeginsWith(url,BAD_CAST STR_ACTION_REDIR)) {
    /*!
    search for this file name in CXP_ROOT using 'url' as regexp and redirect client to this URI
    */
   char *pcPattern;
    xmlChar *pucRedir = NULL;
    xmlChar *pucContent = BAD_CAST"Redirect\n";
    resNodePtr prnTest = NULL;
    xmlChar mpucNameFile[BUFFER_LENGTH];

    cxpCtxtLogPrint(pccServer, 1, "URL %s", url);
   // xmlStrPrintf(mpucNameFile, BUFFER_LENGTH, "Location: \"%s\" not found\r\n\r\n", url);

   pcPattern = url + strlen(STR_ACTION_REDIR);

    while ((prnTest = resNodeListFindPath(cxpCtxtRootGet(pccServer), pcPattern, (RN_FIND_FILE | RN_FIND_SYMLINK | RN_FIND_IN_SUBDIR))) != NULL) {

      if (cxpCtxtAccessIsPermitted(pccServer, prnTest) == FALSE || resNodeIsReadable(prnTest) == FALSE) {
	// access error, continue
      }
      else if ((pucRedir = resNodeGetNameRelative(cxpCtxtRootGet(pccServer), prnTest)) != NULL && STR_IS_NOT_EMPTY(pucRedir)) {

	xmlStrPrintf(mpucNameFile, BUFFER_LENGTH, "/cxproc/exe?path=%s", (char *)pucRedir);
	fprintf(stderr, "%s\n", mpucNameFile);
	break;
      }
    }

#ifdef OFFLINE
    cxpCtxtLogPrint(pccServer, 1, "Redir %s", pucRedir);
#else
      response = MHD_create_response_from_buffer(strlen(pucContent), (void *)pucContent, MHD_RESPMEM_PERSISTENT);
      MHD_add_response_header(response, "Location", mpucNameFile);
      ret = MHD_queue_response(connection, MHD_HTTP_PERMANENT_REDIRECT, response);
      MHD_destroy_response(response);
#endif
  }
  else if (StringBeginsWith(url, BAD_CAST STR_ACTION_XML)) {

    pccRequest = cxpCtxtNew();
    if (pccRequest != NULL && cxpCtxtAddChild(pccServer, pccRequest)) {

      xmlChar *pucPage = "<html><body>Hello!</body></html>";
      xmlNodePtr pndMake;
      xmlNodePtr pndPlain;
      xmlNodePtr pndXml;
      xmlNodePtr pndFile;

      pccRequest->pdocContextNode = xmlNewDoc(BAD_CAST "1.0");
      if (pccRequest->pdocContextNode == NULL) {
	cxpCtxtLogPutsExit(pccRequest, 1, "Mem cxpCtxtRequestCallback()");
      }

      pndMake = xmlNewDocNode(pccRequest->pdocContextNode, NULL, BAD_CAST NAME_MAKE, NULL);
      xmlDocSetRootElement(pccRequest->pdocContextNode, pndMake);
      pccRequest->pdocContextNode->encoding = xmlStrdup(BAD_CAST "UTF-8"); /* according to conversion in ParseImportNodePlainContent() */

      pndPlain = xmlNewChild(pndMake, NULL, BAD_CAST NAME_PLAIN, NULL);
      pndXml = xmlNewChild(pndPlain, NULL, BAD_CAST NAME_XML, NULL);
      // xmlSetProp(pndXml, BAD_CAST "name", BAD_CAST "-");
      pndFile = xmlNewChild(pndXml, NULL, BAD_CAST NAME_FILE, NULL);
  xmlSetProp(pndFile, BAD_CAST "verbosity", BAD_CAST "5");
xmlSetProp(pndFile, BAD_CAST "name", BAD_CAST (url + strlen(STR_ACTION_XML)));

      // pucPage = cxpCtxtProcessDump(pccRequest);
      pucPage = cxpProcessPlainNode(pndPlain, pccRequest);
      if (pucPage) {
	/*!\todo apply text substitutions */

	response = MHD_create_response_from_buffer(strlen(pucPage), (void *)pucPage, MHD_RESPMEM_MUST_FREE);
	ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);
      }
    }
  }

#if 0
  pccRequest = cxpCtxtNew();
  if (pccRequest != NULL && cxpCtxtAddChild(pccServer, pccRequest)) {
    struct MHD_Response *response;
#if 0
    const char *page = "<html><body>Hello, browser!</body></html>";

    response = MHD_create_response_from_buffer(strlen(page), (void *)page, MHD_RESPMEM_PERSISTENT);
    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
#elif 0
    xmlNodePtr pndPlain;

    pndPlain = cxpCtxtRequestParse(pccRequest, url);
    if (pndPlain) {
      xmlChar *pucPage;

      pucPage = cxpProcessPlainNode(pndPlain, pccRequest);
      if (pucPage) {
	response = MHD_create_response_from_buffer(strlen(pucPage), (void *)pucPage, MHD_RESPMEM_PERSISTENT);
	ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);
	//xmlFree(pucPage);
      }
      xmlFreeNode(pndPlain);
    }
#elif 0
    xmlChar *pucPage = "<html><body>Hello!</body></html>";
    xmlNodePtr pndMake;
    xmlNodePtr pndPlain;
    xmlNodePtr pndXml;

    pccRequest->pdocContextNode = xmlNewDoc(BAD_CAST "1.0");
    if (pccRequest->pdocContextNode == NULL) {
      cxpCtxtLogPutsExit(pccRequest, 1, "Mem cxpCtxtRequestCallback()");
    }

    pndMake = xmlNewDocNode(pccRequest->pdocContextNode, NULL, BAD_CAST NAME_MAKE, NULL);
    xmlDocSetRootElement(pccRequest->pdocContextNode, pndMake);
    pccRequest->pdocContextNode->encoding = xmlStrdup(BAD_CAST "UTF-8"); /* according to conversion in ParseImportNodePlainContent() */

    pndPlain = xmlNewChild(pndMake, NULL, BAD_CAST NAME_PLAIN, NULL);
    pndXml = xmlNewChild(pndPlain, NULL, BAD_CAST NAME_XML, NULL);
    // xmlSetProp(pndXml, BAD_CAST "name", BAD_CAST "-");
    xmlNewChild(pndXml, NULL, BAD_CAST NAME_INFO, NULL);

    // pucPage = cxpCtxtProcessDump(pccRequest);
    pucPage = cxpProcessPlainNode(pndPlain, pccRequest);
    if (pucPage) {
      /*!\todo apply text substitutions */

      response = MHD_create_response_from_buffer(strlen(pucPage), (void *)pucPage, MHD_RESPMEM_PERSISTENT);
      ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
      MHD_destroy_response(response);
      xmlFree(pucPage);
    }
#else
    xmlNodePtr pndPlain;

    if ((pndPlain = cxpCtxtRequestParse(pccRequest, url)) != NULL && IS_NODE_PLAIN(pndPlain)) {
      xmlChar *pucPage;

#ifdef OFFLINE
//      pucPage = cxpCtxtProcessDump(pccRequest);
      pucPage = cxpProcessPlainNode(pndPlain, pccRequest);
#else
      pucPage = cxpProcessPlainNode(pndPlain, pccRequest);
#endif
      if (pucPage) {
	/*!\todo apply text substitutions */

#ifdef OFFLINE
	puts(pucPage);
//	xmlFree(pucPage);
#else
	size_t l;
	char b[100];

	l = xmlStrlen(pucPage);
	sprintf(b, "%i", l);
// libmicrohttpd response redirect


//response = MHD_create_response_from_buffer(l, (void *)pucPage, MHD_RESPMEM_MUST_FREE);
response = MHD_create_response_from_buffer(1, (void *)"", MHD_RESPMEM_PERSISTENT);
//MHD_add_response_header(response, MHD_HTTP_HEADER_CONTENT_TYPE, "text/xml; charset=UTF-8");
//MHD_add_response_header(response, MHD_HTTP_HEADER_CONTENT_LENGTH, b);
MHD_add_response_header (response, "Location", "/cxproc/exe?cxp=info");
	//ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
//	response = MHD_create_response(l, (void *)pucPage, MHD_RESPMEM_MUST_FREE);
//	MHD_add_response_header(response, MHD_HTTP_HEADER_REDIRECT_REF, b);
	ret = MHD_queue_response(connection, MHD_HTTP_TEMPORARY_REDIRECT, response);
	MHD_destroy_response(response);
#endif
      }
      fputs(" OK\n", stderr);
    }
    else {
      fputs(" ERROR\n", stderr);
    }

#endif

    // cxpCtxtFree(pccRequest);
  }
#endif

return ret;
} /* end of cxpCtxtRequestCallback() */



/*! Callback procedure performing a HTTP redirect
*/
static int
cxpCtxtRequestRedirectCallback(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data,
			       size_t *upload_data_size, void **con_cls)
{
  int ret = -1;
  //cxpContextPtr pccRequest = NULL;
  struct MHD_Response *response = NULL;

  if (strcmp("GET", method)) {
    cxpCtxtLogPrint(pccServer, 1, "Unknown method %s", method);
  }
  else if (STR_IS_EMPTY(url)) {
    cxpCtxtLogPrint(pccServer, 1, "Empty URL");
  }
  else {
    /*!
    search for this file name in CXP_ROOT using 'url' as regexp and redirect client to this URI
    */
    xmlChar *pucRedir = NULL;
    xmlChar *pucContent = BAD_CAST "Redirect\n";
    resNodePtr prnTest = NULL;
    xmlChar mpucNameFile[BUFFER_LENGTH];

    if ((prnTest = resNodeListFindPath(cxpCtxtRootGet(pccServer), &url[1], (RN_FIND_FILE | RN_FIND_SYMLINK | RN_FIND_IN_SUBDIR))) != NULL &&
	(pucRedir = resNodeGetNameRelative(cxpCtxtRootGet(pccServer), prnTest)) != NULL) {
      xmlStrPrintf(mpucNameFile, BUFFER_LENGTH, "/%s", (char *)pucRedir);
      response = MHD_create_response_from_buffer(strlen(pucContent), (void *)pucContent, MHD_RESPMEM_PERSISTENT);
      MHD_add_response_header(response, "Location", mpucNameFile);
      ret = MHD_queue_response(connection, MHD_HTTP_PERMANENT_REDIRECT, response);
    }
    else {
      xmlStrPrintf(mpucNameFile, BUFFER_LENGTH, "%s not found", url);
      response = MHD_create_response_from_buffer(strlen(mpucNameFile), (void *)mpucNameFile, MHD_RESPMEM_PERSISTENT);
      ret = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
    }
    cxpCtxtLogPrint(pccServer, 1, "URL '%s' to Location: '%s'", url, mpucNameFile);

    MHD_destroy_response(response);
  }
 //cxpCtxtFree(pccRequest);

  return ret;
} /* end of cxpCtxtRequestRedirectCallback() */


int
main(int argc, char *argv[], char *envp[])
{
  struct MHD_Daemon *daemon;

  /*
  this is the HTTP daemon mode
  */

#ifdef _MSC_VER
  if (_setmode(_fileno(stdout), _O_BINARY) == -1) {
    perror( "Cannot set mode" );
  }
#endif

#ifdef _MSC_VER
  /* no workaround required */
#elif defined(_WIN32)
  /* s. Workaround at http://www.linuxquestions.org/questions/programming-9/%5Bsolved%5Dusing-libxml2-on-mingw-xmlfree-crashes-839802/ */
  xmlParseMemory((const char *)" ",1);
  if(xmlFree==NULL) {
    xmlMemGet(&xmlFree,&xmlMalloc,&xmlRealloc,NULL);
  }
#endif

#if 0
    /* for online debugging only */
#ifdef _MSC_VER
    Sleep(10 * CLOCKS_PER_SEC);
#else
    sleep(20);
#endif
#endif

  /* register for exit() */
  if (atexit(xsltCleanupGlobals) != 0
      || atexit(xmlCleanupParser) != 0
      || atexit(domCleanup) != 0
      || atexit(cxpCleanup) != 0
#ifdef HAVE_PIE
      || atexit(pieTextCleanup) != 0
#endif
#ifdef HAVE_LIBMAGICK
      || atexit(MagickCoreTerminus) != 0
#endif
#ifdef HAVE_LIBCURL
//      || atexit(curl_global_cleanup) != 0
#endif
      || atexit(zipIconvCleanup) != 0
    ) {
    exit(EXIT_FAILURE);
  }

#ifdef HAVE_LIBCURL
  curl_global_init(CURL_GLOBAL_DEFAULT);
  /* log all details, except SSL handling */
  //curl_global_trace("all,-ssl");
#endif

  xmlInitParser();
  LIBXML_TEST_VERSION

  xmlKeepBlanksDefault(0);
  xmlRegisterDefaultInputCallbacks();
  xmlRegisterDefaultOutputCallbacks();
  exsltRegisterAll();

  ceInit();
  zipIconvInit();

#ifdef _WIN32
  resPathSetNativeEncoding("ISO-8859-1");
#endif

  pccServer = cxpCtxtParseNew(NULL, STR_IS_EMPTY(argv[1]) ? "config.xml" : argv[1]);
  if (pccServer) {
    int iExit = EXIT_SUCCESS;

    cxpCtxtLogSetLevel(pccServer, 3);
#ifdef _WIN32
    cxpCtxtEncSetEnv(pccServer, BAD_CAST "ISO-8859-1");
#else
/* UTF-8 */
#endif
    cxpCtxtEncSetDefaults(pccServer);
    cxpCtxtEncSetPlain(pccServer, BAD_CAST "ISO-8859-1"); // TODO: use value of CXP_PLAIN_ENC
    cxpCtxtCacheEnable(pccServer, TRUE);

#ifdef OFFLINE
/* Offline debugging */
    cxpCtxtRequestCallback(NULL, NULL, (const char *)"/redir/Test/dir.cxp", (const char *)"GET", (const char *)"1.1", NULL, 0, NULL);
#else

// TEST: curl -v 'http://localhost:8888/dir.cxp' -i -L

    daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, cxpCtxtGetPortNumber(pccServer), NULL, NULL, &cxpCtxtRequestRedirectCallback, NULL, MHD_OPTION_END);
    if (NULL == daemon) {
      return 1;
    }

    (void)getchar();

    MHD_stop_daemon(daemon);
#endif
    iExit = cxpCtxtGetExitCode(pccServer);
    cxpCtxtFree(pccServer);
  }

  return 0;
}
