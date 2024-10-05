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
#include "dom.h"
#include "plain_text.h"
#include "utils.h"


#define CXP_CODING_ZIP "CP437"

iconv_t iconvZipEncode = NULL;     /*! charset of zip headers */

iconv_t iconvZipDecode = NULL;     /*! charset of zip headers */


/*! opens context prnArg

\todo handle in-memory achives for stdout/stdin s. zip_write_open_memory()

\return TRUE if successful
*/
BOOL_T
zipFileOpen(resNodePtr prnArg, const char *pchArgMode)
{
  BOOL_T fResult = FALSE;

  if (prnArg != NULL && prnArg->handleIO != NULL && prnArg->eAccess == rn_access_zip) {
      fResult = TRUE;
      PrintFormatLog(4, "zip_open('%s') already open", resNodeGetNameNormalized(prnArg));
  }
 else if (strchr(pchArgMode,(int)'r') || prnArg->eMode == mode_read) {
    int err;
    char buf[BUFFER_LENGTH];

    if ((prnArg->handleIO = zip_open(resNodeGetNameNormalizedNative(prnArg), 0, &err)) == NULL) {
      zip_error_to_str(buf, sizeof(buf), err, errno);
      PrintFormatLog(1, "can't open zip archive `%s': %s\n", resNodeGetNameNormalizedNative(prnArg), buf);
    }
#if 0
    else if (resNodeIsMemory(prnArg)) { /* achive in memory already */
	  if (zip_read_open_memory((arcPtr)prnArg->handleIO, prnArg->pContent, resNodeGetSize(prnArg)) == ZIP_OK) {
	    prnArg->fExist = TRUE;
	    prnArg->eAccess = rn_access_zip;
	    fResult = TRUE;
	    PrintFormatLog(4, "zip_read_open_memory('%s') OK", resNodeGetNameNormalized(prnArg));
	  }
	  else {
	    zipFileClose(prnArg);
	    resNodeSetError(prnArg, rn_error_zip, "zip_read_open_memory() failed");
	  }
	  /*!\todo read zip from memory buffer ":memory:" zip_read_open_memory() */
     }
#endif
    else {
      prnArg->fExist = TRUE;
      prnArg->eAccess = rn_access_zip;
      fResult = TRUE;
      PrintFormatLog(4, "zip_open('%s') OK", resNodeGetNameNormalized(prnArg));
    }
  }
#if 0
  else if (strchr(pchArgMode,(int)'w') || prnArg->eMode == mode_write) {
    /*\todo append content to existing zip file */

    /*\todo delete existing zip file first */

    }
    else {
      zipFileClose(prnArg);
      resNodeSetError(prnArg, rn_error_zip, "zip_write_new('%s') failed", resNodeGetNameNormalized(prnArg));
    }
  }
#endif
  else {
    resNodeSetError(prnArg, rn_error_open, "unknown mode for zip opening '%s'", resNodeGetNameNormalized(prnArg));
  }

  return fResult;
} /* end of zipFileOpen() */


/*! opens context prnArg

\return TRUE if successful
*/
BOOL_T
zipFileClose(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  assert(prnArg != NULL);

  if (zip_close(resNodeGetHandleIO(prnArg)) == -1) {
    resNodeSetError(prnArg, rn_error_zip, "Error zip_close('%s')", resNodeGetNameNormalized(prnArg));
  }
  else {
    fResult = TRUE;
  }

  prnArg->handleIO = NULL;
  prnArg->eAccess = rn_access_undef;

  return fResult;
} /* end of zipFileClose() */


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
} /* end of zipIconvInit() */


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
} /* end of zipIconvCleanup() */


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


