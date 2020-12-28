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
domTest(void)
{
  int n_ok;
  int i;

  n_ok=0;
  i=0;


  if (RUNTEST) {
    xmlDocPtr pdocTest;
    xmlNodePtr pndTest;
    xmlNsPtr pnsT = NULL;

    i++;
    printf("TEST %i in '%s:%i': domSetNsXsl() = ", i, __FILE__, __LINE__);

    pdocTest = xmlParseFile(TESTPREFIX "option/pie/text/test-pie-14.pie");
    pndTest = xmlDocGetRootElement(pdocTest);

    if (pnsXsl != NULL) {
      printf("Error 1 domSetNsXsl()\n");
    }
    else if ((pnsT = domGetNsXsl()) != pnsXsl && pnsXsl == NULL) {
      printf("Error 2 domSetNsXsl()\n");
    }
    else if (domSetNsRecursive(pndTest, pnsT) == FALSE) {
      printf("Error 3 domSetNsRecursive()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutDocString(stderr,BAD_CAST"Result XSL Document",pdocTest);
    xmlFreeDoc(pdocTest);
  }


  if (RUNTEST) {
    xmlNodePtr pndTest;

    i++;
    printf("TEST %i in '%s:%i': isEmptyTextNode() = ",i,__FILE__,__LINE__);

    pndTest = xmlNewText(BAD_CAST"\t\n\n");
    if (pndTest != NULL && isEmptyTextNode(pndTest) == TRUE && isEmptyTextNode(NULL) == FALSE) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error\n");
    }
    xmlFreeNode(pndTest);
  }

  if (RUNTEST) {
    xmlNodePtr pndTest;

    i++;
    printf("TEST %i in '%s:%i': isValidNodeType() = ",i,__FILE__,__LINE__);

    pndTest = xmlNewText(BAD_CAST"\t\n\n");
    if (pndTest != NULL && isValidNodeType(pndTest) == TRUE && isValidNodeType(NULL) == FALSE) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error\n");
    }
    xmlFreeNode(pndTest);
  }


  if (RUNTEST) {
    xmlDocPtr pdocTest;
    xmlNodePtr pndTest;

    i++;
    printf("TEST %i in '%s:%i': domNodeIsDocRoot() = ",i,__FILE__,__LINE__);

    pdocTest = xmlParseFile(TESTPREFIX "option/pie/text/test-pie-14.pie");
    pndTest = xmlDocGetRootElement(pdocTest);
    if (pdocTest != NULL && pndTest != NULL && domNodeIsDocRoot(pndTest) == TRUE && domNodeIsDocRoot(NULL) == FALSE) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error\n");
    }
    xmlFreeDoc(pdocTest);
  }


  if (RUNTEST) {
    xmlDocPtr pdocTest;

    i++;
    printf("TEST %i in '%s:%i': domDocIsHtml() = ",i,__FILE__,__LINE__);

    pdocTest = xmlParseFile(TESTPREFIX "xhtml/test.html");
    if (pdocTest != NULL && domDocIsHtml(pdocTest) == TRUE && domDocIsHtml(NULL) == FALSE) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error\n");
    }
    xmlFreeDoc(pdocTest);
  }


  if (RUNTEST) {
    xmlNodePtr pndTest;

    i++;
    printf("TEST %i in '%s:%i': domPropIsEqual() + domGetPropFlag() = ",i,__FILE__,__LINE__);

    pndTest = xmlNewNode(NULL,NAME_PIE_PAR);
    if (domPropIsEqual(pndTest, BAD_CAST"valid", NULL) == FALSE
	&& domPropIsEqual(pndTest, NULL, NULL) == FALSE
	&& domPropIsEqual(pndTest, BAD_CAST"valid", BAD_CAST"yes") == FALSE
	&& domGetPropFlag(pndTest,BAD_CAST "valid",TRUE) == TRUE
	&& domGetPropFlag(pndTest,BAD_CAST "valid",FALSE) == FALSE) {
      xmlChar *pucTT = xmlStrdup(BAD_CAST"no");;
      
      domSetPropEat(pndTest,BAD_CAST"valid",pucTT);
      if (domPropIsEqual(pndTest, BAD_CAST"valid", NULL) == FALSE
	  && domPropIsEqual(pndTest, NULL, NULL) == FALSE
	  && domPropIsEqual(pndTest, BAD_CAST"valid", BAD_CAST"yes") == FALSE
	  && domPropIsEqual(pndTest, BAD_CAST"valid", BAD_CAST"no") == TRUE
	  && domGetPropFlag(pndTest,BAD_CAST "valid",TRUE) == FALSE
	  && domGetPropFlag(pndTest,BAD_CAST "valid",FALSE) == FALSE) {
	
	domUnsetPropAll(pndTest);
	if (domPropIsEqual(pndTest, BAD_CAST"valid", NULL) == FALSE
	    && domPropIsEqual(pndTest, NULL, NULL) == FALSE
	    && domPropIsEqual(pndTest, BAD_CAST"valid", BAD_CAST"yes") == FALSE
	    && domPropIsEqual(pndTest, BAD_CAST"valid", BAD_CAST"no") == FALSE
	    && domGetPropFlag(pndTest,BAD_CAST "valid",TRUE) == TRUE
	    && domGetPropFlag(pndTest,BAD_CAST "valid",FALSE) == FALSE) {
	  n_ok++;
	  printf("OK\n");
	}
	else {
	  printf("Error\n");
	}
      }
      else {
	printf("Error\n");
      }
    }
    else {
      printf("Error\n");
    }
    xmlFreeNode(pndTest);
  }


  if (RUNTEST) {
    xmlDocPtr pdocTest;
    xmlNodePtr pndTest;

    i++;
    printf("TEST %i in '%s:%i': domNodeIsDocRoot() = ",i,__FILE__,__LINE__);

    pdocTest = xmlParseFile(TESTPREFIX "option/pie/text/test-pie-14.pie");
    pndTest = xmlDocGetRootElement(pdocTest);
    if (pdocTest != NULL && pndTest != NULL && pndTest->children != NULL
	&& domNodeIsDescendant(pndTest,pndTest->children) == TRUE
	&& domNodeIsDescendant(pndTest,NULL) == FALSE && domNodeIsDescendant(NULL,pndTest->children) == FALSE) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error\n");
    }
    xmlFreeDoc(pdocTest);
  }


  if (RUNTEST) {
    xmlNodePtr pndA;
    xmlNodePtr pndB;

    i++;
    printf("TEST %i in '%s:%i': domNodesAreEqual() = ",i,__FILE__,__LINE__);

    pndA = xmlNewNode(NULL, NAME_PIE_PIE);
    xmlAddChild(pndA, xmlNewPI(NAME_PIE_ERROR, BAD_CAST"pre"));
    xmlNewChild(pndA, NULL, NAME_PIE_PAR, BAD_CAST"par 1");
    xmlNewChild(pndA, NULL, NAME_PIE_PAR, BAD_CAST"par 2");
    xmlNewChild(pndA, NULL, NAME_PIE_PAR, BAD_CAST"par 3");
    xmlAddChild(pndA, xmlNewPI(NAME_PIE_ERROR, BAD_CAST"post"));

    pndB = xmlNewNode(NULL, NAME_PIE_PIE);
    xmlAddChild(pndB, xmlNewPI(NAME_PIE_ERROR, BAD_CAST"pre"));
    xmlNewChild(pndB, NULL, NAME_PIE_PAR, BAD_CAST"par 1");
    xmlNewChild(pndB, NULL, NAME_PIE_PAR, BAD_CAST"par 2");
    xmlNewChild(pndB, NULL, NAME_PIE_PAR, BAD_CAST"par 3");
    xmlAddChild(pndB, xmlNewPI(NAME_PIE_ERROR, BAD_CAST"post"));

    if (pndA != NULL && pndB != NULL
	&& domNodesAreEqual(pndA,pndB) == TRUE
	&& domNodesAreEqual(pndA,NULL) == FALSE && domNodesAreEqual(NULL,pndB) == FALSE) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error\n");
    }
    xmlFreeNode(pndB);
    xmlFreeNode(pndA);
  }


  if (RUNTEST) {
    xmlDocPtr pdocTest;
    xmlNodePtr pndRoot;
    xmlNodePtr pndRelease;

    i++;
    printf("TEST %i in '%s:%i': domUnlinkNodeList(pndRelease) = ",i,__FILE__,__LINE__);

    pdocTest = xmlParseFile(TESTPREFIX "option/pie/text/test-pie-14.pie");
    pndRoot = xmlDocGetRootElement(pdocTest);
    pndRelease = pndRoot->children->children->next;

    domUnlinkNodeList(pndRelease);
    if (pndRoot->children->children->next==NULL && pndRoot->children->last==pndRoot->children->children
	&& pndRelease->parent==NULL && pndRelease->doc==NULL) {
      xmlNodePtr pndT;
      n_ok++;
      pndT = xmlNewNode(NULL,NAME_PIE);
      xmlAddChildList(pndT,pndRelease);
      xmlFreeNode(pndT);
      printf("OK\n");
    }
    else {
      printf("Error\n");
    }
    xmlFreeDoc(pdocTest);
  }

  if (RUNTEST) {
    xmlDocPtr pdocTest;
    xmlNodePtr pndRoot;
    xmlNodePtr pndRelease;

    i++;
    printf("TEST %i in '%s:%i': domUnlinkNodeList(pndRelease) = ",i,__FILE__,__LINE__);

    pdocTest = xmlParseFile(TESTPREFIX "option/pie/text/test-pie-14.pie");
    pndRoot = xmlDocGetRootElement(pdocTest);
    pndRelease = pndRoot->children->children->next;

    domUnlinkNodeList(pndRelease);
    if (pndRoot->children->children->next==NULL && pndRoot->children->last==pndRoot->children->children
	&& pndRelease->parent==NULL && pndRelease->doc==NULL) {
      xmlNodePtr pndT;
      n_ok++;
      pndT = xmlNewNode(NULL,NAME_PIE);
      xmlAddChildList(pndT,pndRelease);
      xmlFreeNode(pndT);
      printf("OK\n");
    }
    else {
      printf("Error\n");
    }
    xmlFreeDoc(pdocTest);
  }

  if (RUNTEST) {
    xmlDocPtr pdocTestA;
    xmlNodePtr pndRootA;

    i++;
    printf("TEST %i in '%s:%i': domIsNodeInTree() = ",i,__FILE__,__LINE__);

    pdocTestA = xmlParseFile(TESTPREFIX "option/pie/text/test-pie-14.pie");
    pndRootA = xmlDocGetRootElement(pdocTestA);

    if (domIsNodeInTree(NULL,NULL) == FALSE
	&& domIsNodeInTree(pndRootA,NULL) == FALSE
	&& domIsNodeInTree(NULL,pndRootA) == FALSE
	//
	&& domIsNodeInTree(pndRootA,pndRootA) == TRUE
	// properties
	//&& domIsNodeInTree(pndRootA,xmlHasProp(pndRootA,BAD_CAST"class")) == TRUE
	// childs
	&& domIsNodeInTree(pndRootA,pndRootA->children) == TRUE
	&& domIsNodeInTree(pndRootA,pndRootA->children->children->children) == TRUE) {
      xmlDocPtr pdocTestB;
      xmlNodePtr pndRootB;

      pdocTestB = xmlParseFile(TESTPREFIX "option/pie/text/test-pie-5.pie");
      pndRootB = xmlDocGetRootElement(pdocTestB);

      if (domIsNodeInTree(pndRootA,pndRootB) == FALSE) {
	n_ok++;
	printf("OK\n");
      }
      else {
	printf("Error\n");
      }
      xmlFreeDoc(pdocTestB);
    }
    else {
      printf("Error\n");
    }
    xmlFreeDoc(pdocTestA);
  }

  if (RUNTEST) {
    xmlDocPtr pdocTestA;
    xmlNodePtr pndRootA;

    i++;
    printf("TEST %i in '%s:%i': domIsTreeOverlapping() = ", i, __FILE__, __LINE__);

    pdocTestA = xmlParseFile(TESTPREFIX "option/pie/text/test-pie-14.pie");
    pndRootA = xmlDocGetRootElement(pdocTestA);

    if (domIsTreeOverlapping(NULL, NULL) == FALSE
      && domIsTreeOverlapping(pndRootA, NULL) == FALSE
      && domIsTreeOverlapping(NULL, pndRootA) == FALSE
      //
      && domIsTreeOverlapping(pndRootA, pndRootA) == TRUE
      // properties
      //&& domIsTreeOverlapping(pndRootA, xmlHasProp(pndRootA, BAD_CAST"class")) == TRUE
      // childs
      && domIsTreeOverlapping(pndRootA, pndRootA->children) == TRUE
      && domIsTreeOverlapping(pndRootA, pndRootA->children->children->children) == TRUE) {
      xmlDocPtr pdocTestB;
      xmlNodePtr pndRootB;

      pdocTestB = xmlParseFile(TESTPREFIX "option/pie/text/test-pie-5.pie");
      pndRootB = xmlDocGetRootElement(pdocTestB);

      if (domIsTreeOverlapping(pndRootA, pndRootB) == FALSE) {
	n_ok++;
	printf("OK\n");
      }
      else {
	printf("Error\n");
      }
      xmlFreeDoc(pdocTestB);
    }
    else {
      printf("Error\n");
    }
    xmlFreeDoc(pdocTestA);
  }

  if (RUNTEST) {
    xmlDocPtr pdocTest;
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': domChangeURL() = ", i, __FILE__, __LINE__);

    prnT = resNodeDirNew(BAD_CAST TESTPREFIX "xml/");
    pdocTest = xmlParseFile(TESTPREFIX "xsl/TestValidate.xsl");
    if (pdocTest) {
      domChangeURL(pdocTest, prnT);
      if (xmlStrEqual(pdocTest->URL, resNodeGetURI(prnT))) {
	n_ok++;
	printf("OK\n");
      }
      else {
	printf("Error\n");
      }
    }
    else {
      printf("Error\n");
    }

    xmlFreeDoc(pdocTest);
    resNodeFree(prnT);
  }

  if (RUNTEST) {
    xmlDocPtr pdocTest;
    xmlNodePtr pndRoot;
    xmlNodePtr pndT;

    i++;
    printf("TEST %i in '%s:%i': domGetFollowingNode() = ", i, __FILE__, __LINE__);

    pdocTest = xmlParseFile(TESTPREFIX "option/pie/text/test-pie-14.pie");
    pndRoot = xmlDocGetRootElement(pdocTest);
    pndT = pndRoot->children;
    domUnlinkNodeList(pndT);
    if (pndRoot->children == NULL && pndT->parent == NULL && domIsTreeOverlapping(pndRoot, pndT) == FALSE) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error\n");
    }

    xmlFreeNodeList(pndT);
    xmlFreeDoc(pdocTest);
  }

  if (RUNTEST) {
    xmlDocPtr pdocTest;
    xmlNodePtr pndRoot;
    xmlNodePtr pndOld;
    xmlNodePtr pndCur;

    i++;
    printf("TEST %i in '%s:%i': domReplaceNodeList() = ", i, __FILE__, __LINE__);

    pdocTest = xmlParseFile(TESTPREFIX "option/pie/text/test-pie-14.pie");
    pndRoot = xmlDocGetRootElement(pdocTest);
    pndOld = pndRoot->children->children->next;

    pndCur = xmlNewNode(NULL, NAME_PIE_PIE);
    xmlAddChild(pndCur, xmlNewPI(NAME_PIE_ERROR, BAD_CAST"pre"));
    xmlNewChild(pndCur, NULL, NAME_PIE_PAR, BAD_CAST"par 1");
    xmlNewChild(pndCur, NULL, NAME_PIE_PAR, BAD_CAST"par 2");
    xmlNewChild(pndCur, NULL, NAME_PIE_PAR, BAD_CAST"par 3");
    xmlAddChild(pndCur, xmlNewPI(NAME_PIE_ERROR, BAD_CAST"post"));

    //domPutDocString(stderr,pdocTest,BAD_CAST"domReplaceNodeList()");
    domReplaceNodeList(pndOld, pndCur->children);
    xmlNewChild(pndRoot->children, NULL, NAME_META, NULL);
    //domPutDocString(stderr,pdocTest,BAD_CAST"domReplaceNodeList()");

    if (pndCur->children == NULL && pndCur->parent == NULL && domIsTreeOverlapping(pndRoot, pndCur) == FALSE) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error\n");
    }
    xmlFreeNode(pndCur);
    xmlFreeNode(pndOld);
    xmlFreeDoc(pdocTest);
  }

  if (RUNTEST) {
    xmlDocPtr pdocTest;
    xmlNodePtr pndRoot;

    i++;
    printf("TEST %i in '%s:%i': domGetFollowingNode() = ", i, __FILE__, __LINE__);

    pdocTest = xmlParseFile(TESTPREFIX "option/pie/text/test-pie-14.pie");
    pndRoot = xmlDocGetRootElement(pdocTest);

    if (domGetFollowingNode(NULL, NAME_PIE_HEADER) == NULL && domGetFollowingNode(pndRoot, NULL) == NULL
      && domGetFollowingNode(pndRoot, NAME_PIE_HEADER) != NULL && IS_ROOT(domGetFollowingNode(pndRoot, BAD_CAST"hhhh"))) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error\n");
    }

    xmlFreeDoc(pdocTest);
  }

  if (RUNTEST) {
    xmlDocPtr pdocT;
    xmlNodePtr pndRoot;

    i++;
    printf("TEST %i in '%s:%i': domSetPropFileXpath() = ",i,__FILE__,__LINE__);

    pdocT = xmlParseFile(TESTPREFIX "option/pie/text/test-pie-14.pie");
    pndRoot = xmlDocGetRootElement(pdocT);
    domSetPropFileXpath(pndRoot,BAD_CAST"xpath",NULL);
    if (domGetPropValuePtr(pndRoot->children,BAD_CAST"xpath")) {
      //domPutDocString(stderr,pdocT,BAD_CAST"cxpAddXpath()");
      n_ok++;
      printf("OK\n");
      xmlFreeDoc(pdocT);
    }
    else {
      printf("Error \n");
    }
  }

  if (RUNTEST) {
    xmlDocPtr pdocTest;
    xmlNodePtr pndRoot;

    i++;
    printf("TEST %i in '%s:%i': domSetPropFileLocator() and domSetPropFileXpath() = ",i,__FILE__,__LINE__);

    pdocTest = xmlParseFile(TESTPREFIX "option/pie/text/test-pie-14.pie");
    pndRoot = xmlDocGetRootElement(pdocTest);

    domSetPropFileLocator(pndRoot,BAD_CAST pdocTest->URL);
    domSetPropFileXpath(pndRoot,BAD_CAST"fxpath",BAD_CAST"//");
    if (xmlStrEqual(domGetPropValuePtr(pndRoot->children,BAD_CAST"flocator"),pdocTest->URL)) {
      xmlSaveFormatFileEnc(TEMPPREFIX "test-pie-14-locator.pie",pdocTest,"UTF-8",1);
      domUnsetPropFileLocator(pndRoot);
      xmlSaveFormatFileEnc(TEMPPREFIX "test-pie-14-locator-removed.pie",pdocTest,"UTF-8",1);
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error\n");
    }
    xmlFreeDoc(pdocTest);
  }


  if (RUNTEST) {
    xmlNodePtr pndTop;
    xmlNodePtr pndTest;
    xmlNodePtr pndTestFirst;

    i++;
    printf("TEST %i in '%s:%i': domGetFirstChild() = ", i, __FILE__, __LINE__);

    pndTop = xmlNewNode(NULL, NAME_PIE_PIE);
    xmlAddChild(pndTop, xmlNewPI(NAME_PIE_ERROR, BAD_CAST"pre"));
    pndTestFirst = xmlNewChild(pndTop, NULL, NAME_PIE_PAR, BAD_CAST"par 1");
    xmlNewChild(pndTop, NULL, NAME_PIE_PAR, BAD_CAST"par 2");
    pndTest = xmlNewChild(pndTop, NULL, NAME_PIE_LIST, BAD_CAST"list 1");
    xmlNewChild(pndTop, NULL, NAME_PIE_PAR, BAD_CAST"par 3");
    xmlAddChild(pndTop, xmlNewPI(NAME_PIE_ERROR, BAD_CAST"post"));

    if (pndTop != NULL && pndTop->children != NULL && pndTop->parent == NULL
	&& domGetFirstChild(NULL,NAME_PIE_PAR) == NULL
	&& domGetFirstChild(pndTop,NAME_PIE_HEADER) == NULL
	&& domGetFirstChild(pndTop,NULL) == pndTestFirst
	&& domGetFirstChild(pndTop,NAME_PIE_LIST) == pndTest) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error\n");
    }
    xmlFreeNodeList(pndTop);
  }


  if (RUNTEST) {
    xmlNodePtr pndTop;
    xmlNodePtr pndTest;
    xmlNodePtr pndTestFirst;

    i++;
    printf("TEST %i in '%s:%i': domGetNextNode() = ", i, __FILE__, __LINE__);

    pndTop = xmlNewNode(NULL, NAME_PIE_PIE);
    xmlAddChild(pndTop, xmlNewPI(NAME_PIE_ERROR, BAD_CAST"pre"));
    xmlNewChild(pndTop, NULL, NAME_PIE_PAR, BAD_CAST"par 1");
    pndTestFirst = xmlNewChild(pndTop, NULL, NAME_PIE_PAR, BAD_CAST"par 2");
    xmlNewChild(pndTop, NULL, NAME_PIE_LIST, BAD_CAST"list 1");
    pndTest = xmlNewChild(pndTop, NULL, NAME_PIE_PAR, BAD_CAST"par 3");
    xmlAddChild(pndTop, xmlNewPI(NAME_PIE_ERROR, BAD_CAST"post"));

    if (pndTop != NULL && pndTop->children != NULL && pndTop->parent == NULL
	&& domGetNextNode(NULL,NAME_PIE_PAR) == NULL
	&& domGetNextNode(pndTestFirst,NAME_PIE_HEADER) == NULL
	&& domGetNextNode(pndTestFirst,NULL) == pndTest
	&& domGetNextNode(pndTestFirst,NAME_PIE_PAR) == pndTest) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error\n");
    }
    xmlFreeNodeList(pndTop);
  }


  if (RUNTEST) {
    xmlDocPtr pdocT;
    xmlNodePtr pndRoot;
    xmlNsPtr pnsXsl;
    xmlNodePtr pndXslNew;
    xmlNodePtr pndNew;

    i++;
    printf("TEST %i in '%s:%i': embedded XSL tree = ",i,__FILE__,__LINE__);

    pndRoot = xmlNewNode(NULL,NAME_XML);
    pndXslNew = xmlNewChild(pndRoot,NULL,BAD_CAST "stylesheet",NULL);
    if (pndXslNew) {
      /*\todo domChangeURL(pdocResult,cxpCtxtLocationGet(pccArg)); ?? */

      /* create local namespace for XSL */
      pnsXsl = xmlNewNs(pndXslNew,XSLT_NAMESPACE,BAD_CAST "xsl");
      xmlSetNs(pndXslNew,pnsXsl);
      xmlSetProp(pndXslNew, BAD_CAST "version", BAD_CAST "1.0");

      pndNew = xmlNewChild(pndXslNew,pnsXsl,BAD_CAST"output",NULL);
      xmlSetProp(pndNew, BAD_CAST "method", BAD_CAST "text");
      pndNew = xmlNewChild(pndXslNew,pnsXsl,BAD_CAST"template",NULL);
      xmlSetProp(pndNew, BAD_CAST "match", BAD_CAST "/");
      /*  */
      pndNew = xmlNewChild(pndNew,pnsXsl,BAD_CAST"value-of",NULL);
      xmlSetProp(pndNew, BAD_CAST "select", BAD_CAST"*");

      /* this is a xsl:stylesheet tree */
      if ((pdocT = domDocFromNodeNew(pndXslNew)) != NULL && xmlStrEqual(domGetXslOutputMethod(pdocT),BAD_CAST"text")) {
	//domPutDocString(stderr,pdocT,BAD_CAST"Result XSL Document");
	n_ok++;
	printf("OK\n");
      }
      else {
	printf("Error \n");
      }
      xmlFreeDoc(pdocT);
      xmlFreeNode(pndRoot);
    }
  }


  if (RUNTEST) {
    xmlChar *pucT = BAD_CAST"ABCDEF";
    xmlNodePtr pndTest;

    i++;
    printf("TEST %i in '%s:%i': domNodeGetContentPtr() = ",i,__FILE__,__LINE__);

    pndTest = xmlNewText(pucT);
    if (pndTest != NULL && xmlStrEqual(domNodeGetContentPtr(pndTest),pucT) && domNodeGetContentPtr(NULL) == NULL) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error\n");
    }
    xmlFreeNode(pndTest);
  }

  if (RUNTEST) {
    xmlChar *pucT = BAD_CAST"UVWXYZ";
    xmlChar *pucTT = NULL;
    xmlNodePtr pndTest;
    xmlNodePtr pndTop;

    i++;
    printf("TEST %i in '%s:%i': domNodeEatContent() = ",i,__FILE__,__LINE__);

    pndTop = xmlNewNode(NULL,NAME_PIE_PAR);
    pndTest = xmlNewTextChild(pndTop,NULL,BAD_CAST"b",pucT);
    if (pndTest != NULL
	&& (pucTT = domNodeEatContent(pndTest)) != NULL && xmlStrEqual(pucTT,pucT) && domNodeGetContentPtr(pndTest) == NULL
	&& domNodeEatContent(NULL) == NULL) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error\n");
    }
    xmlFree(pucTT);
    xmlFreeNode(pndTop);
  }

  
