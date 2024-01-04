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


/*! test code for this module 
 */
int
cxpCtxtEncTest(cxpContextPtr pccArg, char** argv, char** envp)
{
  int n_ok = 0;
  index_t i = 0;

  if (RUNTEST) {
    int j = 0;
    cxpContextPtr pccT;
    char *e[] = {"AAA=1","BBB=2","CCC=3",NULL};
    
    i++;
    printf("TEST %i in '%s:%i': cxpCtxtDup() = ", i, __FILE__, __LINE__);
    
    if ((pccT = cxpCtxtDup(NULL)) == NULL) {
      printf("Error %i\n", 1);
    }
    else if (cxpCtxtIsReadonly(pccT) != FALSE) {
      printf("Error %i\n", 2);
    }
    else if (cxpCtxtEnvGetCount(pccT) != 0) {
      printf("Error %i\n", 3);
    }
    else if (cxpCtxtEnvDup(NULL,NULL) == TRUE) {
      printf("Error %i\n", 4);
    }
    else if (cxpCtxtEnvDup(pccT,NULL) == FALSE) {
      printf("Error %i\n", 5);
    }
    else if (cxpCtxtEnvGetCount(pccT) != 0) {
      printf("Error %i\n", 6);
    }
    else if (cxpCtxtEnvDup(pccT,e) == FALSE) {
      printf("Error %i\n", 7);
    }
    else if (cxpCtxtEnvGetCount(pccT) != 3) {
      printf("Error %i\n", 8);
    }
    else if (cxpCtxtEnvDup(pccT,NULL) == FALSE) {
      printf("Error %i\n", 9);
    }
    else if (cxpCtxtEnvGetCount(pccT) != 0) {
      printf("Error %i\n", 10);
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    cxpCtxtLogInfo(pccT);
    cxpCtxtFree(pccT);
  }


#if 0
  if (RUNTEST) {
    int j = 0;
    cxpContextPtr pccT;
    
    i++;
    printf("TEST %i in '%s:%i': cxpCtxtDup() = ", i, __FILE__, __LINE__);
    
    if ((pccT = cxpCtxtDup(NULL)) == NULL) {
      printf("Error %i\n", 1);
    }
    else if (cxpCtxtEncSetEnv(pccT,"TEST","ABC") < 1) {
      printf("Error %i\n", 3);
    }
    else if (cxpCtxtEnvGetCount(pccT) != 1) {
      printf("Error %i\n", 5);
    }
    else if ((pucT = cxpCtxtEncGetEnv(pccT,"TEST","ABC")) == NULL) {
      printf("Error %i\n", 3);
    }
    else if (cxpCtxtEncSetEnv(pccT,"TEST",NULL) < 1) {
      printf("Error %i\n", 3);
    }
    else if (cxpCtxtEnvGetCount(pccT) != 0) {
      printf("Error %i\n", 5);
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    cxpCtxtLogInfo(pccT);
    cxpCtxtFree(pccT);
  }

  
  if (RUNTEST) {
    int j = 0;
    cxpContextPtr pccT;
    
    i++;
    printf("TEST %i in '%s:%i': cxpCtxtDup() = ", i, __FILE__, __LINE__);
    
    if ((pccT = cxpCtxtDup(NULL)) == NULL) {
      printf("Error %i\n", 1);
    }
    else if (cxpCtxtEnvDup(pccT,envp) == FALSE) {
      printf("Error %i\n", 2);
    }
    else if (cxpCtxtEnvGetCount(pccT) < 1) {
      printf("Error %i\n", 3);
    }
    else if (cxpCtxtEnvDup(pccT,NULL) == FALSE) {
      printf("Error %i\n", 4);
    }
    else if (cxpCtxtEnvGetCount(pccT) != 0) {
      printf("Error %i\n", 5);
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    cxpCtxtLogInfo(pccT);
    cxpCtxtFree(pccT);
  }

#endif
  
  printf("Result in '%s': %i/%i OK\n\n", __FILE__, n_ok, i);

  return (i - n_ok);
}
