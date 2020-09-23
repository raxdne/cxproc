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

#include <libxml/parser.h>

#include "basics.h"
#include "utils.h"
#include <res_node/res_path.h>

iconv_t iconvFsEncode = NULL;     /*! charset filesystem informations */

iconv_t iconvFsDecode = NULL;     /*! charset filesystem informations */

/*!\bug exclude more characters from path names */
#define ISPATH(CHAR) ( ! (CHAR == (xmlChar)'=' || CHAR == (xmlChar)'?' || CHAR == (xmlChar)'<' || CHAR == (xmlChar)'>' || CHAR == (xmlChar)';'))

/*! */
typedef enum {
  PROTOCOL_NIL,
  PROTOCOL_FILE,
  PROTOCOL_HTTP,
  PROTOCOL_HTTPS,
  PROTOCOL_FTP,
  PROTOCOL_FTPS,
  PROTOCOL_ZIP
} resPathProtocol;

static resPathProtocol
resPathGetProtocol(xmlChar *pucArg);

static int
CopyPath(xmlChar *pucArgTarget, xmlChar *pucArgSource);

static BOOL_T
resPathIconvError(int int_errno, size_t nconv);

/* **********************************************************************************************

   filesystem string functions

 */

/*! Resource Path SetNativeEncoding  

\param *pcArg -- pointer to string of native filesystem encoding
\return FALSE in case of error
*/
BOOL_T
resPathSetNativeEncoding(const char *pcArg)
{
  BOOL_T fResult = FALSE;

  if (STR_IS_NOT_EMPTY(pcArg)) {
    int i;

#ifdef _MSC_VER
    i = stricmp(pcArg, "UTF-8");
#else
    i = strcasecmp(pcArg, "UTF-8");
#endif
    if (i) { /* avoid 1:1 conversion */
      /*
      create the required conversion descriptors
       */
      iconvFsEncode = iconv_open("UTF-8", pcArg);

      if (iconvFsEncode == (iconv_t) - 1) {
        if (errno == EINVAL) { /* Something went wrong.  */
	  PrintFormatLog(1, "Conversion from '%s' to 'UTF-8' not possible", pcArg);
	  return fResult;
	}
      }
      else {
        PrintFormatLog(2, "Filesystem conversion '%s' to '%s'", pcArg, "UTF-8");
        iconv(iconvFsEncode, NULL, NULL, NULL, NULL);
      }

      iconvFsDecode = iconv_open(pcArg, "UTF-8");

      if (iconvFsDecode == (iconv_t) - 1) {
        if (errno == EINVAL) { /* Something went wrong.  */
	  PrintFormatLog(1, "Conversion to '%s' to 'UTF-8' not possible", pcArg);
	  return fResult;
	}
      }
      else {
        PrintFormatLog(2, "Filesystem conversion '%s' to '%s'", "UTF-8", pcArg);
        iconv(iconvFsDecode, NULL, NULL, NULL, NULL);
      }
      
      fResult = (iconvFsEncode != NULL && iconvFsDecode != NULL);

      fResult &= (atexit(resPathCleanup) != 0);
    }
    else {
      PrintFormatLog(2, "No Filesystem conversion necessary");
      iconvFsEncode = NULL;
      iconvFsDecode = NULL;
    }
  }

  return fResult;
} /* end of resPathSetNativeEncoding() */


/* module cleanup for libiconv, runs in exit()
 */
void
resPathCleanup(void)
{
  if (iconvFsEncode != NULL) {
    iconv_close(iconvFsEncode);
    iconvFsEncode = NULL;
  }
  if (iconvFsDecode != NULL) {
    iconv_close(iconvFsDecode);
    iconvFsDecode = NULL;
  }
}
/* end of resPathCleanup() */


/*! Resource Path Encode  

\param char *pchArg -- pointer to zero terminated string of path
\return pointer to a new UTF encoded string of 'pucArg' or NULL in case of error
*/
xmlChar*
resPathEncode(const char* pchArg)
{
  xmlChar* pucResult = NULL;
  int iLength = 0;

  if (pchArg != NULL && (iLength = strlen(pchArg)) > 0) {
    if (iconvFsEncode == NULL) {
      pucResult = xmlStrdup(BAD_CAST pchArg); /* no encoding required */
    }
    else {
      char* pchOut;
      char* pchIn;
      size_t int_in;
      size_t int_out;
      size_t nconv;

      int_in = iLength + 1;
      pchIn = (char*)pchArg;

      int_out = iLength * 2;
      pucResult = BAD_CAST xmlMalloc(int_out * sizeof(xmlChar));
      pchOut = (char*) pucResult;

      /* https://www.gnu.org/software/libc/manual/html_node/iconv-Examples.html */
      nconv = iconv(iconvFsEncode, (char**)&pchIn, &int_in, &pchOut, &int_out);
      if (nconv == (size_t)-1 || resPathIconvError(errno, nconv)) {
	xmlChar* pucT;

	xmlFree(pucResult);
	pucResult = xmlStrdup(BAD_CAST pchArg);
	for (pucT = pucResult; pucT != NULL && !isend(*pucT); pucT++) {
	  if (isascii(*pucT)) {
	    /* OK */
	  }
	  else {
	    *pucT = '_';
	  }
	}

	if (pucT) {
	  *pucT = (xmlChar)'\0';
	}
	PrintFormatLog(1, "Use dummy name '%s'", pucResult);
      }
    }
  }

  return pucResult;
} /* end of resPathEncode() */


/*! Resource Path Decode  

\param *pucArg -- pointer to zero terminated string of path in UTF-8 encoding
\return pointer to a new native encoded string of 'pucArg' or NULL in case of error
*/
char *
resPathDecode(xmlChar *pucArg)
{
  char* pchResult = NULL;
  int iLength = 0;

  if (pucArg != NULL && (iLength = xmlStrlen(pucArg)) > 0) {
    if (iconvFsDecode == NULL) {
      pchResult = (char*)xmlStrdup(pucArg);
    }
    else {
      char* pchOut;
      char* pchIn;
      size_t int_in;
      size_t int_out;
      size_t nconv;

      int_in = iLength + 1;
      pchIn = (char*)pucArg;

      int_out = iLength * 2 - 1;
      pchOut = (char*)xmlMalloc(int_out * sizeof(char));
      pchResult = pchOut;

      /* https://www.gnu.org/software/libc/manual/html_node/iconv-Examples.html */
      nconv = iconv(iconvFsDecode, (char**)&pchIn, &int_in, &pchOut, &int_out);
      if (nconv == (size_t)-1 || resPathIconvError(errno, nconv)) {
	char* pchT;

	//xmlFree(pchOut);
	pchResult = (char *)xmlStrdup(BAD_CAST pucArg);
	for (pchT = pchResult; pchT != NULL && !isend(*pchT); pchT++) {
	  if (isascii(*pchT)) {
	    /* OK */
	  }
	  else {
	    *pchT = '_';
	  }
	}

	if (pchT) {
	  *pchT = '\0';
	}
	PrintFormatLog(1, "Use dummy name '%s'", pchResult);
      }
    }
  }

  return pchResult;
} /* end of resPathDecode() */


