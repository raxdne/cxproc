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
    printf("TEST %i in '%s:%i': reading non-existing zip archive = ",i,__FILE__,__LINE__);

    pndZip = xmlNewNode(NULL,NAME_ZIP);
    xmlSetProp(pndZip,BAD_CAST"name",BAD_CAST TEMPPREFIX "d-3.zip");

    if (arcProcessZipNode(pndZip,pccArg) != NULL) {
      printf("Error arcProcessZipNode()\n");
    }
    else {
      printf("OK\n");
      n_ok++;
    }
    xmlFreeNode(pndZip);
  }


  if (RUNTEST) {
    xmlNodePtr pndT;
    xmlNodePtr pndZip;
    xmlDocPtr pdocT;
    xmlNodePtr pndRoot;

    i++;
    printf("TEST %i in '%s:%i': reading zip archive = ", i, __FILE__, __LINE__);

    pndT = xmlNewNode(NULL, NAME_XML);
    pndZip = xmlNewChild(pndT, NULL, NAME_ZIP, NULL);
    xmlSetProp(pndZip, BAD_CAST "name", BAD_CAST TESTPREFIX "option/archive/test-zip-7.zip");

    if ((pdocT = arcProcessZipNode(pndZip, pccArg)) == NULL) {
      printf("Error arcProcessZipNode()\n");
    }
    else if ((pndRoot = xmlDocGetRootElement(pdocT)) == NULL) {
      printf("Error root arcProcessZipNode()\n");
    }
    else if (domNumberOfChild(pndRoot, BAD_CAST "dir") != 1 || domNumberOfChild(pndRoot, BAD_CAST "file") != 1) {
      printf("Error count arcProcessZipNode()\n");
    }
    else {
      printf("OK\n");
      n_ok++;
    }
    //domPutDocString(stderr, BAD_CAST "arcProcessZipNode() result", pdocT);
    xmlFreeDoc(pdocT);
    xmlFreeNode(pndT);
  }


  if (RUNTEST) {
    resNodePtr prnArchive = NULL;

    i++;
    printf("TEST %i in '%s:%i': compress plain list of filenames into archive = ", i, __FILE__, __LINE__);

    if ((prnArchive = resNodeDirNew(BAD_CAST TEMPPREFIX "d-10.tar")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeOpen(prnArchive, "wa") == FALSE) {
      printf("error of resNodeOpen()\n");
    }
    else if (AddTextList(prnArchive, BAD_CAST"test/abc.txt\n\ntest/sub/abc.txt\ntest/def.xml", pccArg) == FALSE) {
      printf("Error AddTextList()\n");
    }
    else if (AddTextList(prnArchive, BAD_CAST"share/doc/cxproc/doc/calendar.txt\nshare/doc/cxproc/doc/cxproc.1", pccArg) == FALSE) {
      printf("Error AddTextList()\n");
    }
    else if (AddTextList(prnArchive, BAD_CAST"share/doc/cxproc/README.md", pccArg) == FALSE) {
      printf("Error AddTextList()\n");
    }
    else if (resNodeClose(prnArchive) == FALSE) {
      printf("error of resNodeClose()\n");
    }
    else if (resNodeReadStatus(prnArchive) == FALSE) {
      printf("error status\n");
    }
    else {
      printf("OK\n");
      n_ok++;
    }
    resNodeFree(prnArchive);
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

    if ((prnArchive = resNodeDirNew(BAD_CAST TEMPPREFIX "d-11.zip")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeOpen(prnArchive, "wa") == FALSE) {
      printf("error of resNodeOpen()\n");
    }
    else if (AddNodeList(prnArchive, pndTestDir, pccArg) == FALSE) {
      printf("Error AddNodeList()\n");
    }
    else if (resNodeClose(prnArchive) == FALSE) {
      printf("error of resNodeClose()\n");
    }
    else {
      printf("OK\n");
      n_ok++;
    }
    resNodeFree(prnArchive);
    xmlFreeNode(pndTestDir);
  }


  if (RUNTEST) {
    xmlNodePtr pndZip;
    xmlNodePtr pndDir;
    xmlNodePtr pndChild;

    i++;
    printf("TEST %i in '%s:%i': parse a directory and zip childs = ",i,__FILE__,__LINE__);

    pndZip = xmlNewNode(NULL,NAME_ZIP);
    xmlSetProp(pndZip,BAD_CAST"name",BAD_CAST TEMPPREFIX "d-12.zip");

    pndDir = xmlNewChild(pndZip,NULL,NAME_DIR,NULL);
    xmlSetProp(pndDir,BAD_CAST"name",BAD_CAST TESTPREFIX);
    pndChild = xmlNewChild(pndDir,NULL,NAME_FILE,NULL);
    xmlSetProp(pndChild,BAD_CAST"name",BAD_CAST"test.bat");
    pndChild = xmlNewChild(pndDir,NULL,NAME_FILE,NULL);
    xmlSetProp(pndChild,BAD_CAST"name",BAD_CAST"test.mak");
    pndChild = xmlNewChild(pndDir,NULL,NAME_FILE,NULL);
    xmlSetProp(pndChild,BAD_CAST"name",BAD_CAST"dummy.txt");
    //domPutNodeString(stderr, BAD_CAST "arcProcessZipNode(): ", pndZip);

    if (arcProcessZipNode(pndZip,pccArg)==NULL) {
      printf("Error arcProcessZipNode()\n");
    }
    else {
      printf("OK\n");
      n_ok++;
    }
    xmlFreeNode(pndZip);
  }  


  printf("TEST in '%s': %i/%i OK\n\n",__FILE__,n_ok,i);

  return (i - n_ok);
}
