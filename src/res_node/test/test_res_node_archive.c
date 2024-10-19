/*
  cxproc - Configurable Xml PROCessor

  Copyright (C) 2006..2024 by Alexander Tenbusch

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
arcTestResNodeRead(void)
{
  int i;
  int n_ok;

  n_ok=0;
  i=0;

  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': test opening of existing ZIP archive = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "option/archive/test-zip-7.zip")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeGetHandleIO(prnT) != NULL) {
      printf("Error resNodeGetHandleIO()\n");
    }
    else if (resNodeOpen(prnT, "ra") == FALSE) {
      printf("Error 1 resNodeOpen() ...\n");
    }
    else if (resNodeGetHandleIO(prnT) == NULL) {
      printf("Error resNodeGetHandleIO()\n");
    }
    else if (resNodeClose(prnT) == FALSE) {
      printf("Error resNodeClose()\n");
    }
    else if (resNodeGetHandleIO(prnT) != NULL) {
      printf("Error resNodeGetHandleIO()\n");
    }
    else {
      printf("OK\n");
      n_ok++;
    }
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    xmlNodePtr pndT = NULL;

    i++;
    printf("TEST %i in '%s:%i': uncompress archive context = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "option\\archive\\test-arc-1.tar")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (arcAppendEntries(prnT, NULL, TRUE) == FALSE) {
      printf("Error 1 arcAppendEntries() ...\n");
    }
    else if (resNodeGetChildCount(prnT, rn_type_file_in_archive) != 4 || resNodeGetChildCount(prnT, rn_type_dir_in_archive) != 0) {
      printf("Error 2 arcAppendEntries() ...\n");
    }
    else if (resNodeGetContentPtr(resNodeGetChild(prnT)) == NULL) {
      printf("Error 3 arcAppendEntries() ...\n");
    }
    else if ((pndT = resNodeToDOM(prnT, RN_INFO_MAX)) == NULL) {
      printf("Error 1 resNodeToDOM() ...\n");
    }
    else if (IS_NODE_FILE(pndT) == FALSE || pndT->children == NULL
	     || domNumberOfChild(pndT->children, NAME_FILE) != 4 || domNumberOfChild(pndT->children, NAME_DIR) != 0) {
      printf("Error 2 resNodeToDOM() ...\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST "arcAppendEntries() result", pndT);
    xmlFreeNode(pndT);
    resNodeFree(prnT);
  }


  if (SKIPTEST) {
    resNodePtr prnT = NULL;
    xmlNodePtr pndT = NULL;

    i++;
    printf("TEST %i in '%s:%i': uncompress archive context = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "option\\archive\\test-arc-1.tar.gz")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (arcAppendEntries(prnT, NULL, TRUE) == FALSE) {
      printf("Error 1 arcAppendEntries() ...\n");
    }
    else if (resNodeGetChildCount(prnT, rn_type_file_in_archive) != 1 || resNodeGetChildCount(prnT, rn_type_dir_in_archive) != 0) {
      printf("Error 2 arcAppendEntries() ...\n");
    }
    else if (resNodeGetContentPtr(resNodeGetChild(prnT)) == NULL) {
      printf("Error 3 arcAppendEntries() ...\n");
    }
    else if ((pndT = resNodeToDOM(prnT, RN_INFO_MAX)) == NULL) {
      printf("Error 1 resNodeToDOM() ...\n");
    }
    else if (IS_NODE_FILE(pndT) == FALSE || pndT->children == NULL
	     || domNumberOfChild(pndT->children, NAME_FILE) != 1 || domNumberOfChild(pndT->children, NAME_DIR) != 0) {
      printf("Error 2 resNodeToDOM() ...\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST "arcAppendEntries() result", pndT);
    xmlFreeNode(pndT);
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    size_t j;
    resNodePtr prnT = NULL;
    resNodePtr prnFound = NULL;

    i++;
    printf("TEST %i in '%s:%i': parse archive context = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "option/archive/test-zip-7.zip")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeListParse(prnT, 999, NULL) == FALSE) {
      printf("Error 1 x resNodeListParse() ...\n");
    }
    else if (resNodeListParse(prnT, 999, NULL) == FALSE) {
      printf("Error 2 x resNodeListParse() ...\n");
    }
    else if ((prnFound = resNodeListFindPath(prnT, BAD_CAST"sub/", (RN_FIND_DIR | RN_FIND_IN_SUBDIR | RN_FIND_IN_ARCHIVE))) == NULL) {
      printf("Error resNodeListFindPath() ...\n");
    }
    else if ((prnFound = resNodeListFindPath(prnT, BAD_CAST "sub/plain.txt", (RN_FIND_FILE | RN_FIND_IN_SUBDIR | RN_FIND_IN_ARCHIVE))) == NULL) {
      printf("Error resNodeListFindPath() ...\n");
    }
    else if ((j = resNodeGetChildCount(prnT, rn_type_dir_in_archive)) != 1) {
      printf("Error resNodeListParse() = %i\n", j);
    }
    else if ((j = resNodeGetChildCount(resNodeGetChild(prnT), rn_type_file_in_archive)) != 4) {
      printf("Error resNodeListParse() = %i\n", j);
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    resNodePtr prnChild = NULL;

    i++;
    printf("TEST %i in '%s:%i': extract binary content from ZIP archive = ", i, __FILE__, __LINE__);
    /* https://github.com/libarchive/libarchive/wiki/WishList#Seek_in_archives */
    
    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "option/archive/test-zip-7.zip/sub/weiter.png")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeIsArchive(prnT) == FALSE) {
      printf("Error resNodeIsArchive()\n");
    }
    else if (arcAppendEntries(prnT,NULL,TRUE) == FALSE) {
      printf("Error 1 arcAppendEntries()\n");
    }
    else if (arcAppendEntries(prnT,NULL,TRUE) == FALSE) {
      printf("Error 1 arcAppendEntries()\n");
    }
    else if ((prnChild = resNodeGetChild(prnT)) == NULL || (prnChild = resNodeGetChild(prnChild)) == NULL) {
      printf("Error resNodeGetSize()\n");
    }
    else if (resNodeIsOpen(prnT) == TRUE) {
      printf("Error 2 resNodeIsOpen()\n");
    }
    else if (resNodeClose(prnT) == TRUE) {
      printf("Error resNodeClose()\n");
    }
    else if (resNodeIsOpen(prnT) == TRUE) {
      printf("Error 3 resNodeIsOpen()\n");
    }
    else if (resNodeUpdate(prnT, RN_INFO_MAX, NULL, NULL) == FALSE) {
      printf("Error 1 resNodeUpdate()\n");
    }
    else if (resNodeUpdate(prnT, RN_INFO_MAX, NULL, NULL) == FALSE) {
      printf("Error 2 resNodeUpdate()\n");
    }
    else if (resNodeGetContentPtr(prnT) != NULL || resNodeGetSize(prnT) != 14576) {
      printf("Error 3 resNodeUpdate()\n");
    }
    else if (resNodeGetContentPtr(resNodeGetChild(prnT)) != NULL || resNodeGetSize(resNodeGetChild(prnT)) != 0) {
      printf("Error 4 resNodeUpdate()\n");
    }
    else if (resNodeGetContentPtr(resNodeGetChild(resNodeGetChild(prnT))) == NULL || resNodeGetSize(resNodeGetChild(resNodeGetChild(prnT))) != 345) {
      printf("Error 5 resNodeUpdate()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
  }

  if (RUNTEST) {
    resNodePtr prnT = NULL;
    resNodePtr prnFile = NULL;
    xmlChar *pucT = NULL;
    xmlChar *pucTT = NULL;

    i++;
    printf("TEST %i in '%s:%i': extract plain text content from ZIP archive = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "option/archive/test-zip-7.zip/sub/plain.txt")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeIsArchive(prnT) == FALSE) {
      printf("Error resNodeIsArchive()\n");
    }
    else if (resNodeUpdate(prnT, RN_INFO_MAX, NULL, NULL) == FALSE) {
      printf("Error 1 resNodeUpdate()\n");
    }
    else if (resNodeGetContentPtr(prnT) != NULL || resNodeGetSize(prnT) != 14576) {
      printf("Error 2 resNodeUpdate()\n");
    }
    else if (resNodeGetContentPtr(resNodeGetChild(prnT)) != NULL || resNodeGetSize(resNodeGetChild(prnT)) != 0) {
      printf("Error 4 resNodeUpdate()\n");
    }
    else if ((prnFile = resNodeGetChild(resNodeGetChild(prnT))) == NULL
      || resNodeGetContentPtr(prnFile) == NULL || resNodeGetSize(prnFile) != 49) {
      printf("Error 5 resNodeUpdate()\n");
    }
    else if ((pucT = plainGetContextTextEat(prnFile, 10)) == NULL || xmlStrlen(pucT) != 49) {
      printf("Error resNodeGetContent()\n");
    }
    else if ((pucTT = resNodeGetContentBase64Eat(prnFile, 10)) == NULL || xmlStrlen(pucTT) != 68) {
      printf("Error resNodeGetContentBase64Eat()\n");
    }
    else {
      //puts((const char *)pucT);
      n_ok++;
      printf("OK\n");
    }
    xmlFree(pucTT);
    xmlFree(pucT);
    resNodeFree(prnT);
  }

  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': process nonexisting entry of existing ZIP archive = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "option/archive/test-zip-7.zip/sub/nonexisting.txt")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeIsArchive(prnT) == FALSE) {
      printf("Error resNodeIsArchive()\n");
    }
    else if (resNodeUpdate(prnT, RN_INFO_MAX, NULL, NULL) == FALSE) {
      printf("Error 1 resNodeUpdate()\n");
    }
    else if (resNodeGetContentPtr(prnT) != NULL || resNodeGetSize(prnT) != 14576) {
      printf("Error 2 resNodeUpdate()\n");
    }
    else if (resNodeGetContentPtr(resNodeGetChild(prnT)) != NULL || resNodeGetSize(resNodeGetChild(prnT)) != 0) {
      printf("Error 3 resNodeUpdate()\n");
    }
    else if (resNodeGetContentPtr(resNodeGetChild(resNodeGetChild(prnT))) != NULL || resNodeGetSize(resNodeGetChild(resNodeGetChild(prnT))) != 0) {
      printf("Error 4 resNodeUpdate()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
  }


  /*!\todo test concurrent access on same archive */
  

  printf("TEST in '%s': %i/%i OK\n\n", __FILE__, n_ok, i);

  return (i - n_ok);
}
/* end of arcTestResNodeRead() */


