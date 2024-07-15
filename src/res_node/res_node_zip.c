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

/*!\todo compress
    existing files
        XML structure
        Plain text list (like SQL result)

    empty directories

    DOM node content

    file comments

    to stdout

    in memory

    update of content?
    
    with or without path mapping
        zip

    <cxp:copy from"test/a.png" to="abc.zip!test-b/b.png"/>
*/


#include <zip.h>

/*
*/
#include <libxml/parser.h>

#include "basics.h"
#include "calendar_element.h"
#include <res_node/res_node_io.h>
#include <cxp/cxp.h>
#include "dom.h"
#include <cxp/cxp_dir.h>
#include "plain_text.h"
#include "utils.h"


#define CXP_CODING_ZIP "CP437"

iconv_t iconvZipEncode = NULL;     /*! charset of zip headers */

iconv_t iconvZipDecode = NULL;     /*! charset of zip headers */


static void
zipIconvCleanup(void);


/*!
*/
BOOL_T
zipIconvInit(void)
{
  iconvZipEncode = iconv_open(CXP_CODING_ZIP, "UTF-8");
  if (iconvZipEncode == (iconv_t)-1) {
    /* Something went wrong.  */
    PrintFormatLog(1, "Conversion to '" CXP_CODING_ZIP "' to UTF-8 not possible");
  }

  /*
  create the required conversion descriptors
  */
  iconvZipDecode = iconv_open("UTF-8", CXP_CODING_ZIP);
  if (iconvZipDecode == (iconv_t)-1) {
    /* Something went wrong.  */
    PrintFormatLog(1, "Conversion from '" CXP_CODING_ZIP "' to UTF-8 not possible");
  }

  /* register for exit() */
  if (atexit(zipIconvCleanup) != 0) {
    exit(EXIT_FAILURE);
  }

  return TRUE;
}
/* end of zipIconvInit() */


/* module cleanup for libiconv, runs in exit()
*/
void
zipIconvCleanup(void)
{
  if (iconvZipEncode != NULL) {
    iconv_close(iconvZipEncode);
    iconvZipEncode = NULL;
  }
  if (iconvZipDecode != NULL) {
    iconv_close(iconvZipDecode);
    iconvZipDecode = NULL;
  }
}
/* end of zipCleanup() */


/*! Encodes a UTF-8 path string to code page 437 (ZIP default).

\param pucArg pointer to path string to convert
\param ppcResult pointer to result pointer
\param fDecode flag for decode/encode
\return TRUE if encoding or decoding of filename is successful, else FALSE
*/
BOOL_T
zipGetFileNameEncoded(xmlChar *pucArg,char **ppcResult)
{
  BOOL_T fResult = FALSE;

  if (iconvZipEncode == NULL) {
    zipIconvInit();
  }

  assert(ppcResult != NULL);

  if (pucArg == NULL || isend(*pucArg)) {
    *ppcResult = NULL;
  }
  else { /* is UTF-8 encoding */
    char mpcOut[BUFFER_LENGTH];
    char *pchOut;
    char *pchIn;
    size_t int_in;
    size_t int_out;
    size_t nconv;

    int_in = xmlStrlen(pucArg) + 1;
    pchIn = (char *)pucArg;
    int_out = BUFFER_LENGTH - 1;
    pchOut = mpcOut;

    nconv = iconv(iconvZipEncode, (char **)&pchIn, &int_in, &pchOut, &int_out);
    //cxpCtxtEncError(pccArg,errno,nconv);
    pchOut = mpcOut;
    *ppcResult = (char *)xmlStrdup(BAD_CAST mpcOut);
    fResult = TRUE;
  }

  return fResult;
}
/* end of zipGetFileNameEncoded() */


/*! Decodes a ZIP path string to UTF-8.

\param pcArg pointer to path string to convert
\param ppucResult pointer to result pointer
\param fDecode flag for decode/encode
\return TRUE if encoding or decoding of filename is successful, else FALSE
*/
BOOL_T
zipGetFileNameDecoded(char *pcArg, xmlChar **ppucResult)
{
  BOOL_T fResult = FALSE;

  if (iconvZipDecode == NULL) {
    zipIconvInit();
  }

  assert(ppucResult != NULL);

  if (pcArg == NULL || isend(*pcArg)) {
    *ppucResult = NULL;
  }
  else if (xmlCheckUTF8(BAD_CAST pcArg)) { /* is UTF-8 encoding already */
    *ppucResult = xmlStrdup(BAD_CAST pcArg);
    fResult = TRUE;
  }
  else { /* is UTF-8 encoding */
    xmlChar mpucOut[BUFFER_LENGTH];
    char *pchOut;
    char *pchIn;
    size_t int_in;
    size_t int_out;
    size_t nconv;

    int_in = strlen((const char *)pcArg) + 1;
    pchIn = pcArg;
    int_out = BUFFER_LENGTH - 1;
    pchOut = (char *)mpucOut;

    nconv = iconv(iconvZipDecode, (char **)&pchIn, &int_in, &pchOut, &int_out);
    //cxpCtxtEncError(pccArg,errno,nconv);
    *ppucResult = xmlStrdup(mpucOut);
    fResult = TRUE;
  }

  return fResult;
}
/* end of zipGetFileNameDecoded() */


