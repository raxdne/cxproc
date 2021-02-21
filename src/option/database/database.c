/* 
   cxproc - Configurable Xml PROCessor
   
   Copyright (C) 2006..2020 by Alexander Tenbusch

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

/*! SQLite frontend

- process database queries
- insert data
- dump existing databases into a DOM
- parse filesystem directories into a database

\todo direct import of CSV, XML into db?
\todo caching of db (in-memory?)
\todo direct output of query result as JSON?
\todo parsing of ZIP files (list content)
 */

#include <libxml/tree.h>

/* 
 */
#include "basics.h"
#include "utils.h"
#include <res_node/res_node_io.h>
#include <pie/calendar_element.h>
#include <cxp/cxp.h>
#include "dom.h"
#include <cxp/cxp_dir.h>
#include <database/database.h>
#include "plain_text.h"

static BOOL_T
TableExists(resNodePtr prnArgDb, xmlChar *pucArgName);

static xmlChar *
dbGetColDeclarationsToPlain(sqlite3_stmt *pStmt);

static xmlChar *
dbGetEntriesToPlain(sqlite3_stmt *pStmt);

static BOOL_T
dbGetColDeclarationsToNode(xmlNodePtr pndArgParent, sqlite3_stmt *pStmt);

static BOOL_T
dbGetEntriesToNode(xmlNodePtr pndArgParent, sqlite3_stmt *pStmt);


/*! \return a checked string of the next SQL statement
*/
xmlChar *
dbTextReadStatement(xmlChar *pucArg)
{
  xmlChar *pucResult = NULL;;

  if (pucArg) {
    xmlChar *pucStart;
    xmlChar *pucEnd;

    for (pucStart=pucArg; *pucStart == '\n' || *pucStart == ';' || *pucStart == ' '; pucStart++) {
      /* skip leading newline etc */
    }

    /* detect statement, check string /^SELECT +/ for security reason (SQL injection?) */
    if (xmlStrcasestr(pucStart,BAD_CAST"CREATE ") == pucStart
	|| xmlStrcasestr(pucStart,BAD_CAST"INSERT ") == pucStart
	|| xmlStrcasestr(pucStart,BAD_CAST"UPDATE ") == pucStart
	|| xmlStrcasestr(pucStart,BAD_CAST"REPLACE ") == pucStart
	|| xmlStrcasestr(pucStart,BAD_CAST"WITH ") == pucStart
	|| xmlStrcasestr(pucStart,BAD_CAST"SELECT ") == pucStart) {
      for (pucEnd = pucStart + 5; ; pucEnd++) {
	/* */
	if (isend(*pucEnd)) {
	  break;
	}
	else if (*pucEnd == '"') {
	  for (pucEnd++; ! isend(*pucEnd) && *pucEnd != '"'; pucEnd++) {
	    /* skip leading newline etc */
	  }
	}
	else if (*pucEnd == '\'') {
	  for (pucEnd++; ! isend(*pucEnd) && *pucEnd != '\''; pucEnd++) {
	    /* skip leading newline etc */
	  }
	}
	else if (*pucEnd == ';') {
	  pucEnd++;
	  break;
	}
	else {
	}
      }
      if (pucEnd - pucStart > 7) {
	pucResult = xmlStrndup(pucStart,pucEnd - pucStart);
      }
    }

  }

  return pucResult;
}
/* end of dbTextReadStatement() */


