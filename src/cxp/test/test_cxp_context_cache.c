/*
  cxproc - Configurable Xml PROCessor

  Copyright (C) 2006..2022 by Alexander Tenbusch

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
cxpCtxtCacheTest(cxpContextPtr pccArg)
{
  int n_ok;
  int i;
  cxpContextPtr pccHere = cxpCtxtDup(pccArg); /*! local copy for testing */

  xmlChar *pucTextAlpha = BAD_CAST "ABC\nDEF\nHIJ\n";
  xmlDocPtr pdocTest;
  xmlNodePtr pndT;
  xmlNodePtr pndTT;

  /*
  */
  pdocTest = xmlNewDoc(BAD_CAST "1.0");
  pndT = xmlNewDocNode(pdocTest, NULL, NAME_MAKE, NULL);
  xmlDocSetRootElement(pdocTest, pndT);
  pdocTest->encoding = xmlStrdup(BAD_CAST "UTF-8");

  pndTT = xmlNewChild(pndT, NULL, NAME_XML, NULL);
  xmlNewChild(pndTT, NULL, BAD_CAST"body", NULL);

  n_ok = 0;
  i=0;


  if (RUNTEST) {
    cxpContextPtr pccT; /* nulled context */

    i++;
    printf("TEST %i in '%s:%i': cxpCtxtCacheEnable() = ", i, __FILE__, __LINE__);

    if ((pccT = cxpCtxtDup(NULL)) == NULL || cxpCtxtLogSetLevel(pccT, 3) != 0) {
      printf("Error cxpCtxtDup()\n");
    }
    else if (cxpCtxtCacheEnable(NULL, TRUE) == TRUE) {
      printf("Error 1\n");
    }
    else if (cxpCtxtCacheEnable(NULL, FALSE) == TRUE) {
      printf("Error 3\n");
    }
    else if (cxpCtxtCacheEnable(pccT, TRUE) == FALSE) {
      printf("Error 4\n");
    }
    else if (cxpCtxtCacheEnable(pccT, FALSE) == TRUE) {
      printf("Error 5\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    cxpCtxtFree(pccT);
  }


  if (RUNTEST) {
    cxpContextPtr pccT;
    resNodePtr prnT = NULL;
    resNodePtr prnTT = NULL;
    xmlChar *pucT = xmlStrdup(pucTextAlpha);
    xmlChar *pucTT = xmlStrdup(pucTextAlpha);
    
    i++;
    printf("TEST %i in '%s:%i': cxpCtxtCacheAppendResNodeEat() = ", i, __FILE__, __LINE__);

    if ((pccT = cxpCtxtDup(NULL)) == NULL || cxpCtxtLogSetLevel(pccT, 3) != 0) {
      printf("Error cxpCtxtDup()\n");
    }
    else if (cxpCtxtCacheEnable(pccT, FALSE) == TRUE) {
      printf("Error 1\n");
    }
    else if (cxpCtxtCacheAppendResNodeEat(NULL, NULL) == TRUE) {
      printf("Error 2\n");
    }
    else if ((prnT = resNodeAliasNew(BAD_CAST"cache-11")) == NULL) {
      printf("Error 3\n");
    }
    else if (resNodeSetContentPtr(prnT,pucT,xmlStrlen(pucT)) != pucT) {
      printf("Error 4\n");
    }
    else if (cxpCtxtCacheAppendResNodeEat(pccT, prnT) == TRUE) {
      printf("Error 5\n");
    }
    else if (cxpCtxtCacheEnable(pccT, TRUE) == FALSE) {
      printf("Error 6\n");
    }
    else if (cxpCtxtCacheAppendResNodeEat(NULL, NULL) == TRUE) {
      printf("Error 7\n");
    }
    if ((prnTT = resNodeAliasNew(BAD_CAST"cache-12")) == NULL) {
      printf("Error 9\n");
    }
    else if (resNodeSetContentPtr(prnTT,pucTT,xmlStrlen(pucTT)) != pucTT) {
      printf("Error 10\n");
    }
    else if (cxpCtxtCacheAppendResNodeEat(pccT, prnTT) == FALSE) {
      printf("Error 11\n");
    }
    else if (cxpCtxtCacheGetResNode(pccT,BAD_CAST"cache-12") != prnTT) {
      printf("Error 12\n");
    }
    else if (xmlStrEqual(cxpCtxtCacheGetBuffer(pccT, BAD_CAST "cache-12"), pucTextAlpha) == FALSE) {
      printf("Error 13\n");
    }
    else if (xmlStrEqual(cxpCtxtCacheGetBuffer(pccT, BAD_CAST "cache-11"), pucTextAlpha) == TRUE) {
      printf("Error 15\n");
    }
    else if (cxpCtxtCacheEnable(pccT, FALSE) == TRUE) {
      printf("Error 16\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    cxpCtxtFree(pccT);
  }

  if (RUNTEST) {
    cxpContextPtr pccT;

    i++;
    printf("TEST %i in '%s:%i': cxpCtxtCacheAppendBuffer() = ", i, __FILE__, __LINE__);

    if ((pccT = cxpCtxtDup(NULL)) == NULL || cxpCtxtLogSetLevel(pccT, 3) != 0) {
      printf("Error cxpCtxtDup()\n");
    }
    else if (cxpCtxtCacheEnable(pccT, FALSE) == TRUE) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheAppendBuffer(NULL, NULL, NULL) == TRUE) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheAppendBuffer(pccT, NULL, BAD_CAST "") == TRUE) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheAppendBuffer(pccT, pucTextAlpha, BAD_CAST "cache-1.txt") == TRUE) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheEnable(pccT, TRUE) == FALSE) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheAppendBuffer(NULL, NULL, NULL) == TRUE) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheAppendBuffer(pccT, NULL, BAD_CAST "") == TRUE) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheAppendBuffer(pccT, pucTextAlpha, BAD_CAST "cache-2.txt") == FALSE) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheAppendBuffer(pccT, pucTextAlpha, BAD_CAST "cache-3.txt") == FALSE) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheAppendBuffer(pccT, pucTextAlpha, BAD_CAST "cache-4.txt") == FALSE) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheGetResNode(pccT, BAD_CAST "cache-1.txt") != NULL) {
      printf("Error \n");
    }
    else if (xmlStrEqual(cxpCtxtCacheGetBuffer(pccT, BAD_CAST "cache-2.txt"), pucTextAlpha) == FALSE) {
      printf("Error \n");
    }
    else if (xmlStrEqual(cxpCtxtCacheGetBuffer(pccT, BAD_CAST "cache-3.txt"), pucTextAlpha) == FALSE) {
      printf("Error \n");
    }
    else if (xmlStrEqual(cxpCtxtCacheGetBuffer(pccT, BAD_CAST "cache-4.txt"), pucTextAlpha) == FALSE) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheEnable(pccT, FALSE) == TRUE) {
      printf("Error \n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    cxpCtxtFree(pccT);
  }

  if (RUNTEST) {
    cxpContextPtr pccT;

    i++;
    printf("TEST %i in '%s:%i': multiple caching ", i, __FILE__, __LINE__);

    if ((pccT = cxpCtxtDup(NULL)) == NULL || cxpCtxtLogSetLevel(pccT, 3) != 0) {
      printf("Error cxpCtxtDup()\n");
    }
    else if (cxpCtxtCacheAppendDoc(pccT, pdocTest, BAD_CAST "cache-1.xml") == TRUE) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheEnable(pccT, TRUE) == FALSE) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheAppendDoc(pccT, pdocTest, BAD_CAST "cache-2.xml") == FALSE) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheAppendDoc(pccT, pdocTest, BAD_CAST "cache-3.xml") == FALSE) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheAppendDoc(pccT, pdocTest, BAD_CAST "cache-4.xml") == FALSE) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheGetResNode(pccT, BAD_CAST "cache-1.xml") != NULL) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheGetResNode(pccT, BAD_CAST "cache-2.xml") == NULL) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheGetResNode(pccT, BAD_CAST "cache-3.xml") == NULL) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheGetResNode(pccT, BAD_CAST "cache-4.xml") == NULL) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheGetDoc(pccT, BAD_CAST "cache-1.xml") != NULL) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheGetDoc(pccT, BAD_CAST "cache-2.xml") == NULL) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheGetDoc(pccT, BAD_CAST "cache-3.xml") == NULL) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheGetDoc(pccT, BAD_CAST "cache-4.xml") == NULL) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheEnable(pccT, FALSE) == TRUE) {
      printf("Error\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    cxpCtxtFree(pccT);
  }

  if (RUNTEST) {
    cxpContextPtr pccT = cxpCtxtDup(pccArg); /*! local copy for testing */
    cxpContextPtr pccTT;
    cxpContextPtr pccTTT;

    i++;
    printf("TEST %i in '%s:%i': caching in cxp tree ",i,__FILE__,__LINE__);

    cxpCtxtAddChild(pccT, cxpCtxtNew());
    cxpCtxtAddChild(pccT, cxpCtxtNew());
    cxpCtxtAddChild(pccT, cxpCtxtNew());
    pccTT = cxpCtxtAddChild(pccT, cxpCtxtNew());
    cxpCtxtAddChild(pccTT, cxpCtxtNew());
    cxpCtxtAddChild(pccTT, cxpCtxtNew());
    pccTTT = cxpCtxtAddChild(pccTT, cxpCtxtNew());
    cxpCtxtAddChild(pccTTT, cxpCtxtNew());
    cxpCtxtAddChild(pccTTT, cxpCtxtNew());

    if (cxpCtxtCacheAppendDoc(pccTT, pdocTest, BAD_CAST "cache-11.xml") == TRUE) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheEnable(pccT, TRUE) == FALSE) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheEnable(pccTT, TRUE) == FALSE) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheEnable(pccTTT, TRUE) == FALSE) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheAppendDoc(pccT, pdocTest, BAD_CAST "cache-12.xml") == FALSE) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheAppendDoc(pccTT, pdocTest, BAD_CAST "cache-13.xml") == FALSE) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheAppendDoc(pccTTT, pdocTest, BAD_CAST "cache-14.xml") == FALSE) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheGetResNode(pccTTT, BAD_CAST "cache-11.xml") != NULL) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheGetResNode(pccT, BAD_CAST "cache-12.xml") == NULL) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheGetResNode(pccTT, BAD_CAST "cache-13.xml") == NULL) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheGetResNode(pccTTT, BAD_CAST "cache-14.xml") == NULL) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheGetDoc(pccT, BAD_CAST "cache-11.xml") != NULL) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheGetDoc(pccT, BAD_CAST "cache-12.xml") == NULL) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheGetDoc(pccTT, BAD_CAST "cache-12.xml") == NULL) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheGetDoc(pccTTT, BAD_CAST "cache-12.xml") == NULL) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheGetDoc(pccTTT, BAD_CAST "cache-13.xml") == NULL) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheGetDoc(pccTT, BAD_CAST "cache-14.xml") != NULL) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheGetDoc(pccTTT, BAD_CAST "cache-14.xml") == NULL) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheEnable(pccTT, FALSE) == TRUE) {
      printf("Error\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    cxpCtxtFree(pccT);
  }

  if (RUNTEST) {
    cxpContextPtr pccT = cxpCtxtDup(pccArg); /*! local copy for testing */
    cxpContextPtr pccTT;
    cxpContextPtr pccTTT;

    i++;
    printf("TEST %i in '%s:%i': caching with symbolic names ", i, __FILE__, __LINE__);

    cxpCtxtAddChild(pccT, cxpCtxtNew());
    cxpCtxtAddChild(pccT, cxpCtxtNew());
    cxpCtxtAddChild(pccT, cxpCtxtNew());
    pccTT = cxpCtxtAddChild(pccT, cxpCtxtNew());
    cxpCtxtAddChild(pccTT, cxpCtxtNew());
    cxpCtxtAddChild(pccTT, cxpCtxtNew());
    pccTTT = cxpCtxtAddChild(pccTT, cxpCtxtNew());
    cxpCtxtAddChild(pccTTT, cxpCtxtNew());
    cxpCtxtAddChild(pccTTT, cxpCtxtNew());

    if (cxpCtxtCacheAppendDoc(pccTT, pdocTest, BAD_CAST "#cache-11#") == TRUE) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheEnable(pccT, TRUE) == FALSE) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheEnable(pccTT, TRUE) == FALSE) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheEnable(pccTTT, TRUE) == FALSE) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheAppendDoc(pccT, pdocTest, BAD_CAST "#cache-12#") == FALSE) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheAppendDoc(pccTT, pdocTest, BAD_CAST "#cache-13#") == FALSE) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheAppendDoc(pccTTT, pdocTest, BAD_CAST "#cache-14#") == FALSE) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheGetResNode(pccTT, BAD_CAST "#cache-11#") != NULL) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheGetResNode(pccT, BAD_CAST "#cache-12#") == NULL) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheGetResNode(pccTT, BAD_CAST "#cache-13#") == NULL) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheGetResNode(pccTTT, BAD_CAST "#cache-14#") == NULL) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheGetDoc(pccT, BAD_CAST "#cache-11#") != NULL) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheGetDoc(pccT, BAD_CAST "#cache-12#") == NULL) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheGetDoc(pccTT, BAD_CAST "#cache-12#") == NULL) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheGetDoc(pccTTT, BAD_CAST "#cache-12#") == NULL) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheGetDoc(pccT, BAD_CAST "#cache-13#") != NULL) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheGetDoc(pccTT, BAD_CAST "#cache-14#") != NULL) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheGetDoc(pccTTT, BAD_CAST "#cache-14#") == NULL) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheEnable(pccTT, FALSE) == TRUE) {
      printf("Error\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    cxpCtxtFree(pccT);
  }

#if 0
  if (RUNTEST) {
    xmlDocPtr pdocTestT;
    xmlNodePtr pndRoot;

    i++;
    printf("TEST %i in '%s:%i': cxpCtxtCacheUnlinkDoc() ", i, __FILE__, __LINE__);

    pdocTestT = cxpCtxtCacheUnlinkDoc(pccT, BAD_CAST "cache-3.xml");

    if (cxpCtxtCacheEnable(pccT, TRUE) == FALSE) {
      printf("Error \n");
    }
    else if (pdocTestT == NULL || (pndRoot = xmlDocGetRootElement(pdocTestT)) == NULL || xmlStrEqual(pndRoot->name, NAME_MAKE) == FALSE) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheGetDoc(pccT, BAD_CAST "cache-3.xml") != NULL) {
      printf("Error\n");
    }
    else {
      cxpCtxtCachePrint(pccT);
      n_ok++;
      printf("OK\n");
    }
    xmlFreeDoc(pdocTestT);
  }

  if (RUNTEST) {
    xmlChar *pucText = NULL;

    i++;
    printf("TEST %i in '%s:%i': cxpCtxtCacheUnlinkBuffer() ", i, __FILE__, __LINE__);

    pucText = cxpCtxtCacheUnlinkBuffer(pccT, BAD_CAST "cache-3.txt");
    
    if (pucText == NULL || xmlStrEqual(pucTextAlpha, pucText) == FALSE) {
      printf("Error \n");
    }
    else if (cxpCtxtCacheGetBuffer(pccT, BAD_CAST "cache-3.txt") != NULL) {
      printf("Error\n");
    }
    else {
      cxpCtxtCachePrint(pccT);
      n_ok++;
      printf("OK\n");
    }
  }
#endif
  
  xmlFreeDoc(pdocTest);
  cxpCtxtFree(pccHere);

  printf("Result in '%s': %i/%i OK\n\n", __FILE__, n_ok, i);

  return (i - n_ok);
}
