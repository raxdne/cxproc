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
#include "calendar_element.h"
#include <cxp/cxp.h>
#include "dom.h"
#include <cxp/cxp_dir.h>
#include <database/cxp_database.h>
#include "plain_text.h"

static resNodePtr
dbResNodeDatabaseOpenNew(xmlNodePtr pndArg, cxpContextPtr pccArg);

static BOOL_T
dbProcessDirNode(resNodePtr prnArgDb, xmlNodePtr pndArgDir, cxpContextPtr pccArg);


/*!
*/
xmlDocPtr
dbProcessDbNodeToDoc(xmlNodePtr pndArgDb, cxpContextPtr pccArg)
{
  xmlDocPtr pdocResult = NULL;

#ifdef DEBUG
  cxpCtxtLogPrint(pccArg,1, "dbProcessDbNodeToDoc(pndArg=%0x,pccArg=%0x)", pndArgDb, pccArg);
#endif

  if (pndArgDb == NULL) {
    /*! ignore NULL pointer */
  }
  else {
    resNodePtr prnDb = NULL; /* The database */

    prnDb = dbResNodeDatabaseOpenNew(pndArgDb,pccArg);
    if (prnDb) {
      if (domGetPropFlag(pndArgDb, BAD_CAST "dump", FALSE)) {
	/* dump */
	pdocResult = dbDumpContextToDoc(prnDb, (DB_PROC_DECL|DB_PROC_ENTRIES));
      }
      else {
	xmlNodePtr pndQuery;
	xmlChar mpucT[BUFFER_LENGTH];
	xmlNodePtr pndMeta;
	xmlNodePtr pndSql;

	/*! create result DOM
	*/
	pdocResult = xmlNewDoc(BAD_CAST "1.0");
	pndSql = xmlNewDocNode(pdocResult, NULL, BAD_CAST "sql", NULL);
	xmlDocSetRootElement(pdocResult, pndSql);

	pndMeta = xmlNewChild(pndSql, NULL, NAME_META, NULL);
	//xmlAddChild(pndMeta,xmlCopyNode(pndMakeSql,1));
	/* Get the current time. */
	domSetPropEat(pndMeta, BAD_CAST "ctime", GetNowFormatStr(BAD_CAST "%s"));
	domSetPropEat(pndMeta, BAD_CAST "ctime2", GetDateIsoString(0));
	cxpInfoProgram(pndMeta, pccArg);

	for (pndQuery = pndArgDb->children; pndQuery; pndQuery = pndQuery->next) {
	  if (IS_NODE_QUERY(pndQuery)) {
	    dbProcessQueryNodeToNode(pndSql, prnDb, pndQuery, (DB_PROC_DECL|DB_PROC_ENTRIES), pccArg);
	  }
	  else {
	    cxpCtxtLogPrint(pccArg,1, "Query is invalid");
	  }
	}
      }
      resNodeClose(prnDb);
      resNodeFree(prnDb);
    }
  }
  return pdocResult;
}
/* end of dbProcessDbNodeToDoc() */