/*! adds all column declarations of this statement
*/
BOOL_T
dbGetColDeclarationsToNode(xmlNodePtr pndArgParent, sqlite3_stmt *pStmt)
{
  int ncols;

  ncols = sqlite3_column_count(pStmt);
  if (ncols > 0) {
    int i;
    xmlNodePtr pndDecl;

    pndDecl = xmlNewChild(pndArgParent, NULL, BAD_CAST"declaration", NULL);
    for (i=0; i<ncols; i++) {
      xmlChar *pucT = NULL;
      xmlChar mucT[BUFFER_LENGTH];
      xmlNodePtr pndCol = xmlNewChild(pndDecl, NULL, BAD_CAST"column", NULL);
      xmlStrPrintf(mucT,BUFFER_LENGTH-1, "%i",i);
      xmlSetProp(pndCol, BAD_CAST "nr", mucT);
      pucT = xmlStrdup(BAD_CAST sqlite3_column_name(pStmt,i));
      xmlSetProp(pndCol, BAD_CAST "name", pucT);
      StringToId((char *)pucT);
      domSetPropEat(pndCol, BAD_CAST "id", pucT);
      xmlSetProp(pndCol, BAD_CAST "type", BAD_CAST sqlite3_column_decltype(pStmt,i));
    }
  }
  return TRUE;
}
/* end of dbGetColDeclarationsToNode() */


/*! adds the column declarations of table 'pucArgName' at database 'pdbArg' as child to 'pndArgParent'

\todo error handling
*/
BOOL_T
dbGetEntriesToNode(xmlNodePtr pndArgParent, sqlite3_stmt *pStmt)
{
  int ncols;
  int iCol;
  int iRow;

  ncols = sqlite3_column_count(pStmt);
  for (iRow=0; iRow < 10e6; iRow++) {
    int iCodeState;
    xmlChar mucT[BUFFER_LENGTH];
    xmlNodePtr pndRow = xmlNewChild(pndArgParent, NULL, BAD_CAST"entry", NULL);

    iCodeState = sqlite3_step (pStmt);
    if (iCodeState == SQLITE_ROW) {
      PrintFormatLog(4,"Append query result entry '%i'",iRow);
      xmlStrPrintf(mucT,BUFFER_LENGTH-1,"%i",iRow);
      xmlSetProp(pndRow, BAD_CAST "nr", mucT);
      for (iCol=0; iCol<ncols; iCol++) {
	const xmlChar *pucT = sqlite3_column_text(pStmt,iCol);
	xmlChar *pucTT = ReplaceStr(pucT,BAD_CAST"&",  BAD_CAST"&amp;");
	/*!\todo replace other XML entities */
	if (pucTT) {
	  xmlChar *pucTTT = xmlStrdup(BAD_CAST sqlite3_column_name(pStmt,iCol));
	  StringToId((char *)pucTTT);
	  xmlNewChild(pndRow, NULL, pucTTT, pucTT);
	  xmlFree(pucTTT);
	  xmlFree(pucTT);
	}
	else {
	  xmlChar *pucTTT = xmlStrdup(BAD_CAST sqlite3_column_name(pStmt,iCol));
	  StringToId((char *)pucTTT);
	  xmlNewChild(pndRow, NULL, pucTTT, pucT);
	  xmlFree(pucTTT);
	}
      }
    }
    else if (iCodeState == SQLITE_DONE) {
      break;
    }
    else {
      PrintFormatLog(1,"SQLite error at row %i",iRow);
      break;
    }
  }
  return TRUE;
}
/* end of dbGetEntriesToNode() */


/*! 
*/
xmlNodePtr
dbProcessQueryToNode(xmlNodePtr pndArgParent, resNodePtr prnArgDb, xmlChar *pucArgQuery, int iOptions)
{
  int rc = 0;
  sqlite3_stmt *pStmt;
  xmlNodePtr pndResult = NULL;
  xmlChar *pucQueryTail;
  xmlChar *pucStatement;
  int iQuery;

  pucQueryTail = pucArgQuery;
  pucStatement = dbTextReadStatement(pucQueryTail);
  for (iQuery=0; pucStatement; iQuery++) {

#ifdef DEBUG
    PrintFormatLog(2, "Next SQL statement '%s'",pucStatement);
#endif

    pndResult = xmlNewChild(pndArgParent, NULL, BAD_CAST"table", NULL);
    xmlSetProp(pndResult, BAD_CAST "query", pucArgQuery);

    rc = sqlite3_prepare_v2((sqlite3 *)resNodeGetHandleIO(prnArgDb), (const char *)pucStatement, -1, &pStmt, 0);
    if (rc == SQLITE_OK) {
      if (iOptions & DB_PROC_DECL) {
	dbGetColDeclarationsToNode(pndResult,pStmt);
      }
      if (iOptions & DB_PROC_ENTRIES) {
        dbGetEntriesToNode(pndResult,pStmt);
      }
    }
    else {
      PrintFormatLog(1,"SQL error");
    }

    sqlite3_finalize(pStmt);

    for (pucQueryTail += xmlStrlen(pucStatement); *pucQueryTail == ';'; pucQueryTail++); /* skip trailing ';' */
    xmlFree(pucStatement);
    pucStatement = dbTextReadStatement(pucQueryTail);
  }

  return pndResult;
}
/* end of dbProcessQueryToNode() */