/*! Resource Path print iconv error messages

\param int_errno
\param nconv

\return TRUE if
*/
BOOL_T
resPathIconvError(int int_errno, size_t nconv)
{
  BOOL_T fResult = FALSE;

  if (int_errno==EILSEQ) {
    PrintFormatLog(1, "invalid byte sequence in the input");
    fResult = TRUE;
  }
  else if (int_errno==E2BIG) {
    PrintFormatLog(1, "it ran out of space in the output buffer.");
    fResult = TRUE;
  }
  else if (int_errno==EINVAL) {
    PrintFormatLog(1, "an incomplete byte sequence at the end of the input buffer.");
    fResult = TRUE;
  }
  else if (int_errno==EBADF) {
    PrintFormatLog(1, "The cd argument is invalid.");
    fResult = TRUE;
  }
  return fResult;
} /* end of resPathIconvError() */


/*! \return TRUE if pucArgPath starts with pucArgNameFile terminated by '\0' or path separator

\param pucArgPath
\param pucArgNameFile

\todo case insensitive
\todo all allowed UTF-8 characters
*/
BOOL_T
resPathIsMatchingBegin(xmlChar *pucArgPath, xmlChar *pucArgNameFile)
{
  BOOL_T fResult = FALSE;

  if (STR_IS_NOT_EMPTY(pucArgPath) && STR_IS_NOT_EMPTY(pucArgNameFile)) {

    for ( ; ; pucArgPath++, pucArgNameFile++) {

      if (issep(*pucArgPath) && issep(*pucArgNameFile)) {
	/* skip multiple separators */
	while (issep(*pucArgPath))
	  pucArgPath++;

	while (issep(*pucArgNameFile))
	  pucArgNameFile++;

	if (isend(*pucArgNameFile)) {
	  /* end of pucArgNameFile reached */
	  fResult = TRUE;
	  break;
	}
      }

      if (isend(*pucArgPath) || isend(*pucArgNameFile)) {
	/* begin of pucArgNameFile reached */
	fResult = (isend(*pucArgNameFile) && (isend(*pucArgPath) || issep(*pucArgPath)));
	break;
      }

      if (*pucArgNameFile != *pucArgPath) {
	break;
      }
      else {
	assert(*pucArgNameFile == *pucArgPath);
      }
    }
  }
  return fResult;
}
/* end of resPathIsMatchingBegin() */


/*! \return TRUE if pucArgPath ends with pucArgNameFile terminated by '\0' or path separator

\param pucArgPath
\param pucArgNameFile

\todo case insensitive
  \todo all allowed UTF-8 characters
*/
BOOL_T
resPathIsMatchingEnd(xmlChar *pucArgPath, xmlChar *pucArgNameFile)
{
  BOOL_T fResult = FALSE;

  if (STR_IS_NOT_EMPTY(pucArgPath) && STR_IS_NOT_EMPTY(pucArgNameFile)) {
    int l;
    xmlChar *pucEndPath;
    xmlChar *pucEndNameFile;

    /* skip trailing separators in pucArgNameFile */
    for (l = xmlStrlen(pucArgNameFile); l > 0 && issep(pucArgNameFile[l-1]); l--) ;

    for (pucEndPath = pucArgPath + xmlStrlen(pucArgPath) - 1, pucEndNameFile = pucArgNameFile + l - 1; ; pucEndPath--, pucEndNameFile--) {

      if (issep(*pucEndNameFile) && issep(*pucEndPath)) {
	/* skip multiple separators */
	while (issep(*pucEndNameFile) && pucEndNameFile > pucArgNameFile)
	  pucEndNameFile--;

	while (issep(*pucEndPath) && pucEndPath > pucArgPath)
	  pucEndPath--;

	if (pucEndPath == pucArgPath || pucEndNameFile == pucArgNameFile) {
	  /* begin of pucArgNameFile reached */
	  fResult = TRUE;
	  break;
	}
      }

      if (*pucEndNameFile != *pucEndPath) {
	break;
      }

      if (pucEndPath == pucArgPath || pucEndNameFile == pucArgNameFile) {
	/* begin of pucArgNameFile reached */
	fResult = (pucEndNameFile == pucArgNameFile && (pucEndPath == pucArgPath || issep(*(pucEndPath - 1))));
	break;
      }
      else {
	assert(*pucEndNameFile == *pucEndPath);
      }
    }
  }
  return fResult;
}
/* end of resPathIsMatchingEnd() */


/*! \return TRUE if pucArgPathDescendant is a descendant of pucArgPath ("pucArgPathDescendant starts with pucArgPath")

\param pucArgPath
\param pucArgPathDescendant
*/
xmlChar *
resPathDiffPtr(xmlChar *pucArgPath, xmlChar *pucArgPathDescendant)
{
  if (STR_IS_NOT_EMPTY(pucArgPath) && STR_IS_NOT_EMPTY(pucArgPathDescendant)) {
    for ( ; ; pucArgPath++, pucArgPathDescendant++) {
      int iDeltaUpperLower = ('a' - 'A');

      if (issep(pucArgPath[0]) && isend(pucArgPath[1])) {
	/* pucArgPath ends with a trailing separator */
	while (issep(*pucArgPathDescendant)) pucArgPathDescendant++;
	return pucArgPathDescendant;
      }
      else if (isend(pucArgPath[0])) {
	/* pucArgPath ends without a trailing separator */
	while (issep(*pucArgPathDescendant)) pucArgPathDescendant++;
	return pucArgPathDescendant;
      }
      else if ((issep(pucArgPathDescendant[0]) && isend(pucArgPathDescendant[1])) || isend(pucArgPathDescendant[0])) {
	if (issep(pucArgPath[0]) && isend(pucArgPath[1])) {
	  return pucArgPathDescendant;
	}
	else {
	  break;
	}
      }
      else if (issep(pucArgPath[0]) && issep(pucArgPathDescendant[0])) {
	// both
      }
#ifdef _MSC_VER
      else if (*pucArgPathDescendant != *pucArgPath
	  && (*pucArgPathDescendant - *pucArgPath) != iDeltaUpperLower
	  && (*pucArgPath - *pucArgPathDescendant) != iDeltaUpperLower) {
	break;
      }
#else
      else if (*pucArgPathDescendant != *pucArgPath) {
	break;
      }
#endif
      else {
	// chars are equal
      }
    }
  }
  return NULL;
}
/* end of resPathDiffPtr() */


/*! \return TRUE if pucArgPathDescendant is a descendant of pucArgPath ("pucArgPathDescendant starts with pucArgPath")

\param pucArgPath
\param pucArgPathDescendant
*/
BOOL_T
resPathIsDescendant(xmlChar *pucArgPath, xmlChar *pucArgPathDescendant)
{
  if (STR_IS_NOT_EMPTY(pucArgPath) && STR_IS_NOT_EMPTY(pucArgPathDescendant)) {
    return (resPathDiffPtr(pucArgPath, pucArgPathDescendant) != NULL);
  }
  return FALSE;
}
/* end of resPathIsDescendant() */