/*!
*/
int
arcTestResNodeWrite(void)
{
  int i;
  int n_ok;

  n_ok=0;
  i=0;

  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': create and close a non-existing TAR file context = ",i,__FILE__,__LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TEMPPREFIX "created.tar")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeOpen(prnT,"wa") == FALSE) {
      printf("Error resNodeOpen()\n");
    }
    else if (resNodeIsOpen(prnT) == FALSE) {
      printf("Error resNodeIsOpen()\n");
    }
    else if (resNodeGetChild(prnT) != NULL) {
      printf("Error resNodeOpen()\n");
    }
    else if (prnT->eAccess != rn_access_archive) {
      printf("Error eAccess\n");
    }
    else if (prnT->eMode != mode_write) {
      printf("Error eAccess\n");
    }
    else if (resNodeClose(prnT) == FALSE) {
      printf("Error resNodeClose()\n");
    }
    else if (resNodeIsOpen(prnT) == TRUE) {
      printf("Error resNodeIsOpen()\n");
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
    resNodePtr prnTT = NULL;

    i++;
    printf("TEST %i in '%s:%i': create and close a non-existing TAR file context = ",i,__FILE__,__LINE__);
    
    if ((prnT = resNodeDirNew(BAD_CAST "\"" TEMPPREFIX "/created.tar\"")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if ((prnTT = resNodeRootNew(NULL,BAD_CAST "\"" TESTPREFIX "plain/Length_1024.txt\"")) == NULL) {
      //xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeRootNew(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeReadStatus(prnTT) == FALSE) {
      //xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeRootNew(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeOpen(prnT,"wa") == FALSE) {
      printf("Error resNodeOpen()\n");
    }
    else if (arcAddResNode(prnT,prnTT,NULL,TESTPREFIX) == FALSE) {
      printf("Error arcFileWrite()\n");
    }
    else if (resNodeClose(prnT) == FALSE) {
      printf("Error resNodeClose()\n");
    }
    else {
      n_ok++;
    }
    resNodeFree(prnTT);
    resNodeFree(prnT);
  }


 if (SKIPTEST) {
    resNodePtr prnArchive = NULL;

    i++;
    printf("TEST %i in '%s:%i': compress res node into archive = ", i, __FILE__, __LINE__);

    if (arcAddResNode(NULL, NULL, NULL, NULL) == TRUE) {
      printf("Error arcAddResNode()\n");
    }
    else if ((prnArchive = resNodeDirNew(BAD_CAST TEMPPREFIX "aaa.zip")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (arcAddResNode(prnArchive, NULL, NULL, NULL) == TRUE) {
      printf("Error arcAddResNode()\n");
    }
    else if (resNodeOpen(prnArchive, "wa") == FALSE) {
      printf("error of resNodeOpen()\n");
    }
    else if (arcAddResNode(prnArchive, NULL, BAD_CAST"html/sub/123.html", NULL) == FALSE) {
      printf("Error arcAddResNode()\n");
    }
    else if (arcAddResNode(prnArchive, NULL, BAD_CAST"html/sub-2/456.html", NULL) == FALSE) {
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
    else if (arcAddResNode(prnArchive, prnT, NULL, NULL) == FALSE) {
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
    else if (arcAddResNode(prnArchive, prnTT, NULL, NULL) == FALSE) {
      printf("Error arcAddResNode()\n");
    }
    else if ((prnTTT = resNodeAddChildNew(prnTT, BAD_CAST"DUMMY")) == NULL) {
      printf("Error resNodeAddChildNew()\n");
    }
    else if (resNodeSetNameAlias(prnTTT, BAD_CAST"y.html") == NULL) {
      printf("Error resNodeSetNameAlias()\n");
    }
    else if (arcAddResNode(prnArchive, prnTTT, NULL, NULL) == FALSE) {
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
    resNodePtr prnT = NULL;
    resNodePtr prnTT = NULL;

    i++;
    printf("TEST %i in '%s:%i': write TAR content to stdout = ",i,__FILE__,__LINE__);
    
    if ((prnT = resNodeDirNew(BAD_CAST "-")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if ((prnTT = resNodeRootNew(NULL,BAD_CAST "\"" TESTPREFIX "plain/Length_1024.txt\"")) == NULL) {
      //xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeRootNew(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeReadStatus(prnTT) == FALSE) {
      //xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeRootNew(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeOpen(prnT,"wa") == FALSE) {
      printf("Error resNodeOpen()\n");
    }
    else if (arcAddResNode(prnT,prnTT,NULL,TESTPREFIX) == FALSE) {
      printf("Error arcFileWrite()\n");
    }
    else if (resNodeClose(prnT) == FALSE) {
      printf("Error resNodeClose()\n");
    }
    else {
      n_ok++;
    }
    resNodeFree(prnTT);
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    size_t j;
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': clone an archive into a different location = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "option/archive/test-zip-7.zip")) == NULL) { // 
      printf("Error resNodeDirNew()\n");
    }
    else if (arcAppendEntries(prnT, NULL, TRUE) == FALSE) {
      printf("Error 1 x arcAppendEntries() ...\n");
    }
    else if ((j = resNodeGetChildCount(resNodeGetChild(prnT), rn_type_file_in_archive)) != 4) {
      printf("Error resNodeListParse() = %i\n", j);
    }
    else if (resNodeSetNameBaseDir(prnT,BAD_CAST TEMPPREFIX) == FALSE) {
      printf("Error resNodeSetNameBaseDir()\n");
    }
    else if (arcFileWrite(prnT) == FALSE) {
      printf("Error arcFileWrite()\n");
    }
    else if (resNodeReadStatus(prnT) == FALSE) {
      //xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeRootNew(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    // puts(resNodeListToPlain(prnT,1));
    resNodeFree(prnT);
  }



  printf("TEST in '%s': %i/%i OK\n\n", __FILE__, n_ok, i);

  return (i - n_ok);
}
/* end of arcTestResNodeWrite() */

