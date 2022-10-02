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
cxpCtxtTest(cxpContextPtr pccArg)
{
  int n_ok;
  int i;

  n_ok=0;
  i=0;


  if (RUNTEST) {
    cxpContextPtr pccT;

    i++;
    printf("TEST %i in '%s:%i': cxpCtxtCliNew() = ",i,__FILE__,__LINE__);

    pccT = cxpCtxtCliNew(-1, NULL, NULL);
    if (pccT != NULL
      && cxpCtxtSetReadonly(pccT, TRUE) == TRUE
      && cxpCtxtIsReadonly(pccT) == TRUE
      && cxpCtxtSetReadonly(pccT, FALSE) == FALSE
      && cxpCtxtIsReadonly(pccT) == FALSE) {
      //cxpCtxtLogInfo(pccT);
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error \n");
    }
    cxpCtxtFree(pccT);
  }


  if (RUNTEST) {
    cxpContextPtr pccT;

    i++;
    printf("TEST %i in '%s:%i': cxpCtxtCliNew() = ",i,__FILE__,__LINE__);

    pccT = cxpCtxtNew();
    if (pccT != NULL
      && cxpCtxtLogGetLevel(pccT) == -1
      && cxpCtxtLogSetLevel(pccT,2) == 0
      && cxpCtxtLogGetLevel(pccT) == 2) {
      //cxpCtxtLogInfo(pccT);
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error \n");
    }
    cxpCtxtFree(pccT);
  }


  if (RUNTEST) {
    cxpContextPtr pccT;

    i++;
    printf("TEST %i in '%s:%i': cxpCtxtEncSetDefaults() = ",i,__FILE__,__LINE__);

    pccT = cxpCtxtCliNew(-1, NULL, NULL);
    if (pccT != NULL && cxpCtxtEncSetDefaults(pccT) && cxpCtxtEncGetFromEnv(pccT) != (iconv_t) -1) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error \n");
    }
    cxpCtxtFree(pccT);
  }


  if (RUNTEST) {
    cxpContextPtr pccT = NULL;
    resNodePtr prnT = NULL;
    resNodePtr prnTT = NULL;
    resNodePtr prnTTT = NULL;

    i++;
    printf("TEST %i in '%s:%i': deny invalid root directory = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX)) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if ((pccT = cxpCtxtCliNew(-1, NULL, NULL)) == NULL || cxpCtxtRootSet(pccT, prnT) == FALSE) {
      printf("Error cxpCtxtRootSet()\n");
    }
    else if (cxpCtxtAccessIsPermitted(pccT, NULL) == TRUE) { /*  */
      printf("Error 1 cxpCtxtAccessIsPermitted()\n");
    }
    else if ((prnTT = resNodeDirNew(BAD_CAST"C:\\User\\Dummy.txt")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (cxpCtxtAccessIsPermitted(pccT, prnTT) == TRUE) { /* in cli mode only!! */
      printf("Error 2 cxpCtxtAccessIsPermitted()\n");
    }
    else if ((prnTTT = resNodeDirNew(BAD_CAST TESTPREFIX "sql/config.cxp")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (cxpCtxtAccessIsPermitted(pccT, prnTTT) == FALSE) { /* in cli mode only!! */
      printf("Error 3 cxpCtxtAccessIsPermitted()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    cxpCtxtFree(pccT);
    resNodeFree(prnTTT);
    resNodeFree(prnTT);
  }


  if (RUNTEST) {
    cxpContextPtr pccT;
    cxpContextPtr pccTT;
    cxpContextPtr pccTop;

    i++;
    printf("TEST %i in '%s:%i': cxpCtxtAddChild() = ", i, __FILE__, __LINE__);

    pccTop = cxpCtxtDup(pccArg);
    pccT = cxpCtxtNew();
    if (pccT != NULL && cxpCtxtAddChild(pccTop,pccT)) {
      pccT = cxpCtxtNew();
      if (pccT != NULL && cxpCtxtAddChild(pccTop,pccT)) {
	pccTT = cxpCtxtNew();
	if (pccTT != NULL && cxpCtxtAddChild(pccT,pccTT)) {
	  if (cxpCtxtRootGet(pccTT) == cxpCtxtRootGet(pccTop)
	      && cxpCtxtLocationGet(pccTT) == cxpCtxtLocationGet(pccTop)
	      && cxpCtxtSearchGet(pccTT) == cxpCtxtSearchGet(pccTop)) {
	    //cxpCtxtLogInfo(pccTop);
	    n_ok++;
	    printf("OK\n");
	  }
	  else {
	    printf("Error \n");
	  }
	}
	else {
	  printf("Error \n");
	}
      }
      else {
	printf("Error \n");
      }
    }
    else {
      printf("Error \n");
    }
    cxpCtxtFree(pccTop);
  }


#if 0
  if (RUNTEST) {
    cxpContextPtr pccT;

    i++;
    printf("TEST %i in '%s:%i': cxpCtxtCliNew() = ", i, __FILE__, __LINE__);

    pccT = cxpCtxtDup(pccArg);
    if (pccT != NULL
      && cxpContextSetArgvNumber(pccT, 2, "abc") == NULL
      && cxpContextGetArgvNumber(pccT, 2) != NULL
      && cxpContextGetArgvNumber(pccArg, 2) == NULL
      && cxpContextSetArgv(pccT, "TEST", "123") == NULL
      && cxpContextGetArgv(pccT, "TEST") != NULL
      && cxpContextGetArgv(pccArg, "TEST") == NULL
      && cxpContextSetEnv(pccT, "TEST", "ABC") == NULL
      && cxpContextGetEnv(pccT, "TEST") != NULL
      && cxpContextGetEnv(pccArg, "TEST") == NULL
      && cxpContextSetCgi(pccT, "test", "dir/abc") == NULL
      && cxpContextGetCgi(pccT, "test") != NULL
      && cxpContextGetCgi(pccArg, "test") == NULL) {
      cxpCtxtLogInfo(pccT);
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error \n");
    }
      cxpCtxtFree(pccT);
  }


#ifdef HAVE_LIBARCHIVE
  if (RUNTEST) {
    resNodePtr prnT = NULL;
    resNodePtr prnChild = NULL;

    i++;
    printf("TEST %i in '%s:%i': archive path = ",i,__FILE__,__LINE__);
    fsSetRoot(NULL);
    prnT = resNodeDirNew(BAD_CAST TESTPREFIX "option/archive/test-zip-7.zip/sub/nonexistant.png");
    if (resNodeIsArchive(prnT)
	&& (prnChild = resNodeGetChild(prnT))
	&& resNodeGetMimeType(prnChild) == MIME_IMAGE_PNG) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resNodeDirNew()\n");
    }
    resNodeFree(prnT);
  }
#endif
#endif

  if (RUNTEST) {
    xmlDocPtr pdocTest;
    xmlNodePtr pndXml;
    xmlNodePtr pndPie;

    i++;
    printf("TEST %i in '%s:%i': cxpCtxtSaveFileNode() = ",i,__FILE__,__LINE__);

    pdocTest = xmlNewDoc(BAD_CAST "1.0");
    pdocTest->encoding = xmlStrdup(BAD_CAST "UTF-8");
    pndXml = xmlNewDocNode(pdocTest, NULL, NAME_XML, NULL); 
    xmlDocSetRootElement(pdocTest,pndXml);
    xmlSetProp(pndXml,BAD_CAST "name",BAD_CAST TEMPPREFIX "aaaÄÖÜbbb.pie");
    pndPie = xmlNewChild(pndXml,NULL,NAME_PIE,NULL);
    xmlSetProp(pndPie,BAD_CAST "class",BAD_CAST "article");

    if (cxpCtxtSaveFileNode(pccArg,pndXml,pdocTest,NULL)) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error\n");
    }
    xmlFreeDoc(pdocTest);
  }


  if (RUNTEST) {
    xmlDocPtr pdocTest;
    xmlNodePtr pndXml;
    xmlNodePtr pndPie;
    resNodePtr prnT = NULL;
    cxpContextPtr pccT;

    i++;
    printf("TEST %i in '%s:%i': cxpCtxtFromAttr() = ",i,__FILE__,__LINE__);

    pdocTest = xmlNewDoc(BAD_CAST "1.0");
    pdocTest->encoding = xmlStrdup(BAD_CAST "UTF-8");
    pndXml = xmlNewDocNode(pdocTest, NULL, NAME_XML, NULL);
    xmlSetProp(pndXml,BAD_CAST "context",BAD_CAST TESTPREFIX);
    xmlDocSetRootElement(pdocTest,pndXml);

    prnT = resNodeConcatNew(BAD_CAST TESTPREFIX,BAD_CAST "xsl/test-xsl-4.xml");
    resNodeChangeDomURL(pdocTest,prnT);

    pccT = cxpCtxtFromAttr(pccArg,pndXml);
    if (resPathIsEquivalent(resNodeGetNameNormalized(cxpCtxtLocationGet(pccT)), BAD_CAST TESTPREFIX)) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error\n");
    }
    resNodeFree(prnT);
    xmlFreeDoc(pdocTest);
  }


  if (RUNTEST) {
    xmlChar* pucTT = NULL;
    xmlChar* pucTTT = NULL;
    xmlDocPtr pdocTest;
    xmlNodePtr pndMake;
    xmlNodePtr pndPie;
    xmlNodePtr pndT;
    xmlNodePtr pndXml;
    resNodePtr prnT = NULL;
    cxpContextPtr pccT = NULL;
    cxpContextPtr pccTT = NULL;

    i++;
    printf("TEST %i in '%s:%i': cxpCtxtFromAttr() = ", i, __FILE__, __LINE__);

    pdocTest = xmlNewDoc(BAD_CAST "1.0");
    pdocTest->encoding = xmlStrdup(BAD_CAST "UTF-8");
    pndMake = xmlNewDocNode(pdocTest, NULL, NAME_MAKE, NULL);
    xmlDocSetRootElement(pdocTest, pndMake);
    xmlSetProp(pndMake, BAD_CAST "dir", BAD_CAST "pwd");
    pndT = xmlNewChild(pndMake,NULL, NAME_XML,NULL);
    pndXml = xmlNewChild(pndT, NULL, NAME_XML, NULL);
    xmlSetProp(pndXml, BAD_CAST "context", BAD_CAST "../..");
    xmlSetProp(pndXml, BAD_CAST "log", BAD_CAST "3");
    pndT = xmlNewChild(pndMake, NULL, NAME_PLAIN, NULL);

    if ((pccT = cxpCtxtFromAttr(pccArg, pndMake)) == NULL) {
      printf("Error 1\n");
    }
    else if ((pucTTT = resPathCollapse(BAD_CAST BUILDPREFIX, FS_PATH_FULL)) == NULL) {
      printf("Error 4\n");
    }
    else if (resPathIsEquivalent(resNodeGetNameNormalized(cxpCtxtLocationGet(pccT)), pucTTT) == FALSE) {
      printf("Error 2\n");
    }
    else if ((pccTT = cxpCtxtFromAttr(pccT, pndXml)) == NULL) {
      printf("Error 3\n");
    }
    else if ((pucTT = resPathCollapse(BAD_CAST BUILDPREFIX "../..", FS_PATH_FULL)) == NULL) {
      printf("Error 4\n");
    }
    else if (resPathIsEquivalent(resNodeGetNameNormalized(cxpCtxtLocationGet(pccTT)), pucTT) == FALSE) {
      printf("Error 5\n");
    }
    else if (cxpCtxtFromAttr(pccT, pndT) != pccT) {
      printf("Error 6\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    //domPutDocString(stderr,BAD_CAST"",pdocTest);
    xmlFree(pucTTT);
    xmlFree(pucTT);
    xmlFreeDoc(pdocTest);
  }


  if (RUNTEST) {
    xmlNodePtr pndXml;
    cxpContextPtr pccT;

    i++;
    printf("TEST %i in '%s:%i': cxpCtxtFromAttr() = ",i,__FILE__,__LINE__);

    pndXml = xmlNewNode(NULL,NAME_XML);
    xmlSetProp(pndXml,BAD_CAST "log",BAD_CAST "3");

    pccT = cxpCtxtFromAttr(pccArg,pndXml);
    if (pccT != NULL && cxpCtxtGetParent(pccT) == pccArg && cxpCtxtLogGetLevel(pccT) == 3) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error\n");
    }
    xmlFreeNode(pndXml);
  }


  printf("Result in '%s': %i/%i OK\n\n", __FILE__, n_ok, i);

  return (i - n_ok);
}
