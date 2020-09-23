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
#include "plain_text.h"
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

static void
cxpCtxtCliPrintHelp(cxpContextPtr pccArg);

static BOOL_T
cxpCtxtCliAddXsl(xmlNodePtr pndArgParent, cxpContextPtr pccArg);

/*! creates a new cxproc Context for a command line request

\param argc -- count of program arguments
\param **argv -- pointer to array of program arguments
\param **envp -- pointer to NULL terminated array of program environment
\return pointer to new allocated context
*/
cxpContextPtr
cxpCtxtCliNew(int argc, char *argv[], char *envp[])
{
  cxpContextPtr pccResult = NULL;

  pccResult = cxpCtxtNew();
  if (pccResult) {
    resNodePtr prnNew;

    pccResult->iCountArgv = argc;
    pccResult->ppcArgv = argv;

    cxpCtxtEnvDup(pccResult,envp);
    cxpCtxtLogSetLevel(pccResult,2);

    cxpCtxtLogPrint(pccResult,2,
      "**************************************************************\n"
      "Based on %s %s-%s",CXP_VERSION_URL,CXP_VERSION_STR,CXP_REVISION_STR);

    /*! set initial context locator to current working directory (startup directory)
    */
    prnNew = resNodeDirNew(NULL);
    cxpCtxtLocationSet(pccResult, prnNew);
    resNodeFree(prnNew);

    cxpCtxtEncSetDefaults(pccResult);
    cxpCtxtEncSetPlain(pccResult, BAD_CAST "ISO-8859-1");

#if 0
    /*! */
    cxpCtxtSetReadonly(pccResult,
      cxpCtxtEnvGetBoolByName(pccResult,
      BAD_CAST "CXP_READONLY",
      FALSE));
#endif

#if 0
    fsSetSearch(pucArgSearch);
#endif
  }
  return pccResult;
} /* end of cxpCtxtCliNew() */


/*! 

\param pccArg -- pointer to context to duplicate
\return 

 "Common Dialog Box Library Overviews" https://msdn.microsoft.com/en-us/library/windows/desktop/ff468806(v=vs.85).aspx

 https://msdn.microsoft.com/en-us/library/windows/desktop/ms645505(v=vs.85).aspx

*/
BOOL_T
cxpCtxtCliMessageBox(xmlChar *pucArgMsg, cxpContextPtr pccArg)
{
  BOOL_T fResult = TRUE;

  if (pucArgMsg != NULL) {
#ifdef _MSC_VER
    int iresult = MessageBox(
      NULL,
      (LPCSTR)pucArgMsg,
      (LPCSTR)"Cxproc Message",
      MB_ICONQUESTION | MB_OKCANCEL | MB_DEFBUTTON1
      );
    fResult = (iresult != IDABORT && iresult != IDCANCEL);
#else
    cxpCtxtLogPrint(pccArg,1, "No dialog '%s' possible", pucArgMsg);
#endif
  }
  return fResult;
} /* end of cxpCtxtCliMessageBox() */


/*! Prints a short usage message for command line interface.

\param pccArg -- pointer to runtime context
*/
void
cxpCtxtCliPrintHelp(cxpContextPtr pccArg)
{
  /*!  */
  xmlChar *pucExecutable;

  pucExecutable = resPathGetBasename(cxpCtxtCliGetValue(pccArg, 0));
  assert(pucExecutable);

  cxpCtxtLogPrint(pccArg, 1,"\n%s - " CXP_VER_FILE_DESCRIPTION_STR " " CXP_VER_FILE_VERSION_STR "\n\n", pucExecutable);

  cxpCtxtLogPrint(pccArg, 1,"Copyright " CXP_VER_COPYRIGHT_STR "\n\n");

  cxpCtxtLogPrint(pccArg, 1,GPL_LICENSE_STR "\n\n");

  cxpCtxtLogPrint(pccArg, 1,"Usage:\n");

  cxpCtxtLogPrint(pccArg, 1,"      plain mode: %s -f [pie2html.xsl] < test.txt\n", pucExecutable);

  cxpCtxtLogPrint(pccArg, 1,"        dir mode: %s .\n", pucExecutable);

  cxpCtxtLogPrint(pccArg, 1,"   calendar mode: %s 2007[..2008]\n", pucExecutable);

  cxpCtxtLogPrint(pccArg, 1,"     config mode: %s config.cxp\n", pucExecutable);

  cxpCtxtLogPrint(pccArg, 1,"                  %s -x '<xml name=\"-\"><info/></xml>'\n", pucExecutable);

  cxpCtxtLogPrint(pccArg, 1,"    archive mode: %s test.iso\n", pucExecutable);

  cxpCtxtLogPrint(pccArg, 1,"                  %s test.zip [/tmp|c:\\temp]\n", pucExecutable);

#ifdef HAVE_JS
  cxpCtxtLogPrint(pccArg, 1,"     script mode: %s test.js\n", pucExecutable);
#endif

#ifdef HAVE_LIBSQLITE3
  cxpCtxtLogPrint(pccArg, 1,"     sqlite mode: %s test.db3\n", pucExecutable);

  cxpCtxtLogPrint(pccArg, 1,"                  %s test.db3 db2xyz.xsl xyz2abc.xsl\n", pucExecutable);

  cxpCtxtLogPrint(pccArg, 1,"                  %s test.db3 /tmp// test/abc.txt ~/Documents//\n", pucExecutable);

  cxpCtxtLogPrint(pccArg, 1,"                  %s test.db3 'SELECT * FROM directory;'\n", pucExecutable);
#endif

  cxpCtxtLogPrint(pccArg, 1,"       test mode: %s -t [config.cxp|2008|c:\\temp]\n", pucExecutable);

  cxpCtxtLogPrint(pccArg, 1,"      pause mode: %s -p config.cxp\n", pucExecutable);

  cxpCtxtLogPrint(pccArg, 1," validation mode: %s -v config-a.cxp [config-b.cxp]\n", pucExecutable);

  cxpCtxtLogPrint(pccArg, 1,"    environ mode: %s -e\n", pucExecutable);

  cxpCtxtLogPrint(pccArg, 1,"      index mode: %s -i [/tmp|c:\\temp]\n", pucExecutable);

  cxpCtxtLogPrint(pccArg, 1,"                  %s -d [/tmp|c:\\temp]\n", pucExecutable);

  cxpCtxtLogPrint(pccArg, 1,"     search mode: %s -s '.*listfil.*' [/tmp|c:\\temp]\n", pucExecutable);

  cxpCtxtLogPrint(pccArg, 1,"\n");

  cxpCtxtLogPrint(pccArg, 1,"Named arguments are: %s cxp=info name=ABC\n\n", pucExecutable);

  cxpCtxtLogPrint(pccArg, 1," CXP_PATH=/usr/local/share/cxproc\n\n");

  cxpCtxtLogPrint(pccArg, 1," CXP_ROOT=/home/developer/www\n\n");

  cxpCtxtLogPrint(pccArg, 1," CXP_DATE=20120323\n\n");

  cxpCtxtLogPrint(pccArg, 1," CXP_LOG=2\n\n");

  cxpCtxtLogPrint(pccArg, 1," CXP_DIR_VERBOSITY=3\n\n");

  cxpCtxtLogPrint(pccArg, 1,"Documentations, Copyrights, Examples see %s\n\n", CXP_VER_URL);

} /* end of cxpCtxtCliPrintHelp() */


