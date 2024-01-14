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
cxpScriptTest(cxpContextPtr pccArg)
{
  int n_ok;
  int i;

  n_ok=0;
  i=0;

  if (RUNTEST) {
    xmlChar* pucT = NULL;
    xmlChar* pucTT = NULL;

    i++;
    printf("TEST %i in '%s:%i': cxpScriptProcessText() = ", i, __FILE__, __LINE__);

    if (cxpScriptProcessText(NULL, NULL) != NULL) {
      printf("Error 1 ()\n");
    }
    else if (cxpScriptProcessText(NULL, pccArg) != NULL) {
      printf("Error 2 ()\n");
    }
    else if ((pucT = cxpScriptProcessText(BAD_CAST"'A' + 'B' + 'C'", pccArg)) == NULL) {
      printf("Error cxpScriptProcessText()\n");
    }
    else if (xmlStrEqual(pucT,BAD_CAST"ABC") == FALSE) {
      printf("Error cxpScriptProcessText()\n");
    }
    else if ((pucTT = cxpScriptProcessText(BAD_CAST"A + B + C", pccArg)) == NULL) {
      printf("Error cxpScriptProcessText()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    xmlFree(pucTT);
    xmlFree(pucT);
  }


  if (RUNTEST) {
    xmlChar* pucT = NULL;
    xmlNodePtr pndSubst = NULL;

    i++;
    printf("TEST %i in '%s:%i': subst attribute = ", i, __FILE__, __LINE__);

    if ((pndSubst = xmlNewNode(NULL, NAME_SUBST)) == NULL || xmlSetProp(pndSubst,BAD_CAST NAME_PIE_SCRIPT,BAD_CAST"'A' + 'B' + 'C'") == NULL) {
      printf("Error xmlNewNode()\n");
    }
    else if ((pucT = cxpScriptProcessText(domGetPropValuePtr(pndSubst,BAD_CAST NAME_PIE_SCRIPT),pccArg)) == NULL) {
      printf("Error subst attribute cxpScriptProcessText()\n");
    }
    else if (xmlStrEqual(pucT,BAD_CAST"ABC") == FALSE) {
      printf("Error subst value cxpScriptProcessText()\n");
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
    printf("TEST %i in '%s:%i': cxpScriptProcessNode() = ", i, __FILE__, __LINE__);

    if (cxpScriptProcessNode(NULL, NULL) != NULL) {
      printf("Error 1 ()\n");
    }
    else if (cxpScriptProcessNode(NULL, pccArg) != NULL) {
      printf("Error 2 ()\n");
    }
    else if ((pndScript = xmlNewNode(NULL, NAME_SCRIPT)) == NULL || xmlAddChild(pndScript, xmlNewText(BAD_CAST"'A' + 'B' + 'C'")) == NULL) {
      printf("Error xmlNewNode()\n");
    }
    else if ((pucT = cxpScriptProcessNode(pndScript, pccArg)) == NULL) {
      printf("Error cxpScriptProcessNode()\n");
    }
    else if (xmlStrEqual(pucT,BAD_CAST"ABC") == FALSE) {
      printf("Error cxpScriptProcessNode()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    xmlFreeNode(pndScript);
    xmlFree(pucT);
  }

  if (RUNTEST) {
    xmlChar* pucT = NULL;
    xmlNodePtr pndScript = NULL;

    i++;
    printf("TEST %i in '%s:%i': cxpScriptProcessNode() = ", i, __FILE__, __LINE__);

    if ((pndScript = xmlNewNode(NULL, NAME_SCRIPT)) == NULL || xmlSetProp(pndScript, BAD_CAST"name",BAD_CAST TESTPREFIX "option/script/test-js-4.js") == NULL) {
      printf("Error xmlNewNode()\n");
    }
    else if ((pucT = cxpScriptProcessNode(pndScript, pccArg)) == NULL) {
      printf("Error cxpScriptProcessNode()\n");
    }
    else if (xmlStrEqual(pucT,BAD_CAST"ABCCDE") == FALSE) {
      printf("Error cxpScriptProcessNode()\n");
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
