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
pieTextTagsTest(void)
{
  int n_ok;
  int i;

  n_ok=0;
  i=0;

  ProcessTags(NULL,NULL);
  
  if (RUNTEST) {
    xmlNodePtr pndPie = NULL;
    xmlNodePtr pndT = NULL;
    xmlNodePtr pndTest;
    xmlNodePtr pndResult;
    xmlNodePtr pndBlock;
    xmlChar *pucT = NULL;

    i++;
    printf("TEST %i in '%s:%i': GetBlockTagRegExpStr() = ", i, __FILE__, __LINE__);

    pndPie = xmlNewNode(NULL, NAME_PIE_PIE);

    pndBlock = xmlNewChild(pndPie, NULL, NAME_PIE_BLOCK, NULL);
    xmlAddChild(pndBlock, xmlNewPI(NAME_PIE_PI_TAG, BAD_CAST"ABC|DEF|HIJ"));
  
    pndTest = xmlNewChild(pndBlock, NULL, NAME_PIE_SECTION, NULL);
    pndTest = xmlNewChild(pndTest, NULL, NAME_PIE_HEADER, BAD_CAST"header");

    pndBlock = xmlNewChild(pndBlock, NULL, NAME_PIE_BLOCK, NULL);
    xmlAddChild(pndBlock, xmlNewPI(NAME_PIE_PI_TAG, BAD_CAST"(KLM|NOP|QRS)"));
  
    pndTest = xmlNewChild(pndBlock, NULL, NAME_PIE_SECTION, NULL);
    pndTest = xmlNewChild(pndTest, NULL, NAME_PIE_HEADER, BAD_CAST"header");
    xmlAddChild(pndBlock, xmlNewPI(NAME_PIE_PI_TAG, BAD_CAST"|UVW|XYZ"));

    if (GetBlockTagRegExpStr(NULL, NULL, FALSE) != NULL) {
      printf("Error 1 GetBlockTagRegExpStr()\n");
    }
    else if ((pucT = GetBlockTagRegExpStr(pndPie, NULL, TRUE)) == NULL) {
      printf("Error 2 GetBlockTagRegExpStr()\n");
    }
    else if (xmlStrEqual(pucT, BAD_CAST"ABC|DEF|HIJ|(KLM|NOP|QRS)|UVW|XYZ") == FALSE) {
      printf("Error 4 GetBlockTagRegExpStr(): '%s'\n",pucT);
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    
    //domPutNodeString(stderr, BAD_CAST"GetBlockTagRegExpStr() result", pndPie);
    xmlFree(pucT);
    xmlFreeNode(pndPie);
  }


#if 0
  if (RUNTEST) {
    xmlNodePtr pndPie;
    xmlNodePtr pndT = NULL;

    i++;
    printf("TEST %i in '%s:%i': SplitStringToHashTagNodes() = ", i, __FILE__, __LINE__);

    if ((pndPie = xmlNewNode(NULL, NAME_PIE_PIE)) == NULL) {
      printf("Error xmlNewNode()\n");
    }
    else if ((pndT = SplitStringToHashTagNodes(NULL,NULL))) {
      printf("Error 1 SplitStringToHashTagNodes()\n");
      xmlFreeNode(pndT);
    }
    else if ((pndT = SplitStringToHashTagNodes(BAD_CAST"",NULL))) {
      printf("Error 2 SplitStringToHashTagNodes()\n");
      xmlFreeNode(pndT);
    }
    else if ((pndT = SplitStringToHashTagNodes(BAD_CAST"AAA @BBB CCC",re_hashtag)) == NULL
      || domNumberOfChild(pndT, NAME_PIE_HTAG) != 1
      || xmlAddChild(pndPie, pndT) == NULL) {
      printf("Error 3 SplitStringToHashTagNodes()\n");
    }
    else if ((pndT = SplitStringToHashTagNodes(BAD_CAST"@abc123", re_hashtag)) == NULL
      || domNumberOfChild(pndT, NAME_PIE_HTAG) != 1
      || xmlAddChild(pndPie, pndT) == NULL) {
      printf("Error 3 SplitStringToHashTagNodes()\n");
    }
    else if ((pndT = SplitStringToHashTagNodes(BAD_CAST"@abc123 xyz @abc123", re_hashtag)) == NULL
      || domNumberOfChild(pndT, NAME_PIE_HTAG) != 2
      || xmlAddChild(pndPie, pndT) == NULL) {
      printf("Error 3 SplitStringToHashTagNodes()\n");
    }
    else if ((pndT = SplitStringToHashTagNodes(BAD_CAST"#abc123", re_hashtag)) == NULL
      || domNumberOfChild(pndT, NAME_PIE_HTAG) != 1
      || xmlAddChild(pndPie, pndT) == NULL) {
      printf("Error 3 SplitStringToHashTagNodes()\n");
    }
    else if ((pndT = SplitStringToHashTagNodes(BAD_CAST"Abcde Defgh", re_hashtag))) {
      printf("Error 3 SplitStringToHashTagNodes()\n");
      xmlFreeNode(pndT);
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST"ProcessTags() result", pndPie);
    xmlFreeNode(pndPie);
  }


  if (RUNTEST) {
    xmlDocPtr pdocPie = NULL;
    xmlNodePtr pndPie = NULL;

    i++;
    printf("TEST %i in '%s:%i': ProcessTags() = ", i, __FILE__, __LINE__);

    if (ProcessTags(NULL, NULL)) {
      printf("Error 1 ProcessTags()\n");
    }
    else if ((pdocPie = xmlReadFile(TESTPREFIX "option/pie/text/test-pie-14.pie", NULL, 0)) == NULL) {
    }
    else if ((pndPie = xmlDocGetRootElement(pdocPie)) == NULL) {
    }
    else if (RecognizeHashtags(pndPie,re_hashtag) == FALSE) {
      printf("Error 2 ProcessTags()\n");
    }
    else if (ProcessTags(pdocPie, BAD_CAST"\\bDUM[^ \\t]*\\b") == FALSE) {
      printf("Error 4 ProcessTags()\n");
    }
#if 0
    else if (domNumberOfChild(pndPie, NAME_PIE_TTAG) != 1) {
      printf("Error 5 ProcessTags()\n");
    }
#endif
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST"ProcessTags() result", pndPie);
    xmlFreeDoc(pdocPie);
  }


  if (RUNTEST) {
    xmlNodePtr pndPie = NULL;
    xmlNodePtr pndT = NULL;

    i++;
    printf("TEST %i in '%s:%i': RecognizeNodeTags() = ", i, __FILE__, __LINE__);

    if (ProcessTags(NULL, NULL)) {
      printf("Error 1 ProcessTags()\n");
    }
    else if (RecognizeNodeTags(NULL, NULL) == FALSE) {
      printf("Error 1 RecognizeNodeTags()\n");
    }
    else if ((pndPie = xmlNewNode(NULL, NAME_PIE_PIE)) == NULL) {
      printf("Error 1 xmlNewNode()\n");
    }
    else if ((pndT = xmlNewChild(pndPie, NULL, NAME_PIE_PAR, BAD_CAST"TEST test @test #test ATEST test")) == NULL) {
      printf("Error 2 xmlNewChild()\n");
    }
    else if (RecognizeNodeTags(pndPie, pndT) == FALSE) {
      printf("Error 2 RecognizeNodeTags()\n");
    }
    else if ((pndT = xmlNewChild(pndPie, NULL, NAME_PIE_PAR, BAD_CAST"A BC DEF HIjk lmn op q")) == NULL) {
      printf("Error 3 xmlNewChild()\n");
    }
    else if (RecognizeNodeTags(pndPie, pndT) == FALSE) {
      printf("Error 3 RecognizeNodeTags()\n");
    }
    else if (domNumberOfChild(pndPie, NAME_PIE_TTAG) != 2) {
      printf("Error 4 ProcessTags()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST"RecognizeNodeTags() result", pndPie);
    xmlFreeNode(pndPie);
  }
#endif


  if (RUNTEST) {
    xmlNodePtr pndPie = NULL;
    xmlNodePtr pndT = NULL;

    i++;
    printf("TEST %i in '%s:%i': CleanListTag() = ", i, __FILE__, __LINE__);

    if (CleanListTag(NULL,FALSE)) {
      printf("Error 1 CleanListTag()\n");
    }
    else if ((pndPie = xmlNewNode(NULL, NAME_PIE_TAGLIST)) == NULL) {
      printf("Error 1 xmlNewNode()\n");
    }
    else if ((pndT = xmlNewChild(pndPie, NULL, NAME_PIE_TTAG, BAD_CAST"")) == NULL) {
      printf("Error 2 xmlNewChild()\n");
    }
    else if (CleanListTag(pndPie,FALSE) == FALSE) {
      printf("Error 2 CleanListTag()\n");
    }
    else if ((pndT = xmlNewChild(pndPie, NULL, NAME_PIE_TTAG, BAD_CAST"Abc")) == NULL) {
      printf("Error 3 xmlNewChild()\n");
    }
    else if ((pndT = xmlNewChild(pndPie, NULL, NAME_PIE_TTAG, BAD_CAST"")) == NULL) {
      printf("Error 4 xmlNewChild()\n");
    }
    else if ((pndT = xmlNewChild(pndPie, NULL, NAME_PIE_TTAG, BAD_CAST"Abcdef")) == NULL) {
      printf("Error 5 xmlNewChild()\n");
    }
    else if ((pndT = xmlNewChild(pndPie, NULL, NAME_PIE_TTAG, BAD_CAST"ABCDEF")) == NULL) {
      printf("Error 6 xmlNewChild()\n");
    }
    else if (CleanListTag(pndPie,FALSE) == FALSE) {
      printf("Error 3 CleanListTag()\n");
    }
    else if (domNumberOfChild(pndPie, NAME_PIE_TTAG) != 3) {
      printf("Error 4 CleanListTag()\n");
    }
    else if (CleanListTag(pndPie,TRUE) == FALSE) {
      printf("Error 5 CleanListTag()\n");
    }
    else if (domNumberOfChild(pndPie, NAME_PIE_TTAG) != 2) {
      printf("Error 6 CleanListTag()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST"CleanListTag() result", pndPie);
    xmlFreeNode(pndPie);
  }


  printf("Result in '%s': %i/%i OK\n\n", __FILE__, n_ok, i);

  return (i - n_ok);
}
/* end of pieTextTagsTest() */
