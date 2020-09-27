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
plainTextTest(void)
{
  int n_ok;
  int i;

  n_ok=0;
  i=0;


  if (RUNTEST) {
    xmlChar *pucContent = NULL;
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': reads content of file context = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "option/pie/text/test-pie-3.txt")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if ((pucContent = plainGetContextTextEat(prnT, -1)) == NULL) {
      printf("Error resNodeOpen()\n");
    }
    else if (resNodeGetSize(prnT) != 259384) {
      printf("Error plainGetContextTextEat(): %i Bytes\n", xmlStrlen(pucContent));
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    xmlFree(pucContent);
    resNodeFree(prnT);
  }

  
#if TESTHTTP
  if (RUNTEST) {
    xmlChar *pucContent = NULL;
    index_t iLength;
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': reads plain text http content = ",i,__FILE__,__LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST HTTPPREFIX "Test/Documents/TestContent.txt")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if ((pucContent = plainGetContextTextEat(prnT, -1)) == NULL) {
      printf("Error plainGetContextTextEat()\n");
    }
    else if ((iLength = xmlStrlen(BAD_CAST pucContent)) != 219) {
      printf("Error xmlStrlen(): %i\n", iLength);
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    xmlFree(pucContent);
    resNodeFree(prnT);
  }
#endif

  printf("Result in '%s': %i/%i OK\n\n",__FILE__,n_ok,i);

  return (i - n_ok);
}
/* end of pieTextTest() */