/*! \return TRUE if pucArgA and pucArgB results in the same target
 pucArgA and pucArgB must be normalized, use resPathCollapse() before
*/
BOOL_T
resPathIsEquivalent(xmlChar *pucArgA, xmlChar *pucArgB)
{
  BOOL_T fResult = FALSE;

  if (pucArgA == NULL || isend(*pucArgA) || pucArgB == NULL || isend(*pucArgB)) {
  }
  else {
    xmlChar *pucA = pucArgA;
    xmlChar *pucB = pucArgB;
    resPathProtocol eProtocolA;
    resPathProtocol eProtocolB;

    eProtocolA = resPathGetProtocol(pucA); /* detect protocol prefix */
    if (eProtocolA == PROTOCOL_FILE) { /* file access */
      assert(xmlStrlen(pucA) > 7);
      /* cut "file://" prefix */
#ifdef _MSC_VER
      for (pucA += 5; issep(*pucA); pucA++); /* skip separators */
#else
      for (pucA += 5; issep(pucA[1]); pucA++); /* skip separators */
#endif
    }

    eProtocolB = resPathGetProtocol(pucB); /* detect protocol prefix */
    if (eProtocolB == PROTOCOL_FILE) { /* file access */
      assert(xmlStrlen(pucB) > 7);
      /* cut "file://" prefix */
#ifdef _MSC_VER
      for (pucB += 5; issep(*pucB); pucB++); /* skip separators */
#else
      for (pucB += 5; issep(pucB[1]); pucB++); /* skip separators */
#endif
    }

    if ((eProtocolA == PROTOCOL_FILE && eProtocolB == PROTOCOL_NIL) || (eProtocolB == PROTOCOL_FILE && eProtocolA == PROTOCOL_NIL)) {
    }
    else if (eProtocolA != eProtocolB) {
      return fResult;
    }

    if (resPathIsDosDrive(pucA)) {
      /* leading DOS drive letter */

      if (resPathIsDosDrive(pucB)) {
#ifdef _MSC_VER
	if (pucA[0] != pucB[0] && (pucA[0] - pucB[0]) != ('a' - 'A')) {
	  /* leading DOS drive letter too, but different one */
	  fResult = FALSE;
	}
#else
	pucA += 2;
	pucB += 2; /* ignoring on non-Windows OS */
#endif
      }
      else {
	pucA += 2;
      }
    }
    else if (resPathIsDosDrive(pucB)) {
      /* leading DOS drive letter */
      pucB += 2;
    }

    while (TRUE) {
      if (issep(*pucA) && issep(*pucB)) {
	while (issep(*pucA)) pucA++;
	while (issep(*pucB)) pucB++;
      }
      else if (! isend(*pucA)
	  && (*pucA == *pucB
#ifdef _MSC_VER
	      || (*pucA - *pucB) == ('a' - 'A') /* case ignoring */
#endif
	      || (issep(*pucA) && issep(*pucB)))
      ) {
	/* OK */
	pucA++;
	pucB++;
      }
      else if (((isend(pucA[0])) || ((issep(pucA[0]) && isend(pucA[1]))	|| ((issep(pucA[0]) && isdot(pucA[1]) && isend(pucA[2])))))
	  &&
	  ((isend(pucB[0])) || ((issep(pucB[0]) && isend(pucB[1])) || ((issep(pucB[0]) && isdot(pucB[1]) && isend(pucB[2])))))
      ) {
	/* both path ends are of '\0' OR '\\\0' OR '\\.\0' */
	fResult = TRUE;
	break;
      }
      else {
	fResult = FALSE;
	break;
      }
    }
  }
  return fResult;
}
/* end of resPathIsEquivalent() */


/*! \return TRUE if pucArg starts as a relative path
*/
BOOL_T
resPathIsRelative(xmlChar *pucArg)
{
  BOOL_T fResult = TRUE;

  if (pucArg) {
    xmlChar *pucT;

    pucT = isquot(*pucArg) ? pucArg + 1 : pucArg; /* skip first quote */
    if (pucT) {
      if (isend(pucT[0]) || isquot(pucT[0]) || (isdot(pucT[0]) && (isend(pucT[1]) || isquot(pucT[1])))) {
	/* empty string */
      }
      else if (resPathIsStd(pucT)) {
	/* stdout or stdin */
	fResult = FALSE;
      }
      else if (resPathIsURL(pucT)) {
	/* URL */
	fResult = FALSE;
      }
      else if (resPathIsDosDrive(pucT) || resPathIsUNC(pucT)) {
	/* starts with a DOS drive letter and path separator */
	fResult = FALSE;
      }
      else if (issep(pucT[0])) {
	/* starts with path separator */
	fResult = FALSE;
      }
    }
  }
  return fResult;
}
/* end of resPathIsRelative() */


/*! \return TRUE if pucArg starts as with a single path separator
*/
BOOL_T
resPathIsLeadingSeparator(xmlChar *pucArg)
{
  return (pucArg != NULL
    &&
    ((issep(pucArg[0]) && ! isseporend(pucArg[1]))
    ||
    (isquot(pucArg[0]) && issep(pucArg[1]) && !isseporend(pucArg[2]))));
}
/* end of resPathIsLeadingSeparator() */


/*! \return TRUE if pucArg ends as with a path separator OR path separator and dot
*/
BOOL_T
resPathIsTrailingSeparator(xmlChar *pucArg)
{
  BOOL_T fResult = FALSE;

  if (pucArg != NULL) {
    int i;

    i = xmlStrlen(pucArg);
    if (fResult == FALSE && i > 2) {
      fResult = issep(pucArg[i-3]) && isdot(pucArg[i-2]) && isquot(pucArg[i-1]);
    }
    if (fResult == FALSE && i > 1) {
      fResult = (issep(pucArg[i-2]) && isquot(pucArg[i-1])) || (issep(pucArg[i-2]) && isdot(pucArg[i-1]));
    }
    if (fResult == FALSE && i > 0) {
      fResult = issep(pucArg[i-1]);
    }
  }
  return fResult;
}
/* end of resPathIsTrailingSeparator() */


/*! \return TRUE if pucArg starts as with a Windows UNC name
*/
BOOL_T
resPathIsUNC(xmlChar *pucArg)
{
  return (pucArg != NULL
    &&
    ((issep(pucArg[0]) && issep(pucArg[1]) && ! isseporend(pucArg[2]))
    ||
    (isquot(pucArg[0]) && (issep(pucArg[1]) && issep(pucArg[2]) && ! isseporend(pucArg[3])))));
}
/* end of resPathIsUNC() */


/*! \return TRUE if pucArg starts as with a DOS Drive name
*/
BOOL_T
resPathIsDosDrive(xmlChar *pucArg)
{
  return (pucArg != NULL
    &&
      (((isalpha(pucArg[0]) && pucArg[1] == (xmlChar)':' && issep(pucArg[2])))
       ||
        (isquot(pucArg[0]) && isalpha(pucArg[1]) && pucArg[2] == (xmlChar)':' && issep(pucArg[3])))
    );
}
/* end of resPathIsDosDrive() */


/*! \return TRUE if pucArg is name of a external URL (not 'file://' !!)
*/
BOOL_T
resPathIsHttpURL(xmlChar *pucArg)
{
  resPathProtocol pT = PROTOCOL_NIL;
  
  pT = resPathGetProtocol(pucArg);
  
  return (pT == PROTOCOL_HTTP || pT == PROTOCOL_HTTPS);
}
/* end of resPathIsHttpURL() */


/*! \return TRUE if pucArg is name of a external URL (only 'file://' !!)
*/
BOOL_T
resPathIsFileURL(xmlChar *pucArg)
{
  resPathProtocol pT = PROTOCOL_NIL;

  pT = resPathGetProtocol(pucArg);

  return (pT == PROTOCOL_FILE);
}
/* end of resPathIsFileURL() */


/*! \return TRUE if pucArg is name of a external URL (not 'file://' !!)
*/
BOOL_T
resPathIsFtpURL(xmlChar *pucArg)
{
  resPathProtocol pT = PROTOCOL_NIL;
  
  pT = resPathGetProtocol(pucArg);
  
  return (pT == PROTOCOL_FTP || pT == PROTOCOL_FTPS);
}
/* end of resPathIsFtpURL() */


