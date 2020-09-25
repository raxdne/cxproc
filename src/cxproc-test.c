/* 
   cxproc - Configurable Xml PROCessor

   Copyright (C) 2006..2020 by Alexander Tenbusch

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  

*/

/*! cxproc test frontend
*/

/*
 */
#include <libxml/HTMLtree.h>
#include <libxml/parser.h>
#include <xmlzipio/xmlzipio.h>

#ifdef HAVE_LIBCURL

#include <curl/curl.h>

#if ! CURL_AT_LEAST_VERSION(7, 62, 0)
#error "this requires curl 7.62.0 or later"
#endif

#endif

/* 
 */
#include <libxslt/xslt.h>
#include <libxslt/xsltutils.h>
#include <libxslt/transform.h>
#include <libxslt/variables.h>

#ifdef LIBXML_THREAD_ENABLED
#include <libxml/globals.h>
#include <libxml/threads.h>
#ifdef HAVE_LIBPTHREAD
#include <pthread.h>
#elif WITH_THREAD
#endif
#endif

/* 
 */
#include "basics.h"
#include "utils.h"
#include <res_node/res_node_ops.h>
#include <cxp/cxp.h>
#include <cxp/cxp_threadp.h>
#include "dom.h"
#include "plain_text.h"
#include <cxp/cxp_dir.h>

#ifdef HAVE_PIE
#include <pie/pie_text.h>
#include <ics/ics.h>
#include <vcf/vcf.h>
#include <pie/pie_calendar.h>
#endif
#ifdef HAVE_PETRINET
#include <petrinet/petrinet.h>
#endif
#ifdef HAVE_RP
#include <rp/rp.h>
#endif
#ifdef HAVE_LIBSQLITE3
#include <database/cxp_database.h>
#endif
#ifdef HAVE_JS
#include <script/script.h>
#endif
#ifdef HAVE_JSON
#include <json/json.h>
#endif
#ifdef HAVE_LIBARCHIVE
#include <archive/cxp_archive.h>
#endif

/* additional/specific testcode for used libs */

#ifdef HAVE_LIBCURL
#include "test/test_libcurl.c"
#endif
#include "test/test_libxml.c"
#include "test/test_libxslt.c"
#include "test/test_pcre.c"
#include "test/test_threads.c"


