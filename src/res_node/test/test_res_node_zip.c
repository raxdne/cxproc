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


/*! test code for this module 
 */
int
resNodeTestZip(void)
{
  int n_ok;
  int i;

  n_ok=0;
  i=0;

#if 0

  if (RUNTEST) {
    resNodePtr prnZip = NULL;

    i++;
    printf("TEST %i in '%s:%i': handle non-existant zip res node = ", i, __FILE__, __LINE__);

    if ((prnZip = resNodeDirNew(BAD_CAST TEMPPREFIX "non-existing.zip")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (zipFileOpen(prnZip,"r") == TRUE) {
      printf("error of zipFileOpen()\n");
    }
    else if (zipAppendEntries(prnZip, NULL, FALSE) == TRUE) {
      printf("Error zipAppendEntries()\n");
    }
    else if (resNodeGetSize(prnZip) > 0) {
      printf("error of resNodeGetSize()\n");
    }
    else if (zipFileClose(prnZip) == TRUE) {
      printf("error of zipFileClose()\n");
    }
    else {
      printf("OK\n");
      n_ok++;
    }
    resNodeFree(prnZip);
  }


  if (RUNTEST) {
    int f;
    resNodePtr prnZip = NULL;
xmlDocPtr pdocT = NULL;

    i++;
    printf("TEST %i in '%s:%i': handle existant zip res node = ", i, __FILE__, __LINE__);

    if ((prnZip = resNodeDirNew(BAD_CAST TESTPREFIX "option/archive/test-zip-7.zip")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeIsZipDocument(prnZip) == TRUE) {
      printf("error of resNodeIsZipDocument()\n");
    }
    else if (zipFileOpen(prnZip,"r") == FALSE) {
      printf("Error zipFileOpen()\n");
    }
    else if (zipAppendEntries(prnZip, NULL, FALSE) == FALSE) {
      printf("Error zipAppendEntries()\n");
    }
    else if (zipFileClose(prnZip) == FALSE) {
      printf("error of zipFileClose()\n");
    }
    else if (resNodeGetSize(prnZip) < 1) {
      printf("error of resNodeGetSize()\n");
    }
    else if ((f = resNodeGetChildCount(prnZip,rn_type_file_in_zip)) != 1) {
      printf("error of resNodeGetChildCount(): %i\n", f);
    }
    else if ((pdocT = resNodeGetContentDoc(prnZip)) == NULL) {
      printf("error of resNodeGetContentDoc(): %i\n", f);
    }
    else {
      printf("OK\n");
      n_ok++;
    }

    domPutDocString(stderr,BAD_CAST"Result Document",pdocT);
xmlFreeDoc(pdocT);

    //puts((const char*)resNodeListToXmlStr(prnZip,RN_INFO_MAX));
    resNodeFree(prnZip);
  }


   if (RUNTEST) {
#define STR_PATH "sub/a.txt"

    resNodePtr prnZip = NULL;
    resNodePtr prnInZip = NULL;
    size_t erroroffset;
    int errornumber;
  int opt_grep_pcre = 0;
  int opt_match_pcre = PCRE2_UTF;
  pcre2_code *re_match = NULL;
#ifdef HAVE_PCRE2
#endif

    i++;
    printf("TEST %i in '%s:%i': access to existant file in zip res node = ", i, __FILE__, __LINE__);

    re_match = pcre2_compile(
      (PCRE2_SPTR8)STR_PATH, /* the pattern */
      PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
      opt_match_pcre,         /* default options */
      &errornumber,          /* for error number */
      &erroroffset,          /* for error offset */
      NULL);                 /* use default compile context */

    if (re_match == NULL) {
    }
    else if ((prnZip = resNodeDirNew(BAD_CAST TESTPREFIX "option/archive/test-zip-7.zip/" STR_PATH)) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeGetMimeType(prnZip) != MIME_APPLICATION_ZIP) {
      printf("error of resNodeGetMimeType()\n");
    }
    else if (zipAppendEntries(prnZip,re_match,TRUE) == FALSE) {
      printf("Error zipAppendEntries()\n");
    }
    else if (resNodeClose(prnZip) == FALSE) {
      printf("error of resNodeClose()\n");
    }
    else if (resNodeGetChildCount(prnZip,rn_type_dir_in_archive) != 1) {
      printf("error of resNodeGetSize()\n");
    }
    else if ((prnInZip = resNodeGetLastDescendant(prnZip)) == NULL) {
      printf("error of resNodeGetSize()\n");
    }
    else if (resNodeGetSize(prnInZip) != 51) {
      printf("error of resNodeGetSize()\n");
    }
    else {
      printf("OK\n");
      n_ok++;
    }

    if (re_match) {
      pcre2_code_free(re_match);
    }

    //puts((const char *)resNodeListToPlain(prnZip, RN_INFO_MAX));
    resNodeFree(prnZip);
  }

#endif

  if (RUNTEST) {
    resNodePtr prnZipDoc = NULL;

    i++;
    printf("TEST %i in '%s:%i': handle non-existant zip document res node = ", i, __FILE__, __LINE__);

    if ((prnZipDoc = resNodeDirNew(BAD_CAST TEMPPREFIX "non-existing.docx")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeIsZipDocument(prnZipDoc) == FALSE) {
      printf("error of resNodeIsZipDocument()\n");
    }
    else if (zipDocumentRead(prnZipDoc,RN_INFO_MAX) == TRUE) {
      printf("Error zipDocumentRead()\n");
    }
    else if (resNodeClose(prnZipDoc) == TRUE) {
      printf("error of resNodeClose()\n");
    }
    else if (resNodeGetSize(prnZipDoc) > 0) {
      printf("error of resNodeGetSize()\n");
    }
    else if (resNodeGetChildCount(prnZipDoc,rn_type_file_in_zip) > 0) {
      printf("error of resNodeGetChildCount()\n");
    }
    else {
      printf("OK\n");
      n_ok++;
    }
    resNodeFree(prnZipDoc);
  }


 if (RUNTEST) {
   int j;
   resNodePtr prnZipDoc = NULL;

   i++;
   printf("TEST %i in '%s:%i': handle existant zip res node as Document = ", i, __FILE__, __LINE__);

   if ((prnZipDoc = resNodeDirNew(BAD_CAST TESTPREFIX "option/archive/test-zip-7.zip")) == NULL) {
     printf("Error resNodeDirNew()\n");
    }
    else if (resNodeIsZipDocument(prnZipDoc) == TRUE) {
      printf("error of resNodeIsZipDocument()\n");
    }
    else if (zipDocumentRead(prnZipDoc,RN_INFO_MAX) == TRUE) {
      printf("Error zipDocumentRead()\n");
    }
    else if (resNodeClose(prnZipDoc) == TRUE) {
      printf("error of resNodeClose()\n");
    }
    else if ((j = resNodeGetCountDescendants(prnZipDoc)) != 1) {
      printf("error of resNodeGetCountDescendants(): %i\n", j);
    }
    else {
      printf("OK\n");
      n_ok++;
    }

    //puts((const char*)resNodeListToXmlStr(prnZipDoc,RN_INFO_MAX));
    resNodeFree(prnZipDoc);
  }


 if (RUNTEST) {
   int j;
   resNodePtr prnZipDoc = NULL;

   i++;
   printf("TEST %i in '%s:%i': handle existant zip document res node = ", i, __FILE__, __LINE__);

   if ((prnZipDoc = resNodeDirNew(BAD_CAST TESTPREFIX "xml/test-xml-zip.odt")) == NULL) {
     printf("Error resNodeDirNew()\n");
    }
    else if (resNodeIsZipDocument(prnZipDoc) == FALSE) {
      printf("error of resNodeIsZipDocument()\n");
    }
    else if (zipDocumentRead(prnZipDoc,RN_INFO_MAX) == FALSE) {
      printf("Error zipDocumentRead()\n");
    }
    else if (resNodeClose(prnZipDoc) == FALSE) {
      printf("error of resNodeClose()\n");
    }
    else if (resNodeGetSize(prnZipDoc) < 1) {
      printf("error of resNodeGetSize()\n");
    }
    else if ((j = resNodeGetCountDescendants(prnZipDoc)) != 23) {
      printf("error of resNodeGetCountDescendants(): %i\n", j);
    }
    else {
      printf("OK\n");
      n_ok++;
    }

    //puts((const char*)resNodeListToXmlStr(prnZipDoc,RN_INFO_MAX));
    resNodeFree(prnZipDoc);
  }


  printf("TEST in '%s': %i/%i OK\n\n",__FILE__,n_ok,i);

  return (i - n_ok);
}