/*! \return TRUE if pucArg is name of a external URL (not 'file://' !!)
*/
BOOL_T
resPathIsURL(xmlChar *pucArg)
{
  resPathProtocol pT = PROTOCOL_NIL;
  
  pT = resPathGetProtocol(pucArg);
  
  return (pT == PROTOCOL_HTTP || pT == PROTOCOL_HTTPS || pT == PROTOCOL_FTP || pT == PROTOCOL_FTPS || pT == PROTOCOL_FILE);
}
/* end of resPathIsURL() */


/*! \return TRUE if pucArg is name of an archive

\todo add MIME types
*/
BOOL_T
resPathBeginIsArchive(xmlChar *pucArg)
{
  BOOL_T fResult = FALSE;

  if (pucArg) {
    xmlChar *pucT;
    xmlChar *pucDot;

    for (pucDot=NULL, pucT=pucArg; pucT; pucT++) {
      if (isseporend(*pucT) || isquot(*pucT)) {
	if (pucDot != NULL && pucT - pucDot > 4) {
	  fResult =
	       (*(pucT - 4) == (xmlChar)'d' && *(pucT - 3) == (xmlChar)'o' && *(pucT - 2) == (xmlChar)'c' && *(pucT - 1) == (xmlChar)'x')
	    || (*(pucT - 4) == (xmlChar)'p' && *(pucT - 3) == (xmlChar)'p' && *(pucT - 2) == (xmlChar)'t' && *(pucT - 1) == (xmlChar)'x')
	    || (*(pucT - 4) == (xmlChar)'x' && *(pucT - 3) == (xmlChar)'l' && *(pucT - 2) == (xmlChar)'s' && *(pucT - 1) == (xmlChar)'x')
	    ;
	  assert(fResult == FALSE || isdot(*(pucT - 5)));
	  break;
	}
	else if (pucDot != NULL && pucT - pucDot > 3) {
	  fResult =
	       (*(pucT - 3) == (xmlChar)'z' && *(pucT - 2) == (xmlChar)'i' && *(pucT - 1) == (xmlChar)'p')
	    || (*(pucT - 3) == (xmlChar)'t' && *(pucT - 2) == (xmlChar)'a' && *(pucT - 1) == (xmlChar)'r')
	    || (*(pucT - 3) == (xmlChar)'i' && *(pucT - 2) == (xmlChar)'s' && *(pucT - 1) == (xmlChar)'o')
	    || (*(pucT - 3) == (xmlChar)'w' && *(pucT - 2) == (xmlChar)'i' && *(pucT - 1) == (xmlChar)'m')
	    || (*(pucT - 3) == (xmlChar)'o' && *(pucT - 2) == (xmlChar)'d' && *(pucT - 1) == (xmlChar)'t')
	    || (*(pucT - 3) == (xmlChar)'o' && *(pucT - 2) == (xmlChar)'d' && *(pucT - 1) == (xmlChar)'s')
	    || (*(pucT - 3) == (xmlChar)'o' && *(pucT - 2) == (xmlChar)'d' && *(pucT - 1) == (xmlChar)'p')
	    ;
	  assert(fResult == FALSE || isdot(*(pucT - 4)));
	  break;
	}
      }
      else if (isdot(*pucT) && pucT > pucArg) {
	pucDot = pucT;
      }

      if (isseporend(*pucT)) {
	break;
      }
    }
  }
  return fResult;
}
/* end of resPathBeginIsArchive() */


/*! \return TRUE if pucArg is name of an compressed file

\todo add MIME types
*/
BOOL_T
resPathBeginIsFileCompressed(xmlChar *pucArg)
{
  BOOL_T fResult = FALSE;

  if (pucArg) {
    xmlChar *pucT;
    xmlChar *pucDot;

    for (pucDot=NULL, pucT=pucArg; pucT; pucT++) {
      if (isseporend(*pucT) || isquot(*pucT)) {
      if (pucDot != NULL && pucT - pucDot > 3) {
	fResult =
	  (*(pucT - 3) == (xmlChar)'b' && *(pucT - 2) == (xmlChar)'z' && *(pucT - 1) == (xmlChar)'2')
	  ;
	assert(fResult == FALSE || isdot(*(pucT - 4)));
	break;
      }
      else if (pucDot != NULL && pucT - pucDot > 2) {
	  fResult =
	    (*(pucT - 2) == (xmlChar)'g' && *(pucT - 1) == (xmlChar)'z')
	    || (*(pucT - 2) == (xmlChar)'b' && *(pucT - 1) == (xmlChar)'z')
	    ;
	  assert(fResult == FALSE || isdot(*(pucT - 3)));
	  break;
	}
      }
      else if (isdot(*pucT) && pucT > pucArg) {
	pucDot = pucT;
      }

      if (isseporend(*pucT)) {
	break;
      }
    }
  }
  return fResult;
}
/* end of resPathBeginIsFileCompressed() */


/*! \return TRUE if pucArg ends with path separator -> is name of a directory, but not if exists
*/
BOOL_T
resPathIsDir(xmlChar *pucArg)
{
  BOOL_T fResult = FALSE;

  if (pucArg) {
    int iLength = xmlStrlen(pucArg);

    if ((iLength > 0 && issep(pucArg[iLength - 1]))
      ||
      (iLength > 1 && isquot(pucArg[iLength - 1]) && issep(pucArg[iLength - 2]))) {
      fResult = TRUE;
    }
  }
  return fResult;
}
/* end of resPathIsDir() */


/*! \return TRUE if pucArg ends with two path separators
*/
BOOL_T
resPathIsDirRecursive(xmlChar *pucArg)
{
  BOOL_T fResult = FALSE;

  if (pucArg) {
    int iLength = xmlStrlen(pucArg);

    if ((iLength > 2 && issep(pucArg[iLength-1]) && issep(pucArg[iLength-2]))
      ||
      (iLength > 3 && isquot(pucArg[iLength - 1]) && issep(pucArg[iLength - 2]) && issep(pucArg[iLength - 3]))) {
      fResult = TRUE;
    }
  }
  return fResult;
}
/* end of resPathIsDirRecursive() */


/*! \return TRUE if pucArg is "-"
*/
BOOL_T
resPathIsStd(xmlChar *pucArg)
{
  return (pucArg != NULL && pucArg[0] == (xmlChar)'-' && isend(pucArg[1]));
}
/* end of resPathIsStd() */


/*! \return TRUE if pucArg is ":memory:"
*/
BOOL_T
resPathIsInMemory(xmlChar *pucArg)
{
  return (STR_IS_NOT_EMPTY(pucArg) && xmlStrEqual(pucArg,BAD_CAST ":memory:") == 1);
}
/* end of resPathIsInMemory() */


/*! \return TRUE if pucArg is the root of a filesystem

\todo detect UNC paths?
*/
BOOL_T
resPathIsRoot(xmlChar *pucArg)
{
  return (pucArg != NULL
	  &&
	  ((issep(pucArg[0]) && isend(pucArg[1]))
	   ||
	   (isquot(pucArg[0]) && issep(pucArg[1]) && isquot(pucArg[2]) && isend(pucArg[3]))
	   ||
	   (resPathIsDosDrive(pucArg) && isend(pucArg[3]))
	   ||
	   (isquot(pucArg[0]) && resPathIsDosDrive(pucArg + 1) && isquot(pucArg[4]) && isend(pucArg[5]))
	   )
	  );
}
/* end of resPathIsRoot() */


/*! \return a dynamically allocated string containing the lowercase extension of pucArg or NULL

 A sequence of digits at the end of a filename is no extension
 (version stamps). Problem with man page filenames.

 */
