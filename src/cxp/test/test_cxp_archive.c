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


/*! test code for this module 
 */
int
cxpArcTest(cxpContextPtr pccArg)
{
  int n_ok;
  int i;

  n_ok=0;
  i=0;

  if (RUNTEST) {
    xmlNodePtr pndZip;

    i++;
    printf("TEST %i in '%s:%i': arcProcessZipNode() = ",i,__FILE__,__LINE__);

    pndZip = xmlNewNode(NULL,NAME_ZIP);
    xmlSetProp(pndZip,BAD_CAST"name",BAD_CAST"tmp/d-3.zip");

    if (arcProcessZipNode(pndZip,pccArg) == NULL) {
      printf("OK\n");
      n_ok++;
    }
    else {
      printf("Error arcProcessZipNode()\n");
    }

    xmlFreeNode(pndZip);
  }


  if (SKIPTEST) {
    xmlNodePtr pndT;
    xmlNodePtr pndZip;
    xmlDocPtr pdocT;

    i++;
    printf("TEST %i in '%s:%i': arcProcessZipNode() = ",i,__FILE__,__LINE__);

    pndT = xmlNewNode(NULL,NAME_XML);
    pndZip = xmlNewChild(pndT,NULL,NAME_ZIP,NULL);
    xmlSetProp(pndZip,BAD_CAST"name",BAD_CAST TESTPREFIX "option/archive/test-zip-7.zip");

    pdocT = arcProcessZipNode(pndZip,pccArg); 
    if (pdocT != NULL) {
      xmlNodePtr pndRoot;

      pndRoot = xmlDocGetRootElement(pdocT);
      if (pndRoot != NULL && domNumberOfChild(pndRoot,BAD_CAST"file") == 4) {
	printf("OK\n");
	n_ok++;
      }
    }
    else {
      printf("Error arcProcessZipNode()\n");
    }

    xmlFreeNode(pndT);
  }


  if (SKIPTEST) {
    xmlNodePtr pndZip;
    xmlNodePtr pndDir;
    xmlNodePtr pndChild;

    i++;
    printf("TEST %i in '%s:%i': parse a directory and zip childs = ",i,__FILE__,__LINE__);

    pndZip = xmlNewNode(NULL,NAME_ZIP);
    xmlSetProp(pndZip,BAD_CAST"name",BAD_CAST"tmp/d-4.zip");

    pndDir = xmlNewChild(pndZip,NULL,NAME_DIR,NULL);
    xmlSetProp(pndDir,BAD_CAST"name",BAD_CAST TESTPREFIX);
    pndChild = xmlNewChild(pndDir,NULL,NAME_FILE,NULL);
    xmlSetProp(pndChild,BAD_CAST"name",BAD_CAST"test.bat");
    pndChild = xmlNewChild(pndDir,NULL,NAME_FILE,NULL);
    xmlSetProp(pndChild,BAD_CAST"name",BAD_CAST"test.mak");
    pndChild = xmlNewChild(pndDir,NULL,NAME_FILE,NULL);
    xmlSetProp(pndChild,BAD_CAST"name",BAD_CAST"dummy.txt");

    if (arcProcessZipNode(pndZip,pccArg)==NULL) {
      printf("OK\n");
      n_ok++;
    }
    else {
      printf("Error arcProcessZipNode()\n");
    }
    xmlFreeNode(pndZip);
  }
  
  if (SKIPTEST) {
    xmlNodePtr pndZip;
    xmlNodePtr pndDir;
    xmlNodePtr pndChild;

    i++;
    printf("TEST %i in '%s:%i': parse a directory and zip childs = ",i,__FILE__,__LINE__);

    pndZip = xmlNewNode(NULL,NAME_ZIP);
    xmlSetProp(pndZip,BAD_CAST"name",BAD_CAST"tmp/d-5.zip");

    pndDir = xmlNewChild(pndZip,NULL,NAME_PLAIN,BAD_CAST TESTPREFIX "test\test.bat\ntest/test.mak\ntest/dummy.txt\n");

    if (arcProcessZipNode(pndZip,pccArg)==NULL) {
      printf("OK\n");
      n_ok++;
    }
    else {
      printf("Error arcProcessZipNode()\n");
    }
    xmlFreeNodeList(pndZip);
  }


  if (SKIPTEST) {
    resNodePtr prnArchive = NULL;

    i++;
    printf("TEST %i in '%s:%i': compress res node into archive = ", i, __FILE__, __LINE__);

    if (arcAddResNode(NULL, NULL, NULL, pccArg) == TRUE) {
      printf("Error arcAddResNode()\n");
    }
    else if ((prnArchive = resNodeDirNew(BAD_CAST TEMPPREFIX "aaa.zip")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (arcAddResNode(prnArchive, NULL, NULL, pccArg) == TRUE) {
      printf("Error arcAddResNode()\n");
    }
    else if (resNodeOpen(prnArchive, "wa") == FALSE) {
      printf("error of resNodeOpen()\n");
    }
    else if (arcAddResNode(prnArchive, NULL, BAD_CAST"html/sub/123.html", pccArg) == FALSE) {
      printf("Error arcAddResNode()\n");
    }
    else if (arcAddResNode(prnArchive, NULL, BAD_CAST"html/sub-2/456.html", pccArg) == FALSE) {
      printf("Error arcAddResNode()\n");
    }
    else if (resNodeClose(prnArchive) == FALSE) {
      printf("error of resNodeClose()\n");
    }
    else if (resNodeGetSize(prnArchive) < 1) {
      printf("error of resNodeGetSize()\n");
    }
    else {
      printf("OK\n");
      n_ok++;
    }
    resNodeFree(prnArchive);
  }


  if (SKIPTEST) {
    resNodePtr prnArchive = NULL;
    resNodePtr prnT = NULL;
    resNodePtr prnTT = NULL;
    resNodePtr prnTTT = NULL;

    i++;
    printf("TEST %i in '%s:%i': compress res node into archive = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST"sub/")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeAddChildNew(prnT, BAD_CAST"a.txt") == NULL) {
      printf("Error resNodeAddChildNew()\n");
    }
    else if (resNodeAddChildNew(prnT, BAD_CAST"b.xml") == NULL) {
      printf("Error resNodeAddChildNew()\n");
    }
    else if ((prnTTT = resNodeAddChildNew(prnT, BAD_CAST"html/")) == NULL) {
      printf("Error resNodeAddChildNew()\n");
    }
    else if (resNodeAddChildNew(prnTTT, BAD_CAST"c.html") == NULL) {
      printf("Error resNodeAddChildNew()\n");
    }
    else if ((prnArchive = resNodeDirNew(BAD_CAST TEMPPREFIX "ccc.zip")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeOpen(prnArchive, "wa") == FALSE) {
      printf("error of resNodeOpen()\n");
    }
    else if (arcAddResNode(prnArchive, prnT, NULL, pccArg) == FALSE) {
      printf("Error arcAddResNode()\n");
    }
    else if ((prnTT = resNodeDirNew(BAD_CAST"test/")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeAddChildNew(prnTT, BAD_CAST"a.txt") == NULL) {
      printf("Error resNodeAddChildNew()\n");
    }
    else if (resNodeAddChildNew(prnTT, BAD_CAST"b.xml") == NULL) {
      printf("Error resNodeAddChildNew()\n");
    }
    else if ((prnTTT = resNodeAddChildNew(prnTT, BAD_CAST"html/")) == NULL) {
      printf("Error resNodeAddChildNew()\n");
    }
    else if (resNodeAddChildNew(prnTTT, BAD_CAST"c.html") == NULL) {
      printf("Error resNodeAddChildNew()\n");
    }
    else if (arcAddResNode(prnArchive, prnTT, NULL, pccArg) == FALSE) {
      printf("Error arcAddResNode()\n");
    }
    else if ((prnTTT = resNodeAddChildNew(prnTT, BAD_CAST"DUMMY")) == NULL) {
      printf("Error resNodeAddChildNew()\n");
    }
    else if (resNodeSetNameAlias(prnTTT, BAD_CAST"y.html") == NULL) {
      printf("Error resNodeSetNameAlias()\n");
    }
    else if (arcAddResNode(prnArchive, prnTTT, NULL, pccArg) == FALSE) {
      printf("Error arcAddResNode()\n");
    }
    else if (resNodeClose(prnArchive) == FALSE) {
      printf("error of resNodeClose()\n");
    }
    else if (resNodeGetSize(prnArchive) < 1) {
      printf("error of resNodeGetSize()\n");
    }
    else {
      printf("OK\n");
      n_ok++;
    }
    resNodeFree(prnArchive);
    resNodeFree(prnT);
  }


  if (SKIPTEST) {
    xmlChar* pucList = BAD_CAST"test/abc.txt\n\ntest/sub/abc.txt\ntest/def.xml";
    resNodePtr prnArchive = NULL;

    i++;
    printf("TEST %i in '%s:%i': compress list of filenames into archive = ", i, __FILE__, __LINE__);

    if ((prnArchive = resNodeDirNew(BAD_CAST TEMPPREFIX "eee.zip")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeOpen(prnArchive, "wa") == FALSE) {
      printf("error of resNodeOpen()\n");
    }
    else if (arcAddTextList(prnArchive, pucList, pccArg) == FALSE) {
      printf("Error arcAddTextList()\n");
    }
    else if (resNodeClose(prnArchive) == FALSE) {
      printf("error of resNodeClose()\n");
    }
    else {
      printf("OK\n");
      n_ok++;
    }
  }


  if (SKIPTEST) {
    xmlNodePtr pndTestDir;
    xmlNodePtr pndT;
    resNodePtr prnArchive = NULL;

    i++;
    printf("TEST %i in '%s:%i': compress res node into archive = ", i, __FILE__, __LINE__);

    pndTestDir = xmlNewNode(NULL, NAME_DIR);
    xmlSetProp(pndTestDir, BAD_CAST"name", BAD_CAST"test");
    pndT = xmlNewChild(pndTestDir, NULL, NAME_FILE, NULL);
    xmlSetProp(pndT, BAD_CAST"name", BAD_CAST"abc.xml");
    pndT = xmlNewChild(pndTestDir, NULL, NAME_FILE, NULL);
    xmlSetProp(pndT, BAD_CAST"name", BAD_CAST"def.txt");
    //domPutNodeString(stderr, BAD_CAST "dirProcessFileNode(): ", pndTestDir);

    if ((prnArchive = resNodeDirNew(BAD_CAST TEMPPREFIX "ddd.zip")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeOpen(prnArchive, "wa") == FALSE) {
      printf("error of resNodeOpen()\n");
    }
    else if (arcAddNodeList(prnArchive, pndTestDir, pccArg) == FALSE) {
      printf("Error arcAddNodeList()\n");
    }
    else if (resNodeClose(prnArchive) == FALSE) {
      printf("error of resNodeClose()\n");
    }
    else {
      printf("OK\n");
      n_ok++;
    }
    //domPutDocString(stderr, BAD_CAST "dirProcessFileNode(): ", pdocTest);
    xmlFreeNode(pndTestDir);
  }


  printf("TEST in '%s': %i/%i OK\n\n",__FILE__,n_ok,i);

  return (i - n_ok);
}
