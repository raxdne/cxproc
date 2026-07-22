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
#include <netinet/in.h>
#include <microhttpd.h>

int iPort = 8183;

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

#define STR_ACTION_EXE "/exe/"

static int 
SetValueForKey(void *cls, enum MHD_ValueKind kind, const char *key, const char *value)
{
  cxpContextPtr pccT = (cxpContextPtr)cls;

   // cxpCtxtLogPrint(pccT, 1, "%s %s", key, value);
    cxpCtxtParamReset(pccT,BAD_CAST key,BAD_CAST value, query);

  return MHD_YES;
} /* end of SetValueForKey() */

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

  pccRequest = cxpCtxtNew();
  if (pccRequest != NULL && cxpCtxtAddChild(pccServer, pccRequest)) {

    if (strcmp("GET", method)) {
      cxpCtxtLogPrint(pccRequest, 1, "Unknown method %s", method);
    }
    else if (STR_IS_EMPTY(url)) {
      // TEST: curl -v 'http://localhost:8888/' -i -L
      cxpCtxtLogPrint(pccRequest, 1, "Empty URL");
    }
    else if (StringBeginsWith(url, BAD_CAST STR_ACTION_INFO)) {
      // TEST: curl -v 'http://localhost:8888/info?a=1&c=asdf&y=1929&m=12&d=11&z=ABCDEFGHIJ' -i -L

      xmlChar *pucPage = "<html><body>Hello!</body></html>";
      xmlNodePtr pndMake;
      xmlNodePtr pndPlain;
      xmlNodePtr pndXml;

      cxpCtxtParamReset(pccRequest, BAD_CAST "demo", BAD_CAST "only", var);
      MHD_get_connection_values(connection, MHD_GET_ARGUMENT_KIND, (MHD_KeyValueIterator)&SetValueForKey, (void *)pccRequest);
      cxpCtxtParamPrint(pccRequest);

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
    else if (StringBeginsWith(url, BAD_CAST STR_ACTION_REFRESH)) {
      xmlChar *pucPage = "<html><body>Refreshing!</body></html>";
      resNodePtr prnT = NULL;

      response = MHD_create_response_from_buffer(strlen(pucPage), (void *)pucPage, MHD_RESPMEM_PERSISTENT);
      ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
      prnT = resNodeDup(cxpCtxtRootGet(pccRequest), (RN_DUP_THIS | RN_DUP_READ));
      cxpCtxtRootSet(pccRequest, prnT);
      cxpCtxtLogPrint(pccRequest, 1, "Refreshing search index of '%s'", resNodeGetNameNormalized(prnT));
      resNodeFree(prnT);
      MHD_destroy_response(response);
    }
    else if (StringBeginsWith(url, BAD_CAST STR_ACTION_REDIR)) {
      /*!
      search for this file name in CXP_ROOT using 'url' as regexp and redirect client to this URI

      TEST: curl -v 'http://localhost:8888/redir/TestLinks.txt' -i -L

      */
      xmlChar *pucRedir = NULL;
      xmlChar *pucPattern = NULL;
      resNodePtr prnTest = NULL;
      xmlChar mpucNameFile[BUFFER_LENGTH];
      resNodePtr prnT = NULL;

      pucPattern = &url[strlen(STR_ACTION_REDIR)];
      //prnT = resNodeDup(cxpCtxtRootGet(pccRequest), (RN_DUP_THIS | RN_DUP_READ));
      prnT = cxpCtxtRootGet(pccServer);

      MHD_get_connection_values(connection, MHD_GET_ARGUMENT_KIND, (MHD_KeyValueIterator)&SetValueForKey, (void *)pccRequest);

      if ((prnTest = resNodeListFindPath(prnT, pucPattern, (RN_FIND_FILE | RN_FIND_IN_SUBDIR))) != NULL &&
	  (pucRedir = resNodeGetNameRelative(prnT, prnTest)) != NULL) {
	xmlChar *pucRedirEncoded = NULL;
	xmlChar *pucContent = BAD_CAST "Redirect\n";

	resPathChangeToSlashes(pucRedir);
	pucRedirEncoded = EncodeRFC1738(pucRedir);
	xmlStrPrintf(mpucNameFile, BUFFER_LENGTH, "/%s", (char *)pucRedirEncoded);
	cxpCtxtLogPrint(pccRequest, 3, "URL '%s' to Location: '%s'", url, mpucNameFile);
	response = MHD_create_response_from_buffer(strlen(pucContent), (void *)pucContent, MHD_RESPMEM_PERSISTENT);
	MHD_add_response_header(response, "Location", mpucNameFile);
	ret = MHD_queue_response(connection, MHD_HTTP_PERMANENT_REDIRECT, response);
	MHD_destroy_response(response);
	xmlFree(pucRedirEncoded);
      }
      else {
	xmlStrPrintf(mpucNameFile, BUFFER_LENGTH, "%s not found", pucPattern);
	cxpCtxtLogPrint(pccRequest, 1, "URL '%s' no Location found", url);
	response = MHD_create_response_from_buffer(strlen(mpucNameFile), (void *)mpucNameFile, MHD_RESPMEM_PERSISTENT);
	ret = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
	MHD_destroy_response(response);
      }
      cxpCtxtParamPrint(pccRequest);
      //resNodeFree(prnT);
      /*! REQ: automatic refresh of index after n seconds */
    }
    else if (StringBeginsWith(url, BAD_CAST STR_ACTION_XML)) {

      // TEST: curl -v 'http://localhost:8888/xml/Test/Documents/TestLinks.txt' -i -L

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
      xmlSetProp(pndFile, BAD_CAST "name", BAD_CAST(url + strlen(STR_ACTION_XML)));

      // pucPage = cxpCtxtProcessDump(pccRequest);
      pucPage = cxpProcessPlainNode(pndPlain, pccRequest);
      if (pucPage) {
	/*!\todo apply text substitutions */

	response = MHD_create_response_from_buffer(strlen(pucPage), (void *)pucPage, MHD_RESPMEM_MUST_FREE);
	ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);
      }
    }
    else {
      // TEST: curl -v 'http://localhost:8888/blahblah/' -i -L

      char *pcPage = "<html><body>Hello!</body></html>";

      cxpCtxtLogPrint(pccRequest, 1, "Invalid URL");

      response = MHD_create_response_from_buffer(strlen(pcPage), (void *)pcPage, MHD_RESPMEM_PERSISTENT);
      ret = MHD_queue_response(connection, MHD_HTTP_SERVICE_UNAVAILABLE, response);
      MHD_destroy_response(response);
    }
  }

  return ret;
} /* end of cxpCtxtRequestCallback() */



