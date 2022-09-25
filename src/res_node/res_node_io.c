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

#include <libxml/HTMLtree.h>
#include <libxml/tree.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#ifdef HAVE_LIBCURL
#include <curl/curl.h>
#else
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
#ifdef HAVE_LIBARCHIVE
#include <archive/archive.h>
#endif

static BOOL_T
OpenArchive(resNodePtr prnArg);

static BOOL_T
CloseArchive(resNodePtr prnArg);

static BOOL_T
OpenURL(resNodePtr prnArg);

static BOOL_T
CloseURL(resNodePtr prnArg);

#ifdef HAVE_LIBCURL
static size_t
_CurlReadFromMemoryCallback(void *contents, size_t size, size_t nmemb, void *prnArg);

static size_t
CurlWriteToMemoryCallback(void *pucArgBlock, size_t iArgSize, size_t iArgNumber, void *pArg);
#endif

static BOOL_T
OpenSqlite(resNodePtr prnArg);

static BOOL_T
CloseSqlite(resNodePtr prnArg);

static BOOL_T
resNodeReadContent(resNodePtr prnArg, int iArgMax);

static size_t
resNodeSetBlockSize(resNodePtr prnArg, size_t iArg);

static size_t
resNodeGetBlockSize(resNodePtr prnArg);

static int
resNodeIncrUsageCount(resNodePtr prnArg);

static int
resNodeResetUsageCount(resNodePtr prnArg);


