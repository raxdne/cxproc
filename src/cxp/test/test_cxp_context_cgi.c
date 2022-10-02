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
cxpCtxtCliTest(cxpContextPtr pccArg)
{
  int n_ok;
  int i;

  n_ok=0;
  i=0;


  if (RUNTEST) {
    cxpContextPtr pccT;

    i++;
    printf("TEST %i in '%s:%i': cxpCtxtCliNew() = ",i,__FILE__,__LINE__);

    pccT = cxpCtxtCliNew(-1, NULL, NULL);
    if (pccT != NULL
      && cxpCtxtSetReadonly(pccT, TRUE) == TRUE
      && cxpCtxtIsReadonly(pccT) == TRUE
      && cxpCtxtSetReadonly(pccT, FALSE) == FALSE
      && cxpCtxtIsReadonly(pccT) == FALSE) {
      cxpCtxtLogInfo(pccT);
      n_ok++;
      printf("OK\n");
      cxpCtxtFree(pccT);
    }
    else {
      printf("Error \n");
    }
  }

#if 0
  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);
    if (cxpCtxtEnvGetCount(pccArg) > 0) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error cxpCtxtEnvGetCount(pccArg)\n");
    }
  }

  if (RUNTEST) {
    xmlChar *pucT;
    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);
    if ((pucT = cxpCtxtEnvGetName(pccArg, 0))) {
      xmlFree(pucT);
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error cxpCtxtEnvGetName(pccArg,0)\n");
    }
  }

  if (RUNTEST) {
    xmlChar *pucT;
    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);
    if ((pucT = cxpCtxtEnvGetName(pccArg,10000))==NULL) {
      n_ok++;
      printf("OK\n");
    }
    else {
      xmlFree(pucT);
      printf("Error cxpCtxtEnvGetName(pccArg,10000)\n");
    }
  }

  if (RUNTEST) {
    xmlChar *pucT;
    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);
    if ((pucT = cxpCtxtEnvGetValue(pccArg, 0))) {
      xmlFree(pucT);
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error cxpCtxtEnvGetValue(pccArg,0)\n");
    }
  }

  if (RUNTEST) {
    xmlChar *pucT;
    xmlChar *pucTT;
    xmlChar *pucTTT;
    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);
    if ((pucT = cxpCtxtEnvGetName(pccArg,0))
      && (pucTT = cxpCtxtEnvGetValue(pccArg,0))
      && (pucTTT = cxpCtxtEnvGetValueByName(pccArg,pucT))
      && xmlStrEqual(pucTT,pucTTT)) {
      xmlFree(pucT);
      xmlFree(pucTT);
      xmlFree(pucTTT);
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error comparison GetEnv*()\n");
    }
  }

  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': ", i, __FILE__, __LINE__);
    if (GetArgCount() > 0) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error GetArgCount()\n");
    }
}

  if (RUNTEST) {
    xmlChar *pucT;
    i++;
    printf("TEST %i in '%s:%i': ", i, __FILE__, __LINE__);
    if ((pucT = GetArgValue(0))) {
      xmlFree(pucT);
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error GetArgValue(0)\n");
    }
  }

  if (RUNTEST) {
    xmlChar *pucT;
    i++;
    printf("TEST %i in '%s:%i': ", i, __FILE__, __LINE__);
    if ((pucT = GetArgValue(10000)) == NULL) {
      n_ok++;
      printf("OK\n");
    }
    else {
      xmlFree(pucT);
      printf("Error GetArgValue(10000)\n");
    }
  }

  if (RUNTEST) {
    xmlChar *pucT;
    i++;
    printf("TEST %i in '%s:%i': ", i, __FILE__, __LINE__);
    if ((pucT = GetArgName(1))) {
      xmlFree(pucT);
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error GetArgName(1)\n");
    }
  }

  if (RUNTEST) {
    xmlChar *pucT;
    i++;
    printf("TEST %i in '%s:%i': argv substitution", i, __FILE__, __LINE__);
    if ((pucT = GetArgSubst(BAD_CAST" %1 %3 %2")) && xmlStrEqual(pucT, BAD_CAST" id=123  alex")) {
      xmlFree(pucT);
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error GetArgSubst()\n");
    }
  }

  if (RUNTEST) {
    xmlChar *pucT;
    xmlChar *pucTT;

    i++;
    printf("TEST %i in '%s:%i': ", i, __FILE__, __LINE__);
    if ((pucT = GetArgName(1)) && (pucTT = GetArgValue(1)) && xmlStrstr(pucTT, pucT)) {
      xmlFree(pucT);
      xmlFree(pucTT);
      //xmlFree(pucTTT);
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error comparison GetArg*()\n");
    }
  }

  if (RUNTEST) {
    cxpContextPtr pccT;

    i++;
    printf("TEST %i in '%s:%i': cxpCtxtCliNew() = ", i, __FILE__, __LINE__);

    pccT = cxpCtxtCliNew(-1, NULL, NULL);
    if (pccT != NULL
      && cxpCtxtCliGetCount(pccT) == 0
      && cxpCtxtCliGet(pccT, BAD_CAST "TEST") == NULL
      && cxpCtxtEnvGetValueByName(pccT, BAD_CAST "TEST") == NULL
//      && cxpContextGetCgi(pccT, "test") == NULL
) {
      cxpCtxtLogInfo(pccT);
      n_ok++;
      printf("OK\n");
      cxpCtxtFree(pccT);
    }
    else {
      printf("Error \n");
    }
  }


  if (RUNTEST) {
    cxpContextPtr pccT;

    i++;
    printf("TEST %i in '%s:%i': cxpCtxtCliNew() = ", i, __FILE__, __LINE__);

    pccT = cxpCtxtDup(pccArg);
    if (pccT != NULL
      && cxpContextGetArgvNumber(pccT, 2) == NULL
      && cxpContextGetArgv(pccT, "TEST") == NULL
      && cxpContextGetEnv(pccT, "TEST") == NULL
      && cxpContextGetCgi(pccT, "test") == NULL) {
      cxpCtxtLogInfo(pccT);
      n_ok++;
      printf("OK\n");
      cxpCtxtFree(pccT);
    }
    else {
      printf("Error \n");
    }
  }


  if (RUNTEST) {
    cxpContextPtr pccT;

    i++;
    printf("TEST %i in '%s:%i': cxpCtxtCliNew() = ", i, __FILE__, __LINE__);

    pccT = cxpCtxtDup(pccArg);
    if (pccT != NULL
      && cxpContextSetArgvNumber(pccT, 2, "abc") == NULL
      && cxpContextGetArgvNumber(pccT, 2) != NULL
      && cxpContextGetArgvNumber(pccArg, 2) == NULL
      && cxpContextSetArgv(pccT, "TEST", "123") == NULL
      && cxpContextGetArgv(pccT, "TEST") != NULL
      && cxpContextGetArgv(pccArg, "TEST") == NULL
      && cxpContextSetEnv(pccT, "TEST", "ABC") == NULL
      && cxpContextGetEnv(pccT, "TEST") != NULL
      && cxpContextGetEnv(pccArg, "TEST") == NULL
      && cxpContextSetCgi(pccT, "test", "dir/abc") == NULL
      && cxpContextGetCgi(pccT, "test") != NULL
      && cxpContextGetCgi(pccArg, "test") == NULL) {
      cxpCtxtLogInfo(pccT);
      n_ok++;
      printf("OK\n");
      cxpCtxtFree(pccT);
    }
    else {
      printf("Error \n");
    }
  }
#endif

  printf("Result in '%s': %i/%i OK\n\n", __FILE__, n_ok, i);

  return (i - n_ok);
}
