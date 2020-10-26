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


int
icsTest(void)
{
  int n_ok;
  int i;

  n_ok=0;
  i=0;

  if (RUNTEST) {
    
    i++;
    printf("TEST %i in '%s:%i': empty icsParse() = ",i,__FILE__,__LINE__);

    if (icsParse(NULL,NULL) == TRUE) {
      printf("Error icsParse()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    xmlDocPtr pdocResult = NULL;
    xmlNodePtr pndFile;
    resNodePtr prnIcs = NULL;
    
    i++;
    printf("TEST %i in '%s:%i': icsParse() = ",i,__FILE__,__LINE__);

    pdocResult = xmlNewDoc(BAD_CAST "1.0");
    pndFile = xmlNewDocNode(pdocResult, NULL, NAME_FILE, NULL); 
    xmlDocSetRootElement(pdocResult,pndFile);
    pdocResult->encoding = xmlStrdup(BAD_CAST "UTF-8"); /* according to conversion in ParseImportNodePlainContent() */

    if ((prnIcs = resNodeConcatNew(BAD_CAST TESTPREFIX, BAD_CAST "option/ics/2446.ics")) == NULL) {
      printf("Error icsParse()\n");
    }
    else if (icsParse(pndFile,prnIcs) == FALSE) {
      printf("Error icsParse()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    xmlSaveFormatFileEnc(TEMPPREFIX "2446.pie", pdocResult, "UTF-8", 1);
    xmlFreeDoc(pdocResult);
    resNodeFree(prnIcs);
  }

  printf("Result in '%s': %i/%i OK\n\n",__FILE__,n_ok,i);

  return (i - n_ok);
}