/*! process the main() arguments into a config DOM

\param pccArg -- pointer to context 
\return a config DOM according to program arguments
*/
BOOL_T
cxpCtxtCliParse(cxpContextPtr pccArg)
{
  BOOL_T fResult = TRUE;
  resNodePtr prnExecutable;
  xmlNodePtr pndDir;
  xmlNodePtr pndMake;
  xmlNodePtr pndPie;
  xmlNodePtr pndImport;
  xmlNodePtr pndXml;
  xmlNodePtr pndXsl;
  xmlChar *pucT;
  xmlChar *pucLevelDirVerbosity;
  int iArgCount;
  xmlNsPtr pnsCxp;

  if (pccArg == NULL) {
    fResult = FALSE;
  }
  else {
    if (pccArg->pdocContextNode) {
      xmlFreeDoc(pccArg->pdocContextNode); /* release the old DOM */
    }

    iArgCount = cxpCtxtCliGetCount(pccArg);
    pucT = cxpCtxtCliGetValue(pccArg,0);
    prnExecutable = resNodeDirNew(pucT);
    xmlFree(pucT);

    cxpCtxtLogPrint(pccArg, 1, "Its me '%s' %s", resNodeGetNameNormalized(prnExecutable), CXP_VER_FILE_VERSION_STR);

    pccArg->pdocContextNode = xmlNewDoc(BAD_CAST "1.0");
    if (pccArg->pdocContextNode == NULL) {
      cxpCtxtLogPutsExit(pccArg,1,"Mem cxpCtxtCliParse()");
    }

    pndMake = xmlNewDocNode(pccArg->pdocContextNode, NULL, NAME_MAKE, NULL);
    xmlDocSetRootElement(pccArg->pdocContextNode, pndMake);
    pccArg->pdocContextNode->encoding = xmlStrdup(BAD_CAST "UTF-8"); /* according to conversion in ParseImportNodePlainContent() */

    xmlSetProp(pndMake, BAD_CAST "log", BAD_CAST "2");

    pnsCxp = xmlNewNs(pndMake, BAD_CAST CXP_VER_URL, BAD_CAST "cxp");
    xmlSetNs(pndMake,pnsCxp);

    if ((pucLevelDirVerbosity = cxpCtxtEnvGetValueByName(pccArg,BAD_CAST "CXP_DIR_VERBOSITY")) != NULL
	&& (xmlStrEqual(pucLevelDirVerbosity, BAD_CAST "1")
	    || xmlStrEqual(pucLevelDirVerbosity, BAD_CAST "3")
	    || xmlStrEqual(pucLevelDirVerbosity, BAD_CAST "4"))) {
      /* OK */
    }
    else {
      pucLevelDirVerbosity = xmlStrdup(BAD_CAST "2");
    }

    /*! build internal DOM
     */
    if (iArgCount < 2) {
      //cxpCtxtCliPrintHelp(pccArg);
    }
    else {
      xmlChar* pucRelease;
      xmlChar *pucArgvFirst;
      xmlNodePtr pndPause = NULL;
      int j = 1;

      if ((pucArgvFirst = cxpCtxtCliGetValue(pccArg, 1)) == NULL) {
	fResult = FALSE;
	return fResult;
      }
      else if (xmlStrEqual(pucArgvFirst, BAD_CAST"-?")) {
	xmlChar *pucArgvNext = cxpCtxtCliGetValue(pccArg,2);

	if (xmlStrstr(pucArgvNext, BAD_CAST ".cxp")) {
	  xmlNodePtr pndT;

	  pndT = xmlNewChild(pndMake, NULL, NAME_XML, NULL);
	  xmlSetProp(pndT, BAD_CAST "name", BAD_CAST "-");
	  pndXml = xmlNewChild(pndT, NULL, NAME_XML, NULL);
	  xmlSetProp(pndXml, BAD_CAST "name", pucArgvNext);
	  xmlSetProp(pndXml, BAD_CAST "search", BAD_CAST"yes");
	  pndXsl = xmlNewChild(pndT, NULL, NAME_XPATH, NULL);
	  xmlSetProp(pndXsl, BAD_CAST "select", BAD_CAST"//descendant::*[local-name() = 'description']");
	  /*!\todo handle result as plain text */
	  //cxpCtxtLogPrintDoc(pccArg, 1, "XML arg result", pccArg->pdocContextNode);

	  xmlFree(pucArgvFirst);
	  xmlFree(pucArgvNext);
	  return fResult;
	}
	else {
	  xmlFreeDoc(pccArg->pdocContextNode);
	  pccArg->pdocContextNode = NULL;
	}
      }
      else if (xmlStrEqual(pucArgvFirst, BAD_CAST"-p")) {
	pndPause = xmlNewNode(NULL, NAME_SYSTEM);
	xmlSetProp(pndPause, BAD_CAST "message", BAD_CAST "PAUSE, Type <ENTER> to continue");
	xmlSetProp(pndPause, BAD_CAST "pause", BAD_CAST "yes");
	j++;
	xmlFree(pucArgvFirst);
	pucArgvFirst = cxpCtxtCliGetValue(pccArg,j);
      }
      else if (xmlStrEqual(pucArgvFirst, BAD_CAST"-t")) {
	fResult = FALSE;
	j++;
	xmlFree(pucArgvFirst);
	pucArgvFirst = cxpCtxtCliGetValue(pccArg,j);
      }

      if (pccArg->pdocContextNode == NULL) {
	/*! write help text
	 */
	cxpCtxtCliPrintHelp(pccArg);
      }
      else if (atoi((char*)pucArgvFirst) > 1900) {
	/*
	  argv[1] is a valid year number, calendar
	*/
	int i;
	xmlChar* pucYear = NULL;

	for (i = j; (pucRelease = cxpCtxtCliGetValue(pccArg, i)); i++) {

	  if (atoi((char*)pucRelease) > 1900) {
	    if (pucYear) {
	      pucYear = xmlStrcat(pucYear, BAD_CAST",");
	      pucYear = xmlStrcat(pucYear, pucRelease);
	      xmlFree(pucRelease);
	    }
	    else {
	      pucYear = pucRelease;
	    }
	  }
	  else {
	    xmlFree(pucRelease);
	    cxpCtxtLogPrint(pccArg, 2, "Ignoring invalid value '%s'", pucRelease);
	    /*  */
	  }
	}

	if (pucYear) {
	  xmlNodePtr pndCalendar;

	  pndXml = xmlNewChild(pndMake, NULL, NAME_XML, NULL);
	  xmlSetProp(pndXml, BAD_CAST "name", BAD_CAST "-");
	  pndCalendar = xmlNewChild(pndXml, NULL, NAME_CALENDAR, NULL);
	  domSetPropEat(pndCalendar, BAD_CAST "year", pucYear);
	  cxpCtxtCliAddXsl(pndXml, pccArg);

	  if (pndPause) {
	    xmlAddChildList(pndMake, pndPause);
	  }
	}
      }
      else if (xmlStrEqual(pucArgvFirst, BAD_CAST"-e")) {
	/*! print program environment
	 */
	pndXml = xmlNewChild(pndMake, NULL, NAME_XML, NULL);
	xmlSetProp(pndXml, BAD_CAST "name", BAD_CAST "-");
	xmlNewChild(pndXml, NULL, NAME_INFO, NULL);
      }
      else if (xmlStrEqual(pucArgvFirst, BAD_CAST"-x")) {
	/*! process following string as CXP
	 */
	xmlChar *pucCxpString;

	if ((pucCxpString = cxpCtxtCliGetValue(pccArg,j + 1))) {
	  if (xmlStrlen(pucCxpString) > 5) {
	    xmlDocPtr pdocT;

	    pdocT = xmlParseMemory((const char*)pucCxpString, xmlStrlen(pucCxpString));
	    if (pdocT) {
	      xmlNodePtr pndRoot;

	      pndRoot = xmlDocGetRootElement(pdocT);
	      if (pndRoot) {
		xmlNodePtr pndRootCopy;

		pndRootCopy = xmlCopyNode(pndRoot, 1);
		if (pndRootCopy) {
		  xmlAddChild(pndMake, pndRootCopy);
		  //domSaveFileXml("-",pndMake->doc);
		}
	      }
	      xmlFreeDoc(pdocT);
	    }
	    else {
	      cxpCtxtLogPrint(pccArg, 1, "Cant parse command line XML '%s'", pucCxpString);
	    }
	  }
	  xmlFree(pucCxpString);
	}
      }
      else if (xmlStrEqual(pucArgvFirst, BAD_CAST"-i") || xmlStrEqual(pucArgvFirst, BAD_CAST"-d")) {
	/*! index all directories non-recusive
	 */
	xmlNodePtr pndT;
	xmlChar *pucDir;

	pndT = xmlNewChild(pndMake, NULL, NAME_XML, NULL);
	pndT = xmlNewChild(pndT, NULL, NAME_DIR, NULL);
	xmlSetProp(pndT, BAD_CAST "index", BAD_CAST (xmlStrEqual(pucArgvFirst, BAD_CAST"-d") ? "delete" : "write"));
	xmlSetProp(pndT, BAD_CAST "verbosity", pucLevelDirVerbosity);
	if ((pucDir = cxpCtxtCliGetValue(pccArg,j + 1))) {
	  if (xmlStrlen(pucDir) > 0) {
	    //cxpCtxtLogPrint(pccArg, 2, "Indexing '%s'", resPathNormalize(pucDir));
	    xmlSetProp(pndT, BAD_CAST "name", resPathNormalize(pucDir));
	    //xmlSetProp(pndT, BAD_CAST "depth", BAD_CAST(resNodeIsRecursive(pucDir) ? "99" : "1"));
	  }
	  xmlFree(pucDir);
	}
      }
      else if (xmlStrEqual(pucArgvFirst, BAD_CAST"-s")) {
	/*! search for file
	 */
	xmlChar *pucPattern;

	if ((pucPattern = cxpCtxtCliGetValue(pccArg,j + 1))) {
	  xmlNodePtr pndPlain;

	  j += 2;
	  pndPlain = xmlNewChild(pndMake, NULL, NAME_PLAIN, NULL);
	  xmlSetProp(pndPlain, BAD_CAST "name", BAD_CAST "-");

	  pndXml = xmlNewChild(pndPlain, NULL, NAME_XML, NULL);
	  pndXml = xmlNewChild(pndXml, NULL, NAME_DIR, NULL);
	  if (STR_IS_NOT_EMPTY(pucPattern)) {
	    xmlSetProp(pndXml, BAD_CAST "verbosity", pucLevelDirVerbosity);
	    xmlSetProp(pndXml, BAD_CAST "imatch", pucPattern);
	    xmlSetProp(pndXml, BAD_CAST "index", BAD_CAST"skip");

	    if (j < iArgCount) {
	      /* there are names of directories to search */
	      int i;
	      for (i = j; i < iArgCount; i++) {
		pucRelease = cxpCtxtCliGetValue(pccArg,i);
		if ((pucT = resPathNormalize(pucRelease))) {
		  pndDir = xmlNewChild(pndXml, NULL, NAME_DIR, NULL);
		  domSetPropEat(pndDir, BAD_CAST "name", pucT);
		  xmlSetProp(pndDir, BAD_CAST "depth", BAD_CAST"99");
		}
		else {
		  cxpCtxtLogPrint(pccArg, 2, "Ignoring '%s'", pucRelease);
		}
		xmlFree(pucRelease);
	      }
	    }
	    else {
	      /* there is no name of directories to search, use internal search paths */
	      resNodePtr prnT;

	      pndDir = xmlNewChild(pndXml, NULL, NAME_DIR, NULL);
	      /* handle multiple search dir names */
	      for (prnT = cxpCtxtSearchGet(pccArg); prnT; prnT = resNodeGetNext(prnT)) {
		xmlSetProp(pndDir, BAD_CAST "name", resNodeGetNameNormalized(prnT));
		xmlSetProp(pndDir, BAD_CAST "depth", BAD_CAST(resNodeIsRecursive(prnT) ? "99" : "1"));
	      }
	    }
	    pndXsl = xmlNewChild(pndPlain, NULL, NAME_XSL, NULL);
	    xmlSetProp(pndXsl, BAD_CAST "name", BAD_CAST "dir/ListFiles.xsl");
	  }
	  xmlFree(pucPattern);
	}
      }
      else if (xmlStrEqual(pucArgvFirst, BAD_CAST"-v")) {
	/*! search for files an validate them
	 */
	int i;

	xmlSetProp(pndMake, BAD_CAST "log", BAD_CAST "1");
	for (i = j + 1; i < iArgCount; i++) {
	  pucRelease = cxpCtxtCliGetValue(pccArg,i);
	  if (pucRelease) {
	    resNodePtr prnT;

	    prnT = resNodeConcatNew(cxpCtxtLocationGetStr(pccArg), pucRelease);
	    if (prnT) {
	      resNodeReadStatus(prnT);
	      resNodeResetMimeType(prnT);
	      if (resNodeGetMimeType(prnT) == MIME_APPLICATION_CXP_XML) {
		//pndPause = xmlNewChild(pndMake, NULL, NAME_SYSTEM, NULL);
		//xmlSetProp(pndPause, BAD_CAST "message", BAD_CAST resNodeGetNameNormalized(prnT));
		pndXml = xmlNewChild(pndMake, NULL, NAME_XML, NULL);
		xmlSetProp(pndXml, BAD_CAST "eval", BAD_CAST "no");
		if (resNodeIsReadable(prnT)) {
		  xmlSetProp(pndXml, BAD_CAST "name", BAD_CAST resNodeGetNameNormalized(prnT));
		}
		else {
		  xmlSetProp(pndXml, BAD_CAST "name", BAD_CAST resNodeGetNameBase(prnT));
		  xmlSetProp(pndXml, BAD_CAST "search", BAD_CAST "yes");
		}
	      }
	      else {
		cxpCtxtLogPrint(pccArg, 2, "Ignoring '%s'", pucRelease);
	      }
	      resNodeFree(prnT);
	    }
	    xmlFree(pucRelease);
	  }
	}
      }
#ifdef HAVE_PIE
      else if (xmlStrEqual(pucArgvFirst, BAD_CAST"-f")) {
	/*
	  Plain text filter for stdin and XSL names in argv[]
	*/
	pndXml = xmlNewChild(pndMake, NULL, NAME_XML, NULL);
	xmlSetProp(pndXml, BAD_CAST "name", BAD_CAST "-");
	xmlSetProp(pndXml, BAD_CAST "schema", BAD_CAST "pie.rng");

	pndPie = xmlNewChild(pndXml, NULL, NAME_PIE, NULL);
	xmlSetProp(pndPie, BAD_CAST "url", BAD_CAST "yes");
	xmlSetProp(pndPie, BAD_CAST "figure", BAD_CAST "yes");
	/* there is no file context when reading from stdin */
	xmlSetProp(pndPie, BAD_CAST "import", BAD_CAST "yes");

	pndImport = xmlNewChild(pndPie, NULL, NAME_PIE_IMPORT, NULL);
	xmlSetProp(pndImport, BAD_CAST "name", BAD_CAST "-");

	cxpCtxtCliAddXsl(pndXml, pccArg);
      }
#endif
      else {
	resNodePtr prnT;
	resNodePtr prnContent;

	if (resPathIsURL(pucArgvFirst)) {
	  prnT = resNodeDirNew(pucArgvFirst);
	}
	else {
	  prnT = resNodeConcatNew(cxpCtxtLocationGetStr(pccArg), pucArgvFirst);
	}
	
	if (prnT) {
	  int iMimeType;

	  resNodeReadStatus(prnT);
	  prnContent = resNodeGetLastDescendant(prnT);
	  resNodeResetMimeType(prnContent);
	  iMimeType = resNodeGetMimeType(prnContent);

	  if (resNodeIsDir(prnContent)) {
	    /*!
	      argv[1] is a valid name of a directory
	    */
	    int i;
	    xmlNodePtr pndPlain;

	    pndPlain = xmlNewChild(pndMake, NULL, NAME_PLAIN, NULL);
	    xmlSetProp(pndPlain, BAD_CAST "name", BAD_CAST "-");
	    pndXml = xmlNewChild(pndPlain, NULL, NAME_XML, NULL);
	    pndDir = xmlNewChild(pndXml, NULL, NAME_DIR, NULL);
	    xmlSetProp(pndDir, BAD_CAST "verbosity", pucLevelDirVerbosity);
	    xmlSetProp(pndDir, BAD_CAST "index", BAD_CAST "skip"); /* skip index files in command line mode */
	    xmlSetProp(pndDir, BAD_CAST "hidden", BAD_CAST "no"); /* skip hidden files in command line mode */

	    for (i = j; (pucRelease = cxpCtxtCliGetValue(pccArg,i)); i++) {
	      resNodePtr prnArgValue;

	      prnArgValue = resNodeConcatNew(cxpCtxtLocationGetStr(pccArg), pucRelease);
	      resNodeReadStatus(prnArgValue);
	      if (resNodeIsDir(prnArgValue)) {
		cxpCtxtLogPrint(pccArg, 2, "Try DIR '%s'", resNodeGetNameNormalized(prnArgValue));
		pndPie = xmlNewChild(pndDir, NULL, NAME_DIR, NULL);
		xmlSetProp(pndPie, BAD_CAST "name", resNodeGetNameNormalized(prnArgValue));
		xmlSetProp(pndPie, BAD_CAST "depth", BAD_CAST(resNodeIsRecursive(prnArgValue) ? "999" : "1"));
	      }
	      else {
		/* ignoring */
	      }
	      resNodeFree(prnArgValue);
	      xmlFree(pucRelease);
	    }
	    cxpCtxtCliAddXsl(pndPlain, pccArg);
	  }
	  else if (resNodeIsURL(prnContent)) {
	    /*
	      argv[1] is an URL
	    */
	    xmlNodePtr pndT;

	    pndXml = xmlNewChild(pndMake, NULL, NAME_XML, NULL);
	    xmlSetProp(pndXml, BAD_CAST "name", BAD_CAST"-");
	    pndT = xmlNewChild(pndXml, NULL, NAME_FILE, NULL);
	    xmlSetProp(pndT, BAD_CAST "name", resNodeGetNameNormalized(prnContent));
	    cxpCtxtCliAddXsl(pndXml, pccArg);
	  }
	  else if (iMimeType == MIME_APPLICATION_CXP_XML) {
	    /*
	      argv[1] is a name of a config XML, existing or not!
	    */
	    pndXml = xmlNewChild(pndMake, NULL, NAME_XML, NULL);
	    xmlSetProp(pndXml, BAD_CAST "eval", BAD_CAST "yes");
	    xmlSetProp(pndXml, BAD_CAST "schema", BAD_CAST "cxp.rng");

	    if (resNodeIsFile(prnContent) && resNodeIsExist(prnContent)) {
	      xmlSetProp(pndXml, BAD_CAST "name", resNodeGetNameNormalized(prnContent));
	    }
	    else {
	      xmlSetProp(pndXml, BAD_CAST "name", resNodeGetNameBase(prnContent));
	      xmlSetProp(pndXml, BAD_CAST "search", BAD_CAST "yes");
	      cxpCtxtLogPrint(pccArg, 2, "Search later for '%s'", resNodeGetNameBase(prnContent));
	    }
	  }
#ifdef HAVE_LIBARCHIVE
	  else if (iMimeType == MIME_APPLICATION_ZIP
		   || iMimeType == MIME_APPLICATION_X_TAR
		   || iMimeType == MIME_APPLICATION_X_ISO9660_IMAGE) {
	    /*
	      argv[1] is a name of an archive file, existing or not!
	    */
	    resNodePtr prnArgNextValue = NULL;
	    xmlNodePtr pndZip;
	    xmlNodePtr pndChild;

	    pucRelease = cxpCtxtCliGetValue(pccArg,j + 1);
	    if (pucRelease) {
	      prnArgNextValue = resNodeConcatNew(cxpCtxtLocationGetStr(pccArg), pucRelease);
	      if (resNodeIsDir(prnArgNextValue) || resNodeIsFile(prnArgNextValue)) {
		int i;

		pndZip = xmlNewChild(pndMake, NULL, NAME_ZIP, NULL); /*!\todo derive node name from resNodeGetExtension(prnContent) */
		xmlSetProp(pndZip, BAD_CAST "name", resNodeGetNameNormalized(prnContent));
		xmlSetProp(pndZip, BAD_CAST "append", BAD_CAST"yes");
		xmlSetProp(pndZip, BAD_CAST "write", BAD_CAST"yes");
		pndChild = xmlNewChild(pndZip, NULL, NAME_XML, NULL);
		pndDir = xmlNewChild(pndChild, NULL, NAME_DIR, NULL);
		xmlSetProp(pndDir, BAD_CAST "verbosity", pucLevelDirVerbosity);
		xmlSetProp(pndDir, BAD_CAST "index", BAD_CAST "skip"); /* skip index files in command line mode */
		xmlSetProp(pndDir, BAD_CAST "hidden", BAD_CAST "no"); /* skip hidden files in command line mode */

		for (i = j + 1; (pucRelease = cxpCtxtCliGetValue(pccArg,i)); i++) {
		  BOOL_T fRecursion;
		  resNodePtr prnArgValue;

		  fRecursion = resPathIsDirRecursive(pucRelease);
		  if (fRecursion) {
		    resPathCutTrailingChars(pucRelease);
		  }

		  prnArgValue = resNodeConcatNew(cxpCtxtLocationGetStr(pccArg), pucRelease);
		  if (resNodeIsFile(prnArgValue)) {
		    if (iMimeType == MIME_TEXT_XSL) {
		      /* ignoring XSL files */
		    }
		    else {
		      pndPie = xmlNewChild(pndDir, NULL, NAME_FILE, NULL);
		      xmlSetProp(pndPie, BAD_CAST "name", resNodeGetNameNormalized(prnArgValue));
		    }
		  }
		  else if (resNodeIsDir(prnArgValue)) {
		    cxpCtxtLogPrint(pccArg, 2, "Try DIR '%s'", resNodeGetNameNormalized(prnArgValue));
		    pndPie = xmlNewChild(pndDir, NULL, NAME_DIR, NULL);
		    xmlSetProp(pndPie, BAD_CAST "depth", BAD_CAST(fRecursion ? "999" : "1"));
		    xmlSetProp(pndPie, BAD_CAST "name", resNodeGetNameNormalized(prnArgValue));
		    xmlSetProp(pndZip, BAD_CAST "context", resNodeGetNameBaseDir(prnArgValue));
		  }
		  else {
		    cxpCtxtLogPrint(pccArg, 2, "Ignoring non-existing directory '%s'", resNodeGetNameNormalized(prnArgValue));
		    /*  */
		  }
		  resNodeFree(prnArgValue);
		  xmlFree(pucRelease);
		}
		cxpCtxtCliAddXsl(pndChild, pccArg);
	      }
	      else {
	      }
	      resNodeFree(prnArgNextValue);
	      xmlFree(pucRelease);
	    }
	    else {
	      pndXml = xmlNewChild(pndMake, NULL, NAME_XML, NULL);
	      xmlSetProp(pndXml, BAD_CAST "name", BAD_CAST "-");

	      pndZip = xmlNewChild(pndXml, NULL, NAME_FILE, NULL);
	      xmlSetProp(pndZip, BAD_CAST "name", resNodeGetNameNormalized(prnContent));
	      xmlSetProp(pndZip, BAD_CAST "verbosity", pucLevelDirVerbosity);
	    }
	  }
#endif
#ifdef HAVE_LIBSQLITE3
	  else if (iMimeType == MIME_APPLICATION_X_SQLITE3) {
	    /*
	      argv[1] is a name of an SQLite file, existing or not!
	    */
	    resNodePtr prnArgNextValue = NULL;

	    pucRelease = cxpCtxtCliGetValue(pccArg,j + 1);
	    if (pucRelease) {
	      prnArgNextValue = resNodeConcatNew(cxpCtxtLocationGetStr(pccArg), pucRelease);
	      xmlFree(pucRelease);
	    }

	    if (resNodeIsDir(prnArgNextValue) || resNodeIsFile(prnArgNextValue)) {
	      int i;
	      xmlNodePtr pndDatabase;

	      pndDatabase = xmlNewChild(pndMake, NULL, NAME_DB, NULL);
	      xmlSetProp(pndDatabase, BAD_CAST "name", resNodeGetNameNormalized(prnContent));
	      xmlSetProp(pndDatabase, BAD_CAST "append", BAD_CAST"yes");
	      xmlSetProp(pndDatabase, BAD_CAST "write", BAD_CAST"yes");
	      pndDir = xmlNewChild(pndDatabase, NULL, NAME_DIR, NULL);
	      xmlSetProp(pndDir, BAD_CAST "verbosity", pucLevelDirVerbosity);
	      xmlSetProp(pndDir, BAD_CAST "index", BAD_CAST "skip"); /* skip index files in command line mode */
	      xmlSetProp(pndDir, BAD_CAST "hidden", BAD_CAST "no"); /* skip hidden files in command line mode */

	      for (i = j + 1; (pucRelease = cxpCtxtCliGetValue(pccArg,i)); i++) {
		BOOL_T fRecursion;
		resNodePtr prnArgValue;

		fRecursion = resPathIsDirRecursive(pucRelease);
		if (fRecursion) {
		  resPathCutTrailingChars(pucRelease);
		}

		prnArgValue = resNodeConcatNew(cxpCtxtLocationGetStr(pccArg), pucRelease);
		if (resNodeIsFile(prnArgValue)) {
		  if (iMimeType == MIME_TEXT_XSL) {
		    /* ignoring XSL files */
		  }
		  else {
		    pndPie = xmlNewChild(pndDir, NULL, NAME_FILE, NULL);
		    xmlSetProp(pndPie, BAD_CAST "name", resNodeGetNameNormalized(prnArgValue));
		  }
		}
		else if (resNodeIsDir(prnArgValue)) {
		  cxpCtxtLogPrint(pccArg, 2, "Try DIR '%s'", resNodeGetNameNormalized(prnArgValue));
		  pndPie = xmlNewChild(pndDir, NULL, NAME_DIR, NULL);
		  xmlSetProp(pndPie, BAD_CAST "depth", BAD_CAST(fRecursion ? "999" : "1"));
		  xmlSetProp(pndPie, BAD_CAST "name", resNodeGetNameNormalized(prnArgValue));
		}
		else {
		  cxpCtxtLogPrint(pccArg, 2, "Ignoring non-existing directory '%s'", resNodeGetNameNormalized(prnArgValue));
		  /*  */
		}
		resNodeFree(prnArgValue);
		xmlFree(pucRelease);
	      }
	    }
	    else if (resNodeIsFile(prnContent)) {
	      /* existing database file and no directory following */
	      xmlChar *pucQuery;
	      xmlNodePtr pndFile;
	      xmlNodePtr pndXmlChild;

	      pndXml = xmlNewChild(pndMake, NULL, NAME_XML, NULL);
	      xmlSetProp(pndXml, BAD_CAST "name", BAD_CAST"-");
	      pndXmlChild = xmlNewChild(pndXml, NULL, NAME_XML, NULL);

	      pucQuery = cxpCtxtCliGetValue(pccArg,j + 1);
	      if (dbTextReadStatement(pucQuery)) {
		/* the next argument is an usable SQL query */
		pndFile = xmlNewChild(pndXmlChild, NULL, NAME_DB, NULL);
		xmlSetProp(pndFile, BAD_CAST "name", resNodeGetNameNormalized(prnContent));
		pndFile = xmlNewChild(pndXmlChild, NULL, NAME_QUERY, pucQuery);
	      }
	      else {
		/* use next arguments for XSL names only */
		pndFile = xmlNewChild(pndXmlChild, NULL, NAME_FILE, NULL);
		xmlSetProp(pndFile, BAD_CAST "name", resNodeGetNameNormalized(prnContent));
		xmlSetProp(pndFile, BAD_CAST "verbosity", pucLevelDirVerbosity);
		cxpCtxtCliAddXsl(pndXml, pccArg);
	      }
	      xmlFree(pucQuery);
	    }
	    else {
	    }
	    resNodeFree(prnArgNextValue);
	  }
#endif
	  else if (iMimeType == MIME_APPLICATION_MSSHORTCUT || resNodeIsLink(prnT)) {
	    xmlNodePtr pndFile;
	    xmlNodePtr pndXmlChild;

	    pndXml = xmlNewChild(pndMake, NULL, NAME_XML, NULL);
	    xmlSetProp(pndXml, BAD_CAST "name", BAD_CAST"-");
	    pndXmlChild = xmlNewChild(pndXml, NULL, NAME_XML, NULL);
	    pndDir = xmlNewChild(pndXmlChild, NULL, NAME_DIR, NULL);
	    xmlSetProp(pndDir, BAD_CAST "verbosity", pucLevelDirVerbosity);
	    pndFile = xmlNewChild(pndDir, NULL, NAME_FILE, NULL);
	    xmlSetProp(pndFile, BAD_CAST "name", resNodeGetNameNormalized(prnContent));
	  }
	  else if (resNodeIsURL(prnT) || resNodeIsFile(prnT) || resNodeIsArchive(prnContent)) {
	    /*
	      argv[1] exists as file, return detailed information meta an content
	    */
	    xmlNodePtr pndXmlChild;
	    int i;

	    pndXml = xmlNewChild(pndMake, NULL, NAME_XML, NULL);
	    xmlSetProp(pndXml, BAD_CAST "name", BAD_CAST"-");
	    xmlSetProp(pndXml, BAD_CAST "encoding", BAD_CAST"UTF-8");
	    pndXmlChild = xmlNewChild(pndXml, NULL, NAME_XML, NULL);
	    pndDir = xmlNewChild(pndXmlChild, NULL, NAME_DIR, NULL);
	    xmlSetProp(pndDir, BAD_CAST "verbosity", pucLevelDirVerbosity);

	    for (i = j; (pucRelease = cxpCtxtCliGetValue(pccArg, i)); i++) {
	      resNodePtr prnArgValue;
	      xmlNodePtr pndFile;

	      prnArgValue = resNodeConcatNew(cxpCtxtLocationGetStr(pccArg), pucRelease);
	      if (resNodeReadStatus(prnArgValue) == FALSE) {
		cxpCtxtLogPrint(pccArg, 2, "Ignoring non-existing file '%s'", pucRelease);
	      }
	      else if (resNodeIsLink(prnArgValue)) {
		cxpCtxtLogPrint(pccArg, 2, "Ignoring symbolic link '%s'", pucRelease);
	      }
	      else if (resNodeGetMimeType(prnArgValue) == MIME_TEXT_XSL) {
		cxpCtxtLogPrint(pccArg, 2, "Ignoring XSL file '%s'", pucRelease);
	      }
	      else {
		pndFile = xmlNewChild(pndDir, NULL, NAME_FILE, NULL);
		xmlSetProp(pndFile, BAD_CAST "name", resNodeGetNameNormalized(prnArgValue));
	      }
	      resNodeFree(prnArgValue);
	      xmlFree(pucRelease);
	    }
	    cxpCtxtCliAddXsl(pndXml, pccArg);
	  }

	  resNodeListFree(prnT);
	}
      }
      xmlFree(pucArgvFirst);
    }
    xmlFree(pucLevelDirVerbosity);
    resNodeFree(prnExecutable);
  }
  return fResult;
} /* end of cxpCtxtCliParse() */


