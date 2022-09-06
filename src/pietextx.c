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

static BOOL_T
Recognize(xmlNodePtr pndArg);

static BOOL_T
AppendBufferToDoc(xmlNodePtr pndArg, xmlChar* pucArg, xmlChar* pucArgContent);


/*\return new DOM for zero-terminated buffer 
*/
BOOL_T
AppendBufferToDoc(xmlNodePtr pndArg, xmlChar *pucArgFileName, xmlChar* pucArgContent)
{
  BOOL_T fResult = FALSE;

#ifdef HAVE_PIE
  if (pndArg == NULL) {
  }
  else if (pucArgFileName == NULL && pucArgContent == NULL) {
  }
  else {
    xmlChar* pucT = NULL;
    xmlChar* pucContent = NULL;
    FILE* pfArgv = NULL;
    rmode_t m;

    //PrintFormatLog(4, "%s\n", pucArgFileName);

    if (STR_IS_NOT_EMPTY(pucArgContent)) {
      //xmlAddChild(pndArg, xmlNewPI(BAD_CAST "error", pucT));
      if (ParsePlainBuffer(pndArg, pucArgContent, RMODE_PAR)) {
      }
      else {
      }
    }
    else if ((pucT = resPathGetExtension(pucArgFileName)) == NULL) {
      xmlAddChild(pndArg, xmlNewPI(BAD_CAST "error", BAD_CAST"extension"));
    }
    else if (xmlStrEqual(pucT, BAD_CAST"pie")) {
      xmlDocPtr pdocT;

      pdocT = xmlParseFile(pucArgFileName);
      if (pdocT) {
	xmlNodePtr pndRoot;

	pndRoot = xmlDocGetRootElement(pdocT);
	if (IS_NODE_PIE_PIE(pndRoot)) {
	  xmlUnlinkNode(pndRoot);
	  xmlNodeSetName(pndRoot, NAME_PIE_BLOCK);
	  xmlAddChild(pndArg, pndRoot);
	}
	else {
	  xmlAddChild(pndArg, xmlNewPI(BAD_CAST "error", BAD_CAST"pie"));
	}
	xmlFreeDoc(pdocT);
      }
    }
    else if ((m = GetModeByExtension(pucT)) == RMODE_UNDEFINED) {
      xmlAddChild(pndArg, xmlNewPI(BAD_CAST "error", pucT));
    }
    else if ((pfArgv = fopen(pucArgFileName, "r")) == NULL) {
      xmlAddChild(pndArg, xmlNewPI(BAD_CAST "error", BAD_CAST"open"));
    }
    else if ((pucContent = ReadUTF8ToBufferNew(pfArgv))) {
      //xmlAddChild(pndPie, xmlNewPI(BAD_CAST "info", pucT));
      if (ParsePlainBuffer(pndArg, pucContent, m)) {
      }
      else {
      }
      xmlFree(pucContent);
    }
    else {
      xmlAddChild(pndArg, xmlNewPI(BAD_CAST "error", BAD_CAST"read"));
    }

    if (pfArgv) {
      fclose(pfArgv);
      pfArgv = NULL;
    }

    xmlFree(pucT);
    fResult = TRUE;
  }
#endif

  return fResult;
}

/*!
 */
BOOL_T
Recognize(xmlNodePtr pndArg)
{
  BOOL_T fResult = FALSE;

#ifdef HAVE_PIE
  if (pndArg == NULL) {
  }
  else {
    RecognizeInlines(pndArg);
    //RecognizeScripts(pndArg);
    RecognizeFigures(pndArg);
    RecognizeUrls(pndArg);
    RecognizeDates(pndArg, MIME_TEXT_PLAIN);
    RecognizeSymbols(pndArg, LANG_DEFAULT);
    RecognizeTasks(pndArg);
    RecognizeHashtags(pndArg, NULL, NULL);
    //InheritHashtags(pndArg, pndArg);
    //RecognizeGlobalTags(pndTags, pndArg);
    //CleanListTag(pndTags, FALSE);	
    fResult = TRUE;
  }
#endif
  return fResult;
}


int
main(int argc, char *argv[], char *envp[]) {
  xmlDocPtr pdocPie = NULL;

  SetLogLevel(1);
#ifdef HAVE_PIE
  CompileRegExpDefaults();
#endif

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

  if (argc > 1 && strcmp(argv[1],"-?") == 0) {
    fprintf(stderr,"'%s' - parsed text as XML\n\n",argv[0]);
    
    fprintf(stderr,"'%s < abc.txt' - parse plain text input and write XML to stdout\n\n",argv[0]);

    fprintf(stderr,"'find . -type f \\( -iname '*.txt' -o -iname '*.pie' \\) | %s -f ' - output of find command, parse plain text files and write XML to stdout\n\n",argv[0]);
  }
  else if ((pdocPie = xmlNewDoc(BAD_CAST "1.0")) != NULL) {
    xmlNodePtr pndPie;
    
    if ((pndPie = xmlNewDocNode(pdocPie, NULL, BAD_CAST"pie", NULL)) != NULL) {
      xmlChar *pucContent;
	
      xmlDocSetRootElement(pdocPie,pndPie);

      if (argc < 2) { /* no program arguments, stdio to stdout */
	if ((pucContent = ReadUTF8ToBufferNew(stdin)) != NULL) {
	  AppendBufferToDoc(pndPie,NULL,pucContent);
	  xmlFree(pucContent);
	}
	else {
	  xmlAddChild(pndPie, xmlNewPI(BAD_CAST "error", BAD_CAST"read"));
	}
      }
      else if (argc < 3 && strcmp(argv[1], "-f") == 0) { /* read paths from stdin */
	int i;
	char mcLine[BUFFER_LENGTH];

	for (; fgets(mcLine, BUFFER_LENGTH, stdin) == mcLine; ) {

	  for (i = strlen(mcLine); i > 0 && (mcLine[i] == '\0' || mcLine[i] == '\n' || mcLine[i] == '\r'); i--) {
	    mcLine[i] = '\0';
	  }
	  //PrintFormatLog(3, "%s", mcLine);

	  if (AppendBufferToDoc(pndPie, BAD_CAST mcLine, NULL)) {
	  }
	  else {
	    xmlAddChild(pndPie, xmlNewPI(BAD_CAST "error", BAD_CAST"read"));
	  }
	}
      }
      else { /* use program arguments as paths */
	int i;

	for (i = 1; i < argc; i++) {
	  AppendBufferToDoc(pndPie, BAD_CAST argv[i], NULL);
	}
      }
      Recognize(pndPie);
      xmlDocFormatDump(stdout, pdocPie, 1);
    }
  }
  exit(EXIT_SUCCESS);
}
