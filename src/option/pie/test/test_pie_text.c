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
pieTextTest(cxpContextPtr pccArg)
{
  int n_ok;
  int i;

  n_ok=0;
  i=0;

  CompileRegExpDefaults();

  if (RUNTEST) {
    xmlNodePtr pndPie;

    i++;
    printf("TEST %i in '%s:%i': import dummy = ", i, __FILE__, __LINE__);

    if ((pndPie = xmlNewNode(NULL, NAME_PIE_PIE)) == NULL) {
      printf("Error xmlNewNode()\n");
    }
    else if (ProcessImportNode(NULL, pccArg) == TRUE) {
      printf("Error 1 ProcessImportNode()\n");
    }
    else if (ProcessImportNode(NULL, NULL) == TRUE) {
      printf("Error 2 ProcessImportNode()\n");
    }
    else if (ProcessImportNode(pndPie, pccArg) == TRUE) {
      printf("Error 3 ProcessImportNode()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    xmlFreeNode(pndPie);
  }


  if (RUNTEST) {
    xmlDocPtr pdocT;
    xmlDocPtr pdocTT = NULL;
    xmlNodePtr pndPie;

    i++;
    printf("TEST %i in '%s:%i': import simple PIE xml file = ", i, __FILE__, __LINE__);

    if ((pdocT = xmlParseFile(TESTPREFIX "option/pie/text/test-pie-2e.pie")) == NULL) {
      printf("Error 1 xmlParseFile()\n");
    }
    else if ((pndPie = xmlDocGetRootElement(pdocT)) == NULL
      || IS_NODE_PIE(pndPie) == FALSE
      || domNumberOfChild(pndPie, NAME_PIE_SECTION) != 1) {
      printf("Error file structure\n");
    }
    else if ((pdocTT = pieProcessPieNode(pndPie, pccArg)) == NULL) {
      printf("Error pieProcessPieNode()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutDocString(stderr, BAD_CAST "split result", pdocTT);
    xmlFreeDoc(pdocT);
    xmlFreeDoc(pdocTT);
  }


  if (RUNTEST) {
    xmlDocPtr  pdocT;
    xmlNodePtr pndPie;
    xmlNodePtr pndT = NULL;

    i++;
    printf("TEST %i in '%s:%i': complex circular import = ", i, __FILE__, __LINE__);

    pndPie = xmlNewNode(NULL, NAME_PIE_PIE);
    pndT = xmlNewChild(pndPie, NULL, NAME_PIE_IMPORT, NULL);
    xmlSetProp(pndT, BAD_CAST "name", BAD_CAST TESTPREFIX "option/pie/text/circular/test-pie-circular-a.pie");

    if ((pdocT = pieProcessPieNode(pndPie, pccArg)) == NULL) {
      printf("Error 1 pieProcessPieNode()\n");
    }
    else if (domNumberOfChild(pndPie, NAME_PIE_SECTION) == 2) {
      printf("Error 2 pieProcessPieNode()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutDocString(stderr, BAD_CAST "split result", pdocT);
    xmlFreeDoc(pdocT);
    xmlFreeNode(pndPie);
  }


  if (RUNTEST) {
    xmlNodePtr pndPie;
    xmlChar *pucContent = BAD_CAST
      "\n"
      "sep=,\n"
      "* Test 2\n"
      "A,B,C\n"
      "D,E,F\n"
      "H,I,J\n"
      "\n"
      ;

    i++;
    printf("TEST %i in '%s:%i': parse CSV text and build tree = ", i, __FILE__, __LINE__);

    if ((pndPie = xmlNewNode(NULL, NAME_PIE_PIE)) == NULL) {
      printf("Error xmlNewNode()\n");
    }
    else if (ParsePlainBuffer(pndPie, pucContent, RMODE_TABLE) == FALSE) {
      printf("Error 1 ParsePlainBuffer()\n");
    }
    else if (domNumberOfChild(pndPie, NAME_PIE_BLOCK) != 1 || domNumberOfChild(pndPie->children, NAME_PIE_TABLE) != 1) {
      printf("Error 2 ParsePlainBuffer()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST "split result", pndPie);
    xmlFreeNode(pndPie);
  }


  if (RUNTEST) {
    xmlNodePtr pndPie;
    xmlChar *pucContent = BAD_CAST
      "\n"
      "* Test of Blocks\n"
      "\n"
      "#begin_of_pre\n"
      "AAAAAAAAAAAAAA\n"
      "#end_of_pre\n"
      "\n"
      "#begin_of_skip\n"
      "SKIP 1 SKIP 1 SKIP\n"
      "#end_of_skip\n"
      "\n"
      "#begin_of_csv\n"
      "H,I,J\n"
      "#end_of_csv\n"
      "\n"
      "<script>\n"
      "128 * 2\n"
      "</script>\n"
      "\n"
      "ABC def HIJ\n"
      "\n"
      "#import(\"def.txt\",root)\n"
      "\n"
      "KLM NOP QRS\n"
      "\n"
      ;

    i++;
    printf("TEST %i in '%s:%i': parse plain text and build tree = ", i, __FILE__, __LINE__);

    if ((pndPie = xmlNewNode(NULL, NAME_PIE_PIE)) == NULL) {
      printf("Error xmlNewNode()\n");
    }
    else if (ParsePlainBuffer(pndPie, pucContent, RMODE_PAR) == FALSE) {
      printf("Error 1 ParsePlainBuffer()\n");
    }
    else if (IS_NODE_PIE(pndPie) == FALSE || domNumberOfChild(pndPie, NAME_PIE_BLOCK) != 1 || domNumberOfChild(pndPie->children, NAME_PIE_SECTION) != 1) {
      printf("Error 2 ParsePlainBuffer()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST "split result", pndPie);
    xmlFreeNode(pndPie);
  }


  if (RUNTEST) {
    xmlChar *pucContent;
    xmlNodePtr pndPie = NULL;
    xmlNodePtr pndBlock;
    resNodePtr prnT = NULL;

    i++;
    printf("TEST %i in '%s:%i': parse plain text file and build tree = ", i, __FILE__, __LINE__);

    if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "option/pie/text/test-pie-loop-step-0.txt")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeGetContent(prnT, 1) == NULL || resNodeGetSize(prnT) < 1) {
      printf("Error content\n");
    }
    else if ((pndPie = xmlNewNode(NULL, NAME_PIE_PIE)) == NULL) {
      printf("Error xmlNewNode()\n");
    }
    else if (ParsePlainBuffer(pndPie, BAD_CAST resNodeGetContentPtr(prnT), RMODE_PAR) == FALSE) {
      printf("Error ParsePlainBuffer()\n");
    }
    else if ((pndBlock = pndPie->children) == NULL || IS_NODE_PIE_BLOCK(pndBlock) == FALSE) {
      printf("Error 1 tree\n");
    }
    else if (domNumberOfChild(pndBlock, NAME_PIE_SECTION) != 3) {
      printf("Error 2 tree\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST "split result", pndPie);
    xmlFreeNode(pndPie);

    resNodeFree(prnT);
  }


  if (RUNTEST) {
    xmlNodePtr pndPie;
    xmlChar *pucContent = BAD_CAST
      "\n"
      "* Test of Blocks script=\"Date();\"\n"
      "\n"
      "This is a Test: script=\"5*5;\" : postfix\n"
      "\n"
      "This is a Test: script=\"5*5;\" sep script=\"5*5*5;\" : postfix\n"
      "\n"
      "This is a Test: SCRIPT=\"5*5;\" : postfix\n"
      "\n"
      ;

    i++;
    printf("TEST %i in '%s:%i': parse plain text with script attributes and build tree = ", i, __FILE__, __LINE__);

    if ((pndPie = xmlNewNode(NULL, NAME_PIE_PIE)) == NULL) {
      printf("Error xmlNewNode()\n");
    }
    else if (ParsePlainBuffer(pndPie, pucContent, RMODE_PAR) == FALSE) {
      printf("Error ParsePlainBuffer()\n");
    }
    else if (IS_NODE_PIE(pndPie) == FALSE || domNumberOfChild(pndPie, NAME_PIE_BLOCK) != 1
      || domNumberOfChild(pndPie->children, NAME_PIE_SECTION) != 1 || domNumberOf(pndPie->children, NAME_PIE_PAR, 0) != 3) {
      printf("Error 1 tree\n");
    }
#if 0
    else if (RecognizeScripts(pndPie) == FALSE) {
      printf("Error RecognizeScripts()\n");
    }
    else if (IS_NODE_PIE(pndPie) == FALSE || domNumberOfChild(pndPie, NAME_PIE_SECTION) != 1
	     || domNumberOf(pndPie, NAME_PIE_PAR, 0) != 3 || domNumberOf(pndPie, NAME_PIE_SCRIPT, 0) != 4) {
      printf("Error 2 tree\n");
    }
#endif
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST "split result", pndPie);
    xmlFreeNode(pndPie);
  }


  if (SKIPTEST) {
    xmlNodePtr pndPie;
    xmlNodePtr pndPieImport;
    xmlChar *pucData = BAD_CAST
      "\t\n20101103 AAA\n20101105 BBB\n20101107 CCC\n20101109 DEF\n      "
      "; 00000001 First of month\n"
      ";00000015 Middle of month\n"
      "00000001-1 Last of month\n"
      "00000101 First of year\n"
      "00000101+124 125th day of year\n"
      "00000101-1 Last of year\n"
      "; every day\n"
      "00000000 %a %d.%m. (%j)\n"
      ";00000000 %Y-%m-%d (%j)\n"
      "000000 %Y %m\n";

    i++;
    printf("TEST %i in '%s:%i': parse text node content = ", i, __FILE__, __LINE__);

    if ((pndPie = xmlNewNode(NULL, NAME_PIE_PIE)) == NULL) {
      printf("Error xmlNewNode()\n");
    }
    else if ((pndPieImport = xmlNewChild(pndPie, NULL, NAME_PIE_IMPORT, pucData)) == NULL
      || xmlSetProp(pndPieImport, BAD_CAST "type", BAD_CAST"log") == NULL) {
      printf("Error xmlNewChild()\n");
    }
    else if (ProcessImportNode(pndPieImport, pccArg) == FALSE) {
      printf("Error ProcessImportNode()\n");
    }
    else if (IS_NODE_PIE(pndPie) == FALSE || domNumberOfChild(pndPie, NAME_PIE_BLOCK) != 1 || domNumberOfChild(pndPie->children, NAME_PIE_PAR) != 22) {
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST "split result", pndPie);
    xmlFreeNode(pndPie);
  }


  if (RUNTEST) {
    xmlNodePtr pndPie;
    xmlNodePtr pndPieImport;

    i++;
    printf("TEST %i in '%s:%i': import of plain text file = ",i,__FILE__,__LINE__);

    if ((pndPie = xmlNewNode(NULL, NAME_PIE_PIE)) == NULL) {
      printf("Error xmlNewNode()\n");
    }
    else if ((pndPieImport = xmlNewChild(pndPie, NULL, NAME_PIE_IMPORT, NULL)) == NULL
      || xmlSetProp(pndPieImport, BAD_CAST "name", BAD_CAST TESTPREFIX "option/pie/text/test-pie-6.txt") == NULL) {
      printf("Error xmlNewChild()\n");
    }
    else if (ProcessImportNode(pndPieImport, pccArg) == FALSE) {
      printf("Error ProcessImportNode()\n");
    }
    else if (IS_NODE_PIE(pndPie) == FALSE || domNumberOfChild(pndPie, NAME_PIE_BLOCK) != 1
      || domNumberOfChild(pndPie->children, NAME_PIE_SECTION) != 1 || domNumberOfChild(pndPie->children, NAME_PIE_IMPORT) != 0) {
      printf("Error 1 tree\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST "split result", pndPie);
    xmlFreeNode(pndPie);
  }


  if (RUNTEST) {
    xmlNodePtr pndPie;
    xmlNodePtr pndT;
    xmlNodePtr pndPieImport;

    i++;
    printf("TEST %i in '%s:%i': import text file to a text node = ",i,__FILE__,__LINE__);

    if ((pndPie = xmlNewNode(NULL, NAME_PIE_PIE)) == NULL) {
      printf("Error xmlNewNode()\n");
    }
    else if ((pndPieImport = xmlNewChild(pndPie, NULL, NAME_PIE_IMPORT, NULL)) == NULL
      || xmlSetProp(pndPieImport, BAD_CAST "name", BAD_CAST TESTPREFIX "plain/test-content-utf16be.txt") == NULL) {
      printf("Error xmlNewChild()\n");
    }
    else if (ProcessImportNode(pndPieImport, pccArg) == FALSE) {
      printf("Error ProcessImportNode()\n");
    }
    else if (IS_NODE_PIE(pndPie) == FALSE || domNumberOfChild(pndPie, NAME_PIE_BLOCK) != 1
      || domNumberOfChild(pndPie->children, NAME_PIE_PAR) != 1) {
      printf("Error 1 tree\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST "split result", pndPie);
    xmlFreeNode(pndPie);
  }


  if (RUNTEST) {
    xmlNodePtr pndT;
    xmlNodePtr pndPie;

    i++;
    printf("TEST %i in '%s:%i': include text file to a text node = ", i, __FILE__, __LINE__);

    if ((pndPie = xmlNewNode(NULL, NAME_PIE_PIE)) == NULL) {
      printf("Error xmlNewNode()\n");
    }
    else if ((pndT = xmlNewChild(pndPie, NULL, NAME_PIE_INCLUDE, NULL)) == NULL
      || xmlSetProp(pndT, BAD_CAST "name", BAD_CAST TESTPREFIX "option/pie/text/test-pie-subst.txt") == NULL) {
      printf("Error xmlNewChild()\n");
    }
    else if ((pndT = xmlNewChild(pndPie, NULL, NAME_SUBST, NULL)) == NULL
      || xmlSetProp(pndT, BAD_CAST "string", BAD_CAST "Logbook") == NULL
      || xmlSetProp(pndT, BAD_CAST "to", BAD_CAST "LOGBOOK") == NULL
      || xmlNewChild(pndPie, NULL, BAD_CAST "hr", NULL) == NULL) {
      printf("Error xmlNewChild()\n");
    }
    else if ((pndT = xmlNewChild(pndPie, NULL, NAME_PIE_INCLUDE, NULL)) == NULL
      || xmlSetProp(pndT, BAD_CAST "name", BAD_CAST TESTPREFIX "option/pie/text/test-pie-12a.txt") == NULL
      || xmlNewChild(pndPie, NULL, BAD_CAST "hr", NULL) == NULL) {
      printf("Error xmlNewChild()\n");
    }
    else if ((pndT = xmlNewChild(pndPie, NULL, NAME_PIE_INCLUDE, NULL)) == NULL
      || xmlSetProp(pndT, BAD_CAST "name", BAD_CAST TESTPREFIX "option/pie/text/test-pie-14.pie") == NULL) {
      printf("Error xmlNewChild()\n");
    }
    else {
      xmlDocPtr pdocT = NULL;

      //domPutNodeString(stderr, BAD_CAST "pre include", pndPie);
      TraverseIncludeNodes(pndPie, pccArg);
      if (IS_NODE_PIE(pndPie) == FALSE || domNumberOfChild(pndPie, NULL) != 10
	|| domNumberOfChild(pndPie->children, NAME_PIE_INCLUDE) != 0) {
	printf("Error 1 tree\n");
      }
      else if ((pdocT = pieProcessPieNode(pndPie, pccArg)) == NULL) {

      }
      else {
	n_ok++;
	printf("OK\n");
      }
      //domPutDocString(stderr, BAD_CAST "subst result", pdocT);
      xmlFreeDoc(pdocT);
    }
    xmlFreeNode(pndPie);
  }


  if (RUNTEST) {
    xmlNodePtr pndPie;
    xmlNodePtr pndTest;
    xmlNodePtr pndImport;
    xmlNodePtr pndResult;

    i++;
    printf("TEST %i in '%s:%i': import from various import nodes = ", i, __FILE__, __LINE__);

    pndPie = xmlNewNode(NULL, NAME_PIE_PIE);

    pndTest = xmlNewChild(pndPie, NULL, NAME_PIE_SECTION, NULL);
    pndTest = xmlNewChild(pndTest, NULL, NAME_PIE_SECTION, NULL);
    xmlNewChild(pndTest, NULL, NAME_PIE_HEADER, BAD_CAST"header 11");
    xmlNewChild(pndTest, NULL, NAME_PIE_PAR, BAD_CAST"pre list");
    pndImport = xmlNewChild(pndTest, NULL, NAME_PIE_IMPORT, BAD_CAST"\n+ A\n\n+ B\n\n+ C\n\n");
    xmlNewChild(pndTest, NULL, NAME_PIE_PAR, BAD_CAST"post list");

    pndTest = xmlNewChild(pndPie, NULL, NAME_PIE_SECTION, NULL);
    xmlNewChild(pndTest, NULL, NAME_PIE_HEADER, BAD_CAST"header 2");
    xmlNewChild(pndTest, NULL, NAME_PIE_PAR, BAD_CAST"pre pre");
    pndImport = xmlNewChild(pndTest, NULL, NAME_PIE_IMPORT, BAD_CAST"P\nQ\nR\nS\nT\n\n");
    xmlSetProp(pndImport, BAD_CAST "type", BAD_CAST"pre");
    xmlNewChild(pndTest, NULL, NAME_PIE_PAR, BAD_CAST"post pre");
    pndImport = xmlNewChild(pndTest, NULL, NAME_PIE_IMPORT, NULL);
    xmlSetProp(pndImport, BAD_CAST "name", BAD_CAST TESTPREFIX "option/pie/text/test-pie-12b.txt");
    pndImport = xmlNewChild(pndTest, NULL, NAME_PIE_IMPORT, NULL);
    xmlSetProp(pndImport, BAD_CAST "name", BAD_CAST TESTPREFIX "option/pie/text/test-pie-12c.txt");

    TraverseImportNodes(pndPie, pccArg);

    if (domNumberOfChild(pndPie, NAME_PIE_SECTION) != 2
      || domNumberOf(pndPie, NAME_PIE_PRE, 0) != 1) {
      printf("Error TraverseImportNodes()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST "split result", pndPie);
    xmlFreeNode(pndPie);
  }


  if (RUNTEST) {
    xmlNodePtr pndRoot;
    xmlNodePtr pndBlock;
    xmlNodePtr pndPie;
    xmlChar *pucContent = BAD_CAST
      "\n"
      "* Test of text node\n"
      "\n"
      "\n"
      "KLM NOP QRS\n"
      "\n"
      ;
    xmlDocPtr  pdocT;

    i++;
    printf("TEST %i in '%s:%i': parse plain text node and build tree = ", i, __FILE__, __LINE__);

    if ((pndPie = xmlNewNode(NULL, NAME_PIE_PIE)) == NULL || xmlAddChild(pndPie, xmlNewText(pucContent)) == NULL) {
      printf("Error xmlNewTextChild()\n");
    }
    else if ((pdocT = pieProcessPieNode(pndPie, pccArg)) == NULL) {
      printf("Error 1 pieProcessPieNode()\n");
    }
    else if ((pndRoot = xmlDocGetRootElement(pdocT)) == NULL || IS_NODE_PIE_PIE(pndRoot) == FALSE) {
      printf("Error 1 tree\n");
    }
    else if ((pndBlock = pndRoot->children) == NULL || IS_NODE_PIE_BLOCK(pndBlock) == FALSE
      || (pndBlock = pndBlock->children) == NULL || IS_NODE_PIE_BLOCK(pndBlock) == FALSE) {
      printf("Error 2 tree\n");
    }
    else if (domNumberOfChild(pndBlock, NAME_PIE_SECTION) != 1) {
      printf("Error 3 tree\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST "split result", pndPie);
    //domPutDocString(stderr, BAD_CAST "split result", pdocT);
    xmlFreeNode(pndPie);
    xmlFreeDoc(pdocT);
  }


  if (RUNTEST) {
    xmlNodePtr pndRoot;
    xmlNodePtr pndBlock;
    xmlNodePtr pndPie;
    xmlNodePtr pndPieChild;
    xmlNodePtr pndT;
    xmlDocPtr  pdocT;

    i++;
    printf("TEST %i in '%s:%i': pieProcessPieNode() import of plain text file = ", i, __FILE__, __LINE__);

    pndPie = xmlNewNode(NULL, NAME_PIE_PIE);
    pndPieChild = xmlNewChild(pndPie, NULL, NAME_PIE_IMPORT, NULL);
    xmlSetProp(pndPieChild, BAD_CAST "name", BAD_CAST TESTPREFIX "option/pie/text/test-pie-6.txt");

    if ((pdocT = pieProcessPieNode(pndPie, pccArg)) == NULL) {
      printf("Error 1 pieProcessPieNode()\n");
    }
    else if ((pndRoot = xmlDocGetRootElement(pdocT)) == NULL || IS_NODE_PIE_PIE(pndRoot) == FALSE) {
      printf("Error 1 tree\n");
    }
    else if ((pndBlock = pndRoot->children) == NULL || IS_NODE_PIE_BLOCK(pndBlock) == FALSE
      || (pndBlock = pndBlock->children) == NULL || IS_NODE_PIE_BLOCK(pndBlock) == FALSE) {
      printf("Error 2 tree\n");
    }
    else if (domNumberOfChild(pndBlock, NAME_PIE_SECTION) != 1) {
      printf("Error 3 tree\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutDocString(stderr, BAD_CAST "split result", pdocT);
    xmlFreeNode(pndPie);
    xmlFreeDoc(pdocT);
  }


  if (RUNTEST) {
    xmlNodePtr pndRoot;
    xmlNodePtr pndBlock;
    xmlNodePtr pndPie;
    xmlNodePtr pndPieChild;
    xmlDocPtr  pdocT;

    i++;
    printf("TEST %i in '%s:%i': pieMain() import of plain text file with imports (circular, multi-step) = ",i,__FILE__,__LINE__);

    pndPie = xmlNewNode(NULL,NAME_PIE_PIE);
    pndPieChild = xmlNewChild(pndPie, NULL, NAME_PIE_IMPORT, NULL);
    xmlSetProp(pndPieChild, BAD_CAST "name", BAD_CAST TESTPREFIX "option/pie/text/test-pie-22.txt");

    if ((pdocT = pieProcessPieNode(pndPie, pccArg)) == NULL) {
      printf("Error 1 pieProcessPieNode()\n");
    }
    else if ((pndRoot = xmlDocGetRootElement(pdocT)) == NULL || IS_NODE_PIE_PIE(pndRoot) == FALSE) {
      printf("Error 1 tree\n");
    }
    else if ((pndBlock = pndRoot->children) == NULL || IS_NODE_PIE_BLOCK(pndBlock) == FALSE
      || (pndBlock = pndBlock->children) == NULL || IS_NODE_PIE_BLOCK(pndBlock) == FALSE) {
      printf("Error 2 tree\n");
    }
    else if (domNumberOfChild(pndBlock, NAME_PIE_SECTION) != 1) {
      printf("Error 3 tree\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutDocString(stderr, BAD_CAST "split result", pdocT);
    xmlFreeNode(pndPie);
    xmlFreeDoc(pdocT);
  }


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
    else if (pieWeightXPathInDoc(pdocResult, pucPattern) == FALSE) {
      printf("Error 1 pieWeightXPathInDoc()\n");
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
    //domPutDocString(stderr, BAD_CAST "pieWeightXPathInDoc(): ", pdocResult);

    xmlXPathFreeObject(result);
    xmlFreeDoc(pdocResult);
  }

  printf("Result in '%s': %i/%i OK\n\n", __FILE__, n_ok, i);

  return (i - n_ok);
}
/* end of pieTextTest() */