#if 0

  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': domAddNodeToError() = ",i,__FILE__,__LINE__);

    if (domAddNodeToError()) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error\n");
    }
  }



  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': domSetNsRecursive() = ",i,__FILE__,__LINE__);

    if (domSetNsRecursive()) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error\n");
    }
  }


  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': domUnsetNs() = ",i,__FILE__,__LINE__);

    if (domUnsetNs()) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error\n");
    }
  }

#endif

      
  if (RUNTEST) {
    /* TEST:
    */
    xmlDocPtr pdocResult = NULL;
    xmlChar *xpath = (xmlChar*) "//p[not(@valid='no')]";
    xmlNodeSetPtr nodeset;
    xmlXPathObjectPtr result = NULL;

    i++;
    printf("TEST %i in '%s:%i': XPath nodeset ", i, __FILE__, __LINE__);

    if ((pdocResult = xmlReadFile((const char *)TESTPREFIX "option/pie/text/test-pie-14.pie", NULL, 0)) == NULL) {
      printf("Error 1 xmlReadFile()\n");
    }
    else if ((result = domGetXPathNodeset(NULL, NULL)) != NULL) {
      printf("Error 1 domGetXPathNodeset()\n");
    }
    else if ((result = domGetXPathNodeset(pdocResult, NULL)) != NULL) {
      printf("Error 2 domGetXPathNodeset()\n");
    }
    else if ((result = domGetXPathNodeset(pdocResult, xpath)) == NULL
      || (nodeset = result->nodesetval) == NULL) {
      printf("Error 3 domGetXPathNodeset()\n");
    }
    else if (nodeset->nodeNr != 2) {
      printf("Error 4 domGetXPathNodeset()\n");
    }
    else {
      int i;
      BOOL_T fResult;

      for (fResult = TRUE, i=0; fResult && i < nodeset->nodeNr; i++) {
	fResult = IS_NODE_PIE_PAR(nodeset->nodeTab[i]);
      }

      if (fResult) {
	n_ok++;
	printf("OK\n");
      }
      else {
	printf("Error 5 domGetXPathNodeset()\n");
      }

    }
    xmlXPathFreeObject(result);
    xmlFreeDoc(pdocResult);
  }