/*! append all trailing XSL filenames in argv as cxp:xsl elements to

\param pccArg -- pointer to context 
\return TRUE if , FALSE in case of 
*/
BOOL_T
cxpCtxtCliAddXsl(xmlNodePtr pndArgParent, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;
  int k;
  resNodePtr prnT;
  BOOL_T fBreak;

#ifdef DEBUG
  cxpCtxtLogPrint(pccArg,2,"cxpCtxtCliAddXsl(cxpContextPtr pccArg)");
#endif

  /* collect the named XSL
   */
  for (fBreak = FALSE, k = 1; fBreak == FALSE && k < cxpCtxtCliGetCount(pccArg); k++) {
    xmlChar *pucArgvN;
    xmlChar *pucT;

    assert(k < 256);
    pucArgvN = cxpCtxtCliGetValue(pccArg,k);
    if (STR_IS_NOT_EMPTY(pucArgvN)) {
      prnT = resNodeDup(cxpCtxtLocationGet(pccArg), RN_DUP_THIS);
      resNodeReset(prnT, pucArgvN);
      resNodeResetMimeType(prnT);
      if (resNodeGetMimeType(prnT) == MIME_TEXT_XSL) {
	xmlNodePtr pndXsl;
	xmlDocPtr pdocXsl;

	cxpCtxtLogPrint(pccArg, 2, "Adding XSL argument '%s'", resNodeGetNameBase(prnT));
	pndXsl = xmlNewChild(pndArgParent, NULL, NAME_XSL, NULL);
	if (resNodeReadStatus(prnT)) {
	  xmlSetProp(pndXsl, BAD_CAST "name", resNodeGetNameNormalized(prnT));
	}
	else {
	  resNodePtr prnXslFound;

	  prnXslFound = resNodeListFindPath(cxpCtxtSearchGet(pccArg), pucArgvN);
	  if (resNodeReadStatus(prnXslFound)) {
	    xmlSetProp(pndXsl, BAD_CAST "name", resNodeGetNameNormalized(prnXslFound));
	  }
	  else {
	    cxpCtxtLogPrint(pccArg, 1, "XSL '%s' not found", pucArgvN);
	    xmlSetProp(pndXsl, BAD_CAST "name", pucArgvN); /* fallback value */
	  }
	  resNodeFree(prnXslFound);
	}
	pdocXsl = cxpXslRetrieve(pndXsl, pccArg);
	if (pdocXsl) {
	  pucT = domGetXslOutputMethod(pdocXsl);
	  if (pucT == NULL) {
	    cxpCtxtLogPrint(pccArg, 1, "XSL pipeline ends without valid output");
	    fBreak = TRUE;
	  }
	  else if (xmlStrEqual(pucT, BAD_CAST"text")) {
	    xmlNodeSetName(pndArgParent, NAME_PLAIN);
	    cxpCtxtLogPrint(pccArg, 1, "XSL pipeline ends with plain text output");
	    fBreak = TRUE;
	  }
	  else if (xmlStrEqual(pucT, BAD_CAST"html")) {
	    xmlNodeSetName(pndArgParent, NAME_XHTML);
	    cxpCtxtLogPrint(pccArg, 1, "XSL pipeline contains XHTML output");
	  }
	  xmlFreeDoc(pdocXsl);
	}
      }
      else {
	/*!\todo handle additional types (XML, cxp, ...) */
      }
  resNodeFree(prnT);
    }
    xmlFree(pucArgvN);
  }

  return fResult;
} /* end of cxpCtxtCliAddXsl() */


