/* 
   cxproc - Configurable Xml PROCessor

   Copyright (C) 2006..2022 by Alexander Tenbusch

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  

*/

/*! CGI test frontend
*/

#include <stdlib.h>
#include <stdio.h>

int
main(int argc, char *argv[], char *envp[])
{
  /*
  this is the CGI test
  */
  int i;

  printf("Status: 200 OK\r\n"
    "Content-Type: text/plain;\r\n\r\n"
    "Hi, it's me\r\n\r\n");

  printf("\nargv: '%i'\n", argc);
  for (i=0; i < argc; i++) {
    printf("%i: '%s'\n", i, argv[i]);
  }

  printf("\nenv:\n");
  for (i=0; envp[i]; i++) {
    printf("%i: '%s'\n", i, envp[i]);
  }

  exit(EXIT_SUCCESS);
}
