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
dbTest(void)
{
  int n_ok;
  int i;

  n_ok=0;
  i=0;

#ifdef HAVE_LIBSQLITE3
  if (RUNTEST) {
    
    xmlChar *pucT;

    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);
    
    
    if ((pucT = dbTextReadStatement(BAD_CAST"CREATE TABLE 'first' ('id' text,'title' text);INSERT INTO 'first' VALUES('0', 'Crash');INSERT INTO 'first' VALUES('1', 'Test');")) == NULL) {
      printf("Error 1 dbTextReadStatement()\n");
    }
    else if (xmlStrlen(pucT) != 46) {
      printf("Error 2 dbTextReadStatement()\n");
      PrintFormatLog(1,"%s",pucT);
    }
    else {
      printf("OK\n");
      n_ok++;
    }
    xmlFree(pucT);
  }

  if (RUNTEST) {
    xmlChar *pucQuery;
    xmlChar *pucQueryTail;
    xmlChar *pucStatement;
    int iQuery;

    i++;
    printf("TEST %i in '%s:%i': dbTextReadStatement() = ",i,__FILE__,__LINE__);

    pucQuery = BAD_CAST"\nSELECT * FROM test ; SELECT ';' FROM test WHERE t1.p LIKE '%%AAA%%';  \n;\nSELECT * FROM test   ";
    pucQueryTail = pucQuery;
    pucStatement = dbTextReadStatement(pucQueryTail);
    for (iQuery=0; pucStatement; iQuery++) {
      pucQueryTail += xmlStrlen(pucStatement);
      if (*pucQueryTail) {
	pucQueryTail++; /* skip trailing ';' */
      }
      xmlFree(pucStatement);
      pucStatement = dbTextReadStatement(pucQueryTail);
    }

    if (iQuery != 3) {
      printf("Error dbTextReadStatement()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    resNodePtr prnDb = NULL; /* The database */
    xmlDocPtr pdocResult = NULL;

    i++;
    printf("TEST %i in '%s:%i': dump an existing db to a DOM ",i,__FILE__,__LINE__);
    
    if ((prnDb = resNodeDirNew(BAD_CAST TESTPREFIX "option/sql/test.db3")) == NULL) {
	printf("Error dbDumpContextToDoc()\n");
    }
    else if (resNodeOpen(prnDb, "rd") == FALSE) {
	printf("Error dbDumpContextToDoc()\n");
    }
    else if ((pdocResult = dbDumpContextToDoc(prnDb,(DB_PROC_DECL|DB_PROC_ENTRIES))) == NULL) {
	printf("Error dbDumpContextToDoc()\n");
    }
    else {
      printf("OK\n");
      n_ok++;
    }
    //domPutDocString(stderr, NULL, pdocResult);
    xmlFreeDoc(pdocResult);
    resNodeFree(prnDb);
  }

  if (RUNTEST) {
    resNodePtr prnDb = NULL; /* The database */

    i++;
    printf("TEST %i in '%s:%i': try to insert into a readonly database ",i,__FILE__,__LINE__);
    
    
    if ((prnDb = resNodeDirNew(BAD_CAST TESTPREFIX "option/sql/test.db3")) == NULL) {
      printf("Error resNodeDirNew()\n");
    }
    else if (resNodeOpen(prnDb, "rd") == FALSE) {
      printf("Error resNodeOpen()\n");
    }
    else if (dbInsert(prnDb,BAD_CAST"insert into meta (timestamp,key,value) values (0,'ERROR','TEST')") == TRUE) {
      printf("Error dbInsert() on read only database\n");
    }
    else {
      printf("OK\n");
      n_ok++;
    }
    resNodeFree(prnDb);
  }

  printf("Result in '%s': %i/%i OK\n\n",__FILE__,n_ok,i);
#else
  printf("Skip TEST in '%s'\n\n",__FILE__);
#endif

  return (i - n_ok);
}