/*! process one or more database queries of string, or multiple query elements to an open database

\param prnArgDb pointer to database handle
\param pndArgQuery pointer to a query element
\param iOptions
\param pccArg processing context in filesystem

\return a pointer to a result string buffer.

*/
xmlNodePtr
dbProcessQueryNodeToNode(xmlNodePtr pndArgParent, resNodePtr prnArgDb, xmlNodePtr pndArgQuery, int iOptions, cxpContextPtr pccArg)
{
  xmlNodePtr pndResult = NULL;

#ifdef DEBUG
  cxpCtxtLogPrint(pccArg,1,
    "dbProcessQueryNodeToNode(prnArgDb=%0x,pndArgQuery='%0x',iOptions=%i,pccArg=%0x)",
    prnArgDb, pndArgQuery, iOptions, pccArg);
#endif

  if (IS_VALID_NODE(pndArgQuery) == FALSE) {

  }
  else if (IS_NODE_QUERY(pndArgQuery)) {
    xmlNodePtr pndQueryChild;
    xmlChar *pucT = NULL;
    int iOptionsHere = iOptions;

    if (domGetPropFlag(pndArgQuery, BAD_CAST"declaration", FALSE)) {
      iOptionsHere |= DB_PROC_DECL;
    }

    if ((pndQueryChild = domGetFirstChild(pndArgQuery, NAME_QUERY))) {
      cxpCtxtLogPrint(pccArg,1, "Processing nested Query");
      /* skip column headers because there are not SQL queries */
      pucT = dbProcessQueryNodeToPlain(prnArgDb, pndQueryChild, DB_PROC_ENTRIES, pccArg);
      if (pucT != NULL && xmlStrlen(pucT) > 0) {
	pndResult = dbProcessQueryToNode(pndArgParent, prnArgDb, pucT, iOptionsHere);
      }
      xmlFree(pucT);
    }
    else if ((pndQueryChild = domGetFirstChild(pndArgQuery, NAME_PLAIN))) {
      pucT = cxpProcessPlainNode(pndQueryChild, pccArg); /*!\todo use resNode */
      if (pucT != NULL && xmlStrlen(pucT) > 0) {
	pndResult = dbProcessQueryToNode(pndArgParent, prnArgDb, pucT, iOptionsHere);
      }
      xmlFree(pucT);
    }
    else if ((pucT = domNodeGetContentPtr(pndArgQuery)) != NULL && xmlStrlen(pucT) > 5) {
      /* simple text node  */
      pndResult = dbProcessQueryToNode(pndArgParent, prnArgDb, pucT, iOptionsHere);
    }
    else {
      cxpCtxtLogPrint(pccArg,1, "No query string found");
    }
  }
  else {
  }
  return pndResult;
}
/* end of dbProcessQueryNodeToNode() */


/*! process one or more database queries of string, or multiple query elements to an open database

\param prnArgDb pointer to database handle
\param pndArgQuery pointer to a query element
\param iOptions
\param pccArg processing context in filesystem

\return a pointer to a result string buffer.

*/
xmlChar *
dbProcessQueryNodeToPlain(resNodePtr prnArgDb, xmlNodePtr pndArgQuery, int iOptions, cxpContextPtr pccArg)
{
  xmlChar *pucResult = NULL;

#ifdef DEBUG
  cxpCtxtLogPrint(pccArg,3,
    "dbProcessQueryNodeToPlain(prnArgDb=%0x,pndArgQuery='%0x',iOptions=%i,pccArg=%0x)",
    prnArgDb, pndArgQuery, iOptions, pccArg);
#endif

  if (pndArgQuery->type == XML_TEXT_NODE && STR_IS_NOT_EMPTY(pndArgQuery->content)) { /* simple text node  */
    pucResult = dbProcessQueryToPlain(prnArgDb, pndArgQuery->content, iOptions);
  }
  else if (IS_NODE_QUERY(pndArgQuery)) {
    xmlNodePtr pndQueryChild;
    xmlChar *pucT = NULL;
    int iOptionsHere = iOptions;

    if (domGetPropFlag(pndArgQuery, BAD_CAST"declaration", FALSE) && IS_NODE_QUERY(pndArgQuery->parent) == FALSE) {
      /* ignore this in nested queriespndArgQuery */
      iOptionsHere |= DB_PROC_DECL;
    }

    if ((pndQueryChild = domGetFirstChild(pndArgQuery, NAME_QUERY))) {
      cxpCtxtLogPrint(pccArg,1, "Processing nested Query");
      /* skip column headers because there are not SQL queries */
      pucT = dbProcessQueryNodeToPlain(prnArgDb, pndQueryChild, DB_PROC_ENTRIES, pccArg);
      if (STR_IS_NOT_EMPTY(pucT)) {
	pucResult = dbProcessQueryToPlain(prnArgDb, pucT, iOptionsHere); /* */
      }
      xmlFree(pucT);
    }
    else if ((pndQueryChild = domGetFirstChild(pndArgQuery, NAME_PLAIN))) {
      pucT = cxpProcessPlainNode(pndQueryChild, pccArg);
      if (STR_IS_NOT_EMPTY(pucT)) {
	pucResult = dbProcessQueryToPlain(prnArgDb, pucT, iOptionsHere);
      }
      xmlFree(pucT);
    }
    else if ((pucT = domNodeGetContentPtr(pndArgQuery)) != NULL && xmlStrlen(pucT) > 5) {
      /* simple text node  */
      pucResult = dbProcessQueryToPlain(prnArgDb, pucT, iOptionsHere);
    }
    else {
      cxpCtxtLogPrint(pccArg,1, "No query string found");
    }
  }
  else {
  }
  return pucResult;
} /* end of dbProcessQueryNodeToPlain() */


