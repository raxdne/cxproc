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

/*! 
*/
int
pieElementTest(void)
{
  int n_ok;
  int i;

  n_ok=0;
  i=0;


  if (RUNTEST) {
    xmlChar *pucT;
    char *pchContent;
    index_t iLength = -1;
    resNodePtr prnT = NULL;
    
    i++;
    printf("TEST %i in '%s:%i': reads first line of plain text file = ",i,__FILE__,__LINE__);

    prnT = resNodeConcatNew(BAD_CAST TESTPREFIX, BAD_CAST "option/pie/text/test-pie-6.txt");
    if (prnT) {
      pchContent = (char*) plainGetContextTextEat(prnT,16);
      pucT = DuplicateNextLine(pchContent,&iLength);
      if (iLength==60) {
	n_ok++;
	printf("OK\n");
      }
      else {
	printf("Error DuplicateNextLine(): %i\n",iLength);
      }
      xmlFree(pucT);
      xmlFree(pchContent);
      resNodeFree(prnT);
    }
  }

  if (RUNTEST) {
    int l;

    i++;
    printf("TEST %i in '%s:%i': reads empty text = ", i, __FILE__, __LINE__);

    if (StrLineIsEmpty(NULL, &l) == FALSE || l != 0) {
      printf("Error 1 StrLineIsEmpty()");
    }
    else if (StrLineIsEmpty(BAD_CAST "", &l) == FALSE || l != 0) {
      printf("Error 2 StrLineIsEmpty()");
    }
    else if (StrLineIsEmpty(BAD_CAST "\t\r\n\n", &l) == FALSE || l != 4) {
      printf("Error 3 StrLineIsEmpty()");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    pieTextElementPtr ppeT;

    i++;
    printf("TEST %i in '%s:%i': reads empty text = ",i,__FILE__,__LINE__);

    if ((ppeT = pieElementNew(BAD_CAST "\t\r\n", RMODE_PAR)) == NULL) {
      printf("Error pieElementNew()");
    }
    else if (pieElementHasNext(ppeT) == FALSE) {
      printf("Error 1 pieElementHasNext()");
    }
    else if (pieElementHasNext(ppeT) == TRUE) {
      printf("Error 2 pieElementHasNext()");
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    pieElementFree(ppeT);
  }

  if (RUNTEST) {
    pieTextElementPtr ppeT;

    i++;
    printf("TEST %i in '%s:%i': parse plain text = ", i, __FILE__, __LINE__);

    ppeT = pieElementNew(BAD_CAST"* ABC\n \nDEF\n\n- HIJ\nKLM\n\n--OPQ\n\n", RMODE_PAR);

    if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsHeader(ppeT) == FALSE || pieElementGetDepth(ppeT) != 1 || pieElementGetStrlen(ppeT) != 3) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsPar(ppeT) == FALSE || pieElementGetDepth(ppeT) != 0 || pieElementGetStrlen(ppeT) != 3) {
      printf("Error %i pieElementHasNext()", 2);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsListItem(ppeT) == FALSE || pieElementGetDepth(ppeT) != 1 || pieElementGetStrlen(ppeT) != 7) {
      printf("Error %i pieElementHasNext()", 2);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsListItem(ppeT) == FALSE || pieElementGetDepth(ppeT) != 2 || pieElementGetStrlen(ppeT) != 3) {
      printf("Error %i pieElementHasNext()", 2);
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    pieElementFree(ppeT);
  }

#ifdef WITH_MARKDOWN

  /* https://daringfireball.net/projects/markdown/syntax#list */

  /* https://spec.commonmark.org/0.29/ */

/* string utility functions */

  if (RUNTEST) {
    int iT;

    i++;
    printf("TEST %i in '%s:%i': Markdown MdStrLineIsEmpty() = ", i, __FILE__, __LINE__);

    if (MdStrLineIsEmpty(NULL, NULL) == TRUE) {
      printf("Error 1 MdStrLineIsEmpty()\n");
    }
    else if (MdStrLineIsEmpty(BAD_CAST"   A", &iT) == TRUE) {
      printf("Error 3 MdStrLineIsEmpty()\n");
    }
    else if (MdStrLineIsEmpty(BAD_CAST"", NULL) == FALSE) {
      printf("Error 2 MdStrLineIsEmpty()\n");
    }
    else if (MdStrLineIsEmpty(BAD_CAST"  \t \r\n", &iT) == FALSE || iT != 5) {
      printf("Error 4 MdStrLineIsEmpty()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }


  if (RUNTEST) {
    int iT;

    i++;
    printf("TEST %i in '%s:%i': Markdown MdStrLineIsList() = ", i, __FILE__, __LINE__);

    if (MdStrLineIsList(NULL, NULL) == TRUE) {
      printf("Error 1 MdStrLineIsList()\n");
    }
    else if (MdStrLineIsList(BAD_CAST"", NULL) == TRUE) {
      printf("Error 2 MdStrLineIsList()\n");
    }
    else if (MdStrLineIsList(BAD_CAST"   A", &iT) == TRUE) {
      printf("Error 3 MdStrLineIsList()\n");
    }
    else if (MdStrLineIsList(BAD_CAST"* AA \n", &iT) == FALSE || iT != 1) {
      printf("Error 4 MdStrLineIsList()\n");
    }
    else if (MdStrLineIsList(BAD_CAST"+ AA \nBBB", &iT) == FALSE || iT != 1) {
      printf("Error 4 MdStrLineIsList()\n");
    }
    else if (MdStrLineIsList(BAD_CAST"- AA \n\nCC", &iT) == FALSE || iT != 1) {
      printf("Error 4 MdStrLineIsList()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }


  if (RUNTEST) {
    int iT;

    i++;
    printf("TEST %i in '%s:%i': Markdown MdStrLineIsListEnum() = ", i, __FILE__, __LINE__);

    if (MdStrLineIsListEnum(NULL, NULL) == TRUE) {
      printf("Error 1 MdStrLineIsListEnum()\n");
    }
    else if (MdStrLineIsListEnum(BAD_CAST"", NULL) == TRUE) {
      printf("Error 2 MdStrLineIsListEnum()\n");
    }
    else if (MdStrLineIsListEnum(BAD_CAST" A.  ", &iT) == TRUE) {
      printf("Error 3 MdStrLineIsListEnum()\n");
    }
    else if (MdStrLineIsListEnum(BAD_CAST"1. Z \n", &iT) == FALSE || iT != 2) {
      printf("Error 4 MdStrLineIsListEnum()\n");
    }
    else if (MdStrLineIsListEnum(BAD_CAST"1) Z   \n\rTTT", &iT) == FALSE || iT != 2) {
      printf("Error 5 MdStrLineIsListEnum()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }


  if (RUNTEST) {
    int iT;

    i++;
    printf("TEST %i in '%s:%i': Markdown MdStrLineIsHeader() = ", i, __FILE__, __LINE__);

    if (MdStrLineIsHeader(NULL, NULL) == TRUE) {
      printf("Error 1 MdStrLineIsHeader()\n");
    }
    else if (MdStrLineIsHeader(BAD_CAST"", NULL) == TRUE) {
      printf("Error 2 MdStrLineIsHeader()\n");
    }
    else if (MdStrLineIsHeader(BAD_CAST"   A", &iT) == TRUE) {
      printf("Error 3 MdStrLineIsHeader()\n");
    }
    else if (MdStrLineIsHeader(BAD_CAST"## AAA ##", &iT) == FALSE || iT != 2) {
      printf("Error 4 MdStrLineIsHeader()\n");
    }
    else if (MdStrLineIsHeader(BAD_CAST"#######  AAA ", &iT) == TRUE || iT != 0) {
      printf("Error 4 MdStrLineIsHeader()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }


  if (RUNTEST) {
    int iT;

    i++;
    printf("TEST %i in '%s:%i': Markdown MdStrLineIsHeaderLine() = ", i, __FILE__, __LINE__);

    if (MdStrLineIsHeaderLine(NULL, NULL) == TRUE) {
      printf("Error 1 MdStrLineIsHeaderLine()\n");
    }
    else if (MdStrLineIsHeaderLine(BAD_CAST"", NULL) == TRUE) {
      printf("Error 2 MdStrLineIsHeaderLine()\n");
    }
    else if (MdStrLineIsHeaderLine(BAD_CAST"   A", &iT) == TRUE) {
      printf("Error 3 MdStrLineIsHeaderLine()\n");
    }
    else if (MdStrLineIsHeaderLine(BAD_CAST"---\r\n", &iT) == FALSE || iT != 4) {
      printf("Error 4 MdStrLineIsHeaderLine()\n");
    }
    else if (MdStrLineIsHeaderLine(BAD_CAST"=======\r\n", &iT) == FALSE || iT != 8) {
      printf("Error 4 MdStrLineIsHeaderLine()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }


  if (RUNTEST) {
    int iT;

    i++;
    printf("TEST %i in '%s:%i': Markdown MdStrLineIsBlockQuote() = ", i, __FILE__, __LINE__);

    if (MdStrLineIsBlockQuote(NULL, NULL) == TRUE) {
      printf("Error 1 MdStrLineIsBlockQuote()\n");
    }
    else if (MdStrLineIsBlockQuote(BAD_CAST"", NULL) == TRUE) {
      printf("Error 2 MdStrLineIsBlockQuote()\n");
    }
    else if (MdStrLineIsBlockQuote(BAD_CAST"   A", &iT) == TRUE) {
      printf("Error 3 MdStrLineIsBlockQuote()\n");
    }
    else if (MdStrLineIsBlockQuote(BAD_CAST"> BBB\r\n", &iT) == FALSE || iT != 2) {
      printf("Error 4 MdStrLineIsBlockQuote()\n");
    }
    else if (MdStrLineIsBlockQuote(BAD_CAST"> \r\n", &iT) == FALSE || iT != 2) {
      printf("Error 4 MdStrLineIsBlockQuote()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }


  if (RUNTEST) {
    int iT;

    i++;
    printf("TEST %i in '%s:%i': Markdown MdStrLineIsBlockCode() = ", i, __FILE__, __LINE__);

    if (MdStrLineIsBlockCode(NULL, NULL) == TRUE) {
      printf("Error 1 MdStrLineIsBlockCode()\n");
    }
    else if (MdStrLineIsBlockCode(BAD_CAST"", NULL) == TRUE) {
      printf("Error 2 MdStrLineIsBlockCode()\n");
    }
    else if (MdStrLineIsBlockCode(BAD_CAST"\tA", &iT) == FALSE || iT != 1) {
      printf("Error 3 MdStrLineIsBlockCode()\n");
    }
    else if (MdStrLineIsBlockCode(BAD_CAST"    BB\r\n", &iT) == FALSE || iT != 4) {
      printf("Error 4 MdStrLineIsBlockCode()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  /* Block Elements */

  if (RUNTEST) {
    pieTextElementPtr ppeT;

    i++;
    printf("TEST %i in '%s:%i': Markdown par ends at empty line = ", i, __FILE__, __LINE__);

    ppeT = pieElementNew(BAD_CAST"ABCDE\nFGHI\n \t  \r\nJKLM\n", RMODE_MD);

    if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsPar(ppeT) == FALSE || pieElementGetDepth(ppeT) != 0 || pieElementGetStrlen(ppeT) != 10) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsPar(ppeT) == TRUE || pieElementGetBeginPtr(ppeT) != NULL || pieElementGetDepth(ppeT) != 0 || pieElementGetStrlen(ppeT) != 0) {
      printf("Error %i pieElementHasNext()", 1);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsPar(ppeT) == FALSE || pieElementGetDepth(ppeT) != 0 || pieElementGetStrlen(ppeT) != 4) {
      printf("Error %i pieElementHasNext()", 2);
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    pieElementFree(ppeT);
  }

  if (RUNTEST) {
    pieTextElementPtr ppeT;

    i++;
    printf("TEST %i in '%s:%i': Markdown par ends at trailing space chars = ", i, __FILE__, __LINE__);

    ppeT = pieElementNew(BAD_CAST"JKLM\nZZZ   \nYY\nX\n\r\r\n", RMODE_MD);

    if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsPar(ppeT) == FALSE || pieElementGetDepth(ppeT) != 0 || pieElementGetStrlen(ppeT) != 11) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsPar(ppeT) == FALSE || pieElementGetDepth(ppeT) != 0 || pieElementGetStrlen(ppeT) != 4) {
      printf("Error %i pieElementHasNext()", 2);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsPar(ppeT) == TRUE || pieElementGetBeginPtr(ppeT) != NULL || pieElementGetDepth(ppeT) != 0 || pieElementGetStrlen(ppeT) != 0) {
      printf("Error %i pieElementHasNext()", 1);
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    pieElementFree(ppeT);
  }

  if (RUNTEST) {
    pieTextElementPtr ppeT;

    i++;
    printf("TEST %i in '%s:%i': Markdown blockquote = ", i, __FILE__, __LINE__);

    ppeT = pieElementNew(BAD_CAST"LLL\n> JJJJ\n> KKK\nMMM NN\n", RMODE_MD);

    if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsPar(ppeT) == FALSE || pieElementGetDepth(ppeT) != 0 || pieElementGetStrlen(ppeT) != 3) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsQuote(ppeT) == FALSE || pieElementGetDepth(ppeT) != 0 || pieElementGetStrlen(ppeT) != 8) {
      printf("Error %i pieElementHasNext()", 2);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsPar(ppeT) == FALSE || pieElementGetDepth(ppeT) != 0 || pieElementGetStrlen(ppeT) != 6) {
      printf("Error %i pieElementHasNext()", 2);
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    pieElementFree(ppeT);
  }

  if (RUNTEST) {
    pieTextElementPtr ppeT;

    i++;
    printf("TEST %i in '%s:%i': Markdown (pre-formatted) codeblock = ", i, __FILE__, __LINE__);

    ppeT = pieElementNew(BAD_CAST"MMM NN  \n\tDDD\n\t#EEEE#\n    FF FF\n\n\n", RMODE_MD);

    if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsPar(ppeT) == FALSE || pieElementGetDepth(ppeT) != 0 || pieElementGetStrlen(ppeT) != 8) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsPre(ppeT) == FALSE || pieElementGetDepth(ppeT) != 0 || pieElementGetStrlen(ppeT) != 17) {
      printf("Error %i pieElementHasNext()", 2);
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    pieElementFree(ppeT);
  }

  if (RUNTEST) {
    pieTextElementPtr ppeT;

    i++;
    printf("TEST %i in '%s:%i': Markdown (pre-formatted) codeblock = ", i, __FILE__, __LINE__);

    ppeT = pieElementNew(BAD_CAST"\n``` script\nMMM NN  \nDDD\n#EEEE#\nFF FF\n```\n\n", RMODE_MD);

    if (pieElementHasNext(ppeT) == FALSE) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsPre(ppeT) == FALSE || pieElementGetDepth(ppeT) != 0 || pieElementGetStrlen(ppeT) != 25) {
      printf("Error %i pieElementHasNext()", 2);
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    pieElementFree(ppeT);
  }

  if (RUNTEST) {
    pieTextElementPtr ppeT;

    i++;
    printf("TEST %i in '%s:%i': Markdown (pre-formatted) codeblock = ", i, __FILE__, __LINE__);

    ppeT = pieElementNew(BAD_CAST"\n~~~ \nMMM NN  \nDDD\n#EEEE#\nFF FF\n~~~\n\n", RMODE_MD);

    if (pieElementHasNext(ppeT) == FALSE) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsPre(ppeT) == FALSE || pieElementGetDepth(ppeT) != 0 || pieElementGetStrlen(ppeT) != 25) {
      printf("Error %i pieElementHasNext()", 2);
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    pieElementFree(ppeT);
  }

  if (RUNTEST) {
    pieTextElementPtr ppeT;

    i++;
    printf("TEST %i in '%s:%i': Markdown header mix = ", i, __FILE__, __LINE__);

    ppeT = pieElementNew(BAD_CAST"BBB\n# CCC\nFFFFF\nDDD\n-----\nHHHH\n", RMODE_MD);

    if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsPar(ppeT) == FALSE || pieElementGetDepth(ppeT) != 0 || pieElementGetStrlen(ppeT) != 3) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsHeader(ppeT) == FALSE || pieElementGetDepth(ppeT) != 1 || pieElementGetStrlen(ppeT) != 3) {
      printf("Error %i pieElementHasNext()", 2);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsHeader(ppeT) == FALSE || pieElementGetDepth(ppeT) != 2 || pieElementGetStrlen(ppeT) != 9) {
      printf("Error %i pieElementHasNext()", 2);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsPar(ppeT) == FALSE || pieElementGetDepth(ppeT) != 0 || pieElementGetStrlen(ppeT) != 4) {
      printf("Error %i pieElementHasNext()", 2);
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    pieElementFree(ppeT);
  }

  if (RUNTEST) {
    pieTextElementPtr ppeT;

    i++;
    printf("TEST %i in '%s:%i': Markdown header mix = ", i, __FILE__, __LINE__);

    ppeT = pieElementNew(BAD_CAST"ABCDE\n=====\n#FGHI#\n##NOPQ\n", RMODE_MD);

    if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsHeader(ppeT) == FALSE || pieElementGetDepth(ppeT) != 1 || pieElementGetStrlen(ppeT) != 5) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsHeader(ppeT) == FALSE || pieElementGetDepth(ppeT) != 1 || pieElementGetStrlen(ppeT) != 4) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsHeader(ppeT) == FALSE || pieElementGetDepth(ppeT) != 2 || pieElementGetStrlen(ppeT) != 4) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    pieElementFree(ppeT);
  }

  if (RUNTEST) {
    pieTextElementPtr ppeT;

    i++;
    printf("TEST %i in '%s:%i': Markdown header mix = ", i, __FILE__, __LINE__);

    ppeT = pieElementNew(BAD_CAST"ABCDE\n=====\n#FGHI#\n##NOPQ\n", RMODE_MD);

    if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsHeader(ppeT) == FALSE || pieElementGetDepth(ppeT) != 1 || pieElementGetStrlen(ppeT) != 5) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsHeader(ppeT) == FALSE || pieElementGetDepth(ppeT) != 1 || pieElementGetStrlen(ppeT) != 4) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsHeader(ppeT) == FALSE || pieElementGetDepth(ppeT) != 2 || pieElementGetStrlen(ppeT) != 4) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    pieElementFree(ppeT);
  }

  if (RUNTEST) {
    pieTextElementPtr ppeT;

    i++;
    printf("TEST %i in '%s:%i': Markdown Lists = ", i, __FILE__, __LINE__);

    ppeT = pieElementNew(BAD_CAST"item\n- a \n*\taa\n+  aa\n\n", RMODE_MD);

    if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsPar(ppeT) == FALSE || pieElementGetDepth(ppeT) != 0 || pieElementGetStrlen(ppeT) != 4) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsListItem(ppeT) == FALSE || pieElementGetDepth(ppeT) != 1 || pieElementGetStrlen(ppeT) != 2) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsListItem(ppeT) == FALSE || pieElementGetDepth(ppeT) != 1 || pieElementGetStrlen(ppeT) != 2) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsListItem(ppeT) == FALSE || pieElementGetDepth(ppeT) != 1 || pieElementGetStrlen(ppeT) != 2) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    pieElementFree(ppeT);
  }

  if (RUNTEST) {
    pieTextElementPtr ppeT;

    i++;
    printf("TEST %i in '%s:%i': Markdown nested Lists = ", i, __FILE__, __LINE__);

    ppeT = pieElementNew(BAD_CAST"item\n- a \n  *\taa\n    +  aa\n\n", RMODE_MD);

    if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsPar(ppeT) == FALSE || pieElementGetDepth(ppeT) != 0 || pieElementGetStrlen(ppeT) != 4) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsListItem(ppeT) == FALSE || pieElementGetDepth(ppeT) != 1 || pieElementGetStrlen(ppeT) != 2) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsListItem(ppeT) == FALSE || pieElementGetDepth(ppeT) != 2 || pieElementGetStrlen(ppeT) != 2) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsListItem(ppeT) == FALSE || pieElementGetDepth(ppeT) != 3 || pieElementGetStrlen(ppeT) != 2) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    pieElementFree(ppeT);
  }

  if (RUNTEST) {
    pieTextElementPtr ppeT;

    i++;
    printf("TEST %i in '%s:%i': Markdown enumerated Lists = ", i, __FILE__, __LINE__);

    ppeT = pieElementNew(BAD_CAST"enum\n1. A\n 1. B\n  1. C  \nDDD", RMODE_MD);

    if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsPar(ppeT) == FALSE || pieElementGetDepth(ppeT) != 0 || pieElementGetStrlen(ppeT) != 4) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsEnum(ppeT) == FALSE || pieElementGetDepth(ppeT) != 1 || pieElementGetStrlen(ppeT) != 1) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsEnum(ppeT) == FALSE || pieElementGetDepth(ppeT) != 1 || pieElementGetStrlen(ppeT) != 1) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsEnum(ppeT) == FALSE || pieElementGetDepth(ppeT) != 1 || pieElementGetStrlen(ppeT) != 3) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    pieElementFree(ppeT);
  }

  if (RUNTEST) {
    pieTextElementPtr ppeT;

    i++;
    printf("TEST %i in '%s:%i': Markdown enumerated Lists = ", i, __FILE__, __LINE__);

    ppeT = pieElementNew(BAD_CAST"enum\n2) A\n 3) B\n  4) C  \nDDD", RMODE_MD);

    if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsPar(ppeT) == FALSE || pieElementGetDepth(ppeT) != 0 || pieElementGetStrlen(ppeT) != 4) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsEnum(ppeT) == FALSE || pieElementGetDepth(ppeT) != 1 || pieElementGetStrlen(ppeT) != 1) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsEnum(ppeT) == FALSE || pieElementGetDepth(ppeT) != 1 || pieElementGetStrlen(ppeT) != 1) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsEnum(ppeT) == FALSE || pieElementGetDepth(ppeT) != 1 || pieElementGetStrlen(ppeT) != 3) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    pieElementFree(ppeT);
  }

  if (RUNTEST) {
    pieTextElementPtr ppeT;

    i++;
    printf("TEST %i in '%s:%i': Markdown enumerated Lists = ", i, __FILE__, __LINE__);

    ppeT = pieElementNew(BAD_CAST"\n* * * *\n____\n----\nRQST\n-----\nabcd\n", RMODE_MD);

    if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementGetDepth(ppeT) != 0 || pieElementGetStrlen(ppeT) != 0) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsRuler(ppeT) == FALSE || pieElementGetDepth(ppeT) != 0 || pieElementGetStrlen(ppeT) != 0) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsRuler(ppeT) == FALSE || pieElementGetDepth(ppeT) != 0 || pieElementGetStrlen(ppeT) != 0) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsRuler(ppeT) == FALSE || pieElementGetDepth(ppeT) != 0 || pieElementGetStrlen(ppeT) != 0) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE
      || pieElementIsHeader(ppeT) == FALSE || pieElementGetDepth(ppeT) != 2 || pieElementGetStrlen(ppeT) != 4) {
      printf("Error %i pieElementHasNext()", 0);
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    pieElementFree(ppeT);
  }

