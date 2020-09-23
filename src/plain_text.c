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

/* 
 */
#include <libxml/tree.h>
#include <libxml/parser.h>

/* 
 */
#include "basics.h"
#include <res_node/res_node_io.h>
#include <cxp/cxp.h>
#include <cxp/cxp_dir.h>
#include "dom.h"
#include "plain_text.h"
#include "utils.h"
#ifdef HAVE_LIBSQLITE3
#include <database/database.h>
#endif
#ifdef HAVE_PIE
#include <pie/pie_text.h>
#include <ics/ics.h>
#include <vcf/vcf.h>
#endif


/*! global iconv conversion descriptors
 */

typedef struct {

    iconv_t plain;  /*! charset plain text file content */

    iconv_t utf16be;  /*!  */
    iconv_t utf16le;  /*!  */
    iconv_t isolat1;  /*!  */

} cd_direction_t;

typedef struct {

  cd_direction_t from;

  cd_direction_t to;

} cd_collection_t;


cd_collection_t cd;


static void
plainIconvCleanup(void);

static BOOL_T
plainIconvInit(void);


/*! Reads and returns the whole file content of this context as a '0' terminated string.

\param prnArg the context
\param iArgMax maximum number of buffer blocks (Bytes = BUFFER_LENGTH * iArgMax)
\return a pointer to the string of content, and reset content of this context struct

\todo do encoding per line??
*/
xmlChar *
plainGetContextTextEat(resNodePtr prnArg, int iArgMax)
{
  xmlChar *pucResult = NULL;

  if (prnArg) {
    xmlChar* pucT;

    pucT = BAD_CAST resNodeGetContent(prnArg, iArgMax);
    if (pucT) {
      int iLength;

      iLength = resNodeGetSize(prnArg);
      if (iLength > 0) {
	pucT[iLength] = (xmlChar) '\0';
	/*! encode to UTF-8 */
	pucResult = plainDetectAndDecode((const unsigned char*)pucT, iLength);
      }
    }
  }
  return pucResult;
}
/* end of plainGetContextTextEat() */


/* cleanup for libiconv
 */
void
plainIconvCleanup(void)
{
  if (cd.from.plain != NULL) {
    iconv_close(cd.from.plain);
    cd.from.plain = NULL;
  }
  if (cd.to.plain != NULL) {
    iconv_close(cd.to.plain);
    cd.to.plain = NULL;
  }
  if (cd.from.utf16be != NULL) {
    iconv_close(cd.from.utf16be);
    cd.from.utf16be = NULL;
  }
  if (cd.to.utf16be != NULL) {
    iconv_close(cd.to.utf16be);
    cd.to.utf16be = NULL;
  }
  if (cd.from.utf16le != NULL) {
    iconv_close(cd.from.utf16le);
    cd.from.utf16le = NULL;
  }
  if (cd.to.utf16le != NULL) {
    iconv_close(cd.to.utf16le);
    cd.to.utf16le = NULL;
  }
  if (cd.from.isolat1 != NULL) {
    iconv_close(cd.from.isolat1);
    cd.from.isolat1 = NULL;
  }
  if (cd.to.isolat1 != NULL) {
    iconv_close(cd.to.isolat1);
    cd.to.isolat1 = NULL;
  }
}


/*! 
*/
BOOL_T
plainIconvInit(void)
{
  /*
    create the required conversion descriptors
  */
  cd.from.utf16be = iconv_open("UTF-8","UTF-16BE");
  if (cd.from.utf16be == (iconv_t) -1) {
    /* Something went wrong.  */
    PrintFormatLog(1,"Conversion from 'UTF-16BE' to UTF-8 not possible");
  }

  cd.to.utf16be = iconv_open("UTF-16BE","UTF-8");
  if (cd.to.utf16be == (iconv_t) -1) {
    /* Something went wrong.  */
    PrintFormatLog(1,"Conversion to 'UTF-16BE' to UTF-8 not possible");
  }

  /*
    create the required conversion descriptors
  */
  cd.from.utf16le = iconv_open("UTF-8","UTF-16LE");
  if (cd.from.utf16le == (iconv_t) -1) {
    /* Something went wrong.  */
    PrintFormatLog(1,"Conversion from 'UTF-16LE' to UTF-8 not possible");
  }

  cd.to.utf16le = iconv_open("UTF-16LE","UTF-8");
  if (cd.to.utf16le == (iconv_t) -1) {
    /* Something went wrong.  */
    PrintFormatLog(1,"Conversion to 'UTF-16LE' to UTF-8 not possible");
  }

  /*
    create the required conversion descriptors
  */
  cd.from.isolat1 = iconv_open("UTF-8","ISO-8859-1");
  if (cd.from.isolat1 == (iconv_t) -1) {
    /* Something went wrong.  */
    PrintFormatLog(1,"Conversion from 'ISO-8859-1' to UTF-8 not possible");
  }

  cd.to.isolat1 = iconv_open("ISO-8859-1","UTF-8");
  if (cd.to.isolat1 == (iconv_t) -1) {
    /* Something went wrong.  */
    PrintFormatLog(1,"Conversion to 'ISO-8859-1' to UTF-8 not possible");
  }

  /*
    create the required conversion descriptors
  */
  cd.from.plain = iconv_open("UTF-8","ISO-8859-1");
  if (cd.from.plain == (iconv_t) -1) {
    /* Something went wrong.  */
    PrintFormatLog(1,"Conversion from 'ISO-8859-1' to UTF-8 not possible");
  }

  cd.to.plain = iconv_open("ISO-8859-1","UTF-8");
  if (cd.to.plain == (iconv_t) -1) {
    /* Something went wrong.  */
    PrintFormatLog(1,"Conversion to 'ISO-8859-1' to UTF-8 not possible");
  }

  /* register for exit() */
  if (atexit(plainIconvCleanup) != 0) {
    exit(EXIT_FAILURE);
  }
  
  return TRUE;
}
/* end of plainIconvInit() */



