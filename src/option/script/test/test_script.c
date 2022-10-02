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

/*!\return processed  the code of script 

\param pndArg
\param pccArg

\return
 */
xmlChar *
scriptProcessScriptBuffer(xmlChar *pucArgScript, xmlChar *pucArgBuffer, cxpContextPtr pccArg)
{
  xmlChar *pucResult = NULL;

#ifdef DEBUG
  cxpCtxtLogPrint(pccArg,1,"scriptProcessScriptBuffer(pndArg=%0x,pccArg=%0x)",pucArgScript,pccArg);
#endif

  if (STR_IS_NOT_EMPTY(pucArgScript)) {
    xmlChar *pucScriptDecoded;

    pucScriptDecoded = xmlStrdup(pucArgScript); /*!\bug decode XML entities to UTF-8 */

    cxpCtxtLogPrint(pccArg, 4, "Run Script code '%s'", pucScriptDecoded);
    duk_push_global_object(pDukContext);


    if (STR_IS_NOT_EMPTY(pucArgBuffer)) {
      void* p;
      xmlChar* pucBufferDecoded;

      pucBufferDecoded = xmlStrdup(pucArgBuffer); /*!\bug decode XML entities to UTF-8 */

#if 0
      // https://duktape.org/api.html#duk_push_buffer
      //p = duk_push_buffer(ctx, 1024, 0);
      //printf("allocated buffer, data area: %p\n", p);

      duk_push_heapptr(pDukContext, (void*)pucBufferDecoded);
      duk_push_external_buffer(pDukContext);
      duk_config_buffer(pDukContext, -1, pucBufferDecoded, strlen(pucBufferDecoded));
      //duk_call(pDukContext, 1);
#else
      duk_push_string(pDukContext, "var s = ");
      duk_push_string(pDukContext, pucBufferDecoded);
      duk_push_string(pDukContext, ";");
      //duk_push_string(pDukContext, "s.replace(/A/,'YYY')");
      duk_join(pDukContext, -1);
      printf("result: %s\n", duk_get_string(pDukContext, -1));  /* "foo; 123; true" */
      duk_peval(pDukContext);
      printf("result: %s\n", duk_safe_to_string(pDukContext, -1));  /* "foo; 123; true" */
      //duk_pop(pDukContext);

#endif
    }

    duk_push_string(pDukContext, (const char*)pucScriptDecoded);

    if (duk_peval(pDukContext) != 0) {
      cxpCtxtLogPrint(pccArg, 1, "Script error: %s", duk_safe_to_string(pDukContext, -1));
      pucResult = xmlStrdup(BAD_CAST duk_safe_to_string(pDukContext, -1));
    }
    else if (duk_check_type(pDukContext, -1, DUK_TYPE_NUMBER) || duk_check_type(pDukContext, -1, DUK_TYPE_STRING)) {
      pucResult = xmlStrdup(BAD_CAST duk_to_string(pDukContext, -1));
    }
    else {
      cxpCtxtLogPrint(pccArg, 1, "Script: undefined result");
      pucResult = xmlStrdup(BAD_CAST "");
    }
    duk_pop(pDukContext);  /* pop result/error */
    xmlFree(pucScriptDecoded);
  }
  return pucResult;
} /* end of scriptProcessScriptBuffer() */


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

  if (RUNTEST) {
    xmlChar* pucT = NULL;

    i++;
    printf("TEST %i in '%s:%i': scriptProcessScriptBuffer() = ", i, __FILE__, __LINE__);

    if (scriptProcessScriptBuffer(NULL, NULL, NULL) != NULL) {
      printf("Error 1 ()\n");
    }
    else if (scriptProcessScriptBuffer(NULL, NULL, pccArg) != NULL) {
      printf("Error 2 ()\n");
    }
    else if ((pucT = scriptProcessScriptBuffer(BAD_CAST"1 + 2 + 3 + 4", NULL, pccArg)) == NULL) {
      printf("Error scriptProcessScriptBuffer()\n");
    }
    else if (xmlStrEqual(pucT,BAD_CAST"10") == FALSE) {
      printf("Error scriptProcessScriptBuffer()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    xmlFree(pucT);
  }

  printf("Result in '%s': %i/%i OK\n\n",__FILE__,n_ok,i);

  return (i - n_ok);
}
