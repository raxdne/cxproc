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
  xmlChar *pucModuleTestReport = xmlStrdup(BAD_CAST"\n");
  xmlChar mucTestLabel[BUFFER_LENGTH];
  xmlChar mucTestResult[BUFFER_LENGTH];

  n_ok=0;
  i=0;

  if (RUNTEST) {

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': processing of dummy resource node = ", i, __FILE__, __LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if (resNodeReset(NULL, NULL) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeReset()\n");
    }
    else if (resNodeReset(NULL, BAD_CAST"") == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 2 resNodeReset()\n");
    }
    else if (resNodeReadStatus(NULL) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeReadStatus()\n");
    }
    else if (resNodeIsExist(NULL) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeIsExist()\n");
    }
    else if (resNodeIsFile(NULL) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeIsFile()\n");
    }
    else if (resNodeIsDir(NULL) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeIsDir()\n");
    }
    else if (resNodeIsStd(NULL) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeIsStd()\n");
    }
    else if (resNodeDup(NULL,256) != NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDup()\n");
    }
    else if (resNodeGetErrorMsg(NULL) != NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeGetErrorMsg()\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    resNodeFree(NULL);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': new filesystem context set to NULL = ", i, __FILE__, __LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    /* not initialized with a name, handle error without SIGSEG */
    if ((prnT = resNodeNew()) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeNew()\n");
    }
    else if (resNodeReadStatus(prnT) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeReadStatus(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeSetContentPtr(prnT, NULL, 1) != NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeSetContentPtr(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': new filesystem context set to NULL = ", i, __FILE__, __LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    /* not initialized with a name, handle error without SIGSEG */
    if ((prnT = resNodeNew()) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeNew()\n");
    }
    else if (resNodeReadStatus(prnT) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeReadStatus(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeSetContentPtr(prnT, NULL, 1) != NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeSetContentPtr(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    xmlChar *pucT = BAD_CAST TESTPREFIX "Base.Ext";
    resNodePtr prnT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': redundant reset of new filesystem context = ", i, __FILE__, __LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if ((prnT = resNodeDirNew(pucT)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeReset(prnT, pucT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeReset(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeResetNameBase(prnT) == FALSE || resNodeGetNameBase(prnT) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeResetNameBase()\n");
    }
    else if (resNodeResetNameBase(prnT) == FALSE || resNodeGetNameBase(prnT) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeResetNameBase()\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': reading of an NULL file context = ", i, __FILE__, __LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if ((prnT = resNodeDirNew(NULL)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeReadStatus(prnT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeReadStatus(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeGetMimeType(prnT) != MIME_INODE_DIRECTORY) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeGetMimeType(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeReset(prnT, BAD_CAST"") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeReset(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeReadStatus(prnT) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeReadStatus(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeGetMimeType(prnT) != MIME_UNDEFINED) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeGetMimeType(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': new filesystem context for non-existing file = ", i, __FILE__, __LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if ((prnT = resNodeDirNew(BAD_CAST "file://" TESTPREFIX "dummy.txt")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resPathIsDir(resNodeGetNameNormalized(prnT)) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resPathIsDir(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeReadStatus(prnT) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeReadStatus(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsExist(prnT) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeIsExist(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsFile(prnT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeIsFile(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsDir(prnT) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeIsDir(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsStd(prnT) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeIsStd(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsReadable(prnT) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeIsReadable(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': new filesystem context for non-existing directory = ", i, __FILE__, __LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if ((prnT = resNodeDirNew(BAD_CAST "file://" TESTPREFIX "dummy/")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resPathIsDir(resNodeGetNameNormalized(prnT))) { /* trailing slashes have to be removed */
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resPathIsDir(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeReadStatus(prnT) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeReadStatus(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsExist(prnT) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeIsExist(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsFile(prnT) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeIsFile(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsDir(prnT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeIsDir(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsStd(prnT) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeIsStd(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsReadable(prnT) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeIsReadable(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': new filesystem context for existing directory = ", i, __FILE__, __LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if ((prnT = resNodeDirNew(BAD_CAST "file:/" TESTPREFIX "option/pie/")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew(): %s\n", resNodeGetErrorMsg(prnT));
    }
    else if (resNodeGetType(prnT) != rn_type_dir) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeGetType(): %s\n", resNodeGetErrorMsg(prnT));
    }
    else if (resPathIsDir(resNodeGetNameNormalized(prnT))) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resPathIsDir(): %s\n", resNodeGetErrorMsg(prnT));
    }
    else if (resNodeReadStatus(prnT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeReadStatus(): %s\n", resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsExist(prnT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeIsExist(): %s\n", resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsFile(prnT) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeIsFile(): %s\n", resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsDir(prnT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeIsDir(): %s\n", resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsStd(prnT) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeIsStd(): %s\n", resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsReadable(prnT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeIsReadable(): %s\n", resNodeGetErrorMsg(prnT));
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': new filesystem context set and re-set to an existing file context = ", i, __FILE__, __LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if ((prnT = resNodeNew()) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeReset(prnT, BAD_CAST TESTPREFIX "xml/config.cxp") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeReset(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeReadStatus(prnT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeReadStatus(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsExist(prnT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeIsExist(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsFile(prnT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeIsFile(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeReset(prnT, BAD_CAST TESTPREFIX "xsl/config.cxp") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeReset(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeReadStatus(prnT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeReadStatus(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsExist(prnT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeIsExist(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsFile(prnT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeIsFile(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    resNodeFree(prnT);
  }

  
  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': new filesystem context set = ", i, __FILE__, __LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "abc.txt")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeReadStatus(prnT) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeReadStatus(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsExist(prnT) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeIsExist(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsDir(prnT) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeIsFile(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsCreateable(prnT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeIsCreateable(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    resNodeUnlink(prnT,FALSE);
    resNodeFree(prnT);
  }

  
  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': resNodeSplitStrNew() = ", i, __FILE__, __LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if (resNodeSplitStrNew(NULL) != NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeSplitStrNew()\n");
    }
    else if ((prnT = resNodeSplitStrNew(BAD_CAST TESTPREFIX "option\\pie\\text\\test-pie-date.pie")) == NULL || resNodeGetCountDescendants(prnT) < 5) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 2 resNodeSplitStrNew()\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }

    resNodeFree(prnT);
  }


  if (RUNTEST) {
    xmlChar* pucT;
    resNodePtr prnPath = NULL;
    resNodePtr prnT = NULL;
    resNodePtr prnTT = NULL;
    resNodePtr prnTTT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': resNodeInsert() = ", i, __FILE__, __LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if ((prnPath = resNodeSplitStrNew(BAD_CAST "option\\pie\\text\\test.pie")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 2 resNodeSplitStrNew()\n");
    }
    else if ((prnT = resNodeDirNew(BAD_CAST"option/")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew()\n");
    }
    else if (resNodeAddChildNew(prnT, BAD_CAST"a.txt") == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeAddChildNew()\n");
    }
    else if ((prnTT = resNodeAddChildNew(prnT, BAD_CAST"pie/")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeAddChildNew()\n");
    }
    else if ((prnTTT = resNodeAddChildNew(prnTT, BAD_CAST"text/")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeAddChildNew()\n");
    }
    else if (resNodeAddChildNew(prnTTT, BAD_CAST"c.html") == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeAddChildNew()\n");
    }
#if 0
    else if (resNodeAddChildNew(prnTTT, BAD_CAST"test.pie") == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeAddChildNew()\n");
    }
    else if (resNodeInsert(NULL,NULL) != NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeInsert()\n");
    }
    else if (resNodeInsert(prnPath,NULL) != NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeInsert()\n");
    }
#endif
    else if (resNodeInsert(prnT, prnPath) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeInsert()\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }

    //pucT = resNodeListToPlain(prnT,RN_INFO_MIN); fputs((const char *)(const char *)pucT,stderr); xmlFree(pucT);
    resNodeFree(prnPath);
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    xmlChar* pucT;
    resNodePtr prnT = NULL;
    resNodePtr prnTT = NULL;
    resNodePtr prnTTT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': resNodeInsert() = ", i, __FILE__, __LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if ((prnT = resNodeSplitStrNew(BAD_CAST "option\\pie\\text\\test.pie")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 2 resNodeSplitStrNew()\n");
    }
    else if (resNodeInsertStrNew(prnT, BAD_CAST "option\\pie\\text\\test.pie") != NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeInsert()\n");
    }
    else if (resNodeInsertStrNew(prnT, BAD_CAST "option\\pie\\test.pie") == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeInsert()\n");
    }
    else if (resNodeInsertStrNew(prnT, BAD_CAST "option\\test.pie") == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeInsert()\n");
    }
    else if (resNodeInsertStrNew(prnT, BAD_CAST "test.pie") == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeInsert()\n");
    }
    else if (resNodeInsertStrNew(prnT, NULL) != NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeInsert()\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    

    //pucT = resNodeListToPlain(prnT,RN_INFO_MIN); fputs((const char *)(const char *)pucT,stderr); xmlFree(pucT);
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    xmlChar* pucT;
    resNodePtr prnT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': resNodeSplitLineBufferNew() = ", i, __FILE__, __LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if ((prnT = resNodeSplitLineBufferNew(NULL)) != NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 2 resNodeSplitStrNew()\n");
    }
    else if ((prnT = resNodeSplitLineBufferNew(BAD_CAST "option\\pie\\text\\test.pie\noption/pie/abc.xml\n\ntmp\\def.txt\n \n\r")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeInsert()\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }

    //pucT = resNodeListToPlain(prnT,RN_INFO_MIN); fputs((const char *)(const char *)pucT,stderr); xmlFree(pucT);
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    xmlChar mucT[BUFFER_LENGTH];

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': build a list of filesystem contexts = ",i,__FILE__,__LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    xmlStrPrintf(mucT,BUFFER_LENGTH-1, TEMPPREFIX"//%c/tmp/%c",PATHLIST_SEPARATOR,PATHLIST_SEPARATOR);
    if ((prnT = resNodeStrNew(mucT)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeStrNew(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeGetLength(prnT) != 2) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeGetLength(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    resNodeListFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': new filesystem context set to an existing file context with quotes = ",i,__FILE__,__LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';
    
    if ((prnT = resNodeRootNew(NULL,BAD_CAST "\"" TESTPREFIX "plain/Length_1024.txt\"")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeRootNew(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeReadStatus(prnT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeRootNew(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }

    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    xmlChar *pucT;
    xmlChar *pucTT;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': URI of an existing file context = ",i,__FILE__,__LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "config-test.cxp")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeRootNew(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if ((pucT = resNodeGetURI(prnT)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeGetURI(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if ((pucTT = resPathCollapse(BAD_CAST"file://///" TESTPREFIX "config-test.cxp", FS_PATH_FULL)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resPathCollapse(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resPathIsEquivalent(pucT,pucTT) == FALSE) {
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }

    xmlFree(pucTT);
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    xmlChar *pucT;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': URI of an existing URL context = ",i,__FILE__,__LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';


    if ((prnT = resNodeDirNew(BAD_CAST"https://www.test.com:8181/path/file.txt?a=b&c=123")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeIsURL(prnT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeIsURL(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if ((pucT = resNodeGetURI(prnT)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeGetURI(): %s\n", resNodeGetErrorMsg(prnT));
    }
    else if (resPathIsEquivalent(pucT,BAD_CAST"https://www.test.com:8181/path/file.txt?a=b&c=123") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resPathIsEquivalent(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    xmlChar *pucT;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': URI of an existing URL context into an archive = ",i,__FILE__,__LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';


    if ((prnT = resNodeDirNew(BAD_CAST HTTPPREFIX "test-zip-7.zip/path/test.txt")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeGetChild(prnT) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if ((pucT = resNodeGetURI(prnT)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeGetURI(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resPathIsEquivalent(pucT,BAD_CAST HTTPPREFIX "test-zip-7.zip") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resPathIsEquivalent(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resPathIsEquivalent(resNodeGetURI(resNodeGetChild(prnT)),BAD_CAST HTTPPREFIX "test-zip-7.zip/path/") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resPathIsEquivalent(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    xmlChar *pucTT;
    resNodePtr prnT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': set basename and basedir of an context = ",i,__FILE__,__LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "xml/config.cxp")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"ERROR\n");
    }
    else if ((pucTT = resPathCollapse(BAD_CAST TESTPREFIX "xml",FS_PATH_FULL)) == NULL) {
    }
    else if (xmlStrcasecmp(resNodeGetNameBase(prnT),BAD_CAST"config.cxp") != 0) {
    }
    else if (resPathIsEquivalent(resNodeGetNameBaseDir(prnT), pucTT) == FALSE) {
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
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
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': new filesystem context  = ", i, __FILE__, __LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    pucTest = resPathNormalize(BAD_CAST TESTPREFIX "plain/test-plain-3.xml");

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew()\n");
    }
    else if ((prnDir = resNodeAddChildNew(prnT, BAD_CAST"plain/")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeAddChildNew()\n");
    }
    else if (resNodeGetType(prnDir) != rn_type_dir) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeGetType()\n");
    }
    else if ((prnFile = resNodeAddChildNew(prnDir, BAD_CAST"test-plain-3.xml")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeAddChildNew()\n");
    }
    else if (resNodeGetType(prnFile) != rn_type_file) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeGetType()\n");
    }
    else if ((pucT = resNodeGetNameNormalized(prnFile)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeGetNameNormalized('%s')\n", pucT);
    }
    else if (resPathIsEquivalent(pucTest, pucT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resPathIsEquivalent(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
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
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': set internal path = ", i, __FILE__, __LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if ((prnT = resNodeDirNew(BAD_CAST pucT)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew()\n");
    }
    else if (resPathIsEquivalent(resNodeGetNameNormalized(prnT), pucTT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resPathIsEquivalent()\n");
    }
    else if (resNodeGetType(prnT) != rn_type_archive) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeGetType()\n");
    }
    else if (resNodeGetMimeType(prnT) != MIME_APPLICATION_ZIP) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeGetMimeType()\n");
    }
    else if ((prnChild = resNodeGetChild(prnT)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeGetChild()\n");
    }
    else if (resNodeGetType(prnChild) != rn_type_dir_in_archive) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeGetType()\n");
    }
    else if (resNodeGetMimeType(prnChild) != MIME_INODE_DIRECTORY) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeGetMimeType()\n");
    }
    else if ((prnChild = resNodeGetChild(prnChild)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeGetChild()\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    resNodeFree(prnT);
    xmlFree(pucTT);
    xmlFree(pucT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': resNodeAddSibling() = ", i, __FILE__, __LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if ((prnT = resNodeDirNew(BAD_CAST"test/dummy.txt")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew()\n");
    }
    else if (resNodeAddSibling(prnT, resNodeDirNew(BAD_CAST"a.png")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeAddSibling()\n");
    }
    else if (resNodeAddSibling(prnT, resNodeDirNew(BAD_CAST"b.png")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeAddSibling()\n");
    }
    else if (resNodeAddSibling(prnT, resNodeDirNew(BAD_CAST"c.png")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeAddSibling()\n");
    }
    else if (resNodeGetLength(prnT) != 4) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeGetLength()\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    resNodeListFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    int j;
    
    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': resNodeAddChildNew() = ", i, __FILE__, __LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if ((prnT = resNodeDirNew(BAD_CAST"test/")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew()\n");
    }
    else if (resNodeAddChildNew(prnT, BAD_CAST"a.png") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeAddChildNew()\n");
    }
    else if (resNodeAddChildNew(prnT, BAD_CAST"b.png") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeAddChildNew()\n");
    }
    else if (resNodeAddChildNew(prnT, BAD_CAST"c.png") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeAddChildNew()\n");
    }
    else if (resNodeGetParent(resNodeGetChild(prnT)) != prnT) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeGetParent()\n");
    }
    else if (resNodeGetParent(resNodeGetNext(resNodeGetChild(prnT))) != prnT) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeGetParent()\n");
    }
    else if ((j = resNodeGetChildCount(prnT, rn_type_file)) != 3) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeGetChildCount() = %i\n",j);
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': resNodeInMemoryNew() = ",i,__FILE__,__LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if ((prnT = resNodeInMemoryNew()) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error\n");
    }
    else {
      n_ok++;
    }
 
    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': resNodeAliasNew() = ",i,__FILE__,__LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';


    if ((prnT = resNodeAliasNew(BAD_CAST":TEST:")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeAliasNew()\n");
    }
    else if (resNodeSetContentPtr(prnT,(void *)xmlStrdup(BAD_CAST"Content"),8) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeSetContentPtr()\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': concat path of an context = ", i, __FILE__, __LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if ((prnT = resNodeConcatNew(BAD_CAST"/tmp//", BAD_CAST"./test-res_node/xml/config.cxp")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeConcatNew()\n");
    }
    else if (resPathIsEquivalent(resNodeGetNameNormalized(prnT), BAD_CAST "\\tmp\\test-res_node\\xml\\config.cxp") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resPathIsEquivalent()\n");
    }
    else if (resPathIsEquivalent(resNodeGetNameBase(prnT), BAD_CAST"config.cxp") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resPathIsEquivalent()\n");
    }
    else if (resPathIsEquivalent(resNodeGetNameBaseDir(prnT), BAD_CAST "\\tmp\\test-res_node\\xml") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resPathIsEquivalent()\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': reset existing filesystem context = ", i, __FILE__, __LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "/thread/config.cxp")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew()\n");
    }
    else if (resNodeReset(prnT, NULL) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeReset()\n");
    }
    else if (resNodeReadStatus(prnT) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeReset()\n");
    }
    else if (resNodeIsDir(prnT) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeReset()\n");
    }
    else if (resNodeIsFile(prnT) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeReset()\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
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
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': resNodeDup() = ", i, __FILE__, __LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew()\n");
    }
    else if (resNodeAddChildNew(prnT, BAD_CAST"a.txt") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeAddChildNew()\n");
    }
    else if (resNodeSetContentPtr(prnT, pucT, xmlStrlen(pucT) + 1) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeSetContentPtr()\n");
    }
    else if (resNodeAddChildNew(prnT, BAD_CAST"b.png") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeAddChildNew()\n");
    }
    else if (resNodeAddChildNew(prnT, BAD_CAST"c.png") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeAddChildNew()\n");
    }
    else if ((prnTT = resNodeDirNew(BAD_CAST TESTPREFIX "empty/")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew()\n");
    }
    else if (resNodeAddSibling(prnT,prnTT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeAddSibling()\n");
    }
    else if ((prnTTT = resNodeDup(prnT, (RN_DUP_CONTENT))) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDup()\n");
    }
    else if (resNodeGetContentPtr(prnTTT) == NULL || resNodeGetContentPtr(prnTTT) == pucT || xmlStrlen(BAD_CAST resNodeGetContentPtr(prnTTT)) != 6) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeGetContentPtr()\n");
    }
    else if ((j = resNodeGetChildCount(prnTTT, rn_type_file)) != 0) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeGetChildCount() = %i\n",j);
    }
    else if (resNodeGetNext(prnTTT) != NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeGetNext()\n");
    }
    else if ((prnTTTT = resNodeDup(prnT, (RN_DUP_CONTENT | RN_DUP_CHILDS | RN_DUP_NEXT))) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDup()\n");
    }
    else if (resNodeGetContentPtr(prnTTTT) == NULL || resNodeGetContentPtr(prnTTTT) == pucT || xmlStrlen(BAD_CAST resNodeGetContentPtr(prnTTTT)) != 6) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeGetContentPtr()\n");
    }
    else if ((j = resNodeGetChildCount(prnTTTT, rn_type_file)) != 3) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeGetChildCount() = %i\n",j);
    }
    else if (resNodeGetNext(prnTTTT) == NULL || resNodeGetNext(prnTTTT) == prnT) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeGetNext()\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
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
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': construct a new filesystem context with string and context = ",i,__FILE__,__LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if ((prnContext = resNodeDirNew(BAD_CAST TESTPREFIX)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew()\n");
    }
    else if ((prnT = resNodeFromNodeNew(prnContext,BAD_CAST"thread/config.cxp")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew()\n");
    }
    else if ((pucTT = resPathCollapse(BAD_CAST TESTPREFIX "thread\\config.cxp",FS_PATH_FULL)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeAddChildNew()\n");
    }
    else if (resPathIsEquivalent(resNodeGetNameNormalized(prnT),pucTT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resPathIsEquivalent()\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    xmlFree(pucTT);
    resNodeFree(prnT);
    resNodeFree(prnContext);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': set recursion of existing filesystem context = ",i,__FILE__,__LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';


    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "/")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew()\n");
    }
    else if (resNodeReadStatus(prnT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1\n");
    }
    else if (resNodeIsDir(prnT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 2\n");
    }
    else if (resNodeIsRecursive(prnT)) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 3\n");
    }
    else if (resNodeSetRecursion(prnT,TRUE) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 4\n");
    }
    else if (resNodeIsRecursive(prnT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 5\n");
    }
    else if (resNodeGetNameBase(prnT) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 6\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    

    resNodeFree(prnT);
  }


  if (SKIPTEST) {
    resNodePtr prnT = NULL;
    resNodePtr prnTT = NULL;
    xmlChar *pucT = BAD_CAST TEMPPREFIX "empty.png";

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': resNodeCommentNew() = ", i, __FILE__, __LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if ((prnT = resNodeDirNew(pucT)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew()\n");
    }
    else if ((prnTT = resNodeCommentNew(prnT)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeReadStatus()\n");
    }
    else if (xmlStrlen(resNodeGetNameNormalized(prnTT)) - xmlStrlen(resNodeGetNameNormalized(prnT)) != 4) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 6\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    

    resNodeFree(prnTT);
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': _resNodeSetNameAncestor() = ",i,__FILE__,__LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';


    if ((prnT = resNodeDirNew(BAD_CAST"AAA")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1\n");
    }
    else if (resNodeReset(prnT,NULL) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 2\n");
    }
    else if (_resNodeSetNameAncestor(prnT,NULL)) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 3\n");
    }
    else if (resNodeReset(prnT,NULL) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 4\n");
    }
    else if (_resNodeSetNameAncestor(NULL,BAD_CAST TEMPPREFIX)) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 5\n");
    }
    else if (resNodeReset(prnT,NULL) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 6\n");
    }
    else if (_resNodeSetNameAncestor(prnT,BAD_CAST TEMPPREFIX) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 7\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    

    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    xmlChar* pucT;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': resNodeSetToParent() = ",i,__FILE__,__LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';


    if ((pucT = resPathCollapse(BAD_CAST TEMPPREFIX, FS_PATH_FULL)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error\n");
    }
    else if ((prnT = resNodeDirNew(BAD_CAST TEMPPREFIX "/AAA/BBB")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error\n");
    }
    else if (resNodeSetToParent(prnT) == FALSE || resNodeSetToParent(prnT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error\n");
    }
    else if (resPathIsEquivalent(resNodeGetNameNormalized(prnT), pucT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    resNodeFree(prnT);
    xmlFree(pucT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': resNodeIsURL() = ",i,__FILE__,__LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if ((prnT = resNodeDirNew(BAD_CAST TEMPPREFIX)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error\n");
    }
    else if (resNodeIsURL(prnT) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error\n");
    }
    else if (resNodeReset(prnT, BAD_CAST CXP_VER_URL) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error\n");
    }
    else if (resNodeIsURL(prnT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    xmlChar *pucT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': resNodeGetExtension() = ", i, __FILE__, __LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if ((prnT = resNodeDirNew(BAD_CAST TEMPPREFIX "empty.png")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error\n");
    }
    else if ((pucT = resNodeGetExtension(prnT)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error\n");
    }
    else if (xmlStrEqual(pucT,BAD_CAST "png") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error\n");
    }
    else if (resNodeReset(prnT,BAD_CAST TEMPPREFIX "empty.jpg") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error\n");
    }
    else if ((pucT = resNodeGetExtension(prnT)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error\n");
    }
    else if (xmlStrEqual(pucT,BAD_CAST "jpg") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error\n");
    }
    else if (resPathIsEquivalent(resNodeGetNameBase(prnT), BAD_CAST "empty.jpg") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': resNodeGetType() = ", i, __FILE__, __LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if ((prnT = resNodeDirNew(BAD_CAST TEMPPREFIX "empty.png")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error\n");
    }
    else if (resNodeGetType(prnT) != rn_type_file) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error\n");
    }
    else if (resNodeReset(prnT,BAD_CAST TEMPPREFIX "empty/") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error\n");
    }
    else if (resNodeGetType(prnT) != rn_type_dir) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error\n");
    }
    else if (resNodeReset(prnT, BAD_CAST CXP_VER_URL) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error\n");
    }
    else if (resNodeGetType(prnT) != rn_type_url_http) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error\n");
    }
    else if (resNodeReset(prnT,BAD_CAST "-") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error\n");
    }
    else if (resNodeGetType(prnT) != rn_type_stdin) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    resNodePtr prnTT = NULL;
    xmlChar *pucT;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': resNodeGetNameRelative() = ", i, __FILE__, __LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if ((prnT = resNodeDirNew(BAD_CAST TEMPPREFIX "empty/dummy.txt")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew()\n");
    }
    else if ((prnTT = resNodeDirNew(BAD_CAST TEMPPREFIX)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew()\n");
    }
    else if ((pucT = resNodeGetNameRelative(prnTT, prnT)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeGetNameRelative()\n");
    }
    else if (resPathIsEquivalent(pucT, BAD_CAST "empty/dummy.txt") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeGetNameRelative()\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    resNodeFree(prnTT);
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    xmlChar *pucA;
    xmlChar *pucB;
    xmlChar *pucC = NULL;
    resNodePtr prnT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': rebuild existing filesystem context = ",i,__FILE__,__LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "thread/config.cxp")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew()\n");
    }
    else if (resNodeReadStatus(prnT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew()\n");
    }
    else if (resNodeIsDir(prnT) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew()\n");
    }
    else if (resNodeIsFile(prnT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew()\n");
    }
    else if ((pucA = resNodeGetNameBaseDir(prnT)) == NULL) {
    }
    else if ((pucB = resNodeGetNameBase(prnT)) == NULL) {
    }
    else if ((pucC = resPathConcatNormalized(pucA,pucB)) == NULL) {
    }
    else if (resNodeReset(prnT,pucC) == FALSE) {
    }
    else if (resNodeReadStatus(prnT) == FALSE) {
    }
    else if (resNodeIsDir(prnT) == TRUE && resNodeIsFile(prnT) == FALSE) {
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    xmlFree(pucC);
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    xmlChar *pucTT = NULL;
    resNodePtr prnT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': re-use of an used context = ",i,__FILE__,__LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "test.mak")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew()\n");
    }
    else if (xmlStrcasecmp(resNodeGetNameBase(prnT),BAD_CAST"test.mak") != 0) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew()\n");
    }
    else if ((pucTT = resPathCollapse(BAD_CAST TESTPREFIX,FS_PATH_FULL)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew()\n");
    }
    else if (resPathIsEquivalent(resNodeGetNameBaseDir(prnT), pucTT) == FALSE) {
    }
    else if ((pucTT = resPathCollapse(BAD_CAST TESTPREFIX "test.mak",FS_PATH_FULL)) == NULL) {
    }
    else if (resPathIsEquivalent(resNodeGetNameNormalized(prnT), pucTT) == FALSE) {
    }
    else if (resNodeSetNameBaseNative(prnT,"t.txt") == FALSE) {
    }
    else if (xmlStrcasecmp(resNodeGetNameBase(prnT),BAD_CAST"t.txt") != 0) {
    }
    else if ((pucTT = resPathCollapse(BAD_CAST TESTPREFIX,FS_PATH_FULL)) == NULL) {
    }
    else if (resPathIsEquivalent(resNodeGetNameBaseDir(prnT), pucTT) == FALSE) {
    }
    else if ((pucTT = resPathCollapse(BAD_CAST TESTPREFIX "t.txt",FS_PATH_FULL)) == NULL) {
    }
    else if (resPathIsEquivalent(resNodeGetNameNormalized(prnT), pucTT) == FALSE) {
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }

    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': parsing of a non-existing filesystem directory = ",i,__FILE__,__LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';


    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "dummy//.")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew()\n");
    }
    else if (resNodeDirAppendEntries(prnT,NULL) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirAppendEntries()\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    size_t j;
    resNodePtr prnT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': parsing of an existing filesystem directory = ", i, __FILE__, __LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "each")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew()\n");
    }
    else if (resNodeReadStatus(prnT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeReadStatus()\n");
    }
    else if (resNodeDirAppendEntries(prnT,NULL) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirAppendEntries()\n");
    }
    else if ((j = resNodeGetChildCount(prnT, rn_type_file)) != 4) {
      /* without resNodeReadStatus() for all childs, they are files by default */
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirAppendEntries() = %i\n", j);
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
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
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': parsing of an existing filesystem directory with pattern matching = ", i, __FILE__, __LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    re_match = pcre2_compile(
      (PCRE2_SPTR8)"C+", /* the pattern */
      PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
      opt_match_pcre,         /* default options */
      &errornumber,          /* for error number */
      &erroroffset,          /* for error offset */
      NULL);                 /* use default compile context */

    if (re_match == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error pcre2_compile()\n");
    }
    else if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew()\n");
    }
    else if (resNodeReadStatus(prnT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeReadStatus()\n");
    }
    else if (resNodeDirAppendEntries(prnT, re_match) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirAppendEntries()\n");
    }
    else if ((j = resNodeGetChildCount(prnT, rn_type_file)) < 10 && j > 11) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirAppendEntries() = %i\n", j);
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    pcre2_code_free(re_match);
    resNodeFree(prnT);
  }
#endif


  if (SKIPTEST) {
    resNodePtr prnT = NULL;
    resNodePtr prnTT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': link resolving twice = ",i,__FILE__,__LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if ((prnT = resNodeDirNew(BAD_CAST"./test.lnk")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew()\n");
    }
    else if (resNodeReadStatus(prnT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew()\n");
    }
    else if (resNodeIsLink(prnT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew()\n");
    }
    if ((prnTT = resNodeResolveLinkChildNew(prnT)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew()\n");
    }
    else if (resNodeGetParent(prnTT) != prnT) {
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    resNodeFree(prnTT);
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    xmlChar *pucT = NULL;
    xmlNodePtr pndT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': resNodeTo*() XML file = ", i, __FILE__, __LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "xml/dummy-a.xml")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew()\n");
    }
    else if (resNodeReadStatus(prnT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeReadStatus()\n");
    }
    else if (resNodeUpdate(prnT, RN_INFO_MAX, NULL, NULL) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeUpdate()\n");
    }
    else if ((pndT = resNodeToDOM(prnT, RN_INFO_MAX)) == NULL || domNumberOfChild(pndT, NULL) != 1) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeToDOM()\n");
    }
    else if ((pucT = resNodeToPlain(prnT, RN_INFO_MAX)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeToPlain() = %i\n",xmlStrlen(pucT));
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
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
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': check filesystem context CRC = ",i,__FILE__,__LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    prnT = resNodeRootNew(BAD_CAST"history/acal/acal.c");
    if (resNodeGetFileCrc(prnT)==1028943022) {
      n_ok++;
    }
    else {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error _resNodeGetFileCrc()\n");
    }
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    xmlChar *pucPath = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': create an local index file = ",i,__FILE__,__LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';


    if ((prnT = resNodeIndexNew(BAD_CAST TESTPREFIX "//")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeIndexNew()\n");
    }
    else if (resNodeIndexSave(prnT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeIsMemory()\n");
    }
    else if ((pucPath = resNodeIndexFind(prnT,BAD_CAST "text\\test-pie-date.pie")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeIsMemory()\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
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
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': resNodeToDOM() = ",i,__FILE__,__LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';


    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "plain/test-plain-3.xml")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error domGetFirstChild()\n");
    }
    else if (resNodeReadStatus(prnT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeReadStatus()\n");
    }
    else if (resNodeUpdate(prnT, RN_INFO_MAX, NULL, NULL) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeUpdate()\n");
    }
    else if ((pndTest = resNodeToDOM(prnT,RN_INFO_MAX)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeToDOM()\n");
    }
    else if (IS_NODE_FILE(pndTest) == FALSE || IS_NODE_PIE(pndTest->children) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error PIE\n");
    }
    else if (domGetAttributePtr(pndTest,BAD_CAST"object") != NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error object\n");
    }
    else if (xmlStrEqual(domGetAttributePtr(pndTest,BAD_CAST"type"), BAD_CAST resNodeGetMimeTypeStr(prnT)) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error domGetFirstChild()\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
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
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': resNodeReadStatus() = ",i,__FILE__,__LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';


    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "dir\\test-cad-creo.drw.11")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew()\n");
    }
    else if (resNodeReadStatus(prnT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeReadStatus()\n");
    }
    else if (resNodeUpdate(prnT, RN_INFO_MAX, NULL, NULL) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeUpdate()\n");
    }
    else if ((pndTest = resNodeToDOM(prnT,RN_INFO_MAX)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeToDOM()\n");
    }
    else if (IS_NODE_FILE(pndTest) == FALSE || pndTest->children != NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error PIE\n");
    }
    else if (xmlStrEqual(domGetAttributePtr(pndTest,BAD_CAST"type"), BAD_CAST resNodeGetMimeTypeStr(prnT)) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error type\n");
    }
    else if (xmlStrEqual(domGetAttributePtr(pndTest,BAD_CAST"object"), BAD_CAST "TESTCONTENT.DRW") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error object\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    //domPutNodeString(stderr, BAD_CAST "resNodeToDOM(): ", pndTest);
    xmlFreeNodeList(pndTest);
    resNodeFree(prnT);
  }

  xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"\nResult in '%s': %i/%i OK\n",__FILE__,n_ok,i);
  fputs((const char *)mucTestResult,stderr);
  fputs((const char *)pucModuleTestReport,stderr);

  xmlFree(pucModuleTestReport);
  return (i - n_ok);
}
