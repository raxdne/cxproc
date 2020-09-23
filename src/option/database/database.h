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

#include <sqlite3.h>

#ifdef HAVE_PCRE2
#include <pcre2.h>
#endif

/*! database processing options */
typedef enum {
    DB_PROC_DEFAULT	= 0,	/*  */
    DB_PROC_DECL	= 1<<0,	/*  */
    DB_PROC_ENTRIES	= 1<<1,	/*  */
    DB_PROC_COUNTER	= 1<<2,	/*  */
    DB_PROC_LINES	= 1<<3	/*  */
} dbProcessOption;

#ifdef TESTCODE
extern int
dbTest(void);
#endif

extern xmlDocPtr
dbDumpContextToDoc(resNodePtr prnArgDb, int iOptions);

extern BOOL_T
dbDumpContextToNode(xmlNodePtr pndArg, resNodePtr prnArg, int iOptions);

extern BOOL_T
dbInsertMetaLog(resNodePtr prnArgDb, xmlChar *pucArgKey, xmlChar *pucValue);

extern xmlDocPtr
dbProcessQueryToDoc(resNodePtr prnArgDb, xmlChar *pucArgQuery, int iOptions);

extern xmlNodePtr
dbProcessQueryToNode(xmlNodePtr pndArgParent, resNodePtr prnArgDb, xmlChar *pucArgQuery, int iOptions);

extern xmlChar *
dbProcessQueryToPlain(resNodePtr prnArgDb, xmlChar *pucArgQuery, int iOptions);

extern xmlChar *
dbTextReadStatement(xmlChar *pucArg);

extern BOOL_T
dbAppendEntries(resNodePtr prnArgDatabase, const pcre2_code *re_match, BOOL_T fArgContent);

extern BOOL_T
dbParseDirCreateTables(resNodePtr prnArgDb);

extern BOOL_T
dbInsert(resNodePtr prnArgDb, xmlChar *pucArgQuery);

extern xmlDocPtr
GetXslInsertAsPlain(void);