/*! process all query childs of a "db" node to a plain text result.

\param pndArgDb pointer to db node
\param pccArg processing context in filesystem

\return a pointer to a result string buffer.
*/
xmlChar *
dbProcessDbNodeToPlain(xmlNodePtr pndArgDb, cxpContextPtr pccArg)
{
  xmlChar* pucResult = NULL;

  if (pndArgDb) {
    resNodePtr prnDb = NULL;
    
    prnDb = dbResNodeDatabaseOpenNew(pndArgDb,pccArg);
    if (prnDb) {
      xmlChar *pucQueryResult = NULL;
      xmlNodePtr pndQuery = pndArgDb->children;

      if (pndQuery != NULL && pndQuery == pndArgDb->last
	  && pndQuery->type == XML_TEXT_NODE && STR_IS_NOT_EMPTY(pndQuery->content)) { /* single text node */
	pucQueryResult = dbProcessQueryNodeToPlain(prnDb, pndQuery, (DB_PROC_ENTRIES|DB_PROC_LINES), pccArg);
	if (pucQueryResult) {
	  if (pucResult) {
	    /*! concatenate query result strings */
	    pucResult = xmlStrcat(pucResult, BAD_CAST "\n");
	    pucResult = xmlStrcat(pucResult, pucQueryResult);
	    xmlFree(pucQueryResult);
	  }
	  else {
	    pucResult = pucQueryResult;
	  }
	}
	else {
	  cxpCtxtLogPrint(pccArg,1, "No usable query: '%s'",pucQueryResult);
	}
      }
      else {
	while (pndQuery) {
	  if (IS_VALID_NODE(pndQuery) && IS_NODE_QUERY(pndQuery)) {
	    pucQueryResult = dbProcessQueryNodeToPlain(prnDb, pndQuery, (DB_PROC_ENTRIES|DB_PROC_LINES), pccArg);
	    if (pucQueryResult) {
	      if (pucResult) {
		/*! concatenate query result strings */
		pucResult = xmlStrcat(pucResult, BAD_CAST "\n");
		pucResult = xmlStrcat(pucResult, pucQueryResult);
		xmlFree(pucQueryResult);
	      }
	      else {
		pucResult = pucQueryResult;
	      }
	    }
	  }
	  else {
	    cxpCtxtLogPrint(pccArg,1, "No usable query: '%s'",pucQueryResult);
	  }
	  pndQuery = pndQuery->next;
	}
      }
      resNodeFree(prnDb);
    }
  }
  return pucResult;
} /* end of dbProcessDbNodeToPlain() */


