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

/*! cxproc command line frontend
*/

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

/* 
 */
#include "basics.h"
#include "utils.h"
#include <res_node/res_node_ops.h>
#include <cxp/cxp.h>
#include "plain_text.h"
#include <cxp/cxp_dir.h>
#include <rp/rp.h>
#include "dom.h"

#ifdef HAVE_JS
#include <script/script.h>
#endif

#ifdef HAVE_LIBARCHIVE
#include <archive/cxp_archive.h>
#endif

#ifdef HAVE_PIE
#include <pie/pie_text.h>
#include <pie/pie_calendar.h>
#endif

/*!\todo named args */

/*!\todo use of GNU getopts.h 'cxproc test.cxp --name=abc --type=4' */

/*!\todo command line argument "-" means read configuration XML from stdin ? */

int
main(int argc, char *argv[], char *envp[])
{
  cxpContextPtr pccMain = NULL;
  xmlChar *pucT;
  resNodePtr prnNew = NULL;
  FILE *cxperr = NULL;

  pucT = BAD_CAST getenv("CXP_LOGFILE");
  if ((STR_IS_NOT_EMPTY(pucT) && (prnNew = resNodeDirNew(pucT)))) {
    cxperr = freopen(resNodeGetNameNormalizedNative(prnNew),"w",stderr);
    resNodeFree(prnNew);
  }

  pucT = BAD_CAST getenv("CXP_LOG");
  if ((STR_IS_NOT_EMPTY(pucT))) {
    SetLogLevelStr(pucT);
  }
  else {
    SetLogLevel(3);
  }

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

  /* register for exit() */
  if (atexit(xsltCleanupGlobals) != 0
      || atexit(xmlCleanupParser) != 0
      || atexit(xmlMemoryDump) != 0
      || atexit(domCleanup) != 0
      || atexit(cxpCleanup) != 0
#ifdef HAVE_PIE
      || atexit(pieTextCleanup) != 0
#endif
    ) {
    exit(EXIT_FAILURE);
  }

  xmlInitParser();
  LIBXML_TEST_VERSION

  if (xmlInitMemory()==0) {
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

  pccMain = cxpCtxtCliNew(argc, argv, envp);
  if (pccMain) {
    int iExit = EXIT_SUCCESS;

#ifdef HAVE_JS
    scriptInit(pccMain);
#endif

    //cxpCtxtCacheEnable(pccMain, TRUE);

#if defined(DEBUG)
    cxpCtxtLogSetLevel(pccMain, 3);
#else
    cxpCtxtLogSetLevel(pccMain, 1);
#endif

#ifdef _WIN32
    cxpCtxtEncSetEnv(pccMain, BAD_CAST "ISO-8859-1"); // TODO: via Windows API?
    cxpCtxtEncSetArgv(pccMain, BAD_CAST "ISO-8859-1");
#endif
    cxpCtxtEncSetPlain(pccMain, BAD_CAST "ISO-8859-1"); // TODO: use value of CXP_PLAIN_ENC

    if (cxpCtxtSearchSet(pccMain, NULL) == FALSE) {
      cxpCtxtLogPrint(pccMain,1,"Error cxpCtxtSearchSet()");
    }

    cxpCtxtCacheEnable(pccMain, TRUE);
    
    if (cxpCtxtCliParse(pccMain)) {
      cxpCtxtCliProcess(pccMain);    /*!\todo use thread */
    }
    else {
      xmlChar* pucDump;

      pucDump = cxpCtxtProcessDump(pccMain);
      if (pucDump) {
        puts((const char*)pucDump);
        xmlFree(pucDump);
      }
    }

    iExit = cxpCtxtGetExitCode(pccMain);
    cxpCtxtFree(pccMain);
    if (cxperr) {
      fclose(cxperr);
    }
    exit(iExit);
  }

  exit(EXIT_FAILURE);
}