/*! 
 \todo change to processing of a "COMMIT;" terminated string?
*/
BOOL_T
dbInsert(resNodePtr prnArgDb, xmlChar *pucArgQuery)
{
  xmlChar *pucQueryTail = pucArgQuery;
  xmlChar *pucQuery;
  sqlite3 *pdbContext;
  int iQuery;
  BOOL_T fResult = TRUE;

  pdbContext = (sqlite3 *)resNodeGetHandleIO(prnArgDb);
  pucQuery = dbTextReadStatement(pucArgQuery);
  for (iQuery=0; pucQuery; iQuery++) {
    int rc = 0;
    sqlite3_stmt *pStmt;

    PrintFormatLog(4,"Query '%s'", pucQuery);

    rc = sqlite3_prepare_v2(pdbContext, (const char *)pucQuery, xmlStrlen(pucQuery), &pStmt, NULL);
    if (rc == SQLITE_OK) {
      int iRow;

      for (iRow=0; iRow < 10e6; iRow++) {
	int iCodeState;

	iCodeState = sqlite3_step (pStmt);
	if (iCodeState == SQLITE_ROW) {
	  PrintFormatLog(4,"Insert query '%i'",iRow);
	}
	else if (iCodeState == SQLITE_DONE) {
	  break;
	}
	else {
	  dbInsertMetaLog(prnArgDb, BAD_CAST"error/query", pucQuery);
	  fResult = FALSE;
	  break;
	}
      }
    }
    else {
      dbInsertMetaLog(prnArgDb, BAD_CAST"error/query+prepare", pucQuery);
      fResult = FALSE;
    }
    sqlite3_finalize(pStmt);

    for (pucQueryTail += xmlStrlen(pucQuery); *pucQueryTail == ';'; pucQueryTail++); /* skip trailing ';' */
    xmlFree(pucQuery);
    pucQuery = dbTextReadStatement(pucQueryTail);
  }

  return fResult;
}
/* end of dbInsert() */


