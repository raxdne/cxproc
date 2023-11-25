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


/*  find -type f -iname '*.txt' | dir2csv > abc.csv

    dir2csv c:/UserData/Test// > Test.csv

 */
int
main(int argc, char *argv[], char *envp[])
{

  /*!\todo use an argument as format string "%H:%M:%S;%N" (s. syntax of 'find -printf ""') */
  
  /*!\bug NTFS Junctions, Links etc (see %LOCALAPPDATA%%) */
  
  /*!\todo output of Byte-Order-Mark */
  
  SetLogLevel(1);

  if (argc > 1 && strcmp(argv[1],"-?") == 0) {
    fprintf(stderr,"'%s' - write parsed directory data into a CSV format\n\n",argv[0]);
    fprintf(stderr,"  'find -type f -iname '*.txt' | %s > abc.csv' - output of find command, parse directory data and write into a CSV format\n\n",argv[0]);
    fprintf(stderr,"  '%s c:/UserData/Test > abc.csv' - output of find command, parse directory data and write into a CSV format\n\n",argv[0]);
  }
  else {
    int i;
    resNodePtr prnI;

    /*! write CSV declarations first */
    fputs("sep=;\n", stdout);
    fputs("\"Mode\";\"Size [Byte]\";\"Recursive Size [Byte]\";\"# Childs\";\"MTime\";\"DiffTime [s]\";\"Name\";\"Extension\";\"Basedir\";\"Owner\";\"MIME\";\"Object Name\"\n", stdout);

    fflush(stdout);
    if (argc > 1) {
      /* use program arguments as paths */

      for (prnI = resNodeDirNew(NULL), i = 1; i < argc; i++) {

	PrintFormatLog(4, "%s\n", argv[i]);

	if (resNodeReset(prnI, BAD_CAST argv[i]) == FALSE) {
	  PrintFormatLog(1, "%s\n", resNodeGetErrorMsg(prnI));
	}
	else if (resNodeUpdate(prnI, RN_INFO_INFO, NULL, NULL) == FALSE) {
	  PrintFormatLog(1, "%s\n", resNodeGetErrorMsg(prnI));
	}
	else {
	  resNodeListDumpRecursively(stdout, prnI, resNodeToCSV);
	}
      }
    }
    else {
      /* read paths from stdin */
      char mcLine[BUFFER_LENGTH];

      for (prnI = resNodeDirNew(NULL); fgets(mcLine, BUFFER_LENGTH, stdin) == mcLine; ) {

	for (i = strlen(mcLine); i > 0 && (mcLine[i] == '\0' || mcLine[i] == '\n' || mcLine[i] == '\r'); i--) {
	  mcLine[i] = '\0';
	}
	PrintFormatLog(4,"%s\n",mcLine);

	if (resNodeReset(prnI, BAD_CAST mcLine) == FALSE) {
	  PrintFormatLog(1, "%s\n", resNodeGetErrorMsg(prnI));
	}
	else if (resNodeUpdate(prnI, RN_INFO_INFO, NULL, NULL) == FALSE) {
	  PrintFormatLog(1, "%s\n", resNodeGetErrorMsg(prnI));
	}
	else {
	  resNodeListDumpRecursively(stdout, prnI, resNodeToCSV);
	}
	fflush(stdout);
      }
    }
    resNodeListFree(prnI);
  }

  exit(EXIT_SUCCESS);
}