int
main(int argc, char** argv, char** envp)
{
  cxpContextPtr pccTest = NULL;
  char* pcTest = NULL;
  int iErrorCode = 0;
  int i;

  SetLogLevel(2);

  for (i = 1; i < argc; i++) {
    if (xmlStrEqual(BAD_CAST argv[i], BAD_CAST "-t") && i < argc - 1) {
      pcTest = argv[++i];
      printf("Test mode for module '%s'\n", pcTest);
    }
  }

#ifdef _MSC_VER
  if (_setmode(_fileno(stdout), _O_BINARY) == -1) {
    perror("Cannot set mode");
  }
#endif

#ifdef _MSC_VER
  /* no workaround required */
#elif defined(_WIN32)
  /* s. Workaround at http://www.linuxquestions.org/questions/programming-9/%5Bsolved%5Dusing-libxml2-on-mingw-xmlfree-crashes-839802/ */
  xmlParseMemory((const char*)" ", 1);

  if (xmlFree == NULL) {
    xmlMemGet(&xmlFree, &xmlMalloc, &xmlRealloc, NULL);
  }
#endif

  /* register for exit() */
  if (atexit(xsltCleanupGlobals) != 0
      || atexit(xmlCleanupParser) != 0
      || atexit(xmlMemoryDump) != 0
      || atexit(domCleanup) != 0
#ifdef HAVE_PIE
      || atexit(pieTextCleanup) != 0
#endif
    ) {
    exit(EXIT_FAILURE);
  }

  xmlInitParser();
  LIBXML_TEST_VERSION

  if (xmlInitMemory() == 0) {
    exit(EXIT_FAILURE);
  }
  xmlKeepBlanksDefault(0);
  xmlRegisterDefaultInputCallbacks();
  xmlRegisterDefaultOutputCallbacks();
#ifdef HAVE_ZLIB
  /* code for xmlzipio http://hal.iwr.uni-heidelberg.de/~christi/projects/xmlzipio.html */
  xmlZipRegisterInputCallback();
  /* it's importend to xmlzipio after the default handlers, so xmlzipio is asked first. */
  xmlZipRegisterOutputCallback();
#endif

#ifdef HAVE_PIE
  ceInit();
#endif

#ifdef _WIN32
  resPathSetNativeEncoding("ISO-8859-1");
#endif

  pccTest = cxpCtxtCliNew(argc, argv, envp);
  if (pccTest) {
    int r;
    int iExit = EXIT_SUCCESS;

#ifdef HAVE_JS
    scriptInit(pccTest);
#endif

    cxpCtxtCacheEnable(pccTest, FALSE);

    resNodeUnlinkRecursivelyStr(BAD_CAST TEMPPREFIX);
    if (resNodeMakeDirectoryStr(BAD_CAST TEMPPREFIX, MODE_DIR_CREATE)) {

      if (pcTest == NULL || xmlStrEqual(BAD_CAST pcTest, BAD_CAST "basics")) {
#ifdef HAVE_LIBCURL
	iErrorCode += curlTest();
#endif
	iErrorCode += xmlTest();
	iErrorCode += xslTest();
	iErrorCode += pcreTest();
	iErrorCode += threadTest();
	iErrorCode += utilsTest();
      }

      if (pcTest == NULL || xmlStrEqual(BAD_CAST pcTest, BAD_CAST "res_node")) {
	iErrorCode += resNodeTestString();
	iErrorCode += resNodeTestMime();
	iErrorCode += resNodeTest();
	iErrorCode += resNodeTestList();
	iErrorCode += resNodeTestProp();
	iErrorCode += resNodeTestInOut();
	iErrorCode += resNodeTestOperations();
      }

      if (pcTest == NULL || xmlStrEqual(BAD_CAST pcTest, BAD_CAST "dom")) {
	iErrorCode += domTest();
      }

      if (pcTest == NULL || xmlStrEqual(BAD_CAST pcTest, BAD_CAST "plain")) {
	iErrorCode += plainTextTest();
      }

      if (pcTest == NULL || xmlStrEqual(BAD_CAST pcTest, BAD_CAST "cxp")) {
	cxpContextPtr pccT = NULL;
	xmlNodePtr pndXml;
	xmlNodePtr pndMake;

	pccT = cxpCtxtDup(pccTest);
	if (pccT) {
	  iErrorCode += cxpCtxtTest(pccT);
	  iErrorCode += cxpCtxtEncTest(pccT);
	  iErrorCode += cxpCtxtCacheTest(pccT);
	  iErrorCode += cxpSubstTest(pccT);
	  iErrorCode += cxpTest(pccT);

	  cxpCtxtIncrExitCode(pccTest, cxpCtxtGetExitCode(pccT));
	  cxpCtxtFree(pccT);
	}
      }

#if defined(HAVE_LIBPTHREAD) || defined(WITH_THREAD)
      if (pcTest == NULL || xmlStrEqual(BAD_CAST pcTest, BAD_CAST "thread")) {
	  iErrorCode += cxpThreadPoolTest(pccTest);
      }
#endif

#ifdef HAVE_LIBARCHIVE
      if (pcTest == NULL || xmlStrEqual(BAD_CAST pcTest, BAD_CAST "archive")) {
	iErrorCode += arcTest(pccTest);
      }
#endif

#ifdef HAVE_LIBSQLITE3
      if (pcTest == NULL || xmlStrEqual(BAD_CAST pcTest, BAD_CAST "sql")) {
	cxpContextPtr pccT = NULL;
	xmlNodePtr pndXml;
	xmlNodePtr pndMake;

	pccT = cxpCtxtDup(pccTest);

	if (pccT) {
	  iErrorCode += dbTest();
	  iErrorCode += dbCxpTest(pccT);

	  cxpCtxtIncrExitCode(pccTest, cxpCtxtGetExitCode(pccT));
	  cxpCtxtFree(pccT);
	}
      }
#endif

      if (pcTest == NULL || xmlStrEqual(BAD_CAST pcTest, BAD_CAST "dir")) {
	cxpContextPtr pccT = NULL;
	xmlNodePtr pndXml;
	xmlNodePtr pndMake;

	pccT = cxpCtxtDup(pccTest);
	if (pccT) {
	  iErrorCode += dirTest(pccT);

	  cxpCtxtIncrExitCode(pccTest, cxpCtxtGetExitCode(pccT));
	  cxpCtxtFree(pccT);
	}
      }

#ifdef HAVE_PETRINET
      if (pcTest == NULL || xmlStrEqual(BAD_CAST pcTest, BAD_CAST "petrinet")) {
	cxpContextPtr pccT = NULL;
	xmlNodePtr pndXml;
	xmlNodePtr pndMake;

	pccT = cxpCtxtDup(pccTest);
	if (pccT) {
	  iErrorCode += petrinetTest(pccT);

	  cxpCtxtIncrExitCode(pccTest,cxpCtxtGetExitCode(pccT));
	  cxpCtxtFree(pccT);
	}
      }
#endif

#ifdef HAVE_PIE
      if (pcTest == NULL || xmlStrEqual(BAD_CAST pcTest, BAD_CAST "pie")) {
	cxpContextPtr pccT = NULL;
	xmlNodePtr pndXml;
	xmlNodePtr pndMake;

	pccT = cxpCtxtDup(pccTest);
	if (pccT) {
	  iErrorCode += pieElementTest();
	  iErrorCode += pieTextTagsTest();
	  iErrorCode += pieTextBlocksTest();
	  iErrorCode += pieTextTest(pccT);
	  iErrorCode += pieTimezoneTest();
	  iErrorCode += ceTest();
	  iErrorCode += pieCalendarTest(pccT);
	  //iErrorCode += icsTest();
	  //iErrorCode += vcfTest();

	  cxpCtxtIncrExitCode(pccTest,cxpCtxtGetExitCode(pccT));
	  cxpCtxtFree(pccT);
	}
      }
#endif

      if (pcTest == NULL || xmlStrEqual(BAD_CAST pcTest, BAD_CAST "parser")) {
#ifdef HAVE_LIBID3TAG
	iErrorCode += audioTest();
#endif
#if defined(HAVE_LIBEXIF) || defined(HAVE_LIBMAGICK)
	iErrorCode += imageTest(pccTest);
#endif
#ifdef HAVE_RP
	iErrorCode += rpTest(pccTest);
#endif
#ifdef HAVE_JSON
	iErrorCode += jsonTest(pccTest);
#endif
#ifdef HAVE_JS
	iErrorCode += scriptTest(pccTest);
#endif
      }
    }

    iExit = cxpCtxtGetExitCode(pccTest);
    cxpCtxtLogPrint(pccTest,1,"Test error code = %i\n", iErrorCode + iExit);
    cxpCtxtFree(pccTest);
    
    exit(iErrorCode + iExit);
  }

  exit(EXIT_FAILURE);
}