/* dump database context tables

\param prnArgDb pointer to database context
\param iOptions flag for

\return TRUE

derived from "sqlite3/shell.c"
*/
BOOL_T
dbDumpContextToNode(xmlNodePtr pndArg, resNodePtr prnArg, int iOptions)
{
  BOOL_T fResult = FALSE;
  BOOL_T fOpenedHere = FALSE;
  char *zSql = 0;
  int rc = 0;
  sqlite3 *pdbContext;
  sqlite3_stmt *pStmt;

  if (resNodeGetHandleIO(prnArg) == NULL) {
    fOpenedHere = resNodeOpen(prnArg, "rd");
  }
  pdbContext = (sqlite3 *)resNodeGetHandleIO(prnArg);

  if (pdbContext==NULL || SQLITE_OK != sqlite3_errcode(pdbContext)) {
    resNodeSetError(prnArg, rn_error_access, "Error: unable to open database \"%s\": %s\n",
      resNodeGetNameNormalized(prnArg), sqlite3_errmsg(pdbContext));
  }
  else if ((rc = sqlite3_prepare_v2(pdbContext, "PRAGMA database_list", -1, &pStmt, 0)) == 0) {
    assert(pdbContext != NULL && sqlite3_errcode(pdbContext) == SQLITE_OK);

    zSql = sqlite3_mprintf(
      "SELECT name FROM sqlite_master"
      " WHERE type IN ('table','view')"
      "   AND name NOT LIKE 'sqlite_%%'"
      "   AND name LIKE ?1");

    while (sqlite3_step(pStmt)==SQLITE_ROW) {
      const char *zDbName = (const char*)sqlite3_column_text(pStmt, 1);

      if (zDbName==0 || strcmp(zDbName, "main")==0) {
	continue;
      }

      if (strcmp(zDbName, "temp")==0) {
	zSql = sqlite3_mprintf(
	  "%z UNION ALL "
	  "SELECT 'temp.' || name FROM sqlite_temp_master"
	  " WHERE type IN ('table','view')"
	  "   AND name NOT LIKE 'sqlite_%%'"
	  "   AND name LIKE ?1", zSql);
      }
      else {
	zSql = sqlite3_mprintf(
	  "%z UNION ALL "
	  "SELECT '%q.' || name FROM \"%w\".sqlite_master"
	  " WHERE type IN ('table','view')"
	  "   AND name NOT LIKE 'sqlite_%%'"
	  "   AND name LIKE ?1", zSql, zDbName, zDbName);
      }
    }
    sqlite3_finalize(pStmt);
    zSql = sqlite3_mprintf("%z ORDER BY 1", zSql);
    rc = sqlite3_prepare_v2(pdbContext, zSql, -1, &pStmt, 0);
    sqlite3_free(zSql);
    if (rc == 0) {
      xmlNodePtr pndDb;

      pndDb = xmlNewChild(pndArg, NULL, BAD_CAST"db", NULL);

      sqlite3_bind_text(pStmt, 1, "%", -1, SQLITE_STATIC);
      while (sqlite3_step(pStmt)==SQLITE_ROW) {
	xmlChar mucQuery[BUFFER_LENGTH];
	xmlChar *pucNameTable = BAD_CAST sqlite3_mprintf("%s", sqlite3_column_text(pStmt, 0));
	xmlNodePtr pndTable;

	//PrintFormatLog(2,"Database table '%s'", pucNameTable);
	xmlStrPrintf(mucQuery, BUFFER_LENGTH, "SELECT * FROM %s;", pucNameTable);
	pndTable = dbProcessQueryToNode(pndDb, prnArg, mucQuery, iOptions);
	if (pndTable) {
	  xmlUnsetProp(pndTable, BAD_CAST"query");
	  xmlSetProp(pndTable, BAD_CAST"name", pucNameTable);
	}
	sqlite3_free(pucNameTable);
      }
      fResult = TRUE;
    }
    else {
      resNodeSetError(prnArg, rn_error_access, "Error: %s\n", sqlite3_errmsg(pdbContext));
    }
    sqlite3_finalize(pStmt);
  }
  else {
    resNodeSetError(prnArg, rn_error_access, "Error: %s\n", sqlite3_errmsg(pdbContext));
  }

  if (fOpenedHere) {
    resNodeClose(prnArg);
  }

  return fResult;
}
/* end of dbDumpContextToNode() */


/*!\return TRUE if database table 'pucArgName' exists
*/
BOOL_T
TableExists(resNodePtr prnArgDb, xmlChar *pucArgName)
{
  BOOL_T fResult = FALSE;
  char mchQuery[BUFFER_LENGTH];

  assert(resNodeGetHandleIO(prnArgDb) != NULL);

  if (xmlStrPrintf(BAD_CAST mchQuery,BUFFER_LENGTH-1,"pragma table_info(%s);",pucArgName) > -1) {
    int rc = 0;
    sqlite3_stmt *pStmt;
    rc = sqlite3_prepare_v2((sqlite3 *)resNodeGetHandleIO(prnArgDb), (const char *)mchQuery, -1, &pStmt, NULL );
    if(rc == SQLITE_OK) {
      rc = sqlite3_step(pStmt);
      if(rc == SQLITE_DONE || rc == SQLITE_ROW) {
	fResult = (sqlite3_data_count(pStmt) > 0);
      }
    }
    sqlite3_finalize(pStmt);
  }
  return fResult;
}
/* end of TableExists() */


