/*
  cxproc - Configurable Xml PROCessor

  Copyright (C) 2006..2021 by Alexander Tenbusch

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
main(int argc, char *argv[], char *envp[]) {
  xmlDocPtr pdocPie = NULL;

  SetLogLevel(1);

  if (argc > 1 && strcmp(argv[1],"-?") == 0) {
    fprintf(stderr,"'%s' - write parsed plain text a XML\n\n",argv[0]);
    fprintf(stderr,"'%s < abc.txt' - parse plain text input and write XML to stdout\n\n",argv[0]);
    fprintf(stderr,"'find -type f -iname '*.txt' | %s -f ' - output of find command, parse plain text files and write XML to stdout\n\n",argv[0]);
  }
  else if ((pdocPie = xmlNewDoc(BAD_CAST "1.0")) != NULL) {
    xmlNodePtr pndPie;
    
    if ((pndPie = xmlNewDocNode(pdocPie, NULL, NAME_PIE_PIE, NULL)) != NULL) {
      xmlChar *pucContent;
	
      xmlDocSetRootElement(pdocPie,pndPie);

      if (argc < 2) { /* no program arguments, stdio to stdout */
	if ((pucContent = ReadUTF8ToBufferNew(stdin)) != NULL) {
#ifdef HAVE_PIE
	  AppendBufferToDoc(pndPie,pucContent);
#else
	  xmlAddChild(pndPie, xmlNewPI(BAD_CAST "error", BAD_CAST"pie"));
#endif
	  xmlFree(pucContent);
	}
	else {
	  xmlAddChild(pndPie, xmlNewPI(BAD_CAST "error", BAD_CAST"read"));
	}
      }
      else if (argc < 3 && strcmp(argv[1],"-f") == 0) { /* read paths from stdin */
	int i;
	char mcLine[BUFFER_LENGTH];

	for ( ; fgets(mcLine,BUFFER_LENGTH,stdin) == mcLine ; ) {
	  FILE *pfLine = NULL;
    
	  for (i=strlen(mcLine); i > 0 && (mcLine[i] == '\0' || mcLine[i] == '\n' || mcLine[i] == '\r'); i--) {
	    mcLine[i] = '\0';
	  }
	  PrintFormatLog(3,"%s",mcLine);
    
	  if ((pfLine = fopen(mcLine,"r")) == NULL) {
	    xmlAddChild(pndPie, xmlNewPI(BAD_CAST "error", BAD_CAST"open"));
	  }
	  else {
	    if ((pucContent = ReadUTF8ToBufferNew(pfLine)) != NULL) {
#ifdef HAVE_PIE
	      AppendBufferToDoc(pndPie,BAD_CAST pucContent);
#else
	      xmlAddChild(pndPie, xmlNewPI(BAD_CAST "error", BAD_CAST"pie"));
#endif
	      xmlFree(pucContent);
	    }
	    else {
	      xmlAddChild(pndPie, xmlNewPI(BAD_CAST "error", BAD_CAST"read"));
	    }
	    fclose(pfLine);
	  }
	}
      }
      else { /* use program arguments as paths */
	int i;

	for (i = 1; i < argc; i++) {
	  FILE *pfArgv = NULL;

	  PrintFormatLog(4, "%s\n", argv[i]);

	  if ((pfArgv = fopen(argv[i],"r")) == NULL) {
	    xmlAddChild(pndPie, xmlNewPI(BAD_CAST "error", BAD_CAST"open"));
	  }
	  else {
	    if ((pucContent = ReadUTF8ToBufferNew(pfArgv)) != NULL) {
#ifdef HAVE_PIE
	      AppendBufferToDoc(pndPie,BAD_CAST pucContent);
#else
	      xmlAddChild(pndPie, xmlNewPI(BAD_CAST "error", BAD_CAST"pie"));
#endif
	      xmlFree(pucContent);
	    }
	    else {
	      xmlAddChild(pndPie, xmlNewPI(BAD_CAST "error", BAD_CAST"read"));
	    }
	    fclose(pfArgv);
	  }
	}
      }
      xmlDocFormatDump(stdout, pdocPie, 1);
    }
  }

  exit(EXIT_SUCCESS);
}