xmlChar *
resPathGetExtension(xmlChar *pucArg)
{
  xmlChar *pucT, *pucResult, *pucTail;
  int diResult;

  if (STR_IS_EMPTY(pucArg)) {
    return NULL;
  }

  for (pucT=pucArg, pucResult=NULL;
       (pucT = BAD_CAST xmlStrchr(pucT,(xmlChar)'.'));
       pucT++, pucResult=pucT) {

    /* check if the tail consists of numerical chars only */
    for (pucTail = pucT+1; pucTail != NULL && isdigit(*pucTail); pucTail++) { /* skip all digits */ }

    if (isend(*pucTail)) {
      /* case (4) */
      if (pucResult==NULL) {
	pucT++;
	diResult = pucTail - pucT;
	if (pucTail - pucArg > 1 && diResult > 0 && diResult < FS_LENGTH_EXT) {
	  return xmlStrdup(pucTail);
	}
      }
      else {
	diResult = pucT - pucResult;
	if (pucResult - pucArg > 1 && diResult > 0 && diResult < FS_LENGTH_EXT) {
	  return xmlStrndup(pucResult,diResult);
	}
      }
      return NULL;
    }
  }

   /*
    (1) simple filename without extension:

    [a|b|c|\0]
    NULL <- pucT
    NULL <- pucResult

    (2) simple filename with extension:

    [a|b|c|.|d|e|f|\0]
             ^
             |
           pucResult
	     |
           pucTail
    NULL <- pucT

    (3) simple filename with dot char inside and extension:

    [a|b|c|.|d|e|f|.|h|i|j|\0]
                     ^
                     |
                   pucResult
		     |
                   pucTail
    NULL <- pucT

    (4) filename with extension and numeric version:

    [d|e|f|.|1|2|\0]
             ^   ^
             |   |
           pucT  |
               pucTail
    NULL <- pucResult

    [a|b|c|.|d|e|f|.|1|2|\0]
             ^     ^     ^
        pucResult  |     |
	         pucT    |
                       pucTail
   */

  if (pucResult != NULL && pucResult - pucArg > 1 && !isend(*pucResult)) {
    diResult = xmlStrlen(pucResult);
    if (diResult > 0 && diResult < FS_LENGTH_EXT) {
      /* case (2) */
      /* case (3) */
      xmlChar *pucTResult = xmlStrdup(pucResult);

      for (pucT = pucTResult; pucT != NULL && !isend(*pucT); pucT++) {
	if (isalpha(*pucT)) {
	  *pucT = (xmlChar)tolower(*pucT);
	}
      }
      return pucTResult;
    }
  }
  /* case (1) */
  return NULL;
}
/* end of resPathGetExtension() */


/*! \return a dynamically allocated string with the whole directory path of the given filename
*/
xmlChar *
resPathGetBasedir(xmlChar *pucArgNameFile)
{
  xmlChar *pucSep;
  xmlChar *ptr;
  xmlChar *pucS;
  xmlChar *pucEnd;
  xmlChar *pucPath = pucArgNameFile;

  if (STR_IS_EMPTY(pucArgNameFile)) {
    return NULL;
  }

  for (pucEnd = pucArgNameFile + xmlStrlen(pucArgNameFile) - 1;
       pucEnd > pucArgNameFile && (issep(*pucEnd) || isdot(*pucEnd));
       pucEnd--
       ) {
    /* ignore trailing separators */
  }

  if (pucEnd == pucArgNameFile) {
    return NULL;
  }

  if (resPathIsDir(pucArgNameFile)) {
    return xmlStrndup(pucArgNameFile, pucEnd - pucArgNameFile + 1);
  }

  for (ptr=BAD_CAST pucArgNameFile, pucSep=NULL;
       ptr!=NULL
       && !isend(*ptr)
	 && ((pucS=BAD_CAST xmlStrchr(BAD_CAST ptr,(xmlChar)'/'))!=NULL
	     || (pucS=BAD_CAST xmlStrchr(BAD_CAST ptr,(xmlChar)'\\'))!=NULL)
	 && pucS < pucEnd;
       ptr=pucS, pucSep=ptr, ptr++) {
    /* search for the last path separator */
  }

  if (pucSep != NULL && pucEnd - pucSep > 0) {
    if (pucSep - pucPath == 0 && issep(*pucPath)) {
      /* the root */
      return xmlStrdup(BAD_CAST"/");
    }
    else if (pucSep - pucPath > 0 || ! isdot(*pucPath)) {
      /* there is a directory path on argv[1] */
      return xmlStrndup(pucPath, pucSep - pucPath);
    }
  }

  return NULL;
}
/* end of resPathGetBasedir() */


/*! \return a dynamically allocated string with the basename of the given filename
*/
xmlChar *
resPathGetBasename(xmlChar *pucArgNameFile)
{
  xmlChar *pucSep;
  xmlChar *ptr;
  xmlChar *pucS;
  xmlChar *pucEnd;
  int l;

  if (STR_IS_EMPTY(pucArgNameFile)) {
    return NULL;
  }

  if (resPathIsDir(pucArgNameFile)) {
    return NULL;
  }

  /* ignore trailing separators */
  l = xmlStrlen(pucArgNameFile);
  for (pucEnd = BAD_CAST &pucArgNameFile[l-1];
       pucEnd > pucArgNameFile && (issep(*pucEnd) || isdot(*pucEnd));
       pucEnd--
       ) {}

  if (pucEnd == pucArgNameFile) {
    return NULL;
  }

  for (ptr=pucArgNameFile, pucSep=NULL;
       ptr!=NULL
       && !isend(*ptr)
	 && ((pucS=BAD_CAST xmlStrchr(BAD_CAST ptr,(xmlChar)'/'))!=NULL
	     || (pucS=BAD_CAST xmlStrchr(BAD_CAST ptr,(xmlChar)'\\'))!=NULL)
	 && pucS < pucEnd;
       ptr=pucS, pucSep=ptr, ptr++) {
    /* search for the last path separator */
  }

  if (pucSep == NULL) {
    /* there is basename only */
    return xmlStrdup(pucArgNameFile);
  }
  else if (pucSep != NULL && pucEnd - pucSep > 0) {
    /* there is a basename */
    return xmlStrndup(pucSep + 1, pucEnd - pucSep);
  }
  return NULL;
}
/* end of resPathGetBasename() */


/*! \return a dynamically allocated string with the rootname of the given filename
*/
xmlChar *
resPathGetRootname(xmlChar *pucArgNameFile)
{
  xmlChar *pucResult = NULL;

  if (STR_IS_NOT_EMPTY(pucArgNameFile)) {
    xmlChar *pucRoot = NULL;

    pucRoot = resPathGetBasename(pucArgNameFile);
    if (STR_IS_NOT_EMPTY(pucRoot)) {
      xmlChar *pucExt;
    
      pucExt = resPathGetExtension(pucRoot);
      if (STR_IS_NOT_EMPTY(pucExt)) {
	xmlChar *pucEnd;
      
	pucEnd = BAD_CAST xmlStrstr(pucRoot, pucExt);
	if (STR_IS_NOT_EMPTY(pucEnd) && (pucEnd - pucRoot > 0)) {
	  pucResult = xmlStrndup(pucRoot, pucEnd - pucRoot - 1);
	}
      }
      xmlFree(pucExt);
    }
    xmlFree(pucRoot);
  }
  return pucResult;
}
/* end of resPathGetRootname() */