/*! create database table for resNodes
 *
 \todo fill tables mimetpes, meta and queries only if not existing
*/
BOOL_T
dbParseDirCreateTables(resNodePtr prnArgDb)
{
  BOOL_T fResult = TRUE;
  xmlChar *pucSql = NULL;
  xmlChar mpucT[BUFFER_LENGTH];
  char* zErr = NULL;
  int rc;
  int i;
  sqlite3 *pdbContext;

  pdbContext = (sqlite3 *)resNodeGetHandleIO(prnArgDb);

  if (TableExists(prnArgDb,BAD_CAST"meta") == FALSE) {
    pucSql = BAD_CAST"create table meta(i INTEGER PRIMARY KEY, timestamp INTEGER, key text, value text);";
    rc = sqlite3_exec(pdbContext, (const char *)pucSql, NULL, NULL, &zErr);
    if (rc == SQLITE_OK) {
    }
    else if (zErr) {
      PrintFormatLog(1, "SQL error 'meta': %s\n", zErr);
      sqlite3_free(zErr);
      fResult = FALSE;
    }
  }
  dbInsertMetaLog(prnArgDb, BAD_CAST"log/create",NULL);

  if (TableExists(prnArgDb,BAD_CAST"directory") == FALSE) {
    pucSql = BAD_CAST"create table directory("
      "i INTEGER PRIMARY KEY, "
      "depth INTEGER, "
      "type INTEGER, "
      "mime INTEGER, "
      "r INTEGER, "
      "w INTEGER, "
      "x INTEGER, "
      "h INTEGER, "
#if 0
      "owner text, "
#endif
      "name text, "
      "ext text, "
      "object text, "
      "size INTEGER, "
      "rsize INTEGER, "
      "path text, "
      "mtime INTEGER, "
      "mtime2 text"
      ");";
    rc = sqlite3_exec(pdbContext, (const char *)pucSql, NULL, NULL, &zErr);
    if (rc == SQLITE_OK) {
    }
    else if (zErr) {
      PrintFormatLog(1, "SQL error 'directory': %s\n", zErr);
      sqlite3_free(zErr);
      fResult = FALSE;
    }
  }

  if (TableExists(prnArgDb,BAD_CAST"mimetypes") == FALSE) {
    pucSql = xmlStrdup(BAD_CAST"create table mimetypes(mime INTEGER, name text);");
    for (i=MIME_UNKNOWN; i < MIME_END; i++) {
      char *pcMime;
	
      pcMime = (char *)resMimeGetTypeStr(i);
      if (pcMime) {
	xmlStrPrintf(mpucT,BUFFER_LENGTH, "insert into mimetypes(mime,name) values (%i,\"%s\");", i, BAD_CAST pcMime);
	pucSql = xmlStrcat(pucSql,mpucT);
      }
    }

    rc = sqlite3_exec(pdbContext, (const char *)pucSql, NULL, NULL, &zErr);
    if (rc == SQLITE_OK) {
    }
    else if (zErr) {
      PrintFormatLog(1, "SQL error 'mimetypes': %s\n", zErr);
      sqlite3_free(zErr);
      fResult = FALSE;
    }
    xmlFree(pucSql);
  }
  
  if (TableExists(prnArgDb,BAD_CAST"queries") == FALSE) {
    pucSql = BAD_CAST"create table queries(query text);"
      "insert into queries(query) values (\"SELECT * FROM meta;\");"
      "insert into queries(query) values (\"SELECT DISTINCT name FROM directory;\");"
      "insert into queries(query) values (\"SELECT sum(size)/(1024*1024*1024) AS GB FROM directory;\");"
      "insert into queries(query) values (\"SELECT path || '/' || name AS File,(size / 1048576) AS MB,mtime2 AS MTime FROM directory WHERE (size > 1048576) ORDER BY MB DESC;\");"
      "insert into queries(query) values (\"SELECT count() AS Count, name AS Name FROM directory GROUP BY name ORDER BY Count DESC;\");";
    rc = sqlite3_exec(pdbContext, (const char *)pucSql, NULL, NULL, &zErr);
    if (rc == SQLITE_OK) {
    }
    else if (zErr) {
      PrintFormatLog(1, "SQL error 'queries': %s\n", zErr);
      sqlite3_free(zErr);
      fResult = FALSE;
    }
  }

  return fResult;
} /* end of dbParseDirCreateTables() */


