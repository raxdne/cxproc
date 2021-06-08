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

/*!\todo Markdown test <https://spec.commonmark.org/0.29/>

    1 Introduction
        1.1 What is Markdown?
        1.2 Why is a spec needed?
        1.3 About this document
    2 Preliminaries
        2.1 Characters and lines
        2.2 Tabs
        2.3 Insecure characters
    3 Blocks and inlines
        3.1 Precedence
        3.2 Container blocks and leaf blocks
    4 Leaf blocks
        4.1 Thematic breaks
        4.2 ATX headings
        4.3 Setext headings
        4.4 Indented code blocks
        4.5 Fenced code blocks
        4.6 HTML blocks
        4.7 Link reference definitions
        4.8 Paragraphs
        4.9 Blank lines
    5 Container blocks
        5.1 Block quotes
        5.2 List items
        5.3 Lists
    6 Inlines
        6.1 Backslash escapes
        6.2 Entity and numeric character references
        6.3 Code spans
        6.4 Emphasis and strong emphasis
        6.5 Links
        6.6 Images
        6.7 Autolinks
        6.8 Raw HTML
        6.9 Hard line breaks
        6.10 Soft line breaks
        6.11 Textual content
    Appendix: A parsing strategy
        Overview
        Phase 1: block structure
        Phase 2: inline structure

 */

