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
resNodeTestString(void)
{
  int n_ok;
  int i;

  n_ok=0;
  i=0;


  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': resPathIsRelative() = ", i, __FILE__, __LINE__);

    if (resPathIsRelative(NULL) == FALSE || resPathIsRelative(BAD_CAST"") == FALSE ||resPathIsRelative(BAD_CAST"\"\"") == FALSE) {
      printf("Error 1\n");
    }
    else if (resPathIsRelative(BAD_CAST".") == FALSE || resPathIsRelative(BAD_CAST"\".\"") == FALSE) {
      printf("Error 2\n");
    }
    else if (resPathIsRelative(BAD_CAST"../abc") == FALSE || resPathIsRelative(BAD_CAST"\"..\\D E F\"") == FALSE) {
      printf("Error 3\n");
    }
    else if (resPathIsRelative(BAD_CAST"\"\\D E F\"") == TRUE) {
      printf("Error 4\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': resPathIsRoot() = ", i, __FILE__, __LINE__);

    if (resPathIsRoot(BAD_CAST"c:/") == FALSE || resPathIsRoot(BAD_CAST"\"D:\\\"") == FALSE || resPathIsRoot(BAD_CAST"\"/\"") == FALSE) {
      printf("Error 1\n");
    }
    else if (resPathIsRoot(BAD_CAST"c:/tmp") == TRUE || resPathIsRoot(BAD_CAST"\"D:\\User and Tester\"") == TRUE || resPathIsRoot(BAD_CAST"/tmp") == TRUE) {
      printf("Error 2\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': resPathIsLeadingSeparator() = ", i, __FILE__, __LINE__);

    if (resPathIsLeadingSeparator(BAD_CAST"/abc") == FALSE || resPathIsLeadingSeparator(BAD_CAST"\"\\abc") == FALSE) {
      printf("Error 1\n");
    }
    else if (resPathIsLeadingSeparator(BAD_CAST"abc") == TRUE || resPathIsLeadingSeparator(BAD_CAST"\" abc \"") == TRUE) {
      printf("Error 2\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': resPathIsTrailingSeparator() = ", i, __FILE__, __LINE__);

    if (resPathIsTrailingSeparator(BAD_CAST"/abc//") == FALSE || resPathIsTrailingSeparator(BAD_CAST"\"\\abc\\\"") == FALSE) {
      printf("Error 1\n");
    }
    else if (resPathIsTrailingSeparator(BAD_CAST"/abc") == TRUE || resPathIsTrailingSeparator(BAD_CAST"\" abc \"") == TRUE) {
      printf("Error 2\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': resPathIsUNC() = ", i, __FILE__, __LINE__);

    if (resPathIsUNC(BAD_CAST"\\\\abc") == FALSE || resPathIsUNC(BAD_CAST"\"//abc") == FALSE
	|| resPathIsUNC(BAD_CAST"\\abc") == TRUE || resPathIsUNC(BAD_CAST"\"///abc") == TRUE) {
      printf("Error\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': resPathIsDosDrive() = ", i, __FILE__, __LINE__);

    if (resPathIsDosDrive(BAD_CAST"C:/abc") == FALSE || resPathIsDosDrive(BAD_CAST"\"i:\\abc\"") == FALSE) {
      printf("Error 1\n");
    }
    else if (resPathIsDosDrive(BAD_CAST"CC:/abc") == TRUE || resPathIsDosDrive(BAD_CAST"\" :\\abc\"") == TRUE) {
      printf("Error 2\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': resPathIsHttpURL() = ", i, __FILE__, __LINE__);

    if (resPathIsHttpURL(BAD_CAST"http://abc") == FALSE || resPathIsHttpURL(BAD_CAST"\"http://abc\"") == FALSE) {
      printf("Error 1\n");
    }
    else if (resPathIsHttpURL(BAD_CAST"http:/abc") == TRUE || resPathIsHttpURL(BAD_CAST"\"http//abc\"") == TRUE) {
      printf("Error 2\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': resPathIsFtpURL() = ", i, __FILE__, __LINE__);

    if (resPathIsFtpURL(BAD_CAST"ftp://abc") == FALSE || resPathIsFtpURL(BAD_CAST"\"ftp://abc\"") == FALSE) {
      printf("Error 1\n");
    }
    else if (resPathIsFtpURL(BAD_CAST"ftp:/ /abc") == TRUE || resPathIsFtpURL(BAD_CAST"\"ftp//abc\"") == TRUE) {
      printf("Error 2\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': resPathIsURL() = ", i, __FILE__, __LINE__);

    if (resPathIsURL(BAD_CAST"https://abc") == FALSE || resPathIsURL(BAD_CAST"\"https://abc\"") == FALSE) {
      printf("Error 1\n");
    }
    else if (resPathIsURL(BAD_CAST"https: //abc") == TRUE || resPathIsURL(BAD_CAST"\"https:/abc\"") == TRUE) {
      printf("Error 2\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': resPathBeginIsFileCompressed() = ", i, __FILE__, __LINE__);

    if (resPathBeginIsFileCompressed(NULL) == TRUE || resPathBeginIsFileCompressed(BAD_CAST"") == TRUE || resPathBeginIsFileCompressed(BAD_CAST"\"\"") == TRUE) {
      printf("Error empty\n");
    }
    else if (resPathBeginIsFileCompressed(BAD_CAST"a") == TRUE || resPathBeginIsFileCompressed(BAD_CAST"abcgz") == TRUE
	     || resPathBeginIsFileCompressed(BAD_CAST"gz") == TRUE || resPathBeginIsFileCompressed(BAD_CAST".gz") == TRUE
	     || resPathBeginIsFileCompressed(BAD_CAST"\"../abc.gz \"") == TRUE || resPathBeginIsFileCompressed(BAD_CAST"\"c:\\tmp\\abc.gz\"") == TRUE
	     || resPathBeginIsFileCompressed(BAD_CAST"abc.docx") == TRUE || resPathBeginIsFileCompressed(BAD_CAST"abc.xlsx") == TRUE || resPathBeginIsFileCompressed(BAD_CAST"abc.pptx") == TRUE
	     || resPathBeginIsFileCompressed(BAD_CAST"SubDir/") == TRUE) {
      printf("Error wrong\n");
    }
    else if (resPathBeginIsFileCompressed(BAD_CAST"abc.gz") == FALSE || resPathBeginIsFileCompressed(BAD_CAST"abc.txt.gz") == FALSE
      || resPathBeginIsFileCompressed(BAD_CAST"abc.gz/def") == FALSE) {
      printf("Error right\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': resPathBeginIsArchive() = ", i, __FILE__, __LINE__);

    if (resPathBeginIsArchive(NULL) == TRUE || resPathBeginIsArchive(BAD_CAST"") == TRUE || resPathBeginIsArchive(BAD_CAST"\"\"") == TRUE) {
      printf("Error empty\n");
    }
    else if (resPathBeginIsArchive(BAD_CAST"a") == TRUE || resPathBeginIsArchive(BAD_CAST"abczip") == TRUE
      || resPathBeginIsArchive(BAD_CAST"zip") == TRUE || resPathBeginIsArchive(BAD_CAST".zip") == TRUE
      || resPathBeginIsArchive(BAD_CAST"\"../abc.zip \"") == TRUE || resPathBeginIsArchive(BAD_CAST"\"c:\\tmp\\abc.zip\"") == TRUE
      || resPathBeginIsArchive(BAD_CAST"SubDir/") == TRUE) {
      printf("Error wrong\n");
    }
    else if (resPathBeginIsArchive(BAD_CAST"abc.zip") == FALSE || resPathBeginIsArchive(BAD_CAST"abc.iso") == FALSE
      || resPathBeginIsArchive(BAD_CAST"abc.docx") == FALSE || resPathBeginIsArchive(BAD_CAST"abc.xlsx") == FALSE || resPathBeginIsArchive(BAD_CAST"abc.pptx") == FALSE
      || resPathBeginIsArchive(BAD_CAST"abc.odt") == FALSE || resPathBeginIsArchive(BAD_CAST"abc.odp") == FALSE || resPathBeginIsArchive(BAD_CAST"abc.ods") == FALSE) {
      printf("Error right\n");
    }
    else if (resPathBeginIsArchive(BAD_CAST"abc.zip/dir/test.txt") == FALSE || resPathBeginIsArchive(BAD_CAST"abc.zip/dir/test.txt") == FALSE
      || resPathBeginIsArchive(BAD_CAST"\"abc.xlsx\\excel/sheet.xml\"") == FALSE) {
      printf("Error \n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': simple path shortcuts = ",i,__FILE__,__LINE__);

    if (resPathIsEquivalent(BAD_CAST"abc",BAD_CAST"abc") == FALSE) {
      printf("Error resPathIsEquivalent()\n");
    }
    else if (resPathIsEquivalent(BAD_CAST"abc/",BAD_CAST"abc") == FALSE) {
      printf("Error resPathIsEquivalent()\n");
    }
    else if (resPathIsEquivalent(BAD_CAST"abc",BAD_CAST"abc/") == FALSE) {
      printf("Error resPathIsEquivalent()\n");
    }
    else if (resPathIsEquivalent(BAD_CAST"abc\\.",BAD_CAST"abc") == FALSE) {
      printf("Error resPathIsEquivalent()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }


  if (RUNTEST) {
    xmlChar *pucT;

    i++;
    printf("TEST %i in '%s:%i': local path with nested archive = ",i,__FILE__,__LINE__);

    pucT = BAD_CAST"C:/User/Dummy.zip/abc/def.zip/abc hij/def.tar/sub/Text.docx/word";
    if ((pucT = resPathGetNameOfNextArchivePtr(pucT)) == NULL
	|| (pucT = resPathGetPathInNextArchivePtr(pucT)) == NULL
	|| resPathIsEquivalent(pucT,BAD_CAST"abc/def.zip/abc hij/def.tar/sub/Text.docx/word") == FALSE) {
      printf("Error resPathGetNameOfNextArchivePtr()\n");
    }
    else if ((pucT = resPathGetNameOfNextArchivePtr(pucT)) == NULL
	     || (pucT = resPathGetPathInNextArchivePtr(pucT)) == NULL
	     || resPathIsEquivalent(pucT,BAD_CAST"abc hij/def.tar/sub/Text.docx/word") == FALSE) {
      printf("Error resPathGetNameOfNextArchivePtr()\n");
    }
    else if ((pucT = resPathGetNameOfNextArchivePtr(pucT)) == NULL
	     || (pucT = resPathGetPathInNextArchivePtr(pucT)) == NULL
	     || resPathIsEquivalent(pucT,BAD_CAST"sub/Text.docx/word") == FALSE) {
      printf("Error resPathGetNameOfNextArchivePtr()\n");
    }
    else if ((pucT = resPathGetNameOfNextArchivePtr(pucT)) == NULL
	     || (pucT = resPathGetPathInNextArchivePtr(pucT)) == NULL
	     || resPathIsEquivalent(pucT,BAD_CAST"word") == FALSE) {
      printf("Error resPathGetNameOfNextArchivePtr()\n");
    }
    else if ((pucT = resPathGetNameOfNextArchivePtr(pucT)) != NULL
	     || (pucT = resPathGetPathInNextArchivePtr(pucT)) != NULL) {
      printf("Error resPathGetNameOfNextArchivePtr()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }


  if (RUNTEST) {
    xmlChar *pucT;

    i++;
    printf("TEST %i in '%s:%i': URL with nested archive = ",i,__FILE__,__LINE__);

    pucT = BAD_CAST HTTPPREFIX "test-zip-7.zip/sub/Text.docx/word";
    if ((pucT = resPathGetNameOfNextArchivePtr(pucT)) == NULL
	|| (pucT = resPathGetPathInNextArchivePtr(pucT)) == NULL
	|| resPathIsEquivalent(pucT,BAD_CAST"sub/Text.docx/word") == FALSE) {
      printf("Error resPathGetNameOfNextArchivePtr()\n");
    }
    else if ((pucT = resPathGetNameOfNextArchivePtr(pucT)) == NULL
	     || (pucT = resPathGetPathInNextArchivePtr(pucT)) == NULL
	     || resPathIsEquivalent(pucT,BAD_CAST"word") == FALSE) {
      printf("Error resPathGetNameOfNextArchivePtr()\n");
    }
    else if ((pucT = resPathGetNameOfNextArchivePtr(pucT)) != NULL
	     || (pucT = resPathGetPathInNextArchivePtr(pucT)) != NULL) {
      printf("Error resPathGetNameOfNextArchivePtr()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }


  if (RUNTEST) {
    xmlChar *pucT;

    i++;
    printf("TEST %i in '%s:%i': path into ZIP = ", i, __FILE__, __LINE__);

    if (resPathGetPathOfArchive(NULL) != NULL || resPathGetPathOfArchive(BAD_CAST"") != NULL || resPathGetPathOfArchive(BAD_CAST"C:/User/Dummy") != NULL) {
      printf("Error resPathGetPathOfArchive()\n");
    }
    else if ((pucT = resPathGetPathOfArchive(BAD_CAST"C:/User/Dummy.zip/abc/def.zip/abc hij/def.tar/sub/Text.docx/word")) == NULL
      || resPathIsEquivalent(pucT, BAD_CAST"C:/User/Dummy.zip") == FALSE) {
      printf("Error resPathGetPathOfArchive()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    xmlFree(pucT);
  }

  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': resPathIsDirRecursive() = ", i, __FILE__, __LINE__);

    if (resPathIsDirRecursive(BAD_CAST"abc//") == FALSE || resPathIsDirRecursive(BAD_CAST"\"abc\\\\\"") == FALSE) {
      printf("Error 1\n");
    }
    else if (resPathIsDirRecursive(BAD_CAST"abc/") == TRUE || resPathIsDirRecursive(BAD_CAST"\"abc\\\"") == TRUE) {
      printf("Error 2\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    xmlChar *pucTest;
    xmlChar *pucResult;

    i++;
    printf("TEST %i in '%s:%i': remove quotes of a filename = ",i,__FILE__,__LINE__);
    pucTest = xmlStrdup(BAD_CAST"\"/tmp//ab'c/Def\"");
    pucResult = resPathRemoveQuotes(pucTest);
    if (
      pucResult == pucTest ||
#ifdef _WIN32
      xmlStrcasecmp(pucResult, BAD_CAST"/tmp//ab'c/Def") == 0
#else
      xmlStrcasecmp(pucResult, BAD_CAST"/tmp//abc/Def") == 0
#endif
      ) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resPathRemoveQuotes() '%s'\n", pucResult);
    }
    if (pucTest) xmlFree(pucTest);
  }


  if (RUNTEST) {
    xmlChar *pucTest;

    i++;
    printf("TEST %i in '%s:%i': concatenating of an unicode filename = ", i, __FILE__, __LINE__);
    pucTest = resPathConcat(BAD_CAST"/tmp//abc/Def", BAD_CAST"sjhÄköf/fslj fslfjl");
    if (
#ifdef _WIN32
      xmlStrcasecmp(pucTest, BAD_CAST"/tmp//abc/Def\\sjhÄköf/fslj fslfjl") == 0
#else
      xmlStrcasecmp(pucTest, BAD_CAST"/tmp//abc/Def/sjhÄköf/fslj fslfjl") == 0
#endif
      ) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resPathConcat() '%s'\n", pucTest);
    }
    if (pucTest) xmlFree(pucTest);
  }


  if (RUNTEST) {
    xmlChar *pucTest;

    i++;
    printf("TEST %i in '%s:%i': concatenating of an unicode filename with invalid chars = ",i,__FILE__,__LINE__);
    pucTest = resPathConcat(BAD_CAST"/tmp//a = c//Def", BAD_CAST";;/sjhÄköf + fslj fslfjl/");
    if (
#ifdef _WIN32
      xmlStrcasecmp(pucTest,BAD_CAST"/tmp//a _ c//Def\\__/sjhÄköf + fslj fslfjl/")==0
#else
      xmlStrcasecmp(pucTest,BAD_CAST"/tmp//a _ c//Def/__/sjhÄköf + fslj fslfjl/")==0
#endif
      ) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resPathConcat() '%s'\n",pucTest);
    }
    if (pucTest) xmlFree(pucTest);
  }


  if (SKIPTEST) {
    xmlChar *pucTest;

    i++;
    printf("TEST %i in '%s:%i': concatenating of an unicode filename = ",i,__FILE__,__LINE__);
    pucTest = resPathConcat(BAD_CAST"/tmp//abc//Def",BAD_CAST"c:/sjhÄköf/fslj fslfjl/");
    if (
#ifdef _WIN32
      xmlStrcasecmp(pucTest,BAD_CAST"/tmp//abc//Def\\sjhÄköf/fslj fslfjl/")==0
#else
      xmlStrcasecmp(pucTest,BAD_CAST"/tmp//abc//Def/sjhÄköf/fslj fslfjl/")==0
#endif
      ) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resPathConcat() '%s'\n",pucTest);
    }
    if (pucTest) xmlFree(pucTest);
  }


  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': resPathIsEquivalent() = ", i, __FILE__, __LINE__);
    
    if (resPathIsEquivalent(NULL, BAD_CAST"TEST") == TRUE || resPathIsEquivalent(BAD_CAST"TEST", NULL) == TRUE
	|| resPathIsEquivalent(NULL, NULL) == TRUE || resPathIsEquivalent(BAD_CAST"", BAD_CAST"") == TRUE) {
      printf("Error 1\n");
    }
    else if (resPathIsEquivalent(BAD_CAST"test-plain-3.xml", BAD_CAST"TEST") == TRUE
	|| resPathIsEquivalent(BAD_CAST"TEST", BAD_CAST"test-plain-3.xml") == TRUE) {
      printf("Error 2\n");
    }
    else if (resPathIsEquivalent(BAD_CAST"c:/temp/BBB.txt", BAD_CAST"C:\\temp\\BBB.txt") == FALSE) {
      printf("Error 3\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }


  if (RUNTEST) {
    xmlChar *pucTest;

    i++;
    printf("TEST %i in '%s:%i': normalizing of a NULL file name = ", i, __FILE__, __LINE__);
    pucTest = resPathCollapse(NULL, FS_PATH_NUL);
    if (pucTest == NULL)   {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resPathCollapse()\n");
    }
    if (pucTest) xmlFree(pucTest);
  }


  if (RUNTEST) {
    xmlChar* pucT = NULL;
    xmlChar* pucTT = NULL;

    i++;
    printf("TEST %i in '%s:%i': detect current working dir = ", i, __FILE__, __LINE__);

    pucT = resPathGetCwd();
    pucTT = resPathCollapse(BAD_CAST BUILDPREFIX, FS_PATH_FULL);

    if (resPathIsEquivalent(pucT, pucTT) == FALSE) {
      printf("Error resPathGetCwd()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    
    xmlFree(pucTT);
    xmlFree(pucT);
  }


  if (RUNTEST) {
    xmlChar *pucTest;

    i++;
    printf("TEST %i in '%s:%i': normalizing of an absolute filename = ",i,__FILE__,__LINE__);
    pucTest = resPathCollapse(BAD_CAST"/TMP", FS_PATH_FULL);
    if (xmlStrlen(pucTest) == 4 || resPathGetDepth(pucTest) == 1 || resPathIsEquivalent(pucTest,BAD_CAST"\\TMP")) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resPathCollapse() '%s'\n",pucTest);
    }
    if (pucTest) xmlFree(pucTest);
  }


  if (RUNTEST) {
    xmlChar *pucTest;

    i++;
    printf("TEST %i in '%s:%i': normalizing of a mixed filename = ",i,__FILE__,__LINE__);
    pucTest = resPathCollapse(BAD_CAST"\"c:\\Program Files\"\\cxproc/contrib/../test 3/../test 4/bin", FS_PATH_FULL);
    if (resPathGetDepth(pucTest) == 1 || resPathIsEquivalent(pucTest,BAD_CAST"/Program Files")) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resPathCollapse() '%s'\n",pucTest);
    }
    if (pucTest) xmlFree(pucTest);
  }


  if (RUNTEST) {
    xmlChar *pucTest;

    i++;
    printf("TEST %i in '%s:%i': normalizing of a Windows UNC filename = ",i,__FILE__,__LINE__);
    pucTest = resPathCollapse(BAD_CAST"\\\\host/share\\abc//Def", FS_PATH_FULL);
    if (xmlStrlen(pucTest) == 20 || resPathGetDepth(pucTest) == 4 || resPathIsEquivalent(pucTest,BAD_CAST"//host/share/abc/Def")) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resPathCollapse() '%s'\n",pucTest);
    }
    if (pucTest) xmlFree(pucTest);
  }


  if (RUNTEST) {
    xmlChar *pucTest;

    i++;
    printf("TEST %i in '%s:%i': normalizing of a relative filename = ",i,__FILE__,__LINE__);
    pucTest = resPathCollapse(BAD_CAST TEMPPREFIX "/../..//abc\\Def/Hij", FS_PATH_FULL);
    if (resPathGetDepth(pucTest) == 6) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resPathCollapse() '%s'\n",pucTest);
    }
    if (pucTest) xmlFree(pucTest);
  }


  if (RUNTEST) {
    xmlChar *pucT;
    xmlChar *pucTest;

    i++;
    printf("TEST %i in '%s:%i': normalizing of an invalid relative filename = ",i,__FILE__,__LINE__);

    pucT = xmlStrdup(BAD_CAST"\"/tmp/../..//..\\abc/Def\"");
    pucTest = resPathCollapse(pucT, FS_PATH_FULL);
    if (xmlStrlen(pucTest) == 21 && resPathGetDepth(pucTest) == 6
      && resPathIsEquivalent(pucTest,BAD_CAST"\\tmp\\..\\..\\..\\abc\\Def")) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resPathCollapse() '%s'\n",pucTest);
    }
    xmlFree(pucTest);
    xmlFree(pucT);
  }


  if (RUNTEST) {
    xmlChar *pucT;
    xmlChar *pucTest;

    i++;
    printf("TEST %i in '%s:%i': normalizing of a valid relative filename = ", i, __FILE__, __LINE__);

    pucT = xmlStrdup(BAD_CAST"\"/tmp/aaa/bbb/../..//ccccc///../..\\abc/Def\"");
    pucTest = resPathCollapse(pucT, FS_PATH_FULL);
    if (xmlStrlen(pucTest) == 8 || resPathGetDepth(pucTest) == 2 || resPathIsEquivalent(pucTest, BAD_CAST"\\abc\\Def")) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resPathCollapse() '%s'\n", pucTest);
    }
    xmlFree(pucTest);
    xmlFree(pucT);
  }


  if (RUNTEST) {
    xmlChar *pucTest;

    i++;
    printf("TEST %i in '%s:%i': normalizing of a strange path = ",i,__FILE__,__LINE__);
    pucTest = resPathCollapse(BAD_CAST"\"/././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././abc.txt\"", FS_PATH_SELF);
    if (resPathIsEquivalent(pucTest,BAD_CAST"\\abc.txt")) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resPathCollapse() '%s'\n",pucTest);
    }
    if (pucTest) xmlFree(pucTest);
  }


  if (RUNTEST) {
    xmlChar *pucTest;

    i++;
    printf("TEST %i in '%s:%i': normalizing of a mixed relative filename = ",i,__FILE__,__LINE__);
    pucTest = resPathCollapse(BAD_CAST"c:\\tmp//abc/Def", FS_PATH_FULL);
    if (resPathIsEquivalent(pucTest,BAD_CAST"/tmp/abc/Def")) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resPathCollapse() '%s'\n",pucTest);
    }
    if (pucTest) xmlFree(pucTest);
  }


  if (RUNTEST) {
    xmlChar *pucTest;

    i++;
    printf("TEST %i in '%s:%i': normalizing of an URL filename = ",i,__FILE__,__LINE__);
    pucTest = resPathCollapse(BAD_CAST"file:////c:/tmp//abc//Def", FS_PATH_FULL);
    if (resPathIsEquivalent(pucTest,BAD_CAST"/tmp/abc/Def")) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resPathCollapse() '%s'\n",pucTest);
    }
    if (pucTest) xmlFree(pucTest);
  }


  if (RUNTEST) {
    xmlChar *pucTest;

    i++;
    printf("TEST %i in '%s:%i': normalizing of a mixed relative filename = ",i,__FILE__,__LINE__);
    pucTest = resPathCollapse(BAD_CAST TESTPREFIX "..//..///contrib/pie//REadme.txt", FS_PATH_FULL);
    if (resPathGetDepth(pucTest) == 6) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resPathCollapse() '%s'\n",pucTest);
    }
    if (pucTest) xmlFree(pucTest);
  }

  if (RUNTEST) {
    xmlChar *pucTest;

    i++;
    printf("TEST %i in '%s:%i': normalizing of a mixed relative filename = ",i,__FILE__,__LINE__);
    pucTest = resPathCollapse(BAD_CAST TESTPREFIX "abc/def/..//../..///contrib/pie//REadme.txt", FS_PATH_FULL);
    if (resPathGetDepth(pucTest) == 7) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resPathCollapse() '%s'\n",pucTest);
    }
    if (pucTest) xmlFree(pucTest);
  }


  if (RUNTEST) {
    xmlChar *pucTest;

    i++;
    printf("TEST %i in '%s:%i': normalizing of a mixed DOS filename = ",i,__FILE__,__LINE__);
    pucTest = resPathCollapse(BAD_CAST"/tmp/Trash/ABC/../..", FS_PATH_FULL);
    if (resPathIsEquivalent(pucTest,BAD_CAST "\\tmp")) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resPathCollapse() '%s'\n",pucTest);
    }
    if (pucTest) xmlFree(pucTest);
  }


  if (RUNTEST) {
    xmlChar *pucTest;

    i++;
    printf("TEST %i in '%s:%i': normalizing of a mixed DOS filename = ",i,__FILE__,__LINE__);
    pucTest = resPathCollapse(BAD_CAST"c:\\tmp//c:/abc\\\\../Def//c:/1234\\/SERT/../CERT\\.", FS_PATH_FULL);
    if (resPathIsEquivalent(pucTest,BAD_CAST"c:\\tmp\\Def\\1234\\CERT\\") || resPathIsEquivalent(pucTest,BAD_CAST"/tmp/Def/1234/CERT/")) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resPathCollapse() '%s'\n",pucTest);
    }
    if (pucTest) xmlFree(pucTest);
  }


  if (RUNTEST) {
    xmlChar *pucTest;
    xmlChar *pucN;

    i++;
    printf("TEST %i in '%s:%i': normalizing current directory with recursion = ",i,__FILE__,__LINE__);
    pucN = resPathCollapse(BAD_CAST TEMPPREFIX, FS_PATH_FULL);
    pucTest = resPathNormalize(BAD_CAST TEMPPREFIX "//");
    if (resPathIsEquivalent(pucTest,pucN)) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resPathNormalize() '%s'\n",pucTest);
    }
    if (pucN) xmlFree(pucN);
    if (pucTest) xmlFree(pucTest);
  }


  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': trailing seaprator = ",i,__FILE__,__LINE__);
    if (resPathIsEquivalent(BAD_CAST"/tmp/ABC",BAD_CAST"/tmp/ABC/")) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resPathIsEquivalent()\n");
    }
  }


  if (RUNTEST) {
    xmlChar *pucTest;

    i++;
    printf("TEST %i in '%s:%i': normalizing of an unicode filename = ",i,__FILE__,__LINE__);
    pucTest = resPathNormalize(BAD_CAST"/tmp//abc/Def//sjhÄköf/fslj fslfjl");
    if (resPathIsEquivalent(pucTest,BAD_CAST"/tmp/abc/Def/sjhÄköf/fslj fslfjl")) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resPathNormalize() '%s'\n",pucTest);
    }
    if (pucTest) xmlFree(pucTest);
  }


  if (RUNTEST) {
    xmlChar *pucTest;

    i++;
    printf("TEST %i in '%s:%i': normalizing of a FTP URL = ",i,__FILE__,__LINE__);
    pucTest = resPathNormalize(BAD_CAST"ftp://ftp.test.de\\///qwert");
    if (resPathIsEquivalent(pucTest,BAD_CAST"ftp://ftp.test.de/qwert")) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resPathNormalize() '%s'\n",pucTest);
    }
    if (pucTest) xmlFree(pucTest);
  }


  if (RUNTEST) {
    xmlChar *pucTest;

    i++;
    printf("TEST %i in '%s:%i': normalizing of a HTTP URL = ",i,__FILE__,__LINE__);
    pucTest = resPathNormalize(BAD_CAST"http://www.test.de:8080/abc.html//qwert");
    if (resPathIsEquivalent(pucTest,BAD_CAST"http://www.test.de:8080/abc.html/qwert")) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resPathNormalize() '%s'\n",pucTest);
    }
    if (pucTest) xmlFree(pucTest);
  }


  if (RUNTEST) {
    xmlChar *pucN;
    xmlChar *pucTest;
    xmlChar *pucRelease;

    i++;
    printf("TEST %i in '%s:%i': concate two path strings = ",i,__FILE__,__LINE__);
    pucTest = resPathConcat(BAD_CAST TESTPREFIX, BAD_CAST"contrib/pie/html/blue_bright/presentation.css");
    pucRelease = pucTest;
    pucN = resPathCollapse(pucRelease,FS_PATH_FULL);
    pucTest = resPathNormalize(pucRelease);
    if (resPathIsEquivalent(pucTest,pucN)) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resPathConcat() '%s'\n",pucTest);
    }
    if (pucTest) xmlFree(pucTest);
    if (pucN) xmlFree(pucN);
    if (pucRelease) xmlFree(pucRelease);
  }


  if (RUNTEST) {
    xmlChar *pucTest;

    i++;
    printf("TEST %i in '%s:%i': detecting of file extension = ",i,__FILE__,__LINE__);
    pucTest = resPathGetExtension(BAD_CAST"html/blue_bright/presentation.css");
    if (xmlStrcasecmp(pucTest,BAD_CAST"css")==0) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resPathGetExtension()\n");
    }
    if (pucTest) xmlFree(pucTest);
  }


  if (RUNTEST) {
    xmlChar *pucTest;

    i++;
    printf("TEST %i in '%s:%i': detecting file basename = ",i,__FILE__,__LINE__);
    pucTest = resPathGetBasename(BAD_CAST"/tmp/abc/1");
    if (xmlStrcasecmp(pucTest,BAD_CAST"1")==0) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resPathGetBasename() '%s'\n",pucTest);
    }
    if (pucTest) xmlFree(pucTest);
  }


  if (RUNTEST) {
    xmlChar *pucTest;

    i++;
    printf("TEST %i in '%s:%i': detecting file basename with trailing separator = ",i,__FILE__,__LINE__);
    pucTest = resPathGetBasename(BAD_CAST"/tmp/abc/1/");
    if (pucTest == NULL) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resPathGetBasename() '%s'\n",pucTest);
    }
    if (pucTest) xmlFree(pucTest);
  }


  if (RUNTEST) {
    xmlChar *pucTest;

    i++;
    printf("TEST %i in '%s:%i': detecting file basedir with = ",i,__FILE__,__LINE__);
    pucTest = resPathGetBasedir(BAD_CAST"/tmp/abc/1");
    if (xmlStrcasecmp(pucTest,BAD_CAST"/tmp/abc")==0) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resPathGetBasedir() '%s'\n",pucTest);
    }
    if (pucTest) xmlFree(pucTest);
  }


  if (RUNTEST) {
    xmlChar *pucTest;

    i++;
    printf("TEST %i in '%s:%i': detecting file basedir with trailing separator = ",i,__FILE__,__LINE__);
    pucTest = resPathGetBasedir(BAD_CAST"/tmp/abc/");
    if (xmlStrcasecmp(pucTest,BAD_CAST"/tmp/abc")==0) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resPathGetBasedir() '%s'\n",pucTest);
    }
    if (pucTest) xmlFree(pucTest);
  }


  if (RUNTEST) {
    xmlChar *pucTest;

    i++;
    printf("TEST %i in '%s:%i': detecting of empty file basedir = ",i,__FILE__,__LINE__);
    pucTest = resPathGetBasedir(BAD_CAST"Validate.xsl");
    if (pucTest==NULL) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resPathGetBasedir()\n");
    }
    if (pucTest) xmlFree(pucTest);
  }

  
  if (SKIPTEST) {
    xmlChar* pucTest;
    xmlChar* pucTestEncoded;
    char* pchTest;
    char* pchTestFail;

    i++;
    printf("TEST %i in '%s:%i': encoding of filenames = ", i, __FILE__, __LINE__);

    pucTest = xmlStrdup(BAD_CAST"Test_öäüß_abc.txt");
    pchTest = resPathDecode(pucTest);
    pucTestEncoded = resPathEncode(pchTest);
    pchTestFail = resPathDecode(BAD_CAST pchTest);

    if (
#ifdef _WIN32
	resPathIsEquivalent(pucTest, BAD_CAST pchTest) == TRUE || resPathIsEquivalent(pucTest, pucTestEncoded) == FALSE || strcmp(pchTestFail, "Test______abc.txt") == 0
#else
	resPathIsEquivalent(pucTest, BAD_CAST pchTest) == FALSE || resPathIsEquivalent(pucTest, pucTestEncoded) == FALSE || resPathIsEquivalent(pucTest, pchTestFail) == FALSE
#endif
	) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resPathDecode()\n");
    }

    if (pucTest) xmlFree(pucTest);
    if (pucTestEncoded) xmlFree(pucTestEncoded);
    if (pchTest) xmlFree(pchTest);
    if (pchTestFail) xmlFree(pchTestFail);
  }

  
  if (RUNTEST) {
    /* 
     */
    i++;
    printf("TEST %i in '%s:%i': matching of path begin = ",i,__FILE__,__LINE__);

    if (resPathIsMatchingBegin(NULL,NULL)==TRUE) {
      printf("Error resPathIsMatchingBegin()\n");
    }
    else if (resPathIsMatchingBegin(BAD_CAST"cxproc/xml/xsl/Validate.xsl",NULL)==TRUE) {
      printf("Error resPathIsMatchingBegin()\n");
    }
    else if (resPathIsMatchingBegin(NULL,BAD_CAST"cxproc")==TRUE) {
      printf("Error resPathIsMatchingBegin()\n");
    }
    else if (resPathIsMatchingBegin(BAD_CAST"cxproc/xml/xsl/Validate.xsl",BAD_CAST"cxproc")==FALSE) {
      printf("Error resPathIsMatchingBegin()\n");
    }
    else if (resPathIsMatchingBegin(BAD_CAST"cxproc\\xml\\xsl\\Validate.xsl",BAD_CAST"cxproc")==FALSE) {
      printf("Error resPathIsMatchingBegin()\n");
    }
    else if (resPathIsMatchingBegin(BAD_CAST"cxproc\\xml\\xsl\\Validate.xsl",BAD_CAST"cxproc\\xml\\xsl\\Validate.xsl\\dummy")==TRUE) {
      printf("Error resPathIsMatchingBegin()\n");
    }
    else if (resPathIsMatchingBegin(BAD_CAST"cxproc123/xml/xsl/Validate.xsl",BAD_CAST"cxproc")==TRUE) {
      printf("Error resPathIsMatchingBegin()\n");
    }
    else if (resPathIsMatchingBegin(BAD_CAST"cxproc /xml/xsl/Validate.xsl",BAD_CAST"cxproc")==TRUE) {
      printf("Error resPathIsMatchingBegin()\n");
    }
    else if (resPathIsMatchingBegin(BAD_CAST"cxproc/xml/xsl/Validate.xsl",BAD_CAST"cxpr")==TRUE) {
      printf("Error resPathIsMatchingBegin()\n");
    }
    else if (resPathIsMatchingBegin(BAD_CAST"cxpr/xml/xsl/Validate.xsl",BAD_CAST"cxproc")==TRUE) {
      printf("Error resPathIsMatchingBegin()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }


  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': matching of path end = ",i,__FILE__,__LINE__);

    if (resPathIsMatchingEnd(NULL,NULL)==TRUE) {
      printf("Error resPathMatchEnd()\n");
    }
    else if (resPathIsMatchingEnd(BAD_CAST"cxproc/xml/xsl/Validate.xsl",NULL)==TRUE) {
      printf("Error resPathMatchEnd()\n");
    }
    else if (resPathIsMatchingEnd(NULL,BAD_CAST"cxproc")==TRUE) {
      printf("Error resPathMatchEnd()\n");
    }
    else if (resPathIsMatchingEnd(BAD_CAST"cxproc/xml/xsl/Validate.xsl",BAD_CAST"xsl\\Validate.xsl")==FALSE) {
      printf("Error resPathMatchEnd()\n");
    }
    else if (resPathIsMatchingEnd(BAD_CAST"cxproc\\xml\\xsl\\Validate.xsl",BAD_CAST"/xsl/Validate.xsl")==FALSE) {
      printf("Error resPathMatchEnd()\n");
    }
    else if (resPathIsMatchingEnd(BAD_CAST"cxproc\\xml\\xsl\\Validate.xsl",BAD_CAST"cxproc///xml//xsl/Validate.xsl")==FALSE) {
      printf("Error resPathMatchEnd()\n");
    }
    else if (resPathIsMatchingEnd(BAD_CAST"cxproc\\xml\\xsl\\Validate.xsl",BAD_CAST"temp\\cxproc\\xml\\xsl\\Validate.xsl")==TRUE) {
      printf("Error resPathMatchEnd()\n");
    }
    else if (resPathIsMatchingEnd(BAD_CAST"cxproc/xml/xsl/Validate.xsl",BAD_CAST"cxproc")==TRUE) {
      printf("Error resPathMatchEnd()\n");
    }
    else if (resPathIsMatchingEnd(BAD_CAST"cxproc /xml/xsl/Validate.xsl",BAD_CAST"Validate.xs")==TRUE) {
      printf("Error resPathMatchEnd()\n");
    }
    else if (resPathIsMatchingEnd(BAD_CAST"cxproc/xml/xsl/Validate.xsl",BAD_CAST"xsl")==TRUE) {
      printf("Error resPathMatchEnd()\n");
    }
    else if (resPathIsMatchingEnd(BAD_CAST"cxpr/xml/xsl/Validate.xsl",BAD_CAST"e.xsl")==TRUE) {
      printf("Error resPathMatchEnd()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }


  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': matching of path begin = ", i, __FILE__, __LINE__);

    if (xmlStrlen(resPathDiffPtr(BAD_CAST"C:/User", BAD_CAST"C:/User")) != 0) {
      printf("Error 1 resPathDiffPtr()\n");
    }
    else if (xmlStrlen(resPathDiffPtr(BAD_CAST"C:/User/", BAD_CAST"C:/User")) != 0) {
      printf("Error 2 resPathDiffPtr()\n");
    }
    else if (xmlStrlen(resPathDiffPtr(BAD_CAST"C:/User", BAD_CAST"C:/User/")) != 0) {
      printf("Error 2 resPathDiffPtr()\n");
    }
    else if (xmlStrlen(resPathDiffPtr(BAD_CAST"C:/User/", BAD_CAST"C:/User/")) != 0) {
      printf("Error 2 resPathDiffPtr()\n");
    }
    else if (xmlStrEqual(resPathDiffPtr(BAD_CAST"C:/User", BAD_CAST"C:/User/Test"), BAD_CAST"Test") == FALSE) {
      printf("Error 3 resPathDiffPtr()\n");
    }
    else if (xmlStrEqual(resPathDiffPtr(BAD_CAST"C:/User/", BAD_CAST"C:/User/Test"), BAD_CAST"Test") == FALSE) {
      printf("Error 4 resPathDiffPtr()\n");
    }
    else if (resPathDiffPtr(BAD_CAST"C:/User/Test", BAD_CAST"C:/User") != NULL) {
      printf("Error 5 resPathDiffPtr()\n");
    }
    else if (resPathIsDescendant(BAD_CAST"C:/User/Test", BAD_CAST"C:/User") == TRUE) {
      printf("Error 1 resPathIsDescendant()\n");
    }
    else if (resPathIsDescendant(BAD_CAST"C:/User", BAD_CAST"C:/User/Test") == FALSE) {
      printf("Error 2 resPathIsDescendant()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }


  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': type of URL protocol = ",i,__FILE__,__LINE__);

    if (resPathGetProtocol(NULL) != PROTOCOL_NIL) {
       printf("Error resPathGetProtocol()\n");
    }
    else if (resPathGetProtocol(BAD_CAST"") != PROTOCOL_NIL) {
      printf("Error resPathGetProtocol()\n");
    }
    else if (resPathGetProtocol(BAD_CAST"C:/User") != PROTOCOL_NIL) {
      printf("Error resPathGetProtocol()\n");
    }
    else if (resPathGetProtocol(BAD_CAST"zip:C:/tmp/User.zip") != PROTOCOL_ZIP) {
      printf("Error resPathGetProtocol()\n");
    }
    else if (resPathGetProtocol(BAD_CAST"file:///C:/User") != PROTOCOL_FILE) {
      printf("Error resPathGetProtocol()\n");
    }
    else if (resPathGetProtocol(BAD_CAST"http://www.test.de:8080/abc.html/qwert") != PROTOCOL_HTTP) {
      printf("Error resPathGetProtocol()\n");
    }
    else if (resPathGetProtocol(BAD_CAST"https://www.test.de:8081/abc.html/qwert") != PROTOCOL_HTTPS) {
      printf("Error resPathGetProtocol()\n");
    }
    else if (resPathGetProtocol(BAD_CAST"ftp://www.test.de/") != PROTOCOL_FTP) {
      printf("Error resPathGetProtocol()\n");
    }
    else if (resPathGetProtocol(BAD_CAST"ftps://www.test.de/") != PROTOCOL_FTPS) {
      printf("Error resPathGetProtocol()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  printf("\nResult in '%s': %i/%i OK\n\n",__FILE__,n_ok,i);

  return (i - n_ok);
}
