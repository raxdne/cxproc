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
        tar
        iso

    <cxp:copy from"test/a.png" to="abc.zip!test-b/b.png"/>
*/


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

#ifdef HAVE_LIBARCHIVE
#include <archive/cxp_archive.h>
#endif


#define CXP_CODING_ZIP "CP437"

iconv_t iconvArcEncode = NULL;     /*! charset of archive headers */

iconv_t iconvArcDecode = NULL;     /*! charset of archive headers */


static void
arcIconvCleanup(void);


/*!
*/
BOOL_T
arcIconvInit(void)
{
  iconvArcEncode = iconv_open(CXP_CODING_ZIP, "UTF-8");
  if (iconvArcEncode == (iconv_t)-1) {
    /* Something went wrong.  */
    PrintFormatLog(1, "Conversion to '" CXP_CODING_ZIP "' to UTF-8 not possible");
  }

  /*
  create the required conversion descriptors
  */
  iconvArcDecode = iconv_open("UTF-8", CXP_CODING_ZIP);
  if (iconvArcDecode == (iconv_t)-1) {
    /* Something went wrong.  */
    PrintFormatLog(1, "Conversion from '" CXP_CODING_ZIP "' to UTF-8 not possible");
  }

  /* register for exit() */
  if (atexit(arcIconvCleanup) != 0) {
    exit(EXIT_FAILURE);
  }

  return TRUE;
}
/* end of arcIconvInit() */


/* module cleanup for libiconv, runs in exit()
*/
void
arcIconvCleanup(void)
{
  if (iconvArcEncode != NULL) {
    iconv_close(iconvArcEncode);
    iconvArcEncode = NULL;
  }
  if (iconvArcDecode != NULL) {
    iconv_close(iconvArcDecode);
    iconvArcDecode = NULL;
  }
}
/* end of arcCleanup() */


/*! \return 0 in case of success
*/
BOOL_T
arcMapReadFormat(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (prnArg != NULL && resNodeGetHandleIO(prnArg) != NULL) {
    int iMimeType;
    int iErr = -1;

    if (archive_read_support_filter_all((arcPtr)resNodeGetHandleIO(prnArg)) != ARCHIVE_OK) {
      resNodeSetError(prnArg, rn_error_archive, "error of archive_read_support_filter_all(): '%s'\n", archive_error_string((arcPtr)resNodeGetHandleIO(prnArg)));
    }

    iMimeType = resNodeGetMimeType(prnArg);
    switch (iMimeType) {

    case MIME_APPLICATION_VND_OASIS_OPENDOCUMENT_TEXT:
    case MIME_APPLICATION_VND_OASIS_OPENDOCUMENT_SPREADSHEET:
    case MIME_APPLICATION_VND_OASIS_OPENDOCUMENT_PRESENTATION:
    case MIME_APPLICATION_VND_SUN_XML_WRITER:
    case MIME_APPLICATION_VND_SUN_XML_CALC:
    case MIME_APPLICATION_VND_SUN_XML_IMPRESS:
    case MIME_APPLICATION_VND_STARDIVISION_WRITER:
    case MIME_APPLICATION_VND_STARDIVISION_CALC:
    case MIME_APPLICATION_VND_STARDIVISION_IMPRESS:
    //case MIME_APPLICATION_MMAP_XML:
    case MIME_APPLICATION_MM_XML:
    case MIME_APPLICATION_XMMAP_XML:
    case MIME_APPLICATION_XMIND_XML:
    case MIME_APPLICATION_XSPF_XML:
    case MIME_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_WORDPROCESSINGML_DOCUMENT:
    case MIME_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_PRESENTATIONML_PRESENTATION:
    case MIME_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_SPREADSHEETML_SHEET:
    case MIME_APPLICATION_ZIP:
      iErr = archive_read_support_format_zip((arcPtr) resNodeGetHandleIO(prnArg));
      break;

    case MIME_APPLICATION_GZIP:
    case MIME_APPLICATION_X_BZIP:
      //iErr = archive_read_support_compression_gzip((arcPtr)resNodeGetHandleIO(prnArg));
      //iErr = archive_read_support_compression_bzip2((arcPtr)resNodeGetHandleIO(prnArg));
      iErr = archive_read_support_format_raw((arcPtr)resNodeGetHandleIO(prnArg));
      break;

    case MIME_APPLICATION_X_TAR:
      iErr = archive_read_support_format_gnutar((arcPtr) resNodeGetHandleIO(prnArg));
      break;

    case MIME_APPLICATION_X_ISO9660_IMAGE:
      iErr = archive_read_support_format_iso9660((arcPtr) resNodeGetHandleIO(prnArg));
      break;

    case MIME_INODE_SYMLINK:
      /*!\todo add link target information */
      break;

    default:
      resNodeSetError(prnArg, rn_error_archive, archive_error_string((arcPtr)resNodeGetHandleIO(prnArg)));
      break;
    }

    fResult = (iErr == ARCHIVE_OK);
  }
  return fResult;
} /* end of arcMapReadFormat() */


