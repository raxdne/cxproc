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

/*! test code for module fs
 */
int
resNodeTestOperations(void)
{
  int n_ok;
  int i;
  /* */
  int iMode = 0;
  //xmlChar *pucMsg = "";
  xmlChar *pucModuleTestReport = xmlStrdup("\n");
  xmlChar mucTestLabel[BUFFER_LENGTH];
  xmlChar mucTestResult[BUFFER_LENGTH];
  
#ifdef _MSC_VER
#else
  iMode = (S_IRUSR | S_IWUSR | S_IXUSR);
#endif

  n_ok=0;
  i=0;


  if (RUNTEST) {
    xmlChar *pucT = BAD_CAST TEMPPREFIX "AAA";
    resNodePtr prnT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': resNodeMakeDirectory() = ", i, __FILE__, __LINE__);
    fputs(mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    resNodeUnlinkStr(pucT);

    if ((prnT = resNodeStrNew(pucT)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeStrNew()\n");
    }
    else if (resNodeMakeDirectory(NULL,iMode) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeMakeDirectory()\n");
    }
    else if (resNodeMakeDirectory(prnT,iMode) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 2 resNodeMakeDirectory()\n");
    }
    else if (resNodeTestDirStr(pucT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeTestDirStr()\n");
    }
    else if (resNodeUnlink(prnT,TRUE) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 2 resNodeUnlink()\n");
    }
    else if (resNodeTestDirStr(pucT) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 2 resNodeTestDirStr()\n");
    }
    else if (resNodeUnlink(NULL,FALSE) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeUnlink()\n");
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

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': create a new subsubdirectory = ", i, __FILE__, __LINE__);
    fputs(mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if (resNodeMakeDirectoryStr(NULL, iMode) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeMakeDirectoryStr()\n");
    }
    else if (resNodeMakeDirectoryStr(BAD_CAST TEMPPREFIX "test-res_node_ops/ooo/kkk", iMode) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 2 resNodeMakeDirectoryStr()\n");
    }
    else if (resNodeTestDirStr(BAD_CAST TEMPPREFIX "test-res_node_ops/ooo/kkk") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeTestDirStr()\n");
    }
    else if (resNodeUnlinkRecursivelyStr(BAD_CAST TEMPPREFIX "test-res_node_ops") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 3 resNodeUnlinkStr()\n");
    }
    else if (resNodeTestDirStr(BAD_CAST TEMPPREFIX "test-res_node_ops") == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 2 resNodeTestDirStr()\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
  }


  if (RUNTEST) {
    xmlChar* pucT;
    resNodePtr prnT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': resNodeSplitLineBufferNew() = ", i, __FILE__, __LINE__);
    fputs(mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if ((prnT = resNodeSplitLineBufferNew(BAD_CAST "option\\pie\\text\\\noption/pie/abc/\n\ntmp/a/b/c/\n \ntmp/a/e/f/g/\r")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 2 resNodeSplitStrNew()\n");
    }
    else if (resNodeSetNameBaseDir(prnT, BAD_CAST TEMPPREFIX) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeMakeDirectory()\n");
    }
    else if (resNodeMakeDirectory(prnT, iMode) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeMakeDirectory()\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    

    //pucT = resNodeListToPlain(prnT,RN_INFO_MIN); fputs((const char *)pucT,stderr); xmlFree(pucT);
    resNodeListFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnFrom = NULL;
    resNodePtr prnTo = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': resNodeTransfer(copy) = ", i, __FILE__, __LINE__);
    fputs(mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    resNodeUnlinkStr(BAD_CAST TEMPPREFIX "test.ics");
    
    if ((prnFrom = resNodeDirNew(BAD_CAST TESTPREFIX "option/pie/text/2446.ics")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeDirNew()\n");
    }
    else if ((prnTo = resNodeDirNew(BAD_CAST TEMPPREFIX "test.ics")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 2 resNodeDirNew()\n");
    }
    else if (resNodeTransfer(NULL, NULL, FALSE) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeTransfer()\n");
    }
    else if (resNodeTransfer(NULL, prnTo, FALSE) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 2 resNodeTransfer()\n");
    }
    else if (resNodeTransfer(prnFrom, NULL, FALSE) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 3 resNodeTransferStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TESTPREFIX "option/pie/text/2446.ics") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeTestFileStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "test.ics") == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 2 resNodeTestFileStr()\n");
    }
    else if (resNodeTestDirStr(BAD_CAST TEMPPREFIX) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeTestDirStr()\n");
    }
    else if (resNodeTransfer(prnFrom, prnTo, FALSE) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 4 resNodeTransfer()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TESTPREFIX "option/pie/text/2446.ics") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 3 resNodeTestFileStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "test.ics") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 4 resNodeTestFileStr()\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    

    resNodeFree(prnTo);
    resNodeFree(prnFrom);
  }


  if (SKIPTEST) {
    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': dummy arguments for resNodeTransferStr(copy) = ", i, __FILE__, __LINE__);
    fputs(mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if (resNodeTransferStr(NULL, NULL, FALSE) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeTransferStr()\n");
    }
    else if (resNodeTransferStr(NULL, BAD_CAST TEMPPREFIX, FALSE) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeTransferStr()\n");
    }
    else if (resNodeTransferStr(BAD_CAST TEMPPREFIX, NULL, FALSE) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeTransferStr()\n");
    }
    else if (resNodeTestDirStr(BAD_CAST TEMPPREFIX) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeTestDirStr()\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
  }

  
  if (RUNTEST) {
    resNodePtr prnFrom = NULL;
    resNodePtr prnTo = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': resNodeTransfer(move) = ", i, __FILE__, __LINE__);
    fputs(mucTestLabel,stderr);
    mucTestResult[0] = '\0';
    
    if ((prnFrom = resNodeDirNew(BAD_CAST TEMPPREFIX "test.ics")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeDirNew()\n");
    }
    else if ((prnTo = resNodeDirNew(BAD_CAST TEMPPREFIX "sub/subsub/t.ics")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 2 resNodeDirNew()\n");
    }
    else if (resNodeTransfer(NULL, NULL, TRUE) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeTransfer()\n");
    }
    else if (resNodeTransfer(NULL, prnTo, TRUE) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 2 resNodeTransfer()\n");
    }
    else if (resNodeTransfer(prnFrom, NULL, TRUE) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 3 resNodeTransferStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "sub/subsub/t.ics") == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeTestFileStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "test.ics") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 2 resNodeTestFileStr()\n");
    }
    else if (resNodeTestDirStr(BAD_CAST TEMPPREFIX) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeTestDirStr()\n");
    }
    else if (resNodeTransfer(prnFrom, prnTo, TRUE) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 4 resNodeTransferStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "sub/subsub/t.ics") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 3 resNodeTestFileStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "test.ics") == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 4 resNodeTestFileStr()\n");
    }
    else if (resNodeTestDirStr(BAD_CAST TEMPPREFIX) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeTestDirStr()\n");
    }
    else if (resNodeUnlinkRecursivelyStr(BAD_CAST TEMPPREFIX "sub") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 3 resNodeUnlinkStr()\n");
    }
    else if (resNodeTestDirStr(BAD_CAST TEMPPREFIX "sub") == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 2 resNodeTestDirStr()\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    resNodeFree(prnTo);
    resNodeFree(prnFrom);
  }


  if (SKIPTEST) {
    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': dummy arguments for resNodeTransferStr(move) = ", i, __FILE__, __LINE__);
    fputs(mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if (resNodeTransferStr(NULL, NULL, TRUE) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeTransferStr()\n");
    }
    else if (resNodeTransferStr(NULL, BAD_CAST TEMPPREFIX, TRUE) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 2 resNodeTransferStr()\n");
    }
    else if (resNodeTransferStr(BAD_CAST TEMPPREFIX, NULL, TRUE) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 3 resNodeTransferStr()\n");
    }
    else if (resNodeTestDirStr(BAD_CAST TEMPPREFIX) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 4 resNodeTestFileStr()\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
  }


  if (RUNTEST) {
    resNodePtr prnFrom = NULL;
    resNodePtr prnTo = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': copying of an existing plain text file to existing directory = ", i, __FILE__, __LINE__);
    fputs(mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if (resNodeTransferStr(BAD_CAST TESTPREFIX "option/pie/text/2446.ics", BAD_CAST TEMPPREFIX, FALSE) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeTransferStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TESTPREFIX "option/pie/text/2446.ics") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeTestFileStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "2446.ics") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 2 resNodeTestFileStr()\n");
    }
    else if (resNodeUnlinkStr(BAD_CAST TEMPPREFIX "2446.ics") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeUnlinkStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "2446.ics") == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 2 resNodeTestFileStr()\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
  }


  if (RUNTEST) {
    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': copying of an existing plain text file to a file in a non-existing directory = ", i, __FILE__, __LINE__);
    fputs(mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if (resNodeTransferStr(BAD_CAST TESTPREFIX "option/pie/text/2446.ics", BAD_CAST TEMPPREFIX "test-res_node_ops/t2.txt", FALSE) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeTransferStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TESTPREFIX "option/pie/text/2446.ics") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeTestFileStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "test-res_node_ops/t2.txt") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 2 resNodeTestFileStr()\n");
    }
    else if (resNodeUnlinkRecursivelyStr(BAD_CAST TEMPPREFIX "test-res_node_ops") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 3 resNodeUnlinkStr()\n");
    }
    else if (resNodeTestDirStr(BAD_CAST TEMPPREFIX "test-res_node_ops") == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 2 resNodeTestDirStr()\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
  }


  if (RUNTEST) {
    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': copying of an existing plain text file to stdout = ", i, __FILE__, __LINE__);

    if (resNodeTransferStr(BAD_CAST TESTPREFIX "plain/test-unicode-bom.txt", BAD_CAST "-", FALSE) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeTransferStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TESTPREFIX "plain/test-unicode-bom.txt") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeTestFileStr()\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
  }


#ifdef HAVE_LIBARCHIVE
  if (RUNTEST) {
    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': copying of an existing plain text file from archive = ", i, __FILE__, __LINE__);
    fputs(mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if (resNodeTransferStr(BAD_CAST TESTPREFIX "option\\archive\\test-zip-7.zip\\sub\\a.txt", BAD_CAST TEMPPREFIX, FALSE) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeTransferStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "a.txt") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeTestFileStr()\n");
    }
    else if (resNodeUnlinkStr(BAD_CAST TEMPPREFIX "a.txt") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeUnlinkStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TESTPREFIX "option\\archive\\test-zip-7.zip") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 2 resNodeTestFileStr()\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
  }
#endif


  if (RUNTEST) {
    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': avoid moving of an existing plain text file stderr oder stdout = ",i,__FILE__,__LINE__);

    if (resNodeTransferStr(BAD_CAST TESTPREFIX "option/pie/text/2446.ics",BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt", FALSE) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeTransferStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeTestFileStr()\n");
    }
    else if (resNodeTransferStr(BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt",BAD_CAST"-", TRUE) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeTransferStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 2 resNodeTestFileStr()\n");
    }
    else if (resNodeUnlinkRecursivelyStr(BAD_CAST TEMPPREFIX "test-res_node_ops") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeUnlinkStr()\n");
    }
    else if (resNodeTestDirStr(BAD_CAST TEMPPREFIX "test-res_node_ops") == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 3 resNodeTestFileStr()\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
  }


  if (RUNTEST) {
    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': avoid moving of an existing plain text file into same directory = ",i,__FILE__,__LINE__);

    if (resNodeTransferStr(BAD_CAST TESTPREFIX "option/pie/text/2446.ics",BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt", FALSE) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeTransferStr()\n");
    }
    else if (resNodeTransferStr(BAD_CAST TESTPREFIX "option/pie/text/2446.ics",BAD_CAST TEMPPREFIX "test-res_node_ops/source/u.txt", FALSE) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 2 resNodeTransferStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeTestFileStr()\n");
    }
    else if (resNodeTransferStr(BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt",BAD_CAST TEMPPREFIX "test-res_node_ops/source/", TRUE) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 resNodeTransferStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 2 resNodeTestFileStr()\n");
    }
    else if (resNodeTransferStr(BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt",BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt", TRUE) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 2 resNodeTransferStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 3 resNodeTestFileStr()\n");
    }
    else if (resNodeTransferStr(BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt",BAD_CAST TEMPPREFIX "test-res_node_ops/source/u.txt", TRUE) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 3 resNodeTransferStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 4 resNodeTestFileStr()\n");
    }
    else if (resNodeTransferStr(BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt",BAD_CAST TEMPPREFIX "test-res_node_ops/source/u.txt", FALSE) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 4 resNodeTransferStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "test-res_node_ops/source/u.txt") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 5 resNodeTestFileStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 6 resNodeTestFileStr()\n");
    }
    else if (resNodeUnlinkRecursivelyStr(BAD_CAST TEMPPREFIX "test-res_node_ops") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeUnlinkStr()\n");
    }
    else if (resNodeTestDirStr(BAD_CAST TEMPPREFIX "test-res_node_ops") == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeTestFileStr()\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
  }

  if (SKIPTEST) {
    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': moving of an existing plain text file to an other volume = ", i, __FILE__, __LINE__);
    fputs(mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if (
      resNodeTransferStr(BAD_CAST TESTPREFIX "option/pie/text/2446.ics", BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt", FALSE) == TRUE
      && resNodeTestFileStr(BAD_CAST TESTPREFIX "option/pie/text/2446.ics") == TRUE
      && resNodeTestFileStr(BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt") == TRUE
#ifdef _MSC_VER
      && resNodeTransferStr(BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt", BAD_CAST"F:/tmp/test-res_node_ops/target/u.txt", TRUE) == TRUE
      && resNodeTestFileStr(BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt") == FALSE
      && resNodeTestFileStr(BAD_CAST"F:/tmp/test-res_node_ops/target/u.txt") == TRUE
      && resNodeUnlinkStr(BAD_CAST"F:/tmp/test-res_node_ops/target/u.txt") == TRUE
      && resNodeUnlinkStr(BAD_CAST"F:/tmp/test-res_node_ops/target") == TRUE
      && resNodeUnlinkStr(BAD_CAST"F:/tmp/test-res_node_ops/source") == TRUE
      && resNodeUnlinkStr(BAD_CAST"F:/tmp/test-res_node_ops") == TRUE
#else
      && resNodeTransferStr(BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt", BAD_CAST TEMPPREFIX "test-res_node_ops/target/u.txt", TRUE) == TRUE
      && resNodeTestFileStr(BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt") == FALSE
      && resNodeTestFileStr(BAD_CAST TEMPPREFIX "test-res_node_ops/target/u.txt") == TRUE
      && resNodeUnlinkStr(BAD_CAST TEMPPREFIX "test-res_node_ops/target/u.txt") == TRUE
      && resNodeUnlinkStr(BAD_CAST TEMPPREFIX "test-res_node_ops/target") == TRUE
      && resNodeUnlinkStr(BAD_CAST TEMPPREFIX "test-res_node_ops/source") == TRUE
      && resNodeUnlinkStr(BAD_CAST TEMPPREFIX "test-res_node_ops") == TRUE
#endif
      ) {
      n_ok++;
    }
    else {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeTransferStr()\n");
    }
  }

  xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"\nResult in '%s': %i/%i OK\n",__FILE__,n_ok,i);
  fputs(mucTestResult,stderr);
  fputs(pucModuleTestReport,stderr);

  xmlFree(pucModuleTestReport);

  return (i - n_ok);
}