/*! this is the command line mode (not CGI mode!)

\param pccArg -- pointer to context 
\return TRUE if , FALSE in case of 
*/
BOOL_T
cxpCtxtCliProcess(cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;
  xmlChar *pucT;

#ifdef DEBUG
  cxpCtxtLogPrint(pccArg, 2, "cxpCtxtCliProcess(cxpContextPtr pccArg)");
#endif

  pucT = cxpCtxtEnvGetValueByName(pccArg,BAD_CAST "CXP_LOGFILE");
  if (pucT) {
    //cxpCtxtLogSetFile(pccArg, pucT);
    xmlFree(pucT);
  }

  pucT = cxpCtxtEnvGetValueByName(pccArg,BAD_CAST "CXP_LOG");
  if (pucT) {
    if (cxpCtxtLogSetLevelStr(pccArg, pucT) == 0) {
      /* level value from environment */
    }
    xmlFree(pucT);
  }

  if (cxpCtxtSearchSet(pccArg, NULL) == FALSE) {
    cxpCtxtLogPrint(pccArg, 2, "No default search path directory found");
  }

  if (pccArg->pdocContextNode) {
    cxpProcessMakeNode(xmlDocGetRootElement(pccArg->pdocContextNode), pccArg);
  }
  else {
    cxpProcessMakeNode(pccArg->pndContextNode, pccArg);
  }

  return fResult;
} /* end of cxpCtxtCliProcess() */


#ifdef TESTCODE
#include "test/test_cxp_context_cli.c"
#endif