/*! \return a dynamically allocated string with doubled '\\'
*/
xmlChar *
resPathGetQuoted(xmlChar *pucArgNameFile)
{
  xmlChar *pucResult = NULL;

  if (STR_IS_NOT_EMPTY(pucArgNameFile)) {
    int l;

    l = xmlStrlen(pucArgNameFile);
    pucResult = BAD_CAST xmlMalloc(l*2);
    if (pucResult) {
      int i;
      int j;

      for (i=j=0; i<l; i++,j++) {

	if (pucArgNameFile[i] == '\\') {
	  /* path separator */
	  pucResult[j] = '\\';
	  j++;
	}
	pucResult[j] = pucArgNameFile[i];
      }
      pucResult[j] = (xmlChar)'\0';
    }
  }
  return pucResult;
}
/* end of resPathGetQuoted() */


/*! \return pointer to end of the given path without quotes etc
*/
xmlChar *
resPathCutTrailingChars(xmlChar *pucArgPath)
{
  xmlChar *pucResult = pucArgPath;

  if (STR_IS_NOT_EMPTY(pucArgPath)) {
    xmlChar *pucEnd;
    int l;

    /* cut trailing separators */
    l = xmlStrlen(pucArgPath);
    for (pucEnd = BAD_CAST &pucArgPath[l-1];
      pucEnd > pucArgPath && (issep(*pucEnd) || isdot(*pucEnd));
      pucEnd--
      ) {
      *pucEnd = (xmlChar)'\0';
    }
    pucResult = pucEnd;
  }
  return pucResult;
}
/* end of resPathCutTrailingChars() */


/*! normalize all path separators to '/'

\return the pointer to pucArgPath or NULL if fails
*/
xmlChar *
resPathChangeToSlashes(xmlChar *pucArgPath)
{
  xmlChar *pucResult = NULL;

  if (STR_IS_NOT_EMPTY(pucArgPath)) {
    xmlChar *pucT;

    for (pucT = pucArgPath; pucT != NULL && !isend(*pucT); pucT++) {
      if (*pucT == '\\') {
	*pucT = '/';
      }
    }
    pucResult = pucArgPath;
  }
  return pucResult;
}
/* end of resPathChangeToSlashes() */


/*! \return the pointer to next path separator in pucArgPath or NULL if fails
*/
xmlChar *
resPathGetNextSeparator(xmlChar *pucArgPath)
{
  xmlChar *pucResult = NULL;

  if (STR_IS_NOT_EMPTY(pucArgPath)) {
    xmlChar *pucT;

    for (pucT = pucArgPath; pucT != NULL && !isend(*pucT); pucT++) {
      if (issep(*pucT)) {
	pucResult = pucT;
	break;
      }
    }
  }
  return pucResult;
}
/* end of resPathGetNextSeparator() */


/*! \return search for first occurance of pucArgNeedle in pucArgPath
*/
xmlChar *
resPathGetDirFind(xmlChar *pucArgPath, xmlChar *pucArgNeedle)
{
  xmlChar *pucResult = NULL;

  if (STR_IS_NOT_EMPTY(pucArgPath) && STR_IS_NOT_EMPTY(pucArgNeedle)) {
    xmlChar *pucT;

    pucT = BAD_CAST xmlStrcasestr(pucArgPath,pucArgNeedle);
    if (pucT) {
      int i;
      int iLengthNameDir;

      for (i=0; ! isseporend(pucT[i]); i++) {
	/* find next path separator or end of string */
      }
      iLengthNameDir = &pucT[i] - pucArgPath;

      if (iLengthNameDir > 0 && iLengthNameDir < BUFFER_LENGTH) {
	pucResult = xmlStrndup(pucArgPath,iLengthNameDir);
      }
    }
  }
  return pucResult;
}
/* end of resPathGetDirFind() */


/*! \return the path depth of context (= number of path separators)
 */
int
resPathGetDepth(xmlChar *pucArgPath)
{
  int iResult = -1;
  int iLength = -1;

  if (pucArgPath != NULL && (iLength = xmlStrlen(pucArgPath)) > 0) {
    int i;

    for (i = ((resPathIsDosDrive(pucArgPath) && iLength > 3) ? 3 : 0), iResult = 1; i < iLength; i++) {
      if (issep(pucArgPath[i])) {
	if (i > 0) { /* skip leading separators */
	  iResult++;
	}
	while (i < iLength && issep(pucArgPath[i])) {
	  i++;
	}
      }
    }
  }
  return iResult;
}
/* end of resPathGetDepth() */


/*! copies all path chars from pucArgSource to pucArgTarget

  \param pucArgTarget
  \param pucArgSource

  \return number of copied chars
 */
int
CopyPath(xmlChar *pucArgTarget, xmlChar *pucArgSource)
{
  xmlChar *pucS;
  xmlChar *pucT;
  
  assert(pucArgTarget);
  assert(pucArgSource);

  for ( pucT = pucArgTarget, pucS = pucArgSource; pucS != NULL && pucT != NULL; pucT++, pucS++ ) {

    while (isquot(*pucS)) pucS++;    /*! remove quote chars from target */

    if (isend(*pucS)) {
      *pucT = (xmlChar)'\0';
      break;
    }
    else if (ISPATH(*pucS)) {
      /* this character is valid for paths */
      *pucT = *pucS;
    }
    else {
      PrintFormatLog(1,"Invalid character '%c' in path",*pucS);
      *pucT = (xmlChar)'_';
    }
  }

  return (pucT - pucArgTarget);
}
/* end of CopyPath() */


/*! concatenates pucArgA and pucArgB to a new string

  \param pucArgA
  \param pucArgB
  \return a new allocated string with all concatenated arguments separated by '/' or NULL if fails

  for fast processing of pre-tested values (use resPathConcatNormalized() else)
 */
xmlChar *
resPathConcat(xmlChar *pucArgA, xmlChar *pucArgB)
{
  xmlChar *pucResult = NULL;

  if (STR_IS_NOT_EMPTY(pucArgA)) {
    if (STR_IS_NOT_EMPTY(pucArgB)) {
      /* concatenate strings, separated by slash */
      int iLength;

      pucResult = BAD_CAST xmlMalloc(xmlStrlen(pucArgA) + xmlStrlen(pucArgB) + 2);
      iLength = CopyPath(pucResult,pucArgA);

      if (resPathIsDosDrive(pucArgB)) {
	/* cut leading DOS drive */
	iLength += CopyPath(&pucResult[iLength],&pucArgB[2]);
      }
      else if (issep(*pucArgB) || issep(pucResult[iLength - 1])) {
	/* dont insert an path separator if next argument begins with one */
	iLength += CopyPath(&pucResult[iLength],pucArgB);
      }
      else {
#ifdef _WIN32
	iLength += CopyPath(&pucResult[iLength],BAD_CAST"\\");
	iLength += CopyPath(&pucResult[iLength],pucArgB);
#else
	iLength += CopyPath(&pucResult[iLength],BAD_CAST"/");
	iLength += CopyPath(&pucResult[iLength],pucArgB);
#endif
      }
    }
    else {
      pucResult = BAD_CAST xmlMalloc(xmlStrlen(pucArgA) + 1);
      CopyPath(pucResult,pucArgA);
    }
  }

  return pucResult;
}
/* end of resPathConcat() */


