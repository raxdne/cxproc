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
cxpArcTest(cxpContextPtr pccArg)
{
  int n_ok;
  int i;

  n_ok=0;
  i=0;

  if (RUNTEST) {
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


  if (RUNTEST) {
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


  if (RUNTEST) {
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


  if (RUNTEST) {
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
