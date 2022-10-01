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
    else if (pieElementHasNext(ppeT) == FALSE || pieElementGetDepth(ppeT) != 0 || pieElementGetStrlen(ppeT) != 0) {
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
