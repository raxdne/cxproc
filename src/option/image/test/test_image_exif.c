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

/*!
*/
int
imageExifTest(void)
{
  int i;
  int n_ok;

  n_ok=0;
  i=0;

  if (RUNTEST) {
    
    i++;
    printf("TEST %i in '%s:%i': empty imgParseFileExif() = ",i,__FILE__,__LINE__);

    if (imgParseFileExif(NULL,NULL) == TRUE) {
      printf("Error imgParseFileExif()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    xmlNodePtr pndT = NULL;
    resNodePtr prnT = NULL;
    xmlChar *pucT = xmlStrdup(BAD_CAST TESTPREFIX "option/image/Empty.jpeg");
    
    i++;
    printf("TEST %i in '%s:%i': empty file imgParseFileExif() = ",i,__FILE__,__LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST pucT)) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeGetType(prnT) != rn_type_file) {
      printf("Error resNodeGetType()\n");
    }
    else if (resNodeGetChild(prnT) != NULL) {
      printf("Error resNodeGetChild()\n");
    }
    else if (resNodeGetMimeType(prnT) != MIME_IMAGE_JPEG) {
      printf("Error resNodeGetMimeType()\n");
    }
    else if ((pndT = resNodeToDOM(prnT, RN_INFO_MAX)) == NULL) {
      printf("Error resNodeToDOM()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    //domPutNodeString(stderr, BAD_CAST"imgParseFileExif() result", pndT);
    xmlFreeNode(pndT);
    resNodeFree(prnT);
    xmlFree(pucT);
  }


  if (RUNTEST) {
    xmlNodePtr pndT = NULL;
    resNodePtr prnT = NULL;
    xmlChar *pucT = xmlStrdup(BAD_CAST TESTPREFIX "option/image/Alps.jpg");
    
    i++;
    printf("TEST %i in '%s:%i': imgParseFileExif() = ",i,__FILE__,__LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST pucT)) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeGetType(prnT) != rn_type_file) {
      printf("Error resNodeGetType()\n");
    }
    else if (resNodeGetChild(prnT) != NULL) {
      printf("Error resNodeGetChild()\n");
    }
    else if (resNodeGetMimeType(prnT) != MIME_IMAGE_JPEG) {
      printf("Error resNodeGetMimeType()\n");
    }
    else if ((pndT = resNodeToDOM(prnT, RN_INFO_MAX)) == NULL) {
      printf("Error resNodeToDOM()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    //domPutNodeString(stderr, BAD_CAST"imgParseFileExif() result", pndT);
    xmlFreeNode(pndT);
    resNodeFree(prnT);
    xmlFree(pucT);
  }

  printf("Result in '%s': %i/%i OK\n\n",__FILE__,n_ok,i);

  return (i - n_ok);
}
/* end of imageExifTest() */
