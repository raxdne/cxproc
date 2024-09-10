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
#include <zip.h>

#include <libxml/HTMLtree.h>
#include <libxml/tree.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#ifdef HAVE_LIBCURL
#include <curl/curl.h>
#elif 0
#include <libxml/uri.h>
#include <libxml/nanohttp.h>
#endif

#include "basics.h"
#include "utils.h"
#include <res_node/res_node_ops.h>
#include <cxp/cxp.h>
#include "dom.h"
#include "plain_text.h"

#ifdef HAVE_LIBSQLITE3
#include <database/database.h>
#endif

static BOOL_T
OpenURL(resNodePtr prnArg);

static BOOL_T
CloseURL(resNodePtr prnArg);

static BOOL_T
OpenSqlite(resNodePtr prnArg);

static BOOL_T
CloseSqlite(resNodePtr prnArg);

static BOOL_T
resNodeReadContent(resNodePtr prnArg, int iArgMax);

static int
resNodeIncrUsageCount(resNodePtr prnArg);

static int
resNodeResetUsageCount(resNodePtr prnArg);


/*!\todo resNodeSetContentTime() */

/*!\todo resNodeGetContentTime() to compare to Modification time of context */


/*! set mode of context prnArg according to pchArgMode ("r|w|w+|wb|wb|rz|wz|a|aw|rd|wd|wd+")

  \return TRUE if successful
*/
BOOL_T
resNodeSetMode(resNodePtr prnArg, const char *pchArgMode)
{
  BOOL_T fResult = TRUE;

  assert(pchArgMode != NULL && (strchr(pchArgMode, (int)'r') || strchr(pchArgMode, (int)'w')));

  if (strchr(pchArgMode, (int)'w')) {
    if (strchr(pchArgMode, (int)'+')) {
      prnArg->eMode = mode_append;
    }
    else {
      prnArg->eMode = mode_write;
    }
  }
  else {
    prnArg->eMode = mode_read;
  }
      
  if (resNodeGetBlockSize(prnArg) < BUFFER_LENGTH) {
    resNodeSetBlockSize(prnArg, BUFFER_LENGTH); /* default for blockwise reading */
  }

  return fResult;
} /* end of resNodeSetMode() */