/*! Encodes a UTF-8 path string to code page 437 (ZIP default).

\param pucArg pointer to path string to convert
\param ppcResult pointer to result pointer
\param fDecode flag for decode/encode
\return TRUE if encoding or decoding of filename is successful, else FALSE
*/
BOOL_T
arcGetFileNameEncoded(xmlChar *pucArg,char **ppcResult)
{
  BOOL_T fResult = FALSE;

  if (iconvArcEncode == NULL) {
    arcIconvInit();
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

    nconv = iconv(iconvArcEncode, (char **)&pchIn, &int_in, &pchOut, &int_out);
    //cxpCtxtEncError(pccArg,errno,nconv);
    pchOut = mpcOut;
    *ppcResult = (char *)xmlStrdup(BAD_CAST mpcOut);
    fResult = TRUE;
  }

  return fResult;
}
/* end of arcGetFileNameEncoded() */


/*! Decodes a ZIP path string to UTF-8.

\param pcArg pointer to path string to convert
\param ppucResult pointer to result pointer
\param fDecode flag for decode/encode
\return TRUE if encoding or decoding of filename is successful, else FALSE
*/
BOOL_T
arcGetFileNameDecoded(char *pcArg, xmlChar **ppucResult)
{
  BOOL_T fResult = FALSE;

  if (iconvArcDecode == NULL) {
    arcIconvInit();
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

    nconv = iconv(iconvArcDecode, (char **)&pchIn, &int_in, &pchOut, &int_out);
    //cxpCtxtEncError(pccArg,errno,nconv);
    *ppucResult = xmlStrdup(mpucOut);
    fResult = TRUE;
  }

  return fResult;
}
/* end of arcGetFileNameDecoded() */


