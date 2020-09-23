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
    printf("TEST %i in '%s:%i': resNodeMakeDirectory() = ", i, __FILE__, __LINE__);

    resNodeUnlinkStr(pucT);

    if ((prnT = resNodeStrNew(pucT)) == NULL) {
      printf("Error resNodeStrNew()\n");
    }
    else if (resNodeMakeDirectory(NULL,iMode) == TRUE) {
      printf("Error 1 resNodeMakeDirectory()\n");
    }
    else if (resNodeMakeDirectory(prnT,iMode) == FALSE) {
      printf("Error 2 resNodeMakeDirectory()\n");
    }
    else if (resNodeTestDirStr(pucT) == FALSE) {
      printf("Error 1 resNodeTestDirStr()\n");
    }
    else if (resNodeUnlink(prnT,TRUE) == FALSE) {
      printf("Error 2 resNodeUnlink()\n");
    }
    else if (resNodeTestDirStr(pucT) == TRUE) {
      printf("Error 2 resNodeTestDirStr()\n");
    }
    else if (resNodeUnlink(NULL,FALSE) == TRUE) {
      printf("Error 1 resNodeUnlink()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
  }


  if (RUNTEST) {

    i++;
    printf("TEST %i in '%s:%i': create a new subsubdirectory = ", i, __FILE__, __LINE__);

    if (resNodeMakeDirectoryStr(NULL, iMode) == TRUE) {
      printf("Error 1 resNodeMakeDirectoryStr()\n");
    }
    else if (resNodeMakeDirectoryStr(BAD_CAST TEMPPREFIX "test-res_node_ops/ooo/kkk", iMode) == FALSE) {
      printf("Error 2 resNodeMakeDirectoryStr()\n");
    }
    else if (resNodeTestDirStr(BAD_CAST TEMPPREFIX "test-res_node_ops/ooo/kkk") == FALSE) {
      printf("Error 1 resNodeTestDirStr()\n");
    }
    else if (resNodeUnlinkRecursivelyStr(BAD_CAST TEMPPREFIX "test-res_node_ops") == FALSE) {
      printf("Error 3 resNodeUnlinkStr()\n");
    }
    else if (resNodeTestDirStr(BAD_CAST TEMPPREFIX "test-res_node_ops") == TRUE) {
      printf("Error 2 resNodeTestDirStr()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }


  if (RUNTEST) {
    resNodePtr prnFrom = NULL;
    resNodePtr prnTo = NULL;

    i++;
    printf("TEST %i in '%s:%i': resNodeTransfer(copy) = ", i, __FILE__, __LINE__);

    resNodeUnlinkStr(BAD_CAST TEMPPREFIX "test.ics");
    
    if ((prnFrom = resNodeDirNew(BAD_CAST TESTPREFIX "pie/2446.ics")) == NULL) {
      printf("Error 1 resNodeDirNew()\n");
    }
    else if ((prnTo = resNodeDirNew(BAD_CAST TEMPPREFIX "test.ics")) == NULL) {
      printf("Error 2 resNodeDirNew()\n");
    }
    else if (resNodeTransfer(NULL, NULL, FALSE) == TRUE) {
      printf("Error 1 resNodeTransfer()\n");
    }
    else if (resNodeTransfer(NULL, prnTo, FALSE) == TRUE) {
      printf("Error 2 resNodeTransfer()\n");
    }
    else if (resNodeTransfer(prnFrom, NULL, FALSE) == TRUE) {
      printf("Error 3 resNodeTransferStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TESTPREFIX "pie/2446.ics") == FALSE) {
      printf("Error 1 resNodeTestFileStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "test.ics") == TRUE) {
      printf("Error 2 resNodeTestFileStr()\n");
    }
    else if (resNodeTestDirStr(BAD_CAST TEMPPREFIX) == FALSE) {
      printf("Error resNodeTestDirStr()\n");
    }
    else if (resNodeTransfer(prnFrom, prnTo, FALSE) == FALSE) {
      printf("Error 4 resNodeTransfer()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TESTPREFIX "pie/2446.ics") == FALSE) {
      printf("Error 3 resNodeTestFileStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "test.ics") == FALSE) {
      printf("Error 4 resNodeTestFileStr()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    resNodeFree(prnTo);
    resNodeFree(prnFrom);
  }


  if (SKIPTEST) {
    i++;
    printf("TEST %i in '%s:%i': dummy arguments for resNodeTransferStr(copy) = ", i, __FILE__, __LINE__);

    if (resNodeTransferStr(NULL, NULL, FALSE) == TRUE) {
      printf("Error resNodeTransferStr()\n");
    }
    else if (resNodeTransferStr(NULL, BAD_CAST TEMPPREFIX, FALSE) == TRUE) {
      printf("Error resNodeTransferStr()\n");
    }
    else if (resNodeTransferStr(BAD_CAST TEMPPREFIX, NULL, FALSE) == TRUE) {
      printf("Error resNodeTransferStr()\n");
    }
    else if (resNodeTestDirStr(BAD_CAST TEMPPREFIX) == FALSE) {
      printf("Error resNodeTestDirStr()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  
  if (RUNTEST) {
    resNodePtr prnFrom = NULL;
    resNodePtr prnTo = NULL;

    i++;
    printf("TEST %i in '%s:%i': resNodeTransfer(move) = ", i, __FILE__, __LINE__);
    
    if ((prnFrom = resNodeDirNew(BAD_CAST TEMPPREFIX "test.ics")) == NULL) {
      printf("Error 1 resNodeDirNew()\n");
    }
    else if ((prnTo = resNodeDirNew(BAD_CAST TEMPPREFIX "sub/subsub/t.ics")) == NULL) {
      printf("Error 2 resNodeDirNew()\n");
    }
    else if (resNodeTransfer(NULL, NULL, TRUE) == TRUE) {
      printf("Error 1 resNodeTransfer()\n");
    }
    else if (resNodeTransfer(NULL, prnTo, TRUE) == TRUE) {
      printf("Error 2 resNodeTransfer()\n");
    }
    else if (resNodeTransfer(prnFrom, NULL, TRUE) == TRUE) {
      printf("Error 3 resNodeTransferStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "sub/subsub/t.ics") == TRUE) {
      printf("Error 1 resNodeTestFileStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "test.ics") == FALSE) {
      printf("Error 2 resNodeTestFileStr()\n");
    }
    else if (resNodeTestDirStr(BAD_CAST TEMPPREFIX) == FALSE) {
      printf("Error resNodeTestDirStr()\n");
    }
    else if (resNodeTransfer(prnFrom, prnTo, TRUE) == FALSE) {
      printf("Error 4 resNodeTransferStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "sub/subsub/t.ics") == FALSE) {
      printf("Error 3 resNodeTestFileStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "test.ics") == TRUE) {
      printf("Error 4 resNodeTestFileStr()\n");
    }
    else if (resNodeTestDirStr(BAD_CAST TEMPPREFIX) == FALSE) {
      printf("Error 1 resNodeTestDirStr()\n");
    }
    else if (resNodeUnlinkRecursivelyStr(BAD_CAST TEMPPREFIX "sub") == FALSE) {
      printf("Error 3 resNodeUnlinkStr()\n");
    }
    else if (resNodeTestDirStr(BAD_CAST TEMPPREFIX "sub") == TRUE) {
      printf("Error 2 resNodeTestDirStr()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    resNodeFree(prnTo);
    resNodeFree(prnFrom);
  }


  if (SKIPTEST) {
    i++;
    printf("TEST %i in '%s:%i': dummy arguments for resNodeTransferStr(move) = ", i, __FILE__, __LINE__);

    if (resNodeTransferStr(NULL, NULL, TRUE) == TRUE) {
      printf("Error 1 resNodeTransferStr()\n");
    }
    else if (resNodeTransferStr(NULL, BAD_CAST TEMPPREFIX, TRUE) == TRUE) {
      printf("Error 2 resNodeTransferStr()\n");
    }
    else if (resNodeTransferStr(BAD_CAST TEMPPREFIX, NULL, TRUE) == TRUE) {
      printf("Error 3 resNodeTransferStr()\n");
    }
    else if (resNodeTestDirStr(BAD_CAST TEMPPREFIX) == FALSE) {
      printf("Error 4 resNodeTestFileStr()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }


  if (RUNTEST) {
    resNodePtr prnFrom = NULL;
    resNodePtr prnTo = NULL;

    i++;
    printf("TEST %i in '%s:%i': copying of an existing plain text file to existing directory = ", i, __FILE__, __LINE__);

    if (resNodeTransferStr(BAD_CAST TESTPREFIX "pie/2446.ics", BAD_CAST TEMPPREFIX, FALSE) == FALSE) {
      printf("Error resNodeTransferStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TESTPREFIX "pie/2446.ics") == FALSE) {
      printf("Error 1 resNodeTestFileStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "2446.ics") == FALSE) {
      printf("Error 2 resNodeTestFileStr()\n");
    }
    else if (resNodeUnlinkStr(BAD_CAST TEMPPREFIX "2446.ics") == FALSE) {
      printf("Error resNodeUnlinkStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "2446.ics") == TRUE) {
      printf("Error 2 resNodeTestFileStr()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }


  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': copying of an existing plain text file to a file in a non-existing directory = ", i, __FILE__, __LINE__);

    if (resNodeTransferStr(BAD_CAST TESTPREFIX "pie/2446.ics", BAD_CAST TEMPPREFIX "test-res_node_ops/t2.txt", FALSE) == FALSE) {
      printf("Error 1 resNodeTransferStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TESTPREFIX "pie/2446.ics") == FALSE) {
      printf("Error 1 resNodeTestFileStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "test-res_node_ops/t2.txt") == FALSE) {
      printf("Error 2 resNodeTestFileStr()\n");
    }
    else if (resNodeUnlinkRecursivelyStr(BAD_CAST TEMPPREFIX "test-res_node_ops") == FALSE) {
      printf("Error 3 resNodeUnlinkStr()\n");
    }
    else if (resNodeTestDirStr(BAD_CAST TEMPPREFIX "test-res_node_ops") == TRUE) {
      printf("Error 2 resNodeTestDirStr()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }


  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': copying of an existing plain text file to stdout = ", i, __FILE__, __LINE__);

    if (resNodeTransferStr(BAD_CAST TESTPREFIX "plain/test-unicode-bom.txt", BAD_CAST "-", FALSE) == FALSE) {
      printf("Error resNodeTransferStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TESTPREFIX "plain/test-unicode-bom.txt") == FALSE) {
      printf("Error resNodeTestFileStr()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }


#ifdef HAVE_LIBARCHIVE
  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': copying of an existing plain text file from archive = ", i, __FILE__, __LINE__);

    if (resNodeTransferStr(BAD_CAST DATAPREFIX "Archive/TestArchive.zip/Test/SubTest/SubTest/Length_1024.txt", BAD_CAST TEMPPREFIX, FALSE) == FALSE) {
      printf("Error resNodeTransferStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "Length_1024.txt") == FALSE) {
      printf("Error 1 resNodeTestFileStr()\n");
    }
    else if (resNodeUnlinkStr(BAD_CAST TEMPPREFIX "Length_1024.txt") == FALSE) {
      printf("Error resNodeUnlinkStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST DATAPREFIX "Archive/TestArchive.zip") == FALSE) {
      printf("Error 2 resNodeTestFileStr()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }
#endif


  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': avoid moving of an existing plain text file stderr oder stdout = ",i,__FILE__,__LINE__);

    if (resNodeTransferStr(BAD_CAST TESTPREFIX "pie/2446.ics",BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt", FALSE) == FALSE) {
      printf("Error resNodeTransferStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt") == FALSE) {
      printf("Error 1 resNodeTestFileStr()\n");
    }
    else if (resNodeTransferStr(BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt",BAD_CAST"-", TRUE) == TRUE) {
      printf("Error 1 resNodeTransferStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt") == FALSE) {
      printf("Error 2 resNodeTestFileStr()\n");
    }
    else if (resNodeUnlinkRecursivelyStr(BAD_CAST TEMPPREFIX "test-res_node_ops") == FALSE) {
      printf("Error resNodeUnlinkStr()\n");
    }
    else if (resNodeTestDirStr(BAD_CAST TEMPPREFIX "test-res_node_ops") == TRUE) {
      printf("Error 3 resNodeTestFileStr()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }


  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': avoid moving of an existing plain text file into same directory = ",i,__FILE__,__LINE__);

    if (resNodeTransferStr(BAD_CAST TESTPREFIX "pie/2446.ics",BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt", FALSE) == FALSE) {
      printf("Error 1 resNodeTransferStr()\n");
    }
    else if (resNodeTransferStr(BAD_CAST TESTPREFIX "pie/2446.ics",BAD_CAST TEMPPREFIX "test-res_node_ops/source/u.txt", FALSE) == FALSE) {
      printf("Error 2 resNodeTransferStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt") == FALSE) {
      printf("Error 1 resNodeTestFileStr()\n");
    }
    else if (resNodeTransferStr(BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt",BAD_CAST TEMPPREFIX "test-res_node_ops/source/", TRUE) == TRUE) {
      printf("Error 1 resNodeTransferStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt") == FALSE) {
      printf("Error 2 resNodeTestFileStr()\n");
    }
    else if (resNodeTransferStr(BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt",BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt", TRUE) == TRUE) {
      printf("Error 2 resNodeTransferStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt") == FALSE) {
      printf("Error 3 resNodeTestFileStr()\n");
    }
    else if (resNodeTransferStr(BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt",BAD_CAST TEMPPREFIX "test-res_node_ops/source/u.txt", TRUE) == TRUE) {
      printf("Error 3 resNodeTransferStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt") == FALSE) {
      printf("Error 4 resNodeTestFileStr()\n");
    }
    else if (resNodeTransferStr(BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt",BAD_CAST TEMPPREFIX "test-res_node_ops/source/u.txt", FALSE) == FALSE) {
      printf("Error 4 resNodeTransferStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "test-res_node_ops/source/u.txt") == FALSE) {
      printf("Error 5 resNodeTestFileStr()\n");
    }
    else if (resNodeTestFileStr(BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt") == FALSE) {
      printf("Error 6 resNodeTestFileStr()\n");
    }
    else if (resNodeUnlinkRecursivelyStr(BAD_CAST TEMPPREFIX "test-res_node_ops") == FALSE) {
      printf("Error resNodeUnlinkStr()\n");
    }
    else if (resNodeTestDirStr(BAD_CAST TEMPPREFIX "test-res_node_ops") == TRUE) {
      printf("Error resNodeTestFileStr()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (SKIPTEST) {
    i++;
    printf("TEST %i in '%s:%i': moving of an existing plain text file to an other volume = ", i, __FILE__, __LINE__);
    if (
      resNodeTransferStr(BAD_CAST TESTPREFIX "pie/2446.ics", BAD_CAST TEMPPREFIX "test-res_node_ops/source/t.txt", FALSE) == TRUE
      && resNodeTestFileStr(BAD_CAST TESTPREFIX "pie/2446.ics") == TRUE
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
      printf("OK\n");
    }
    else {
      printf("Error resNodeTransferStr()\n");
    }
  }


  printf("\nResult in '%s': %i/%i OK\n\n", __FILE__, n_ok, i);

  return (i - n_ok);
}
