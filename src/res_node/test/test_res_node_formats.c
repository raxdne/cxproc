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

/*! test code for contexts in module fs
 */
int
resNodeTestFormats(void)
{
  int n_ok;
  int i;
  xmlChar *pucModuleTestReport = xmlStrdup(BAD_CAST"\n");
  xmlChar mucTestLabel[BUFFER_LENGTH];
  xmlChar mucTestResult[BUFFER_LENGTH];

  n_ok=0;
  i=0;

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
    else if ((pndT = resNodeToDOM(prnT, RN_INFO_MAX)) == NULL || (j = domNumberOfChild(pndT, NULL)) != 19) {
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

    //domPutNodeString(stderr, BAD_CAST "resNodeToDOM(): ", pndT);
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
	if ((pucT = resNodeListToXmlStr(prnT, RN_INFO_MAX))) {
	  //puts((const char*)pucT);
	  n_ok++;
	  printf("OK\n");
	}
	else {
	  printf("Error resNodeListToXmlStr() ...\n");
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


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    xmlChar *pucT = NULL;
    xmlNodePtr pndT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': resNodeTo*() XML file = ", i, __FILE__, __LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "xml/dummy-a.xml")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew()\n");
    }
    else if (resNodeHasDetails(prnT, RN_INFO_STAT) == TRUE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeHasDetails()\n");
    }
    else if (resNodeReadStatus(prnT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeReadStatus()\n");
    }
    else if (resNodeHasDetails(prnT, RN_INFO_STAT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeHasDetails()\n");
    }
    else if (resNodeUpdate(prnT, RN_INFO_MAX, NULL, NULL) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeUpdate()\n");
    }
    else if (resNodeHasDetails(prnT, RN_INFO_STAT | RN_INFO_STRUCT | RN_INFO_INFO) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeHasDetails()\n");
    }
    else if (resNodeUpdate(prnT, RN_INFO_MAX, NULL, NULL) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeUpdate()\n");
    }
    else if ((pndT = resNodeToDOM(prnT, RN_INFO_MAX)) == NULL || domNumberOfChild(pndT, NULL) != 1) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeToDOM()\n");
    }
    else if ((pucT = resNodeToPlain(prnT, RN_INFO_MAX)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeToPlain() = %i\n",xmlStrlen(pucT));
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    //domPutNodeString(stderr, BAD_CAST "resNodeToDOM(): ", pndT);
    //puts((const char *)pucT);
    xmlFree(pucT);
    xmlFreeNode(pndT);
    resNodeFree(prnT);
  }


  if (SKIPTEST) {
    resNodePtr prnT = NULL;
    xmlNodePtr pndT = NULL;

    i++;
    printf("TEST %i in '%s:%i': reads content of archive file context = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "xml/test-xml-zip.docx")) == NULL) {
      printf("Error 2 resNodeGetSize()\n");
    }
    else if ((pndT = resNodeToDOM(prnT,RN_INFO_MAX)) == NULL) {
      printf("Error resNodeToDOM()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST "ODT ", pndT);
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    xmlChar *pucT = NULL;
    xmlChar *pucTT = NULL;
    resNodePtr prnT = NULL;

    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': rebuild existing filesystem context of a file = ",i,__FILE__,__LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "thread//")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew()\n");
    }
    else if ((pucT = resNodeToCSV(prnT,RN_OUT_MIN)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeToCSV(prnT,RN_OUT_MIN)\n");
    }
    else if ((pucTT = resNodeToCSV(prnT,RN_OUT_MAX)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeToCSV(prnT,RN_OUT_MAX)\n");
    }
#if 0
    else if (resNodeListDumpRecursively(stdout, prnT, RN_OUT_MIN, resNodeToCSV) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeListDumpRecursively()\n");
    }
#endif
    else {
      n_ok++;
    }
    //puts((const char *)pucT);
    //puts((const char *)pucTT);

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    xmlFree(pucTT);
    xmlFree(pucT);
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    xmlNodePtr pndTest = NULL;
    resNodePtr prnT = NULL;
    xmlChar *pucT;
    
    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': resNodeToDOM() = ",i,__FILE__,__LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';


    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "plain/test-plain-004.xml")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error domGetFirstChild()\n");
    }
    else if (resNodeReadStatus(prnT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeReadStatus()\n");
    }
    else if (resNodeUpdate(prnT, RN_INFO_MAX, NULL, NULL) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeUpdate()\n");
    }
    else if ((pndTest = resNodeToDOM(prnT,RN_INFO_MAX)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeToDOM()\n");
    }
    else if (IS_NODE_FILE(pndTest) == FALSE || IS_NODE_PIE(pndTest->children) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error PIE\n");
    }
    else if (domGetPropValuePtr(pndTest,BAD_CAST"object") != NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error object\n");
    }
    else if (xmlStrEqual(domGetPropValuePtr(pndTest,BAD_CAST"type"), BAD_CAST resNodeGetMimeTypeStr(prnT)) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error domGetFirstChild()\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    //domPutNodeString(stderr, BAD_CAST "resNodeToDOM(): ", pndTest);
    xmlFreeNodeList(pndTest);
    resNodeFree(prnT);
  }

  
  if (SKIPTEST) {
    xmlNodePtr pndTest = NULL;
    resNodePtr prnT = NULL;
    xmlChar *pucT;
    
    i++;
    xmlStrPrintf(mucTestLabel,BUFFER_LENGTH,"\nTEST %i in '%s:%i': resNodeReadStatus() = ",i,__FILE__,__LINE__);
    fputs((const char *)mucTestLabel,stderr);
    mucTestResult[0] = '\0';


    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "dir\\test-cad-creo.drw.11")) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeDirNew()\n");
    }
    else if (resNodeReadStatus(prnT) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeReadStatus()\n");
    }
    else if (resNodeUpdate(prnT, RN_INFO_MAX, NULL, NULL) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeUpdate()\n");
    }
    else if ((pndTest = resNodeToDOM(prnT,RN_INFO_MAX)) == NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error resNodeToDOM()\n");
    }
    else if (IS_NODE_FILE(pndTest) == FALSE || pndTest->children != NULL) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error PIE\n");
    }
    else if (xmlStrEqual(domGetPropValuePtr(pndTest,BAD_CAST"type"), BAD_CAST resNodeGetMimeTypeStr(prnT)) == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error type\n");
    }
    else if (xmlStrEqual(domGetPropValuePtr(pndTest,BAD_CAST"object"), BAD_CAST "TESTCONTENT.DRW") == FALSE) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error object\n");
    }
    else {
      n_ok++;
    }

    if (xmlStrlen(mucTestResult) > 0) {
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestLabel);
      pucModuleTestReport = xmlStrcat(pucModuleTestReport,mucTestResult);
    }
    
    //domPutNodeString(stderr, BAD_CAST "resNodeToDOM(): ", pndTest);
    xmlFreeNodeList(pndTest);
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    xmlChar* pucT;
    xmlNodePtr pndT;
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': resNodeFromDOM() = ", i, __FILE__, __LINE__);

    if ((pndT = xmlNewNode(NULL, NAME_FILE)) == NULL
      || xmlSetProp(pndT,BAD_CAST"name", BAD_CAST"dummy.xml") == NULL
      || xmlSetProp(pndT,BAD_CAST"size", BAD_CAST"11111") == NULL
      || xmlSetProp(pndT,BAD_CAST"type", BAD_CAST"text/xml") == NULL
      || xmlSetProp(pndT,BAD_CAST"mtime", BAD_CAST"1661019995") == NULL
      || xmlSetProp(pndT,BAD_CAST"read", BAD_CAST"yes") == NULL
      || xmlSetProp(pndT,BAD_CAST"write", BAD_CAST"no") == NULL
      || xmlSetProp(pndT,BAD_CAST"execute", BAD_CAST"no") == NULL
      || xmlSetProp(pndT,BAD_CAST"prefix", BAD_CAST TEMPPREFIX) == NULL
      || xmlNewChild(pndT,NULL,BAD_CAST"base64", BAD_CAST"PD94bWwgdmVyc2lvbj0iMS4wIj8+CjxkdW1teS1hPgogIDxkdW1teS1hLWNoaWxkLz4KPC9kdW1teS1hPgo=") == NULL) {
      printf("Error xmlNewChild()\n");
    }
    else if ((prnT = resNodeFromDOM(pndT,0)) == NULL) {
      printf("Error resNodeFromDOM()\n");
    }
    else if (resNodePutContent(prnT) == FALSE) {
      printf("Error resNodePutContent()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST "resNodeFromDOM(): ", pndT);
    //pucT = resNodeListToPlain(prnT,RN_INFO_MAX); fputs((const char *)pucT,stderr); xmlFree(pucT);
    resNodeFree(prnT);
    xmlFreeNode(pndT);
  }


  xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"\nResult in '%s': %i/%i OK\n",__FILE__,n_ok,i);
  fputs((const char *)mucTestResult,stderr);
  fputs((const char *)pucModuleTestReport,stderr);

  xmlFree(pucModuleTestReport);
  return (i - n_ok);
}
