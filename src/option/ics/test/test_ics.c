/*
  cxproc - Configurable Xml PROCessor

  Copyright (C) 2006..2024 by Alexander Tenbusch

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


int
icsTest(void)
{
  int n_ok;
  int i;

  n_ok=0;
  i=0;

  if (RUNTEST) {

    i++;
    printf("TEST %i in '%s:%i': detectNextBlock() = ", i, __FILE__, __LINE__);

    if (detectNextBlock(NULL, 0, -1, NULL, NULL, NULL, NULL) == TRUE) {
      printf("Error 1 detectNextBlock()\n");
    }
    else if (detectNextLine(NULL, 0, -1, NULL, NULL, NULL, NULL) == TRUE) {
      printf("Error 2 detectNextLine()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

#if 1
 if (RUNTEST) {
    char *pchT = "   \n\n AAA BBB;\n\nDTSTART:20250106\n\nAAA\n\nBEGIN:\nSUMMARY:My Test\n  ";
    //char *pchT = "   VCALENDAR\n\nBBB;\nBEGIN:VCALENDAR\nAAA\nBBB\nBEGIN:VEVENT\nDTSTART:20250106\nDTEND:20250109\nSUMMARY:My Test\nEND:VEVENT\nEND:VCALENDAR\nAAA\nBEGIN:VCALENDAR\nAAA\nBBB\nBEGIN:VEVENT\nDTSTART:20250106\n\nEND:VEVENT";
    int j0, j1;
    int k0, k1;
    int n0, n1;
    int v0, v1;
    int l;
    int n;
int y;

    i++;
    printf("TEST %i in '%s:%i': detectNextLine() as Iterator = ", i, __FILE__, __LINE__);

    for (y = 0, k0 = 0, k1 = strlen(pchT), n = 0; n < 10; n++, k0 = v1) {
      if (detectNextLine(pchT, k0, k1, &n0, &n1, &v0, &v1)) {
	xmlChar *pucN;
	xmlChar *pucV;

	pucN = xmlStrndup(&pchT[n0], n1 - n0 + 1);
	pucV = xmlStrndup(&pchT[v0], v1 - v0);
	printf("%i '%s': '%s'\n", n, pucN, pucV);
	y++;
	xmlFree(pucV);
	xmlFree(pucN);
      }
      else if (v1 == k0) {
	break;
      }
      else {
	printf("%i empty\n", n);
      }
    }

    if (y != 3) {
      printf("Error 2 detectNextLine()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

 if (RUNTEST) {
    char *pchT = "   \n\n AAA BBB;\n\nDTSTART:20250106\n\nAAA\n\nBEGIN:\nSUMMARY:My Test\n  ";
    //char *pchT = "   VCALENDAR\n\nBBB;\nBEGIN:VCALENDAR\nAAA\nBBB\nBEGIN:VEVENT\nDTSTART:20250106\nDTEND:20250109\nSUMMARY:My Test\nEND:VEVENT\nEND:VCALENDAR\nAAA\nBEGIN:VCALENDAR\nAAA\nBBB\nBEGIN:VEVENT\nDTSTART:20250106\n\nEND:VEVENT";
    int j0, j1;
    int k0, k1;
    int n0, n1;
    int v0, v1;
    int l;
    int n;
int y;

    i++;
    printf("TEST %i in '%s:%i': detectNextBlock() as Iterator = ", i, __FILE__, __LINE__);

    for (y = 0, k0 = 0, k1 = strlen(pchT), n = 0; n < 10; n++, k0 = j1) {
      if (detectNextBlock(pchT, k0, k1, &j0, &k0, &k1, &j1)) {
	xmlChar *pucN;

	pucN = xmlStrndup(&pchT[j0], j1 - k0 + 1);
	printf("%i '%s'\n", n, pucN);
	y++;
	xmlFree(pucN);
      }
      else if (v1 == k0) {
	break;
      }
      else {
	printf("%i empty\n", n);
      }
    }

    if (y != 3) {
      printf("Error 2 detectNextLine()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    char *pchT = "   \n\n AAA BBB;\n\nDTSTART:20250106\n\nAAA\nSUMMARY:My Test\n  ";
    int j0, j1;
    int k0, k1;
    int n0, n1;
    int v0, v1;
    int l;

    i++;
    printf("TEST %i in '%s:%i': detectNextBlock() = ", i, __FILE__, __LINE__);

    if (detectNextBlock(pchT, k0, k1, &j0, &k0, &k1, &j1) == TRUE) { //
      printf("Error 2 detectNextBlock()\n");
    }
    else if (detectNextLine(pchT, k0, k1, &n0, &n1, &v0, &v1) == TRUE) {
      printf("Error 3 detectNextBlock()\n");
    }
    else if (detectNextLine(pchT, v1, k1, &n0, &n1, &v0, &v1) == FALSE) {
      printf("Error 3 detectNextBlock()\n");
    }
    else if (detectNextLine(pchT, v1, k1, &n0, &n1, &v0, &v1) == TRUE) {
      printf("Error 3 detectNextBlock()\n");
    }
    else if (detectNextLine(pchT, v1, k1, &n0, &n1, &v0, &v1) == FALSE) {
      printf("Error 3 detectNextBlock()\n");
    }
    else if (detectNextLine(pchT, v1, k1, &n0, &n1, &v0, &v1) == TRUE) {
      printf("Error 3 detectNextBlock()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    char *pchT = "   \n\n BEGIN:VCALENDAR\nAAA BBB;\n\nBEGIN:VEVENT\nDTSTART:20250106\nDTEND:20250109\nSUMMARY:My Test\nEND:VEVENT\nEND:VCALENDAR\nAAA:BBB;";
    int j0, j1;
    int k0, k1;
    int n0, n1;
    int v0, v1;
    int l;

    i++;
    printf("TEST %i in '%s:%i': detectNextBlock() = ", i, __FILE__, __LINE__);

    l = strlen(pchT);
    if (detectNextBlock(pchT, 0, l, &j0, &k0, &k1, &j1) == FALSE || j0 != 6 || k0 != 21 || k1 != 104 || j1 != 117) { //
      printf("Error 2 detectNextBlock()\n");
    }
    else if (detectNextLine(pchT, k0, k1, &n0, &n1, &v0, &v1) == TRUE) {
      printf("Error 3 detectNextBlock()\n");
    }
    else if (detectNextBlock(pchT, v1, l, &j0, &k0, &k1, &j1) == FALSE || j0 != 32  || k0 != 44 || k1 != 93 || j1 != 103) {
      printf("Error 4 detectNextBlock()\n");
    }
    else if (detectNextLine(pchT, k0, k1, &n0, &n1, &v0, &v1) == FALSE || n0 != 45 || n1 != 51 || v0 != 53 || v1 != 61) {
      printf("Error 5 detectNextBlock()\n");
    }
    else if (detectNextLine(pchT, v1, k1, &n0, &n1, &v0, &v1) == FALSE || n0 != 62 || n1 != 66 || v0 != 68 || v1 != 76) {
      printf("Error 6 detectNextBlock()\n");
    }
    else if (detectNextLine(pchT, v1, k1, &n0, &n1, &v0, &v1) == FALSE || n0 != 77 || n1 != 83 || v0 != 85 || v1 != 92) {
      printf("Error 7 detectNextBlock()\n");
    }
    else if (detectNextLine(pchT, v1, k1, &n0, &n1, &v0, &v1) == TRUE) {
      printf("Error 8 detectNextBlock()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    char *pchT = "   VCALENDAR\n\nBBB;\nBEGIN:VCALENDAR\nAAA\nBBB\nBEGIN:VEVENT\nDTSTART:20250106\nDTEND:20250109\nSUMMARY:My Test\nEND:VEVENT\nEND:VCALENDAR\nAAA\nBEGIN:VCALENDAR\nAAA\nBBB\nBEGIN:VEVENT\nDTSTART:20250106\n\nEND:VEVENT\nEND:VCALENDAR";
    xmlDocPtr pdocResult = NULL;
    xmlNodePtr pndFile;
    
    i++;
    printf("TEST %i in '%s:%i': icsParse() = ",i,__FILE__,__LINE__);

    pdocResult = xmlNewDoc(BAD_CAST "1.0");
    pndFile = xmlNewDocNode(pdocResult, NULL, NAME_FILE, NULL); 
    xmlDocSetRootElement(pdocResult,pndFile);
    pdocResult->encoding = xmlStrdup(BAD_CAST "UTF-8"); /* according to conversion in ParseImportNodePlainContent() */

    if (icsParseString(pndFile, "") == TRUE) { //
      printf("Error icsParseString()\n");
    }
    else if (icsParseString(pndFile, pchT) == FALSE) { //
      printf("Error icsParseString()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    domPutNodeString(stderr,"ICS",pndFile);
  }


  if (RUNTEST) {
    
    i++;
    printf("TEST %i in '%s:%i': empty icsParse() = ",i,__FILE__,__LINE__);

    if (icsParse(NULL,NULL) == TRUE) {
      printf("Error icsParse()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }
#endif


  if (RUNTEST) {
    xmlDocPtr pdocResult = NULL;
    xmlNodePtr pndFile;
    resNodePtr prnIcs = NULL;
    
    i++;
    printf("TEST %i in '%s:%i': icsParse() = ",i,__FILE__,__LINE__);

    pdocResult = xmlNewDoc(BAD_CAST "1.0");
    pndFile = xmlNewDocNode(pdocResult, NULL, NAME_FILE, NULL); 
    xmlDocSetRootElement(pdocResult,pndFile);
    pdocResult->encoding = xmlStrdup(BAD_CAST "UTF-8"); /* according to conversion in ParseImportNodePlainContent() */

    if ((prnIcs = resNodeConcatNew(BAD_CAST TESTPREFIX, BAD_CAST "option/ics/2446.ics")) == NULL) {
      printf("Error icsParse()\n");
    }
    else if (icsParse(pndFile,prnIcs) == FALSE) {
      printf("Error icsParse()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    xmlSaveFormatFileEnc(TEMPPREFIX "2446.pie", pdocResult, "UTF-8", 1);
    xmlFreeDoc(pdocResult);
    resNodeFree(prnIcs);
  }

  if (RUNTEST) {
    xmlDocPtr pdocResult = NULL;
    xmlNodePtr pndFile;
    resNodePtr prnIcs = NULL;
    
    i++;
    printf("TEST %i in '%s:%i': icsParse() = ",i,__FILE__,__LINE__);

    pdocResult = xmlNewDoc(BAD_CAST "1.0");
    pndFile = xmlNewDocNode(pdocResult, NULL, NAME_FILE, NULL); 
    xmlDocSetRootElement(pdocResult,pndFile);
    pdocResult->encoding = xmlStrdup(BAD_CAST "UTF-8"); /* according to conversion in ParseImportNodePlainContent() */

    if ((prnIcs = resNodeConcatNew(BAD_CAST TESTPREFIX, BAD_CAST "option/ics/2446-corrupt.ics")) == NULL) {
      printf("Error icsParse()\n");
    }
    else if (icsParse(pndFile,prnIcs) == FALSE) {
      printf("Error icsParse()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    xmlSaveFormatFileEnc(TEMPPREFIX "2446-corrupt.pie", pdocResult, "UTF-8", 1);
    xmlFreeDoc(pdocResult);
    resNodeFree(prnIcs);
  }

  printf("Result in '%s': %i/%i OK\n\n",__FILE__,n_ok,i);

  return (i - n_ok);
}