int
main(int argc, char *argv[], char *envp[])
{

  /*
  this is the HTTP daemon mode
  */

#ifdef _MSC_VER
  if (_setmode(_fileno(stdout), _O_BINARY) == -1) {
    perror("Cannot set mode");
  }
#endif

#ifdef _MSC_VER
  /* no workaround required */
#elif defined(_WIN32)
  /* s. Workaround at http://www.linuxquestions.org/questions/programming-9/%5Bsolved%5Dusing-libxml2-on-mingw-xmlfree-crashes-839802/ */
  xmlParseMemory((const char *)" ", 1);
  if (xmlFree == NULL) {
    xmlMemGet(&xmlFree, &xmlMalloc, &xmlRealloc, NULL);
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

  pccServer = cxpCtxtMainNew(argc, argv, envp);
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
    
    // cxpCtxtParamReset(pccServer,BAD_CAST"demo",BAD_CAST "only", cgi);
    cxpCtxtParamPrint(pccServer);

    if (resNodeListParse(cxpCtxtRootGet(pccServer), 99, NULL)) {}

    // TEST: curl -v 'http://localhost:8888/info?a=1&c=asdf&y=1929&m=12&d=11&z=ABCDEFGHIJ' -i -L

    {
      /* PROMPT: How to restrict responses of libmicrohttpd to localhost? */
      int iPort;
      struct MHD_Daemon *daemon;
      struct sockaddr_in loopback_addr;

      iPort = cxpCtxtGetPortNumber(pccServer);

      // 1. configure the sockaddr struct for IPv4 loopback
      loopback_addr.sin_family = AF_INET;
      loopback_addr.sin_port = htons(iPort);
      loopback_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // Binds strictly to 127.0.0.1

      // 2. Start the daemon using MHD_OPTION_SOCK_ADDR
      daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, iPort, NULL, NULL,					// No access policy callback needed
				&cxpCtxtRequestCallback, NULL, MHD_OPTION_SOCK_ADDR, (struct sockaddr *)&loopback_addr, // Enforce localhost
				MHD_OPTION_END);

      if (daemon) {
	cxpCtxtLogPrint(pccServer, 1, "Server running securely on http://127.0.0.1:%d", iPort);
	(void)getchar();
	MHD_stop_daemon(daemon);
      }
    }

    iExit = cxpCtxtGetExitCode(pccServer);
    cxpCtxtFree(pccServer);
  }

  return 0;
}
