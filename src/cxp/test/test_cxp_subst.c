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
cxpSubstTest(cxpContextPtr pccArg)
{
  int n_ok;
  int i;

  n_ok=0;
  i=0;

  if (RUNTEST) {
    cxpSubstPtr pT;

    i++;
    printf("TEST %i in '%s:%i': cxpSubstNew() + cxpSubstFree() = ", i, __FILE__, __LINE__);

    if ((pT = cxpSubstNew()) == NULL) {
      printf("Error cxpSubstNew()\n");
    }
    else if (cxpSubstPrint(NULL, pccArg) == TRUE) {
      printf("Error 1 cxpSubstPrint()\n");
    }
    else if (cxpSubstPrint(pT, pccArg) == FALSE) {
      printf("Error 2 cxpSubstPrint()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    cxpSubstFree(pT);
  }

  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': ApplySubstText() = ",i,__FILE__,__LINE__);

    if (ApplySubstText(NULL,BAD_CAST"",NULL)) {
      printf("Error ApplySubstText()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    xmlNodePtr pndTest;
    xmlNodePtr pndInclude;

    i++;
    printf("TEST %i in '%s:%i': cxpSubstIncludeNodes() = ",i,__FILE__,__LINE__);

    pndTest = xmlNewNode(NULL,NAME_XML);
    pndInclude = xmlNewChild(pndTest,NULL,NAME_INCLUDE,NULL);
    xmlSetProp(pndInclude,BAD_CAST "name",BAD_CAST TESTPREFIX "option/pie/calendar/test-date.pie");
    
    if (cxpSubstIncludeNodes(NULL,pccArg)) {
      printf("Error 1 cxpSubstIncludeNodes()\n");
    }
    else if (cxpSubstIncludeNodes(pndTest,pccArg) == FALSE) {
      printf("Error 2 cxpSubstIncludeNodes()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST"include", pndTest);
    xmlFreeNode(pndTest);
  }

  if (RUNTEST) {
    cxpSubstPtr pT;
    xmlNodePtr pndSubst;

    i++;
    printf("TEST %i in '%s:%i': cxpSubstDetect() = ",i,__FILE__,__LINE__);

    pndSubst = xmlNewNode(NULL,NAME_SUBST);
    xmlSetProp(pndSubst,BAD_CAST "string",BAD_CAST"ABC");
    xmlAddChild(pndSubst,xmlNewText(BAD_CAST"XYZ"));

    if ((pT = cxpSubstDetect(pndSubst,pccArg)) == NULL) {
      printf("Error cxpSubstDetect()\n");
    }
    else if (xmlStrEqual(pT->pucName,BAD_CAST"ABC") == FALSE) {
      printf("Error pucName\n");
    }
    else if (cxpSubstGetPtr(pT) != pT->pucDefault || xmlStrEqual(pT->pucDefault,BAD_CAST"XYZ") == FALSE) {
      printf("Error pucDefault\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST"subst", pndSubst);
    cxpSubstFree(pT);
    xmlFreeNode(pndSubst);
  }


  if (RUNTEST) {
    cxpSubstPtr pT;
    xmlNodePtr pndSubst;

    i++;
    printf("TEST %i in '%s:%i': cxpSubstDetect() = ",i,__FILE__,__LINE__);

    pndSubst = xmlNewNode(NULL,NAME_SUBST);
    xmlSetProp(pndSubst,BAD_CAST "string",BAD_CAST"ABC");
    xmlSetProp(pndSubst,BAD_CAST "to",BAD_CAST"DEF");
    xmlSetProp(pndSubst,BAD_CAST "default",BAD_CAST"DUMMY");

    if ((pT = cxpSubstDetect(pndSubst,pccArg)) == NULL) {
      printf("Error cxpSubstDetect()\n");
    }
    else if (xmlStrEqual(pT->pucName,BAD_CAST"ABC") == FALSE) {
      printf("Error pucName\n");
    }
    else if (cxpSubstGetPtr(pT) != pT->pucTo || xmlStrEqual(pT->pucTo,BAD_CAST"DEF") == FALSE) {
      printf("Error pucTo\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST"subst", pndSubst);
    cxpSubstFree(pT);
    xmlFreeNode(pndSubst);
  }

  
  if (RUNTEST) {
    cxpSubstPtr pT;
    xmlNodePtr pndSubst;

    i++;
    printf("TEST %i in '%s:%i': cxpSubstDetect() = ",i,__FILE__,__LINE__);

    pndSubst = xmlNewNode(NULL,NAME_SUBST);
    xmlSetProp(pndSubst,BAD_CAST "string",BAD_CAST"ABC");
    xmlSetProp(pndSubst,BAD_CAST "now",BAD_CAST"%Y-%m-%d");

    if ((pT = cxpSubstDetect(pndSubst,pccArg)) == NULL) {
      printf("Error cxpSubstDetect()\n");
    }
    else if (xmlStrEqual(pT->pucName,BAD_CAST"ABC") == FALSE) {
      printf("Error pucName\n");
    }
    else if (cxpSubstGetPtr(pT) != pT->pucNow) {
      printf("Error pucNow\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST"subst", pndSubst);
    cxpSubstFree(pT);
    xmlFreeNode(pndSubst);
  }

  if (RUNTEST) {
    cxpSubstPtr pT;
    xmlNodePtr pndSubst;

    i++;
    printf("TEST %i in '%s:%i': cxpSubstDetect() = ",i,__FILE__,__LINE__);

    pndSubst = xmlNewNode(NULL,NAME_SUBST);
    xmlSetProp(pndSubst,BAD_CAST "regexp",BAD_CAST"[A-Z]{1,3}");
    xmlSetProp(pndSubst,BAD_CAST "to",BAD_CAST"YYY");

    if ((pT = cxpSubstDetect(pndSubst,pccArg)) == NULL) {
      printf("Error cxpSubstDetect()\n");
    }
    else if (pT->preFrom == NULL) {
      printf("Error preFrom\n");
    }
    else if (pT->pucTo == NULL) {
      printf("Error preTo\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST"subst", pndSubst);
    cxpSubstFree(pT);
    xmlFreeNode(pndSubst);
  }

  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': cxpSubstApply() = ",i,__FILE__,__LINE__);

    if (cxpSubstApply(NULL,NULL,pccArg)) {
      printf("Error\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    xmlNodePtr pndTest;
    xmlNodePtr pndT;
    xmlNodePtr pndTT;
    xmlNodePtr pndSubst;

    i++;
    printf("TEST %i in '%s:%i': cxpSubstInChildNodes() = ",i,__FILE__,__LINE__);

    pndTest = xmlNewNode(NULL,NAME_XML);
    pndSubst = xmlNewChild(pndTest,NULL,NAME_SUBST,NULL);
    xmlSetProp(pndSubst,BAD_CAST "string",BAD_CAST "%%AAAA%%");
    xmlSetProp(pndSubst,BAD_CAST "to",BAD_CAST "BBBB");

    xmlNewChild(pndTest,NULL,NAME_PLAIN,BAD_CAST "AAAA");
    xmlNewChild(pndTest,NULL,NAME_PLAIN,BAD_CAST "BBBB");
    xmlNewChild(pndTest,NULL,NAME_PLAIN,BAD_CAST "%%CCCC%%");
    pndT = xmlNewChild(pndTest,NULL,NAME_PLAIN,NULL);
    xmlSetProp(pndT, BAD_CAST "name", BAD_CAST "%%AAAA%%");
    pndT = xmlNewChild(pndT, NULL, NAME_PLAIN, NULL);
    xmlNewChild(pndT, NULL, NAME_PLAIN, BAD_CAST"%%AAAA%%");
    xmlNewChild(pndT, NULL, NAME_PLAIN, BAD_CAST"AAAA");
    pndTT = xmlNewChild(pndT,NULL,NAME_PLAIN,NULL);
    xmlAddChild(pndTT,xmlNewPI(BAD_CAST"cxproc",BAD_CAST"%%AAAA%%"));
    xmlNewChild(pndTT,NULL,NAME_PLAIN,BAD_CAST "%%AAAA%%");
    xmlAddChild(pndTT,xmlNewPI(BAD_CAST"cxproc",BAD_CAST"%%AAAA%%"));
    xmlAddChild(pndTT, xmlNewComment(BAD_CAST"%%AAAA%%"));

    //domPutNodeString(stderr, BAD_CAST"pre subst", pndTest);

    if (cxpSubstInChildNodes(NULL,NULL,pccArg) == TRUE) {
      printf("Error 1\n");
    }
    else if (cxpSubstInChildNodes(pndTest,NULL,pccArg) == FALSE) {
      printf("Error 2\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST"post subst", pndTest);
    xmlFreeNode(pndTest);
  }


  if (RUNTEST) {
    xmlNodePtr pndTest;
    xmlNodePtr pndT;
    xmlNodePtr pndTT;
    xmlNodePtr pndTTT;
    xmlNodePtr pndSubst;

    i++;
    printf("TEST %i in '%s:%i': cxpSubstInChildNodes() = ",i,__FILE__,__LINE__);

    pndTest = xmlNewNode(NULL,NAME_XML);
    pndSubst = xmlNewChild(pndTest,NULL,NAME_SUBST,NULL);
    xmlSetProp(pndSubst,BAD_CAST "string",BAD_CAST "%%AAAA%%");
    xmlSetProp(pndSubst,BAD_CAST "to",BAD_CAST "%%BBBB%%");
    pndSubst = xmlNewChild(pndTest,NULL,NAME_SUBST,NULL);
    xmlSetProp(pndSubst,BAD_CAST "string",BAD_CAST "%%DATE%%");
    xmlSetProp(pndSubst,BAD_CAST "now",BAD_CAST "pre %Y%m%dT%H%M%SZ post");
    pndSubst = xmlNewChild(pndTest,NULL,NAME_SUBST,NULL);
    xmlSetProp(pndSubst,BAD_CAST "string",BAD_CAST "%%CCCC%%");
    xmlSetProp(pndSubst,BAD_CAST "to",BAD_CAST "%%AAAA%%");
    pndSubst = xmlNewChild(pndTest,NULL,NAME_SUBST,NULL);
    xmlSetProp(pndSubst,BAD_CAST "string",BAD_CAST "%%FILE%%");
    xmlSetProp(pndSubst,BAD_CAST "to",BAD_CAST "abc.xml");
    pndSubst = xmlNewChild(pndTest,NULL,NAME_SUBST,NULL);
    xmlSetProp(pndSubst,BAD_CAST "string",BAD_CAST "%%TYPE%%");
    xmlSetProp(pndSubst,BAD_CAST "type",BAD_CAST "%%FILE%%");
    xmlSetProp(pndSubst,BAD_CAST "default",BAD_CAST "plain/dummy");

    xmlNewChild(pndTest,NULL,NAME_PLAIN,BAD_CAST "AAAA");
    xmlNewChild(pndTest,NULL,NAME_PLAIN,BAD_CAST "BBBB");
    xmlNewChild(pndTest,NULL,NAME_PLAIN,BAD_CAST "%%CCCC%%");
    pndT = xmlNewChild(pndTest,NULL,NAME_PLAIN,NULL);
    pndT = xmlNewChild(pndT,NULL,NAME_PLAIN,NULL);
    xmlNewChild(pndT,NULL,NAME_PLAIN,NULL);
    pndTT = xmlNewChild(pndT, NULL, NAME_PLAIN, BAD_CAST"pre ");
    pndTTT = xmlNewChild(pndTT,NULL,NAME_SUBST,NULL);
    xmlSetProp(pndTTT, BAD_CAST "now", BAD_CAST "%Y-%m-%d");
    xmlAddChild(pndTT, xmlNewText(BAD_CAST" post"));
    pndTT = xmlNewChild(pndT,NULL,NAME_PLAIN,NULL);
    xmlAddChild(pndTT,xmlNewPI(BAD_CAST"cxproc",BAD_CAST"dummy"));
    xmlNewChild(pndTT,NULL,NAME_PLAIN,BAD_CAST "%%DATE%%");
    xmlAddChild(pndTT,xmlNewPI(BAD_CAST"cxproc",BAD_CAST"dummy"));
    xmlNewChild(pndTT,NULL,NAME_PLAIN,BAD_CAST "MIME Type of '%%FILE%%' is %%TYPE%%");
    xmlNewChild(pndT,NULL,NAME_PLAIN,NULL);
    pndT = xmlNewChild(pndT,NULL,NAME_PLAIN,NULL);
    pndT = xmlNewChild(pndT,NULL,NAME_PLAIN,NULL);
    xmlNewChild(pndT,NULL,NAME_PLAIN,BAD_CAST "%%DATE%%");
    
    //domPutNodeString(stderr, BAD_CAST"pre subst", pndTest);

    if (cxpSubstInChildNodes(NULL,NULL,pccArg) == TRUE) {
      printf("Error 1\n");
    }
    else if (cxpSubstInChildNodes(pndTest, NULL, pccArg) == FALSE) {
      printf("Error 2\n");
    }
    else if (cxpSubstReplaceNodes(pndTest,pccArg) == FALSE) {
      printf("Error 3\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST"post subst", pndTest);
    xmlFreeNode(pndTest);
  }


  if (RUNTEST) {
    xmlNodePtr pndTest;
    xmlNodePtr pndT;
    xmlNodePtr pndTT;
    xmlNodePtr pndTTT;
    xmlNodePtr pndSubst;

    i++;
    printf("TEST %i in '%s:%i': cxpSubstInChildNodes() regexp = ",i,__FILE__,__LINE__);

    pndTest = xmlNewNode(NULL,NAME_XML);
    pndSubst = xmlNewChild(pndTest,NULL,NAME_SUBST,NULL);
    xmlSetProp(pndSubst,BAD_CAST "regexp",BAD_CAST "([A-Z]{3})");
    xmlSetProp(pndSubst,BAD_CAST "to",BAD_CAST " :: $1/$1 :: ");
    pndSubst = xmlNewChild(pndTest,NULL,NAME_SUBST,NULL);
    xmlSetProp(pndSubst,BAD_CAST "regexp",BAD_CAST "Z{3}");
    xmlNewChild(pndTest,NULL,NAME_PLAIN,BAD_CAST "BBBB");
    xmlNewChild(pndTest,NULL,NAME_PLAIN,BAD_CAST "AAAA");
    xmlNewChild(pndTest,NULL,NAME_PLAIN,BAD_CAST "%%CCC%%");

    //domPutNodeString(stderr, BAD_CAST"pre subst", pndTest);

    if (cxpSubstInChildNodes(pndTest,NULL,pccArg) == FALSE) {
      printf("Error 3\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST"post subst", pndTest);
    xmlFreeNode(pndTest);
  }


  if (RUNTEST) {
    xmlChar *pucT;
    cxpSubstPtr pT;

    i++;
    printf("TEST %i in '%s:%i': cxpSubstInStringNew() = ", i, __FILE__, __LINE__);

    if ((pucT = cxpSubstInStringNew(NULL, NULL, pccArg)) != NULL) {
      printf("Error 1 cxpSubstInStringNew()\n");
    }
    else if ((pucT = cxpSubstInStringNew(BAD_CAST"Abc Def hij", NULL, pccArg)) != NULL) {
      printf("Error 2 cxpSubstInStringNew()\n");
    }
#if 0
    else if ((pucT = cxpSubstInStringNew(BAD_CAST"Abc Def hij", pT, pccArg)) != NULL) {
      printf("Error 2 cxpSubstInStringNew()\n");
    }
#endif
    else {
      n_ok++;
      printf("OK\n");
    }
    xmlFree(pucT);
  }

  printf("Result in '%s': %i/%i OK\n\n", __FILE__, n_ok, i);

  return (i - n_ok);
}