#endif

  
  if (RUNTEST) {
    pieTextElementPtr ppeT;

    i++;
    printf("TEST %i in '%s:%i': pieElementWeight() = ", i, __FILE__, __LINE__);

    if ((ppeT = pieElementNew(BAD_CAST"\n\n", RMODE_PAR)) == NULL) {
      printf("Error %i pieElementNew()", 0);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE || pieElementIsPar(ppeT) == TRUE || pieElementWeight(ppeT) != -1 || xmlStrlen(ppeT->pucContent) != 0) {
      printf("Error %i pieElementWeight()", 0);
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    pieElementFree(ppeT);
  }

  
  if (RUNTEST) {
    pieTextElementPtr ppeT;

    i++;
    printf("TEST %i in '%s:%i': pieElementWeight() = ", i, __FILE__, __LINE__);

    if ((ppeT = pieElementNew(BAD_CAST"Abc def++\n\n*** HHHH   +++   \n\nKKKK\n\n", RMODE_PAR)) == NULL) {
      printf("Error %i pieElementNew()", 0);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE || pieElementIsPar(ppeT) == FALSE || pieElementWeight(ppeT) != 2 || xmlStrlen(ppeT->pucContent) != 7) {
      printf("Error %i pieElementWeight()", 0);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE || pieElementIsHeader(ppeT) == FALSE || pieElementWeight(ppeT) != 3 || xmlStrlen(ppeT->pucContent) != 7) {
      printf("Error %i pieElementWeight()", 0);
    }
    else if (pieElementHasNext(ppeT) == FALSE || pieElementParse(ppeT) == FALSE || pieElementIsPar(ppeT) == FALSE || pieElementWeight(ppeT) != 0 || xmlStrlen(ppeT->pucContent) != 4) {
      printf("Error %i pieElementWeight()", 0);
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    pieElementFree(ppeT);
  }


  if (RUNTEST) {
    xmlChar* pucContent;
    pieTextElementPtr ppeT;
    index_t k = 0;
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': reads plain text file line by line = ", i, __FILE__, __LINE__);

    prnT = resNodeConcatNew(BAD_CAST TESTPREFIX, BAD_CAST "option/pie/text/test-pie-10.txt");
    if (prnT) {
      pucContent = plainGetContextTextEat(prnT, 16);
      if (pucContent) {
	ppeT = pieElementNew(pucContent, RMODE_LINE);
	for (k=0; pieElementHasNext(ppeT); k++) {
	  pieElementParse(ppeT);
	  //pieElementReplaceCharMarkup(ppeT);
	}
	pieElementFree(ppeT);
	xmlFree(pucContent);
      }
      resNodeFree(prnT);
    }

    if (k == 7) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error pieTextReadElement() %i\n", k);
    }
  }

  printf("Result in '%s': %i/%i OK\n\n",__FILE__,n_ok,i);

  return (i - n_ok);
}
/* end of pieElementTest() */