/*!
*/
BOOL_T
dbInsertMetaLog(resNodePtr prnArgDb, xmlChar *pucArgKey, xmlChar *pucValue)
{
  xmlChar mpucT[BUFFER_LENGTH];
  xmlChar mpucOut[BUFFER_LENGTH];
  char* zErr = NULL;
  int rc;
  time_t system_zeit_1;

  if (pucArgKey == NULL) {
    /* add readable time */
    pucArgKey = BAD_CAST"log/timestamp";
  }

  time(&system_zeit_1);
  if (pucValue == NULL) {
    /* add readable time */
    xmlStrPrintf(mpucT,BUFFER_LENGTH, "%s",ctime(&system_zeit_1));
    chomp(mpucT);
    pucValue = mpucT;
  }

  PrintFormatLog(2,"Database log '%s': '%s'", pucArgKey, pucValue);
  xmlStrPrintf(mpucOut,BUFFER_LENGTH,
    "insert into meta (timestamp,key,value) values (%li,\"%s\",\"%s\")",
    (long int)system_zeit_1, pucArgKey, pucValue );

  rc = sqlite3_exec((sqlite3 *)resNodeGetHandleIO(prnArgDb), (const char *)mpucOut, NULL, NULL, &zErr);
  if (rc == SQLITE_OK) {
  }
  else if (zErr) {
    PrintFormatLog(1, "SQL error: %s -> %s", mpucOut, zErr);
    sqlite3_free(zErr);
    return FALSE;
  }
  return TRUE;
}
/* end of dbInsertMetaLog() */


/*! adds all column declarations of this statement
*/
xmlChar *
dbGetColDeclarationsToPlain(sqlite3_stmt *pStmt)
{
  xmlChar *pucResult = NULL;
  int ncols;

  ncols = sqlite3_column_count(pStmt);
  if (ncols > 0) {
    int iCol;

    PrintFormatLog(2,"Read column declarations '%s'","");
    for (iCol=0; iCol<ncols; iCol++) {
      if (iCol > 0) {
	pucResult = xmlStrcat(pucResult,BAD_CAST"\t");
      }
      pucResult = xmlStrcat(pucResult,BAD_CAST sqlite3_column_name(pStmt,iCol));
    }
    pucResult = xmlStrcat(pucResult,BAD_CAST"\n");
  }

  return pucResult;
}
/* end of dbGetColDeclarationsToPlain() */


/*! adds the column declarations of table 'pucArgName' at database 'pdbArg' as child to 'pndArgParent'

\todo error handling
*/
xmlChar *
dbGetEntriesToPlain(sqlite3_stmt *pStmt)
{
  xmlChar *pucResult = NULL;
  int ncols;
  int iCol;
  int iRow;

  PrintFormatLog(2,"Append query result '%s'","");

  ncols = sqlite3_column_count(pStmt);
  if (ncols > 0) {
    for (iRow=0; iRow < 10e6; iRow++) {
      int iCodeState;

      iCodeState = sqlite3_step (pStmt);
      if (iCodeState == SQLITE_ROW) {
	PrintFormatLog(4,"Append query result entry '%i'",iRow);
	for (iCol=0; iCol<ncols; iCol++) {
	  if (iCol > 0) {
	    pucResult = xmlStrcat(pucResult,BAD_CAST"\t");
	  }
	  pucResult = xmlStrcat(pucResult,sqlite3_column_text(pStmt,iCol));
	}
	pucResult = xmlStrcat(pucResult,BAD_CAST"\n");
      }
      else if (iCodeState == SQLITE_DONE) {
	break;
      }
      else {
	PrintFormatLog(1,"SQLite error at row %i",iRow);
	break;
      }
    }
  }
  return pucResult;
}
/* end of dbGetEntriesToPlain() */


