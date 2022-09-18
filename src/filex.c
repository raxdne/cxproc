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

/*! command line frontend for resource nodes
*/

/*
 */
#include <libxml/parser.h>
#include <xmlzipio/xmlzipio.h>

/* 
 */
#include "basics.h"
#include "utils.h"
#include <res_node/res_node_io.h>
#include "dom.h"

#ifdef HAVE_PIE
#include <pie/pie_text_blocks.h>
#endif

#ifdef HAVE_LIBARCHIVE
#include <archive/archive.h>
#endif


int
main(int argc, char *argv[], char *envp[])
{
  int e = EXIT_FAILURE;

  SetLogLevel(1);

  /* register for exit() */
  if (atexit(xmlCleanupParser) != 0
      || atexit(xmlMemoryDump) != 0
      || atexit(domCleanup) != 0
#ifdef HAVE_PIE
      || atexit(pieTextBlocksCleanup) != 0
#endif
    ) {
    exit(EXIT_FAILURE);
  }
  /*!\bug add atexit(pieTextTagsCleanup) */

  xmlInitParser();
  LIBXML_TEST_VERSION

  if (xmlInitMemory()==0) {
    exit(EXIT_FAILURE);
  }

  /* code for xmlzipio http://hal.iwr.uni-heidelberg.de/~christi/projects/xmlzipio.html */
  xmlKeepBlanksDefault(0);
  xmlRegisterDefaultInputCallbacks();
  xmlRegisterDefaultOutputCallbacks();
#ifdef HAVE_ZLIB
  /* code for xmlzipio http://hal.iwr.uni-heidelberg.de/~christi/projects/xmlzipio.html */
  xmlZipRegisterInputCallback();
  /* it's importend to xmlzipio after the default handlers, so xmlzipio is asked first. */
  xmlZipRegisterOutputCallback();
#endif

#ifdef _WIN32
  resPathSetNativeEncoding("ISO-8859-1");
#endif

  if (argc > 1) {
    resNodePtr prnNew = NULL;
    int i;

    for (i=1; i<argc; i++) {
      resNodePtr prnArgv;

      /*
      dir/file
      dir/link
      dir/archive
      dir/database
      dir//
      archive/dir/file
      archive/dir/archive/dir/file
      url/file
      url/archive
      url/database
      */

      prnArgv = resNodeDirNew(BAD_CAST argv[i]);
      if (prnArgv) {
	if (prnNew) {
	  resNodeAddSibling(prnNew, prnArgv);
	}
	else {
	  prnNew = prnArgv;
	}
      }
    }

    if (prnNew) {
      xmlDocPtr pdocResult;
      resNodePtr prnT;

      /*! append separate result DOMs to result DOM */
      
      pdocResult = xmlNewDoc(BAD_CAST "1.0");
      if (pdocResult) {
	xmlNodePtr pndRootNew;
      
	pndRootNew = xmlNewNode(NULL, BAD_CAST"dir");
	if (pndRootNew) {
	  xmlDocSetRootElement(pdocResult,pndRootNew);
	
	  for (prnT=prnNew; prnT; prnT = resNodeGetNext(prnT)) {
	    xmlNodePtr pndT;

	    if (resNodeUpdate(prnT, RN_INFO_META | RN_INFO_STRUCT, NULL, NULL)) {

	      if (resNodeIsArchive(prnT)) {
		resNodePtr prnTT;

		prnTT = resNodeGetLastDescendant(prnT);
		if (resNodeIsDirInArchive(prnTT)) {
		  if (resNodeUpdate(prnTT, RN_INFO_META | RN_INFO_STRUCT, NULL, NULL)) {
		    xmlAddChild(pndRootNew, resNodeToDOM(prnTT, RN_INFO_META | RN_INFO_STRUCT));
		  }
		}
		else if (resNodeIsFileInArchive(prnTT)) {
		  if (resNodeIsArchive(prnTT)) {
		    if (resNodeUpdate(prnTT, RN_INFO_META | RN_INFO_STRUCT, NULL, NULL)) {
		      xmlAddChild(pndRootNew, resNodeToDOM(prnTT, RN_INFO_META | RN_INFO_STRUCT));
		    }
		  }
		  else {
		    xmlAddChild(pndRootNew, resNodeToDOM(prnTT, RN_INFO_MAX));
		  }
		}
		else {
		  xmlAddChild(pndRootNew, resNodeToDOM(prnT, RN_INFO_META | RN_INFO_STRUCT));
		}
	      }
	      else if (resNodeIsDir(prnT)) {
		xmlAddChild(pndRootNew, resNodeToDOM(prnT, RN_INFO_META | RN_INFO_STRUCT));
	      }
	      else if (resNodeIsFile(prnT)) {
		xmlAddChild(pndRootNew, resNodeToDOM(prnT, RN_INFO_MAX));
	      }
	    }
	  }
	  e = EXIT_SUCCESS;
	}
	xmlSaveFormatFileEnc("-", pdocResult, "UTF-8", 1);
	xmlFreeDoc(pdocResult);
      }
      resNodeListFree(prnNew);
    }
  }
  else {
    /*!\todo read paths from stdin (e.g. output of find) */
  }

  exit(e);
}