/*! \return a new allocated string with all concatenated arguments separated by '/' or NULL if fails
*/
xmlChar *
resPathConcatNormalized(xmlChar *pucArgA, xmlChar *pucArgB)
{
  xmlChar *pucResult = NULL;

  if (STR_IS_NOT_EMPTY(pucArgA)) {
    if (STR_IS_NOT_EMPTY(pucArgB)) {
      xmlChar *pucRelease;

      pucRelease = resPathConcat(pucArgA,pucArgB);
      pucResult = resPathNormalize(pucRelease);
      xmlFree(pucRelease);
    }
    else {
      pucResult = resPathNormalize(pucArgA);
    }
  }
  return pucResult;
}
/* end of resPathConcatNormalized() */


/*! \return a new allocated string of (resPathGetPathInNextArchivePtr(P) - P) or NULL if fails
*/
xmlChar *
resPathGetPathOfArchive(xmlChar *pucArg)
{
  xmlChar *pucResult = NULL;

  if (pucArg) {
    xmlChar *pucSep;

    if ((pucSep = resPathGetPathInNextArchivePtr(pucArg)) && pucSep - pucArg > 4) {
      pucResult = xmlStrndup(pucArg, pucSep - pucArg);
      resPathCutTrailingChars(pucResult);
      resPathRemoveQuotes(pucResult);
    }
  }
  return pucResult;
} /* end of resPathGetPathOfArchive() */


/*! \return a pointer to next archive name in pucArg or NULL by default

FALSE
	abc.zip
	def.txt

TRUE
	abc.zip/def.txt
	c:\\tmp\\abc.docx/word/document.xml
	c:\\tmp\\xyz.zip\\abc.docx/word/document.xml
	/tmp/abc.tar.gz/test/logo.png
	/tmp/abc.odt/content.xml

\todo handle other archive types .tar, .bz, .gz etc
\todo no-case-sensitive
*/
xmlChar *
resPathGetNameOfNextArchivePtr(xmlChar *pucArg)
{
  xmlChar *pucResult = NULL;

  if (pucArg) {
    while(issep(*pucArg) || isquot(*pucArg)) pucArg++; /* skip leading quotes and separators */
  
    if (resPathBeginIsArchive(pucArg)) {
      pucResult = pucArg; /* begin of pucArg is result */
    }
    else if ((pucResult = resPathGetNextSeparator(pucArg)) != NULL) {
      assert(pucResult > pucArg);
      while(issep(*pucResult) || isquot(*pucResult)) pucResult++; /* skip trailing quotes and separators */
      if (isend(*pucResult)) {
	pucResult = NULL; /* empty */
      }
      else {
	pucResult = resPathGetNameOfNextArchivePtr(pucResult); /* recursion */
      }
    }
    else {
      /* no archive name at all */
    }
  }
  return pucResult;
} /* end of resPathGetNameOfNextArchivePtr() */


/*! \return a pointer to next path in archive in pucArg or NULL by default
*/
xmlChar *
resPathGetPathInNextArchivePtr(xmlChar *pucArg)
{
  xmlChar *pucResult = NULL;
  xmlChar *pucArchive;

  if ((pucArchive = resPathGetNameOfNextArchivePtr(pucArg))) {
    if ((pucResult = resPathGetNextSeparator(pucArchive)) != NULL) {
      assert(pucResult > pucArchive);
      while(issep(*pucResult) || isquot(*pucResult)) pucResult++;
      if (isend(*pucResult)) {
	pucResult = NULL; /* empty */
      }
    }
  }
  return pucResult;
} /* end of resPathGetPathInNextArchivePtr() */


/*! \returns the enum value of pucArg's protocol
*/
resPathProtocol
resPathGetProtocol(xmlChar *pucArg)
{
  resPathProtocol iResult = PROTOCOL_NIL;

  if (STR_IS_NOT_EMPTY(pucArg)) {
    xmlChar *pucT;

    pucT = (isquot(*pucArg)) ? pucArg + 1 : pucArg;

    if (pucT[0] == (xmlChar)'z'
      && pucT[1] == (xmlChar)'i'
      && pucT[2] == (xmlChar)'p'
      && pucT[3] == (xmlChar)':') {
      iResult = PROTOCOL_ZIP;
    }
    else if (pucT[0] == (xmlChar)'h'
      && pucT[1] == (xmlChar)'t'
      && pucT[2] == (xmlChar)'t'
      && pucT[3] == (xmlChar)'p') {
      if (pucT[4] == (xmlChar)':'
	&& pucT[5] == (xmlChar)'/'
	&& pucT[6] == (xmlChar)'/') {
	iResult = PROTOCOL_HTTP;
      }
      else if (pucT[4] == (xmlChar)'s'
	&& pucT[5] == (xmlChar)':'
	&& pucT[6] == (xmlChar)'/'
	&& pucT[7] == (xmlChar)'/') {
	iResult = PROTOCOL_HTTPS;
      }
    }
    else if (pucT[0] == (xmlChar)'f'
      && pucT[1] == (xmlChar)'t'
      && pucT[2] == (xmlChar)'p') {
      if (pucT[3] == (xmlChar)':'
	&& pucT[4] == (xmlChar)'/'
	&& pucT[5] == (xmlChar)'/') {
	iResult = PROTOCOL_FTP;
      }
      else if (pucT[3] == (xmlChar)'s'
	&& pucT[4] == (xmlChar)':'
	&& pucT[5] == (xmlChar)'/'
	&& pucT[6] == (xmlChar)'/') {
	iResult = PROTOCOL_FTPS;
      }
    }
    else if (pucT[0] == (xmlChar)'f'
      && pucT[1] == (xmlChar)'i'
      && pucT[2] == (xmlChar)'l'
      && pucT[3] == (xmlChar)'e'
      && pucT[4] == (xmlChar)':'
      && pucT[5] == (xmlChar)'/'
      /* next slashes are not mandatory */ ) {
      iResult = PROTOCOL_FILE;
    }
  }
  return iResult;
}
/* end of resPathGetProtocol() */


