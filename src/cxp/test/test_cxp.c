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
cxpTest(cxpContextPtr pccArg)
{
  int n_ok;
  int i;
  cxpContextPtr pccTest = NULL;
  resNodePtr prnS = NULL;

  n_ok=0;
  i=0;

  if ((pccTest = cxpCtxtDup(pccArg)) == NULL) {
    printf("Error cxpCtxtDup()\n");
    return 1;
  }
  else if ((prnS = resNodeStrNew(BAD_CAST TESTPREFIX)) == NULL) {
    printf("Error resNodeStrNew()\n");
    return 1;
  }
  else {
    cxpCtxtSearchSet(pccTest,prnS);
    cxpCtxtLocationSet(pccTest,prnS);
    resNodeFree(prnS);
  }

  
  if (RUNTEST) {
    xmlDocPtr pdocT;

    i++;
    printf("TEST %i in '%s:%i': cxpInfo() = ",i,__FILE__,__LINE__);

    if ((pdocT = cxpProcessInfoNode(NULL,pccArg)) == NULL) {
      printf("Error cxpProcessInfoNode()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //cxpCtxtLogPrintDoc(pccArg, 1, "cxpInfo()", pdocT);
    xmlFreeDoc(pdocT);
  }


  if (RUNTEST) {
    xmlNodePtr pndRoot;
    resNodePtr prnT = NULL;
    xmlChar *pucT;

    i++;
    printf("TEST %i in '%s:%i': cxpAttributeLocatorResNodeNew() = ",i,__FILE__,__LINE__);

    pndRoot = xmlNewNode(NULL,NAME_PLAIN);
    xmlSetProp(pndRoot,BAD_CAST"context",BAD_CAST TESTPREFIX);

    if ((pucT = resPathCollapse(BAD_CAST TESTPREFIX,FS_PATH_SEP | FS_PATH_SELF | FS_PATH_UP)) == NULL) {
      printf("Error resPathCollapse()\n");
    }
    else if ((prnT = cxpAttributeLocatorResNodeNew(pccArg,pndRoot,NULL)) == NULL) {
      printf("Error cxpAttributeLocatorResNodeNew()\n");
    }   
    else if (resPathIsEquivalent(resNodeGetNameNormalized(prnT), pucT) == FALSE) {
      printf("Error resPathIsEquivalent()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    xmlFree(pucT);
    resNodeFree(prnT);
    xmlFreeNode(pndRoot);
  }


  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': cxpResNodeResolveNew() = ",i,__FILE__,__LINE__);

    if (cxpResNodeResolveNew(pccArg,NULL,NULL,0) != NULL) {
      printf("Error 1\n");
    }
    else if (cxpResNodeResolveNew(NULL, NULL, NULL, CXP_O_READ) != NULL) {
      printf("Error 2\n");
    }
    else if (cxpResNodeResolveNew(NULL, NULL, NULL, CXP_O_NONE) != NULL) {
      printf("Error 3\n");
    }
    else if (cxpResNodeResolveNew(NULL, NULL, BAD_CAST"", 0) != NULL) {
      printf("Error 4\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    resNodePtr prnTT = NULL;

    i++;
    printf("TEST %i in '%s:%i': cxpResNodeResolveNew() = 'stdin' shortcut string", i, __FILE__, __LINE__);

    if ((prnT = cxpResNodeResolveNew(pccTest, NULL, BAD_CAST "-", CXP_O_READ)) == NULL) {
      printf("Error 1\n");
    }
    else if (resNodeIsReadable(prnT) == FALSE) {
      printf("Error 2\n");
    }
    else if ((prnTT = cxpResNodeResolveNew(pccTest, NULL, BAD_CAST "-", CXP_O_WRITE)) == NULL) {
      printf("Error 3\n");
    }
    else if (resNodeIsReadable(prnTT) == TRUE) {
      printf("Error 4\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnTT);
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': cxpResNodeResolveNew() = non-existing context by shortcut string", i, __FILE__, __LINE__);

    if ((prnT = cxpResNodeResolveNew(pccTest, NULL, BAD_CAST "dummy/config.cxp", CXP_O_READ)) != NULL) {
      printf("Error 1\n");
    }
    else if((prnT = cxpResNodeResolveNew(pccTest, NULL, BAD_CAST TESTPREFIX "dummy/config.cxp", CXP_O_READ)) != NULL) {
      printf("Error 2\n");
    }
    else if ((prnT = cxpResNodeResolveNew(pccTest, NULL, BAD_CAST "not.cxp", CXP_O_SEARCH)) != NULL) {
      printf("Error 5\n");
    }
    else if ((prnT = cxpResNodeResolveNew(pccTest, NULL, BAD_CAST "circular/config-circular.cxp", CXP_O_READ)) != NULL) {
      printf("Error 2\n");
    }
    else if (resNodeIsReadable(prnT) == TRUE) {
      printf("Error 4\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    BOOL_T fResult = TRUE;
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': cxpResNodeResolveNew() = existing context by shortcut string", i, __FILE__, __LINE__);

    if ((prnT = cxpResNodeResolveNew(pccTest, NULL, BAD_CAST "plain/config.cxp", CXP_O_READ)) == NULL) {
      printf("Error 1\n");
      fResult = FALSE;
    }
    resNodeFree(prnT);

    if((prnT = cxpResNodeResolveNew(pccTest, NULL, BAD_CAST TESTPREFIX "plain/config.cxp", CXP_O_READ | CXP_O_FILE)) == NULL) {
      printf("Error 2\n");
      fResult = FALSE;
    }
    resNodeFree(prnT);

    if((prnT = cxpResNodeResolveNew(pccTest, NULL, BAD_CAST TESTPREFIX "plain/config.cxp", CXP_O_WRITE | CXP_O_FILE)) == NULL) {
      printf("Error 3\n");
      fResult = FALSE;
    }
    resNodeFree(prnT);

    if((prnT = cxpResNodeResolveNew(pccTest, NULL, BAD_CAST TESTPREFIX "plain/config.cxp", CXP_O_READ | CXP_O_DIR)) != NULL) {
      printf("Error 4\n");
      fResult = FALSE;
    }
    resNodeFree(prnT);

    if((prnT = cxpResNodeResolveNew(pccTest, NULL, BAD_CAST TESTPREFIX "plain/", CXP_O_READ | CXP_O_FILE)) != NULL) {
      printf("Error 5\n");
      fResult = FALSE;
    }
    resNodeFree(prnT);

    if((prnT = cxpResNodeResolveNew(pccTest, NULL, BAD_CAST TESTPREFIX "plain", CXP_O_WRITE | CXP_O_DIR)) == NULL) {
      printf("Error 6\n");
      fResult = FALSE;
    }
    resNodeFree(prnT);

    if((prnT = cxpResNodeResolveNew(pccTest, NULL, BAD_CAST TESTPREFIX, CXP_O_READ | CXP_O_DIR)) == NULL) {
      printf("Error 7\n");
      fResult = FALSE;
    }
    resNodeFree(prnT);

    if ((prnT = cxpResNodeResolveNew(pccTest, NULL, BAD_CAST "circular/config-circular.cxp", CXP_O_SEARCH | CXP_O_FILE | CXP_O_READ)) == NULL) {
      printf("Error 8\n");
      fResult = FALSE;
    }
    resNodeFree(prnT);

    if ((prnT = cxpResNodeResolveNew(pccTest, NULL, BAD_CAST "circular/config-circular.cxp", CXP_O_SEARCH | CXP_O_DIR)) == NULL) {
      printf("Error 9\n");
      fResult = FALSE;
    }
    resNodeFree(prnT);

    if (fResult) {
      n_ok++;
      printf("OK\n");
    }
  }


  if (RUNTEST) {
    xmlNodePtr pndRoot;
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': cxpResNodeResolveNew() = non-existing context by node attribute", i, __FILE__, __LINE__);

    if ((pndRoot = xmlNewNode(NULL, NAME_PLAIN)) == NULL) {
      printf("Error 1\n");
    }
    else if ((xmlSetProp(pndRoot, BAD_CAST"name", BAD_CAST "dummy/config.cxp")) == NULL
      || (prnT = cxpResNodeResolveNew(pccTest, pndRoot, NULL, CXP_O_READ)) != NULL) {
      printf("Error 2\n");
    }
    else if ((xmlSetProp(pndRoot, BAD_CAST"name", BAD_CAST TESTPREFIX "dummy/config.cxp")) == NULL
      || (prnT = cxpResNodeResolveNew(pccTest, pndRoot, NULL, CXP_O_READ)) != NULL) {
      printf("Error 3\n");
    }
    else if ((xmlSetProp(pndRoot, BAD_CAST"name", BAD_CAST "not.cxp")) == NULL
      || (prnT = cxpResNodeResolveNew(pccTest, pndRoot, NULL, CXP_O_SEARCH)) != NULL) {
      printf("Error 4\n");
    }
    else if ((xmlSetProp(pndRoot, BAD_CAST"name", BAD_CAST "circular/config-circular.cxp")) == NULL
      || (prnT = cxpResNodeResolveNew(pccTest, pndRoot, NULL, CXP_O_READ)) != NULL) {
      printf("Error 5\n");
    }
    else if ((prnT = cxpResNodeResolveNew(pccTest, pndRoot, NULL, (CXP_O_SEARCH | CXP_O_READ))) == NULL) {
      printf("Error 6\n");
    }
    else if (resNodeIsReadable(prnT) == FALSE) {
      printf("Error 7\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
    xmlFreeNode(pndRoot);
  }


  if (RUNTEST) {
    xmlNodePtr pndRoot;
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': cxpResNodeResolveNew() = node attribute", i, __FILE__, __LINE__);

    if ((pndRoot = xmlNewNode(NULL, NAME_DIR)) == NULL
	|| (xmlSetProp(pndRoot, BAD_CAST"name", BAD_CAST "xml/config.cxp")) == NULL) {
      printf("Error 1\n");
    }
    else if ((prnT = cxpResNodeResolveNew(pccTest, pndRoot, NULL, CXP_O_READ | CXP_O_DIR)) == NULL) {
      printf("Error 6\n");
    }
    else if (resNodeIsReadable(prnT) == FALSE) {
      printf("Error 7\n");
    }
    else if (resNodeIsDir(prnT) == FALSE) {
      printf("Error 8\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
    xmlFreeNode(pndRoot);
  }


  if (RUNTEST) {
    xmlNodePtr pndRoot;
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': cxpResNodeResolveNew() = existing archive context by node attribute", i, __FILE__, __LINE__);

    if ((pndRoot = xmlNewNode(NULL, NAME_PLAIN)) == NULL
      || (xmlSetProp(pndRoot, BAD_CAST"name", BAD_CAST TESTPREFIX "option\\archive\\test-zip-7.zip/sub/plain.txt")) == NULL) {
      printf("Error 3\n");
    }
    else if ((prnT = cxpResNodeResolveNew(pccTest, pndRoot, NULL, CXP_O_READ)) == NULL) {
      printf("Error 3\n");
    }
    else if (resNodeIsReadable(prnT) == FALSE) {
      printf("Error 4\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
    xmlFreeNode(pndRoot);
  }

#if TESTHTTP
  if (SKIPTEST) {
    xmlNodePtr pndRoot;
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': cxpResNodeResolveNew() = existing URL context by node attribute", i, __FILE__, __LINE__);

    pndRoot = xmlNewNode(NULL, NAME_PLAIN);
    xmlSetProp(pndRoot, BAD_CAST"name", BAD_CAST HTTPPREFIX "test-zip-7.zip");

    if ((prnT = cxpResNodeResolveNew(pccTest, pndRoot, NULL, CXP_O_READ)) == NULL) {
      printf("Error 3\n");
    }
    else if (resNodeIsReadable(prnT) == TRUE) {
      printf("Error 4\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
  }
#endif


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    resNodePtr prnTT = NULL;
    xmlDocPtr pdocT = NULL;
    xmlNodePtr pndRoot;
    xmlNodePtr pndT;

    i++;
    printf("TEST %i in '%s:%i': cxpResNodeResolveNew() = non-existing context by node attribute", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "option/pie/text/config.cxp")) == NULL) {
    }
    else if ((pdocT = resNodeReadDoc(prnT)) == NULL || (pndRoot = xmlDocGetRootElement(pdocT)) == NULL) {
    }
    else if ((pndT = domGetFirstChild(pndRoot, BAD_CAST"xml")) == NULL) {
      printf("Error 3\n");
    }
    else if ((prnTT = cxpResNodeResolveNew(pccTest, pndT, NULL, CXP_O_READ)) == NULL) {
      printf("Error 3\n");
    }
    else if (resPathIsEquivalent(resNodeGetNameNormalized(prnTT),BAD_CAST TESTPREFIX "option/pie/text/xml/config-pie-import-xml.cxp") == FALSE) {
      printf("Error 4\n");
    }
    else if (resNodeIsReadable(prnTT) == FALSE) {
      printf("Error 4\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnTT);
    xmlFreeDoc(pdocT);
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': cxpResNodeResolveNew() = existing context", i, __FILE__, __LINE__);

    if ((prnT = cxpResNodeResolveNew(pccArg, NULL, BAD_CAST TESTPREFIX "xml/config-xml-subst-dialog.cxp", CXP_O_READ)) == NULL) {
      printf("Error 3\n");
    }
    else if (resNodeIsReadable(prnT) == FALSE) {
      printf("Error 4\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    xmlNodePtr pndRoot;
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': cxpResNodeResolveNew() = existing context using node",i,__FILE__,__LINE__);

    if ((pndRoot = xmlNewNode(NULL, NAME_PLAIN)) == NULL
      || (xmlSetProp(pndRoot, BAD_CAST"name", BAD_CAST TESTPREFIX "each/simple.txt")) == NULL) {
      printf("Error 3\n");
    }
    else if ((prnT = cxpResNodeResolveNew(pccTest, pndRoot, NULL, CXP_O_READ)) == NULL) {
      printf("Error 1\n");
    }
    else if (resNodeIsReadable(prnT) == FALSE) {
      printf("Error 2\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
    xmlFreeNode(pndRoot);
  }


  if (RUNTEST) {
    xmlNodePtr pndRoot;
    xmlNodePtr pndT;
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': cxpResNodeResolveNew() = existing context using node", i, __FILE__, __LINE__);

    if ((pndRoot = xmlNewNode(NULL, NAME_PLAIN)) == NULL
      || (xmlSetProp(pndRoot, BAD_CAST"context", BAD_CAST TESTPREFIX)) == NULL) {
      printf("Error 3\n");
    }
    else if ((pndT = xmlNewChild(pndRoot,NULL, NAME_XML, NULL)) == NULL
      || (pndT = xmlNewChild(pndRoot, NULL, NAME_XML, NULL)) == NULL) {
      printf("Error 3\n");
    }
    else if ((pndT = xmlNewChild(pndT, NULL, NAME_FILE, NULL)) == NULL
      || (xmlSetProp(pndT, BAD_CAST"name", BAD_CAST "plain\\test-unicode16-bom.txt")) == NULL) {
      printf("Error 3\n");
    }
    else if ((prnT = cxpResNodeResolveNew(pccArg, pndT, NULL, CXP_O_READ)) == NULL) {
      printf("Error 1\n");
    }
    else if (resNodeIsReadable(prnT) == FALSE) {
      printf("Error 2\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST "split result", pndRoot);
    resNodeFree(prnT);
    xmlFreeNode(pndRoot);
  }


  if (RUNTEST) {
    xmlNodePtr pndRoot;
    xmlNodePtr pndT;
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': cxpResNodeResolveNew() = existing context using node", i, __FILE__, __LINE__);

    if ((pndRoot = xmlNewNode(NULL, NAME_PLAIN)) == NULL
      || (xmlSetProp(pndRoot, BAD_CAST"context", BAD_CAST TESTPREFIX "././xml")) == NULL) {
      printf("Error 3\n");
    }
    else if ((pndT = xmlNewChild(pndRoot, NULL, NAME_PLAIN, NULL)) == NULL
      || (pndT = xmlNewChild(pndRoot, NULL, NAME_PLAIN, NULL)) == NULL) {
      printf("Error 3\n");
    }
    else if ((pndT = xmlNewChild(pndT, NULL, NAME_PLAIN, NULL)) == NULL
      || (xmlSetProp(pndT, BAD_CAST"name", BAD_CAST "sub1\\sub11\\dummy-a.xml")) == NULL) {
      printf("Error 3\n");
    }
    else if ((prnT = cxpResNodeResolveNew(pccArg, pndT, NULL, CXP_O_WRITE)) == NULL) {
      printf("Error 1\n");
    }
    else if (resNodeIsReadable(prnT) == FALSE) {
      printf("Error 2\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST "split result", pndRoot);
    resNodeFree(prnT);
    xmlFreeNode(pndRoot);
  }


  if (RUNTEST) {
    xmlNodePtr pndRoot;
    xmlNodePtr pndT;
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': cxpResNodeResolveNew() = non-existing context in a writable directory", i, __FILE__, __LINE__);

    if ((pndRoot = xmlNewNode(NULL, NAME_PLAIN)) == NULL
      || (xmlSetProp(pndRoot, BAD_CAST"name", BAD_CAST "new.txt")) == NULL) {
      printf("Error 3\n");
    }
    else if ((pndT = xmlNewChild(pndRoot, NULL, NAME_PLAIN, BAD_CAST"TEST")) == NULL) {
      printf("Error 3\n");
    }
    else if ((prnT = cxpResNodeResolveNew(pccTest, pndRoot, NULL, CXP_O_WRITE)) == NULL) {
      printf("Error 1\n");
    }
    else if (resNodeIsExist(prnT) == TRUE) {
      printf("Error 2\n");
    }
    else if (resNodeIsWriteable(prnT) == FALSE) {
      printf("Error 2\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //resNodeUnlink(prnT,FALSE);
    resNodeFree(prnT);
    xmlFreeNode(pndRoot);
  }


  if (RUNTEST) {
    xmlNodePtr pndCopy;

    i++;
    printf("TEST %i in '%s:%i': cxpProcessCopyNode() = dummy arguments", i, __FILE__, __LINE__);

    if ((pndCopy = xmlNewNode(NULL, NAME_FILECOPY)) == NULL) {
      printf("Error 1\n");
    }
    else if (cxpProcessCopyNode(pndCopy,pccTest) == TRUE) {
      printf("Error 1b\n");
    }
    else if ((xmlSetProp(pndCopy, BAD_CAST"from", BAD_CAST TESTPREFIX "dummy/abc.pie")) == NULL) {
      printf("Error 2\n");
    }
    else if (cxpProcessCopyNode(pndCopy,pccTest) == TRUE) {
      printf("Error 3\n");
    }
    else if ((xmlSetProp(pndCopy, BAD_CAST"from", BAD_CAST TESTPREFIX "xml/baustelle.pie")) == NULL) {
      printf("Error 4\n");
    }
    else if (cxpProcessCopyNode(pndCopy,pccTest) == TRUE) {
      printf("Error 5\n");
    }
    else if ((xmlSetProp(pndCopy, BAD_CAST"to", BAD_CAST TEMPPREFIX "dummy/abc.pie")) == NULL) {
      printf("Error 6\n");
    }
    else if (cxpProcessCopyNode(pndCopy,pccTest) == FALSE) {
      printf("Error 7\n");
    }
    else if (resNodeTestDirStr(BAD_CAST TEMPPREFIX "dummy") == FALSE) {
      printf("Error 8\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "dummy/abc.pie") == FALSE) {
      printf("Error 9\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //cxpCtxtLogPrintNode(pccArg, 1,"COPY",pndCopy);
    xmlFreeNode(pndCopy);
  }


  if (RUNTEST) {
    xmlNodePtr pndSystem;

    i++;
    printf("TEST %i in '%s:%i': process system node", i, __FILE__, __LINE__);

    if ((pndSystem = xmlNewNode(NULL, NAME_SYSTEM)) == NULL) {
      printf("Error 1\n");
    }
    else if (cxpProcessSystemNode(pndSystem, pccTest) == TRUE) {
      printf("Error 2\n");
    }
    else if ((xmlSetProp(pndSystem, BAD_CAST"mkdir", BAD_CAST TEMPPREFIX "a/b/c/d/e/")) == NULL) {
      printf("Error 3\n");
    }
    else if (cxpProcessSystemNode(pndSystem, pccTest) == FALSE) {
      printf("Error 4\n");
    }
    else if (resNodeTestDirStr(BAD_CAST TEMPPREFIX "a/b/c/d/e/") == FALSE) {
      printf("Error 5\n");
    }
    else if (xmlUnsetProp(pndSystem, BAD_CAST"mkdir") != 0
      || (xmlSetProp(pndSystem, BAD_CAST"rmdir", BAD_CAST TEMPPREFIX "a")) == NULL
      || (xmlSetProp(pndSystem, BAD_CAST"recursive", BAD_CAST "yes")) == NULL) {
      printf("Error 6\n");
    }
    else if (cxpProcessSystemNode(pndSystem, pccTest) == FALSE) {
      printf("Error 7\n");
    }
    else if (resNodeTestDirStr(BAD_CAST TEMPPREFIX "a") == TRUE) {
      printf("Error 8\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //cxpCtxtLogPrintNode(pccArg, 1,"SYSTEM",pndSystem);
    xmlFreeNode(pndSystem);
  }


  if (RUNTEST) {
    xmlNodePtr pndCopy;

    i++;
    printf("TEST %i in '%s:%i': cxpProcessCopyNode() = move", i, __FILE__, __LINE__);

    if ((pndCopy = xmlNewNode(NULL, NAME_FILECOPY)) == NULL) {
      printf("Error 1\n");
    }
    else if ((xmlSetProp(pndCopy, BAD_CAST"from", BAD_CAST TEMPPREFIX "dummy/abc.pie")) == NULL) {
      printf("Error 2\n");
    }
    else if ((xmlSetProp(pndCopy, BAD_CAST"to", BAD_CAST TEMPPREFIX "def/hij.pie")) == NULL) {
      printf("Error 3\n");
    }
    else if ((xmlSetProp(pndCopy, BAD_CAST"delete", BAD_CAST "yes")) == NULL) {
      printf("Error 4\n");
    }
    else if (cxpProcessCopyNode(pndCopy,pccTest) == FALSE) {
      printf("Error 5\n");
    }
    else if (resNodeTestDirStr(BAD_CAST TEMPPREFIX "dummy") == FALSE) {
      printf("Error 6\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "dummy/abc.pie") == TRUE) {
      printf("Error 7\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "def/hij.pie") == FALSE) {
      printf("Error 8\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //cxpCtxtLogPrintNode(pccArg, 1,"COPY",pndCopy);
    xmlFreeNode(pndCopy);
  }


  if (RUNTEST) {
    xmlDocPtr pdocT = NULL;
    xmlNodePtr pndRoot;

    i++;
    printf("TEST %i in '%s:%i': cxpProcessXml() = ",i,__FILE__,__LINE__);

    if ((pndRoot = xmlNewNode(NULL, NAME_XML)) == NULL
      || (xmlSetProp(pndRoot, BAD_CAST"name", BAD_CAST"test-plain-3.xml")) == NULL
      || (xmlSetProp(pndRoot, BAD_CAST"search", BAD_CAST"yes")) == NULL) {
      printf("Error 1\n");
    }
    else if ((pdocT = cxpProcessXmlNode(pndRoot, pccArg)) != NULL) {
      printf("Error 2 cxpProcessXmlNode()\n");
    }
    else if ((pdocT = cxpProcessXmlNode(pndRoot, pccTest)) == NULL) {
      printf("Error 3 cxpProcessXmlNode()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    xmlFreeDoc(pdocT);
    xmlFreeNode(pndRoot);
  }


#if TESTHTTP
  if (RUNTEST) {
    xmlDocPtr pdocT = NULL;
    xmlNodePtr pndRoot;

    i++;
    printf("TEST %i in '%s:%i': cxpProcessXml() using URL = ", i, __FILE__, __LINE__);

    if ((pndRoot = xmlNewNode(NULL, NAME_XML)) == NULL
      || (xmlSetProp(pndRoot, BAD_CAST"name", BAD_CAST"http://www.tagesschau.de/newsticker.rdf")) == NULL) {
      printf("Error 1\n");
    }
    else if ((pdocT = cxpProcessXmlNode(pndRoot, pccArg)) == NULL) {
      printf("Error 2 cxpProcessXmlNode()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    domPutDocString(stderr, BAD_CAST "RDF ", pdocT);
    xmlFreeDoc(pdocT);
    xmlFreeNode(pndRoot);
  }
#endif


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    resNodePtr prnTT = NULL;
    cxpContextPtr pccT;
    xmlChar mucT[BUFFER_LENGTH];

    i++;
    printf("TEST %i in '%s:%i': set cxp context search path to multiple directories = ", i, __FILE__, __LINE__);

    pccT = cxpCtxtDup(pccArg);
    xmlStrPrintf(mucT, BUFFER_LENGTH-1, "%s//%c/tmp/%c%c%s//%c",
      TESTPREFIX, PATHLIST_SEPARATOR, PATHLIST_SEPARATOR, PATHLIST_SEPARATOR, TEMPPREFIX, PATHLIST_SEPARATOR);

    if ((prnT = resNodeStrNew(mucT)) == NULL) {
      printf("Error resNodeStrNew()\n");
    }
    else if (cxpCtxtSearchSet(pccT, prnT) == FALSE) {
      printf("Error cxpCtxtSearchSet()\n");
    }
    else if ((prnTT = resNodeListFindPath(cxpCtxtSearchGet(pccT), BAD_CAST"config.cxp", RN_FIND_FILE)) == NULL) {
      printf("Error resNodeListFindPath()\n");
    }
    else if (resNodeIsFile(prnTT) == FALSE) {
      printf("Error resNodeIsFile()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    resNodeListFree(prnT);
    cxpCtxtFree(pccT);
  }


  if (RUNTEST) {
    cxpContextPtr pccT;
    cxpContextPtr pccTT;
    cxpContextPtr pccTop;
    resNodePtr prnT = NULL;
    resNodePtr prnTT = NULL;
    xmlChar mucT[BUFFER_LENGTH];

    i++;
    printf("TEST %i in '%s:%i': set cxp context search path to multiple directories = ", i, __FILE__, __LINE__);

    pccTop = cxpCtxtDup(pccArg);

    xmlStrPrintf(mucT, BUFFER_LENGTH-1, "%s//%c/tmp/%c%c%s//%c",
      TESTPREFIX, PATHLIST_SEPARATOR, PATHLIST_SEPARATOR, PATHLIST_SEPARATOR, TEMPPREFIX, PATHLIST_SEPARATOR);

    if ((prnT = resNodeStrNew(mucT)) == NULL) {
      printf("Error resNodeStrNew()\n");
    }
    else if (cxpCtxtSearchSet(pccTop, prnT) == FALSE) {
      printf("Error cxpCtxtSearchSet()\n");
    }
    else if((pccT = cxpCtxtNew()) == NULL || cxpCtxtAddChild(pccTop, pccT) == FALSE) {
      printf("Error cxpCtxtAddChild()\n");
    }
    else if ((pccTT = cxpCtxtNew()) == NULL || cxpCtxtAddChild(pccTop, pccTT) == FALSE) {
      printf("Error cxpCtxtAddChild()\n");
    }
    else if ((prnTT = resNodeListFindPath(cxpCtxtSearchGet(pccTT), BAD_CAST"config.cxp", RN_FIND_FILE)) == NULL) {
      printf("Error resNodeListFindPath()\n");
    }
    else if (resNodeIsFile(prnTT) == FALSE) {
      printf("Error resNodeIsFile()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    resNodeListFree(prnT);
    cxpCtxtFree(pccTop);
  }


  if (RUNTEST) {
    xmlDocPtr pdocT;
    xmlNodePtr pndRoot;

    i++;
    printf("TEST %i in '%s:%i': cxpXslRetrieve() = ",i,__FILE__,__LINE__);

    if ((pndRoot = xmlNewNode(NULL, NAME_XSL)) == NULL
      || (xmlSetProp(pndRoot, BAD_CAST"name", BAD_CAST"xsl/NotValid.xsl")) == NULL
      || (xmlSetProp(pndRoot, BAD_CAST"search", BAD_CAST"yes")) == NULL) {
      printf("Error 1\n");
    }
    else if ((pdocT = cxpXslRetrieve(pndRoot, pccArg)) != NULL) {
      printf("Error 1 cxpXslRetrieve()\n");
    }
    else if ((pdocT = cxpXslRetrieve(pndRoot, pccTest)) == NULL) {
      printf("Error 2 cxpXslRetrieve()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    xmlFreeDoc(pdocT);
    xmlFreeNode(pndRoot);
  }


  if (RUNTEST) {
    xmlDocPtr pdocT;
    xmlNodePtr pndRoot;

    i++;
    printf("TEST %i in '%s:%i': cxpXslRetrieve() = ", i, __FILE__, __LINE__);

    if ((pndRoot = xmlNewNode(NULL, NAME_XSL)) == NULL
      || (xmlSetProp(pndRoot, BAD_CAST"name", BAD_CAST"xsl/TestValidate.xsl")) == NULL
      || (xmlSetProp(pndRoot, BAD_CAST"search", BAD_CAST"yes")) == NULL) {
      printf("Error 3\n");
    }
    else if ((pdocT = cxpXslRetrieve(pndRoot, pccArg)) != NULL) {
      printf("Error cxpXslRetrieve()\n");
    }
    else if ((pdocT = cxpXslRetrieve(pndRoot, pccTest)) == NULL) {
      printf("Error cxpXslRetrieve()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    xmlFreeDoc(pdocT);
    xmlFreeNode(pndRoot);
  }


  if (RUNTEST) {
    xmlChar *pucT = NULL;
    xmlNodePtr pndPlain = NULL;
    xmlNodePtr pndChild;

    i++;
    printf("TEST %i in '%s:%i': cxpProcessPlainNode() = ",i,__FILE__,__LINE__);

    pndPlain = xmlNewNode(NULL,NAME_PLAIN);
    pndChild = xmlNewText(BAD_CAST"Dummy text node");
    xmlAddChild(pndPlain,pndChild);
    pndChild = xmlNewChild(pndPlain,NULL,NAME_PLAIN,BAD_CAST"Dummy plain node");
    pndChild = xmlNewChild(pndPlain,NULL,NAME_SCRIPT,BAD_CAST"'AAA' + 'BBB' + 'CCC'");
    
    //    pndChild = xmlNewChild(pndPlain, NULL, NAME_XSL, NULL);
    //    xmlSetProp(pndChild,BAD_CAST"name",BAD_CAST TESTPREFIX "xsl/TestValidate.xsl");
    
    cxpCtxtLogPrintNode(pccArg, 1, "PLAIN instruction", pndPlain);

    if ((pucT = cxpProcessPlainNode(pndPlain,pccArg)) == NULL) {
      printf("Error cxpProcessPlainNode()\n");
    }
    else if (xmlStrlen(pucT) != 40) {
      printf("Error xmlStrlen()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    cxpCtxtLogPrint(pccArg, 1, "PLAIN result:\n%s", pucT);
    xmlFree(pucT);
    xmlFreeNode(pndPlain);
  }


  if (RUNTEST) {
    xmlChar *pucT = NULL;
    xmlDocPtr pdocXsl = NULL;
    cxpContextPtr pccT;
    char *param[] ={ "str_path", "'/abc.txt'", "pattern", "'htag = 'muller''", "int_a", "123", "flag", "false()", NULL };

    i++;
    printf("TEST %i in '%s:%i': cxpChangeXslParam() = ", i, __FILE__, __LINE__);

    pccT = cxpCtxtDup(pccArg);

    pdocXsl = xmlReadFile(TESTPREFIX "xsl/TestValidate.xsl", NULL, 0);

    if (cxpChangeXslParam(NULL, NULL, NULL, NULL)) {
      printf("Error 1 cxpChangeXslParam()\n");
    }
    else if (cxpChangeXslParam(pdocXsl, NULL, FALSE, pccT)) {
      printf("Error 2 cxpChangeXslParam()\n");
    }
    else if (cxpChangeXslParam(pdocXsl, param, FALSE, pccT) == FALSE) {
      printf("Error 3 cxpChangeXslParam()\n");
    }
    else if (cxpChangeXslParam(pdocXsl, param, TRUE, pccT) == FALSE) {
      printf("Error 4 cxpChangeXslParam()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //cxpCtxtLogPrintDoc(pccArg, 1, "XML result", pdocXsl);

    cxpCtxtFree(pccT);
    xmlFree(pucT);
    xmlFreeDoc(pdocXsl);
  }


  if (RUNTEST) {
    xmlDocPtr pdocT = NULL;
    xmlDocPtr pdocXml = NULL;
    xmlDocPtr pdocXsl = NULL;

    i++;
    printf("TEST %i in '%s:%i': cxpXslTransformToDom() = ",i,__FILE__,__LINE__);

    pdocXml = xmlReadFile(TESTPREFIX "xml/config.cxp",NULL,0);
    pdocXsl = xmlReadFile(TESTPREFIX "xsl/TestValidate.xsl",NULL,0);

    if ((pdocT = cxpXslTransformToDom(pdocXml, pdocXsl, NULL, pccArg)) == NULL) {
      printf("Error cxpXslTransformToDom()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //cxpCtxtLogPrintDoc(pccArg, 1, "XML result", pdocT);
    
    if (pdocT != NULL && pdocT != pdocXml) {
      xmlFreeDoc(pdocT);
    }
    xmlFreeDoc(pdocXml);
    xmlFreeDoc(pdocXsl);
  }

  if (RUNTEST) {
    xmlDocPtr pdocT = NULL;
    xmlDocPtr pdocXml = NULL;
    xmlNodePtr pndXml = NULL;
    xmlNodePtr pndChild;
    xmlNodePtr pndT;

    i++;
    printf("TEST %i in '%s:%i': cxpProcessTransformations() = ",i,__FILE__,__LINE__);

    pndXml = xmlNewNode(NULL,NAME_XML);
    pndChild = xmlNewChild(pndXml,NULL,NAME_XSL,NULL);
    xmlSetProp(pndChild,BAD_CAST"name",BAD_CAST TESTPREFIX "xsl/TestValidate.xsl");
    pndT = xmlNewChild(pndChild, NULL, BAD_CAST "variable", NULL);
    xmlSetProp(pndT, BAD_CAST"name", BAD_CAST "int_test");
    xmlSetProp(pndT, BAD_CAST"select", BAD_CAST "123");
    pndT = xmlNewChild(pndChild, NULL, BAD_CAST "variable", NULL);
    xmlSetProp(pndT, BAD_CAST"name", BAD_CAST "flag_test");
    xmlSetProp(pndT, BAD_CAST"select", BAD_CAST "false()");
    pndT = xmlNewChild(pndChild, NULL, BAD_CAST "variable", NULL);
    xmlSetProp(pndT, BAD_CAST"name", BAD_CAST "str_test");
    xmlSetProp(pndT, BAD_CAST"select", BAD_CAST "'123'");
    pndChild = xmlNewChild(pndXml, NULL, NAME_XSL, NULL);
    xmlSetProp(pndChild,BAD_CAST"name",BAD_CAST TESTPREFIX "xsl/TestValidate.xsl");
    
    //cxpCtxtLogPrintNode(pccArg, 1, "XML instruction", pndXml);

    if ((pdocXml = xmlReadFile(TESTPREFIX "xsl/config.cxp", NULL, 0)) == NULL) {
      printf("Error xmlReadFile()\n");
    }
    else if (cxpProcessTransformations(NULL, NULL, NULL, NULL, pccArg)) {
      printf("Error cxpProcessTransformations()\n");
    }
    else if (cxpProcessTransformations(pdocXml, pndXml, &pdocT, NULL, pccArg) == FALSE && pdocT == NULL) {
      printf("Error cxpProcessTransformations()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //cxpCtxtLogPrintDoc(pccArg, 1, "XML result", pdocT);
    xmlFreeDoc(pdocT);
    xmlFreeDoc(pdocXml);
    xmlFreeNode(pndXml);
  }


  if (RUNTEST) {
    xmlDocPtr pdocT = NULL;
    xmlDocPtr pdocXml = NULL;
    xmlNodePtr pndXml = NULL;
    xmlNodePtr pndChild;
    xmlNodePtr pndT;

    i++;
    printf("TEST %i in '%s:%i': subst in DOM = ", i, __FILE__, __LINE__);

    pndXml = xmlNewNode(NULL, NAME_XML);
    pndChild = xmlNewChild(pndXml, NULL, NAME_XML, NULL);
    xmlSetProp(pndChild, BAD_CAST"name", BAD_CAST TESTPREFIX "xml/baustelle.pie");
    pndChild = xmlNewChild(pndXml, NULL, NAME_SUBST, NULL);
    xmlSetProp(pndChild, BAD_CAST"string", BAD_CAST "Baustelle");
    xmlSetProp(pndChild, BAD_CAST"to", BAD_CAST "12345");
    pndChild = xmlNewChild(pndXml, NULL, NAME_XPATH, NULL);
    xmlSetProp(pndChild, BAD_CAST"select", BAD_CAST "/pie/section/h");

    //cxpCtxtLogPrintNode(pccArg, 1, "XML instruction", pndXml);

    if ((pdocXml = xmlReadFile(TESTPREFIX "xml/baustelle.pie", NULL, 0)) == NULL) {
      printf("Error xmlReadFile()\n");
    }
    else if (cxpProcessTransformations(pdocXml, pndXml, NULL, NULL, pccArg) == TRUE) {
      printf("Error 1 cxpProcessTransformations()\n");
    }
    else if (cxpProcessTransformations(pdocXml, pndXml, &pdocT, NULL, pccArg) == FALSE) {
      printf("Error 2 cxpProcessTransformations()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //cxpCtxtLogPrintDoc(pccArg, 1, "XML result", pdocT);

    xmlFreeDoc(pdocT);
    xmlFreeDoc(pdocXml);
    xmlFreeNode(pndXml);
  }


  if (RUNTEST) {
    xmlChar *pucT = NULL;
    xmlDocPtr pdocXml = NULL;
    xmlNodePtr pndPlain = NULL;
    xmlNodePtr pndChild;
    xmlNodePtr pndT;
    resNodePtr prnT = NULL;
    cxpContextPtr pccT;

    i++;
    printf("TEST %i in '%s:%i': subst in plain text = ", i, __FILE__, __LINE__);

    pccT = cxpCtxtDup(pccArg);

    pndPlain = xmlNewNode(NULL, NAME_PLAIN);
    pndChild = xmlNewChild(pndPlain, NULL, NAME_XML, NULL);
    xmlSetProp(pndChild, BAD_CAST"name", BAD_CAST "xml/baustelle.pie");
    pndChild = xmlNewChild(pndPlain, NULL, NAME_XSL, NULL);
    xmlSetProp(pndChild,BAD_CAST"name",BAD_CAST"Validate.xsl");
    xmlSetProp(pndChild, BAD_CAST"search", BAD_CAST"yes");
    pndChild = xmlNewChild(pndPlain, NULL, NAME_SUBST, NULL);
    xmlSetProp(pndChild, BAD_CAST"string", BAD_CAST "Baustelle");
    xmlSetProp(pndChild, BAD_CAST"to", BAD_CAST "98765");

    //cxpCtxtLogPrintNode(pccArg, 1, "XML instruction", pndPlain);

    if ((prnT = resNodeStrNew(BAD_CAST TESTPREFIX "..//")) == NULL) {
      printf("Error resNodeStrNew()\n");
    }
    else if (cxpCtxtSearchSet(pccT, prnT) == FALSE) {
      printf("Error cxpCtxtSearchSet()\n");
    }
    else if ((pdocXml = xmlReadFile(TESTPREFIX "xml/baustelle.pie", NULL, 0)) == NULL) {
      printf("Error xmlReadFile()\n");
    }
    else if (cxpProcessTransformations(pdocXml, pndPlain, NULL, &pucT, pccT) == FALSE || STR_IS_EMPTY(pucT)) {
      printf("Error 2 cxpProcessTransformations()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //puts((const char *)pucT);
    resNodeFree(prnT);
    cxpCtxtFree(pccT);
    xmlFree(pucT);
    xmlFreeDoc(pdocXml);
    xmlFreeNode(pndPlain);
  }


  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': ValidateSchema() = ",i,__FILE__,__LINE__);

    if (ValidateSchema(NULL, NULL, pccArg) == TRUE) {
      printf("Error ValidateSchema()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }


  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': domValidateTree() = ",i,__FILE__,__LINE__);

    if (domValidateTree(NULL) != NULL) {
      printf("Error domValidateTree()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  cxpCtxtFree(pccTest);

  printf("Result in '%s': %i/%i OK\n\n",__FILE__,n_ok,i);

  return (i - n_ok);
}