/*! 
*/
int
pieTextBlocksTest(void)
{
  int n_ok;
  int i;

  n_ok=0;
  i=0;

  CompileRegExpDefaults();

  if (RUNTEST) {
    xmlNodePtr pndPie;
    xmlNodePtr pndTest;

    i++;
    printf("TEST %i in '%s:%i': = ",i,__FILE__,__LINE__);

    pndPie = xmlNewNode(NULL,NAME_PIE_PIE);

    pndTest = xmlNewChild(pndPie, NULL, NAME_PIE_IMPORT, NULL);
    
    if (GetModeByAttr(NULL) != RMODE_PAR) {
      printf("Error 1\n");
    }
    else if (GetModeByAttr(pndPie) != RMODE_PAR) {
      printf("Error 2\n");
    }
    else if (GetModeByAttr(pndTest) != RMODE_PAR) {
      printf("Error 3\n");
    }
    else if (SetTypeAttr(pndTest,RMODE_LINE) == FALSE || GetModeByAttr(pndTest) != RMODE_LINE) {
      printf("Error 4\n");
    }
    else if (SetTypeAttr(pndTest,RMODE_LINE) == FALSE || GetModeByAttr(pndTest) != RMODE_LINE) {
      printf("Error 5\n");
    }
    else if (SetTypeAttr(pndTest,RMODE_TABLE) == TRUE) {
      printf("Error 6\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    xmlFreeNode(pndPie);
  }

  if (RUNTEST) {
    
    i++;
    printf("TEST %i in '%s:%i': = ",i,__FILE__,__LINE__);
    
    if (GetModeByExtension(NULL) != RMODE_PAR || GetModeByExtension(BAD_CAST"") != RMODE_PAR || GetModeByExtension(BAD_CAST"xyz") != RMODE_PAR) {
      printf("Error 1\n");
    }
    else if (GetModeByExtension(BAD_CAST"csv") != RMODE_TABLE) {
      printf("Error 2\n");
    }
    else if (GetModeByExtension(BAD_CAST"txt") != RMODE_PAR) {
      printf("Error 3\n");
    }
    else if (GetModeByExtension(BAD_CAST"md") != RMODE_MD) {
      printf("Error 3\n");
    }
    else if (GetModeByExtension(BAD_CAST"log") != RMODE_LINE) {
      printf("Error 4\n");
    }
    else if (GetModeByExtension(BAD_CAST"js") != RMODE_PRE) {
      printf("Error 5\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

#ifdef LEGACY

  if (RUNTEST) {
    xmlChar *pucResult;
    xmlChar *pucData = BAD_CAST "d:\\abc\\def\\ghi";

    i++;
    printf("TEST %i in '%s:%i': translate DOS drive path into URL = ",i,__FILE__,__LINE__);

    if ((pucResult = TranslateUncToUrl(pucData)) == NULL) {
      printf("Error converting UNC path\n");
    }
    else if (xmlStrEqual(pucResult, BAD_CAST"file:///d:/abc/def/ghi") == 0) {
      printf("Error converting UNC path\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    xmlFree(pucResult);
  }

  if (RUNTEST) {
    xmlChar *pucResult;
    xmlChar *pucData = BAD_CAST "\\\\abc\\def\\ghi";

    i++;
    printf("TEST %i in '%s:%i': translate UNC path into URL = ",i,__FILE__,__LINE__);

    if ((pucResult = TranslateUncToUrl(pucData)) == NULL) {
      printf("Error converting UNC path\n");
    }
    else if (xmlStrEqual(pucResult, BAD_CAST"file:////abc/def/ghi") == 0) {
      printf("Error converting UNC path\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    xmlFree(pucResult);
  }

  if (RUNTEST) {
    xmlChar *pucResult;
    xmlChar *pucData = BAD_CAST "UNC mapping \\\\abc\\def\\ghi\\ flskdjfdslk";

    i++;
    printf("TEST %i in '%s:%i': translate embedded UNC path into URL = ",i,__FILE__,__LINE__);

    if ((pucResult = TranslateUncToUrl(pucData)) == NULL) {
      printf("Error converting UNC path\n");
    }
    else if (xmlStrEqual(pucResult, BAD_CAST"UNC mapping file:////abc/def/ghi/ flskdjfdslk") == 0) {
      printf("Error converting UNC path\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    xmlFree(pucResult);
  }


#endif
  

  if (RUNTEST) {
    xmlChar *pucData = BAD_CAST "TODO: Phillips Entsafter &amp; R\xC3\xBChrger\xC3\xA4t @ebay";
    xmlNodePtr pndT;
    xmlNodePtr pndTT = NULL;

    i++;
    printf("TEST %i in '%s:%i': detect todo markup = ", i, __FILE__, __LINE__);

    if ((pndT = xmlNewNode(NULL, NAME_PIE_PAR)) == NULL || (xmlAddChild(pndT, xmlNewText(pucData))) == NULL) {
      printf("Error xmlNewTextChild\n");
    }
    else if ((pndTT = TaskNodeNew(pndT)) == NULL || IS_NODE_PIE_TASK(pndTT) == FALSE || IS_NODE_PIE_HEADER(pndTT->children) == FALSE) {
      printf("Error TaskNodeNew\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST "todo result", pndTT);
    xmlFreeNode(pndTT);
    xmlFreeNode(pndT);
  }

  if (RUNTEST) {
    xmlNodePtr pndPie;
    xmlNodePtr pndT;

    i++;
    printf("TEST %i in '%s:%i': split Markdown automatic link < ... > = ",i,__FILE__,__LINE__);

    if ((pndPie = xmlNewNode(NULL,NAME_PIE_PIE)) == NULL) {
      printf("Error xmlNewNode\n");
    }
    else if ((pndT = SplitStringToAutoLinkNodes(NULL)) != NULL) {
      printf("Error 1 splitting URL\n");
      xmlAddChild(pndPie,pndT);
    }
    else if ((pndT = SplitStringToAutoLinkNodes(BAD_CAST"AAAA abc")) != NULL) {
      printf("Error 2 splitting URL\n");
      xmlAddChild(pndPie,pndT);
    }
    else if ((pndT = SplitStringToAutoLinkNodes(BAD_CAST"AAAA <> abc <a>")) != NULL) {
      printf("Error 3 splitting URL\n");
      xmlAddChild(pndPie,pndT);
    }
    else if ((pndT = SplitStringToAutoLinkNodes(BAD_CAST "pre URL <http://www.abc.de> or <ABC.HTML>  or &lt;ABC.HTML&gt; or \xE2\x80\x99y.htm\xE2\x80\x98 post URL")) == NULL || xmlAddChild(pndPie, pndT) == NULL) {
      printf("Error 4 splitting URL\n");
    }
    else if (pndT->children == NULL
      || pndT->children->next == NULL
      || pndT->children->next->properties == NULL
      || pndT->children->next->properties->children == NULL
      || pndT->children->next->properties->children->content == NULL) {
      printf("Error 5 splitting URL\n");
    }
    else if (xmlStrEqual(pndT->children->next->properties->children->content, BAD_CAST"http://www.abc.de") == NULL) {
      printf("Error 6 splitting URL\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST"import result", pndPie);
    xmlFreeNode(pndPie);
  }


  if (RUNTEST) {
    xmlNodePtr pndPie;

    i++;
    printf("TEST %i in '%s:%i': split Markdown link & image = ",i,__FILE__,__LINE__);

    if ((pndPie = SplitTupelToLinkNodesMd(NULL)) != NULL) {
      printf("Error 1 splitting URL\n");
    }
    else if ((pndPie = SplitTupelToLinkNodesMd(BAD_CAST"AAAA abc")) != NULL) {
      printf("Error 2 splitting URL\n");
    }
    else if ((pndPie = SplitTupelToLinkNodesMd(BAD_CAST"AAAA ||b| abc")) != NULL) {
      printf("Error 3 splitting URL\n");
    }
    else if ((pndPie = SplitTupelToLinkNodesMd(BAD_CAST "pre URL [ABC](http://www.abc.de) and ![ABC](abc.png) post URL")) == NULL) {
      printf("Error 4 splitting URL\n");
    }
    else if (pndPie->children == NULL
	|| pndPie->children->next == NULL
	|| pndPie->children->next->properties == NULL
	|| pndPie->children->next->properties->children == NULL
	|| pndPie->children->next->properties->children->content == NULL) {
      printf("Error 5 splitting URL\n");
    }
    else if (xmlStrEqual(pndPie->children->next->properties->children->content,BAD_CAST"http://www.abc.de") == NULL) {
      printf("Error 6 splitting URL\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST"import result", pndPie);
    xmlFreeNode(pndPie);
  }


#ifdef LEGACY

  if (RUNTEST) {
    xmlNodePtr pndPie;

    i++;
    printf("TEST %i in '%s:%i': split URL = ",i,__FILE__,__LINE__);

    if ((pndPie = SplitTupelToLinkNodes(NULL)) != NULL) {
      printf("Error 1 splitting URL\n");
    }
    else if ((pndPie = SplitTupelToLinkNodes(BAD_CAST"AAAA abc")) != NULL) {
      printf("Error 2 splitting URL\n");
    }
    else if ((pndPie = SplitTupelToLinkNodes(BAD_CAST"AAAA ||b| abc")) != NULL) {
      printf("Error 3 splitting URL\n");
    }
    else if ((pndPie = SplitTupelToLinkNodes(BAD_CAST "pre URL |http://www.abc.de|ABC| post URL")) == NULL) {
      printf("Error 4 splitting URL\n");
    }
    else if (pndPie->children == NULL
	|| pndPie->children->next == NULL
	|| pndPie->children->next->properties == NULL
	|| pndPie->children->next->properties->children == NULL
	|| pndPie->children->next->properties->children->content == NULL) {
      printf("Error 5 splitting URL\n");
    }
    else if (xmlStrEqual(pndPie->children->next->properties->children->content,BAD_CAST"http://www.abc.de") == NULL) {
      printf("Error 6 splitting URL\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, "import result", pndPie);
    xmlFreeNode(pndPie);
  }

#endif
  

  if (RUNTEST) {
    xmlNodePtr pndPie;
    xmlNodePtr pndTest;
    xmlNodePtr pndResult;
    xmlNodePtr pndBlock;
    pieTextElementPtr ppeT;
    xmlChar *pucContent = BAD_CAST "* AAA\n\nBBB";

    i++;
    printf("TEST %i in '%s:%i': = ", i, __FILE__, __LINE__);

    pndPie = xmlNewNode(NULL, NAME_PIE_PIE);

    pndBlock = xmlNewChild(pndPie, NULL, NAME_PIE_BLOCK, NULL);
    pndTest = xmlNewChild(pndBlock, NULL, NAME_PIE_SECTION, NULL);
    pndTest = xmlNewChild(pndTest, NULL, NAME_PIE_SECTION, NULL);
    pndTest = xmlNewChild(pndTest, NULL, NAME_PIE_HEADER, BAD_CAST"header");
    pndTest = xmlNewChild(pndBlock, NULL, NAME_PIE_SECTION, NULL);
    xmlNewChild(pndTest, NULL, NAME_PIE_HEADER, BAD_CAST"header");
    pndTest = xmlNewChild(pndTest, NULL, NAME_PIE_SECTION, NULL);
    pndTest = xmlNewChild(pndTest, NULL, NAME_PIE_SECTION, NULL);
    pndResult = xmlNewChild(pndTest, NULL, NAME_PIE_SECTION, NULL);
    xmlNewChild(pndResult, NULL, NAME_PIE_HEADER, BAD_CAST"result");

    //domPutNodeString(stderr, BAD_CAST"import result", pndPie);

    if ((ppeT = pieElementNew(pucContent, RMODE_PAR, LANG_DEFAULT)) == NULL) {
    }
    else if (pieElementHasNext(ppeT) == FALSE) {
    }
    else if (pieElementParse(ppeT) == FALSE) {
    }
    else if (pieElementIsHeader(ppeT) == FALSE) {
    }
    else if ((pndTest = GetParentElement(ppeT, pndPie)) != pndBlock) {
      printf("Error 1 GetParentElement()\n");
    }
    else if (pieElementHasNext(ppeT) == FALSE) {
    }
    else if (pieElementParse(ppeT) == FALSE) {
    }
    else if (pieElementIsPar(ppeT) == FALSE) {
    }
    else if ((pndTest = GetParentElement(ppeT, pndPie)) == NULL) {
      printf("Error 2 GetParentElement()\n");
    }
    else if (GetParentElement(ppeT, NULL) != NULL) {
      printf("Error 3 GetParentElement()\n");
    }
    else if (GetParentElement(NULL, pndPie) != NULL) {
      printf("Error 4 GetParentElement()\n");
    }
    else if (GetParentElement(NULL, NULL) != NULL) {
      printf("Error 5 GetParentElement()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    pieElementFree(ppeT);
    xmlFreeNode(pndPie);
  }


  if (RUNTEST) {
    xmlNodePtr pndPie;
    xmlNodePtr pndParent;
    xmlNodePtr pndBlock;
    xmlNodePtr pndList;
    xmlNodePtr pndPar;
    xmlNodePtr pndSection;
    pieTextElementPtr ppeT;
    xmlChar *pucContent = BAD_CAST "* ABC\n\nDEF\n\n+ GHI \u2714\n\n++ JKL\u2718 \n\nMNO\n";

    i++;
    printf("TEST %i in '%s:%i': = ", i, __FILE__, __LINE__);

    pndPie = xmlNewNode(NULL, NAME_PIE_PIE);
    pndBlock = xmlNewChild(pndPie, NULL, NAME_PIE_BLOCK, NULL);

    if ((ppeT = pieElementNew(pucContent, RMODE_PAR, LANG_DEFAULT)) == NULL) {
      printf("Error pieElementNew()\n");
    }
    else if (pieElementHasNext(ppeT) == FALSE) {
      printf("Error 1 pieElementHasNext()\n");
    }
    else if (pieElementParse(ppeT) == FALSE) {
    }
    else if (pieElementIsHeader(ppeT) == FALSE) {
      printf("Error 1 pieElementIsPar()\n");
    }
    else if ((pndSection = pieElementToDOM(ppeT)) == NULL) {
      printf("Error 1 pieElementToDOM()\n");
    }
    else if ((pndParent = GetParentElement(ppeT, pndPie)) != pndBlock || xmlAddChild(pndParent, pndSection) == NULL) {
      printf("Error 1 GetParentElement()\n");
    }
    else if (pieElementHasNext(ppeT) == FALSE) {
      printf("Error 1 pieElementHasNext()\n");
    }
    else if (pieElementParse(ppeT) == FALSE) {
    }
    else if (pieElementIsPar(ppeT) == FALSE) {
      printf("Error 1 pieElementIsPar()\n");
    }
    else if ((pndPar = pieElementToDOM(ppeT)) == NULL) {
      printf("Error 1 pieElementToDOM()\n");
    }
    else if ((pndParent = GetParentElement(ppeT, pndPie)) != pndSection || xmlAddChild(pndParent, pndPar) == NULL) {
      printf("Error 1 GetParentElement()\n");
    }
    else if (pieElementHasNext(ppeT) == FALSE) {
      printf("Error 1 pieElementHasNext()\n");
    }
    else if (pieElementParse(ppeT) == FALSE) {
    }
    else if (pieElementIsListItem(ppeT) == FALSE) {
      printf("Error 1 pieElementIsPar()\n");
    }
    else if ((pndList = pieElementToDOM(ppeT)) == NULL) {
      printf("Error 1 pieElementToDOM()\n");
    }
    else if ((pndParent = GetParentElement(ppeT, pndPie)) == NULL || xmlAddChild(pndParent, pndList) == NULL) {
      printf("Error 1 GetParentElement()\n");
    }
    else if (pieElementHasNext(ppeT) == FALSE) {
      printf("Error 1 pieElementHasNext()\n");
    }
    else if (pieElementParse(ppeT) == FALSE) {
    }
    else if (pieElementIsListItem(ppeT) == FALSE) {
      printf("Error 1 pieElementIsPar()\n");
    }
    else if ((pndList = pieElementToDOM(ppeT)) == NULL) {
      printf("Error 1 pieElementToDOM()\n");
    }
    else if ((pndParent = GetParentElement(ppeT, pndPie)) == NULL || xmlAddChild(pndParent, pndList) == NULL) {
      printf("Error 1 GetParentElement()\n");
    }
    else if (pieElementHasNext(ppeT) == FALSE) {
      printf("Error 1 pieElementHasNext()\n");
    }
    else if (pieElementParse(ppeT) == FALSE) {
    }
    else if (pieElementIsPar(ppeT) == FALSE) {
      printf("Error 1 pieElementIsPar()\n");
    }
    else if ((pndPar = pieElementToDOM(ppeT)) == NULL) {
      printf("Error 1 pieElementToDOM()\n");
    }
    else if ((pndParent = GetParentElement(ppeT, pndPie)) == NULL || xmlAddChild(pndParent, pndPar) == NULL) {
      printf("Error 1 GetParentElement()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, pucContent, pndPie);

    pieElementFree(ppeT);
    xmlFreeNode(pndPie);
  }


  if (RUNTEST) {
    xmlNodePtr pndPie;
    xmlNodePtr pndTest;

    i++;
    printf("TEST %i in '%s:%i': = ", i, __FILE__, __LINE__);

    pndPie = xmlNewNode(NULL, NAME_PIE_PIE);

    pndTest = xmlNewChild(pndPie, NULL, NAME_PIE_SECTION, NULL);
    pndTest = xmlNewChild(pndTest, NULL, NAME_PIE_SECTION, NULL);
    xmlAddChild(pndTest, xmlNewPI(NAME_PIE_IMPORT, BAD_CAST "c:/temp/a.txt"));
    pndTest = xmlNewChild(pndTest, NULL, NAME_PIE_SECTION, NULL);
    pndTest = xmlNewChild(pndTest, NULL, NAME_PIE_HEADER, BAD_CAST"header");

    if (domNodeIsDescendant(pndPie, pndTest) == FALSE) {
      printf("Error domNodeIsDescendant()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, "import result", pndPie);
    xmlFreeNode(pndPie);
  }


  if (RUNTEST) {
    xmlNodePtr pndPie;
    xmlNodePtr pndTest;
    resNodePtr prnT = NULL;
    resNodePtr prnTT = NULL;
    resNodePtr prnTTT = NULL;

    i++;
    printf("TEST %i in '%s:%i': circular import in dynamic DOM = ", i, __FILE__, __LINE__);

    prnT = resNodeDirNew(BAD_CAST "c:/temp/a.txt");
    prnTT = resNodeDirNew(BAD_CAST "c:/temp/B.txt");
    prnTTT = resNodeDirNew(BAD_CAST "c:/temp/C.PNG");

    pndPie = xmlNewNode(NULL, NAME_PIE_PIE);

    pndTest = xmlNewChild(pndPie, NULL, NAME_PIE_SECTION, NULL);
    pndTest = xmlNewChild(pndTest, NULL, NAME_PIE_SECTION, NULL);
    xmlSetProp(pndTest, BAD_CAST "context", resNodeGetURI(prnT));
    pndTest = xmlNewChild(pndTest, NULL, NAME_PIE_SECTION, NULL);
    pndTest = xmlNewChild(pndTest, NULL, NAME_PIE_HEADER, BAD_CAST"header");

    //domPutNodeString(stderr, BAD_CAST "import result", pndPie);
    if (IsImportCircular(NULL, NULL)) {
      printf("Error 1 IsImportCircular()\n");
    }
    else if (IsImportCircular(pndTest, NULL)) {
      printf("Error 2 IsImportCircular()\n");
    } 
    else if (IsImportCircular(pndTest, prnT) == FALSE) {
      printf("Error 3 IsImportCircular()\n");
    }
    else if (IsImportCircular(pndTest, prnTT)) {
      printf("Error 4 IsImportCircular()\n");
    }
    else if (IsImportCircular(pndTest, prnTTT)) {
      printf("Error 5 IsImportCircular()\n");
    }
    else if (IsImportCircular(pndTest, prnTTT)) {
      printf("Error 6 IsImportCircular()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    xmlFreeNode(pndPie);
    resNodeFree(prnTTT);
    resNodeFree(prnTT);
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    xmlNodePtr pndPie;
    xmlNodePtr pndT = NULL;
    xmlNodePtr pndTT = NULL;
    xmlNodePtr pndTTT = NULL;
    xmlNodePtr pndTNext = NULL;
    xmlNodePtr pndTLast = NULL;
    resNodePtr prnT = NULL;
    resNodePtr prnTT = NULL;
    resNodePtr prnTTT = NULL;

    i++;
    printf("TEST %i in '%s:%i': circular import in dynamic DOM = ", i, __FILE__, __LINE__);

    prnT = resNodeDirNew(BAD_CAST "c:/temp/a.txt");
    prnTT = resNodeDirNew(BAD_CAST "c:/temp/B.pie");
    prnTTT = resNodeDirNew(BAD_CAST "c:/temp/C.txt");

    pndPie = xmlNewNode(NULL, NAME_PIE_PIE);

    pndT = xmlNewChild(pndPie, NULL, NAME_PIE_SECTION, NULL);
    pndT = xmlNewChild(pndT, NULL, NAME_PIE_SECTION, NULL);
    xmlSetProp(pndT, BAD_CAST "context", resNodeGetURI(prnT));
    
    pndTT = xmlNewChild(pndT, NULL, NAME_PIE_SECTION, NULL);
    xmlSetProp(pndTT, BAD_CAST "context", resNodeGetURI(prnTT));
    
    pndTTT = xmlNewChild(pndTT, NULL, NAME_PIE_SECTION, NULL);
    xmlSetProp(pndTTT, BAD_CAST "context", resNodeGetURI(prnTTT));
    
    pndTLast = xmlNewChild(pndTTT, NULL, NAME_PIE_HEADER, BAD_CAST"header");

    pndTNext = xmlNewChild(pndPie, NULL, NAME_PIE_SECTION, BAD_CAST"header");

    //domPutNodeString(stderr, BAD_CAST "import node", pndPie);
    
    if (IsImportCircular(pndPie, prnT) == TRUE) {
      printf("Error 1 IsImportCircular()\n");
    }
    else if (IsImportCircular(pndT, prnT) == TRUE) {
      printf("Error 2 IsImportCircular()\n");
    }
    else if (IsImportCircular(pndTT, prnT) == FALSE) {
      printf("Error 2 IsImportCircular()\n");
    }
    else if (IsImportCircular(pndTTT, prnT) == FALSE) {
      printf("Error 2 IsImportCircular()\n");
    }
    else if (IsImportCircular(pndTLast, prnT) == FALSE) {
      printf("Error 2 IsImportCircular()\n");
    }
    else if (IsImportCircular(pndTNext, prnT) == TRUE) {
      printf("Error 2 IsImportCircular()\n");
    }
    else if (IsImportCircular(pndPie, prnTT) == TRUE) {
      printf("Error 1 IsImportCircular()\n");
    }
    else if (IsImportCircular(pndT, prnTT) == TRUE) {
      printf("Error 2 IsImportCircular()\n");
    }
    else if (IsImportCircular(pndTT, prnTT) == TRUE) {
      printf("Error 2 IsImportCircular()\n");
    }
    else if (IsImportCircular(pndTTT, prnTT) == FALSE) {
      printf("Error 2 IsImportCircular()\n");
    }
    else if (IsImportCircular(pndTLast, prnTT) == FALSE) {
      printf("Error 2 IsImportCircular()\n");
    }
    else if (IsImportCircular(pndTNext, prnTT) == TRUE) {
      printf("Error 2 IsImportCircular()\n");
    }
    else if (IsImportCircular(pndPie, prnTTT) == TRUE) {
      printf("Error 1 IsImportCircular()\n");
    }
    else if (IsImportCircular(pndT, prnTTT) == TRUE) {
      printf("Error 2 IsImportCircular()\n");
    }
    else if (IsImportCircular(pndTT, prnTTT) == TRUE) {
      printf("Error 2 IsImportCircular()\n");
    }
    else if (IsImportCircular(pndTTT, prnTTT) == TRUE) {
      printf("Error 2 IsImportCircular()\n");
    }
    else if (IsImportCircular(pndTLast, prnTTT) == FALSE) {
      printf("Error 2 IsImportCircular()\n");
    }
    else if (IsImportCircular(pndTNext, prnTTT) == TRUE) {
      printf("Error 2 IsImportCircular()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    xmlFreeNode(pndPie);
    resNodeFree(prnTTT);
    resNodeFree(prnTT);
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    xmlDocPtr pdocTest;
    xmlNodePtr pndTest;
    resNodePtr prnT = NULL;
    resNodePtr prnTT = NULL;
    resNodePtr prnTTT = NULL;
    resNodePtr prnTTTT = NULL;

    i++;
    printf("TEST %i in '%s:%i': circular import in file DOM = ", i, __FILE__, __LINE__);

    prnT = resNodeDirNew(BAD_CAST TESTPREFIX "option/pie/text/test-pie-22.pie");
    prnTT = resNodeDirNew(BAD_CAST TESTPREFIX "../test/.\\option/pie\\text\\test-pie-22.pie");
    prnTTT = resNodeDirNew(BAD_CAST "c:/temp/B.txt");

    if ((pdocTest = xmlReadFile(resNodeGetNameNormalizedNative(prnT), NULL, 0)) == NULL) {
      printf("Error xmlReadFile()\n");
    }
    else if ((pndTest = xmlDocGetRootElement(pdocTest)->children) == NULL) {
      printf("Error xmlDocGetRootElement()\n");
    }
    else if ((prnTTTT = resNodeDirNew(BAD_CAST pdocTest->doc->URL)) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if ((xmlSetProp(xmlDocGetRootElement(pdocTest), BAD_CAST "context", resNodeGetURI(prnTTTT))) == NULL) {
      printf("Error xmlSetProp()\n");
    }
    else if (IsImportCircular(pndTest, prnT) == FALSE) {
      printf("Error 1 IsImportCircular()\n");
    }
    else if (IsImportCircular(pndTest, prnTT) == FALSE) {
      printf("Error 2 IsImportCircular()\n");
    } 
    else if (IsImportCircular(pndTest, prnTTT) == TRUE) {
      printf("Error 3 IsImportCircular()\n");
    } 
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutDocString(stderr, BAD_CAST "import node", pdocTest);
    xmlFreeDoc(pdocTest);
    resNodeFree(prnTTTT);
    resNodeFree(prnTTT);
    resNodeFree(prnTT);
    resNodeFree(prnT);
  }


  if (RUNTEST) {
    xmlNodePtr pndPie;

    i++;
    printf("TEST %i in '%s:%i': parse empty plain text = ", i, __FILE__, __LINE__);

    if ((pndPie = xmlNewNode(NULL, NAME_PIE_PIE)) == NULL) {
      printf("Error xmlNewNode()\n");
    }
    else if (ParsePlainBuffer(pndPie, NULL, RMODE_PAR) != NULL) {
      printf("Error 1 ParsePlainBuffer()\n");
    }
    else if (ParsePlainBuffer(pndPie, BAD_CAST"", RMODE_PAR) != NULL) {
      printf("Error 2 ParsePlainBuffer()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST"import result", pndPie);
    xmlFreeNode(pndPie);
  }


  if (RUNTEST) {
    xmlNodePtr pndPie;
    xmlNodePtr pndBlock;
    xmlChar *pucContent = BAD_CAST
      "; begin\n\n"
      "* ABCDE\n\n"
      "** FGHI\n\n"
      "*** JKLM\n\n"
      "AAA & >>BBB<<\n\n"
      "+ 1\n\n"
      "+ 2\n\n"
      "+ 3\n\n\n"
      "CCC  \n\r\n"
      "fig. abc.png: picture\n\n"
      "#begin_of_skip\n\n"
      "*** SKIP ***\n\n"
      "#end_of_skip\n\n"
      "%% NPQR\n\r\n"
      "- 1\n\n"
      "-- 11\n\n"
      "--- 111\n\n"
      "---- 1111\n\n"
      "- 2\n\n"
      "- 3\n\n\n"
      "#begin_of_pre\n\n"
      "*** PRE ***\n\n"
      "#end_of_pre\n\n"
      "***** STUV\n\r\n"
      "TODO: task markup\n\n"
      "* WXYZ\n\n"
      "#import(\"abc.pie\")\n\n"
      "; end\r\n     "
      "#begin_of_skip\n\n"
      "non-existant\n     "
      "* non-existant\r\n     "
      ;

    i++;
    printf("TEST %i in '%s:%i': parse multi-block plain text and build list of import elements = ", i, __FILE__, __LINE__);

    if ((pndPie = xmlNewNode(NULL, NAME_PIE_PIE)) == NULL) {
      printf("Error xmlNewNode()\n");
    }
    else if (ParsePlainBuffer(pndPie, pucContent, RMODE_PAR) == NULL) {
      printf("Error 1 ParsePlainBuffer()\n");
    }
    else if ((pndBlock = pndPie->children) == NULL || IS_NODE_PIE_BLOCK(pndBlock) == FALSE) {
      printf("Error 2 ParsePlainBuffer()\n");
    }
    else if (domNumberOfChild(pndBlock, NAME_PIE_SECTION) != 2) {
      printf("Error 3 ParsePlainBuffer()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST"import result", pndPie);
    xmlFreeNode(pndPie);
  }


#ifdef WITH_MARKDOWN

  if (RUNTEST) {
    xmlNodePtr pndPie;
    xmlNodePtr pndBlock;
    xmlChar *pucContent = BAD_CAST
      "# ABCDE #\n"
      "## FGHI ##\n"
      "### JKLM\n"
      "AAA & >>BBB<<\n"
      "- 1\n"
      "- 2\n"
      "- 3\n\n"
      "CCC  \n\r\n"
      "fig. abc.png: picture\n\n"
      "NPQR\r\n"
      "====\n"
      "- 1\n"
      "* 2\n"
      "+ 3\n\n"
      "##### STUV\n\r\n"
      "TODO: task markup\n\n"
      "WXYZ\n"
      "----"
      ;

    i++;
    printf("TEST %i in '%s:%i': parse markdown text and build list of import elements = ", i, __FILE__, __LINE__);

    if ((pndPie = xmlNewNode(NULL, NAME_PIE_PIE)) == NULL) {
      printf("Error xmlNewNode()\n");
    }
    else if (ParsePlainBuffer(pndPie, pucContent, RMODE_MD) == NULL) {
      printf("Error 1 ParsePlainBuffer()\n");
    }
    else if ((pndBlock = pndPie->children) == NULL || IS_NODE_PIE_BLOCK(pndBlock) == FALSE) {
      printf("Error 2 ParsePlainBuffer()\n");
    }
    else if (domNumberOfChild(pndBlock, NAME_PIE_SECTION) != 2) {
      printf("Error 3 ParsePlainBuffer()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST"import result", pndPie);
    xmlFreeNode(pndPie);
  }

#endif


  if (RUNTEST) {
    resNodePtr prnT = NULL;
    xmlNodePtr pndPie;
    xmlNodePtr pndBlock;
    xmlChar *pucT = NULL;

    i++;
    printf("TEST %i in '%s:%i': parse multi-block plain text from file and build list of import elements = ", i, __FILE__, __LINE__);

    if ((pndPie = xmlNewNode(NULL, NAME_PIE_PIE)) == NULL) {
      printf("Error xmlNewNode()\n");
    }
    else if ((prnT = resNodeDirNew(BAD_CAST TESTPREFIX "option/pie/text/test-pie-loop-step-0.txt")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (ParsePlainBuffer(pndPie, BAD_CAST resNodeGetContent(prnT,-1), RMODE_PAR) == NULL) {
      printf("Error 1 ParsePlainBuffer()\n");
    }
    else if ((pndBlock = pndPie->children) == NULL || IS_NODE_PIE_BLOCK(pndBlock) == FALSE) {
      printf("Error 2 ParsePlainBuffer()\n");
    }
    else if (domNumberOfChild(pndBlock, NAME_PIE_SECTION) != 3) {
      printf("Error 3 ParsePlainBuffer()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST"import result", pndPie);
    resNodeFree(prnT);
    xmlFreeNode(pndPie);
  }


  if (RUNTEST) {
    xmlChar *pucT = NULL;
    xmlChar* pucTT = NULL;

    i++;
    printf("TEST %i in '%s:%i': StringDecodeCharMarkupNew() = ", i, __FILE__, __LINE__);

    if ((pucT = StringDecodeCharMarkupNew(NULL, LANG_DEFAULT)) != NULL) {
      printf("Error 1 StringDecodeCharMarkupNew()\n");
    }
    else if ((pucT = StringDecodeCharMarkupNew("", LANG_DEFAULT)) != NULL) {
      printf("Error 1b StringDecodeCharMarkupNew()\n");
    }
    else if ((pucT = StringDecodeCharMarkupNew(BAD_CAST"<=> A=&gt; B<= C <-> D-&gt;E<- F --- -- G &gt;&gt;H<< I >J<", LANG_DEFAULT)) == NULL) {
      printf("Error 2 StringDecodeCharMarkupNew()\n");
    }
    else if ((pucTT = StringDecodeCharMarkupNew(BAD_CAST"&lt;=> A=> B<= C &lt;-> D->E<- F --- -- G >>H&lt;&lt; I >J<", LANG_DEFAULT)) == NULL) {
      printf("Error 3 StringDecodeCharMarkupNew()\n");
    }
    else if (xmlStrEqual(pucT, pucTT) == FALSE) {
      printf("Error 4 StringDecodeCharMarkupNew()\n");
    }
    else if (xmlStrEqual(pucTT, BAD_CAST
      STR_UTF8_LEFT_RIGHT_SINGLE_ARROW " A"
      STR_UTF8_RIGHTWARDS_SINGLE_ARROW " B"
      STR_UTF8_LEFTWARDS_SINGLE_ARROW " C "
      STR_UTF8_LEFT_RIGHT_ARROW " D"
      STR_UTF8_RIGHTWARDS_ARROW "E"
      STR_UTF8_LEFTWARDS_ARROW " F "
      STR_UTF8_EM_DASH " "
      STR_UTF8_EN_DASH " G "
      STR_UTF8_LEFT_DOUBLE_QUOTATION_MARK "H"
      STR_UTF8_RIGHT_DOUBLE_QUOTATION_MARK " I "
      ">J<"
    ) == FALSE) {
      printf("Error 5 StringDecodeCharMarkupNew()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    xmlFree(pucT);
    xmlFree(pucTT);
  }


  if (RUNTEST) {
    xmlNodePtr pndPie;
    xmlNodePtr pndP = NULL;
    xmlNodePtr pndT = NULL;

    i++;
    printf("TEST %i in '%s:%i': parse inlines must fail = ", i, __FILE__, __LINE__);

    if ((pndPie = xmlNewNode(NULL, NAME_PIE_PIE)) == NULL) {
      printf("Error xmlNewNode()\n");
    }
    else if ((pndT = SplitStringToInlineNodes(NULL)) != NULL || (pndT = SplitStringToInlineNodes(BAD_CAST"")) != NULL) {
      printf("Error SplitStringToInlineNodes()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST"task result", pndPie);
    xmlFreeNode(pndT);
    xmlFreeNode(pndPie);
  }


  if (RUNTEST) {
    xmlNodePtr pndPie;
    xmlNodePtr pndT = NULL;

    i++;
    printf("TEST %i in '%s:%i': parse inlines = ", i, __FILE__, __LINE__);

    if ((pndPie = xmlNewNode(NULL, NAME_PIE_PIE)) == NULL) {
      printf("Error xmlNewNode()\n");
    }
    else if ((pndT = SplitStringToInlineNodes(BAD_CAST"ABC __EM EM__ or **EM\n EM** or `ttt` ")) == NULL || xmlAddChild(pndPie, pndT) == NULL) {
      printf("Error 1 SplitStringToInlineNodes()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST"result", pndPie);
    xmlFreeNode(pndPie);
  }


#ifdef WITH_MARKDOWN

  /* Span Elements */

  if (SKIPTEST) {
    int k;
    BOOL_T fResult = TRUE;
    xmlChar *pucT;
    xmlNodePtr pndTT = NULL;
    pieTextElementPtr ppeT;

    i++;
    printf("TEST %i in '%s:%i': Markdown Links = ", i, __FILE__, __LINE__);

    pndTT = xmlNewNode(NULL, NAME_PIE_PIE);
    pucT = BAD_CAST
      "ABCDE\n"
      "##NOPQ\n\n"
      ;

    puts((const char *)pucT);

    ppeT = pieElementNew(pucT, RMODE_MD, LANG_DEFAULT);

    /*! loop for reading pie text elements */
    for (k=0; pieElementHasNext(ppeT); k++) {
      xmlChar *pucTT;
      xmlNodePtr pndT = NULL;

      pieElementParse(ppeT);
      pucTT = pieElementToPlain(ppeT);
      puts((const char *)pucTT);
      xmlFree(pucTT);

      switch (k) {
#if 0
      case 0:
	fResult &= ((pieElementIsPar(ppeT) && pieElementGetDepth(ppeT) == 0
	  && (pndT = pieElementToDOM(ppeT)) && IS_NODE_PIE_PAR(pndT)
	  && pndT->children && xmlStrlen(pndT->children->content) == 10));
	break;
      case 1:
	fResult &= ((pieElementIsPar(ppeT) && pieElementGetDepth(ppeT) == 0
	  && (pndT = pieElementToDOM(ppeT)) && IS_NODE_PIE_PAR(pndT)
	  && pndT->children && xmlStrlen(pndT->children->content) == 8));
	break;
      case 2:
	fResult &= ((pieElementIsPar(ppeT) && pieElementGetDepth(ppeT) == 0
	  && (pndT = pieElementToDOM(ppeT)) && IS_NODE_PIE_PAR(pndT)
	  && pndT->children && xmlStrlen(pndT->children->content) == 3));
	break;
      case 3:
	fResult &= ((pieElementIsHeader(ppeT) && pieElementGetDepth(ppeT) == 1
	  && (pndT = pieElementToDOM(ppeT)) && IS_NODE_PIE_SECTION(pndT)
	  && IS_NODE_PIE_HEADER(pndT->children) && pndT->children->children && xmlStrlen(pndT->children->children->content) == 3));
	break;
#endif
      default:
	pndT = pieElementToDOM(ppeT);
	break;
      }
      xmlAddChild(pndTT, pndT);
    }

    if (fResult && k == 5) {
      n_ok++;
      printf("OK\n");
    }

    pieElementFree(ppeT);
    //domPutNodeString(stderr, BAD_CAST "import node", pndTT);
    xmlFreeNode(pndTT);
  }

  /* Markdown Emphasis and code is common to plain text (s. above) */

  if (SKIPTEST) {
    int k;
    BOOL_T fResult = TRUE;
    xmlChar *pucT;
    xmlNodePtr pndTT = NULL;
    pieTextElementPtr ppeT;

    i++;
    printf("TEST %i in '%s:%i': Markdown Images = ", i, __FILE__, __LINE__);

    pndTT = xmlNewNode(NULL, NAME_PIE_PIE);
    pucT = BAD_CAST
      "# Images\n"
      ;

    ppeT = pieElementNew(pucT, RMODE_MD, LANG_DEFAULT);

    /*! loop for reading pie text elements */
    for (k=0; pieElementHasNext(ppeT); k++) {
      xmlNodePtr pndT = NULL;

      pieElementParse(ppeT);
#if 0
      if (k == 0
	&& pieElementIsHeader(ppeT) && pieElementGetDepth(ppeT) == 1 && (pndT = pieElementToDOM(ppeT)) && IS_NODE_PIE_SECTION(pndT)
	&& IS_NODE_PIE_HEADER(pndT->children) && pndT->children->children && xmlStrlen(pndT->children->children->content) == 3) {
      }
      else if (k == 1
	&& pieElementIsPar(ppeT) && pieElementGetDepth(ppeT) == 0 && (pndT = pieElementToDOM(ppeT)) && IS_NODE_PIE_PAR(pndT)
	&& pndT->children && xmlStrlen(pndT->children->content) == 3) {
      }
      else if (k == 2
	&& pieElementIsListItem(ppeT) && pieElementGetDepth(ppeT) == 1 && (pndT = pieElementToDOM(ppeT)) && IS_NODE_PIE_PAR(pndT)
	&& pndT->children && xmlStrlen(pndT->children->content) == 3) {
      }
      else if (k == 3
	&& pieElementIsPar(ppeT) && pieElementGetDepth(ppeT) == 0 && (pndT = pieElementToDOM(ppeT)) && IS_NODE_PIE_PAR(pndT)
	&& pndT->children && xmlStrlen(pndT->children->content) == 3) {
      }
      else if (k == 4
	&& pieElementIsListItem(ppeT) && pieElementGetDepth(ppeT) == 2 && (pndT = pieElementToDOM(ppeT)) && IS_NODE_PIE_PAR(pndT)
	&& pndT->children && xmlStrlen(pndT->children->content) == 3) {
      }
      else {
	fResult = FALSE;
	printf("Error %i pieElementParse()\n", k);
      }
#else
      pndT = pieElementToDOM(ppeT);
#endif
      xmlAddChild(pndTT, pndT);
    }

    if (fResult && k == 5) {
      n_ok++;
      printf("OK\n");
    }

    pieElementFree(ppeT);
    //domPutNodeString(stderr, BAD_CAST "import node", pndTT);
    xmlFreeNode(pndTT);
  }

  /* Miscellaneous */

  if (SKIPTEST) {
    int k;
    BOOL_T fResult = TRUE;
    xmlChar *pucT;
    xmlNodePtr pndTT = NULL;
    pieTextElementPtr ppeT;

    i++;
    printf("TEST %i in '%s:%i': Markdown Backslash Escapes = ", i, __FILE__, __LINE__);

    pndTT = xmlNewNode(NULL, NAME_PIE_PIE);
    pucT = BAD_CAST
      "ABCDE\n"
      ;


  }

  if (SKIPTEST) {
    int k;
    BOOL_T fResult = TRUE;
    xmlChar *pucT;
    xmlNodePtr pndTT = NULL;
    pieTextElementPtr ppeT;

    i++;
    printf("TEST %i in '%s:%i': Markdown Automatic Links = ", i, __FILE__, __LINE__);

    pndTT = xmlNewNode(NULL, NAME_PIE_PIE);
    pucT = BAD_CAST
      "# Automatic Links\n"
      ;

  }

#endif


  if (RUNTEST) {
    xmlNodePtr pndPie;
    xmlNodePtr pndP = NULL;
    xmlNodePtr pndT = NULL;

    i++;
    printf("TEST %i in '%s:%i': parse date must fail = ", i, __FILE__, __LINE__);

    if ((pndPie = xmlNewNode(NULL, NAME_PIE_PIE)) == NULL) {
      printf("Error xmlNewNode()\n");
    }
    else if ((pndT = SplitStringToDateNodes(NULL,MIME_TEXT_PLAIN)) != NULL || (pndT = SplitStringToDateNodes(BAD_CAST"",MIME_TEXT_PLAIN)) != NULL) {
      printf("Error SplitStringToDateNodes()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST"task result", pndPie);
    xmlFreeNode(pndT);
    xmlFreeNode(pndPie);
  }


  if (RUNTEST) {
    xmlNodePtr pndPie;
    xmlNodePtr pndT = NULL;

    i++;
    printf("TEST %i in '%s:%i': parse date = ", i, __FILE__, __LINE__);

    if ((pndPie = xmlNewNode(NULL, NAME_PIE_PIE)) == NULL) {
      printf("Error xmlNewNode()\n");
    }
    else if ((pndT = SplitStringToDateNodes(BAD_CAST"TODO: 20160301 done",MIME_TEXT_PLAIN)) == NULL || xmlAddChild(pndPie, pndT) == NULL) {
      printf("Error 1 SplitStringToDateNodes()\n");
    }
    else if ((pndT = SplitStringToDateNodes(BAD_CAST"20160301,20160303 and 20160304",MIME_TEXT_PLAIN)) == NULL || xmlAddChild(pndPie, pndT) == NULL) {
      printf("Error 2 SplitStringToDateNodes()\n");
    }
    else if ((pndT = SplitStringToDateNodes(BAD_CAST"2016-03-01;2016-03-03 and 2016-03-04",MIME_TEXT_PLAIN)) == NULL || xmlAddChild(pndPie, pndT) == NULL) {
      printf("Error 3 SplitStringToDateNodes()\n");
    }
    else if ((pndT = SplitStringToDateNodes(BAD_CAST"20160307+11",MIME_TEXT_PLAIN)) == NULL || xmlAddChild(pndPie, pndT) == NULL) {
      printf("Error 3 SplitStringToDateNodes()\n");
    }
    else if ((pndT = SplitStringToDateNodes(BAD_CAST"2016/2017",MIME_TEXT_PLAIN)) != NULL) {
      printf("Error 4 SplitStringToDateNodes()\n");
      xmlFreeNode(pndT);
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST"date result", pndPie);
    xmlFreeNode(pndPie);
  }


  if (RUNTEST) {
    xmlNodePtr pndPie;
    xmlNodePtr pndP = NULL;
    xmlNodePtr pndT = NULL;

    i++;
    printf("TEST %i in '%s:%i': parse TODO must fail = ", i, __FILE__, __LINE__);

    if ((pndPie = xmlNewNode(NULL, NAME_PIE_PIE)) == NULL) {
      printf("Error xmlNewNode()\n");
    }
    else if ((pndT = TaskNodeNew(NULL)) != NULL) {
      printf("Error TaskNodeNew()\n");
    }
    else if ((pndP = xmlNewChild(pndPie, NULL, NAME_PIE_PAR, BAD_CAST"")) == NULL || (pndT = TaskNodeNew(pndP)) != NULL) {
      printf("Error TaskNodeNew()\n");
    }
    else if ((pndP = xmlNewChild(pndPie, NULL, NAME_PIE_PAR, BAD_CAST"todo: ERROR")) == NULL || (pndT = TaskNodeNew(pndP)) != NULL) {
      printf("Error TaskNodeNew()\n");
    }
    else if ((pndP = xmlNewChild(pndPie, NULL, NAME_PIE_HEADER, BAD_CAST"TODO: ERROR")) == NULL || (pndT = TaskNodeNew(pndP)) != NULL) {
      printf("Error TaskNodeNew()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST"task result", pndPie);
    xmlFreeNode(pndT);
    xmlFreeNode(pndPie);
  }


  if (RUNTEST) {
    xmlNodePtr pndPie;
    xmlNodePtr pndP = NULL;
    xmlNodePtr pndList = NULL;
    xmlNodePtr pndT = NULL;

    i++;
    printf("TEST %i in '%s:%i': parse TODO = ", i, __FILE__, __LINE__);

    if ((pndPie = xmlNewNode(NULL, NAME_PIE_PIE)) == NULL || (pndList = xmlNewNode(NULL, NAME_PIE_PIE)) == NULL) {
      printf("Error xmlNewNode()\n");
    }
    else if ((pndP = xmlNewChild(pndPie, NULL, NAME_PIE_PAR, BAD_CAST"TODO: might\n be \ndone")) == NULL
      || (pndT = TaskNodeNew(pndP)) == NULL || xmlAddChild(pndList, pndT) == NULL) {
      printf("Error 1 TaskNodeNew()\n");
    }
    else if (IS_NODE_PIE_TASK(pndT) == FALSE
      || xmlStrEqual(domGetPropValuePtr(pndT, BAD_CAST"class"), BAD_CAST"todo") == FALSE
      || domNumberOfChild(pndT, NAME_PIE_HEADER) != 1) {
      printf("Error 2 TaskNodeNew()\n");
    }
    else if ((pndP = xmlNewChild(pndPie, NULL, NAME_PIE_PAR, BAD_CAST"TODO: ")) == NULL
      || (pndT = xmlNewChild(pndP, NULL, NAME_PIE_LINK, BAD_CAST"ABC")) == NULL
      || xmlSetProp(pndT, BAD_CAST"href", BAD_CAST"abc.html") == NULL
      || (pndT = xmlAddChild(pndP, xmlNewText(BAD_CAST" "))) == NULL
      || (pndT = xmlNewChild(pndP, NULL, NAME_PIE_HTAG, BAD_CAST"@me")) == NULL
      || (pndT = xmlAddChild(pndP, xmlNewText(BAD_CAST" "))) == NULL
      || (pndT = xmlNewChild(pndP, NULL, NAME_PIE_HTAG, BAD_CAST"#abc")) == NULL
      || (pndT = xmlAddChild(pndP, xmlNewText(BAD_CAST"+++"))) == NULL
      || (pndT = TaskNodeNew(pndP)) == NULL || xmlAddChild(pndList, pndT) == NULL) {
      printf("Error 1 TaskNodeNew()\n");
    }
    else if (IS_NODE_PIE_TASK(pndT) == FALSE
      || xmlStrEqual(domGetPropValuePtr(pndT, BAD_CAST"class"), BAD_CAST"todo") == FALSE
      || domNumberOfChild(pndT, NAME_PIE_HEADER) != 1) {
      printf("Error 2 TaskNodeNew()\n");
    }
    else if ((pndP = xmlNewChild(pndPie, NULL, NAME_PIE_PAR, NULL)) == NULL
      || (pndT = xmlNewChild(pndP, NULL, NAME_PIE_LINK, BAD_CAST"TODO: ABC @me #abc")) == NULL
      || xmlSetProp(pndT, BAD_CAST"href", BAD_CAST"abc.html") == NULL
      || (pndT = TaskNodeNew(pndP)) == NULL || xmlAddChild(pndList, pndT) == NULL) {
      printf("Error 1 TaskNodeNew()\n");
    }
    else if (IS_NODE_PIE_TASK(pndT) == FALSE
      || xmlStrEqual(domGetPropValuePtr(pndT, BAD_CAST"class"), BAD_CAST"todo") == FALSE
      || domNumberOfChild(pndT, NAME_PIE_HEADER) != 1) {
      printf("Error 2 TaskNodeNew()\n");
    }
    else if ((pndP = xmlNewChild(pndPie, NULL, NAME_PIE_PAR, BAD_CAST"DONE: finished")) == NULL
      || (pndT = TaskNodeNew(pndP)) == NULL || xmlAddChild(pndList, pndT) == NULL) {
      printf("Error 1 TaskNodeNew()\n");
    }
    else if (IS_NODE_PIE_TASK(pndT) == FALSE
      || xmlStrEqual(domGetPropValuePtr(pndT, BAD_CAST"class"), BAD_CAST"todo") == FALSE
      || xmlStrEqual(domGetPropValuePtr(pndT, BAD_CAST"state"), BAD_CAST"done") == FALSE
      || domNumberOfChild(pndT, NAME_PIE_HEADER) != 1) {
      printf("Error 2 TaskNodeNew()\n");
    }
    else if ((pndP = xmlNewChild(pndPie, NULL, NAME_PIE_PAR, BAD_CAST"BUG: problem")) == NULL
      || (pndT = TaskNodeNew(pndP)) == NULL || xmlAddChild(pndList, pndT) == NULL) {
      printf("Error 1 TaskNodeNew()\n");
    }
    else if (IS_NODE_PIE_TASK(pndT) == FALSE
      || xmlStrEqual(domGetPropValuePtr(pndT, BAD_CAST"class"), BAD_CAST"bug") == FALSE
      || domNumberOfChild(pndT, NAME_PIE_HEADER) != 1) {
      printf("Error 2 TaskNodeNew()\n");
    }
    else if ((pndP = xmlNewChild(pndPie, NULL, NAME_PIE_PAR, BAD_CAST"REQ: requirement")) == NULL
      || (pndT = TaskNodeNew(pndP)) == NULL || xmlAddChild(pndList, pndT) == NULL) {
      printf("Error 1 TaskNodeNew()\n");
    }
    else if (IS_NODE_PIE_TASK(pndT) == FALSE
      || xmlStrEqual(domGetPropValuePtr(pndT, BAD_CAST"class"), BAD_CAST"req") == FALSE
      || domNumberOfChild(pndT, NAME_PIE_HEADER) != 1) {
      printf("Error 2 TaskNodeNew()\n");
    }
    else if ((pndP = xmlNewChild(pndPie, NULL, NAME_PIE_PAR, BAD_CAST"TARGET: my target")) == NULL
      || (pndT = TaskNodeNew(pndP)) == NULL || xmlAddChild(pndList, pndT) == NULL) {
      printf("Error 1 TaskNodeNew()\n");
    }
    else if (IS_NODE_PIE_TASK(pndT) == FALSE
      || xmlStrEqual(domGetPropValuePtr(pndT, BAD_CAST"class"), BAD_CAST"target") == FALSE
      || domNumberOfChild(pndT, NAME_PIE_HEADER) != 1) {
      printf("Error 2 TaskNodeNew()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST"tasks", pndPie);
    //domPutNodeString(stderr, BAD_CAST"task result", pndList);
    xmlFreeNode(pndList);
    xmlFreeNode(pndPie);
  }


  printf("Result in '%s': %i/%i OK\n\n", __FILE__, n_ok, i);

  return (i - n_ok);
}
/* end of pieTextBlocksTest() */