/*! generic extractor for ZIP archives using libzip

similar to arcAppendEntries()

\param prnArgZip the zip node
\param re_match pointer to a compiled regexp
\param fArgContent flag to extract data of a matching header

\return TRUE if successful, else FALSE
*/
BOOL_T
zipAppendEntries(resNodePtr prnArgZip, const pcre2_code *re_match, BOOL_T fArgContent)
{
  BOOL_T fResult = FALSE;

  /*!\bug test permission to this directory first */

  /*!\todo handle zip in zip recursively */

  if (resNodeIsZipDocument(prnArgZip) && resNodeIsOpen(prnArgZip) && resNodeGetChild(prnArgZip)) {
    /* resNode is open/parsed already */
  }
  else if (resNodeReadStatus(prnArgZip) && resNodeGetMimeType(prnArgZip) == MIME_APPLICATION_ZIP && (resNodeIsOpen(prnArgZip) || zipFileOpen(prnArgZip,"r"))) {
    char buf[BUFFER_LENGTH];
    int i, len;

    PrintFormatLog(4, "Begin of '%s'", resNodeGetNameNormalized(prnArgZip));
    for (i = 0; i < zip_get_num_entries((struct zip *)resNodeGetHandleIO(prnArgZip), 0) && i < 1e5; i++) {
      struct zip_stat sb;
      xmlChar *pucNameEncoded = NULL;

      if (zip_stat_index((struct zip *)resNodeGetHandleIO(prnArgZip), i, 0, &sb) == 0 && STR_IS_NOT_EMPTY(sb.name) &&
	  zipGetFileNameDecoded(sb.name, &pucNameEncoded) && STR_IS_NOT_EMPTY(pucNameEncoded)) {
	resNodePtr prnInZip = NULL;
	resNodePtr prnAncestor;
	resNodePtr prnChild = NULL;
	struct zip_file *zf;
	time_t mtimeEntry;

	if (resNodeGetChild(prnArgZip) != NULL) { /* */
	  prnChild = resNodeListFindPath(resNodeGetChild(prnArgZip), pucNameEncoded, RN_FIND_ALL);
	}
	else {
	  /* use pucNameEncoded */
	}
	PrintFormatLog(4, " %s", pucNameEncoded);

	/*!\todo exclude hidden entries? */

#ifdef HAVE_PCRE2
	if (re_match) { /* filename matching */
	  int rc = 1;

	  pcre2_match_data *match_data;

	  match_data = pcre2_match_data_create_from_pattern(re_match, NULL);
	  rc = pcre2_match(re_match,				 /* result of pcre2_compile() */
			   (PCRE2_SPTR8)pucNameEncoded,		 /* the subject string */
			   strlen((const char *)pucNameEncoded), /* the length of the subject string */
			   0,					 /* start at offset 0 in the subject */
			   0,					 /* default options */
			   match_data,				 /* vector of integers for substring information */
			   NULL);				 /* number of elements (NOT size in bytes) */

	  if (rc < 0) {
	    // PrintFormatLog(4, "%s ignore '%s'", NAME_FILE, pucNameEncoded);
	    continue;
	  }
	  else {
	    prnInZip = resNodeAddChildNew(prnArgZip, pucNameEncoded);
	  }
	  pcre2_match_data_free(match_data); /* Release memory used for the match */
	}
	else {
	  prnInZip = resNodeAddChildNew(prnArgZip, pucNameEncoded);
	}
#else
	prnInZip = resNodeAddChildNew(prnArgZip, pucNameEncoded);
#endif

	if (prnInZip) {
	  fResult = TRUE;
	  if (resPathIsDir(pucNameEncoded)) {
	    prnInZip->eType = rn_type_dir_in_zip;
	  }
	  else {
	    resNodeSetSize(prnInZip, sb.size);
	    prnInZip->eType = rn_type_file_in_zip;
	    // sb.mtime
	    resNodeResetMimeType(prnInZip);
	    prnInZip->fExist = TRUE;
	    prnInZip->fStat = TRUE;
	    prnInZip->fRead = TRUE;
	    prnInZip->eAccess = rn_access_zip;

	    if (fArgContent && sb.size < 1e6) {
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
		// assert(resNodeGetSi(prnArgZip) == sum);
	      }
	      else {
		PrintFormatLog(1, "zip[%i] read error", i);
		// fResult = FALSE;
	      }
	    }
	    /* set all parent directories in this zip too */
	    for (prnAncestor = resNodeGetParent(prnInZip); resNodeGetType(prnAncestor) == rn_type_dir_in_zip; prnAncestor = resNodeGetParent(prnAncestor)) {
	      prnAncestor->fExist = TRUE;
	      prnAncestor->fStat = TRUE;
	      prnAncestor->fRead = TRUE;
	      prnAncestor->eAccess = rn_access_zip;
	    }
	  }
	}
	else {
	  PrintFormatLog(1, "zip access error");
	  fResult = FALSE;
	}
      }
    }
  }
  return fResult;
} /* end of zipAppendEntries() */