/*! Append the directory listing to the given database

\bug iLevelVerboseArg not tested
*/
BOOL_T
dbParseDirTraverse(resNodePtr prnArgDb, resNodePtr prnArgContext, int iDepthArg, int iLevelVerboseArg, int iOptions, const pcre2_code *re_match, cxpContextPtr pccArg)
{
  BOOL_T fResult = TRUE;

  if (resNodeIsHidden(prnArgContext) && (iOptions & FS_PARSE_HIDDEN) == 0) {
    dbInsertMetaLog(prnArgDb, BAD_CAST"log/ignore+hidden", resNodeGetNameNormalized(prnArgContext));
  }
  else if (resNodeIsDir(prnArgContext)) {
    xmlChar* pucStatement;
    resNodePtr prnT;

    if (resNodeIsReadable(prnArgContext) == FALSE) {
      dbInsertMetaLog(prnArgDb, BAD_CAST"error/read", resNodeGetNameNormalized(prnArgContext));
    }
    else if (resNodeListParse(prnArgContext, 1, re_match)) { /*! read Resource Node as list of childs */
      for (prnT = resNodeGetChild(prnArgContext); iDepthArg > 0 && prnT != NULL && fResult; prnT = resNodeGetNext(prnT)) {
    	resNodeIncrRecursiveSize(prnArgContext, resNodeGetSize(prnT));
	fResult &= dbParseDirTraverse(prnArgDb, prnT, iDepthArg - 1, iLevelVerboseArg, iOptions, re_match, pccArg);
      }
      resNodeFree(resNodeGetChild(prnArgContext)); /* release the context list of current directory */
    }
    else {
      dbInsertMetaLog(prnArgDb, BAD_CAST"error/parse+list", resNodeGetNameNormalized(prnArgContext));
    }

    pucStatement = resNodeToSQL(prnArgContext, 1);
    dbInsert(prnArgDb, pucStatement);
    xmlFree(pucStatement);
  }
#ifdef HAVE_LIBARCHIVE
  else if (resNodeIsArchive(prnArgContext)) {

    if (resNodeIsReadable(prnArgContext) == FALSE) {
      dbInsertMetaLog(prnArgDb, BAD_CAST"error/read", resNodeGetNameNormalized(prnArgContext));
    }
    else if (resNodeListParse(prnArgContext, 1, re_match)) { /*! read Resource Node as list of childs */
      xmlChar *pucStatement;

      pucStatement = resNodeListToSQL(prnArgContext, iOptions);
      dbInsert(prnArgDb, pucStatement);
      xmlFree(pucStatement);

      resNodeFree(resNodeGetChild(prnArgContext)); /* release the context list of current directory */
    }
    else {
      dbInsertMetaLog(prnArgDb, BAD_CAST"error/parse+list", resNodeGetNameNormalized(prnArgContext));
    }
  }
#endif
#ifdef HAVE_PIE
  else if (resNodeGetMimeType(prnArgContext) == MIME_APPLICATION_PIE_XML_INDEX) {
    /* ignore index files */
  }
#endif
#ifdef HAVE_PCRE2
  else if (resNodeIsMatching(prnArgContext, re_match)==FALSE) {
    /*  re_match */
    cxpCtxtLogPrint(pccArg, 4, "Skip non-matching '%s': '%s'", NAME_FILE, resNodeGetNameBase(prnArgContext));
  }
#endif
  else {
    /*! read single Resource Node */
    if (resNodeReadStatus(prnArgContext)) {
      xmlChar *pucStatement;

      if (iLevelVerboseArg > 1) {
	resNodeResetMimeType(prnArgContext);
#if 0
	if (iLevelVerboseArg > 2) {
	  resNodeSetOwner(prnArgContext);
	}
#endif
      }

      pucStatement = resNodeToSQL(prnArgContext, 1);
      dbInsert(prnArgDb, pucStatement);
      xmlFree(pucStatement);
    }
    else {
      dbInsertMetaLog(prnArgDb, BAD_CAST"error/read", resNodeGetNameNormalized(prnArgContext));
    }
  }

  return fResult;
} /* end of dbParseDirTraverse() */


/*!\return an allocated resource node according to attributes of pndArg
*/
resNodePtr
dbResNodeDatabaseOpenNew(xmlNodePtr pndArg, cxpContextPtr pccArg)
{
  xmlChar *pucNameDb;
  BOOL_T fWrite;
  BOOL_T fAppend;
  resNodePtr prnResult = NULL;

  /*!\todo allow INSERT and UPDATE queries with values from an input DOM */

  /*
    detect the necessary connection informations
  */
  pucNameDb = domGetPropValuePtr(pndArg, BAD_CAST "name");
  fWrite = domGetPropFlag(pndArg, BAD_CAST"write", FALSE);
  fAppend = domGetPropFlag(pndArg, BAD_CAST"append", fWrite);

  if (STR_IS_EMPTY(pucNameDb) || resPathIsInMemory(pucNameDb)) {
    /*!\bug in-memory database not usable (test/sql "test-db-8-in-memory.xml") */
    // prnResult = resNodeInMemoryNew();
  }
  else {
    prnResult = resNodeConcatNew(cxpCtxtLocationGetStr(pccArg), pucNameDb);
  }

  if (prnResult) {
    /*! create new db or append to an existing db according to attribute "append" */
    resNodeSetMimeType(prnResult, MIME_APPLICATION_X_SQLITE3);
    assert(resNodeIsDatabase(prnResult));
    
    resNodeOpen(prnResult, (fWrite ? (fAppend ? "wd+" : "wd") : "rd"));
  }
  return prnResult;
} /* end of dbResNodeDatabaseOpenNew() */


/*! process the DIR child instructions of pndArgDb

\param pndArgDb node pointer to all parser options

\todo additional argument depth_to_go for recursion limit
 */