/*! opens context prnArg according to pchArgMode ("r|w|w+|wb|wb|rz|wz|a|aw|rd|wd|wd+")

  set the context handle and access method for IO

  \return TRUE if successful
*/
BOOL_T
resNodeOpen(resNodePtr prnArg, const char *pchArgMode)
{
  BOOL_T fResult = FALSE;

  assert(pchArgMode != NULL && (strchr(pchArgMode, (int)'r') || strchr(pchArgMode, (int)'w')));

  if (resNodeResetError(prnArg)) {

    assert(prnArg->handleIO == NULL || prnArg->handleIO == (void *)stdin || prnArg->handleIO == (void *)stdout);
    
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
    else if (strchr(pchArgMode,(int)'a') && resMimeIsArchive(resNodeGetMimeType(prnArg))) {

      if (resPathIsURL(resNodeGetNameNormalized(prnArg))) {
	fResult = OpenURL(prnArg);
      }
      else {
	fResult = OpenArchive(prnArg);
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

\todo handle in-memory achives for stdout/stdin s. archive_write_open_memory()

\return TRUE if successful
*/
BOOL_T
OpenArchive(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

#ifdef HAVE_LIBARCHIVE
  assert(prnArg != NULL && prnArg->handleIO == NULL);

  if (prnArg->eMode == mode_read) {
    prnArg->handleIO = (void *)archive_read_new();
    if (prnArg->handleIO) {
      if (arcMapReadFormat(prnArg)) {
	if (resNodeIsMemory(prnArg)) { /* achive in memory already */
	  if (archive_read_open_memory((arcPtr)prnArg->handleIO, prnArg->pContent, resNodeGetSize(prnArg)) == ARCHIVE_OK) {
	    prnArg->fExist = TRUE;
	    prnArg->eAccess = rn_access_archive;
	    fResult = TRUE;
	    PrintFormatLog(4, "archive_read_open_memory('%s') OK", resNodeGetNameNormalized(prnArg));
	  }
	  else {
	    CloseArchive(prnArg);
	    resNodeSetError(prnArg, rn_error_archive, "archive_read_open_memory() failed");
	  }
	  /*!\todo read archive from memory buffer ":memory:" archive_read_open_memory() */
	}
	else if (archive_read_open_filename((arcPtr)prnArg->handleIO, resNodeGetNameNormalizedNative(prnArg), BUFFER_LENGTH) == ARCHIVE_OK) {
	  prnArg->fExist = TRUE;
	  prnArg->eAccess = rn_access_archive;
	  fResult = TRUE;
	  PrintFormatLog(4, "archive_read_open_filename('%s') OK", resNodeGetNameNormalized(prnArg));
	}
	else {
	  CloseArchive(prnArg);
	  resNodeSetError(prnArg, rn_error_archive, "archive_read_open_filename('%s') failed", resNodeGetNameNormalized(prnArg));
	}
      }
      else {
	CloseArchive(prnArg);
	resNodeSetError(prnArg, rn_error_archive, "arcMapReadFormat('%s') failed", resNodeGetNameNormalized(prnArg));
      }
    }
    else {
      CloseArchive(prnArg);
      resNodeSetError(prnArg, rn_error_archive, "archive_read_new('%s') failed", resNodeGetNameNormalized(prnArg));
    }
  }
  else if (prnArg->eMode == mode_write) {
    /*\todo append content to existing archive file */

    /*\todo delete existing archive file first */

    prnArg->handleIO = (void *)archive_write_new();
    if (prnArg->handleIO) {
      //archive_write_set_options((arcPtr)prnArg->handleIO, "compression=store");
      if ((resNodeGetMimeType(prnArg) == MIME_APPLICATION_ZIP
	&& archive_write_set_format_zip((arcPtr)prnArg->handleIO) == ARCHIVE_OK)
#if 0
	||
	(resNodeGetMimeType(prnArg) == MIME_APPLICATION_X_TAR
	&& archive_write_set_format_gnutar((arcPtr)prnArg->handleIO) == ARCHIVE_OK)
	||
	(resNodeGetMimeType(prnArg) == MIME_APPLICATION_X_ISO9660_IMAGE
	&& archive_write_set_format_iso9660((arcPtr)prnArg->handleIO) == ARCHIVE_OK)
#endif
	) {

	if (archive_write_add_filter_none((arcPtr)prnArg->handleIO) == ARCHIVE_OK) {
#if 0
	  if (resNodeIsStd(prnArg)) {
	    const size_t iSize = 1024 * 1024 * 1024;

	    if ((prnArg->pContent = xmlMalloc(iSize))) {
	      if (archive_write_open_memory((arcPtr)prnArg->handleIO, prnArg->pContent, iSize, &prnArg->liSizeContent) == ARCHIVE_OK) {
		prnArg->fExist = TRUE;
		prnArg->eAccess = rn_access_archive;
		fResult = TRUE;
		PrintFormatLog(4, "archive_write_open_memory('%s') OK", resNodeGetNameNormalized(prnArg));
	      }
	    }
	    else {
	      CloseArchive(prnArg);
	      resNodeSetError(prnArg, rn_error_archive, "xmlMalloc() failed");
	    }
	  }
#endif
	  if (archive_write_open_filename((arcPtr)prnArg->handleIO, resNodeGetNameNormalizedNative(prnArg)) == ARCHIVE_OK) {
	    prnArg->fExist = TRUE;
	    prnArg->eAccess = rn_access_archive;
	    fResult = TRUE;
	    PrintFormatLog(4, "archive_write_open_filename('%s') OK", resNodeGetNameNormalized(prnArg));
	  }
	  else {
	    CloseArchive(prnArg);
	    resNodeSetError(prnArg, rn_error_archive, "archive_write_new('%s') failed", resNodeGetNameNormalized(prnArg));
	  }
	}
	else {
	  CloseArchive(prnArg);
	  //	      resNodeSetError(prnArg,rn_error_archive, "archive_write_set_format_ustar('%s') failed: %s",
	  //			     resNodeGetNameNormalized(prnArg), archive_error_string((arcPtr)resNodeGetHandleIO(prnArg)));
	}
      }
      else {
	CloseArchive(prnArg);
	resNodeSetError(prnArg, rn_error_archive, "archive_read_support_format_*('%s') failed: %s",
	  resNodeGetNameNormalized(prnArg), archive_error_string((arcPtr)resNodeGetHandleIO(prnArg)));
      }
    }
    else {
      CloseArchive(prnArg);
      resNodeSetError(prnArg, rn_error_archive, "archive_write_new('%s') failed", resNodeGetNameNormalized(prnArg));
    }
  }
  else {
    resNodeSetError(prnArg, rn_error_open, "unknown mode for archive opening '%s'", resNodeGetNameNormalized(prnArg));
  }
#endif

  return fResult;
} /* end of OpenArchive() */


/*! opens context prnArg

\return TRUE if successful
*/
BOOL_T
CloseArchive(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

#ifdef HAVE_LIBARCHIVE
  assert(prnArg != NULL);

  if (prnArg->eMode == mode_read) {
    archive_read_close((arcPtr)resNodeGetHandleIO(prnArg));
    if (archive_read_free((arcPtr)resNodeGetHandleIO(prnArg)) == ARCHIVE_OK) {
      fResult = TRUE;
    }
    else {
      resNodeSetError(prnArg, rn_error_archive, "Error archive_read_free('%s')", resNodeGetNameNormalized(prnArg));
    }
  }
  else if (prnArg->eMode == mode_write) {
    archive_write_close((arcPtr)resNodeGetHandleIO(prnArg));
    if (archive_write_free((arcPtr)resNodeGetHandleIO(prnArg)) == ARCHIVE_OK) {
      fResult = TRUE;
    }
    else {
      resNodeSetError(prnArg, rn_error_archive, "Error archive_write_free('%s')", resNodeGetNameNormalized(prnArg));
    }
  }
  else {
    resNodeSetError(prnArg, rn_error_archive, "Error closing archive '%s'", resNodeGetNameNormalized(prnArg));
  }
  prnArg->handleIO = NULL;
  prnArg->eAccess = rn_access_undef;
#endif

  return fResult;
} /* end of CloseArchive() */


/*! opens context prnArg

\return TRUE if successful
*/
BOOL_T
OpenURL(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  assert(prnArg != NULL && prnArg->handleIO == NULL);

#ifdef HAVE_LIBCURL
  // s. "curl\docs\examples\getinmemory.c" or "curl\docs\examples\fopen.c"

  if (prnArg->eMode == mode_write) {
    resNodeSetError(prnArg, rn_error_open, "writing mode for URL not implemented yet '%s'", resNodeGetNameNormalized(prnArg));
    //prnArg->eAccess = rn_access_curl;
  }
  else {
    /* init the curl session */
    prnArg->handleIO = curl_easy_init();
    if (prnArg->handleIO) {
      CURLcode res;

      if (prnArg->curlURL) {
	curl_easy_setopt(prnArg->handleIO, CURLOPT_CURLU, prnArg->curlURL);
      }
      else {
	/* specify URL to get */
	curl_easy_setopt(prnArg->handleIO, CURLOPT_URL, resNodeGetNameNormalizedNative(prnArg));
      }

      curl_easy_setopt(prnArg->handleIO, CURLOPT_CONNECT_ONLY, 1L);
      curl_easy_setopt(prnArg->handleIO, CURLOPT_TIMEOUT, 2L);
      res = curl_easy_perform(prnArg->handleIO);
      if (res == CURLE_OK) { /* only connected! */
	long code;

	curl_easy_setopt(prnArg->handleIO, CURLOPT_CONNECT_ONLY, 0L);

	/* send all data to this function  */
	curl_easy_setopt(prnArg->handleIO, CURLOPT_WRITEFUNCTION, CurlWriteToMemoryCallback);

	/* we pass our 'chunk' struct to the callback function */
	curl_easy_setopt(prnArg->handleIO, CURLOPT_WRITEDATA, (void *)prnArg);

	/* some servers don't like requests that are made without a user-agent
	field, so we provide one */
	curl_easy_setopt(prnArg->handleIO, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	/*\todo get "Content-Disposition" as prnArg->alias */

	prnArg->eAccess = rn_access_curl;
	prnArg->fExist = TRUE;
	prnArg->fStat = TRUE;
	fResult = TRUE;
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
  }
#else
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
#else
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
    //assert(prnArg->eAccess == rn_access_undef && prnArg->handleIO == NULL);
    //assert(resNodeIsArchive(prnArg) == FALSE);

    fResult = (resNodeOpen(prnArg,"w") && resNodeSaveContent(prnArg) && resNodeClose(prnArg));
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
    fResult = ((prnArg->handleIO != NULL && prnArg->eAccess != rn_access_undef) || prnArg->eType == rn_type_file_in_archive);
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
    if (prnArg->eAccess == rn_access_archive) {
      fResult = CloseArchive(prnArg);
    }
    else if (prnArg->eType == rn_type_file_in_archive) {
    }
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
    pucFilename = resPathGetQuoted(resNodeGetNameNormalized(prnArg));
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
    char *pcBlockNew;
    resNodePtr prnArg = (struct _resNode *) pArg;

    iBlockSize = iArgSize * iArgNumber;
    PrintFormatLog(3, "CurlWriteToMemoryCallback '%i' Byte of '%s'", iBlockSize, resNodeGetNameNormalized(prnArg));
    pcBlockNew = (char *)xmlRealloc(prnArg->pContent, prnArg->liSizeContent + iBlockSize + 1);
    if (pcBlockNew) {
      memcpy(&(pcBlockNew[prnArg->liSizeContent]), pucArgBlock, iBlockSize);
      prnArg->liSizeContent += iBlockSize;
      pcBlockNew[prnArg->liSizeContent] = '\0';
      prnArg->pContent = pcBlockNew;
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
    char *pchInput = NULL;  /*! pointer to collection buffer */
    size_t cchResultAllocated = 0;
    size_t cchResult = 0;		/*! counter for collected string length */
    size_t cchReadInput = 0;		/*! counter for collected string length */
    int iLoop;

    assert(prnArg->eMode == mode_read);
    //assert(resNodeGetType(prnArg) != rn_type_archive);
    assert(resNodeGetType(prnArg) != rn_type_file_in_archive);
    assert(resNodeGetType(prnArg) != rn_type_dir_in_archive);
    assert(resNodeGetType(prnArg) != rn_type_file_compressed);

    resNodeResetError(prnArg);
    //assert(resNodeGetHandleIO(prnArg) != NULL || resNodeGetHandleIO(resNodeGetParent(prnArg)) != NULL || resNodeGetHandleIO(resNodeGetAncestorArchive(prnArg)) != NULL);

    resNodeSetContentPtr(prnArg,NULL,0);
    fResult = TRUE;
    if (resNodeGetBlockSize(prnArg) < 1) {
      resNodeSetBlockSize(prnArg,BUFFER_LENGTH);
    }

    if (prnArg->eAccess == rn_access_curl && prnArg->handleIO != NULL) {
#ifdef HAVE_LIBCURL
      CURLcode res;

      /* get it! */
      res = curl_easy_perform((CURL *) resNodeGetHandleIO(prnArg));
      if (res == CURLE_OK) {
	char *pcType = NULL;
	curl_off_t cl;
	
	/* check the size: https://curl.haxx.se/libcurl/c/CURLINFO_CONTENT_LENGTH_DOWNLOAD_T.html */
	res = curl_easy_getinfo((CURL *)resNodeGetHandleIO(prnArg), CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &cl);
	if (res == CURLE_OK) {
	  prnArg->liSize = cl;
	}

	/* ask for filetime: https://curl.haxx.se/libcurl/c/CURLINFO_FILETIME_T.html */
	curl_easy_setopt((CURL *)resNodeGetHandleIO(prnArg), CURLOPT_FILETIME, 1L);
	if (res == CURLE_OK) {
	  curl_off_t filetime;

	  res = curl_easy_getinfo((CURL *)resNodeGetHandleIO(prnArg), CURLINFO_FILETIME_T, &filetime);
	  if ((res == CURLE_OK) && (filetime >= 0)) {
	    prnArg->tMtime = (time_t)filetime;
	  }
	}
	curl_easy_setopt((CURL *)resNodeGetHandleIO(prnArg), CURLOPT_FILETIME, 0L);

	res = curl_easy_getinfo((CURL *) resNodeGetHandleIO(prnArg),CURLINFO_CONTENT_TYPE,&pcType);
	if (res == CURLE_OK && STR_IS_NOT_EMPTY(pcType)) {
	  PrintFormatLog(3, "URL Content Type is '%s'", pcType);
	  prnArg->eMimeType = resMimeGetType(pcType);
	}
      }
      else {
	resNodeSetError(prnArg,rn_error_memory, "curl_easy_perform() failed: %s", curl_easy_strerror(res));
	fResult = FALSE;
      }
#elif 0
      /*\bug fix code */
      if (resNodeGetType(prnArg) == rn_type_url_http) {
	cchReadInput = xmlIOHTTPRead(resNodeGetHandleIO(prnArg), &(pchInput[cchResult]), resNodeGetBlockSize(prnArg));
      }
      else if (resNodeGetType(prnArg) == rn_type_url_ftp) {
	cchReadInput = xmlIOFTPRead(resNodeGetHandleIO(prnArg), &(pchInput[cchResult]), resNodeGetBlockSize(prnArg));
      }
#endif
    }
    else if (resNodeGetSize(prnArg) > 0) { /* content size is known already */
      void *pInput;  /*! pointer to buffer */

      pInput = xmlMalloc(resNodeGetSize(prnArg) + 1);
      if (pInput) {
	memset(pInput,0,resNodeGetSize(prnArg) + 1); /* to get a null termination of buffer */
#ifdef _MSC_VER
	cchReadInput = fread_s(pInput, (size_t)(iArgMax * BUFFER_LENGTH), (size_t)1, resNodeGetSize(prnArg), (FILE *)resNodeGetHandleIO(prnArg));
#else
	cchReadInput = fread(pInput, 1, resNodeGetSize(prnArg), (FILE *)resNodeGetHandleIO(prnArg));
#endif

	/*!\todo handle VC++ newline processing */
	if (ferror((FILE *)resNodeGetHandleIO(prnArg))) {
	  /*!\todo why is input_length != stStat.st_size with VC++ */
	  xmlFree(pInput);
	  resNodeSetError(prnArg, rn_error_read, "File read error");
	  fResult = FALSE;
	}
	else if (cchReadInput != resNodeGetSize(prnArg)) {
	  xmlFree(pInput);
	  resNodeSetError(prnArg, rn_error_memory, "Memory error");
	  fResult = FALSE;
	}
	else {
	  resNodeSetContentPtr(prnArg, pInput, cchReadInput);
	}
      }
      else {
	resNodeSetError(prnArg, rn_error_memory, "Memory error");
	fResult = FALSE;
      }
    }
    else { /* loop for dynamically allocated memory blocks */
      iLoop = (iArgMax < 1 || iArgMax > 1024 * 1024) ? 1024 : iArgMax;
      do {

	if (iLoop < 1) {
	  resNodeSetError(prnArg,rn_error_memory, "Maximum buffer size '%i' Byte reached", cchResultAllocated);
	  break;
	}

	if (pchInput == NULL || cchResultAllocated - cchResult < resNodeGetBlockSize(prnArg)) {
	  /*
	    inrease the size of buffer
	  */
	  cchResultAllocated += resNodeGetBlockSize(prnArg) + 1;
#ifdef DEBUG
	  PrintFormatLog(3, "Resize text input buffer to '%i' Byte", cchResultAllocated);
#endif
	  pchInput = (char *)xmlRealloc((void *)pchInput, cchResultAllocated);
	  iLoop--;
	}

	if (pchInput) {
	  if (resNodeGetType(prnArg) == rn_type_file || resNodeGetType(prnArg) == rn_type_stdin) {
	    cchReadInput = fread(&(pchInput[cchResult]), (size_t)1, resNodeGetBlockSize(prnArg), (FILE *)resNodeGetHandleIO(prnArg));
	    /*!\todo handle VC++ newline processing */
	    if (ferror((FILE *)resNodeGetHandleIO(prnArg))) {
	      /*!\todo why is input_length != stStat.st_size with VC++ */
	      resNodeSetError(prnArg,rn_error_read, "File read error");
	      fResult = FALSE;
	    }
	  }
	}
	else {
	  resNodeSetError(prnArg,rn_error_memory, "Not enough memory");
	}
	cchResult += cchReadInput;
      } while (fResult == TRUE && cchReadInput == resNodeGetBlockSize(prnArg));

      if (fResult == TRUE && pchInput != NULL) {
	pchInput[cchResult] = '\0'; /*!\bug if (content length == n * resNodeGetBlockSize(prnArg)) */
	resNodeSetContentPtr(prnArg, (void *)pchInput, cchResult);
	if (resNodeGetType(resNodeGetParent(prnArg)) == rn_type_file_compressed) {
	  resNodeResetMimeType(resNodeGetParent(prnArg));
	}
      }
      else {
	xmlFree(pchInput);
	resNodeSetContentPtr(prnArg,NULL,0);
      }
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
void *
resNodeGetContent(resNodePtr prnArg, int iArgMax)
{
  void *pResult = NULL;

  if (prnArg) {
    if ((pResult = resNodeGetContentPtr(prnArg))) {
      /* content read already */
    }
    else {
      if (resNodeIsStd(prnArg)) {
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
      else if (resNodeIsFileInArchive(prnArg)) {
	resNodePtr prnArchive;
	resNodePtr prnUrl;

	if ((prnArchive = resNodeGetAncestorArchive(prnArg)) != NULL) {

	  if (resPathIsURL(resNodeGetNameNormalized(prnArchive)) && ! resNodeIsMemory(prnArchive)) {
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
  else if (resNodeIsError(prnArg)) {
    PrintFormatLog(1, "Unusable read context '%s'", resNodeGetNameNormalized(prnArg));
  }
  else if ((pdocResult = cxpCtxtCacheGetDoc(pccArg, resNodeGetNameNormalized(prnArg))) != NULL) {
    /* there is a cached DOM */
    pdocResult = xmlCopyDoc(pdocResult, 1);
    /* found in cache, set file context to DOM */
    resNodeChangeDomURL(pdocResult, prnArg);
  }
#endif
#ifdef HAVE_ZLIB
  else if (resNodeGetMimeType(prnArg) == MIME_APPLICATION_MMAP_XML
#ifndef HAVE_LIBARCHIVE
    || resNodeGetMimeType(prnArg) == MIME_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_WORDPROCESSINGML_DOCUMENT
    || resNodeGetMimeType(prnArg) == MIME_APPLICATION_VND_OASIS_OPENDOCUMENT_SPREADSHEET
    || resNodeGetMimeType(prnArg) == MIME_APPLICATION_VND_OASIS_OPENDOCUMENT_TEXT
#endif
  ) {
    char* pcT;
    xmlChar* pucPath = NULL;

    pucPath = xmlStrdup(BAD_CAST "zip:");
    pucPath = xmlStrcat(pucPath, resNodeGetNameNormalized(prnArg));

    if (resNodeGetMimeType(prnArg) == MIME_APPLICATION_MMAP_XML) {
      pucPath = xmlStrcat(pucPath, BAD_CAST "!/Document.xml");
    }
#ifndef HAVE_LIBARCHIVE
    else if (resNodeGetMimeType(prnArg) == MIME_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_WORDPROCESSINGML_DOCUMENT) {
      pucPath = xmlStrcat(pucPath, BAD_CAST "!/word/document.xml");
    }
    else if (resNodeGetMimeType(prnArg) == MIME_APPLICATION_VND_OASIS_OPENDOCUMENT_SPREADSHEET
	     || resNodeGetMimeType(prnArg) == MIME_APPLICATION_VND_OASIS_OPENDOCUMENT_TEXT) {
      pucPath = xmlStrcat(pucPath, BAD_CAST "!/content.xml");
    }
#endif

    pcT = resPathDecode(pucPath); /* handle non-ASCII paths */
    options = XML_PARSE_RECOVER | XML_PARSE_NOENT | XML_PARSE_NOERROR | XML_PARSE_NOWARNING | XML_PARSE_NSCLEAN | XML_PARSE_NODICT;
    pdocResult = xmlReadFile((const char*)pcT, NULL, options);
    /*!\todo re-implement using libarchive, to remove xmlzipio module */
    xmlFree(pcT);
    xmlFree(pucPath);
  }
#endif
#ifndef HAVE_LIBARCHIVE
  else if (resNodeIsFileInArchive(prnArg)) {
    PrintFormatLog(1, "Cant uncompress file '%s'", resNodeGetNameNormalized(prnArg));
  }
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
