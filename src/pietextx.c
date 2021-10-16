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

   buffer to XML DOM (blocks, tags, tasks, URLs, tables ...) encoding?

   file to stdout "-f input.txt -o output.pie" ""

   pipe stdin/stdout (UTF-8 only)

   tcl package

   python package

 */

#include <stdio.h>
#include <stdlib.h>
#include <libxml/tree.h>

#include <basics.h>
#include <utils.h>
#include <dom.h>
#include <pie/pie_dtd.h>
#include <pie/pie_text_blocks.h>


/*\return new DOM for zero-terminated buffer */
BOOL_T
AppendBufferToDoc(xmlNodePtr pndArg, xmlChar *pucArg) {
  BOOL_T fResult = FALSE;

  if (pndArg == NULL) {
  }
  else if (pucArg == NULL) {
  }
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
  else {
    xmlSetProp(pndArg, BAD_CAST "error", BAD_CAST"parse");
  }

  return fResult;
}


/*  */
char *
ReadToBuffer(FILE *argin) {
  char *pcResult = NULL;

  if (argin != NULL) {
    size_t b;
    size_t l;

    for (b = 0, l = BUFFER_LENGTH; (pcResult = (char *) xmlRealloc(pcResult,l)) != NULL; b += BUFFER_LENGTH, l += BUFFER_LENGTH) {
      size_t k;
	
      fprintf(stderr,"! %lu Byte allocated\n",l);

      if ((k = fread(&pcResult[b],1,BUFFER_LENGTH, argin)) < BUFFER_LENGTH) {
	/* end of input reached */
	l = b + k;
	pcResult[l] = '\0';
	fprintf(stderr,"! %lu Byte read\n",l);
	break;
      }
    }
  } 
  return pcResult;
}


int
main() {
  xmlDocPtr pdocPie;

  if ((pdocPie = xmlNewDoc(BAD_CAST "1.0")) != NULL) {
    xmlNodePtr pndPie;
    
    if ((pndPie = xmlNewDocNode(pdocPie, NULL, NAME_PIE_PIE, NULL)) != NULL) {
      char *pcContent;
	
      xmlDocSetRootElement(pdocPie,pndPie);

      if ((pcContent = ReadToBuffer(stdin)) != NULL) {
	//puts(pcContent);
	AppendBufferToDoc(pndPie,BAD_CAST pcContent);
	xmlFree(pcContent);
      }
      else {
	//xmlSetProp(pnd, BAD_CAST "error", BAD_CAST"parse");
      }
      
      xmlDocFormatDump(stdout, pdocPie, 1);
    }
  }

  exit(EXIT_SUCCESS);
}
