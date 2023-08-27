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


/*! test code for contexts in module fs
 */
int
resNodeTestList(void)
{
  int n_ok;
  int i;

  n_ok=0;
  i=0;


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    resNodePtr prnDir = NULL;
    resNodePtr prnDirSub = NULL;
    resNodePtr prnDirSubSub = NULL;
    resNodePtr prnFile = NULL;

    i++;
    printf("TEST %i in '%s:%i': new filesystem context  = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX)) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if ((prnDir = resNodeAddChildNew(prnT, BAD_CAST"Documents/")) == NULL || resNodeGetType(prnDir) != rn_type_dir) {
      printf("Error resNodeAddChildNew()\n");
    }
    else if (prnDir != resNodeGetChild(prnT) || prnT != resNodeGetParent(prnDir)) {
      printf("Error resNodeAddChildNew()\n");
    }
    else if ((prnFile = resNodeAddChildNew(prnT, BAD_CAST"Documents/DummyContent.xml")) == NULL || resNodeGetType(prnFile) != rn_type_file) {
      printf("Error resNodeAddChildNew()\n");
    }
    else if (prnFile != resNodeGetChild(prnDir)) {
      printf("Error resNodeAddChildNew()\n");
    }
    else if ((prnFile = resNodeAddChildNew(prnT, BAD_CAST"Documents/TestContent.xml")) == NULL || resNodeGetType(prnFile) != rn_type_file) {
      printf("Error resNodeAddChildNew()\n");
    }
    else if (prnFile != resNodeGetNext(resNodeGetChild(prnDir))) {
      printf("Error resNodeAddChildNew()\n");
    }
    else if (prnFile != resNodeListFindPath(prnDir, BAD_CAST"Documents/TestContent.xml", (RN_FIND_FILE | RN_FIND_IN_SUBDIR))) {
      printf("Error resNodeListFindPath()\n");
    }
    else if ((prnDirSub = resNodeAddChildNew(prnT, BAD_CAST"Documents/SubDummy/")) == NULL || resNodeGetType(prnDirSub) != rn_type_dir) {
      printf("Error resNodeAddChildNew()\n");
    }
    else if (prnDir != resNodeGetParent(prnDirSub)) {
      printf("Error resNodeAddChildNew()\n");
    }
    else if ((prnDirSubSub = resNodeAddChildNew(prnT, BAD_CAST"Documents/SubDummy/SubDummy/")) == NULL || resNodeGetType(prnDirSubSub) != rn_type_dir) {
      printf("Error resNodeAddChildNew()\n");
    }
    else if (prnDir != resNodeGetParent(resNodeGetParent(prnDirSubSub))) {
      printf("Error resNodeAddChildNew()\n");
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

    i++;
    printf("TEST %i in '%s:%i': find a file in res node tree  = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX)) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if ((prnFile = resNodeListFindPath(prnT, BAD_CAST"config-test.cxp", (RN_FIND_FILE | RN_FIND_IN_SUBDIR))) == NULL) {
      printf("Error 1 resNodeListFindPath()\n");
    }
    else if ((prnFile = resNodeListFindPath(prnT, BAD_CAST"pn-1.pie", (RN_FIND_FILE | RN_FIND_IN_SUBDIR))) == NULL) {
      printf("Error 2 resNodeListFindPath()\n");
    }
    else if ((prnFile = resNodeListFindPath(prnT, BAD_CAST"rp/", (RN_FIND_DIR | RN_FIND_IN_SUBDIR))) == NULL) {
      printf("Error 3 resNodeListFindPath()\n");
    }
    else if ((prnFile = resNodeListFindPath(prnT, BAD_CAST"non/existing.file", (RN_FIND_FILE | RN_FIND_IN_SUBDIR))) != NULL) {
      printf("Error 4 resNodeListFindPath()\n");
    }
#ifdef HAVE_LIBARCHIVE
    else if ((prnFile = resNodeListFindPath(prnT, BAD_CAST"sub/a.txt", RN_FIND_FILE | RN_FIND_IN_ARCHIVE)) == NULL) {
      printf("Error 1 resNodeListFindPath() inside archive\n");
    }
    else if ((prnFile = resNodeListFindPath(prnFile, BAD_CAST"a.txt", RN_FIND_FILE | RN_FIND_IN_ARCHIVE)) == NULL) {
      printf("Error 2 resNodeListFindPath() inside archive\n");
    }
#endif
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    size_t j;
    resNodePtr prnT = NULL;               /* haystack */
    resNodePtr prnNeedleFile = NULL;      /* needle */
    resNodePtr prnNeedleInArchive = NULL; /* needle */
    resNodePtr prnNeedleDir = NULL;       /* needle */
    resNodePtr prnFound = NULL;
    resNodePtr prnTT = NULL;
    resNodePtr prnTTT = NULL;

    i++;
    printf("TEST %i in '%s:%i': build non-existing filesystem contexts and search for context = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "Virtual/")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeAddChildNew(prnT, BAD_CAST"Settings/") == NULL) {
      printf("Error resNodeAddChildNew() ...\n");
    }
    else if (resNodeAddChildNew(prnT, BAD_CAST"Notes.txt") == NULL) {
      printf("Error resNodeAddChildNew() ...\n");
    }
    else if ((prnTT = resNodeAddChildNew(prnT, BAD_CAST"Images/")) == NULL) {
      printf("Error resNodeAddChildNew() ...\n");
    }
    else if (resNodeAddChildNew(prnT, BAD_CAST"Images/0987654321.JPG") == NULL) {
      printf("Error resNodeAddChildNew() ...\n");
    }
    else if (resNodeAddChildNew(prnTT, BAD_CAST"1234567890.JPG") == NULL) {
      printf("Error resNodeAddChildNew() ...\n");
    }
    else if (resNodeAddChildNew(prnTT, BAD_CAST"Sub/1234567891.JPG") == NULL) {
      printf("Error resNodeAddChildNew() ...\n");
    }
    else if (resNodeAddChildNew(prnT, BAD_CAST"Settings/default.ini") == NULL) {
      printf("Error resNodeAddChildNew() ...\n");
    }
    else if ((prnNeedleFile = resNodeAddChildNew(prnT, BAD_CAST"Dummy/Dummy/ABC.txt")) == NULL) {
      printf("Error resNodeAddChildNew() ...\n");
    }
    else if (resNodeAddChildNew(prnT, BAD_CAST"Dummy/Dummy//Dummy\\Dummy\\ABC.txt") == NULL) {
      printf("Error resNodeAddChildNew() ...\n");
    }
    else if (resNodeAddChildNew(prnT, BAD_CAST"Work.docx") == NULL) {
      printf("Error resNodeAddChildNew() ...\n");
    }
    else if ((prnTT = resNodeAddChildNew(prnT, BAD_CAST"Backups/2019-04-22-Home.zip")) == NULL) {
      printf("Error resNodeAddChildNew() ...\n");
    }
    else if ((prnTTT = resNodeAddChildNew(prnTT, BAD_CAST"Images/0987654321.JPG")) == NULL) {
      printf("Error resNodeAddChildNew() ...\n");
    }
    else if (resNodeAddChild(prnT, resNodeCommentNew(prnTTT)) == NULL) {
      printf("Error resNodeCommentNew() ...\n");
    }
    else if (resNodeAddChildNew(prnTT, BAD_CAST"Settings/default.ini") == NULL) {
      printf("Error resNodeAddChildNew() ...\n");
    }
    else if ((prnNeedleInArchive = resNodeAddChildNew(prnTT, BAD_CAST"Settings/key.dat")) == NULL) {
      printf("Error resNodeAddChildNew() ...\n");
    }
    else if ((prnNeedleDir = resNodeAddChildNew(prnT, BAD_CAST"Audio/Podcasts/Blah Blah/")) == NULL) {
      printf("Error resNodeAddChildNew() ...\n");
    }
    else if (resNodeAddChildNew(prnT, BAD_CAST"Fun/") == NULL) {
      printf("Error resNodeAddChildNew() ...\n");
    }
    else if ((j = resNodeGetChildCount(prnT, rn_type_dir)) != 6 || (j = resNodeGetChildCount(prnT, rn_type_file)) != 1) {
      printf("Error resNodeDirAppendEntries() = %i\n", j);
    }
    else if (resNodeListFindChild(prnT, BAD_CAST"Notes.txt") != resNodeGetNext(resNodeGetChild(prnT))) {
      printf("Error resNodeListFindChild() ...\n");
    }
    else if ((prnFound = resNodeListFindPath(prnT, BAD_CAST"Work.docx", (RN_FIND_FILE | RN_FIND_IN_SUBDIR | RN_FIND_IN_ARCHIVE))) == NULL) {
      printf("Error 1 resNodeListFindPath() ...\n");
    }
    else if (resNodeListFindPath(prnT, BAD_CAST"Audio/", (RN_FIND_DIR | RN_FIND_IN_SUBDIR | RN_FIND_IN_ARCHIVE)) == NULL) {
      printf("Error 2 resNodeListFindPath() ...\n");
    }
    else if (resNodeListFindPath(prnT, BAD_CAST"Dummy/ABC.txt", (RN_FIND_FILE | RN_FIND_IN_SUBDIR | RN_FIND_IN_ARCHIVE)) != prnNeedleFile) {
      printf("Error 3 resNodeListFindPath() ...\n");
    }
    else if (resNodeListFindPath(prnT, BAD_CAST"key.dat", (RN_FIND_FILE | RN_FIND_IN_SUBDIR | RN_FIND_IN_ARCHIVE)) != prnNeedleInArchive) {
      printf("Error 4 resNodeListFindPath() ...\n");
    }
    else if (resNodeListFindPath(prnT, BAD_CAST"Podcasts/Blah Blah", (RN_FIND_DIR | RN_FIND_IN_SUBDIR | RN_FIND_IN_ARCHIVE)) != prnNeedleDir) {
      printf("Error 5 resNodeListFindPath() ...\n");
    }
    else if (resNodeListFindPath(prnT, BAD_CAST"^.*567891\\.jpg", (RN_FIND_FILE | RN_FIND_IN_SUBDIR | RN_FIND_REGEXP)) == NULL) {
      printf("Error 5b resNodeListFindPath() ...\n");
    }
    else if ((prnFound = resNodeListFindPath(prnT, BAD_CAST"Dummy/", (RN_FIND_DIR | RN_FIND_IN_SUBDIR))) == NULL) {
      printf("Error 6 resNodeListFindPath() ...\n");
    }
    else if ((prnFound = resNodeListFindPath(prnT, BAD_CAST"Dummy/Dummy/Dummy/", RN_FIND_ALL)) == NULL) {
      printf("Error 7 resNodeListFindPath() ...\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    //puts((const char*)resNodeListToXml(prnT,RN_INFO_MIN));
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    resNodePtr prnU = NULL;

    i++;
    printf("TEST %i in '%s:%i': unlink in non-existing contexts = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX)) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeListUnlink(NULL) == TRUE) {
      printf("Error '%s' ...\n", resNodeGetErrorMsg(prnT));
    }
    else if (resNodeAddChildNew(prnT, BAD_CAST"Virtual/ABC/Settings/") == NULL) {
      printf("Error '%s' ...\n", resNodeGetErrorMsg(prnT));
    }
    else if ((prnU = resNodeAddChildNew(prnT, BAD_CAST"Settings/abc.txt")) == NULL) {
      printf("Error '%s' ...\n", resNodeGetErrorMsg(prnT));
    }
    else if (resNodeListUnlink(prnU) != TRUE) {
      printf("Error '%s' ...\n", resNodeGetErrorMsg(prnT));
    }
    else if (resNodeListUnlink(prnU) == FALSE) {
      printf("Error '%s' ...\n", resNodeGetErrorMsg(prnT));
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    //puts((const char*)resNodeListToXml(prnT,RN_INFO_MIN));
    resNodeFree(prnU);
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': build non-existing relative contexts = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX)) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeAddChildNew(prnT, BAD_CAST"Virtual/ABC/../../Settings/") == NULL) {
      printf("Error '%s' ...\n", resNodeGetErrorMsg(prnT));
    }
    else if (resNodeAddChildNew(prnT, BAD_CAST"Settings/abc.txt") == NULL) {
      printf("Error '%s' ...\n", resNodeGetErrorMsg(prnT));
    }
    else if (resNodeAddChildNew(prnT, BAD_CAST"../../../Settings/") != NULL) {
      printf("Error '%s' ...\n", resNodeGetErrorMsg(prnT));
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    //puts((const char*)resNodeListToXml(prnT,RN_INFO_MIN));
    resNodeFree(prnT);
  }


#ifdef HAVE_LIBARCHIVE
  if (RUNTEST) {
    size_t j;
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': build non-existing nested archive contexts = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "option/")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeAddChildNew(prnT, BAD_CAST"archive/test-zip-7.zip/sub/plain.txt") == NULL) {
      printf("Error resNodeAddChildNew() ...\n");
    }
    else if (resNodeAddChildNew(prnT, BAD_CAST"archive/test-zip-7.zip/TestContent.odt/content.xml") == NULL) {
      printf("Error resNodeAddChildNew() ...\n");
    }
    else if ((j = resNodeGetChildCount(prnT, rn_type_archive)) != 1
	     || (j = resNodeGetChildCount(resNodeGetChild(prnT), rn_type_dir_in_archive)) != 1
	     || (j = resNodeGetChildCount(resNodeGetChild(resNodeGetChild(prnT)), rn_type_file_in_archive)) != 2) {
      printf("Error resNodeAddChildNew() = %i\n", j);
    }
    else if (resNodeSetRecursion(prnT,TRUE) == FALSE
	     || resNodeUpdate(prnT, RN_INFO_MAX, NULL, NULL) == FALSE
	     || resNodeGetError(prnT) != rn_error_stat) {
      printf("Error resNodeUpdate() '%s' ...\n", resNodeGetErrorMsg(prnT));
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    //puts((const char*)resNodeListToXml(prnT,RN_INFO_MAX));
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    size_t j;
    resNodePtr prnT = NULL;
    resNodePtr prnTT = NULL;

    i++;
    printf("TEST %i in '%s:%i': handle a compressed file context = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX)) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeAddChildNew(prnT, BAD_CAST "option\\archive\\test-arc.txt.gz") == NULL) {
      printf("Error resNodeAddChildNew() ...\n");
    }
    else if (resNodeSetRecursion(prnT, TRUE) == FALSE || resNodeUpdate(prnT, RN_INFO_MAX, NULL, NULL) == FALSE
	     || resNodeGetType(prnT) != rn_type_dir || resNodeGetError(prnT) != rn_error_none) {
      printf("Error resNodeUpdate() '%s' ...\n", resNodeGetErrorMsg(prnT));
    }
    else if ((prnTT = resNodeGetChild(prnT)) == NULL || resNodeGetType(prnTT) != rn_type_dir) {
      printf("Error resNodeUpdate() 1\n");
    }
    else if ((prnTT = resNodeGetChild(prnTT)) == NULL || resNodeGetType(prnTT) != rn_type_dir) {
      printf("Error resNodeUpdate() 2\n");
    }
    else if ((prnTT = resNodeGetChild(prnTT)) == NULL || resNodeGetType(prnTT) != rn_type_file_compressed || resNodeGetMimeType(prnTT) != MIME_APPLICATION_GZIP) {
      printf("Error resNodeUpdate() 3\n");
    }
    else if ((prnTT = resNodeGetChild(prnTT)) == NULL || resNodeGetType(prnTT) != rn_type_file_in_archive || resNodeGetMimeType(prnTT) != MIME_TEXT_PLAIN) {
      printf("Error resNodeUpdate() 4\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    //puts((const char*)resNodeListToXml(prnT,RN_INFO_MAX));
    resNodeFree(prnT);
  }
#endif

  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': parse filesystem context = ", i, __FILE__, __LINE__);

    if (resNodeListParse(NULL, 999, NULL) == TRUE) {
      printf("Error resNodeListParse() ...\n");
    }
    else if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "each\\config.cxp")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeListParse(prnT, -3, NULL) == TRUE) {
      printf("Error resNodeListParse() ...\n");
    }
    else if (resNodeListParse(prnT, 3, NULL) == FALSE) {
      printf("Error resNodeListParse() ...\n");
    }
    else if (resNodeGetChild(prnT) != NULL || resNodeGetNext(prnT) != NULL || resNodeGetParent(prnT) != NULL) {
      printf("Error resNodeDirAppendEntries()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    //puts((const char*)resNodeListToXml(prnT,RN_INFO_MAX));
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    size_t j;
    resNodePtr prnT = NULL;
    resNodePtr prnFound = NULL;

    i++;
    printf("TEST %i in '%s:%i': parse filesystem context recursively and search for context = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX)) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeListParse(prnT, 999, NULL) == FALSE) {
      printf("Error resNodeListParse() ...\n");
    }
    else if ((j = resNodeGetChildCount(prnT, rn_type_dir)) != 12) {
      printf("Error resNodeDirAppendEntries() = %i\n", j);
    }
    else if ((prnFound = resNodeListFindPath(prnT, BAD_CAST"dir/", (RN_FIND_DIR | RN_FIND_IN_SUBDIR))) == NULL) {
      printf("Error resNodeListFindPath() ...\n");
    }
    else if ((prnFound = resNodeListFindPath(prnT, BAD_CAST TESTPREFIX "dir/config.cxp", (RN_FIND_FILE | RN_FIND_IN_SUBDIR))) == NULL) {
      printf("Error resNodeListFindPath() ...\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    //puts((const char*)resNodeListToXml(prnT,RN_INFO_MAX));
    resNodeFree(prnT);
  }


  if (SKIPTEST) {
    size_t j;
    resNodePtr prnT = NULL;
    resNodePtr prnTarget = NULL;

    i++;
    printf("TEST %i in '%s:%i': parse filesystem context of a symbolic link = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "CAD Link.lnk")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeListParse(prnT, 9, NULL) == FALSE) {
      printf("Error resNodeListParse() ...\n");
    }
    else if ((prnTarget = resNodeGetChild(prnT)) == NULL || resNodeListParse(prnTarget, 1, NULL) == FALSE) {
      printf("Error resNodeListParse() ...\n");
    }
    else if (resNodeGetType(prnT) != rn_type_symlink || resNodeGetType(prnTarget) != rn_type_dir) {
      printf("Error resNodeListParse() ...\n");
    }
    else if ((j = resNodeGetChildCount(prnTarget, rn_type_dir)) != 0) {
      printf("Error resNodeDirAppendEntries() = %i\n", j);
    }
    else if ((j = resNodeGetChildCount(prnTarget, rn_type_file)) != 9) {
      printf("Error resNodeDirAppendEntries() = %i\n", j);
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    //puts((const char*)resNodeListToXml(prnT,RN_INFO_MAX));
    resNodeFree(prnT);
  }


#ifdef HAVE_LIBARCHIVE
  if (RUNTEST) {
    size_t j;
    resNodePtr prnT = NULL;
    resNodePtr prnTT = NULL;
    resNodePtr prnFound = NULL;
    xmlChar *pucArgFind = BAD_CAST"META-INF";
    
    i++;
    printf("TEST %i in '%s:%i': find iterator = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "xml")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if ((prnFound = resNodeListFindPathNext(prnFound, NULL, 0)) != NULL) {
      printf("Error 1 resNodeListFindPathNext()\n");
    }
    else if ((prnFound = resNodeListFindPathNext(prnFound, BAD_CAST "", RN_FIND_FILE)) != NULL) {
      printf("Error 2 resNodeListFindPathNext()\n");
    }
    else if ((prnFound = resNodeListFindPath(prnT, pucArgFind, (RN_FIND_FILE | RN_FIND_IN_ARCHIVE))) == NULL) {
      printf("Error 3 resNodeListFindPath()\n");
    }
    else if ((prnTT = prnFound) == NULL || (prnFound = resNodeListFindPathNext(prnFound, pucArgFind, RN_FIND_FILE)) == NULL || (prnTT == prnFound)) {
      printf("Error 4 resNodeListFindPathNext()\n");
    }
    else if ((prnTT = prnFound) == NULL || (prnFound = resNodeListFindPathNext(prnFound, pucArgFind, RN_FIND_FILE)) == NULL || (prnTT == prnFound)) {
      printf("Error 5 resNodeListFindPathNext()\n");
    }
    else if ((prnTT = prnFound) == NULL || (prnFound = resNodeListFindPathNext(prnFound, pucArgFind, RN_FIND_FILE)) == NULL || (prnTT == prnFound)) {
      printf("Error 6 resNodeListFindPathNext()\n");
    }
    else if (resNodeListFindPathNext(prnFound, pucArgFind, (RN_FIND_FILE | RN_FIND_IN_ARCHIVE)) != NULL) {
      printf("Error 7 resNodeListFindPathNext()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    //puts((const char*)resNodeListToXml(prnT,RN_INFO_MAX));
    resNodeFree(prnT);
  }
#endif
  

#ifdef HAVE_PCRE2
  if (RUNTEST) {
    resNodePtr prnT = NULL;
    resNodePtr prnFound = NULL;
    size_t erroroffset;
    int errornumber;
    int erroffset;
    int opt_match_pcre = PCRE2_UTF;
    pcre2_code *re_match = NULL;

    i++;
    printf("TEST %i in '%s:%i': find filesystem context = ", i, __FILE__, __LINE__);

    re_match = pcre2_compile(
      (PCRE2_SPTR8)"\\.cxp$", /* the pattern */
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
    else if (resNodeListParse(prnT, 999, re_match) == FALSE) {
      printf("Error resNodeListParse() ...\n");
    }
    else if ((prnFound = resNodeListFindPath(prnT, BAD_CAST"dir/config.cxp", (RN_FIND_FILE | RN_FIND_IN_SUBDIR))) == NULL) {
      printf("Error resNodeListFindPath() ...\n");
    }
    else if (resPathIsEquivalent(resNodeGetNameBase(prnFound), BAD_CAST"config.cxp") == FALSE) {
      printf("Error resNodeListFindPath() ...\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    pcre2_code_free(re_match);
    //puts((const char*)resNodeListToXml(prnT,RN_INFO_MIN));
    resNodeFree(prnT);
  }
#endif


  if (RUNTEST) {
    size_t j;
    resNodePtr prnT = NULL;
    resNodePtr prnTT = NULL;
    resNodePtr prnFound = NULL;

    i++;
    printf("TEST %i in '%s:%i': parse filesystem context recursively and search for context = ", i, __FILE__, __LINE__);

    if ((prnTT = resNodeDirNew(BAD_CAST TESTPREFIX "option")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeAddSibling(prnTT, resNodeDirNew(BAD_CAST TEMPPREFIX)) == FALSE) {
      printf("Error resNodeAddSibling()\n");
    }
    else if (resNodeAddSibling(prnTT, resNodeDirNew(BAD_CAST TESTPREFIX "xml/")) == FALSE) {
      printf("Error resNodeAddSibling()\n");
    }
    else if (resNodeAddSibling(prnTT, resNodeDirNew(BAD_CAST TESTPREFIX "DUMMY")) == FALSE) {
      printf("Error resNodeAddSibling()\n");
    }
    else if (resNodeAddSibling(prnTT, resNodeDirNew(BAD_CAST TESTPREFIX "plain//")) == FALSE) {
      printf("Error resNodeAddSibling()\n");
    }
    else if (resNodeListParse(prnTT, 999, NULL) == FALSE) {
      printf("Error resNodeListParse() ...\n");
    }
    else if ((j = resNodeGetChildCount(prnTT, rn_type_dir)) != 10) {
      printf("Error resNodeGetChildCount() = %i\n", j);
    }
    else if ((prnT = resNodeDup(prnTT, RN_DUP_NEXT | RN_DUP_READ)) == NULL) {
      printf("Error resNodeDup()\n");
    }
#if 0
    else if (resNodeListParse(prnT, 999, NULL) == FALSE) {
      printf("Error resNodeListParse() ...\n");
    }
    else if ((j = resNodeGetChildCount(prnT, rn_type_dir)) != 10) {
      printf("Error resNodeDirAppendEntries() = %i\n", j);
    }
#endif
    else if ((prnFound = resNodeListFindPath(prnT, BAD_CAST"xml", (RN_FIND_DIR | RN_FIND_IN_SUBDIR))) == NULL) {
      printf("Error 1 resNodeListFindPath() ...\n");
    }
    else if ((prnFound = resNodeListFindPath(prnT, BAD_CAST "pie/text/config.cxp", (RN_FIND_FILE | RN_FIND_IN_SUBDIR))) == NULL) {
      printf("Error 2 resNodeListFindPath() ...\n");
    }
    else if ((prnFound = resNodeListFindPath(prnT, BAD_CAST "plain/config.cxp", (RN_FIND_ALL))) == NULL) {
      printf("Error 3 resNodeListFindPath() ...\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    //puts((const char*)resNodeListToPlain(prnTT,RN_INFO_MAX));
    resNodeFree(prnT);
    resNodeFree(prnTT);
  }


  if (RUNTEST) {
    int j = 0;
    xmlChar *pucPlain = NULL;
    xmlChar *pucJSON = NULL;
    xmlChar *pucSql = NULL;
    resNodePtr prnT = NULL;
    xmlNodePtr pndT = NULL;

    i++;
    printf("TEST %i in '%s:%i': resNodeTo*() recursive directory = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX)) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeListParse(prnT, 9, NULL) == FALSE) {
      printf("Error resNodeReadStatus()\n");
    }
    else if (resNodeUpdate(prnT, RN_INFO_MAX, NULL, NULL) == FALSE) {
      printf("Error resNodeReadStatus()\n");
    }
    else if ((pndT = resNodeToDOM(prnT, RN_INFO_MAX)) == NULL || (j = domNumberOfChild(pndT, NULL)) != 13) {
      printf("Error resNodeToDOM(): %i\n", j);
    }
    else if ((pucPlain = resNodeToPlain(prnT, RN_INFO_MAX)) == NULL) {
      printf("Error resNodeToPlain() = %i\n", j);
    }
    else if ((pucJSON = resNodeToJSON(prnT, RN_INFO_MAX)) == NULL) {
      printf("Error resNodeToJSON() = %i\n", j);
    }
    else if ((pucSql = resNodeToSQL(prnT, RN_INFO_MAX)) == NULL) {
      printf("Error resNodeToSQL() = %i\n", j);
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //puts((const char *)pucPlain);
    //puts((const char *)pucJSON);
    //puts((const char *)pucSql);

    xmlFree(pucSql);
    xmlFree(pucJSON);
    xmlFree(pucPlain);
    xmlFreeNode(pndT);
    resNodeFree(prnT);
  }


#ifdef HAVE_LIBARCHIVE
  if (RUNTEST) {
    resNodePtr prnT = NULL;
    xmlChar *pucT = NULL;
    xmlChar *pucTT = NULL;
    xmlNodePtr pndT = NULL;

    i++;
    printf("TEST %i in '%s:%i': resNodeTo*() archive file = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "option/archive/test-zip-7.zip")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeUpdate(prnT, RN_INFO_MAX,NULL,NULL) == FALSE) {
      printf("Error resNodeReadStatus()\n");
    }
    else if ((pndT = resNodeToDOM(prnT, RN_INFO_MAX)) == NULL || domNumberOfChild(pndT, NULL) != 1) {
      printf("Error resNodeToDOM()\n");
    }
#ifdef DEBUG
    else if ((pucTT = resNodeListToGraphviz(prnT, RN_INFO_MAX)) == NULL) {
      printf("Error resNodeToPlain() = %i\n", xmlStrlen(pucTT));
    }
#endif
    else if ((pucT = resNodeToPlain(prnT, RN_INFO_MAX)) == NULL) {
      printf("Error resNodeToPlain() = %i\n", xmlStrlen(pucT));
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    
#ifdef DEBUG_YYY
    fputs("digraph g {\n"
	  "fontname=\"Helvetica,Arial,sans-serif\"\n"
	  "node [fontname=\"Helvetica,Arial,sans-serif\"]\n"
	  "edge [fontname=\"Helvetica,Arial,sans-serif\"]\n"
	  "graph [\n"
	  "rankdir = \"LR\"\n"
	  "];\n"
	  "node [\n"
	  "//fontsize = \"16\"\n"
	  "shape = \"record\"\n"
	  "];\n"
	  "edge [\n"
	  "];\n"
	  "node0 [label = \"NULL\"];\n", stdout);
    puts((const char *)pucTT);
    fputs("}\n\n", stdout);
#endif

    xmlFree(pucTT);
    xmlFree(pucT);
    xmlFreeNode(pndT);
    resNodeFree(prnT);
  }
#endif


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    prnT = resNodeDirNew(BAD_CAST TESTPREFIX "xml/");
    if (prnT) {
      /*  */

      if (resNodeListParse(prnT, 2, NULL)) {
	xmlNodePtr pndT;
	xmlChar *pucT = NULL;

	i++;
	printf("TEST %i in '%s:%i': transform resNodeList to DOM = ", i, __FILE__, __LINE__);
	if ((pndT = resNodeListToDOM(prnT, RN_INFO_MAX))) {
	  xmlFreeNode(pndT);
	  n_ok++;
	  printf("OK\n");
	}
	else {
	  printf("Error resNodeListToDOM() ...\n");
	}

	i++;
	printf("TEST %i in '%s:%i': transform resNodeList to XML text = ", i, __FILE__, __LINE__);
	if ((pucT = resNodeListToXml(prnT, RN_INFO_MAX))) {
	  //puts((const char*)pucT);
	  n_ok++;
	  printf("OK\n");
	}
	else {
	  printf("Error resNodeListToXml() ...\n");
	}
	xmlFree(pucT);

	i++;
	printf("TEST %i in '%s:%i': transform resNodeList to plain text = ", i, __FILE__, __LINE__);
	if ((pucT = resNodeListToPlain(prnT, RN_INFO_MAX))) {
	  //puts((const char*)pucT);
	  n_ok++;
	  printf("OK\n");
	}
	else {
	  printf("Error resNodeListToPlain() ...\n");
	}
	xmlFree(pucT);

	i++;
	printf("TEST %i in '%s:%i': transform resNodeList to JSON text = ", i, __FILE__, __LINE__);
	if ((pucT = resNodeListToJSON(prnT, RN_INFO_INFO))) {
	  //puts((const char*)pucT);
	  n_ok++;
	  printf("OK\n");
	}
	else {
	  printf("Error resNodeListToJSON() ...\n");
	}
	xmlFree(pucT);

	i++;
	printf("TEST %i in '%s:%i': transform resNodeList to SQL statements = ", i, __FILE__, __LINE__);
	if ((pucT = resNodeListToSQL(prnT, RN_INFO_MAX))) {
	  //puts((const char*)pucT);
	  n_ok++;
	  printf("OK\n");
	}
	else {
	  printf("Error resNodeListToSQL() ...\n");
	}
	xmlFree(pucT);

      }
      resNodeFree(prnT);
    }
    else {
      printf("Error resNodeNew()\n");
    }
  }


  printf("\nResult in '%s': %i/%i OK\n\n", __FILE__, n_ok, i);

  return (i - n_ok);
}