#ifdef HAVE_PIE
  if (RUNTEST) {
    /* TEST:
    */
    xmlDocPtr pdocResult = NULL;
    xmlChar *pucXpathCheck = (xmlChar*) "//*[@w]";
    xmlChar *pucPattern = (xmlChar*) "//*[name()='p']";
    xmlNodeSetPtr nodeset;
    xmlXPathObjectPtr result = NULL;
    xmlNodePtr pndRoot;

    i++;
    printf("TEST %i in '%s:%i': XPath nodeset ", i, __FILE__, __LINE__);

    if ((pdocResult = xmlParseFile(TESTPREFIX "option/pie/text/test-pie-14.pie")) == NULL
      || (pndRoot = xmlDocGetRootElement(pdocResult)) == NULL) {
      printf("Error 1 xmlParseFile()\n");
    }
    else if (domWeightXPathInDoc(pdocResult, pucPattern) == FALSE) {
      printf("Error 1 domWeightXPathInDoc()\n");
    }
    else if ((result = domGetXPathNodeset(pdocResult, pucXpathCheck)) == NULL
      || (nodeset = result->nodesetval) == NULL) {
      printf("Error 3 domGetXPathNodeset()\n");
    }
    else if (nodeset->nodeNr != 5) {
      printf("Error 4 domGetXPathNodeset()\n");
    }
    else {
	n_ok++;
	printf("OK\n");
    }
    //CleanUpTree(pndRoot);
    //domPutDocString(stderr, BAD_CAST "domWeightXPathInDoc(): ", pdocResult);

    xmlXPathFreeObject(result);
    xmlFreeDoc(pdocResult);
  }