/*! opens context prnArg according to pchArgMode ("r|w|w+|wb|wb|rz|wz|a|aw|rd|wd|wd+")

  set the context handle and access method for IO

  \return TRUE if successful
*/
BOOL_T
resNodeOpen(resNodePtr prnArg, const char *pchArgMode)
{
  BOOL_T fResult = FALSE;

  if (resNodeResetError(prnArg)) {

   // assert(prnArg->handleIO == NULL || prnArg->handleIO == (void *)stdin || prnArg->handleIO == (void *)stdout);
    resNodeSetMode(prnArg, pchArgMode);
    
    PrintFormatLog(3, "Open file '%s' in '%s' mode", resNodeGetNameNormalized(prnArg), pchArgMode);
    
    if (resNodeIsStd(prnArg)) {
      prnArg->eAccess = rn_access_std;
      if (prnArg->eMode == mode_write) {
	resNodeSetType(prnArg,rn_type_stdout);
	prnArg->handleIO = (void *)stdout;
	prnArg->fRead = FALSE;
	prnArg->fWrite = TRUE;
      }
      else {
	resNodeSetType(prnArg,rn_type_stdin);
	prnArg->handleIO = (void *)stdin;
	prnArg->fRead = TRUE;
	prnArg->fWrite = FALSE;
      }
      prnArg->fExist = TRUE;
      fResult = TRUE;
    }
    else if (resNodeIsDir(prnArg)
	|| (resNodeIsURL(prnArg) == FALSE && resPathIsDir(resNodeGetNameNormalized(prnArg)))) {
      prnArg->eAccess = rn_access_directory;
      /* nothing to prepare */
      fResult = TRUE;
    }
    else if (strchr(pchArgMode,(int)'a') && resMimeIsZipDocument(resNodeGetMimeType(prnArg))) {

      if (resPathIsURL(resNodeGetNameNormalized(prnArg))) {
	fResult = OpenURL(prnArg);
      }
      else {
	fResult = zipFileOpen(prnArg,pchArgMode);
      }

      if (fResult) {
	if (prnArg->eMode == mode_write || prnArg->eMode == mode_append) {
	  prnArg->fRead = TRUE;
	  prnArg->fWrite = TRUE;
	}
	else {
	  prnArg->fRead = TRUE;
	  prnArg->fWrite = FALSE;
	}
	prnArg->fExist = TRUE;
      }
    }
#ifdef HAVE_LIBARCHIVE
    else if (strchr(pchArgMode,(int)'a') && resMimeIsArchive(resNodeGetMimeType(prnArg))) {

      if (resPathIsURL(resNodeGetNameNormalized(prnArg))) {
	fResult = OpenURL(prnArg);
      }
      else {
	fResult = arcFileOpen(prnArg);
      }

      if (fResult) {
	if (prnArg->eMode == mode_write || prnArg->eMode == mode_append) {
	  prnArg->fRead = TRUE;
	  prnArg->fWrite = TRUE;
	}
	else {
	  prnArg->fRead = TRUE;
	  prnArg->fWrite = FALSE;
	}
	prnArg->fExist = TRUE;
      }
    }
#endif
    /*!\todo  else if (strchr(pchArgMode, (int)'i') && resNodeIsImage(prnArg)) ??? */
    else if (strchr(pchArgMode, (int)'d') && resNodeIsDatabase(prnArg)) {
      fResult = OpenSqlite(prnArg);
      if (fResult) {
	if (prnArg->eMode == mode_write || prnArg->eMode == mode_append) {
	  prnArg->fRead = TRUE;
	  prnArg->fWrite = TRUE;
	}
	else {
	  prnArg->fRead = TRUE;
	  prnArg->fWrite = FALSE;
	}
	prnArg->fExist = TRUE;
      }
    }
#if 0
    else if (resNodeIsArchive(prnArg)) {

      if (resPathIsURL(resNodeGetNameNormalized(prnArg))) {
	fResult = OpenURL(prnArg);
      }
      else {
	fResult = TRUE;
      }

      if (fResult) {
	if (prnArg->eMode == mode_write || prnArg->eMode == mode_append) {
	  prnArg->fRead = TRUE;
	    prnArg->fWrite = TRUE;
	}
	else {
	  prnArg->fRead = TRUE;
	  prnArg->fWrite = FALSE;
	}
	prnArg->fExist = TRUE;
      }
    }
#endif
    else if (resNodeIsFileInArchive(prnArg)) {
      resNodePtr prnArchive;

      /* check if anchestor archive is parsed already */
      if ((prnArchive = resNodeGetAncestorArchive(prnArg))) {
	fResult = (resNodeListFindPath(prnArchive,resNodeGetNameBase(prnArg), RN_FIND_ALL) != NULL);
	if (fResult) {
	  if (prnArg->eMode == mode_write || prnArg->eMode == mode_append) {
	    prnArg->fRead = TRUE;
	    prnArg->fWrite = TRUE;
	  }
	  else {
	    prnArg->fRead = TRUE;
	    prnArg->fWrite = FALSE;
	  }
	  prnArg->fExist = TRUE;
	}
      }
    }
    else if (resNodeIsURL(prnArg)) {
      fResult = OpenURL(prnArg);
      if (fResult) {
	if (prnArg->eMode == mode_write || prnArg->eMode == mode_append) {
	  prnArg->fRead = TRUE;
	  prnArg->fWrite = TRUE;
	}
	else {
	  prnArg->fRead = TRUE;
	  prnArg->fWrite = FALSE;
	}
	prnArg->fExist = TRUE;
      }
    }
    else if (resNodeIsFile(prnArg) || resNodeIsArchive(prnArg) || resNodeGetError(prnArg) == rn_error_stat) {
#ifdef _MSC_VER
      errno_t iError;
#endif
      char *pchMode;

      switch (prnArg->eMode) {
      case mode_append:
	pchMode = (char *)"a+";
	break;
      case mode_write:
	pchMode = (char *)"wb";
	break;
      default:
	pchMode = (char *)"rb";
      }

#ifdef _MSC_VER
      iError = fopen_s((FILE **)&(prnArg->handleIO), resNodeGetNameNormalizedNative(prnArg), pchMode);
#elif defined _WIN32
      prnArg->handleIO = (void *)fopen(resNodeGetNameNormalizedNative(prnArg),pchMode);
#else
      prnArg->handleIO = (void *)fopen(resNodeGetNameNormalizedNative(prnArg),pchMode);
#endif
      if (prnArg->handleIO) {
	resNodeSetType(prnArg,rn_type_file);
	prnArg->eAccess = rn_access_file;
	fResult = TRUE;
	if (fResult) {
	  if (prnArg->eMode == mode_write || prnArg->eMode == mode_append) {
	    prnArg->fRead = TRUE;
	    prnArg->fWrite = TRUE;
	  }
	  else {
	    prnArg->fRead = TRUE;
	    prnArg->fWrite = FALSE;
	  }
	  prnArg->fExist = TRUE;
	}
      }
      else {
	/*!\todo handle error code 'errno' */
	resNodeSetError(prnArg,rn_error_open, "Cant open '%s'", resNodeGetNameNormalizedNative(prnArg));
      }
    }
    else { /* */
   	resNodeSetError(prnArg,rn_error_open, "Cant open '%s'", resNodeGetNameNormalizedNative(prnArg));
    }
  }
  return fResult;
} /* end of resNodeOpen() */