BOOL_T
dbProcessDirNode(resNodePtr prnArgDb, xmlNodePtr pndArgDir, cxpContextPtr pccArg)
{
  BOOL_T fResult = TRUE;

  xmlNodePtr pndChild = NULL;
  xmlChar *pucAttrDepth;
  xmlChar *pucAttrVerbosity;
  xmlChar *pucAttrMatch;
  int iDepth;
  int iLevelVerbose;
  int iOptions = FS_PARSE_DEFAULT;

  xmlChar mpucT[BUFFER_LENGTH];
  pcre2_code *re_match = NULL;
  
  if (IS_VALID_NODE(pndArgDir) == FALSE) {
    return FALSE;
  }

  /*!\todo set depth attribute per single dir element, instead of global */
  if ((pucAttrDepth = domGetPropValuePtr(pndArgDir,BAD_CAST "depth"))!=NULL
      && ((iDepth = atoi((char *)pucAttrDepth)) > 0)) {
  }
  else {
    iDepth = 9999;
  }
  cxpCtxtLogPrint(pccArg,2,"Set DIR depth to '%i'", iDepth);

  iLevelVerbose = dirMapInfoVerbosity(pndArgDir,pccArg);
  
#ifdef HAVE_PCRE2
  if (((pucAttrMatch = domGetPropValuePtr(pndArgDir, BAD_CAST "imatch")) != NULL
    && xmlStrlen(pucAttrMatch) > 0)
    ||
    ((pucAttrMatch = domGetPropValuePtr(pndArgDir, BAD_CAST "match")) != NULL
    && xmlStrlen(pucAttrMatch) > 0)) {
    size_t erroroffset;
    int errornumber;
    int opt_match_pcre = PCRE2_UTF;

    if (xmlHasProp(pndArgDir, BAD_CAST "imatch")) {
      cxpCtxtLogPrint(pccArg,2, "Use caseless file match '%s' in %s with depth '%i'", pucAttrMatch, NAME_DIR, iDepth);
      opt_match_pcre |= PCRE2_CASELESS;
    }
    else {
      cxpCtxtLogPrint(pccArg,2, "Use file match '%s' in %s with depth '%i'", pucAttrMatch, NAME_DIR, iDepth);
    }

    re_match = pcre2_compile(
      (PCRE2_SPTR8)pucAttrMatch, /* the pattern */
      PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
      opt_match_pcre,         /* default options */
      &errornumber,          /* for error number */
      &erroroffset,          /* for error offset */
      NULL);                 /* use default compile context */

    if (re_match == NULL) {
      /* regexp error handling */
      dbInsertMetaLog(prnArgDb, BAD_CAST "error/regexp", pucAttrMatch);
      return FALSE;
    }
  }
#endif

  if (domGetPropFlag(pndArgDir,BAD_CAST "hidden",FALSE)) {
    cxpCtxtLogPrint(pccArg,2,"Read hidden directories");
    iOptions |= FS_PARSE_HIDDEN;
  }
  else {
    cxpCtxtLogPrint(pccArg,2,"Ignore hidden directories");
  }

  /*!\todo use exclude="(~|)" (or: mach="" type="exclude") */

  for (pndChild = ((pndArgDir->children) ? pndArgDir->children : pndArgDir); pndChild; pndChild = pndChild->next) {
    xmlChar *pucPath;

    if (IS_VALID_NODE(pndChild) == FALSE) {
      continue;
    }

    pucPath = domGetPropValuePtr(pndChild,BAD_CAST "name");
    if (pucPath == NULL || xmlStrlen(pucPath) < 1) {
#ifdef HAVE_CGI
      pucPath = xmlStrdup(resNodeGetNameNormalized(cxpCtxtRootGet(pccArg)));
#else
      pucPath = NULL;
#endif
    }
    else {
      pucPath = xmlStrdup(pucPath);
    }

    if (pucPath) {
      resNodePtr prnT;

      prnT = resNodeConcatNew(cxpCtxtLocationGetStr(pccArg),pucPath);
      if (cxpCtxtAccessIsPermitted(pccArg,prnT) == FALSE) {
	dbInsertMetaLog(prnArgDb, BAD_CAST "error/access", resNodeGetNameNormalized(prnT));
      }
      else if (resNodeReadStatus(prnT) == FALSE) {
	dbInsertMetaLog(prnArgDb, BAD_CAST "error/exists", resNodeGetNameNormalized(prnT));
      }
      else if (prnArgDb != NULL
	  && dbParseDirTraverse(prnArgDb, prnT, iDepth, iLevelVerbose, iOptions, re_match, pccArg)) {
      }
      else {
      }
      resNodeFree(prnT);
    }
    else {
      dbInsertMetaLog(prnArgDb, BAD_CAST "error/type+unknown", pucPath);
    }

    if (pucPath) {
      xmlFree(pucPath);
    }
  }

  dbInsertMetaLog(prnArgDb, NULL, NULL);

#ifdef HAVE_PCRE2
  if (re_match)
    pcre2_code_free(re_match);
#endif

  return fResult;
} /* end of dbProcessDirNode() */