/*! Detect encoding of string at pucArg and convert to UTF-8 encoding.

\param pucArg

\return pointer to new buffer or NULL
*/
xmlChar *
plainDetectAndDecode(const unsigned char *pucArg, int iLength)
{
  int iA, iB;
  int iOffset;
  xmlChar *pucResult = NULL;
  xmlCharEncoding encResult;
  iconv_t cdHere;
  unsigned char *pucIn;

  assert(pucArg);

  if (cd.from.utf16be == NULL) {
    plainIconvInit();
  }
  cdHere = cd.from.isolat1;

  /*
    try to detect encoding using BOM or xmlDetectCharEncoding()
  */
  if (pucArg[0]=='\xEF' && pucArg[1]=='\xBB' && pucArg[2]=='\xBF') /* UTF-8 */ {
    iOffset = 3;
    encResult = XML_CHAR_ENCODING_UTF8;
  }
  else if ((pucArg[0]=='\xFE' && pucArg[1]=='\xFF') /* UTF-16, big-endian */) {
    iOffset = 2;
    encResult = XML_CHAR_ENCODING_UTF16BE;
  }
  else if ((pucArg[0]=='\xFF' && pucArg[1]=='\xFE') /* UTF-16, little-endian */) {
    iOffset = 2;
    encResult = XML_CHAR_ENCODING_UTF16LE;
  }
#if 0
  else if ((pucArg[0]=='\x00' && pucArg[1]=='\x00' && pucArg[2]=='\xFE' && pucArg[3]=='\xFF') /* UTF-32, big-endian */
	   ||
	   (pucArg[0]=='\xFF' && pucArg[1]=='\xFE' && pucArg[2]=='\x00' && pucArg[3]=='\x00') /* UTF-32, little-endian */) {
  }
#endif
  else if (xmlCheckUTF8(pucArg)) {
    iOffset = 0;
    encResult = XML_CHAR_ENCODING_UTF8;
  }
  else {
    iOffset = 0;
    encResult = xmlDetectCharEncoding((const unsigned char*)pucArg,iLength);
  }
  PrintFormatLog(4,"Encoding detected '%i'",encResult);

  pucIn  = BAD_CAST (pucArg + iOffset);			   /* skip BOM */

  /* remove unusable chars from input string, especially for Windows ... */
  for (iA=0, iB=0; iA < iLength && pucIn[iA] != '\0'; iA++) {
    if (pucIn[iA] == '\r') {
      /* ignore this char */
    }
    else {
      pucIn[iB] = pucIn[iA];
      iB++;
    }
  }
  pucIn[iB] = '\0';

  if (iLength < 1) {
    pucResult = xmlStrdup(BAD_CAST "");
  }
  else if (encResult == XML_CHAR_ENCODING_UTF8) {
    /* no conversion required or not possible */
    pucResult = xmlStrdup(pucIn);
  }
  else {
    size_t int_out;
    size_t int_in;
    size_t nconv;
    char *pucOut;

    switch (encResult) {
    case XML_CHAR_ENCODING_UTF8:
      break;
    case XML_CHAR_ENCODING_UTF16BE:
      cdHere = cd.from.utf16be;
      /*!\todo UTF16BE not yet implemented */
      break;
    case XML_CHAR_ENCODING_UTF16LE:
      cdHere = cd.from.utf16le;
      /*!\todo UTF16LE not yet implemented */
      break;
    case XML_CHAR_ENCODING_8859_1:
    case XML_CHAR_ENCODING_NONE:
    default:
      break;
    }

    if (cdHere) {
      int_in  = iLength - iOffset;
      int_out = iLength * 2;

      pucResult = BAD_CAST xmlMalloc((int_out + 1) * sizeof(xmlChar));
      if (pucResult) {
	pucOut = (char*)pucResult;
	nconv = iconv(cdHere, (char**)&pucIn, &int_in, &pucOut, &int_out);
	if (errno==EILSEQ) {
	  PrintFormatLog(1, "invalid byte sequence in the input");
	}
	else if (errno==E2BIG) {
	  PrintFormatLog(1, "it ran out of space in the output buffer.");
	}
	else if (errno==EINVAL) {
	  PrintFormatLog(1, "an incomplete byte sequence at the end of the input buffer.");
	}
	else if (errno==EBADF) {
	  PrintFormatLog(1, "The cd argument is invalid.");
	}

#if 0
	if (nconv == (size_t) -1) {
	  /* Not everything went right.  It might only be
	     an unfinished byte sequence at the end of the
	     buffer.  Or it is a real problem.  */
	  if (errno == EINVAL)
	    /* This is harmless.  Simply move the unused
	       bytes to the beginning of the buffer so that
	       they can be used in the next round.  */
	    memmove (inbuf, inptr, insize);
	  else {
	    /* It is a real problem.  Maybe we ran out of
	       space in the output buffer or we have invalid
	       input.  In any case back the file pointer to
	       the position of the last processed byte.  */
	    lseek (fd, -insize, SEEK_CUR);
	    result = -1;
	    break;
	  }
	}
#endif

	*pucOut = '\0';
      }
    }
    else {
      pucResult = xmlStrdup(pucArg); /*\todo set pucResult = pucArg to avoid additional copy */
    }
  }

  return pucResult;
}
/* end of plainDetectAndDecode() */


#ifdef TESTCODE
#include "test/test_plain_text.c"
#endif