/*! wrapper for , append all directory entries as childs

similar to resNodeDirAppendEntries()

\param prnArgArchive the archive node
\param re_match pointer to a compiled regexp
\param fArgContent flag to extract data of a matching header
\return TRUE if successful, else FALSE
*/
BOOL_T
arcAppendEntries(resNodePtr prnArgArchive, const pcre2_code* re_match, BOOL_T fArgContent)
{
  BOOL_T fResult = FALSE;

  /*!\bug test permission to this directory first */

  /*!\todo handle archive in archive recursively */

  if (resNodeReadStatus(prnArgArchive) && resNodeIsArchive(prnArgArchive) && resNodeOpen(prnArgArchive, "ra")) {
    PrintFormatLog(4, "Begin of '%s'", resNodeGetNameNormalized(prnArgArchive));
    while (fResult == FALSE) {
      /* Read entries, match up names with regexp. */
      arcEntryPtr pArcEntryT;
      int iError;

      iError = archive_read_next_header((arcPtr)resNodeGetHandleIO(prnArgArchive), &pArcEntryT);
      if (iError == ARCHIVE_EOF) {
	PrintFormatLog(4, "End of '%s'", resNodeGetNameNormalized(prnArgArchive));
	fResult = TRUE;
      }
      else if (iError == ARCHIVE_OK) {
	char* pcT;
	xmlChar* pucNameEncoded = NULL;

	pcT = (char*)archive_entry_pathname(pArcEntryT);
	if (STR_IS_NOT_EMPTY(pcT) && arcGetFileNameDecoded(pcT, &pucNameEncoded) && STR_IS_NOT_EMPTY(pucNameEncoded)) {
	  resNodePtr prnChild = NULL;
	  time_t mtimeEntry;

	  if (resNodeGetChild(prnArgArchive) != NULL && resNodeGetMimeType(prnArgArchive) == MIME_APPLICATION_GZIP) { /* */
	    prnChild = resNodeGetChild(prnArgArchive);
	  }
	  else if (resNodeGetChild(prnArgArchive) != NULL) { /* */
	    prnChild = resNodeListFindPath(resNodeGetChild(prnArgArchive), pucNameEncoded, RN_FIND_ALL);
	  }
	  else if (resNodeGetMimeType(prnArgArchive) == MIME_APPLICATION_GZIP && resNodeGetNameObject(prnArgArchive) != NULL) {
	    xmlFree(pucNameEncoded);
	    pucNameEncoded = xmlStrdup(resNodeGetNameObject(prnArgArchive)); /* use parent object name */
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
		prnChild = resNodeAddChildNew(prnArgArchive, pucNameEncoded);
	      }
	      pcre2_match_data_free(match_data);   /* Release memory used for the match */
	    }
	    else {
	      prnChild = resNodeAddChildNew(prnArgArchive, pucNameEncoded);
	    }
#else
	    prnChild = resNodeAddChildNew(prnArgArchive, pucNameEncoded);
#endif
	  }

	  if (prnChild) {
	    resNodePtr prnAncestor;

	    resNodeSetContentPtr(prnChild, NULL, 0);
	    resNodeResetMimeType(prnChild);
	    prnChild->fExist = TRUE;
	    prnChild->fStat = TRUE;
	    prnChild->fRead = TRUE;
	    prnChild->eAccess = rn_access_archive;

	    /* set all parent directories in this archive too */
	    for (prnAncestor = resNodeGetParent(prnChild);
	      resNodeGetType(prnAncestor) == rn_type_dir_in_archive;
	      prnAncestor = resNodeGetParent(prnAncestor)) {
	      prnAncestor->fExist = TRUE;
	      prnAncestor->fStat = TRUE;
	      prnAncestor->fRead = TRUE;
	      prnAncestor->eAccess = rn_access_archive;
	    }


	    if (resNodeGetMimeType(prnArgArchive) == MIME_APPLICATION_GZIP) {
	      resNodeSetType(prnChild, rn_type_file_in_archive);
	      resNodeSetType(prnArgArchive, rn_type_file_compressed);
	      prnChild->liSize = (long)BUFFER_LENGTH * 1024L; /*\todo handle file size dynamically */
	    }
	    else {
	      prnChild->liSize = (long)archive_entry_size(pArcEntryT);
	      prnChild->tMtime = (time_t)archive_entry_mtime(pArcEntryT);
	    }

	    if (fArgContent && prnChild->liSize > 0) {
	      void* pData;
	      int iSizeWanted = prnChild->liSize;

	      pData = xmlMalloc(iSizeWanted + 1);
	      if (pData) {
		int iSizeGot;

		// https://github.com/libarchive/libarchive/wiki/Examples
		/* read data of current archive header */
		iSizeGot = archive_read_data((arcPtr)resNodeGetHandleIO(prnArgArchive), pData, iSizeWanted);
		if (iSizeGot < 0) {
		  xmlFree(pData);
		  resNodeSetError(prnChild, rn_error_archive, "Archive read error: %s", archive_error_string((arcPtr)resNodeGetHandleIO(prnArgArchive)));
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

		  if (resMimeIsArchive(prnChild->eMimeType)) {
		    fResult = arcAppendEntries(prnChild, re_match, fArgContent);
		  }
		}
	      }
	    }
	  }
	  else {
	    resNodeSetError(prnArgArchive, rn_error_parse, "error no child in arcAppendEntries()\n");
	  }
	}
	xmlFree(pucNameEncoded);
      }
      else {
	resNodeSetError(prnArgArchive, rn_error_parse, "%s", archive_error_string((arcPtr)resNodeGetHandleIO(prnArgArchive)));
	break;
      }
    }
    resNodeClose(prnArgArchive);
  }
  return fResult;
} /* end of arcAppendEntries() */


#ifdef TESTCODE
#include "test/test_archive.c"
#endif
