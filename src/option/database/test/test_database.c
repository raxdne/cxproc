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
    pucT = dbTextReadStatement(BAD_CAST"CREATE TABLE 'first' ('id' text,'title' text);INSERT INTO 'first' VALUES('0', 'Crash');INSERT INTO 'first' VALUES('1', 'Test');");
    if (pucT != NULL && xmlStrlen(pucT)==46) {
      PrintFormatLog(1,"%s",pucT);
      printf("OK\n");
      n_ok++;
      xmlFree(pucT);
    }
    else {
      printf("Error dbTextReadStatement()\n");
    }
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

    if (iQuery == 3) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error dbTextReadStatement()\n");
    }
  }

  if (RUNTEST) {
    resNodePtr prnDb = NULL; /* The database */
    xmlDocPtr pdocResult = NULL;

    i++;
    printf("TEST %i in '%s:%i': dump an existing db to a DOM ",i,__FILE__,__LINE__);
    prnDb = resNodeDirNew(BAD_CAST TESTPREFIX "option/sql/test.db3");
    if (prnDb) {
      resNodeOpen(prnDb,"rd");
      pdocResult = dbDumpContextToDoc(prnDb,(DB_PROC_DECL|DB_PROC_ENTRIES));
      resNodeFree(prnDb);
      if (pdocResult) {
	//domPutDocString(stderr, NULL, pdocResult);
	printf("OK\n");
	n_ok++;
	xmlFreeDoc(pdocResult);
      }
      else {
	printf("Error dbDumpContextToDoc()\n");
      }
    }
  }

  if (RUNTEST) {
    resNodePtr prnDb = NULL; /* The database */

    i++;
    printf("TEST %i in '%s:%i': try to insert into a readonly database ",i,__FILE__,__LINE__);
    prnDb = resNodeDirNew(BAD_CAST TESTPREFIX "option/sql/test.db3");
    if (prnDb) {
      resNodeOpen(prnDb,"rd");
      if (dbInsert(prnDb,BAD_CAST"insert into meta (timestamp,key,value) values (0,'ERROR','TEST')") == FALSE) {
	printf("OK\n");
	n_ok++;
      }
      else {
	printf("Error resNodeOpenSqlite()\n");
      }
      resNodeFree(prnDb);
    }
  }

  printf("Result in '%s': %i/%i OK\n\n",__FILE__,n_ok,i);
#else
  printf("Skip TEST in '%s'\n\n",__FILE__);
#endif

  return (i - n_ok);
}
