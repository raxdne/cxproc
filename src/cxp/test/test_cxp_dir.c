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


/*! test code for this module 
 */
int
dirTest(cxpContextPtr pccArg)
{
  int n_ok;
  int i;

  n_ok=0;
  i=0;

  if (RUNTEST) {
    xmlDocPtr pdocTest;
    xmlNodePtr pndTestFile;
    xmlNodePtr pndT;

    i++;
    printf("TEST %i in '%s:%i': dirProcessDirNode() = ", i, __FILE__, __LINE__);

    pndTestFile = xmlNewNode(NULL, NAME_FILE);
    xmlSetProp(pndTestFile, BAD_CAST"name", BAD_CAST TESTPREFIX "xml/baustelle.pie");

    if ((pdocTest = dirProcessDirNode(pndTestFile, NULL, pccArg)) == NULL) {
      printf("Error dirProcessDirNode()\n");
    }
    else if ((pndT = xmlDocGetRootElement(pdocTest)) == NULL || IS_NODE_PIE(pndT) == FALSE) {
      printf("Error xmlDocGetRootElement()\n");
    }
    else if ((pndT = domGetFirstChild(pndT, NAME_FILE)) == NULL) {
      printf("Error domGetFirstChild()\n");
    }
    else {
      printf("OK\n");
      n_ok++;
    }
    //domPutDocString(stderr, BAD_CAST "dirProcessFileNode(): ", pdocTest);
    xmlFreeDoc(pdocTest);
    xmlFreeNode(pndTestFile);
  }


  if (RUNTEST) {
    xmlDocPtr pdocTest;
    xmlNodePtr pndTestFile;
    xmlNodePtr pndRoot;
    xmlNodePtr pndT;
    int k = -1;

    i++;
    printf("TEST %i in '%s:%i': parse a file and count number of childs = ",i,__FILE__,__LINE__);

    pndTestFile = xmlNewNode(NULL,NAME_FILE);
    xmlSetProp(pndTestFile, BAD_CAST"name", BAD_CAST TESTPREFIX "xml/test-xml-zip.odt");
    xmlSetProp(pndTestFile, BAD_CAST"verbosity", BAD_CAST"5");

    if ((pdocTest = dirProcessDirNode(pndTestFile, NULL, pccArg)) == NULL) {
      printf("Error dirProcessDirNode()\n");
    }
    else if ((pndRoot = xmlDocGetRootElement(pdocTest)) == NULL || IS_NODE_PIE(pndRoot) == FALSE) {
      printf("Error xmlDocGetRootElement()\n");
    }
    else if ((k = domNumberOfChild(pndRoot, NAME_FILE)) != 1) {
      printf("Error domNumberOfChild() FILE: %i\n", k);
    }
    else if ((k = domNumberOfChild(pndRoot, NAME_DIR)) != 0) {
      printf("Error domNumberOfChild() DIR: %i\n", k);
    }
#ifdef HAVE_LIBARCHIVE
    else if ((pndT = domGetFirstChild(pndRoot, NAME_FILE)) == NULL || (pndT = domGetFirstChild(pndT, NAME_ARCHIVE)) == NULL) {
      printf("Error domGetFirstChild()\n");
    }
#endif
    else {
      printf("OK\n");
      n_ok++;
    }
    //domPutDocString(stderr, BAD_CAST "dirProcessFileNode(): ", pdocTest);
    xmlFreeDoc(pdocTest);
    xmlFreeNode(pndTestFile);
  }


  if (RUNTEST) {
    xmlDocPtr pdocTest;
    xmlNodePtr pndTestDir;
    xmlNodePtr pndT;
    int k = -1;

    i++;
    printf("TEST %i in '%s:%i': parse a directory and count number of childs = ",i,__FILE__,__LINE__);

    pndTestDir = xmlNewNode(NULL,NAME_DIR);
    xmlSetProp(pndTestDir,BAD_CAST"name",BAD_CAST TESTPREFIX);
    //xmlSetProp(pndTestDir, BAD_CAST"verbosity", BAD_CAST"2");
    xmlSetProp(pndTestDir, BAD_CAST"depth", BAD_CAST"2");

    if ((pdocTest = dirProcessDirNode(pndTestDir, NULL, pccArg)) == NULL) {
      printf("Error dirProcessDirNode()\n");
    }
    else if ((pndT = xmlDocGetRootElement(pdocTest)) == NULL) {
      printf("Error xmlDocGetRootElement()\n");
    }
    else if ((pndT = domGetFirstChild(pndT, NAME_DIR)) == NULL) {
      printf("Error domGetFirstChild()\n");
    }
    else if ((k = domNumberOfChild(pndT, NAME_FILE)) != 1) {
      printf("Error domNumberOfChild(): %i\n", k);
    }
    else if ((k = domNumberOfChild(pndT, NAME_DIR)) != 12) {
      printf("Error domNumberOfChild(): %i\n", k);
    }
    else {
      printf("OK\n");
      n_ok++;
    }
    //domPutDocString(stderr, BAD_CAST "dirProcessFileNode(): ", pdocTest);
    xmlFreeDoc(pdocTest);
    xmlFreeNode(pndTestDir);
  }


  if (RUNTEST) {
    xmlChar *pucT;
    xmlNodePtr pndT;
    xmlNodePtr pndTT;
    xmlNodePtr pndPie;
    xmlNodePtr pndDir;
    resNodePtr prnT = NULL;
    resNodePtr prnTT = NULL;
    resNodePtr prnTTT = NULL;
    cxpContextPtr pccT = NULL;

    i++;
    printf("TEST %i in '%s:%i': SetTopPrefix() = ",i,__FILE__,__LINE__);

    pndPie = xmlNewNode(NULL, NAME_PIE);

    if ((prnTTT = resNodeDirNew(BAD_CAST TESTPREFIX)) == NULL) {
      printf("Error resNodeNew()\n");
    }
    else if ((pccT = cxpCtxtDup(pccArg)) == NULL || cxpCtxtRootSet(pccT, prnTTT) == FALSE) {
      printf("Error cxpCtxtRootSetStr()\n");
    }
    else if ((pndDir = xmlNewChild(pndPie, NULL, NAME_DIR, NULL)) == NULL) {
      printf("Error xmlNewChild()\n");
    }
    else if ((pndT = xmlNewChild(pndDir, NULL, NAME_DIR, NULL)) == NULL) {
      printf("Error xmlNewChild()\n");
    }
    else if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX)) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (xmlSetProp(pndT, BAD_CAST"prefix", resNodeGetNameNormalized(prnT)) == FALSE) {
      printf("Error xmlSetProp()\n");
    }
    else if ((pndTT = xmlNewChild(pndDir, NULL, NAME_DIR, NULL)) == NULL) {
      printf("Error xmlNewChild()\n");
    }
    else if ((prnTT = resNodeDirNew(BAD_CAST TESTPREFIX "xml/")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (xmlSetProp(pndTT, BAD_CAST"prefix", resNodeGetNameNormalized(prnTT)) == FALSE) {
      printf("Error xmlSetProp()\n");
    }
    else if (SetTopPrefix(NULL, NULL, NULL) == TRUE) {
      printf("Error 1 SetTopPrefix()\n");
    }
    else if (SetTopPrefix(NULL, NULL, pccT) == TRUE) {
      printf("Error 2 SetTopPrefix()\n");
    }
    else if (SetTopPrefix(pndT, prnT, NULL) == FALSE) {
      printf("Error 3 SetTopPrefix()\n");
    }
    else if (SetTopPrefix(pndDir, prnT, pccT) == FALSE) {
      printf("Error 4 SetTopPrefix()\n");
    }
    else if (domGetAttributeNode(pndT, BAD_CAST"prefix") == NULL) {
      printf("Error domGetAttributeNode()\n");
    }
    else if ((pucT = domGetAttributePtr(pndTT, BAD_CAST"prefix")) == NULL || xmlStrEqual(pucT, resNodeGetNameNormalized(prnTT)) == FALSE) {
      printf("Error xmlStrEqual()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST "SetTopPrefix(): ", pndPie);
    resNodeFree(prnT);
    resNodeFree(prnTT);
    cxpCtxtFree(pccT);
    xmlFreeNode(pndPie);
  }

  
  printf("TEST in '%s': %i/%i OK\n\n",__FILE__,n_ok,i);

  return (i - n_ok);
}