/*!
*/
xmlChar *
dbProcessQueryToPlain(resNodePtr prnArgDb, xmlChar *pucArgQuery, int iOptions)
{
  int rc = 0;
  sqlite3_stmt *pStmt;
  xmlChar *pucResult = NULL;
  xmlChar *pucQueryTail;
  xmlChar *pucStatement;
  int iQuery;

  pucQueryTail = pucArgQuery;
  pucStatement = dbTextReadStatement(pucQueryTail);
  for (iQuery=0; pucStatement; iQuery++) {

#ifdef DEBUG
    PrintFormatLog(2, "Next SQL statement '%s'",pucStatement);
#endif

    rc = sqlite3_prepare_v2((sqlite3 *)resNodeGetHandleIO(prnArgDb), (const char *)pucStatement, -1, &pStmt, 0);
    if (rc == SQLITE_OK) {
      if (iOptions & DB_PROC_DECL) {
	pucResult = xmlStrcat(pucResult,dbGetColDeclarationsToPlain(pStmt));
      }
      if (iOptions & DB_PROC_ENTRIES) {
	pucResult = xmlStrcat(pucResult,dbGetEntriesToPlain(pStmt));
	if (iOptions & DB_PROC_LINES) {
	  pucResult = xmlStrcat(pucResult,BAD_CAST"\n");
	}
      }
    }
    else {
      PrintFormatLog(1,"SQL error");
    }

    sqlite3_finalize(pStmt);

    for (pucQueryTail += xmlStrlen(pucStatement); *pucQueryTail == ';'; pucQueryTail++); /* skip trailing ';' */
    xmlFree(pucStatement);
    pucStatement = dbTextReadStatement(pucQueryTail);
  }

  return pucResult;
}
/* end of dbProcessQueryToPlain() */


/* creates a new Document and dump database context tables

\param prnArgDb pointer to database context
\param iOptions flag for

\return pointer to result DOM
*/
xmlDocPtr
dbDumpContextToDoc(resNodePtr prnArgDb, int iOptions)
{
  xmlDocPtr pdocResult = NULL;
  xmlNodePtr pndMeta;
  xmlNodePtr pndSql;
  xmlChar mpucT[BUFFER_LENGTH];
  sqlite3 *pdbContext;

  pdbContext = (sqlite3 *)resNodeGetHandleIO(prnArgDb);
  if (pdbContext != NULL && sqlite3_errcode(pdbContext) == SQLITE_OK) {
    assert( pdbContext != NULL && sqlite3_errcode(pdbContext) == SQLITE_OK );

    /*! create DOM
    */
    pdocResult = xmlNewDoc(BAD_CAST "1.0");
    pndSql = xmlNewDocNode(pdocResult, NULL, BAD_CAST "sql", NULL); 
    xmlDocSetRootElement(pdocResult, pndSql);

    pndMeta = xmlNewChild(pndSql, NULL, NAME_META, NULL);
    //xmlAddChild(pndMeta,xmlCopyNode(pndMakeSql,1));
    /* Get the current time. */
    xmlStrPrintf(mpucT,BUFFER_LENGTH, "%i", GetTodayTime());
    xmlSetProp(pndMeta, BAD_CAST "ctime", mpucT);
    xmlSetProp(pndMeta, BAD_CAST "ctime2", GetTodayTag());
    // cxpInfoProgram(pndMeta, NULL);

    dbDumpContextToNode(pndSql,prnArgDb,iOptions);
  }
  return pdocResult;
}
/* end of dbDumpContextToDoc() */


