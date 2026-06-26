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

#define PORT 8888

static int
answer_to_connection(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data,
		     size_t *upload_data_size, void **con_cls)
{
  struct MHD_Response *response;
  int ret;

#if 1
  const char *page = "<html><body>Hello, browser!</body></html>";

  response = MHD_create_response_from_buffer (strlen (page), (void *) page, MHD_RESPMEM_PERSISTENT);
  ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
  MHD_destroy_response(response);
#elif 0
  response = MHD_create_response_from_buffer(strlen(url), (void *)url, MHD_RESPMEM_PERSISTENT);

  ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
  MHD_destroy_response(response);
#else
#endif

  fputs(url, stderr);
  fputs(" OK\n", stderr);

  return ret;
}

int
main()
{
  struct MHD_Daemon *daemon;

  /*
  this is the HTTP daemon mode
  */
  int res;
  cxpContextPtr pccMain = NULL;
  xmlChar *pucT = NULL;
  xmlChar *pucTT = NULL;
  FILE *cxperr = NULL;

  if ((pucTT = BAD_CAST getenv("CXP_LOGFILE")) != NULL) {
    /* use defined value as log file location */
    pucT = xmlStrdup(pucTT);
  }
  else if ((pucTT = BAD_CAST getenv("DOCUMENT_ROOT")) != NULL) {
    /* derive log file location from HTTP server configuration */
    pucT = resPathConcatNormalizedStr(pucTT, BAD_CAST "/cxproc-cgi.log");
  }
  else if ((pucTT = resPathGetCwdStr()) != NULL) {
    /* try a default value */
    pucT = resPathConcatNormalizedStr(pucTT, BAD_CAST "/cxproc-cgi.log");
    xmlFree(pucTT);
  }
  else {
    fprintf(stderr, "using stderr\n");
  }

  if (STR_IS_NOT_EMPTY(pucT)) {
    resNodePtr prnNew;

    if ((prnNew = resNodeDirNew(pucT)) != NULL) {
      if (resNodeIsWriteable(prnNew) && (cxperr = freopen((const char *)resNodeGetNameNormalizedNative(prnNew), "w", stderr)) != NULL) {
	fprintf(stderr, "using '%s' as error output\n", pucT);
      }
      else {
	//fprintf(stderr, "using stderr\n");
      }
      resNodeFree(prnNew);
    }
  }
  xmlFree(pucT);

  SetLogLevel(2); /* default value */

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

#if 0

  pccMain = cxpCtxtCgiNew(argc,argv,envp);
  if (pccMain) {
    int iExit = EXIT_SUCCESS;

    //cxpCtxtCacheEnable(pccMain, TRUE);

#ifdef _WIN32
    cxpCtxtEncSetEnv(pccMain, BAD_CAST "ISO-8859-1");
#else
    /* UTF-8 */
#endif
    cxpCtxtEncSetDefaults(pccMain);
    cxpCtxtEncSetPlain(pccMain, BAD_CAST "ISO-8859-1"); // TODO: use value of CXP_PLAIN_ENC

    cxpCtxtCacheEnable(pccMain, TRUE);
    cxpCtxtCgiParse(pccMain);
    // cxpCtxtLogInfo(pccMain);
    cxpCtxtCgiProcess(pccMain);    /*!\todo use thread */
    iExit = cxpCtxtGetExitCode(pccMain);
    cxpCtxtFree(pccMain);

    if (cxperr != NULL && cxperr != stderr) {
      fclose(cxperr);
    }

  }
#endif

  daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL, &answer_to_connection, NULL, MHD_OPTION_END);
  if (NULL == daemon) {
    return 1;
  }

  (void)getchar();

  MHD_stop_daemon(daemon);
  return 0;
}