#endif

  
#ifdef HAVE_PCRE2
  if (RUNTEST) {
    xmlDocPtr pdocTest;
    xmlNodePtr pndRoot;
    xmlNodePtr pndT;

    i++;
    printf("TEST %i in '%s:%i': domNodeGrepNew() = ", i, __FILE__, __LINE__);

    pdocTest = xmlParseFile(TESTPREFIX "option/pie/text/test-pie-14.pie");
    pndRoot = xmlDocGetRootElement(pdocTest);

    if ((pndT = domNodeGrepNew(NULL,NULL)) != NULL) {
      printf("Error 2\n");
    }
    else if ((pndT = domNodeGrepNew(pndRoot,NULL)) != NULL) {
      printf("Error 3\n");
    }
    else if ((pndT = domNodeGrepNew(pndRoot,BAD_CAST"[E8]")) == NULL) {
      printf("Error 4\n");
    }
    else if (IS_NODE_GREP(pndT) == FALSE
	     || IS_NODE(pndT->children,"match") == FALSE
	     || IS_NODE(pndT->children->children,"pie") == FALSE
	     || pndT->children->children->children != NULL) {
      printf("Error 5\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    //domPutNodeString(stderr,BAD_CAST"domNodeGrepNew()",pndT);
    xmlFreeNode(pndT);
    xmlFreeDoc(pdocTest);
  }
#endif


  printf("Result in '%s': %i/%i OK\n\n", __FILE__, n_ok, i);

  return (i - n_ok);
}
/* end of domTest() */
