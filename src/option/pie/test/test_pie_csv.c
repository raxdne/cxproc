/*
  cxproc - Configurable Xml PROCessor

  Copyright (C) 2006..2024 by Alexander Tenbusch

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
pieTextCsvTest(void)
{
  int n_ok;
  int i;

  n_ok=0;
  i=0;

  CompileRegExpDefaults();

  if (RUNTEST) {
    xmlChar *pucData = BAD_CAST "1,\"ABC\",\"D,,,,,F\",11";
    xmlChar *pucResult;

    pucResult = pucData;
    i++;
    printf("TEST %i in '%s:%i': find separators in string = ",i,__FILE__,__LINE__);

    if (GetNextSeparatorPtr(NULL,",") != NULL) {
      printf("Error finding separators\n");
    }
    else if ((pucResult = GetNextSeparatorPtr(pucResult,",")) == NULL) {
      printf("Error finding separators\n");
    }
    else if (pucResult != &pucData[1]) {
      printf("Error finding separators\n");
    }
    else if ((pucResult = GetNextSeparatorPtr(&pucResult[1],",")) == NULL) {
      printf("Error finding separators\n");
    }
    else if (pucResult != &pucData[7]) {
      printf("Error finding separators\n");
    }
    else if ((pucResult = GetNextSeparatorPtr(&pucResult[1],",")) == NULL) {
      printf("Error finding separators\n");
    }
    else if (pucResult != &pucData[17]) {
      printf("Error finding separators\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }


  printf("Result in '%s': %i/%i OK\n\n", __FILE__, n_ok, i);

  return (i - n_ok);
}
/* end of pieTextCsvTest() */
