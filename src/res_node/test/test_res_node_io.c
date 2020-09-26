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
resNodeTestInOut(void)
{
  int n_ok;
  int i;

  n_ok=0;
  i=0;

  if (RUNTEST) {

    i++;
    printf("TEST %i in '%s:%i': open and close a NULL context = ", i, __FILE__, __LINE__);

    if (resNodeOpen(NULL, "r") == TRUE) {
      printf("Error resNodeOpen()\n");
    }
    else if (resNodeIsOpen(NULL) == TRUE) {
      printf("Error resNodeIsOpen()\n");
    }
    else if (resNodeClose(NULL) == TRUE) {
      printf("Error resNodeClose()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': open and close a non existing file context = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "tmp/dummy.txt")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeOpen(prnT, "rb") == TRUE) {
      printf("Error resNodeOpen()\n");
    }
    else if (resNodeIsOpen(prnT) == TRUE) {
      printf("Error resNodeIsOpen()\n");
    }
    else if (resNodeClose(prnT) == TRUE) {
      printf("Error resNodeClose()\n");
    }
    else if (resNodeOpen(prnT, "w+") == TRUE) {
      printf("Error resNodeOpen()\n");
    }
    else if (resNodeIsOpen(prnT) == TRUE) {
      printf("Error resNodeIsOpen()\n");
    }
    else if (resNodeClose(prnT) == TRUE) {
      printf("Error resNodeClose()\n");
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
    printf("TEST %i in '%s:%i': open and close an existing file context = ",i,__FILE__,__LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "plain/Length_128.txt")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeOpen(prnT, "rb") == FALSE) {
      printf("Error resNodeOpen()\n");
    }
    else if (resNodeIsOpen(prnT) == FALSE) {
      printf("Error resNodeIsOpen()\n");
    }
    else if (resNodeClose(prnT) == FALSE) {
      printf("Error resNodeClose()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
  }

#ifdef HAVE_LIBSQLITE3
  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': open and close an existing local SQLite file context = ",i,__FILE__,__LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "option/sql/test.db3")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeOpen(prnT, "r") == FALSE) {
      printf("Error resNodeOpen()\n");
    }
    else if (resNodeIsOpen(prnT) == FALSE) {
      printf("Error resNodeIsOpen()\n");
    }
    else if (resNodeClose(prnT) == FALSE) {
      printf("Error resNodeClose()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
  }
#endif


#ifdef HAVE_LIBARCHIVE
  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': open and close an existing ZIP file context = ",i,__FILE__,__LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "option/archive/test-zip-7.zip")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeIsArchive(prnT) == FALSE) {
      printf("Error resNodeIsArchive()\n");
    }
    else if (resNodeOpen(prnT,"ra") == FALSE) {
      printf("Error resNodeOpen()\n");
    }
    else if (prnT->eMode != mode_read) {
      printf("Error eAccess\n");
    }
    else if (resNodeIsOpen(prnT) == FALSE) {
      printf("Error resNodeIsOpen()\n");
    }
    else if (resNodeGetChild(prnT) != NULL) {
      printf("Error resNodeOpen()\n");
    }
    else if (resNodeGetLength(prnT) != 1) {
      printf("Error resNodeGetLength() ...\n");
    }
    else if (prnT->eAccess != rn_access_archive) {
      printf("Error eAccess\n");
    }
    else if (resNodeClose(prnT) == FALSE) {
      printf("Error resNodeClose()\n");
    }
    else if (resNodeIsOpen(prnT) == TRUE) {
      printf("Error resNodeIsOpen()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    
    resNodeFree(prnT);
  }

  if (SKIPTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': create and close a non-existing ZIP file context = ",i,__FILE__,__LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST"tmp/created.zip")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeOpen(prnT,"wa") == FALSE) {
      printf("Error resNodeOpen()\n");
    }
    else if (resNodeIsOpen(prnT) == FALSE) {
      printf("Error resNodeIsOpen()\n");
    }
    else if (resNodeGetChild(prnT) != NULL) {
      printf("Error resNodeOpen()\n");
    }
    else if (prnT->eAccess != rn_access_archive) {
      printf("Error eAccess\n");
    }
    else if (prnT->eMode != mode_write) {
      printf("Error eAccess\n");
    }
    else if (resNodeClose(prnT) == FALSE) {
      printf("Error resNodeClose()\n");
    }
    else if (resNodeIsOpen(prnT) == TRUE) {
      printf("Error resNodeIsOpen()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    
    resNodeUnlink(prnT,FALSE);
    resNodeFree(prnT);
  }
#endif


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    long int liLength = 0;

    i++;
    printf("TEST %i in '%s:%i': reads content of a small file context = ",i,__FILE__,__LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "plain/Length_128.txt")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeOpen(prnT,"rb") == FALSE) {
      printf("Error resNodeOpen()\n");
    }
    else if (resNodeReadContent(prnT,1) == FALSE) {
      printf("Error resNodeReadContent()\n");
    }
    else if (resNodeGetSize(prnT) != 258) {
      printf("Error resNodeReadContent(): %i Bytes\n",resNodeGetSize(prnT));
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    
    resNodeFree(prnT);
  }

  
  if (RUNTEST) {
    resNodePtr prnT = NULL;

    void *pContent;
    long int liLengthLarge = 512 * SIZE_MEGA;
    xmlChar *pucNameFile = BAD_CAST TEMPPREFIX "512MB.dat";

    i++;
    printf("TEST %i in '%s:%i': write huge content of a resource node = ",i,__FILE__,__LINE__);

    if ((pContent = xmlMalloc(liLengthLarge)) != NULL) {
      memset(pContent, 0x0F, liLengthLarge);

      if ((prnT = resNodeDirNew(pucNameFile)) == NULL) {
	printf("Error resNodeDirNew()\n");
      }
      else if (resNodeOpen(prnT,"w") == FALSE) {
	printf("Error resNodeOpen()\n");
      }
      else if (resNodeSetContentPtr(prnT,pContent,liLengthLarge) != pContent) {
	printf("Error resNodeSetContentPtr()\n");
      }
      else if (resNodeSaveContent(prnT) == FALSE) {
	printf("Error resNodeSaveContent()\n");
      }      
      else {
	n_ok++;
	printf("OK\n");
      }

      resNodeFree(prnT);
      resNodeUnlinkStr(BAD_CAST pucNameFile);
    }
  }

#ifdef HAVE_LIBSQLITE3
  
  if (RUNTEST) {
    resNodePtr prnT = NULL;
    xmlChar *pucNameFile = BAD_CAST TESTPREFIX "option/sql/test.db3";

    i++;
    printf("TEST %i in '%s:%i': read content of a database resource node = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(pucNameFile)) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeReadStatus(prnT) == FALSE) {
      printf("Error resNodeReadStatus()\n");
    }
    else if (resNodeGetContent(prnT, 1024) == NULL) {
      printf("Error resNodeGetContent()\n");
    }
    else if (resNodeGetSize(prnT) != 8192) {
      printf("Error resNodeGetSize()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    resNodeFree(prnT);
  }

#endif

  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': reads content of file context = ",i,__FILE__,__LINE__);

    prnT = resNodeDirNew(BAD_CAST TESTPREFIX "plain/Length_128.txt");
    if (prnT) {
      xmlChar *pucContent;

      if ((pucContent = plainGetContextTextEat(prnT,8)) != NULL && xmlUTF8Strlen(pucContent) == 129) {
	n_ok++;
	printf("OK\n");
      }
      else {
	printf("Error plainGetContextTextEat(): %i UTF-8 Bytes\n",xmlUTF8Strlen(pucContent));
      }
      xmlFree(pucContent);
      resNodeFree(prnT);
    }
    else {
      printf("Error resNodeNew()\n");
    }
  }

  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': reads base 64 content of file context = ",i,__FILE__,__LINE__);

    prnT = resNodeDirNew(BAD_CAST TESTPREFIX "plain/Length_128.txt");
    if (prnT) {
      xmlChar *pucContent;

      if ((pucContent = resNodeGetContentBase64Eat(prnT,8)) != NULL
	  && xmlStrEqual(pucContent,BAD_CAST"w4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhMOEw4TDhA0K")) {
	n_ok++;
	printf("OK\n");
      }
      else {
	printf("Error resNodeGetContentBase64Eat(): %i Base 64 Bytes\n",xmlStrlen(pucContent));
      }
      xmlFree(pucContent);
      resNodeFree(prnT);
    }
    else {
      printf("Error resNodeNew()\n");
    }
  }

#if TESTHTTP

  if (RUNTEST) {
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': open and close a dummy HTTP file context = ",i,__FILE__,__LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST "http://192.168.1.111/")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeIsExist(prnT) == TRUE) {
      printf("Error resNodeOpen()\n");
    }
    else if (resNodeOpen(prnT, "w") == TRUE) {
      printf("Error resNodeOpen()\n");
    }
    else if (resNodeOpen(prnT, "r") == TRUE) {
      printf("Error resNodeOpen()\n");
    }
    else if (resNodeIsOpen(prnT) == TRUE) {
      printf("Error resNodeIsOpen()\n");
    }
    else if (resNodeClose(prnT) == TRUE) {
      printf("Error resNodeClose()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
  }

  if (RUNTEST) {
    resNodePtr prnT = NULL;
    xmlChar *pucContent = NULL;

    i++;
    printf("TEST %i in '%s:%i': open and close an local HTTP file context = ",i,__FILE__,__LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST HTTPPREFIX)) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeIsURL(prnT) == FALSE) {
      printf("Error resNodeIsURL()\n");
    }
    else if (resNodeOpen(prnT, "w") == TRUE) {
      printf("Error resNodeOpen()\n");
    }
    else if (resNodeOpen(prnT, "r") == FALSE) {
      printf("Error resNodeOpen()\n");
    }
    else if (resNodeIsExist(prnT) == FALSE) {
      printf("Error resNodeIsExist()\n");
    }
    else if (resNodeIsOpen(prnT) == FALSE) {
      printf("Error resNodeIsOpen()\n");
    }
    else if (resNodeReadContent(prnT, 1024) == FALSE) {
      printf("Error resNodeReadContent(): there is no content\n");
    }
    else if (resNodeGetError(prnT) == TRUE) {
      printf("Error resNodeGetError(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeClose(prnT) == FALSE) {
      printf("Error resNodeClose()\n");
    }
    else if ((pucContent = plainGetContextTextEat(prnT,8)) == NULL || xmlUTF8Strlen(pucContent) < 40) {
      printf("Error plainGetContextTextEat()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    xmlFree(pucContent);
    resNodeFree(prnT);
  }

  if (RUNTEST) {
    resNodePtr prnT = NULL;
    xmlDocPtr pdocT = NULL;

    i++;
    printf("TEST %i in '%s:%i': read a global HTTP file context as DOM = ",i,__FILE__,__LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST "http://www.tagesschau.de/newsticker.rdf")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if ((pdocT = resNodeGetContentDoc(prnT)) == NULL) {
      printf("Error resNodeGetContentDoc()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutDocString(stderr, BAD_CAST "RDF ", pdocT);
    resNodeFree(prnT);
  }

  if (RUNTEST) {
    resNodePtr prnT = NULL;
    xmlChar *pucContent = NULL;

    i++;
    printf("TEST %i in '%s:%i': open and close an local file context = ",i,__FILE__,__LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST "file://" TESTPREFIX "plain/Length_128.txt")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeIsURL(prnT) == TRUE) {
      printf("Error resNodeIsURL()\n");
    }
    else if (resNodeOpen(prnT, "r") == FALSE) {
      printf("Error resNodeOpen()\n");
    }
    else if (resNodeReadContent(prnT, 1024) == FALSE) {
      printf("Error resNodeReadContent(): there is no content\n");
    }
    else if (resNodeIsExist(prnT) == FALSE) {
      printf("Error resNodeIsExist()\n");
    }
    else if (resNodeIsOpen(prnT) == FALSE) {
      printf("Error resNodeIsOpen()\n");
    }
    else if (resNodeGetError(prnT) == TRUE) {
      printf("Error resNodeGetError(): %s\n",resNodeGetErrorMsg(prnT));
    }
    else if (resNodeClose(prnT) == FALSE) {
      printf("Error resNodeClose()\n");
    }
    else if ((pucContent = plainGetContextTextEat(prnT,8)) == NULL || resNodeGetSize(prnT) != 258) {
      printf("Error plainGetContextTextEat()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    xmlFree(pucContent);
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    resNodePtr prnChild = NULL;

    i++;
    printf("TEST %i in '%s:%i': open, read and close a HTTP archive resource (resNodeGetContent) = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST HTTPPREFIX "test-zip-7.zip/sub/plain.txt")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if ((prnChild = resNodeGetChild(prnT)) == NULL || (prnChild = resNodeGetChild(prnChild)) == NULL) {
      printf("Error 1 resNodeGetSize()\n");
    }
    else if (resNodeGetContent(prnChild, 1024) == NULL) {
      printf("Error resNodeGetContent(): there is no content\n");
    }
    else if (resNodeGetSize(prnChild) != 49) {
      printf("Error 2 resNodeGetSize()\n");
    }
    else {
      //puts((const char *)resNodeGetContentPtr(prnChild));
      n_ok++;
      printf("OK\n");
    }
    resNodeFree(prnT);
  }

#endif

  if (RUNTEST) {
    resNodePtr prnT = NULL;
    xmlDocPtr pdocT = NULL;

    i++;
    printf("TEST %i in '%s:%i': resNodeReadDoc() = ",i,__FILE__,__LINE__);

    prnT = resNodeDirNew(BAD_CAST TESTPREFIX "option/pie/text/config.cxp");
    if (resNodeReadDoc(NULL) == NULL
	&& (pdocT = resNodeReadDoc(prnT)) != NULL) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error\n");
    }
    xmlFreeDoc(pdocT);
    resNodeFree(prnT);
  }


  printf("\nResult in '%s': %i/%i OK\n\n",__FILE__,n_ok,i);

  return (i - n_ok);
}
