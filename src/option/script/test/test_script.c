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
scriptTest(cxpContextPtr pccArg)
{
  int n_ok;
  int i;

  int iA, iB;

  n_ok=0;
  i=0;

  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);
    
    getXmlBody(BAD_CAST"<abc/>",&iA,&iB);
    if (iA==0 && iB==5) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("ERROR\n");
    }
  }

  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);
    
    getXmlBody(BAD_CAST"<abc>ABC</abc>", &iA, &iB);
    if (iA==0 && iB==13) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("ERROR\n");
    }
  }

  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);
    
    getXmlBody(BAD_CAST"<abc>A<qers/>A</abc>", &iA, &iB);
    if (iA==0 && iB==19) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("ERROR\n");
    }
  }

  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);
    
    getXmlBody(BAD_CAST"<?xml version=\"1.0\" encoding=\"UTF-8\"?>  <abc>A<qers/>A</abc>", &iA, &iB);
    if (iA==40 && iB==59) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("ERROR\n");
    }
  }

  if (RUNTEST) {
    xmlChar* pucT = NULL;
    xmlNodePtr pndSubst = NULL;

    i++;
    printf("TEST %i in '%s:%i': scriptProcessScriptAttribute() = ", i, __FILE__, __LINE__);

    if (scriptProcessScriptAttribute(NULL, NULL) != NULL) {
      printf("Error 1 ()\n");
    }
    else if (scriptProcessScriptAttribute(NULL, pccArg) != NULL) {
      printf("Error 2 ()\n");
    }
    else if ((pndSubst = xmlNewNode(NULL, NAME_SUBST)) == NULL || xmlSetProp(pndSubst,BAD_CAST"script",BAD_CAST"'A' + 'B' + 'C'") == NULL) {
      printf("Error xmlNewNode()\n");
    }
    else if ((pucT = scriptProcessScriptAttribute(pndSubst, pccArg)) == NULL) {
      printf("Error scriptProcessScriptAttribute()\n");
    }
    else if (xmlStrEqual(pucT,BAD_CAST"ABC") == FALSE) {
      printf("Error scriptProcessScriptAttribute()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    xmlFreeNode(pndSubst);
    xmlFree(pucT);
  }

  if (RUNTEST) {
    xmlChar* pucT = NULL;
    xmlNodePtr pndScript = NULL;

    i++;
    printf("TEST %i in '%s:%i': scriptProcessScriptNode() = ", i, __FILE__, __LINE__);

    if (scriptProcessScriptNode(NULL, NULL) != NULL) {
      printf("Error 1 ()\n");
    }
    else if (scriptProcessScriptNode(NULL, pccArg) != NULL) {
      printf("Error 2 ()\n");
    }
    else if ((pndScript = xmlNewNode(NULL, NAME_SCRIPT)) == NULL || xmlAddChild(pndScript, xmlNewText(BAD_CAST"'A' + 'B' + 'C'")) == NULL) {
      printf("Error xmlNewNode()\n");
    }
    else if ((pucT = scriptProcessScriptNode(pndScript, pccArg)) == NULL) {
      printf("Error scriptProcessScriptNode()\n");
    }
    else if (xmlStrEqual(pucT,BAD_CAST"ABC") == FALSE) {
      printf("Error scriptProcessScriptNode()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    xmlFreeNode(pndScript);
    xmlFree(pucT);
  }

  printf("Result in '%s': %i/%i OK\n\n",__FILE__,n_ok,i);

  return (i - n_ok);
}
