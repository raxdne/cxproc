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

/*! buffer to XML DOM (blocks, tags, tasks, URLs, tables ...) encoding? */

/*!\todo file to stdout "-f input.txt -o output.pie" "" */

/*!\todo pipe stdin/stdout (UTF-8 only) */

/*!\todo tcl package */

/*!\todo python package */


#include <stdio.h>
#include <stdlib.h>
#include <libxml/tree.h>

#include <basics.h>
#include <utils.h>
#include <dom.h>

#ifdef HAVE_PIE
#include <pie/pie_text_blocks.h>
#endif


/*\return new DOM for zero-terminated buffer */
BOOL_T
AppendBufferToDoc(xmlNodePtr pndArg, xmlChar *pucArg) {
  BOOL_T fResult = FALSE;

  if (pndArg == NULL) {
  }
  else if (pucArg == NULL) {
  }
#ifdef HAVE_PIE
  else if (ParsePlainBuffer(pndArg, pucArg, RMODE_PAR)) {
    RecognizeInlines(pndArg);
    //RecognizeScripts(pndArg);
    RecognizeFigures(pndArg);
    RecognizeUrls(pndArg);
    RecognizeDates(pndArg,MIME_TEXT_PLAIN);
    RecognizeSymbols(pndArg, LANG_DEFAULT);
    RecognizeTasks(pndArg);
    RecognizeHashtags(pndArg,NULL, NULL);
    //InheritHashtags(pndArg, pndArg);
    //RecognizeGlobalTags(pndTags, pndArg);
    //CleanListTag(pndTags, FALSE);	    
  }
#endif
  else {
    xmlSetProp(pndArg, BAD_CAST "error", BAD_CAST"parse");
  }

  return fResult;
}


int
main() {
  xmlDocPtr pdocPie;

  SetLogLevel(3);

  if ((pdocPie = xmlNewDoc(BAD_CAST "1.0")) != NULL) {
    xmlNodePtr pndPie;
    
    if ((pndPie = xmlNewDocNode(pdocPie, NULL, NAME_PIE_PIE, NULL)) != NULL) {
      xmlChar *pucContent;
	
      xmlDocSetRootElement(pdocPie,pndPie);

      if ((pucContent = ReadUTF8ToBufferNew(stdin)) != NULL) {
#ifdef HAVE_PIE
	AppendBufferToDoc(pndPie,BAD_CAST pucContent);
#else
	xmlSetProp(pndPie, BAD_CAST "error", BAD_CAST"pie");
	//fputs(pucContent,stdout);
#endif
	xmlFree(pucContent);
      }
      else {
	//xmlSetProp(pnd, BAD_CAST "error", BAD_CAST"parse");
      }
      
      xmlDocFormatDump(stdout, pdocPie, 1);
    }
  }

  exit(EXIT_SUCCESS);
}