/*! wrapper for , append all directory entries as childs

similar to arcAppendEntries()

\param prnArgZip the zip node
\param re_match pointer to a compiled regexp
\param fArgContent flag to extract data of a matching header
\return TRUE if successful, else FALSE
*/
BOOL_T
zipAppendEntries(resNodePtr prnArgZip, const pcre2_code* re_match, BOOL_T fArgContent)
{
  BOOL_T fResult = FALSE;

  /*!\bug test permission to this directory first */

  /*!\todo handle zip in zip recursively */

  if (resNodeReadStatus(prnArgZip) && resNodeIsZipDocument(prnArgZip) && resNodeOpen(prnArgZip, "ra")) {
    char buf[BUFFER_LENGTH];
    int err;
    int i, len;

    PrintFormatLog(4, "Begin of '%s'", resNodeGetNameNormalized(prnArgZip));
    for (i = 0; i < zip_get_num_entries((struct zip *)resNodeGetHandleIO(prnArgZip), 0); i++) {
      struct zip_stat sb;

      fResult = TRUE;
      if (zip_stat_index((struct zip *)resNodeGetHandleIO(prnArgZip), i, 0, &sb) == 0) {
	resNodePtr prnInZip;
	    resNodePtr prnAncestor;
	char *pcContent = NULL;
	struct zip_file *zf;

	prnInZip = resNodeAddChildNew(prnArgZip, sb.name);
	resNodeSetSize(prnInZip, sb.size);
	// sb.mtime

	if ((zf = zip_fopen_index((struct zip *)resNodeGetHandleIO(prnArgZip), i, 0)) != NULL) {
	  long long sum;

	  PrintFormatLog(3, "zip[%i] of '%s'", i, resNodeGetNameNormalized(prnInZip));
	  sum = 0;
	  while (sum != sb.size) {
	    len = zip_fread(zf, buf, sizeof(buf));
	    if (len < 0) {
	      PrintFormatLog(1, "zip[%i] read error", i);
	fResult = FALSE;
	      break;
	    }
	    else {
	      resNodeAppendContent(prnInZip, (void *)buf, len);
	      sum += len;
	    }
	  }
	  zip_fclose(zf);
//assert(resNodeGetSi(prnArgZip) == sum);

	  resNodeResetMimeType(prnInZip);
	  prnInZip->fExist = TRUE;
	  prnInZip->fStat = TRUE;
	  prnInZip->fRead = TRUE;
	  prnInZip->eAccess = rn_access_zip;

	  /* set all parent directories in this zip too */
	  for (prnAncestor = resNodeGetParent(prnInZip); resNodeGetType(prnAncestor) == rn_type_dir_in_zip; prnAncestor = resNodeGetParent(prnAncestor)) {
	    prnAncestor->fExist = TRUE;
	    prnAncestor->fStat = TRUE;
	    prnAncestor->fRead = TRUE;
	    prnAncestor->eAccess = rn_access_zip;
	  }
	}
	else {
	  PrintFormatLog(1, "zip[%i] read error", i);
	fResult = FALSE;
	}
      }
      else {
	PrintFormatLog(1, "zip access error");
	fResult = FALSE;
      }
    }

#if 0

    while (fResult == FALSE) {
      /* Read entries, match up names with regexp. */
      zipEntryPtr pZipEntryT;
      int iError;

      iError = zip_read_next_header((arcPtr)resNodeGetHandleIO(prnArgZip), &pArcEntryT);
      if (iError == ZIP_EOF) {
	PrintFormatLog(4, "End of '%s'", resNodeGetNameNormalized(prnArgZip));
	fResult = TRUE;
      }
      else if (iError == ZIP_OK) {
	char* pcT;
	xmlChar* pucNameEncoded = NULL;

	pcT = (char*)zip_entry_pathname(pArcEntryT);
	if (STR_IS_NOT_EMPTY(pcT) && arcGetFileNameDecoded(pcT, &pucNameEncoded) && STR_IS_NOT_EMPTY(pucNameEncoded)) {
	  resNodePtr prnChild = NULL;
	  time_t mtimeEntry;

	  if (resNodeGetChild(prnArgZip) != NULL && resNodeGetMimeType(prnArgZip) == MIME_APPLICATION_GZIP) { /* */
	    prnChild = resNodeGetChild(prnArgZip);
	  }
	  else if (resNodeGetChild(prnArgZip) != NULL) { /* */
	    prnChild = resNodeListFindPath(resNodeGetChild(prnArgZip), pucNameEncoded, RN_FIND_ALL);
	  }
	  else if (resNodeGetMimeType(prnArgZip) == MIME_APPLICATION_GZIP && resNodeGetNameObject(prnArgZip) != NULL) {
	    xmlFree(pucNameEncoded);
	    pucNameEncoded = xmlStrdup(resNodeGetNameObject(prnArgZip)); /* use parent object name */
	  }
	  else {
	    /* use pucNameEncoded */
	  }
	  PrintFormatLog(4, " %s", pucNameEncoded);

	  /*!\todo exclude hidden entries? */

	  if (prnChild) {
	    /* no match test for existing childrens */
	  }
	  else {
#ifdef HAVE_PCRE2
	    if (re_match) {   /* filename matching */
	      int rc = 1;

	      pcre2_match_data* match_data;

	      match_data = pcre2_match_data_create_from_pattern(re_match, NULL);
	      rc = pcre2_match(
		re_match,        /* result of pcre2_compile() */
		(PCRE2_SPTR8)pucNameEncoded,  /* the subject string */
		strlen((const char*)pucNameEncoded),             /* the length of the subject string */
		0,              /* start at offset 0 in the subject */
		0,              /* default options */
		match_data,        /* vector of integers for substring information */
		NULL);            /* number of elements (NOT size in bytes) */

	      if (rc < 0) {
		//PrintFormatLog(4, "%s ignore '%s'", NAME_FILE, pucNameEncoded);
	      }
	      else {
		prnChild = resNodeAddChildNew(prnArgZip, pucNameEncoded);
	      }
	      pcre2_match_data_free(match_data);   /* Release memory used for the match */
	    }
	    else {
	      prnChild = resNodeAddChildNew(prnArgZip, pucNameEncoded);
	    }
#else
	    prnChild = resNodeAddChildNew(prnArgZip, pucNameEncoded);
#endif
	  }

	  if (prnChild) {


	    if (resNodeGetMimeType(prnArgZip) == MIME_APPLICATION_GZIP) {
	      resNodeSetType(prnChild, rn_type_file_in_zip);
	      resNodeSetType(prnArgZip, rn_type_file_compressed);
	      prnChild->liSize = (long)BUFFER_LENGTH * 1024L; /*\todo handle file size dynamically */
	    }
	    else {
	      prnChild->liSize = (long)zip_entry_size(pArcEntryT);
	      prnChild->tMtime = (time_t)zip_entry_mtime(pArcEntryT);
	    }

	    if (fArgContent && prnChild->liSize > 0) {
	      void* pData;
	      int iSizeWanted = prnChild->liSize;

	      pData = xmlMalloc(iSizeWanted + 1);
	      if (pData) {
		int iSizeGot;

		// https://github.com/libzip/libzip/wiki/Examples
		/* read data of current zip header */
		iSizeGot = zip_read_data((arcPtr)resNodeGetHandleIO(prnArgZip), pData, iSizeWanted);
		if (iSizeGot < 0) {
		  xmlFree(pData);
		  resNodeSetError(prnChild, rn_error_zip, "Zip read error: %s", zip_error_string((arcPtr)resNodeGetHandleIO(prnArgZip)));
		  fResult = FALSE;
		}
		else {
		  if (iSizeGot < iSizeWanted) {
		    pData = xmlRealloc(pData, iSizeGot + 1);
		  }
		  if (resMimeIsPlain(prnChild->eMimeType) || resMimeIsPlain(prnChild->eMimeType)) {
		    xmlChar* pucData = BAD_CAST pData;
		    pucData[iSizeGot] = '\0'; /* null termination of text input */
		  }
		  resNodeSetContentPtr(prnChild, pData, iSizeGot);
	fResult = TRUE;

		  if (resMimeIsZipDocument(prnChild->eMimeType)) {
		    fResult = arcAppendEntries(prnChild, re_match, fArgContent);
		  }
		}
	      }
	    }
	  }
	  else {
	    resNodeSetError(prnArgZip, rn_error_parse, "error no child in arcAppendEntries()\n");
	  }
	}
	xmlFree(pucNameEncoded);
      }
      else {
	resNodeSetError(prnArgZip, rn_error_parse, "%s", zip_error_string((arcPtr)resNodeGetHandleIO(prnArgZip)));
	break;
      }
    }
#endif
  }
  return fResult;
} /* end of zipAppendEntries() */

#ifdef TESTCODE
#include "test/test_res_node_zip.c"
#endif
