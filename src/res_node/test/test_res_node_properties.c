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
resNodeTestProp(void)
{
  int n_ok;
  int i;

  n_ok=0;
  i=0;

  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': set dummy properties = ", i, __FILE__, __LINE__);

    prnT = resNodeDirNew(BAD_CAST TESTPREFIX "dummy.xml");
    if (prnT == NULL) {
      printf("Error resNodeNew()\n");
    }
    else if (resNodeSetProp(NULL, NULL, NULL) != NULL) {
      printf("Error resNodeSetProp() ...\n");
    }
    else if (resNodeSetProp(NULL, BAD_CAST"XYZ", BAD_CAST"ABC") != NULL) {
      printf("Error resNodeSetProp() ...\n");
    }
    else if (resNodeSetProp(prnT, NULL, NULL) != NULL) {
      printf("Error resNodeSetProp() ...\n");
    }
    else if (resNodeSetProp(prnT, NULL, BAD_CAST"") != NULL) {
      printf("Error resNodeSetProp() ...\n");
    }
    else if (resNodeSetProp(prnT, BAD_CAST"", NULL) != NULL) {
      printf("Error resNodeSetProp() ...\n");
    }
    else if (resNodeSetProp(prnT, NULL, BAD_CAST"XYZ") != NULL) {
      printf("Error resNodeSetProp() ...\n");
    }
    else if (resNodeSetProp(prnT, BAD_CAST"XYZ", NULL) != NULL) {
      printf("Error resNodeSetProp() ...\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
  }

  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': get dummy properties = ", i, __FILE__, __LINE__);

    prnT = resNodeDirNew(BAD_CAST TESTPREFIX "dummy.xml");
    if (prnT == NULL) {
      printf("Error resNodeNew()\n");
    }
    else if (resNodeGetProp(NULL, NULL) != NULL) {
      printf("Error resNodeGetProp() ...\n");
    }
    else if (resNodeGetProp(prnT, NULL) != NULL) {
      printf("Error resNodeGetProp() ...\n");
    }
    else if (resNodeGetProp(prnT, BAD_CAST"") != NULL) {
      printf("Error resNodeGetProp() ...\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
  }

  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': new properties = ", i, __FILE__, __LINE__);

    prnT = resNodeDirNew(BAD_CAST TESTPREFIX "abc.xml");
    if (prnT == NULL) {
      printf("Error resNodeNew()\n");
    }
    else if (resNodeSetProp(prnT, BAD_CAST"author", BAD_CAST"Me Myself") == NULL) {
      printf("Error resNodeSetProp() ...\n");
    }
    else if (resNodeSetProp(prnT, BAD_CAST"title", BAD_CAST"My Biography") == NULL) {
      printf("Error resNodeSetProp() ...\n");
    }
    else if (xmlStrEqual(resNodeGetProp(prnT, BAD_CAST"author"), BAD_CAST"Me Myself") == FALSE) {
      printf("Error resNodeGetProp() ...\n");
    }
    else if (resNodeGetProp(prnT, BAD_CAST"AUTHOR") != NULL) {
      printf("Error resNodeGetProp() ...\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
  }

  printf("\nResult in '%s': %i/%i OK\n\n", __FILE__, n_ok, i);

  return (i - n_ok);
}
