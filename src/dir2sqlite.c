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

/*! command line frontend for resource nodes
*/

/*
 */
#include <libxml/parser.h>
#include <xmlzipio/xmlzipio.h>

/* 
 */
#include "basics.h"
#include "utils.h"
#include <res_node/res_node_io.h>
#include "dom.h"

#ifdef HAVE_LIBARCHIVE
#include <archive/archive.h>
#endif


/*  find -type f -iname '*.txt' | dir2sqlite | sqlite3 abc.db3

    dir2sqlite c:/UserData/Test | sqlite3 abc.db3

    dir2sqlite c:/UserData/Test// > Test.sqlite

 */
int
main(int argc, char *argv[], char *envp[])
{

  /*!\todo create SQL queries dynamically, usable in option/database/database.c */
  
  SetLogLevel(1);

  if (argc > 1 && strcmp(argv[1],"-?") == 0) {
    fprintf(stderr,"'%s' - write parsed directory data into a sqlite3 dump format\n\n",argv[0]);
    fprintf(stderr,"  'find -type f -iname '*.txt' | %s | sqlite3 abc.db3' - output of find command, parse directory data and write into a sqlite3 dump format\n\n",argv[0]);
    fprintf(stderr,"  '%s c:/UserData/Test | sqlite3 abc.db3' - output of find command, parse directory data and write into a sqlite3 dump format\n\n",argv[0]);
    fprintf(stderr,"  '%s c:/UserData/Test > Test.sqlite' - output of find command, parse directory data and write into a sqlite3 dump format\n\n",argv[0]);
  }
  else {
    int i;
    xmlChar *pucT;
    resNodePtr prnI;
    time_t system_zeit_1;

    /*! write sqlite declarations first */
    pucT = resNodeDatabaseSchemaStr();
    fputs((const char *)pucT,stdout);
    xmlFree(pucT);

    time(&system_zeit_1);
    fprintf(stdout,
	    "INSERT INTO 'meta' VALUES (%li,\"%s\",\"%s\");\n",
	    (long int)system_zeit_1, "parse/begin", "");
    
    fflush(stdout);
    if (argc > 1) {
      /* use program arguments as paths */

      for (i = 1, prnI = resNodeDirNew(NULL); i < argc; i++) {

	PrintFormatLog(4, "%s\n", argv[i]);

	if (resNodeReset(prnI, BAD_CAST argv[i]) == FALSE) {
	  PrintFormatLog(1, "%s\n", resNodeGetErrorMsg(prnI));
	}
	else if (resNodeUpdate(prnI, RN_INFO_INFO, NULL, NULL) == FALSE) {
	  PrintFormatLog(1, "%s\n", resNodeGetErrorMsg(prnI));
	}
	else {
	  resNodeListDumpRecursively(stdout, prnI, resNodeToSQL);
	}
      }
    }
    else {
      /* read paths from stdin */
      char mcLine[BUFFER_LENGTH];

      for (prnI = resNodeDirNew(NULL); fgets(mcLine, BUFFER_LENGTH, stdin) == mcLine; ) {
    
	for (i=strlen(mcLine); i > 0 && (mcLine[i] == '\0' || mcLine[i] == '\n' || mcLine[i] == '\r'); i--) {
	  mcLine[i] = '\0';
	}
	PrintFormatLog(3,"%s\n",mcLine);
    
	if (resNodeReset(prnI, BAD_CAST mcLine) == FALSE) {
	  PrintFormatLog(1, "%s\n", resNodeGetErrorMsg(prnI));
	}
	else if (resNodeUpdate(prnI, RN_INFO_INFO, NULL, NULL) == FALSE) {
	  PrintFormatLog(1, "%s\n", resNodeGetErrorMsg(prnI));
	}
	else {
	  resNodeListDumpRecursively(stdout, prnI, resNodeToSQL);
	}
	fflush(stdout);
      }
    }

    time(&system_zeit_1);
    fprintf(stdout,
	    "INSERT INTO 'meta' VALUES (%li,\"%s\",\"%s\");\n",
	    (long int)system_zeit_1, "parse/end", "");
  }
  
  exit(EXIT_SUCCESS);
}