/*! handler for Document formats based on ZIP archive

similar to zipAppendEntries()

\param prnArgZip the zip node
\param iArgOptions
\return TRUE if successful, else FALSE
*/
BOOL_T
zipDocumentRead(resNodePtr prnArgZip, int iArgOptions)
{
  BOOL_T fResult = FALSE;

  if (prnArgZip->pContent != NULL || prnArgZip->pdocContent != NULL) {
    /* use existing buffer content */
  }
  else if (resNodeReadStatus(prnArgZip) && resNodeIsZipDocument(prnArgZip) && (resNodeIsOpen(prnArgZip) || zipFileOpen(prnArgZip, "r"))) {
    int i, len;
    zip_uint64_t sum;

    PrintFormatLog(4, "Begin of '%s'", resNodeGetNameNormalized(prnArgZip));
    for (i = 0, sum = 0; i < zip_get_num_entries((struct zip *)resNodeGetHandleIO(prnArgZip), 0) || i < 1e5; i++) {
      struct zip_stat sb;

      fResult = TRUE;
      if (zip_stat_index((struct zip *)resNodeGetHandleIO(prnArgZip), i, 0, &sb) == 0) {
	resNodePtr prnInZip;
	struct zip_file *zf;

	prnInZip = resNodeAddChildNew(prnArgZip, BAD_CAST sb.name);
	if (sb.size < 1) {
	  prnInZip->eType = rn_type_dir_in_zip;
	}
	else {
	  prnInZip->eType = rn_type_file_in_zip;
	  resNodeSetSize(prnInZip, sb.size);
	  sum += sb.size;
	}
	prnInZip->tMtime = sb.mtime;
	prnInZip->fExist = TRUE;
	prnInZip->fStat = TRUE;
	prnInZip->fRead = TRUE;
	prnInZip->eAccess = rn_access_zip;

	if (resPathIsEquivalent(BAD_CAST ".rels", resNodeGetNameBase(prnInZip)) &&
	    resNodeGetMimeType(prnArgZip) == MIME_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_WORDPROCESSINGML_DOCUMENT) {
	  resNodeSetMimeType(prnInZip, MIME_TEXT_XML);
	}
	else {
	  resNodeResetMimeType(prnInZip);
	}

	if ((iArgOptions & RN_INFO_CONTENT) == 0 && ((iArgOptions & RN_INFO_XML) && resMimeIsXml(resNodeGetMimeType(prnInZip)) == FALSE)) {
	  PrintFormatLog(3, "skip content zip[%i] '%s'", i, sb.name);
	}
	else if (sb.size > 1e7 || sum > 1e8) {
	  resNodeSetError(prnInZip, rn_error_undef, "skip big zip[%i] '%s'", i, sb.name);
	  PrintFormatLog(3, "skip big zip[%i] '%s'", i, sb.name);
	}
	else if ((zf = zip_fopen_index((struct zip *)resNodeGetHandleIO(prnArgZip), i, 0)) != NULL) {
	  long long sum;
	  char buf[BUFFER_LENGTH];

	  PrintFormatLog(3, "zip[%i] of '%s'", i, sb.name);
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
	}
	else {
	  PrintFormatLog(1, "zip[%i] read error", i);
	  fResult = FALSE;
	}
      }
    }
  }
  return fResult;
} /* end of zipDocumentRead() */


/*!\todo to write resNode to a ZIP document

\param prnArgZip the zip node
\param iArgOptions
\return TRUE if successful, else FALSE
*/
BOOL_T
zipDocumentWrite(resNodePtr prnArgZip, int iArgOptions)
{
  BOOL_T fResult = FALSE;

  /*!\todo to be implemented via libarchive :-( */

  return fResult;
} /* end of zipDocumentWrite() */


#ifdef TESTCODE
#include "test/test_res_node_zip.c"
#endif
