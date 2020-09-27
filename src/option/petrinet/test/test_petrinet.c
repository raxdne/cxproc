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
petrinetTest(cxpContextPtr pccArg)
{
  int i;
  int n_ok;

  n_ok=0;
  i=0;

  if (RUNTEST) {
    petrinet_t *ppnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': construct empty pnet = ",i,__FILE__,__LINE__);

    if (pnetInitKnots(NULL,NULL,pccArg)) {
      printf("Error pnetInitKnots()\n");
    }
    else if (pnetNewKnots(NULL,pccArg) || pnetInitKnots(NULL,NULL,pccArg)) {
      printf("Error pnetNewKnots()\n");
    }
    else if (pnetNewEdges(NULL,pccArg)) {
      printf("Error pnetNewEdges()\n");
    }
    else if (pnetNewPath(NULL,pccArg)) {
      printf("Error pnetNewPath()\n");
    }
    else if ((ppnT = pnetNew(NULL,pccArg)) != NULL) {
      printf("Error pnetNew()\n");
    }
    else {
      pnetPrintKnots(NULL,pccArg);
      pnetPrintEdges(NULL,pccArg);
      n_ok++;
      printf("OK\n");
    }
    pnetFree(ppnT);
  }


  if (RUNTEST) {
    xmlDocPtr pdocTest;
    petrinet_t *ppnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': feed a dummy XML = ",i,__FILE__,__LINE__);

    if ((pdocTest = xmlReadFile(TESTPREFIX "xml/baustelle.pie",NULL,0)) == NULL) {
      printf("Error\n");
    }
    else if ((ppnT = pnetNew(pdocTest,pccArg)) != NULL) {
      printf("Error\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    pnetFree(ppnT);
    xmlFreeDoc(pdocTest);
  }


  if (RUNTEST) {
    xmlDocPtr pdocTest;
    xmlDocPtr pdocPath;
    petrinet_t *ppnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': use a conformant XML = ",i,__FILE__,__LINE__);

    if ((pdocTest = xmlReadFile(TESTPREFIX "option/petrinet/pn-1.pie",NULL,0)) == NULL) {
      printf("Error: xmlReadFile()\n");
    }
    else if ((ppnT = pnetNew(pdocTest,pccArg)) == NULL) {
      printf("Error: pnetNew()\n");
    }
    else if (ppnT->n_s != 16 ||  ppnT->n_t != 16) {
      printf("Error\n");
    }
    else if (pnetSetStart(ppnT,NULL,pccArg) == FALSE
	     || pnetSetStart(ppnT, BAD_CAST "", pccArg) == FALSE
	     || pnetSetStart(ppnT, BAD_CAST "YYY", pccArg) == TRUE
	     || pnetSetStart(ppnT, BAD_CAST "arbeitsblatt", pccArg) == FALSE) {
      printf("Error pnetSetStart()\n");
    }
    else if (pnetSetTarget(ppnT,NULL,pccArg) == FALSE
	     || pnetSetTarget(ppnT, BAD_CAST "", pccArg) == FALSE
	     || pnetSetTarget(ppnT, BAD_CAST "YYY", pccArg) == TRUE
	     || pnetSetTarget(ppnT, BAD_CAST "html_datei", pccArg) == FALSE) {
      printf("Error pnetSetTarget()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    pnetPrintKnots(ppnT,pccArg);
    pnetPrintEdges(ppnT,pccArg);
    pnetFree(ppnT);
    xmlFreeDoc(pdocTest);
  }


  if (RUNTEST) {
    xmlDocPtr pdocTest;
    petrinet_t *ppnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': path search = ",i,__FILE__,__LINE__);

    if ((pdocTest = xmlReadFile(TESTPREFIX "option/petrinet/pn-1.pie",NULL,0)) == NULL) {
      printf("Error: xmlReadFile()\n");
    }
    else if ((ppnT = pnetNew(pdocTest,pccArg)) == NULL) {
      printf("Error: pnetNew()\n");
    }
    else if (ppnT->n_s != 16 || ppnT->s == NULL
	     || ppnT->s[7].i != 7 || ppnT->s[7].type != state || KNOTID(&(ppnT->s[7])) == NULL
	     || statep(&(ppnT->s[7])) == FALSE || state_index(&(ppnT->s[7])) != 7) {
      printf("Error: states\n");
    }
    else if (ppnT->n_t != 16 || ppnT->t == NULL
	     || ppnT->t[9].i != 9 || ppnT->t[9].type != transition || KNOTID(&(ppnT->t[9])) == NULL
	     || transitionp(&(ppnT->t[9])) == FALSE || transition_index(&(ppnT->t[9])) != 9) {
      printf("Error: transitions\n");
    }
    else if (pnetSetStart(ppnT, BAD_CAST "arbeitsblatt", pccArg) == FALSE) {
      printf("Error: pnetSetStart()\n");
    }
    else if (pnetSetTarget(ppnT, BAD_CAST "papiervorlage", pccArg) == FALSE) {
      printf("Error: pnetSetTarget()\n");
    }
    else if (pnetSearchPathForward(ppnT) == FALSE) {
      printf("Error: pnetSearchPathForward()\n");
    }
    else if (pnetSearchBranchBackward(ppnT) == FALSE) {
      printf("Error: pnetSearchBranchBackward()\n");
    }
    else if (pnetSetTarget(ppnT, BAD_CAST "pnm_datei", pccArg) == FALSE) {
      printf("Error: 2. pnetSetTarget()\n");
    }
    else {
      int j = 0;
      int k = 0;

      pnetSetPathMarker(ppnT,FALSE);
      do {
	k++;
	cxpCtxtLogPrint(pccArg, 2, "Loop %i", k);
	if (pnetSearchPathForward(ppnT)) {
	  pnetSetPathMarker(ppnT,TRUE);
	  j++;
	}
      } while (k < LOOPS_MAX && pnetSearchBranchBackward(ppnT));
      
      if (j == 2) {
	n_ok++;
	printf("OK\n");
      }
      else {
	printf("Error\n");
      }
    }

    pnetPrint(ppnT,pccArg);
    pnetFree(ppnT);
    xmlFreeDoc(pdocTest);
  }

  if (RUNTEST) {
    xmlDocPtr pdocTest;
    petrinet_t *ppnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': use a conformant XML = ",i,__FILE__,__LINE__);

    pdocTest = xmlReadFile(TESTPREFIX "option/petrinet/pn-1.pie",NULL,0);
    if ((ppnT = pnetNew(pdocTest,pccArg))) {
      xmlDocPtr pdocPath = NULL;

      pnetPrint(ppnT,pccArg);
      if (pnetSetStart(ppnT, BAD_CAST "arbeitsblatt", pccArg) == TRUE
	  && pnetSetTarget(ppnT, BAD_CAST "html_datei", pccArg) == TRUE
	  && (pdocPath = pnetProcessPathnet(ppnT, TRUE, pccArg)) != NULL) {
	
	cxpCtxtLogPrintDoc(pccArg, 3, "search result, ", pdocPath);
	n_ok++;
	printf("OK\n");
      }
      else {
	printf("Error\n");
      }
      xmlFreeDoc(pdocPath);
    }
    else {
      printf("Error\n");
    }

    pnetFree(ppnT);
    xmlFreeDoc(pdocTest);
  }


  if (RUNTEST) {
    xmlDocPtr pdocTest;
    petrinet_t *ppnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': pnetProcessPathtable() = ",i,__FILE__,__LINE__);

    pdocTest = xmlReadFile(TESTPREFIX "option/petrinet/pn-1.pie",NULL,0);
    if ((ppnT = pnetNew(pdocTest,pccArg))) {
      xmlDocPtr pdocPath = NULL;

      if (pnetSetStart(ppnT, BAD_CAST "arbeitsblatt", pccArg) == TRUE
	&& pnetSetTarget(ppnT, BAD_CAST "html_datei", pccArg) == TRUE
	&& (pdocPath = pnetProcessPathtable(ppnT, pccArg)) != NULL
	&& ppnT->n_paths == 122) {
	
	cxpCtxtLogPrintDoc(pccArg, 1, "search result, ", pdocPath);
	n_ok++;
	printf("OK\n");
      }
      else {
	printf("Error\n");
      }
      xmlFreeDoc(pdocPath);
    }
    else {
      printf("Error\n");
    }

    pnetFree(ppnT);
    xmlFreeDoc(pdocTest);
  }


  if (SKIPTEST) {
    xmlDocPtr pdocTest;
    petrinet_t *ppnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': pnetProcessPathtable() = ",i,__FILE__,__LINE__);

    pdocTest = xmlReadFile(TESTPREFIX "../www/html/pie/browser-cgi/PieTransformations.xml",NULL,0);
    if ((ppnT = pnetNew(pdocTest,pccArg)) != NULL && ppnT->n_s == 59 && ppnT->n_t == 75 && ppnT->n_e == 162) {
      xmlDocPtr pdocPath = NULL;

      if (pnetSetStart(ppnT, BAD_CAST "text/plain", pccArg) == TRUE
	&& pnetSetTarget(ppnT, BAD_CAST "text/html", pccArg) == TRUE
	&& (pdocPath = pnetProcessPathtable(ppnT, pccArg)) != NULL
	&& ppnT->n_paths == 3) {
	
	cxpCtxtLogPrintDoc(pccArg, 1, "search result, ", pdocPath);
	n_ok++;
	printf("OK\n");
      }
      else {
	printf("Error\n");
      }
      xmlFreeDoc(pdocPath);
    }
    else {
      printf("Error\n");
    }

    pnetFree(ppnT);
    xmlFreeDoc(pdocTest);
  }


  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': pnetProcessNode() = ",i,__FILE__,__LINE__);

    if (pnetProcessNode(NULL,pccArg) == NULL) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error\n");
    }
  }


  printf("Result in '%s': %i/%i OK\n\n", __FILE__, n_ok, i);

  return (i - n_ok);
}
/* end of petrinetTest() */