/*! process database node an his childs without any return value
*/
BOOL_T
dbProcessDbNode(xmlNodePtr pndArg, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;

  if (IS_VALID_NODE(pndArg)) {
    resNodePtr prnDb;
    
    prnDb = dbResNodeDatabaseOpenNew(pndArg,pccArg);
    if (resNodeIsWriteable(prnDb)) {
      xmlChar *pucQuery = NULL;
      xmlNodePtr pndChild = NULL;

      fResult = TRUE;
      
      if (domGetFirstChild(pndArg, NAME_DIR) != NULL) { /* dir and file nodes will be parsed */
	dbParseDirCreateTables(prnDb);
	dbInsertMetaLog(prnDb, NULL, NULL);
	for (pndChild = pndArg->children; pndChild; pndChild = pndChild->next) {
	  dbProcessDirNode(prnDb, pndChild, pccArg);
	}
      }
      else if (domGetFirstChild(pndArg, NAME_QUERY)) { /* SQL statements to insert data */
	for (pndChild = pndArg->children; pndChild; pndChild = pndChild->next) {
	  if (IS_NODE_QUERY(pndChild)) {
	    pucQuery = cxpProcessPlainNode(pndChild, pccArg);
	    if (STR_IS_NOT_EMPTY(pucQuery)) {
	      dbInsert(prnDb, pucQuery);
	    }
	    xmlFree(pucQuery);
	  }
	}
      }
      else if (domGetFirstChild(pndArg, NAME_PLAIN)) { /* plain text SQL statements to insert data */
	for (pndChild = pndArg->children; pndChild; pndChild = pndChild->next) {
	  if (IS_NODE_PLAIN(pndChild)) {
	    pucQuery = cxpProcessPlainNode(pndChild, pccArg);
	    if (STR_IS_NOT_EMPTY(pucQuery)) {
	      dbInsert(prnDb, pucQuery);
	    }
	    xmlFree(pucQuery);
	  }
	}
      }
      else if (pndArg->children != NULL && pndArg->children == pndArg->last && pndArg->children->type == XML_TEXT_NODE) { /* single text node */
	pucQuery = xmlStrdup(pndArg->children->content);
	if (STR_IS_NOT_EMPTY(pucQuery)) {
	  dbInsert(prnDb, pucQuery);
	}
	else {
	  cxpCtxtLogPrint(pccArg,1, "No usable query: '%s'",pucQuery);
	}
	xmlFree(pucQuery);
      }
#if 0
      else if (pndArg->children) { /* different child nodes */
	xmlNodePtr pndChild;

	for (pndChild = pndArg->children; pndChild; pndChild = pndChild->next) {
	  if (IS_ENODE(pndChild)) { /* a generic DOM, try to get queries via XSLT */
	    xmlDocPtr pdocXsl;
	    xmlDocPtr pdocT;

	    pdocXsl = GetXslInsertAsPlain();
	    if (pdocXsl == NULL) {
	      cxpCtxtLogPrint(pccArg,1, "No DOM built");
	      return fResult;
	    }
	    cxpCtxtLogPrintDoc(pccArg, 4, "SQL", pdocXsl);

	    pdocT = domDocFromNodeNew(pndArg->children);
	    if (pdocT) {
	      cxpCtxtLogPrintDoc(pccArg, 4, "Data", pdocT);
	      cxpProcessTransformations(pdocT, pdocXsl, NULL, &pucQuery, pccArg);
	      xmlFreeDoc(pdocT);
	    }
	    xmlFreeDoc(pdocXsl);
	  }

	  if (STR_IS_NOT_EMPTY(pucQuery)) {
	    dbInsert(prnDb, pucQuery);
	  }
	  else {
	    cxpCtxtLogPrint(pccArg,1, "No usable query: '%s'",pucQuery);
	  }
	  xmlFree(pucQuery);
	}
      }
#endif
      else { /* node without any childs */
      }
    }
    resNodeClose(prnDb);
    resNodeFree(prnDb);
  }  
  return fResult;
} /* end of dbProcessDbNode() */


#ifdef TESTCODE
#include "test/test_cxp_database.c"
#endif

