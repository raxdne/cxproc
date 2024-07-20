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


/*! test code for this module 
 */
int
resNodeTestZip(void)
{
  int n_ok;
  int i;

  n_ok=0;
  i=0;

  if (RUNTEST) {
    resNodePtr prnZip = NULL;

    i++;
    printf("TEST %i in '%s:%i': handle non-existant zip res node = ", i, __FILE__, __LINE__);

    if ((prnZip = resNodeDirNew(BAD_CAST TEMPPREFIX "aaa.zip")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeOpen(prnZip, "wa") == TRUE) {
      printf("error of resNodeOpen()\n");
    }
    else if (zipAppendEntries(prnZip, NULL, FALSE) == TRUE) {
      printf("Error zipAppendEntries()\n");
    }
    else if (resNodeGetSize(prnZip) > 0) {
      printf("error of resNodeGetSize()\n");
    }
    else if (resNodeClose(prnZip) == TRUE) {
      printf("error of resNodeClose()\n");
    }
    else {
      printf("OK\n");
      n_ok++;
    }
    resNodeFree(prnZip);
  }


  if (RUNTEST) {
    resNodePtr prnZip = NULL;

    i++;
    printf("TEST %i in '%s:%i': handle existant zip res node = ", i, __FILE__, __LINE__);

    if ((prnZip = resNodeDirNew(BAD_CAST TESTPREFIX "option/archive/2023-11-17-PLMDOCM-ReleaseChecklist-wt12mig112-Build_5.2.1.17.mmap")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeIsZipDocument(prnZip) == FALSE) {
      printf("error of resNodeIsZipDocument()\n");
    }
    else if (zipAppendEntries(prnZip, NULL, FALSE) == TRUE) {
      printf("Error zipAppendEntries()\n");
    }
    else if (resNodeClose(prnZip) == FALSE) {
      printf("error of resNodeClose()\n");
    }
    else if (resNodeGetSize(prnZip) < 1) {
      printf("error of resNodeGetSize()\n");
    }
    else if (resNodeGetChildCount(prnZip,rn_type_file_in_archive) != 11) {
      printf("error of resNodeGetSize()\n");
    }
    else {
      printf("OK\n");
      n_ok++;
    }

    //puts((const char*)resNodeListToXml(prnZip,RN_INFO_MAX));
    resNodeFree(prnZip);
  }


  if (RUNTEST) {
    resNodePtr prnZip = NULL;

    i++;
    printf("TEST %i in '%s:%i': handle existant zip res node = ", i, __FILE__, __LINE__);

    if ((prnZip = resNodeDirNew(BAD_CAST TESTPREFIX "xml/test-xml-zip.odt")) == NULL) { // "../../../Trash/2023-11-17-PLMDOCM-ReleaseChecklist-wt12mig112-Build_5.2.1.17.mmap"
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeIsZipDocument(prnZip) == FALSE) {
      printf("error of resNodeIsZipDocument()\n");
    }
    else if (zipDocumentRead(prnZip) == FALSE) {
      printf("Error zipDocumentRead()\n");
    }
    else if (resNodeClose(prnZip) == FALSE) {
      printf("error of resNodeClose()\n");
    }
    else if (resNodeGetSize(prnZip) < 1) {
      printf("error of resNodeGetSize()\n");
    }
    else if (resNodeGetChildCount(prnZip,rn_type_file_in_zip) != 4) {
      printf("error of resNodeGetSize()\n");
    }
    else {
      printf("OK\n");
      n_ok++;
    }

    puts((const char*)resNodeListToXml(prnZip,RN_INFO_MAX));
    resNodeFree(prnZip);
  }


  printf("TEST in '%s': %i/%i OK\n\n",__FILE__,n_ok,i);

  return (i - n_ok);
}
