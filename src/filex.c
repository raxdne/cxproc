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

#ifdef HAVE_LIBARCHIVE
#include <archive/archive.h>
#endif

#include "git_info.h"


int
main(int argc, char *argv[], char *envp[])
{
  int e = EXIT_FAILURE;
  int i;
  xmlChar *pucT;
  xmlDocPtr pdocT = NULL;
  xmlNodePtr pndT = NULL;

  SetLogLevel(1);

  /* register for exit() */
  if (atexit(xmlCleanupParser) != 0
      || atexit(xmlMemoryDump) != 0
      || atexit(domCleanup) != 0
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

      filex -t xml|html|json|plain|sql
      filex -i dir//
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
      resNodePtr prnT;

      for (prnT=prnNew; prnT; prnT = resNodeGetNext(prnT)) {
	xmlNodePtr pndT;

	if (resNodeReadStatus(prnT) && (pndT = resNodeToDOM(prnT, RN_INFO_MAX)) != NULL) {
	  xmlBufferPtr buffer;

	  buffer = xmlBufferCreate();
	  if (buffer) {
	    int iLength = 0;

	    iLength = xmlNodeDump(buffer, pndT->doc, pndT, 0, 1);
	    if (iLength > 0) {
	      xmlChar* pucT;

	      pucT = xmlBufferDetach(buffer);
	      fputs((const char*)pucT, stdout);
	      xmlFree(pucT);
	      e = EXIT_SUCCESS;
	    }
	    xmlBufferFree(buffer);
	  }
	}
      }
      resNodeListFree(prnNew);
    }
  }
  else {
    e = EXIT_SUCCESS;
  }

  exit(e);
}