/*! collapses a given string pointer to path pieces

  \param pucArg pointer to path string
  \param iArgOpts options of collapsing
  \returns a pointer to a new allocated string

- removes redundant path separators,
- removes quotes
- replace in-path '..'

- stdin
-
*/
xmlChar *
resPathCollapse(xmlChar *pucArg, int iArgOpts)
{
  BOOL_T fPathLocal = TRUE;
  BOOL_T fPathValid = TRUE;
  xmlChar *pucA, *pucB;
  xmlChar *pucResult = NULL;
  int ciLength;
  resPathProtocol eProtocol;

  if (pucArg == NULL) {
    return pucResult;
  }

#if 0
  pucResult = xmlStrdup(pucArg);
  resPathRemoveQuotes(pucResult);
#else
  for ( ; isquot(*pucArg); pucArg++); /* skip leading quotes */

  for (ciLength=0; ! isend(pucArg[ciLength]) && ! isquot(pucArg[ciLength]); ciLength++) { /* count chars between quotes */
    if (ciLength > BUFFER_LENGTH) {
      return pucResult;
    }
  }
  pucResult = xmlStrndup(pucArg,ciLength); /* copy without quotes */
#endif
  
  if (STR_IS_EMPTY(pucResult) || resPathIsStd(pucResult) || (iArgOpts & FS_PATH_NUL)) {
    /* path contains nothing or stdout "-" */
    return pucResult;
  }

  if ((isdot(pucResult[0]) && isseporend(pucResult[1])) || (isdot(pucResult[0]) && isdot(pucResult[1]) && isseporend(pucResult[2]))) {
    /* path starts with "." or ".." */
    xmlChar *pucT, *pucTT;

    pucT = resPathGetCwd();

    pucTT = resPathConcat(pucT,pucResult);
    xmlFree(pucT);
    xmlFree(pucResult);
    pucResult = pucTT;
  }

  pucA = pucB = pucResult;

  eProtocol = resPathGetProtocol(pucArg); /* detect protocol prefix */
  if (eProtocol == PROTOCOL_ZIP) {  /* access to ZIP file */
    PrintFormatLog(4,"ZIP path '%s' must be absolute",pucResult);
    return pucResult;
  }
  else if (eProtocol == PROTOCOL_HTTP) { /* web access */
    pucA += 7; /* this is an URL */
    pucB = pucA;
    fPathLocal = FALSE;
  }
  else if (eProtocol == PROTOCOL_HTTPS) {
    pucA += 8; /* this is an URL */
    pucB = pucA;
    fPathLocal = FALSE;
  }
  else if (eProtocol == PROTOCOL_FTP) { /* web access */
    pucA += 6; /* this is an URL */
    pucB = pucA;
    fPathLocal = FALSE;
  }
  else if (eProtocol == PROTOCOL_FTPS) {
    pucA += 7; /* this is an URL */
    pucB = pucA;
    fPathLocal = FALSE;
  }
  else if (eProtocol == PROTOCOL_FILE) { /* file access */
    if (isend(pucResult[7])) {
      /* empty string after prefix "file://" */
      xmlFree(pucResult);
      return NULL;
    }
    else { /* cut "file://" prefix */
#ifdef _MSC_VER
      for (pucA += 5; issep(*pucA); pucA++); /* skip separators */
#else
      for (pucA += 5; issep(pucA[1]); pucA++); /* skip separators */
#endif
      pucB = pucResult;
    }
  }
  else if (resPathIsUNC(pucResult)) {
    /* absolute UNC path */
    for (pucA = pucResult; issep(pucA[0]); pucA++) { /* skip separators */
#ifdef _WIN32
      pucA[0] = (xmlChar)'\\';
#else
      pucA[0] = (xmlChar)'/';
#endif
    }
    pucB = pucA;
  }
  else if (issep(pucResult[0])) {
    /* absolute path */
  }
  else if (resPathIsDosDrive(pucResult)) {
#ifdef _WIN32
    /* OK */
#else
    pucA += 2; /* skip leading DOS drive */
    pucB = pucResult;
#endif
  }
  else {
    /*
      local relative path
    */
    for (; fPathValid && isdot(pucA[0]) && isdot(pucA[1]) && isseporend(pucA[2]); pucA += 3) { /* skip all leading "../" */
      if ((iArgOpts & FS_PATH_SEP)) {
	/*! set path separators according to OS */
#ifdef _WIN32
	pucA[2] = (xmlChar)(fPathLocal ? '\\' : '/');
#else
	pucA[2] = (xmlChar)'/';
#endif
      }
    }
    pucB = pucA;
    while ((iArgOpts & FS_PATH_SEP) && issep(pucA[0])) pucA++; /* skip redundant seaparators */
  }

  while (fPathValid && ! isend(pucA[0])) {

    if (((iArgOpts & FS_PATH_SEP) || (iArgOpts & FS_PATH_SELF) || (iArgOpts & FS_PATH_UP)) && issep(pucA[0])) {

      if ((iArgOpts & FS_PATH_SEP)) {
	/*! set path separators according to OS */
#ifdef _WIN32
	pucB[0] = (xmlChar)(fPathLocal ? '\\' : '/');
#else
	pucB[0] = (xmlChar)'/';
#endif
      }
      pucB++;

      while (fPathValid && issep(pucA[0])) pucA++; /* skip redundant seaparators */

      while ((iArgOpts & FS_PATH_SELF) && fPathValid && isdot(pucA[0]) && issep(pucA[1])) pucA += 2; /* skip all dir selfs "./" */

      while ((iArgOpts & FS_PATH_UP) && fPathValid && isdot(pucA[0]) && isdot(pucA[1]) && isseporend(pucA[2])) { /* skip all "../" */

	if (pucB - pucResult < 1) {
	  fPathValid = FALSE;
	}
	else {

	  for (pucB--; pucB >= pucResult && issep(pucB[0]); pucB--) ;

	  for (; pucB > pucResult && ! issep(pucB[0]); pucB--) ;

	  if (pucB >= pucResult && issep(pucB[0])) {
	    pucB++;
	  }

	  pucA += 2;
	  while (fPathValid && issep(pucA[0])) pucA++; /* skip redundant seaparators */
	}
      }

      if (resPathIsDosDrive(pucA) && (eProtocol != PROTOCOL_FILE || pucA - pucResult > 8)) {
	/* skip a DOS drive letter in the middle of path */
	pucA += 3;
      }
    }
    else {
      *pucB = *pucA; /* copy single char */
      pucA++;
      pucB++;
    }
  }

  if (fPathValid) {
    while ((iArgOpts & FS_PATH_SELF) && pucB > pucResult && isdot(*(pucB - 1))) pucB--; /* skip all trailing dir selfs "/." */
    pucB[0] = (xmlChar)'\0'; /* terminate result */
  }
  else {
    xmlFree(pucResult);
    pucResult = resPathCollapse(pucArg, FS_PATH_SELF | FS_PATH_SEP); /* build a medium collapsed result */
    if (pucResult == NULL) {
      pucResult = resPathCollapse(pucArg, FS_PATH_NUL); /* build a minimal collapsed result */
    }
  }

  return pucResult;
}
/* end of resPathCollapse() */


/*! normalize a given string pointer to path pieces

  \param pucArg pointer to path string
  \returns a pointer to a new allocated string

- removes redundant path separators,
- removes all quotes
- set seaparators according to OS
- replace in-path '..'

- stdin
- 
*/
xmlChar *
resPathNormalize(xmlChar *pucArg)
{
  xmlChar *pucResult = NULL;

  /*!\todo translate relative path value to absolute */

  if (STR_IS_EMPTY(pucArg)) {
    /* not an usable argument */
  }
  else if ((pucResult = resPathCollapse(pucArg, FS_PATH_FULL))) {
    /* OK */
  }
  return pucResult;
}
/* end of resPathNormalize() */


/*! \return pointer pucArg, all quotes removed
*/
xmlChar *
resPathRemoveQuotes(xmlChar *pucArg)
{
  if (STR_IS_NOT_EMPTY(pucArg)) {
    xmlChar *pucA;
    xmlChar *pucB;

    for (pucA = pucB = pucArg; !isend(*pucA); pucA++) {
      if (isquot(*pucA)) {
      }
      else {
	*pucB = *pucA;
	pucB++;
      }
    }
    *pucB = (xmlChar)'\0';
  }
  return pucArg;
}
/* end of resPathRemoveQuotes() */


/*! \return a new allocated string of current working directory

 * \bug not thread safe!!
*/
xmlChar *
resPathGetCwd(void)
{
  char buffer[BUFFER_LENGTH];
  char *pchValue;
  xmlChar *pucResult = NULL;

#ifdef _MSC_VER
  pchValue = _getcwd((char *)buffer, BUFFER_LENGTH);
#else
  pchValue = getcwd((char *)buffer, BUFFER_LENGTH);
#endif
  if (STR_IS_NOT_EMPTY(pchValue)) {
    pucResult = resPathEncode((const char *)buffer);
  }
  else {
    PrintFormatLog(1, "No valid working directory");
  }

  return pucResult;
}
/* end of resPathGetCwd() */


#ifdef TESTCODE
#include "test/test_res_path.c"
#endif

