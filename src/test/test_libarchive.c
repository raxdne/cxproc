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
zipTest(cxpContextPtr pccArg)
{
  int i;
  int n_ok;

  n_ok=0;
  i=0;

  if (RUNTEST) {
    xmlNodePtr pndZip;
    cxpContextPtr pccT = cxpContextNew(NULL);

    i++;
    printf("TEST %i in '%s:%i': zipProcessZipNode() = ",i,__FILE__,__LINE__);

    pndZip = xmlNewNode(NULL,NAME_ZIP);
    xmlSetProp(pndZip,BAD_CAST"name",BAD_CAST"tmp/d-3.zip");

    if (zipProcessZipNode(pndZip,pccT) == NULL) {
      printf("OK\n");
      n_ok++;
    }
    else {
      printf("Error zipProcessZipNode()\n");
    }

    xmlFreeNode(pndZip);
    cxpCtxtFree(pccT);
  }


  if (RUNTEST) {
    xmlNodePtr pndT;
    xmlNodePtr pndZip;
    xmlDocPtr pdocT;
    cxpContextPtr pccT = cxpContextNew(NULL);

    i++;
    printf("TEST %i in '%s:%i': zipProcessZipNode() = ",i,__FILE__,__LINE__);

    pndT = xmlNewNode(NULL,NAME_XML);
    pndZip = xmlNewChild(pndT,NULL,NAME_ZIP,NULL);
    xmlSetProp(pndZip,BAD_CAST"name",BAD_CAST TESTPREFIX "option/archive/test-zip-7.zip");

    pdocT = zipProcessZipNode(pndZip,pccT); 
    if (pdocT != NULL) {
      xmlNodePtr pndRoot;

      pndRoot = xmlDocGetRootElement(pdocT);
      if (pndRoot != NULL && domNumberOfChild(pndRoot,BAD_CAST"file") == 4) {
	printf("OK\n");
	n_ok++;
      }
    }
    else {
      printf("Error zipProcessZipNode()\n");
    }

    xmlFreeNode(pndT);
    cxpCtxtFree(pccT);
  }


  if (RUNTEST) {
    xmlNodePtr pndZip;
    xmlNodePtr pndDir;
    xmlNodePtr pndChild;
    cxpContextPtr pccT = cxpContextNew(NULL);

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

    if (zipProcessZipNode(pndZip,pccT)==NULL) {
      printf("OK\n");
      n_ok++;
    }
    else {
      printf("Error zipProcessZipNode()\n");
    }
    xmlFreeNode(pndZip);
    cxpCtxtFree(pccT);
  }
  
  if (SKIPTEST) {
    xmlNodePtr pndZip;
    xmlNodePtr pndDir;
    xmlNodePtr pndChild;
    cxpContextPtr pccT = cxpContextNew(NULL);

    i++;
    printf("TEST %i in '%s:%i': parse a directory and zip childs = ",i,__FILE__,__LINE__);

    pndZip = xmlNewNode(NULL,NAME_ZIP);
    xmlSetProp(pndZip,BAD_CAST"name",BAD_CAST"tmp/d-5.zip");

    pndDir = xmlNewChild(pndZip,NULL,NAME_PLAIN,BAD_CAST TESTPREFIX "test\test.bat\ntest/test.mak\ntest/dummy.txt\n");

    if (zipProcessZipNode(pndZip,pccT)==NULL) {
      printf("OK\n");
      n_ok++;
    }
    else {
      printf("Error zipProcessZipNode()\n");
    }
    xmlFreeNodeList(pndZip);
    cxpCtxtFree(pccT);
  }

  return (i - n_ok);
}
/* end of zipTest() */