/* \return pointer to result DOM
*/
xmlDocPtr
GetXslInsertAsPlain(void)
{
  xmlDocPtr pdocResult = NULL;

  const char *pcXsl =
      "<?xml version=\"1.0\"?>"
      "<xsl:stylesheet xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\" version=\"1.0\">"
      "  <xsl:output method=\"text\" encoding=\"UTF-8\"/>"
      "  <xsl:template match=\"/\">"
      "    <xsl:variable name=\"str_table\" select=\"name(*[1])\"/>"
      "    <xsl:value-of select=\"concat('CREATE TABLE IF NOT EXISTS ',$str_table,'(')\"/>"
      "    <xsl:for-each select=\"*/*[1]\">"
      "      <xsl:choose>"
      "        <xsl:when test=\"@valid = 'no'\"/>"
      "        <xsl:otherwise>"
      "          <xsl:for-each select=\"*\">"
      "            <xsl:if test=\"position() &gt; 1\">"
      "              <xsl:text>,</xsl:text>"
      "            </xsl:if>"
      "            <xsl:choose>"
      "              <xsl:when test=\"@type\">"
      "                <xsl:value-of select=\"concat(name(.),' ',@type)\"/>"
      "              </xsl:when>"
      "              <xsl:otherwise>"
      "                <xsl:value-of select=\"concat(name(.),' text')\"/>"
      "              </xsl:otherwise>"
      "            </xsl:choose>"
      "          </xsl:for-each>"
      "          <xsl:value-of select=\"concat(');','&#10;')\"/>"
      "        </xsl:otherwise>"
      "      </xsl:choose>"
      "    </xsl:for-each>"
      "    <xsl:for-each select=\"*/*\">"
      "      <xsl:choose>"
      "        <xsl:when test=\"@valid = 'no'\"/>"
      "        <xsl:otherwise>"
      "          <xsl:value-of select=\"concat('INSERT INTO ',$str_table,'(')\"/>"
      "          <xsl:for-each select=\"*\">"
      "            <xsl:if test=\"position() &gt; 1\">"
      "              <xsl:text>,</xsl:text>"
      "            </xsl:if>"
      "            <xsl:value-of select=\"name()\"/>"
      "          </xsl:for-each>"
      "          <xsl:value-of select=\"concat(') VALUES (','')\"/>"
      "          <xsl:for-each select=\"*\">"
      "            <xsl:variable name=\"str_name\" select=\"name()\"/>"
      "            <xsl:if test=\"position() &gt; 1\">"
      "              <xsl:text>,</xsl:text>"
      "            </xsl:if>"
      "            <xsl:choose>"
      "              <xsl:when test=\"/*/*[1]/*[name() = $str_name]/@type = 'int'\">"
      "                <xsl:value-of select=\".\"/>"
      "              </xsl:when>"
      "              <xsl:otherwise>"
      "                <xsl:value-of select=\"concat('&quot;',.,'&quot;')\"/>"
      "              </xsl:otherwise>"
      "            </xsl:choose>"
      "          </xsl:for-each>"
      "          <xsl:value-of select=\"concat(');','&#10;')\"/>"
      "        </xsl:otherwise>"
      "      </xsl:choose>"
      "    </xsl:for-each>"
      "  </xsl:template>"
      "</xsl:stylesheet>"
      ;

  pdocResult = xmlParseMemory(pcXsl,strlen(pcXsl));

  return pdocResult;
}
/* end of GetXslInsertAsPlain() */


/*! wrapper for , append all database entries as childs

similar to resNodeDirAppendEntries()

\param prnArgArchive the archive node
\param re_match pointer to a compiled regexp
\param fArgContent flag to extract data of a matching header
\return TRUE if successful, else FALSE
*/
BOOL_T
dbAppendEntries(resNodePtr prnArgDatabase, const pcre2_code *re_match, BOOL_T fArgContent)
{
  BOOL_T fResult = FALSE;

  if (resNodeReadStatus(prnArgDatabase) && resNodeIsDatabase(prnArgDatabase) && resNodeOpen(prnArgDatabase, "wd+")) {
    fResult = (resNodeSetContentDocEat(prnArgDatabase, dbDumpContextToDoc(prnArgDatabase, (DB_PROC_DECL|DB_PROC_ENTRIES))) != NULL);
  }
  resNodeClose(prnArgDatabase);
  return fResult;
} /* end of dbAppendEntries() */


#ifdef TESTCODE
#include "test/test_database.c"
#endif

