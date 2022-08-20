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
pieTimezoneTest(void)
{
  int n_ok;
  int i;

  n_ok=0;
  i=0;

  if (SKIPTEST) {

    i++;
    printf("TEST %i in '%s:%i': tzGetRuntimeOffsetToUTC() = ",i,__FILE__,__LINE__);

    if (tzGetRuntimeOffsetToUTC() == +60) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error\n");
    }
  }

  if (RUNTEST) {
    int z = PIE_TZ_CHADT;

    i++;
    printf("TEST %i in '%s:%i': tzGetNumber() = ", i, __FILE__, __LINE__);

    if (tzGetNumber(tzGetId(z)) == PIE_TZ_CHADT) {
      printf("%i\t'%s'\t'%s'\t%i\t%i\n", z, tzGetId(z), tzGetName(z), tzGetNumber(tzGetId(z)), tzGetOffset(z));
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error\n");
    }
  }

  printf("Result in '%s': %i/%i OK\n\n", __FILE__, n_ok, i);

  return (i - n_ok);
}
/* end of pieTextTest() */