/*! opens context prnArg

\return TRUE if successful
*/
BOOL_T
OpenURL(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (prnArg != NULL) {

#ifdef HAVE_LIBCURL
    // s. "curl\docs\examples\getinmemory.c" or "curl\docs\examples\fopen.c"

    if (prnArg->eMode == mode_write) {
      resNodeSetError(prnArg, rn_error_open, "writing mode for URL not implemented yet '%s'", resNodeGetNameNormalized(prnArg));
      // prnArg->eAccess = rn_access_curl;
    }
    else if (prnArg->curlURL) {
      CURLcode res = CURLE_OK;

      prnArg->eAccess = rn_access_curl;

      if (prnArg->handleIO == NULL) {
	char *pcURL = NULL;
	CURLUcode rc;

	rc = curl_url_get(prnArg->curlURL, CURLUPART_URL, &pcURL, 0);
	if (rc == CURLUE_OK) {
	  /* init the curl session */
	  prnArg->handleIO = curl_easy_init();
	  curl_easy_setopt(prnArg->handleIO, CURLOPT_URL, pcURL);
	}
	curl_free(pcURL);
      }

      if (prnArg->handleIO) {
	curl_easy_setopt(prnArg->handleIO, CURLOPT_TIMEOUT, 2L);

	/* some servers don't like requests that are made without a user-agent field, so we provide one */
	curl_easy_setopt(prnArg->handleIO, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	/* send all data to this function  */
	curl_easy_setopt(prnArg->handleIO, CURLOPT_WRITEFUNCTION, CurlWriteToMemoryCallback);

	/* we pass our 'chunk' struct to the callback function */
	curl_easy_setopt(prnArg->handleIO, CURLOPT_WRITEDATA, (void *)prnArg);

	curl_easy_setopt(prnArg->handleIO, CURLOPT_CONNECT_ONLY, 1L);
	res = curl_easy_perform(prnArg->handleIO);
	if (res == CURLE_OK) { /* only connected! */
	  prnArg->fExist = TRUE;
	  fResult = TRUE;
	  curl_easy_setopt(prnArg->handleIO, CURLOPT_CONNECT_ONLY, 0L);
	}
	else {
	  CloseURL(prnArg);
	  resNodeSetError(prnArg, rn_error_open, "Error '%s': '%s'", resNodeGetNameNormalized(prnArg), curl_easy_strerror(res));
	}
      }
      else {
	CloseURL(prnArg);
	resNodeSetError(prnArg, rn_error_open, "Error '%s': '%s'", resNodeGetNameNormalized(prnArg), curl_easy_strerror(res));
      }
    }
    else {
      CloseURL(prnArg);
      resNodeSetError(prnArg, rn_error_open, "opening error '%s'", resNodeGetNameNormalized(prnArg));
    }
#elif 0
    if (prnArg->eType == rn_type_url_http) {
      prnArg->handleIO = xmlNanoHTTPOpen(resNodeGetNameNormalizedNative(prnArg), NULL);
    }
    else if (prnArg->eType == rn_type_url_ftp) {
      //prnArg->handleIO = xmlNanoFTPOpen(resNodeGetNameNormalizedNative(prnArg), NULL);
    }

    if (prnArg->handleIO) {
      prnArg->eAccess = rn_access_xmlio;
      fResult = TRUE;
    }
    else {
      resNodeSetError(prnArg, rn_error_open, "opening error '%s'", resNodeGetNameNormalized(prnArg));
    }
#endif
  }
  return fResult;
} /* end of OpenURL() */


/*! closes context prnArg

\return TRUE if successful
*/
BOOL_T
CloseURL(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

#ifdef HAVE_LIBCURL
  assert(prnArg != NULL);

  /* cleanup curl stuff */
  curl_easy_cleanup((CURL *)prnArg->handleIO);
  fResult = TRUE;
#elif 0
  if (resNodeGetType(prnArg) == rn_type_url_http) {
    xmlIOHTTPClose(prnArg->handleIO);
    fResult = TRUE;
  }
  else if (resNodeGetType(prnArg) == rn_type_url_ftp) {
    xmlIOFTPClose(prnArg->handleIO);
    fResult = TRUE;
  }
  else {
    assert(TRUE);
  }
#endif
  prnArg->handleIO = NULL;
  prnArg->eAccess = rn_access_undef;

  return fResult;
} /* end of CloseURL() */


/*! saves context prnArg content (open, save & close)

  \return TRUE if successful
*/
BOOL_T
resNodePutContent(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (prnArg) {
    int iMode = 0;

    //assert(prnArg->eAccess == rn_access_undef && prnArg->handleIO == NULL);
    //assert(resNodeIsArchive(prnArg) == FALSE);

#ifdef _MSC_VER
    /* no create mode */
#else
    iMode = (S_IRUSR | S_IWUSR | S_IXUSR);
#endif

    fResult = (resNodeMakeDirectory(prnArg, iMode) && resNodeOpen(prnArg,"w") && resNodeSaveContent(prnArg) && resNodeClose(prnArg));
  }
  return fResult;
} /* end of resNodePutContent() */


/*! saves context prnArg content
  \return TRUE if successful
*/
BOOL_T
resNodeSaveContent(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (prnArg) {

    assert(prnArg->eAccess != rn_access_undef && prnArg->handleIO != NULL);
    
    if (prnArg->eAccess == rn_access_std) {
      if (fwrite(resNodeGetContentPtr(prnArg), resNodeGetSize(prnArg), 1, (FILE *)resNodeGetHandleIO(prnArg)) == 1) {
      }
      else {
	/*!\todo handle error code 'errno' */
	resNodeSetError(prnArg, rn_error_write, "Cant use stdout");
      }
    }
#ifdef HAVE_LIBCURL
    else if (prnArg->eAccess == rn_access_directory
	     || (prnArg->eAccess != rn_access_curl && resPathIsDir(resNodeGetNameNormalized(prnArg)))) {
      /* nothing to prepare */
    }
    else if (prnArg->eAccess == rn_access_curl) {
      PrintFormatLog(3, "Save to URL '%s'", resNodeGetNameNormalized(prnArg));
    }
#else
    else if (prnArg->eAccess == rn_access_directory
      || resPathIsDir(resNodeGetNameNormalized(prnArg))) {
      /* nothing to prepare */
    }
#endif
    else if (prnArg->eAccess == rn_access_archive) {
#ifdef HAVE_LIBARCHIVE
      PrintFormatLog(3, "Save '%s' as archive", resNodeGetNameNormalized(prnArg));
      //resNodeSetError(prnArg,rn_error_open, "unknown mode for archive opening '%s'", resNodeGetNameNormalized(prnArg));
      /*!\todo read archive from memory buffer ":memory:" archive_read_open_memory() */
#else
      PrintFormatLog(1, "Compiled without archive option!!");
#endif
    }
    else if (prnArg->eAccess == rn_access_zip) {
      PrintFormatLog(3, "Save '%s' as zip", resNodeGetNameNormalized(prnArg));
      //resNodeSetError(prnArg,rn_error_open, "unknown mode for zip opening '%s'", resNodeGetNameNormalized(prnArg));
      /*!\todo read zip from memory buffer ":memory:" zip_read_open_memory() */
    }
    else if (prnArg->eAccess == rn_access_file) {
      PrintFormatLog(3, "Save file '%s'", resNodeGetNameNormalized(prnArg));
      if (fwrite(resNodeGetContentPtr(prnArg),resNodeGetSize(prnArg),(size_t)1,(FILE *)resNodeGetHandleIO(prnArg)) == 1) {
      }
      else {
	/*!\todo handle error code 'errno' */
	resNodeSetError(prnArg,rn_error_open, "Cant save '%s'", resNodeGetNameNormalizedNative(prnArg));
      }
    }
    else { /* URI path */
    }
    fResult = (resNodeGetError(prnArg) == rn_error_none);
  }
  return fResult;
} /* end of resNodeSaveContent() */


/*! \returns TRUE if io handle of context 'prnArg' is open.

  \param prnArg filesystem context 
*/
BOOL_T
resNodeIsOpen(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (prnArg) {
    fResult = ((prnArg->handleIO != NULL && prnArg->eAccess != rn_access_undef) || prnArg->eType == rn_type_file_in_archive || prnArg->eType == rn_type_file_in_zip);
  }
  return fResult;
} /* end of resNodeIsOpen() */


/*! Closes an open handle of context 'prnArg'.

  \param prnArg filesystem context to close
*/
BOOL_T
resNodeClose(resNodePtr prnArg)
{
  BOOL_T fResult = TRUE;
  
  if (resNodeIsOpen(prnArg)) {
    PrintFormatLog(3, "Closing '%s'", resNodeGetNameNormalized(prnArg));
    if (prnArg->eAccess == rn_access_zip) {
      fResult = zipFileClose(prnArg);
    }
#ifdef HAVE_LIBARCHIVE
    else if (prnArg->eAccess == rn_access_archive) {
      fResult = arcFileClose(prnArg);
    }
    else if (prnArg->eType == rn_type_file_in_archive) {
    }
#endif
#ifdef HAVE_LIBCURL
    else if (prnArg->eAccess == rn_access_curl) {
      fResult = CloseURL(prnArg);
    }
#endif
    else if (prnArg->eAccess == rn_access_sqlite) {
      fResult = CloseSqlite(prnArg);
    }
    else if (prnArg->eAccess == rn_access_file) {
      fResult = (fclose((FILE *)prnArg->handleIO) == 0);
    }
    else if (prnArg->eAccess == rn_access_directory) {
#ifdef _MSC_VER
      //FindClose((HANDLE)prnArg->handleIO);
      //xmlFree(prnArg->handleIO);
#else
      //closedir((DIR *)prnArg->handleIO);
#endif
      fResult = TRUE;
    }
    prnArg->handleIO = NULL;
    prnArg->eAccess = rn_access_undef;
  }
  else {
    fResult = FALSE;
  }
  return fResult;
} /* end of resNodeClose() */


/*! check and encode URL and opens the according Sqlite database file

\todo move to "option/database/database.c"

\return TRUE if Database file was opened NOW!
*/
BOOL_T
OpenSqlite(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

#ifdef HAVE_LIBSQLITE3
  /* SQLite file */
  xmlChar *pucFilename;

  assert(prnArg != NULL && prnArg->handleIO == NULL);

  if (resNodeGetType(prnArg) == rn_type_memory) {
    pucFilename = xmlStrdup(resNodeGetNameNormalized(prnArg));
  }
  else {
    pucFilename = resPathGetQuotedStr(resNodeGetNameNormalized(prnArg));
  }

  /*\todo test if database file exists or is open */
  if (prnArg->eMode == mode_write || prnArg->eMode == mode_append) {
    if (prnArg->eMode == mode_append) {
      /* access is permitted and dir or file exists */
      PrintFormatLog(2,"Open Database '%s' in append write mode", resNodeGetNameNormalized(prnArg));
    }
    else {
      /* access is permitted and dir or file exists */
      PrintFormatLog(2,"Open Database '%s' in create write mode", resNodeGetNameNormalized(prnArg));
      resNodeUnlink(prnArg,FALSE);
    }
    
    sqlite3_open_v2((char *)pucFilename,
      (sqlite3 **) &(prnArg->handleIO),
      SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
      NULL);
    if (prnArg->handleIO != NULL && SQLITE_OK == sqlite3_errcode((sqlite3 *)prnArg->handleIO)) {
      int rc;

      prnArg->fExist = TRUE;
      rc = sqlite3_exec((sqlite3 *)prnArg->handleIO, "PRAGMA journal_mode = OFF ; PRAGMA synchronous = OFF;", NULL, NULL, NULL);
      if (rc == SQLITE_OK) {
	prnArg->eAccess = rn_access_sqlite;
	fResult = TRUE;
      }
      else {
	resNodeSetError(prnArg, rn_error_sql, "SQL error");
      }
    }
    else {
      prnArg->handleIO = NULL;
      resNodeSetError(prnArg, rn_error_undef, "sqlite open undef");
    }
  }
  else {
    PrintFormatLog(1,"Open Database '%s' in readonly mode", pucFilename);
    sqlite3_open_v2((char *)pucFilename,
      (sqlite3 **)  &(prnArg->handleIO),
      SQLITE_OPEN_READONLY,
      NULL);
    if (prnArg->handleIO != NULL && SQLITE_OK == sqlite3_errcode((sqlite3 *)prnArg->handleIO)) {
      prnArg->fExist = TRUE;
      prnArg->eAccess = rn_access_sqlite;
      fResult = TRUE;
    }
    else {
      prnArg->handleIO = NULL;
      resNodeSetError(prnArg,rn_error_sql, "sqlite open undef");
    }
  }
  xmlFree(pucFilename);
#endif
  return fResult;
} /* end of OpenSqlite() */


/*! check and encode URL and opens the according Sqlite database file

\todo move to "option/database/database.c"

\return TRUE if Database file was opened NOW!
*/
BOOL_T
CloseSqlite(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

#ifdef HAVE_LIBSQLITE3
  assert(prnArg != NULL && prnArg->eAccess == rn_access_sqlite);

  sqlite3_close((sqlite3 *)prnArg->handleIO);
  fResult = TRUE;
#endif

  return fResult;
} /* end of CloseSqlite() */


#ifdef HAVE_LIBCURL

/* s. https://curl.haxx.se/libcurl/c/getinmemory.html */

/*! \return TRUE if successful
*/
size_t
_CurlReadFromMemoryCallback(void *contents, size_t size, size_t nmemb, void *prnArg)
{
  size_t realsize = size * nmemb;
#if 0
  resNodePtr prn = (resNodePtr)prnArg;
  prn->pContent = xmlRealloc(prn->pContent, prn->liSizeContent + realsize + 1);
  if (prn->pContent == NULL) {
    /* out of memory! */
    resNodeSetError(prnArg,rn_error_memory, "not enough memory (realloc returned NULL)\n");
    return 0;
  }

  xmlMemcpy(&(prn->pContent[prn->liSizeContent]), contents, realsize);
  prn->liSizeContent += realsize;
  prn->pContent[prn->liSizeContent] = 0;
#endif
  return realsize;
} /* end of _CurlReadFromMemoryCallback() */


/*! \return length of last block or else 0

  https://curl.haxx.se/libcurl/c/CURLOPT_WRITEFUNCTION.html
*/
size_t
CurlWriteToMemoryCallback(void *pucArgBlock, size_t iArgSize, size_t iArgNumber, void *pArg)
{
  size_t iBlockSize = 0;
      
  if (pArg != NULL && iArgNumber > 0) {
    resNodePtr prnArg = (struct _resNode *) pArg;

    iBlockSize = iArgSize * iArgNumber;
    if (resNodeAppendContent(prnArg, pucArgBlock, iBlockSize) != NULL) {
      /* OK */
    }
    else {
      resNodeSetError(prnArg,rn_error_memory, "not enough memory (realloc returned NULL)\n");
      iBlockSize = 0;
    }
  }
  return iBlockSize;
} /* end of CurlWriteToMemoryCallback() */

#endif


/*! Reads a whole file content into an allocated buffer.

\param prnArg
\param iArgMax maximum number of input buffer block resizes

\todo read only a range of lines from file

\todo detect Unicode BOM, store in resNode

\return TRUE if successful
*/
BOOL_T
resNodeReadContent(resNodePtr prnArg, int iArgMax)
{
  BOOL_T fResult = FALSE;

  if (prnArg != NULL) {
    size_t cchResultAllocated = 0;
    size_t cchReadInput = 0; /*! counter for collected string length */
    int iLoop;

    assert(prnArg->eMode == mode_read);
    // assert(resNodeGetType(prnArg) != rn_type_archive);
    assert(resNodeGetType(prnArg) != rn_type_file_in_archive);
    assert(resNodeGetType(prnArg) != rn_type_dir_in_archive);
    assert(resNodeGetType(prnArg) != rn_type_file_compressed);

    resNodeResetError(prnArg);
    // assert(resNodeGetHandleIO(prnArg) != NULL || resNodeGetHandleIO(resNodeGetParent(prnArg)) != NULL ||
    // resNodeGetHandleIO(resNodeGetAncestorArchive(prnArg)) != NULL);

    resNodeResetContentPtr(prnArg);
    fResult = TRUE;
    if (resNodeGetBlockSize(prnArg) < 1) {
      resNodeSetBlockSize(prnArg, BUFFER_LENGTH);
    }

    if (resNodeIsURL(prnArg) && prnArg->handleIO != NULL) {
#ifdef HAVE_LIBCURL
      CURLcode res = CURLE_OK;

      res = curl_easy_perform((CURL *)resNodeGetHandleIO(prnArg));
      if (res == CURLE_OK) {
	char *pcType = NULL;

	res = curl_easy_getinfo((CURL *)resNodeGetHandleIO(prnArg), CURLINFO_CONTENT_TYPE, &pcType);
	if (res == CURLE_OK && STR_IS_NOT_EMPTY(pcType)) {
	  PrintFormatLog(3, "URL Content Type is '%s'", pcType);
	  prnArg->eMimeType = resMimeGetType(pcType);
	}

	/* prnArg->liSizeContent is set by CurlWriteToMemoryCallback() already */
      }
      else {
	resNodeSetError(prnArg, rn_error_memory, "curl_easy_perform() failed: %s", curl_easy_strerror(res));
	fResult = FALSE;
      }
#else
      PrintFormatLog(1, "No access to URL '%s' without libcurl", resNodeGetNameNormalized(prnArg));
#endif
    }
    else if (resNodeGetType(prnArg) == rn_type_file || resNodeGetType(prnArg) == rn_type_stdin) {
      char *pchI = NULL; /*! pointer to collection buffer */

      pchI = xmlMalloc(resNodeGetBlockSize(prnArg));

      iLoop = (iArgMax < 1 || iArgMax > 1024 * 1024) ? 1024 : iArgMax;
      do {

	if (iLoop < 1) {
	  resNodeSetError(prnArg, rn_error_memory, "Maximum buffer size '%i' Byte reached", cchResultAllocated);
	  break;
	}

#ifdef _MSC_VER
	cchReadInput = fread_s((void *)pchI, resNodeGetBlockSize(prnArg), resNodeGetBlockSize(prnArg), (size_t)1, (FILE *)resNodeGetHandleIO(prnArg));
#else
	cchReadInput = fread(pchI, (size_t)1, resNodeGetBlockSize(prnArg), (FILE *)resNodeGetHandleIO(prnArg));
#endif

	if (ferror((FILE *)resNodeGetHandleIO(prnArg))) {
	  resNodeSetError(prnArg, rn_error_read, "File read error");
	  fResult = FALSE;
	}
	else {
	  resNodeAppendContent(prnArg, pchI, cchReadInput);
	  iLoop--;
	}

      } while (fResult == TRUE && cchReadInput == resNodeGetBlockSize(prnArg));

      resNodeResetMimeType(resNodeGetParent(prnArg));
      xmlFree(pchI);
    }
    else {
      resNodeSetError(prnArg, rn_error_undef, "Type error");
      fResult = FALSE;
    }
    fResult = (prnArg->pContent != NULL);
  }
  return fResult;
} /* end of resNodeReadContent() */


  /*! Reads the whole file content of this context, sets liSizeContent and returns a POINTER to the buffer.

  \param prnArg the context
  \param iArgMax maximum number of buffer blocks (Bytes = BUFFER_LENGTH * iArgMax)
  \return a pointer to the buffer of content or NULL in case of errors
  */
  void *resNodeGetContent(resNodePtr prnArg, int iArgMax)
  {
    void *pResult = NULL;

    if (prnArg) {
      if ((pResult = resNodeGetContentPtr(prnArg))) {
	/* content read already */
      }
      else {
	if (resNodeIsURL(prnArg)) {
	  if (resNodeOpen(prnArg, "rb") && resNodeReadContent(prnArg, iArgMax)) {
	    pResult = resNodeGetContentPtr(prnArg);
	  }
	}
	else if (resNodeIsStd(prnArg)) {
	  if (resNodeOpen(prnArg, "rb") && resNodeReadContent(prnArg, iArgMax)) {
	    pResult = resNodeGetContentPtr(prnArg);
	  }
	}
	else if (resNodeIsDatabase(prnArg)) { /* handle database content as binary content only */
	  if (resNodeOpen(prnArg, "rb") == FALSE) {
	    /* error while opening */
	  }
	  else if (resNodeReadContent(prnArg, iArgMax)) {
	    pResult = resNodeGetContentPtr(prnArg);
	  }
	}
#if 0
	else if (resNodeIsFileInZip(prnArg)) {
	  resNodePtr prnZip;
	  resNodePtr prnUrl;

	  if ((prnZip = resNodeGetAncestorZip(prnArg)) != NULL) {
	    if (resPathIsURL(resNodeGetNameNormalized(prnZip)) && !resNodeIsMemory(prnZip)) {
	      /* must fetch zip content from URL into memory */
	      if (resNodeOpen(prnZip, "rb")) {
		resNodeReadContent(prnZip, iArgMax);
		resNodeClose(prnZip);
	      }
	    }
	    if (zipAppendEntries(prnZip, NULL, TRUE)) {
	      pResult = resNodeGetContentPtr(prnArg);
	    }
	  }
	}
#endif
	else if (resNodeIsFileInArchive(prnArg)) {
	  resNodePtr prnArchive;
	  resNodePtr prnUrl;

	  if ((prnArchive = resNodeGetAncestorArchive(prnArg)) != NULL) {

	    if (resPathIsURL(resNodeGetNameNormalized(prnArchive)) && !resNodeIsMemory(prnArchive)) {
	      /* must fetch archive content from URL into memory */
	      if (resNodeOpen(prnArchive, "rb")) {
		resNodeReadContent(prnArchive, iArgMax);
		resNodeClose(prnArchive);
	      }
	    }

#ifdef HAVE_LIBARCHIVE
	  if (arcAppendEntries(prnArchive, NULL, TRUE)) {
	    pResult = resNodeGetContentPtr(prnArg);
	  }
#endif
	}
      }
      else if (resNodeGetHandleIO(prnArg) == NULL && resNodeOpen(prnArg, "rb") == FALSE) {
	/* error while opening */
      }
      else if (resNodeReadContent(prnArg, iArgMax)) {
	pResult = resNodeGetContentPtr(prnArg);
      }
      resNodeClose(prnArg);
    }
  }
  return pResult;
} /* end of resNodeGetContent() */


/*! Reads and returns the whole file content of this context as a Base 64 string.

\param prnArg the context
\param iArgMax maximum number of buffer blocks (Bytes = BUFFER_LENGTH * iArgMax)
\return a pointer to the string of content, and reset content of this context struct
*/
xmlChar *
resNodeGetContentBase64Eat(resNodePtr prnArg, int iArgMax)
{
  char *pcResult = NULL;

  if (resNodeGetContent(prnArg, iArgMax)) {
    size_t lenResult;

    if ((lenResult = resNodeGetSize(prnArg) * 2) > 0) {
      pcResult = (char *)xmlMalloc(lenResult + 1);
      if (pcResult) {
	if (! base64encode(prnArg->pContent, resNodeGetSize(prnArg), pcResult, lenResult)) {
	  resNodeSetError(prnArg,rn_error_encoding, "Base 64 encoding error");
	  xmlFree(pcResult);
	  pcResult = NULL;
	}
      }
    }
  }
  return BAD_CAST pcResult;
} /* end of resNodeGetContentBase64Eat() */


/*! callback procedure to concatenate the content of this resource.

\param prnArg the context
\param pArg pointer to next block
\param iSize block size
\return pointer to the content of this context
*/
void *
resNodeAppendContent(resNodePtr prnArg, void *pArg, size_t iSize)
{
  void *pResult = NULL;

  if (prnArg) {
    prnArg->pContent = xmlRealloc(prnArg->pContent, prnArg->liSizeContent + iSize + 1);
    if (prnArg->pContent) {
      char *pcT = (char *)prnArg->pContent; /* to provide a type with size for array memory access */
      
      memcpy(&(pcT[prnArg->liSizeContent]), pArg, iSize);
      prnArg->liSizeContent += iSize;
      pcT[prnArg->liSizeContent] = '\0'; /* null termination for string content */
      pResult = prnArg->pContent;
    }
    else {
      resNodeSetError(prnArg, rn_error_memory, "MEM error");
    }
  }
  return pResult;
} /* end of resNodeAppendContent() */


/*! Sets and returns the content of this context.

\param prnArg the context
\return pArg
*/
void *
resNodeSetContentPtr(resNodePtr prnArg, void *pArg, size_t iSize)
{
  void *pResult = NULL;
  
  if (prnArg) {
    resNodeResetUsageCount(prnArg);
    xmlFree(prnArg->pContent); /* old Buffer */
    prnArg->pContent = pArg;
    pResult = prnArg->pContent;
    prnArg->liSizeContent = iSize;
  }
  return pResult;
} /* end of resNodeSetContentPtr() */


/*! \return a pointer to context content buffer
*/
void *
resNodeGetContentPtr(resNodePtr prnArg)
{
  void *pResult = NULL;

  if (prnArg) {
    pResult = BAD_CAST prnArg->pContent; /* old Buffer */
    resNodeIncrUsageCount(prnArg);
  }

  return pResult;
} /* end of resNodeGetContentPtr() */


/*! \return a pointer to context content buffer
*/
void *
resNodeEatContentPtr(resNodePtr prnArg)
{
  void *pResult = NULL;

  if (prnArg) {
    pResult = resNodeGetContentPtr(prnArg); /* old Buffer */
    prnArg->pContent = NULL;
  }

  return pResult;
} /* end of resNodeEatContentPtr() */


/*! eats and returns the content of this context.

\param prnArg the context
\return pucArg
*/
xmlDocPtr
resNodeSetContentDocEat(resNodePtr prnArg, xmlDocPtr pdocArg)
{
  if (prnArg) {
    xmlFreeDoc(prnArg->pdocContent); /* old DOM */
    resNodeResetUsageCount(prnArg);
    prnArg->pdocContent = pdocArg;
  }
  return pdocArg;
} /* end of resNodeSetContentDocEat() */


/*! \return a pointer to context content DOM

*/
xmlDocPtr
resNodeGetContentDoc(resNodePtr prnArg)
{
  xmlDocPtr pdocResult = NULL;

  if (prnArg) {
    if (prnArg->pdocContent == NULL) {
      pdocResult = resNodeReadDoc(prnArg);
      resNodeSetContentDocEat(prnArg, pdocResult);
    }
    else {
      pdocResult = prnArg->pdocContent;
      resNodeIncrUsageCount(prnArg);
    }
  }
  return pdocResult;
} /* end of resNodeGetContentDoc() */


/*! \return a pointer to context content DOM
*/
xmlDocPtr
resNodeEatContentDoc(resNodePtr prnArg)
{
  xmlDocPtr pdocResult = NULL;

  if (prnArg) {
    pdocResult = resNodeGetContentDoc(prnArg); /* old DOM */
    prnArg->pdocContent = NULL;
  }

  return pdocResult;
} /* end of resNodeEatContentDoc() */


/*! \return TRUE if content was transferred from prnArgFrom to prnArgTo
*/
BOOL_T
resNodeSwapContent(resNodePtr prnArgFrom, resNodePtr prnArgTo)
{
  BOOL_T fResult = FALSE;

  if (prnArgTo == NULL) {
    resNodeSetError(prnArgTo,rn_error_copy,"There is no target for content");
  }
  else if (resNodeGetContent(prnArgFrom, 1024) == FALSE) {
    resNodeSetError(prnArgFrom, rn_error_copy, "Can't get content '%s'", resNodeGetNameNormalized(prnArgFrom));
  }
  else { /* swap content */
    void *pContent;
    size_t liSizeContent;		/*! size of _read_ content at pContent */
    size_t liSizeBlock;  		/*! size of blocks while reading content */
    xmlDocPtr pdocContent;	/*! pointer to read DOM */
    int iCountUse;  		/*! usage counter for this context (caching) */

    pContent = prnArgTo->pContent;
    liSizeContent = prnArgTo->liSizeContent;
    liSizeBlock = prnArgTo->liSizeBlock;
    pdocContent = prnArgTo->pdocContent;
    iCountUse = prnArgTo->iCountUse;
    
    prnArgTo->pContent = prnArgFrom->pContent;
    prnArgTo->liSizeContent = prnArgFrom->liSizeContent;
    prnArgTo->liSizeBlock = prnArgFrom->liSizeBlock;
    prnArgTo->pdocContent = prnArgFrom->pdocContent;
    prnArgTo->iCountUse = prnArgFrom->iCountUse;
    
    prnArgFrom->pContent = pContent;
    prnArgFrom->liSizeContent = liSizeContent;
    prnArgFrom->liSizeBlock = liSizeBlock;
    prnArgFrom->pdocContent = pdocContent;
    prnArgFrom->iCountUse = iCountUse;
    
    fResult = TRUE;
  }

  return fResult;
} /* end of resNodeSwapContent() */


/*! wrapper fuction for xmlReadFile() with filename decoding
*/
xmlDocPtr
resNodeReadDoc(resNodePtr prnArg)
{
  xmlDocPtr pdocResult = NULL;
  int options = 0;

  if (resNodeIsReadable(prnArg) == FALSE) {
    PrintFormatLog(1, "Unusable read context '%s'", resNodeGetNameNormalized(prnArg));
  }
#if 0
  else if (prnArg != NULL && prnArg->pdocContent == NULL && prnArg->pContent != NULL && resMimeIsXml(prnArg->eMimeType) &&
	(StringBeginsWith(prnArg->pContent, "<?xml version=") || StringBeginsWith(prnArg->pContent, "<!DOCTYPE ") ||
	 StringBeginsWith(prnArg->pContent, "<html>"))) {
      prnArg->pdocContent = xmlParseMemory(prnArg->pContent, prnArg->liSizeContent);
      pdocResult = prnArg->pdocContent;
    } 
  else if (resNodeIsError(prnArg)) {
    PrintFormatLog(1, "Unusable read context '%s'", resNodeGetNameNormalized(prnArg));
  }
  else if ((pdocResult = cxpCtxtCacheGetDoc(pccArg, resNodeGetNameNormalized(prnArg))) != NULL) {
    /* there is a cached DOM */
    pdocResult = xmlCopyDoc(pdocResult, 1);
    /* found in cache, set file context to DOM */
    resNodeChangeDomURL(pdocResult, prnArg);
  }
#ifndef HAVE_LIBARCHIVE
  else if (resNodeIsFileInArchive(prnArg)) {
    PrintFormatLog(1, "Cant uncompress file '%s'", resNodeGetNameNormalized(prnArg));
  }
#endif
#endif
  else {

    if (resNodeGetContent(prnArg,1024)) {
      if (resNodeGetMimeType(prnArg) == MIME_TEXT_HTML) {
	options = XML_PARSE_RECOVER | XML_PARSE_NOENT | XML_PARSE_NOERROR | XML_PARSE_NOWARNING | XML_PARSE_NSCLEAN | XML_PARSE_NODICT;
	pdocResult = htmlReadMemory((const char *)resNodeGetContentPtr(prnArg), (int)resNodeGetSize(prnArg), NULL, (const char*)"UTF-8", options);
      }
      else {
	options = XML_PARSE_RECOVER | XML_PARSE_NOENT | XML_PARSE_NOERROR | XML_PARSE_NOWARNING | XML_PARSE_NSCLEAN | XML_PARSE_NODICT;
	pdocResult = xmlReadMemory((const char *)resNodeGetContentPtr(prnArg), (int)resNodeGetSize(prnArg), NULL, NULL, options);
      }
    }

    if (pdocResult) {
      // set file context to DOM
      resNodeChangeDomURL(pdocResult,prnArg);
    }
    else {
      PrintFormatLog(1,"Cant parse file '%s'", resNodeGetNameNormalized(prnArg));
    }
  }

  return pdocResult;
} /* end of resNodeReadDoc() */


/*! \param prnArg the context
  \return increased value of iCountUse or -1 in case of errors
*/
int
resNodeIncrUsageCount(resNodePtr prnArg)
{
  if (prnArg != NULL) {
    prnArg->iCountUse++;
    return prnArg->iCountUse;
  }
  return -1;
} /* end of resNodeIncrUsageCount() */


/*! \param prnArg the context
  \return increased value of iCountUse or -1 in case of errors
*/
int
resNodeResetUsageCount(resNodePtr prnArg)
{
  if (prnArg != NULL) {
    prnArg->iCountUse = 0;
    return prnArg->iCountUse;
  }
  return -1;
} /* end of resNodeResetUsageCount() */


/*! \param prnArg the context
  \return value of iCountUse or -1 in case of errors
*/
int
resNodeGetUsageCount(resNodePtr prnArg)
{
  if (prnArg != NULL) {
    return prnArg->iCountUse;
  }
  return -1;
} /* end of resNodeGetUsageCount() */


/*! \return handleIO of prnArg or NULL in case of errors
*/
void *
resNodeGetHandleIO(resNodePtr prnArg)
{
  if (prnArg != NULL) {
    return prnArg->handleIO;
  }
  return NULL;
} /* end of resNodeGetHandleIO() */


/*! Sets and returns the liSizeBlock of this context.

  \param prnArg the context
  \return value of liSizeBlock or -1 in case of errors
*/
size_t
resNodeSetBlockSize(resNodePtr prnArg, size_t iArg)
{
  if (prnArg != NULL) {
    prnArg->liSizeBlock = iArg;
    return resNodeGetBlockSize(prnArg);
  }
  return 0;
} /* end of resNodeSetBlockSize() */


/*! Sets and returns the liSizeBlock of this context.

\param prnArg the context
\return value of liSizeBlock or -1 in case of errors
*/
size_t
resNodeGetBlockSize(resNodePtr prnArg)
{
  if (prnArg != NULL) {
    return prnArg->liSizeBlock;
  }
  return 0;
} /* end of resNodeGetBlockSize() */


/*! change the URL of DOM pdocArg to URI of pccArg

\param pdocArg pointer to DOM
\param pccArg the filesystem context

 */
void
resNodeChangeDomURL(xmlDocPtr pdocArg, resNodePtr prnArg)
{
#ifdef DEBUG
  PrintFormatLog(3,"resNodeChangeDomURL(pdocArg=%0x,prnArg=%0x) to '%s'",pdocArg,prnArg,resNodeGetURI(prnArg));
#endif

  if (pdocArg != NULL && prnArg != NULL) {
    xmlChar *pucUri;

    pucUri = resNodeGetURI(prnArg);
    if (pucUri) {
      xmlFree((void *) pdocArg->URL);
      pdocArg->URL = xmlStrdup(pucUri);
    }
  }
}
/* end of resNodeChangeDomURL() */


#ifdef TESTCODE
#include "test/test_res_node_io.c"
#endif
