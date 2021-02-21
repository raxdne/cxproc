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
arcTest(void)
{
  int i;
  int n_ok;

  n_ok=0;
  i=0;


  if (RUNTEST) {
    arcPtr a;
    int r;

    i++;
    printf("TEST %i in '%s:%i': test supported archive formats = ", i, __FILE__, __LINE__);

    if ((a = archive_read_new()) != NULL && archive_read_support_filter_all(a) == ARCHIVE_OK) {
      r = archive_read_support_format_gnutar(a);
      if (r == ARCHIVE_OK) {
	printf("OK\n");
	n_ok++;
      }
      else {
	printf("gnutar reading not fully supported on this platform\n");
      }
      archive_read_free(a);
    }
  }

  if (RUNTEST) {
    arcPtr a;

    i++;
    printf("TEST %i in '%s:%i': test opening of non-existing archive = ", i, __FILE__, __LINE__);

    if ((a = archive_read_new()) != NULL && archive_read_open_filename(a, TESTPREFIX "option/archive/test-arc-fail.tar", 2) != ARCHIVE_OK) {
      printf("OK\n");
      n_ok++;
    }
    else {
      printf("error handling of archive_read_open_filename()\n");
    }
    archive_read_free(a);
  }

  if (RUNTEST) {
    arcPtr a;
    arcEntryPtr ae;
    char *pcNameFile = strdup(TESTPREFIX "option/archive\\test-arc.txt.gz");
    int cchReadInput;		/*! counter for collected string length */
    char mcContent[BUFFER_LENGTH];

    i++;
    printf("TEST %i in '%s:%i': check data of existing gzip archive = ", i, __FILE__, __LINE__);

    resPathChangeToSlashes(BAD_CAST pcNameFile);
    
    if ((a = archive_read_new()) == NULL) {
      printf("error of archive_read_new(): '%s'\n", archive_error_string(a));
    }
    else if (archive_read_support_filter_all(a) != ARCHIVE_OK) {
      printf("error of archive_read_support_filter_all(): '%s'\n", archive_error_string(a));
    }
    else if (archive_read_support_format_empty(a) != ARCHIVE_OK) {
      printf("error of archive_read_support_format_empty(): '%s'\n", archive_error_string(a));
    }
    else if (archive_read_support_format_raw(a) != ARCHIVE_OK) {
      printf("error of archive_read_support_format_raw(): '%s'\n", archive_error_string(a));
    }
    else if (archive_read_open_filename(a, pcNameFile, BUFFER_LENGTH) != ARCHIVE_OK) {
      printf("error of archive_read_open_filename(): '%s'\n", archive_error_string(a));
    }
    else if (archive_read_next_header(a, &ae) != ARCHIVE_OK) {
      printf("Could not read next file from %s\n", pcNameFile);
    }
    else if (cchReadInput = archive_read_data(a, mcContent, BUFFER_LENGTH) != 1024) {
      printf("Archive read error: %s", archive_error_string(a));
    }
    else if (archive_read_next_header(a, &ae) != ARCHIVE_EOF) {
      printf("Could not detect ARCHIVE_EOF from %s\n", pcNameFile);
    }
    else {
      printf("OK\n");
      n_ok++;
    }
    archive_read_close(a);
    archive_read_free(a);
    free(pcNameFile);
  }

  if (RUNTEST) {
    arcPtr a;
    char *pcNameFile = TESTPREFIX "option/archive/test-zip-7.zip";
    BOOL_T fTest = TRUE;

    i++;
    printf("TEST %i in '%s:%i': check entries of existing ZIP archive = ", i, __FILE__, __LINE__);

    if ((a = archive_read_new()) == NULL) {
      printf("error of archive_read_new()\n");
      fTest = FALSE;
    }
    else if (archive_read_support_format_zip(a) != ARCHIVE_OK) {
      printf("error of archive_read_support_format_zip()\n");
      fTest = FALSE;
    }
    else if (archive_read_open_filename(a, pcNameFile, 2) != ARCHIVE_OK) {
      printf("error of archive_read_open_filename()\n");
      fTest = FALSE;
    }
    else {
      const char *n[] ={ "sub/", "sub/a.txt", "sub/b.txt", "sub/plain.txt", "sub/weiter.png", "TestContent.odt", NULL }; // , "sub/Umlaut ���.txt"
      arcEntryPtr ae;
      char *pcT;
      xmlChar *pucT = NULL;
      int j;

      /* Read entries, match up names with list above. */
      for (j = 0; fTest && n[j]; ++j) {
	if (archive_read_next_header(a, &ae) != ARCHIVE_OK) {
	  printf("Could not read file %d (%s) from %s\n", j, n[j], pcNameFile);
	  fTest = FALSE;
	}
	else if ((pcT = (char *)archive_entry_pathname(ae)) != NULL
	  && arcGetFileNameDecoded(pcT, &pucT)
	  && xmlStrEqual(BAD_CAST n[j], pucT)) {
	}
	else {
	  printf("Could not read file %d (%s) from %s\n", j, n[j], pcNameFile);
	  fTest = FALSE;
	}
	xmlFree(pucT);
      }

      /* Verify the end-of-archive. */
      if (archive_read_next_header(a, &ae) == ARCHIVE_EOF) {
      }
      else {
	printf("Could not detect ARCHIVE_EOF from %s\n", pcNameFile);
	fTest = FALSE;
      }

      /* Verify the format. */
      if (archive_format(a) == ARCHIVE_FORMAT_ZIP) {
      }
      else {
	printf("Could not read file format from %s\n", pcNameFile);
	fTest = FALSE;
      }

      archive_read_close(a);
    }
    archive_read_free(a);

    if (fTest) {
      printf("OK\n");
      n_ok++;
    }
  }


  if (SKIPTEST) {
    arcPtr a;
    char *pcNameFile = TESTPREFIX "option/archive/TestContent.mmap";
    BOOL_T fTest = TRUE;

    i++;
    printf("TEST %i in '%s:%i': check entries of existing ZIP/MMAP archive = ", i, __FILE__, __LINE__);

    if ((a = archive_read_new()) == NULL) {
      printf("error of archive_read_new()\n");
      fTest = FALSE;
    }
    else if (archive_read_support_format_zip(a) != ARCHIVE_OK) {
      printf("error of archive_read_support_format_zip()\n");
      fTest = FALSE;
    }
    else if (archive_read_open_filename(a, pcNameFile, 2) != ARCHIVE_OK) {
      printf("error of archive_read_open_filename()\n");
      fTest = FALSE;
    }
    else {
      const char *n[] ={ "Document.xml", "xsd/MindManagerDelta.xsd", "bin/91D56B1A-7011-4CC6-827C-6D562F70DE95.bin", "xsd/MindManagerApplication.xsd", "xsd/", "xsd/MindManagerCore.xsd", "bin/", "xsd/MindManagerPrimitive.xsd", "Preview.png", NULL };
      arcEntryPtr ae;
      char *pcT;
      xmlChar *pucT = NULL;
      int j;

      /* Read entries, match up names with list above. */
      for (j = 0; fTest && n[j]; ++j) {
	if (archive_read_next_header(a, &ae) != ARCHIVE_OK) {
	  printf("Could not read file %d (%s) from %s: '%s'\n", j, n[j], pcNameFile, archive_error_string(a));
	  fTest = FALSE;
	}
	else if ((pcT = (char *)archive_entry_pathname(ae)) != NULL
	  && arcGetFileNameDecoded(pcT, &pucT)
	  && xmlStrEqual(BAD_CAST n[j], pucT)) {
	}
	else {
	  printf("Could not read file %d (%s) from %s\n", j, n[j], pcNameFile);
	  fTest = FALSE;
	}
	xmlFree(pucT);
      }

      /* Verify the end-of-archive. */
      if (archive_read_next_header(a, &ae) == ARCHIVE_EOF) {
      }
      else {
	printf("Could not detect ARCHIVE_EOF from %s\n", pcNameFile);
	fTest = FALSE;
      }

      /* Verify the format. */
      if (archive_format(a) == ARCHIVE_FORMAT_ZIP) {
      }
      else {
	printf("Could not read file format from %s\n", pcNameFile);
	fTest = FALSE;
      }

      archive_read_close(a);
    }
    archive_read_free(a);

    if (fTest) {
      printf("OK\n");
      n_ok++;
    }
  }

  if (RUNTEST) {
    arcPtr a;
    char *pcNameFile = TESTPREFIX "option/archive/test-arc-1.tar";
    BOOL_T fTest = TRUE;

    i++;
    printf("TEST %i in '%s:%i': check entries of existing TAR archive = ", i, __FILE__, __LINE__);

    if ((a = archive_read_new()) != NULL
      && archive_read_support_format_gnutar(a) == ARCHIVE_OK
      && archive_read_open_filename(a, pcNameFile, 2) == ARCHIVE_OK) {
      const char *n[] ={
	"2446.ics",
	"test-pie-11.txt",
	"test-pie-19.mm",
	"test-pie-20.odt",
	NULL };

      arcEntryPtr ae;
      char *pcT;
      xmlChar *pucT;
      int j;

      /* Read entries, match up names with list above. */
      for (j = 0; fTest && n[j]; ++j) {
	if (archive_read_next_header(a, &ae) != ARCHIVE_OK) {
	  printf("Could not read file %d (%s) from %s\n", j, n[j], pcNameFile);
	  fTest = FALSE;
	}
	else if ((pcT = (char *)archive_entry_pathname(ae)) != NULL
	  && arcGetFileNameDecoded(pcT, &pucT)
	  && xmlStrEqual(BAD_CAST n[j], pucT)) {
	}
	else {
	  printf("Could not read file %d (%s) from %s\n", j, n[j], pcNameFile);
	  fTest = FALSE;
	}
	xmlFree(pucT);
      }

      /* Verify the end-of-archive. */
      if (archive_read_next_header(a, &ae) == ARCHIVE_EOF) {
      }
      else {
	printf("Could not detect ARCHIVE_EOF from %s\n", pcNameFile);
	fTest = FALSE;
      }

      /* Verify the format. */
      if (archive_format(a) == ARCHIVE_FORMAT_TAR) {
      }
      else {
	printf("Could not read file format from %s\n", pcNameFile);
	fTest = FALSE;
      }

      archive_read_close(a);
    }
    else {
      printf("error of archive_read_open_filename()\n");
      fTest = FALSE;
    }
    archive_read_free(a);

    if (fTest) {
      printf("OK\n");
      n_ok++;
    }
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': test opening of existing ZIP archive = ", i, __FILE__, __LINE__);

    prnT = resNodeDirNew(BAD_CAST TESTPREFIX "option/archive/test-zip-7.zip");
    if (prnT) {
      if (resNodeOpen(prnT, "ra") == TRUE) {

	if (archive_format((arcPtr)resNodeGetHandleIO(prnT)) != ARCHIVE_FAILED) {
	  printf("Could not verify file format from %s\n", resNodeGetNameNormalized(prnT));
	}

	resNodeClose(prnT);
	if (resNodeGetHandleIO(prnT) == NULL) {
	  printf("OK\n");
	  n_ok++;
	}
	else {
	  printf("error of resNodeClose()\n");
	}
      }
      else {
	printf("error of resNodeOpen()\n");
      }
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


  if (RUNTEST) {
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
  
  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': read entry content of existing ODT = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "option/pie/text/test-pie-20.odt/content.xml")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeIsArchive(prnT) == FALSE) {
      printf("Error resNodeIsArchive()\n");
    }
    else if (resNodeUpdate(prnT, RN_INFO_MAX, NULL, NULL) == FALSE) {
      printf("Error 1 resNodeUpdate()\n");
    }
    else if (resNodeGetContentPtr(prnT) != NULL || resNodeGetSize(prnT) != 8818) {
      printf("Error 2 resNodeUpdate()\n");
    }
    else if (resNodeGetContentPtr(resNodeGetChild(prnT)) == NULL || resNodeGetSize(resNodeGetChild(prnT)) != 2968) {
      printf("Error 3 resNodeUpdate()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
  }


  printf("TEST in '%s': %i/%i OK\n\n", __FILE__, n_ok, i);

  return (i - n_ok);
}
/* end of arcTest() */
