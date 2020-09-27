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

/* 

resNodeDirConcatNew(xmlChar *pucArgPath, xmlChar *pucArgPathB)

resNodeSetBase(resNodePtr prnArg, xmlChar *pucArg)

resNodeIsReadable(resNodePtr prnArg)

 */


/*! test code for contexts in module fs
 */
int
resNodeTest(void)
{
  int n_ok;
  int i;

  n_ok=0;
  i=0;

  if (RUNTEST) {

    i++;
    printf("TEST %i in '%s:%i': processing of dummy resource node = ", i, __FILE__, __LINE__);

    if (resNodeReset(NULL, NULL) == TRUE) {
      printf("Error 1 resNodeReset()\n");
    }
    else if (resNodeReset(NULL, BAD_CAST"") == TRUE) {
      printf("Error 2 resNodeReset()\n");
    }
    else if (resNodeReadStatus(NULL) == TRUE) {
      printf("Error resNodeReadStatus()\n");
    }
    else if (resNodeIsExist(NULL) == TRUE) {
      printf("Error resNodeIsExist()");
    }
    else if (resNodeIsFile(NULL) == TRUE) {
      printf("Error resNodeIsFile()");
    }
    else if (resNodeIsDir(NULL) == TRUE) {
      printf("Error resNodeIsDir()");
    }
    else if (resNodeIsStd(NULL) == TRUE) {
      printf("Error resNodeIsStd()");
    }
    else if (resNodeDup(NULL,256) != NULL) {
      printf("Error resNodeDup()");
    }
    else if (resNodeGetErrorMsg(NULL) != NULL) {
      printf("Error resNodeGetErrorMsg()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(NULL);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': new filesystem context set to NULL = ", i, __FILE__, __LINE__);

    /* not initialized with a name, handle error without SIGSEG */
    if ((prnT = resNodeNew()) == NULL) {
      printf("Error resNodeNew() ...\n");
    }
    else if (resNodeReadStatus(prnT) == TRUE) {
      printf("Error resNodeReadStatus(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeSetContentPtr(prnT, NULL, 1) != NULL) {
      printf("Error resNodeSetContentPtr(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': new filesystem context set to NULL = ", i, __FILE__, __LINE__);

    /* not initialized with a name, handle error without SIGSEG */
    if ((prnT = resNodeNew()) == NULL) {
      printf("Error resNodeNew() ...\n");
    }
    else if (resNodeReadStatus(prnT) == TRUE) {
      printf("Error resNodeReadStatus(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeSetContentPtr(prnT, NULL, 1) != NULL) {
      printf("Error resNodeSetContentPtr(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    xmlChar *pucT = BAD_CAST DATAPREFIX "Base.Ext";
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': redundant reset of new filesystem context = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(pucT)) == NULL) {
      printf("Error resNodeDirNew(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeReset(prnT, pucT) == FALSE) {
      printf("Error resNodeReset(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeResetNameBase(prnT) == FALSE || resNodeGetNameBase(prnT) == NULL) {
      printf("Error resNodeResetNameBase()\n");
    }
    else if (resNodeResetNameBase(prnT) == FALSE || resNodeGetNameBase(prnT) == NULL) {
      printf("Error resNodeResetNameBase()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': reading of an NULL file context = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(NULL)) == NULL) {
      printf("Error resNodeDirNew(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeReadStatus(prnT) == FALSE) {
      printf("Error resNodeReadStatus(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeGetMimeType(prnT) != MIME_INODE_DIRECTORY) {
      printf("Error resNodeGetMimeType(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeReset(prnT, BAD_CAST"") == FALSE) {
      printf("Error resNodeReset(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeReadStatus(prnT) == TRUE) {
      printf("Error resNodeReadStatus(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeGetMimeType(prnT) != MIME_UNDEFINED) {
      printf("Error resNodeGetMimeType(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': new filesystem context for non-existing file = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST "file://" TESTPREFIX "dummy.txt")) == NULL) {
      printf("Error resNodeDirNew(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resPathIsDir(resNodeGetNameNormalized(prnT)) == TRUE) {
      printf("Error resPathIsDir(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeReadStatus(prnT) == TRUE) {
      printf("Error resNodeReadStatus(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsExist(prnT) == TRUE) {
      printf("Error resNodeIsExist(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsFile(prnT) == FALSE) {
      printf("Error resNodeIsFile(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsDir(prnT) == TRUE) {
      printf("Error resNodeIsDir(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsStd(prnT) == TRUE) {
      printf("Error resNodeIsStd(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsReadable(prnT) == TRUE) {
      printf("Error resNodeIsReadable(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': new filesystem context for non-existing directory = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST "file://" TESTPREFIX "dummy/")) == NULL) {
      printf("Error resNodeDirNew(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resPathIsDir(resNodeGetNameNormalized(prnT)) == FALSE) {
      printf("Error resPathIsDir(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeReadStatus(prnT) == TRUE) {
      printf("Error resNodeReadStatus(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsExist(prnT) == TRUE) {
      printf("Error resNodeIsExist(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsFile(prnT) == TRUE) {
      printf("Error resNodeIsFile(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsDir(prnT) == FALSE) {
      printf("Error resNodeIsDir(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsStd(prnT) == TRUE) {
      printf("Error resNodeIsStd(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsReadable(prnT) == TRUE) {
      printf("Error resNodeIsReadable(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': new filesystem context for existing directory = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST "file:/" TESTPREFIX "option/pie/")) == NULL) {
      printf("Error resNodeDirNew(): %s\n", resNodeGetErrorMsg(prnT));
    }
    else if (resNodeGetType(prnT) != rn_type_dir) {
      printf("Error resNodeGetType(): %s\n", resNodeGetErrorMsg(prnT));
    }
    else if (resPathIsDir(resNodeGetNameNormalized(prnT)) == FALSE) {
      printf("Error resPathIsDir(): %s\n", resNodeGetErrorMsg(prnT));
    }
    else if (resNodeReadStatus(prnT) == FALSE) {
      printf("Error resNodeReadStatus(): %s\n", resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsExist(prnT) == FALSE) {
      printf("Error resNodeIsExist(): %s\n", resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsFile(prnT) == TRUE) {
      printf("Error resNodeIsFile(): %s\n", resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsDir(prnT) == FALSE) {
      printf("Error resNodeIsDir(): %s\n", resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsStd(prnT) == TRUE) {
      printf("Error resNodeIsStd(): %s\n", resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsReadable(prnT) == FALSE) {
      printf("Error resNodeIsReadable(): %s\n", resNodeGetErrorMsg(prnT));
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': new filesystem context set and re-set to an existing file context = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeNew()) == NULL) {
      printf("Error resNodeDirNew(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeReset(prnT, BAD_CAST TESTPREFIX "xml/config.cxp") == FALSE) {
      printf("Error resNodeReset(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeReadStatus(prnT) == FALSE) {
      printf("Error resNodeReadStatus(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsExist(prnT) == FALSE) {
      printf("Error resNodeIsExist(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsFile(prnT) == FALSE) {
      printf("Error resNodeIsFile(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeReset(prnT, BAD_CAST TESTPREFIX "xsl/config.cxp") == FALSE) {
      printf("Error resNodeReset(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeReadStatus(prnT) == FALSE) {
      printf("Error resNodeReadStatus(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsExist(prnT) == FALSE) {
      printf("Error resNodeIsExist(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsFile(prnT) == FALSE) {
      printf("Error resNodeIsFile(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
  }

  
  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': new filesystem context set = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "abc.txt")) == NULL) {
      printf("Error resNodeDirNew(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeReadStatus(prnT) == TRUE) {
      printf("Error resNodeReadStatus(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsExist(prnT) == TRUE) {
      printf("Error resNodeIsExist(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsDir(prnT) == TRUE) {
      printf("Error resNodeIsFile(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsCreateable(prnT) == FALSE) {
      printf("Error resNodeIsCreateable(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeUnlink(prnT,FALSE);
    resNodeFree(prnT);
  }

  
  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': resNodeSplitStrNew() = ", i, __FILE__, __LINE__);

    if (resNodeSplitStrNew(NULL) != NULL) {
      printf("Error 1 resNodeSplitStrNew()\n");
    }
    else if ((prnT = resNodeSplitStrNew(BAD_CAST DATAPREFIX "Documents/TestContent.xml")) == NULL || resNodeGetCountDescendants(prnT) < 5) {
      printf("Error 2 resNodeSplitStrNew()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    xmlChar mucT[BUFFER_LENGTH];

    i++;
    printf("TEST %i in '%s:%i': build a list of filesystem contexts = ",i,__FILE__,__LINE__);

    xmlStrPrintf(mucT,BUFFER_LENGTH-1, TEMPPREFIX"//%c/tmp/%c",PATHLIST_SEPARATOR,PATHLIST_SEPARATOR);
    if ((prnT = resNodeStrNew(mucT)) == NULL) {
      printf("Error resNodeStrNew(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeGetLength(prnT) != 2) {
      printf("Error resNodeGetLength(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeListFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': new filesystem context set to an existing file context with quotes = ",i,__FILE__,__LINE__);
    prnT = resNodeRootNew(NULL,BAD_CAST "\"" TESTPREFIX "plain/Length_1024.txt\"");
    if (prnT != NULL && resNodeReadStatus(prnT) == TRUE) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resNodeRootNew(): %s\n",resNodeGetErrorMsg(prnT));
    }
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    xmlChar *pucT;
    xmlChar *pucTT;

    i++;
    printf("TEST %i in '%s:%i': URI of an existing file context = ",i,__FILE__,__LINE__);
    prnT = resNodeDirNew(BAD_CAST TESTPREFIX "config-test.cxp");
    if (prnT) {
      pucT = resNodeGetURI(prnT);
      pucTT = resPathCollapse(BAD_CAST"file://///" TESTPREFIX "config-test.cxp", FS_PATH_FULL);
      if (pucT != NULL && resPathIsEquivalent(pucT,pucTT)) {
        n_ok++;
        printf("OK\n");
      }
      else {
        printf("Error resNodeGetURI(): %s\n",resNodeGetErrorMsg(prnT));
      }
      xmlFree(pucTT);
      resNodeFree(prnT);
    }
    else {
      printf("Error resNodeNew()\n");
    }
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    xmlChar *pucT;

    i++;
    printf("TEST %i in '%s:%i': URI of an existing URL context = ",i,__FILE__,__LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST"https://www.test.com:8181/path/file.txt?a=b&c=123")) == NULL) {
      printf("Error resNodeDirNew(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsURL(prnT) == FALSE) {
      printf("Error resNodeIsURL(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if ((pucT = resNodeGetURI(prnT)) == NULL) {
      printf("Error resNodeGetURI(): %s\n", resNodeGetErrorMsg(prnT));
    }
    else if (resPathIsEquivalent(pucT,BAD_CAST"https://www.test.com:8181/path/file.txt?a=b&c=123") == FALSE) {
      printf("Error resPathIsEquivalent(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    xmlChar *pucT;

    i++;
    printf("TEST %i in '%s:%i': URI of an existing URL context into an archive = ",i,__FILE__,__LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST HTTPPREFIX "test-zip-7.zip/path/test.txt")) == NULL) {
      printf("Error resNodeDirNew(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeGetChild(prnT) == NULL) {
      printf("Error resNodeDirNew(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if ((pucT = resNodeGetURI(prnT)) == NULL) {
      printf("Error resNodeGetURI(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resPathIsEquivalent(pucT,BAD_CAST HTTPPREFIX "test-zip-7.zip") == FALSE) {
      printf("Error resPathIsEquivalent(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resPathIsEquivalent(resNodeGetURI(resNodeGetChild(prnT)),BAD_CAST HTTPPREFIX "test-zip-7.zip/path/") == FALSE) {
      printf("Error resPathIsEquivalent(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    xmlChar *pucTT;
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': set basename and basedir of an context = ",i,__FILE__,__LINE__);
    prnT = resNodeDirNew(BAD_CAST TESTPREFIX "xml/config.cxp");
    if (prnT) {
      pucTT = resPathCollapse(BAD_CAST TESTPREFIX "xml",FS_PATH_FULL);
      if ( xmlStrcasecmp(resNodeGetNameBase(prnT),BAD_CAST"config.cxp")==0
	   && resPathIsEquivalent(resNodeGetNameBaseDir(prnT), pucTT)) {
	n_ok++;
	printf("OK\n");
      }
      else {
        printf("Error resNodeDirNew()\n");
      }
      xmlFree(pucTT);
    }
    else {
      printf("ERROR\n");
    }
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    resNodePtr prnDir = NULL;
    resNodePtr prnFile = NULL;
    xmlChar *pucTest;
    xmlChar *pucT = NULL;

    i++;
    printf("TEST %i in '%s:%i': new filesystem context  = ", i, __FILE__, __LINE__);

    pucTest = resPathNormalize(BAD_CAST TESTPREFIX "plain/test-plain-3.xml");

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX)) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if ((prnDir = resNodeAddChildNew(prnT, BAD_CAST"plain/")) == NULL) {
      printf("Error resNodeAddChildNew()\n");
    }
    else if (resNodeGetType(prnDir) != rn_type_dir) {
      printf("Error resNodeGetType()\n");
    }
    else if ((prnFile = resNodeAddChildNew(prnDir, BAD_CAST"test-plain-3.xml")) == NULL) {
      printf("Error resNodeAddChildNew()\n");
    }
    else if (resNodeGetType(prnFile) != rn_type_file) {
      printf("Error resNodeGetType()\n");
    }
    else if ((pucT = resNodeGetNameNormalized(prnFile)) == NULL) {
      printf("Error resNodeGetNameNormalized('%s') ...\n", pucT);
    }
    else if (resPathIsEquivalent(pucTest, pucT) == FALSE) {
      printf("Error resPathIsEquivalent(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    xmlFree(pucTest);
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    resNodePtr prnChild = NULL;
    xmlChar *pucT = xmlStrdup(BAD_CAST TESTPREFIX "option/archive/test-zip-7.zip/sub/plain.txt");
    xmlChar *pucTT = resPathNormalize(BAD_CAST TESTPREFIX "option/archive/test-zip-7.zip");

    i++;
    printf("TEST %i in '%s:%i': set internal path = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST pucT)) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resPathIsEquivalent(resNodeGetNameNormalized(prnT), pucTT) == FALSE) {
      printf("Error resPathIsEquivalent()\n");
    }
    else if (resNodeGetType(prnT) != rn_type_archive) {
      printf("Error resNodeGetType()\n");
    }
    else if (resNodeGetMimeType(prnT) != MIME_APPLICATION_ZIP) {
      printf("Error resNodeGetMimeType()\n");
    }
    else if ((prnChild = resNodeGetChild(prnT)) == NULL) {
      printf("Error resNodeGetChild()\n");
    }
    else if (resNodeGetType(prnChild) != rn_type_dir_in_archive) {
      printf("Error resNodeGetType()\n");
    }
    else if (resNodeGetMimeType(prnChild) != MIME_INODE_DIRECTORY) {
      printf("Error resNodeGetMimeType()\n");
    }
    else if ((prnChild = resNodeGetChild(prnChild)) == NULL) {
      printf("Error resNodeGetChild()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
    xmlFree(pucTT);
    xmlFree(pucT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': resNodeAddSibling() = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST"test/dummy.txt")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeAddSibling(prnT, resNodeDirNew(BAD_CAST"a.png")) == NULL) {
      printf("Error resNodeAddSibling()\n");
    }
    else if (resNodeAddSibling(prnT, resNodeDirNew(BAD_CAST"b.png")) == NULL) {
      printf("Error resNodeAddSibling()\n");
    }
    else if (resNodeAddSibling(prnT, resNodeDirNew(BAD_CAST"c.png")) == NULL) {
      printf("Error resNodeAddSibling()\n");
    }
    else if (resNodeGetLength(prnT) != 4) {
      printf("Error resNodeGetLength()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeListFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    int j;
    
    i++;
    printf("TEST %i in '%s:%i': resNodeAddChildNew() = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST"test/")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeAddChildNew(prnT, BAD_CAST"a.png") == FALSE) {
      printf("Error resNodeAddChildNew()\n");
    }
    else if (resNodeAddChildNew(prnT, BAD_CAST"b.png") == FALSE) {
      printf("Error resNodeAddChildNew()\n");
    }
    else if (resNodeAddChildNew(prnT, BAD_CAST"c.png") == FALSE) {
      printf("Error resNodeAddChildNew()\n");
    }
    else if (resNodeGetParent(resNodeGetChild(prnT)) != prnT) {
      printf("Error resNodeGetParent()\n");
    }
    else if (resNodeGetParent(resNodeGetNext(resNodeGetChild(prnT))) != prnT) {
      printf("Error resNodeGetParent()\n");
    }
    else if ((j = resNodeGetChildCount(prnT, rn_type_file)) != 3) {
      printf("Error resNodeGetChildCount() = %i\n",j);
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': resNodeInMemoryNew() = ",i,__FILE__,__LINE__);

    if ((prnT = resNodeInMemoryNew())) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error\n");
    }
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': resNodeAliasNew() = ",i,__FILE__,__LINE__);

    if ((prnT = resNodeAliasNew(BAD_CAST":TEST:")) == NULL) {
      printf("Error resNodeAliasNew()\n");
    }
    else if (resNodeSetContentPtr(prnT,(void *)xmlStrdup(BAD_CAST"Content"),8) == NULL) {
      printf("Error resNodeSetContentPtr()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': concat path of an context = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeConcatNew(BAD_CAST"/tmp//", BAD_CAST"./test-res_node/xml/config.cxp")) == NULL) {
      printf("Error resNodeConcatNew()\n");
    }
    else if (resPathIsEquivalent(resNodeGetNameNormalized(prnT), BAD_CAST "\\tmp\\test-res_node\\xml\\config.cxp") == FALSE) {
      printf("Error resPathIsEquivalent()\n");
    }
    else if (resPathIsEquivalent(resNodeGetNameBase(prnT), BAD_CAST"config.cxp") == FALSE) {
      printf("Error resPathIsEquivalent()\n");
    }
    else if (resPathIsEquivalent(resNodeGetNameBaseDir(prnT), BAD_CAST "\\tmp\\test-res_node\\xml") == FALSE) {
      printf("Error resPathIsEquivalent()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': reset existing filesystem context = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "/thread/config.cxp")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeReset(prnT, NULL) == FALSE) {
      printf("Error resNodeReset()\n");
    }
    else if (resNodeReadStatus(prnT) == TRUE) {
      printf("Error resNodeReset()\n");
    }
    else if (resNodeIsDir(prnT) == TRUE) {
      printf("Error resNodeReset()\n");
    }
    else if (resNodeIsFile(prnT) == TRUE) {
      printf("Error resNodeReset()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    resNodePtr prnTT = NULL;
    resNodePtr prnTTT = NULL;
    resNodePtr prnTTTT = NULL;
    xmlChar *pucT = xmlStrdup(BAD_CAST"abcdef");
    int j;
    
    i++;
    printf("TEST %i in '%s:%i': resNodeDup() = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX)) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeAddChildNew(prnT, BAD_CAST"a.txt") == FALSE) {
      printf("Error resNodeAddChildNew()\n");
    }
    else if (resNodeSetContentPtr(prnT, pucT, xmlStrlen(pucT) + 1) == FALSE) {
      printf("Error resNodeSetContentPtr()\n");
    }
    else if (resNodeAddChildNew(prnT, BAD_CAST"b.png") == FALSE) {
      printf("Error resNodeAddChildNew()\n");
    }
    else if (resNodeAddChildNew(prnT, BAD_CAST"c.png") == FALSE) {
      printf("Error resNodeAddChildNew()\n");
    }
    else if ((prnTT = resNodeDirNew(BAD_CAST TESTPREFIX "empty/")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeAddSibling(prnT,prnTT) == FALSE) {
      printf("Error resNodeAddSibling()\n");
    }
    else if ((prnTTT = resNodeDup(prnT, (RN_DUP_CONTENT))) == NULL) {
      printf("Error resNodeDup()\n");
    }
    else if (resNodeGetContentPtr(prnTTT) == NULL || resNodeGetContentPtr(prnTTT) == pucT || xmlStrlen(BAD_CAST resNodeGetContentPtr(prnTTT)) != 6) {
      printf("Error resNodeGetContentPtr()\n");
    }
    else if ((j = resNodeGetChildCount(prnTTT, rn_type_file)) != 0) {
      printf("Error resNodeGetChildCount() = %i\n",j);
    }
    else if (resNodeGetNext(prnTTT) != NULL) {
      printf("Error resNodeGetNext()\n");
    }
    else if ((prnTTTT = resNodeDup(prnT, (RN_DUP_CONTENT | RN_DUP_CHILDS | RN_DUP_NEXT))) == NULL) {
      printf("Error resNodeDup()\n");
    }
    else if (resNodeGetContentPtr(prnTTTT) == NULL || resNodeGetContentPtr(prnTTTT) == pucT || xmlStrlen(BAD_CAST resNodeGetContentPtr(prnTTTT)) != 6) {
      printf("Error resNodeGetContentPtr()\n");
    }
    else if ((j = resNodeGetChildCount(prnTTTT, rn_type_file)) != 3) {
      printf("Error resNodeGetChildCount() = %i\n",j);
    }
    else if (resNodeGetNext(prnTTTT) == NULL || resNodeGetNext(prnTTTT) == prnT) {
      printf("Error resNodeGetNext()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeListFree(prnT);
    resNodeListFree(prnTTT);
    resNodeListFree(prnTTTT);
  }


  if (RUNTEST) {
    xmlChar *pucTT;
    resNodePtr prnContext = NULL;
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': construct a new filesystem context with string and context = ",i,__FILE__,__LINE__);

    prnContext = resNodeDirNew(BAD_CAST TESTPREFIX);
    prnT = resNodeFromNodeNew(prnContext,BAD_CAST"thread/config.cxp");
    pucTT = resPathCollapse(BAD_CAST TESTPREFIX "thread\\config.cxp",FS_PATH_FULL);
    if (prnT != NULL && resPathIsEquivalent(resNodeGetNameNormalized(prnT),pucTT)) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resNodeRead()\n");
    }
    xmlFree(pucTT);
    resNodeFree(prnT);
    resNodeFree(prnContext);
  }
  else {
    printf("Error resNodeNew()\n");
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': set recursion of existing filesystem context = ",i,__FILE__,__LINE__);

    prnT = resNodeDirNew(BAD_CAST TESTPREFIX "/");
    if (resNodeReadStatus(prnT)==TRUE
	&& resNodeIsDir(prnT)==TRUE
	&& resNodeIsRecursive(prnT)==FALSE
	&& resNodeSetRecursion(prnT,TRUE)==TRUE
	&& resNodeIsRecursive(prnT)==TRUE
	&& resNodeGetNameBase(prnT) == NULL) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resNodeDirNew()\n");
    }
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    resNodePtr prnTT = NULL;
    xmlChar *pucT = BAD_CAST TEMPPREFIX "empty.png";

    i++;
    printf("TEST %i in '%s:%i': resNodeCommentNew() = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(pucT)) != NULL
	&& (prnTT = resNodeCommentNew(prnT)) != NULL
	&& xmlStrlen(resNodeGetNameNormalized(prnTT)) - xmlStrlen(resNodeGetNameNormalized(prnT)) == 4
      ) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error\n");
    }
    resNodeFree(prnTT);
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': _resNodeSetNameAncestor() = ",i,__FILE__,__LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST"AAA")) != NULL
	&& resNodeReset(prnT,NULL) == TRUE
	&& _resNodeSetNameAncestor(prnT,NULL) == FALSE
	&& resNodeReset(prnT,NULL) == TRUE
	&& _resNodeSetNameAncestor(NULL,BAD_CAST TEMPPREFIX) == FALSE
	&& resNodeReset(prnT,NULL) == TRUE
	&& _resNodeSetNameAncestor(prnT,BAD_CAST TEMPPREFIX) == TRUE) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error\n");
    }
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    xmlChar* pucT;

    i++;
    printf("TEST %i in '%s:%i': resNodeSetToParent() = ",i,__FILE__,__LINE__);

    if ((pucT = resPathCollapse(BAD_CAST TEMPPREFIX, FS_PATH_FULL)) == NULL) {
      printf("Error\n");
    }
    else if ((prnT = resNodeDirNew(BAD_CAST TEMPPREFIX "/AAA/BBB")) == NULL) {
      printf("Error\n");
    }
    else if (resNodeSetToParent(prnT) == FALSE || resNodeSetToParent(prnT) == FALSE) {
      printf("Error\n");
    }
    else if (resPathIsEquivalent(resNodeGetNameNormalized(prnT), pucT) == FALSE) {
      printf("Error\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
    xmlFree(pucT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': resNodeIsURL() = ",i,__FILE__,__LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TEMPPREFIX)) != NULL
	&& resNodeIsURL(prnT) == FALSE
	&& resNodeReset(prnT, BAD_CAST CXP_VER_URL) == TRUE
	&& resNodeIsURL(prnT) == TRUE
	) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error\n");
    }
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    xmlChar *pucT = NULL;

    i++;
    printf("TEST %i in '%s:%i': resNodeGetExtension() = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TEMPPREFIX "empty.png")) != NULL
	&& (pucT = resNodeGetExtension(prnT)) != NULL
	&& xmlStrEqual(pucT,BAD_CAST "png")
	&& resNodeReset(prnT,BAD_CAST TEMPPREFIX "empty.jpg") == TRUE
	&& (pucT = resNodeGetExtension(prnT)) != NULL
	&& xmlStrEqual(pucT,BAD_CAST "jpg")
	&& resPathIsEquivalent(resNodeGetNameBase(prnT), BAD_CAST "empty.jpg")
      ) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error\n");
    }
    resNodeFree(prnT);
  }


  if (SKIPTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': resNodeGetType() = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TEMPPREFIX "empty.png")) != NULL
	&& resNodeGetType(prnT) == rn_type_file
	&& resNodeReset(prnT,BAD_CAST TEMPPREFIX "empty/") == TRUE
	&& resNodeGetType(prnT) == rn_type_dir
	&& resNodeReset(prnT, BAD_CAST CXP_VER_URL) == TRUE
	&& resNodeGetType(prnT) == rn_type_url_http
	&& resNodeReset(prnT,BAD_CAST "-") == TRUE
	&& resNodeGetType(prnT) == rn_type_stdin
      ) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error\n");
    }
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    resNodePtr prnTT = NULL;
    xmlChar *pucT;

    i++;
    printf("TEST %i in '%s:%i': resNodeGetNameRelative() = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TEMPPREFIX "empty/dummy.txt")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if ((prnTT = resNodeDirNew(BAD_CAST TEMPPREFIX)) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if ((pucT = resNodeGetNameRelative(prnTT, prnT)) == NULL) {
      printf("Error resNodeGetNameRelative()\n");
    }
    else if (resPathIsEquivalent(pucT, BAD_CAST "empty/dummy.txt") == FALSE) {
      printf("Error resNodeGetNameRelative()\n");
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
    printf("TEST %i in '%s:%i': rebuild existing filesystem context = ",i,__FILE__,__LINE__);
    prnT = resNodeDirNew(BAD_CAST TESTPREFIX "thread/config.cxp");
    if (prnT) {
      if (resNodeReadStatus(prnT)==TRUE && resNodeIsDir(prnT)==FALSE && resNodeIsFile(prnT)==TRUE) {
        xmlChar *pucA;
        xmlChar *pucB;
        xmlChar *pucC;

        pucA = resNodeGetNameBaseDir(prnT);
        pucB = resNodeGetNameBase(prnT);

        pucC = resPathConcatNormalized(pucA,pucB);

        if (resNodeReset(prnT,pucC)) {
          if (resNodeReadStatus(prnT)==TRUE && resNodeIsDir(prnT)==FALSE && resNodeIsFile(prnT)==TRUE) {
            n_ok++;
            printf("OK\n");
          }
          else {
            printf("Error resNodeRead()\n");
          }
        }
        else {
          printf("Error resNodeReset()\n");
        }
        xmlFree(pucC);
      }
      resNodeFree(prnT);
    }
    else {
      printf("Error resNodeNew()\n");
    }
  }


  if (RUNTEST) {
    xmlChar *pucTT = NULL;
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': re-use of an used context = ",i,__FILE__,__LINE__);
    prnT = resNodeDirNew(BAD_CAST TESTPREFIX "test.mak");
    if (prnT) {
      if (xmlStrcasecmp(resNodeGetNameBase(prnT),BAD_CAST"test.mak")==0) {
	if ((pucTT = resPathCollapse(BAD_CAST TESTPREFIX,FS_PATH_FULL))
	    && resPathIsEquivalent(resNodeGetNameBaseDir(prnT), pucTT)) {
	  xmlFree(pucTT);
	  if ((pucTT = resPathCollapse(BAD_CAST TESTPREFIX "test.mak",FS_PATH_FULL))
	      && resPathIsEquivalent(resNodeGetNameNormalized(prnT), pucTT)
	      && resNodeSetNameBaseNative(prnT,"t.txt")==TRUE
	      && xmlStrcasecmp(resNodeGetNameBase(prnT),BAD_CAST"t.txt")==0) {
	    xmlFree(pucTT);
	    if ((pucTT = resPathCollapse(BAD_CAST TESTPREFIX,FS_PATH_FULL))
		&& resPathIsEquivalent(resNodeGetNameBaseDir(prnT), pucTT)) {
	      xmlFree(pucTT);
	      if ((pucTT = resPathCollapse(BAD_CAST TESTPREFIX "t.txt",FS_PATH_FULL))
		  && resPathIsEquivalent(resNodeGetNameNormalized(prnT), pucTT)) {
		xmlFree(pucTT);
		n_ok++;
		printf("OK\n");
	      }
	    }
	  }
	}
      }
      else {
	printf("Error resNodeDirNew()\n");
      }
    }
    else {
      printf("ERROR\n");
    }
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': parsing of a non-existing filesystem directory = ",i,__FILE__,__LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "dummy//.")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeDirAppendEntries(prnT,NULL) == TRUE) {
      printf("Error resNodeDirAppendEntries()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    size_t j;
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': parsing of an existing filesystem directory = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "each")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeReadStatus(prnT) == FALSE) {
      printf("Error resNodeReadStatus()\n");
    }
    else if (resNodeDirAppendEntries(prnT,NULL) == FALSE) {
      printf("Error resNodeDirAppendEntries()\n");
    }
    else if ((j = resNodeGetChildCount(prnT, rn_type_file)) != 4) {
      /* without resNodeReadStatus() for all childs, they are files by default */
      printf("Error resNodeDirAppendEntries() = %i\n", j);
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
  }
  
#ifdef HAVE_PCRE2
  if (RUNTEST) {
    size_t j;
    resNodePtr prnT = NULL;
    size_t erroroffset;
    int errornumber;
    int erroffset;
    int opt_match_pcre = PCRE2_UTF | PCRE2_CASELESS;
    pcre2_code *re_match = NULL;

    i++;
    printf("TEST %i in '%s:%i': parsing of an existing filesystem directory with pattern matching = ", i, __FILE__, __LINE__);

    re_match = pcre2_compile(
      (PCRE2_SPTR8)"C+", /* the pattern */
      PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
      opt_match_pcre,         /* default options */
      &errornumber,          /* for error number */
      &erroroffset,          /* for error offset */
      NULL);                 /* use default compile context */

    if (re_match == NULL) {
      printf("Error pcre2_compile()\n");
    }
    else if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX)) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeReadStatus(prnT) == FALSE) {
      printf("Error resNodeReadStatus()\n");
    }
    else if (resNodeDirAppendEntries(prnT, re_match) == FALSE) {
      printf("Error resNodeDirAppendEntries()\n");
    }
    else if ((j = resNodeGetChildCount(prnT, rn_type_file)) < 10 && j > 11) {
      printf("Error resNodeDirAppendEntries() = %i\n", j);
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    pcre2_code_free(re_match);
    resNodeFree(prnT);
  }


  if (SKIPTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': link resolving twice = ",i,__FILE__,__LINE__);
    prnT = resNodeDirNew(BAD_CAST"./test.lnk");
    if (prnT) {
      if (resNodeReadStatus(prnT)==TRUE && resNodeIsLink(prnT)) {
        resNodePtr prnTT = NULL;

        prnTT = resNodeResolveLinkChildNew(prnT);
        if (prnTT) {
          if (resNodeGetParent(prnTT) == prnT) {
            n_ok++;
            printf("OK\n");
          }
          else {
            printf("Error resNodeAddChild()\n");
          }
	  resNodeFree(prnTT);
       }
        else {
          printf("ERROR\n");
        }
      }
      else {
        printf("Error resNodeResolveLink()\n");
      }
    }
    else {
      printf("ERROR\n");
    }
    resNodeFree(prnT);
  }
#endif


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    xmlChar *pucT = NULL;
    xmlNodePtr pndT = NULL;

    i++;
    printf("TEST %i in '%s:%i': resNodeTo*() XML file = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "xml/dummy-a.xml")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeReadStatus(prnT) == FALSE) {
      printf("Error resNodeReadStatus()\n");
    }
    else if (resNodeUpdate(prnT, RN_INFO_MAX, NULL, NULL) == FALSE) {
      printf("Error resNodeUpdate()\n");
    }
    else if ((pndT = resNodeToDOM(prnT, RN_INFO_MAX)) == NULL || domNumberOfChild(pndT, NULL) != 1) {
      printf("Error resNodeToDOM()\n");
    }
    else if ((pucT = resNodeToPlain(prnT, RN_INFO_MAX)) == NULL) {
      printf("Error resNodeToPlain() = %i\n",xmlStrlen(pucT));
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST "resNodeToDOM(): ", pndT);
    //puts((const char *)pucT);
    xmlFree(pucT);
    xmlFreeNode(pndT);
    resNodeFree(prnT);
  }


#if 0
  if (FALSE) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': check filesystem context CRC = ",i,__FILE__,__LINE__);
    prnT = resNodeRootNew(BAD_CAST"history/acal/acal.c");
    if (resNodeGetFileCrc(prnT)==1028943022) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error _resNodeGetFileCrc()\n");
    }
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    xmlChar *pucPath = NULL;

    i++;
    printf("TEST %i in '%s:%i': create an local index file = ",i,__FILE__,__LINE__);

    if ((prnT = resNodeIndexNew(BAD_CAST DATAPREFIX "//")) == NULL) {
      printf("Error resNodeIndexNew()\n");
    }
    else if (resNodeIndexSave(prnT) == FALSE) {
      printf("Error resNodeIsMemory()\n");
    }
    else if ((pucPath = resNodeIndexFind(prnT,BAD_CAST"TestContent.docx")) == NULL) {
      printf("Error resNodeIsMemory()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    xmlFree(pucPath);
    resNodeFree(prnT);
  }
#endif


  if (RUNTEST) {
    xmlNodePtr pndTest = NULL;
    resNodePtr prnT = NULL;
    xmlChar *pucT;
    
    i++;
    printf("TEST %i in '%s:%i': resNodeToDOM() = ",i,__FILE__,__LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "plain/test-plain-3.xml")) == NULL) {
      printf("Error domGetFirstChild()\n");
    }
    else if (resNodeReadStatus(prnT) == FALSE) {
      printf("Error resNodeReadStatus()\n");
    }
    else if (resNodeUpdate(prnT, RN_INFO_MAX, NULL, NULL) == FALSE) {
      printf("Error resNodeUpdate()\n");
    }
    else if ((pndTest = resNodeToDOM(prnT,RN_INFO_MAX)) == NULL) {
      printf("Error resNodeToDOM()\n");
    }
    else if (IS_NODE_FILE(pndTest) == FALSE || IS_NODE_PIE(pndTest->children) == FALSE) {
      printf("Error PIE\n");
    }
    else if (domGetAttributePtr(pndTest,BAD_CAST"object") != NULL) {
      printf("Error object\n");
    }
    else if (xmlStrEqual(domGetAttributePtr(pndTest,BAD_CAST"type"), BAD_CAST resNodeGetMimeTypeStr(prnT)) == FALSE) {
      printf("Error domGetFirstChild()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST "resNodeToDOM(): ", pndTest);
    xmlFreeNodeList(pndTest);
    resNodeFree(prnT);
  }

  
  if (SKIPTEST) {
    xmlNodePtr pndTest = NULL;
    resNodePtr prnT = NULL;
    xmlChar *pucT;
    
    i++;
    printf("TEST %i in '%s:%i': resNodeReadStatus() = ",i,__FILE__,__LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST DATAPREFIX "CAD/TestContent.drw.11")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeReadStatus(prnT) == FALSE) {
      printf("Error resNodeReadStatus()\n");
    }
    else if (resNodeUpdate(prnT, RN_INFO_MAX, NULL, NULL) == FALSE) {
      printf("Error resNodeUpdate()\n");
    }
    else if ((pndTest = resNodeToDOM(prnT,RN_INFO_MAX)) == NULL) {
      printf("Error resNodeToDOM()\n");
    }
    else if (IS_NODE_FILE(pndTest) == FALSE || pndTest->children != NULL) {
      printf("Error PIE\n");
    }
    else if (xmlStrEqual(domGetAttributePtr(pndTest,BAD_CAST"type"), BAD_CAST resNodeGetMimeTypeStr(prnT)) == FALSE) {
      printf("Error type\n");
    }
    else if (xmlStrEqual(domGetAttributePtr(pndTest,BAD_CAST"object"), BAD_CAST "TESTCONTENT.DRW") == FALSE) {
      printf("Error object\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST "resNodeToDOM(): ", pndTest);
    xmlFreeNodeList(pndTest);
    resNodeFree(prnT);
  }

  
  printf("\nResult in '%s': %i/%i OK\n\n",__FILE__,n_ok,i);

  return (i - n_ok);
}
