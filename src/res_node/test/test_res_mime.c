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

/*! test code for contexts in module fs
 */
int
resNodeTestMime(void)
{
  int n_ok;
  int i;

  n_ok=0;
  i=0;

  if (RUNTEST) {

    i++;
    printf("TEST %i in '%s:%i': MIME undefined = ",i,__FILE__,__LINE__);
    if (resMimeGetTypeFromExt(NULL) == MIME_UNDEFINED && resMimeGetTypeFromExt(BAD_CAST"yyy") == MIME_UNKNOWN) {
	n_ok++;
	printf("OK\n");
      }
      else {
	printf("Error resMimeGetTypeFromExt() ...\n");
      }
  }

  if (RUNTEST) {

    i++;
    printf("TEST %i in '%s:%i': MIME defined = ", i, __FILE__, __LINE__);
    if (resMimeGetTypeFromExt(BAD_CAST"mm") == MIME_APPLICATION_MM_XML) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resMimeGetTypeFromExt() ...\n");
    }
  }

  if (RUNTEST) {

    i++;
    printf("TEST %i in '%s:%i': MIME Content-type = ", i, __FILE__, __LINE__);
    if (resMimeGetType("text/x-script.tcl") == MIME_TEXT_X_SCRIPT_TCL) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error resMimeGetType() ...\n");
    }
  }

  printf("\nResult in '%s': %i/%i OK\n\n", __FILE__, n_ok, i);

  return (i - n_ok);
}
