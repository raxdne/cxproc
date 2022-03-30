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

#include <math.h>
#include <inttypes.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h>

#include "basics.h"
#include "utils.h"
#include <cxp/cxp_dtd.h>


/*! internal level for debug messages */
static int level_set = -1;

/*! set the internal debug level

0 - no log messages at all, error messages only
1 - minimum log level
*/
int
SetLogLevel(int level)
{
  int iResult = -1;

  if (level > 0) {
    if (level != level_set) {
      if (level > LEVEL_MAX) {
	level = LEVEL_MAX;
      }
      PrintFormatLog(0,"Change log level from '%i' to '%i'", level_set, level);
      level_set = level;
      iResult = 0;
    }
  }
  else {
    level_set = 0;
    iResult = 0;
  }
  return iResult;
}
/* end of SetLogLevel() */


/*! set the internal debug level via string

0 - no log messages at all, error messages only
1 - minimum log level
*/
int
SetLogLevelStr(xmlChar *pucArg)
{
  int iResult = -1;

  if (pucArg != NULL && xmlStrlen(pucArg) > 0) {
    int i;

    if (*pucArg == (xmlChar)'\"') {
      pucArg++;
    }

    i =  atoi((const char *)pucArg);
    iResult = SetLogLevel(i);
  }
  return iResult;
}
/* end of SetLogLevelStr() */


/*! reads blockwise content from input stream 'argin'
  \todo UTF8Check() encoding of input should be UTF-8
  \return pointer to a dynamically allocated buffer or NULL in case of errors
*/
xmlChar *
ReadUTF8ToBufferNew(FILE* argin) 
{
  xmlChar* pucResult = NULL;

  if (argin != NULL) {
    size_t b;
    size_t l;

    for (b = 0, l = BUFFER_LENGTH; (pucResult = BAD_CAST xmlRealloc((void *)pucResult, l)) != NULL; b += BUFFER_LENGTH, l += BUFFER_LENGTH) {
      size_t k;

      PrintFormatLog(4, "%i Byte allocated", l);

      if ((k = fread(&pucResult[b], 1, BUFFER_LENGTH, argin)) < BUFFER_LENGTH) {
	/* end of input reached */
	l = b + k;
	pucResult[l] = '\0';
	PrintFormatLog(2, "%i Byte read", l);
	break;
      }
    }
  }
  return pucResult;
} /* end of ReadUTF8ToBufferNew() */


/*! \return TRUE if pchArg was converted to lowercases successfully
  Stops at the string end or line end of pchArg, works properly when pchArg contains ASCII chars only!
*/
BOOL_T
StringToLower(char *pchArg)
{
  BOOL_T fResult = FALSE;

  if (pchArg) {
    char *pchT;

    /*! convert to wide char -> to lower -> UTF-8 */

    for (pchT=pchArg; *pchT != '\0' && *pchT != '\n' && *pchT != '\r'; pchT++) {
      *pchT = (char)tolower(*pchT);
    }

    fResult = (pchT > pchArg);
  }
  return fResult;
} /* end of StringToLower() */


/*! \return TRUE if char is a hexadecimal digit
*/
BOOL_T
ishexdigit(char chArg)
{
  return (isdigit(chArg) || (chArg >= 'A' && chArg < 'G') || (chArg >= 'a' && chArg < 'g'));
}
/* end of ishexdigit() */


/*! replace spaces, convert to lower case
*/
BOOL_T
StringToId(char *pchArg)
{
  char *pchT;

  for (pchT=pchArg; *pchT != '\0' && *pchT != '\n' && *pchT != '\r'; pchT++) {
    if (isalnum(*pchT)) {
      *pchT = (char)tolower(*pchT);
    }
    else {
      *pchT = '_';
    }
  }

  return TRUE;
}
/* end of StringToId() */


/*! \return TRUE if pchArg was converted to uppercases successfully

  Stops at the string end or line end of pchArg, works properly when pchArg contains ASCII chars only!
*/
BOOL_T
StringToUpper(char *pchArg)
{
  BOOL_T fResult = FALSE;

  if (pchArg) {
    char *pchT;

    /*! convert to wide char -> to upper -> UTF-8 */

    for (pchT=pchArg; *pchT != '\0' && *pchT != '\n' && *pchT != '\r'; pchT++) {
      *pchT = (char)toupper(*pchT);
    }

    fResult = (pchT > pchArg);
  }
  return fResult;
} /* end of StringToUpper() */


/*! \return TRUE if all chars of pchArgNeedle are the begin of pchArgBegin
*/
BOOL_T
StringBeginsWith(char *pchArgBegin, const char *pchArgNeedle)
{
  BOOL_T fResult = FALSE;

  if (STR_IS_NOT_EMPTY(pchArgBegin) && STR_IS_NOT_EMPTY(pchArgNeedle)) {
    int i;

    assert(pchArgBegin != pchArgNeedle);

    for (fResult = TRUE, i=0; fResult; i++) {
      if (pchArgNeedle[i] == pchArgBegin[i]) {
	if (pchArgNeedle[i] == '\0') {
	  break; /* end of a string found */
	}
      }
      else if (pchArgNeedle[i] == '\0') {
	break; /* end of a string found */
      }
      else {
	fResult = FALSE;
      }
    }
  }
  return fResult;
} /* end of StringBeginsWith() */


/*! \return TRUE if all chars of pchArgNeedle are the end of pchArgEnd
*/
char *
StringEndsWith(char* pchArgEnd, const char* pchArgNeedle)
{
  char *pcResult = NULL;

  if (STR_IS_NOT_EMPTY(pchArgEnd) && STR_IS_NOT_EMPTY(pchArgNeedle)) {
    int i, j;

    assert(pchArgEnd != pchArgNeedle);

    for (i = (int)strlen(pchArgEnd), j = (int)strlen(pchArgNeedle); i > -1 && j > -1; i--, j--) {
      if (pchArgNeedle[j] == pchArgEnd[i]) {
	if (j == 0) {
	  pcResult = &pchArgEnd[i];
	  break; /* end of a string found */
	}
      }
      else {
	break; /* end of a string found */
      }
    }
  }
  return pcResult;
} /* end of StringEndsWith() */


/*! \return pointer pucArg, all numeric character references replaced by valid UTF-8 encoding

  s. "contrib/pie/www/Test/Documents/TestMarkupUnicode.txt"
*/
xmlChar *
StringDecodeNumericCharsNew(xmlChar *pucArg)
{
  xmlChar *pucResult = NULL;

  if (STR_IS_NOT_EMPTY(pucArg)) {
    int i, k;
    
    pucResult = xmlStrdup(pucArg);

    for (k=i=0; pucArg[i]; i++) {
      int iCode;
      xmlChar *pucT = NULL;
      
      iCode = -1;

      if (pucArg[i] == (xmlChar)'\\' && (pucArg[i+1] == (xmlChar)'u' || pucArg[i+1] == (xmlChar)'U') && ishexdigit(pucArg[i+2])) {
	iCode = strtol((const char *)&pucArg[i+2], (char **)&pucT, 16); /* hexadecimal */
      }
      else if (pucArg[i] == (xmlChar)'&' && pucArg[i+1] == (xmlChar)'#') {
	if (pucArg[i+2] == (xmlChar)'x' && ishexdigit(pucArg[i+3])) {
	  iCode = strtol((const char*)&pucArg[i+3], (char**)&pucT, 16); /* hexadecimal */
	}
	else if (isdigit(pucArg[i+2])) {
	  iCode = strtol((const char*)&pucArg[i+2], (char**)&pucT, 10); /* decimal */
	}

	if (pucT != NULL && pucT[0] == (xmlChar)';') {
	  pucT++;
	}
	else {
	  /* ignore this */
	  iCode = -1;
	}
      }

      if (iCode > -1 && pucT != NULL) {
	/* numeric character reference detected */
	int j;

	if (iCode > 129 && iCode < 160) {
	  /* to be skipped (s. http://www.cs.tut.fi/~jkorpela/www/windows-chars.html) */
	  j = (int)(pucT - &pucArg[i]);
	  i += j - 1;
	  k += j;
	}
	else {
	  j = xmlCopyCharMultiByte(&pucResult[k], iCode);
	  assert(j > 0);
	  k += j;
	  i = (int)(pucT - pucArg - 1);
	}
      }
      else {
	pucResult[k] = pucArg[i];
	k++;
      }
    }
    pucResult[k] = (xmlChar)'\0';
  }
  return pucResult;
} /* end of StringDecodeNumericCharsNew() */


/*! \return pointer pucArg, all '&', '<', '>' characters replaced by valid XML entity

simplified replacement for xmlEncodeEntitiesReentrant() for basic entities only
*/
xmlChar*
StringEncodeXmlDefaultEntitiesNew(xmlChar* pucArg)
{
  xmlChar* pucResult = NULL;

  if (STR_IS_NOT_EMPTY(pucArg)) {
    int i, j, k;
  xmlChar* pucA;
  xmlChar* pucB;

    pucResult = NULL;

    for (i=j=0, pucA=pucB=pucArg; pucArg[j]; j++) {

      if (pucArg[j] == (xmlChar)'&') {
	pucResult = xmlStrncat(pucResult,&pucArg[i],j-i);
	pucResult = xmlStrcat(pucResult, BAD_CAST"&amp;");
	i = j + 1;
      }
      else if (pucArg[j] == (xmlChar)'<') {
	pucResult = xmlStrncat(pucResult,&pucArg[i],j-i);
	pucResult = xmlStrcat(pucResult,BAD_CAST"&lt;");
	i = j + 1;
      }
      else if (pucArg[j] == (xmlChar)'>') {
	pucResult = xmlStrncat(pucResult,&pucArg[i],j-i);
	pucResult = xmlStrcat(pucResult,BAD_CAST"&gt;");
	i = j + 1;
      }
    }
    pucResult = xmlStrcat(pucResult, &pucArg[i]);
  }
  return pucResult;
} /* end of StringEncodeXmlDefaultEntitiesNew() */


/*! \return pointer
*/
xmlChar *
StringReplaceUmlauteNew(const xmlChar *pucArg)
{
  xmlChar *pucResult = NULL;

  if (STR_IS_NOT_EMPTY(pucArg) && (pucResult = xmlStrdup(pucArg)) != NULL) {
    int i, j;
    BOOL_T fReplaced;
    
    for (fReplaced = FALSE, i=0; pucResult[i]; i++) {
      int iCode;
      //xmlChar *pucT = NULL;

      /* detect Multibyte characters */
      if (pucResult[i] == (xmlChar)'\xC3' && pucResult[i+1] == (xmlChar)'\xA4') {
	//pucT = BAD_CAST"ae";
	pucResult[i++] = (xmlChar)'a';
	pucResult[i] = (xmlChar)'e';
	fReplaced = TRUE;
      }
      else if (pucResult[i] == (xmlChar)'\xC3' && pucResult[i+1] == (xmlChar)'\x84') {
	pucResult[i++] = (xmlChar)'A';
	pucResult[i] = (xmlChar)'e';
	fReplaced = TRUE;
      }
      else if (pucResult[i] == (xmlChar)'\xC3' && pucResult[i+1] == (xmlChar)'\xB6') {
	pucResult[i++] = (xmlChar)'o';
	pucResult[i] = (xmlChar)'e';
	fReplaced = TRUE;
      }
      else if (pucResult[i] == (xmlChar)'\xC3' && pucResult[i+1] == (xmlChar)'\x96') {
	pucResult[i++] = (xmlChar)'O';
	pucResult[i] = (xmlChar)'e';
	fReplaced = TRUE;
      }
      else if (pucResult[i] == (xmlChar)'\xC3' && pucResult[i+1] == (xmlChar)'\xBC') {
	pucResult[i++] = (xmlChar)'u';
	pucResult[i] = (xmlChar)'e';
	fReplaced = TRUE;
      }
      else if (pucResult[i] == (xmlChar)'\xC3' && pucResult[i+1] == (xmlChar)'\x9C') {
	pucResult[i++] = (xmlChar)'U';
	pucResult[i] = (xmlChar)'e';
	fReplaced = TRUE;
      }
      else if (pucResult[i] == (xmlChar)'\xC3' && pucResult[i+1] == (xmlChar)'\x9F') {
	pucResult[i++] = (xmlChar)'s';
	pucResult[i] = (xmlChar)'s';
	fReplaced = TRUE;
      }

      //pucResult = xmlStrncat(pucResult,pucT,2);
    }

    if (fReplaced == FALSE) {
      xmlFree(pucResult);
      pucResult = NULL;
    }
  }
  return pucResult;
} /* end of StringReplaceUmlauteNew() */


/*! removes all pair of apostrophs in pucArg

\return TRUE if there is a pair of apostrophs in pucArg (and string is modified)
*/
BOOL_T
StringRemovePairQuotes(xmlChar *pucArg)
{
  BOOL_T fResult = FALSE;

  if (STR_IS_NOT_EMPTY(pucArg)) {
    xmlChar *pucA;
    xmlChar *pucB;

    for (pucA = pucArg; isspace(*pucA); pucA++);

    for (pucB = pucArg + xmlStrlen(pucArg) - 1; pucB > pucA && isspace(*pucB); pucB--) ;

    if (isend(*pucA)) {
      /* only spaces, string is empty */
      pucArg[0] = (xmlChar)'\0';
    }
    else if (pucA == pucB) {
      /* string is one char only */
      pucArg[0] = *pucA;
      pucArg[1] = (xmlChar)'\0';
    }
    else if (*pucA == (xmlChar)'\'' || *pucA == (xmlChar)'\"') {
      if (*pucB == *pucA) {
	/* there is a pair of apostrophs in pucArg */
	if (pucB - pucA > 1) {
	  memmove(pucArg,pucA+1,pucB - pucA - 1);
	  pucArg[pucB - pucA - 1] = (xmlChar)'\0';
	}
	else {
	  /* the quotes are neighbours, string empty */
	  pucArg[0] = (xmlChar)'\0';
	}
	fResult = TRUE;
      }
    }
    else if (pucB - pucA > 1) {
      /* there is no pair of quotes, but eliminate the leading and trailing spaces */
      if (pucA > pucArg) {
	memmove(pucArg, pucA, pucB - pucA + 2);
	pucArg[pucB - pucA + 1] = (xmlChar)'\0';
      }
      else {
	/* terminate pucArg before trailing spaces */
	pucB[1] = (xmlChar)'\0';
      }
    }
  }
  return fResult;
} /* end of StringRemovePairQuotes() */


/*! removes all pair quote in pucArg

\return TRUE if pucArg is not empty, elese FALSE
*/
BOOL_T
StringRemoveBackslashes(xmlChar* pucArg)
{
  BOOL_T fResult = FALSE;

  if (STR_IS_NOT_EMPTY(pucArg)) {
    xmlChar* pucA;
    xmlChar* pucB;

    for (pucA = pucB = pucArg; isend(*pucA) == FALSE; pucA++, pucB++) {
      if (*pucA == (xmlChar)'\\') {
	pucA++;
      }
      *pucB = *pucA;
    }
    *pucB = (xmlChar)'\0';
    fResult = TRUE;
  }
  return fResult;
} /* end of StringRemoveBackslashes() */


/*! \return pointer to a quoted copy of pucArg
*/
xmlChar *
StringAddPairQuotesNew(xmlChar *pucArg, xmlChar *pucArgChar)
{
  xmlChar *pucResult = NULL;

  if (STR_IS_EMPTY(pucArg)) {
  }
  else if (HasStringPairQuotes(pucArg)) {
    pucResult = xmlStrdup(pucArg);
  }
  else {
    xmlChar *pucT;

    /*!\todo insert backslashes to exisiting quotes first */

    pucT = xmlStrdup(pucArgChar);
    pucT = xmlStrcat(pucT,pucArg);
    pucT = xmlStrcat(pucT,pucArgChar);

    assert(xmlStrlen(pucT) > xmlStrlen(pucArg));
    pucResult = pucT;
  }
  return pucResult;
} /* end of StringAddPairQuotesNew() */


/*! \return TRUE if 'pucArg' starts and ends with single or double quotes
*/
BOOL_T
HasStringPairQuotes(xmlChar *pucArg)
{
  BOOL_T fResult = FALSE;

  if (STR_IS_NOT_EMPTY(pucArg)) {
    xmlChar *pucA;

    for (pucA = pucArg; isspace(*pucA); pucA++);

    if (*pucA == (xmlChar)'\'' || *pucA == (xmlChar)'\"') {
      xmlChar *pucB;

      for (pucB = pucArg + xmlStrlen(pucArg) - 1; pucB > pucArg && isspace(*pucB); pucB--);
      
      fResult = (*pucB == *pucA);
    }
  }
  return fResult;
} /* end of HasStringPairQuotes() */


/**
 * xmlStrnstr:
 * @str:  the xmlChar * array (haystack)
 * @l:    the length of array
 * @val:  the xmlChar to search (needle)
 *
 * a strstr for xmlChar's
 *
 * Returns the xmlChar * for the first occurrence or NULL.
 */

xmlChar *
Strnstr(const xmlChar *str, const int l, const xmlChar *val) {
    int n;
    int i;
    
    if (str == NULL) return(NULL);
    if (val == NULL) return(NULL);
    n = xmlStrlen(val);

    if (n == 0) return(BAD_CAST str);
    for (i=0; i<l && *str != 0; i++ ) { /* non input consuming */
	if (*str == *val) {
	  if (!xmlStrncmp(str, val, n)) return(BAD_CAST str);
	}
	str++;
    }
    return(BAD_CAST NULL);
}


/*! replace string at needle with string at replace in string at haystack

  \return pointer to the new concatenated string or NULL if not contained
*/
xmlChar *
ReplaceStr(const xmlChar *haystack, const xmlChar *needle, const xmlChar *replace)
{
  int l_needle;
  xmlChar *found;

  if (STR_IS_EMPTY(haystack) || STR_IS_EMPTY(needle)) {
    return NULL;
  }

  if (replace == NULL) {
    replace = BAD_CAST "";
  }

  l_needle = xmlStrlen(needle);
  assert(l_needle > 0);

  found = (xmlChar *) xmlStrstr(haystack,needle);
  if (found) {
    xmlChar *prefix;
    xmlChar *pucRelease;

    if (needle[0] == '&' && needle[1] == '\0' && found[1] == '#') {
      int i;

      for ( i = (found[2] == 'x') ? 3 : 2; ishexdigit(found[i]); i++) { }

      if (i > 2 && i < 9 && found[i] == ';') {
	/* end of XML entity */
	found = (xmlChar *) xmlStrstr(&found[i+1],needle);
      }
    }

    if (found) {
      xmlChar *postfix;

      postfix = ReplaceStr(&found[l_needle],needle,replace);
      if (postfix == NULL) {
        postfix = xmlStrdup(&found[l_needle]);
      }

      if (found - haystack > 0) {
        prefix = xmlStrndup(haystack, (int)(found - haystack));
        pucRelease = prefix;
        prefix = xmlStrncatNew(pucRelease,replace,xmlStrlen(replace));
        xmlFree(pucRelease);
        pucRelease = prefix;
        prefix = xmlStrncatNew(prefix,postfix,xmlStrlen(postfix));
        xmlFree(pucRelease);
      }
      else {
        /* found points to the start of haystack */
        prefix = xmlStrncatNew(replace,postfix,xmlStrlen(postfix));
      }
      xmlFree(postfix);
    }
    else {
      prefix = NULL;
    }

    return prefix;
  }
  return NULL;
}
/* end of ReplaceStr() */


/*! fprintf's log messages

\todo remove!!
 */
void
_printf__log(int level, const char *fmt, ...)
{
  va_list ap;

  if (level > level_set) { /*!\bug handle global variable */
    return;
  }

  assert(fmt != NULL);

  va_start(ap,fmt);

  //fprintf(stderr,"(%is) ",GetRunningTime());
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");

  va_end(ap);
#if defined(DEBUG) && defined(_WIN32)
  fflush(stderr);
#endif
}
/* end of _printf__log() */


/*! normalize spaces, line breaks etc. in arg string
*/
char *
NormalizeStringSpaces(char *pchArg)
{
  if (pchArg) {
    char *c, *d;
    BOOL_T fSkipped = FALSE;

    /*! skip leading spaces at c */
    for (c = pchArg; isspace(*c); c++) {}
    /*! */
    for (d = pchArg; *c; c++) {
      if (isspace(*c)) {
	if (fSkipped) {
	  /* char before was also skipped,  */
	}
	else {
	  *d = ' ';
	  d++;
	  fSkipped = TRUE;
	}
      }
      else {
	*d = *c;
	d++;
	fSkipped = FALSE;
      }
    }
    /*! remove trailing spaces at d */
    do {
      *d = '\0';
    } while (d-- > pchArg && isspace(*d));
  }
  return pchArg;
} /* end of NormalizeStringSpaces() */


/*! normalize line breaks in arg string
*/
char *
NormalizeStringNewLines(char *pchArg)
{
  char *pchResult = NULL;

  if (STR_IS_NOT_EMPTY(pchArg)) {
    char *pchA;
    char *pchB;

    for (pchResult=pchA=pchB=pchArg; ! isend(*pchA); pchA++) {
      if (*pchA == '\r') {
      }
      else {
	*pchB = *pchA;
	pchB++;
      }
    }
    *pchB = (char)'\0';
  }

  return pchResult;
}
/* end of NormalizeStringNewLines() */


/*! remove the line ends similar to perl's chomp()
*/
int
chomp(unsigned char *c)
{
  int iResult = 0;
  if (c) {
    unsigned char *a;
    unsigned char *b;

    for ( a=b=c; *b==(unsigned char)' '; b++) { }

    for ( ; *b != (unsigned char)'\0'; b++) {
      switch (*b) {
      case '\n':
      case '\r':
	break;
      default:
	*a = *b;
	a++;
      }
    }
    *a = (unsigned char)'\0';
    for ( a--; a>c && *a==(unsigned char)' '; a--) {
      *a = (unsigned char)'\0';
    }
    iResult = (int)(a-c);
  }
  return iResult;
}
/* end of chomp() */

/*
 * Base 64 encoding/decoding (s. http://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64)
 *
 * */

int
base64encode(const void* data_buf, size_t dataLength, char* result, size_t resultSize)
{
   const char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
   const uint8_t *data = (const uint8_t *)data_buf;
   size_t resultIndex = 0;
   size_t x;
   uint32_t n = 0;
   int padCount = dataLength % 3;
   uint8_t n0, n1, n2, n3;

   if(data_buf == NULL ) return 0;   /* indicate failure: no data buffer at all */
   if(result == NULL ) return 0;   /* indicate failure: no result buffer at all */

   /* increment over the length of the string, three characters at a time */
   for (x = 0; x < dataLength; x += 3)
   {
      /* these three 8-bit (ASCII) characters become one 24-bit number */
      n = data[x] << 16;

      if((x+1) < dataLength)
         n += data[x+1] << 8;

      if((x+2) < dataLength)
         n += data[x+2];

      /* this 24-bit number gets separated into four 6-bit numbers */
      n0 = (uint8_t)(n >> 18) & 63;
      n1 = (uint8_t)(n >> 12) & 63;
      n2 = (uint8_t)(n >> 6) & 63;
      n3 = (uint8_t)n & 63;

      /*
       * if we have one byte available, then its encoding is spread
       * out over two characters
       */
      if(resultIndex >= resultSize) return 0;   /* indicate failure: buffer too small */
      result[resultIndex++] = base64chars[n0];
      if(resultIndex >= resultSize) return 0;   /* indicate failure: buffer too small */
      result[resultIndex++] = base64chars[n1];

      /*
       * if we have only two bytes available, then their encoding is
       * spread out over three chars
       */
      if((x+1) < dataLength)
      {
         if(resultIndex >= resultSize) return 0;   /* indicate failure: buffer too small */
         result[resultIndex++] = base64chars[n2];
      }

      /*
       * if we have all three bytes available, then their encoding is spread
       * out over four characters
       */
      if((x+2) < dataLength)
      {
         if(resultIndex >= resultSize) return 0;   /* indicate failure: buffer too small */
         result[resultIndex++] = base64chars[n3];
      }
   }

   /*
    * create and add padding that is required if we did not have a multiple of 3
    * number of characters available
    */
   if (padCount > 0)
   {
      for (; padCount < 3; padCount++)
      {
         if(resultIndex >= resultSize) return 0;   /* indicate failure: buffer too small */
         result[resultIndex++] = '=';
      }
   }
   if(resultIndex >= resultSize) return 0;   /* indicate failure: buffer too small */
   result[resultIndex] = 0;
   return 1;   /* indicate success */
}


/*!
 *
 * */

#define WHITESPACE 64
#define EQUALS     65
#define INVALID    66

static const unsigned char d[] = {
    66,66,66,66,66,66,66,66,66,64,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,62,66,66,66,63,52,53,
    54,55,56,57,58,59,60,61,66,66,66,65,66,66,66, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,66,66,66,66,66,66,26,27,28,
    29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66
};

/* see also libxml2 xmlBase64Decode() */

int
base64decode(char *in, size_t inLen, unsigned char *out, size_t *outLen)
{
    char *end = in + inLen;
    size_t buf = 1, len = 0;

    if(in == NULL ) return 0;   /* indicate failure: no data buffer at all */
    if(out == NULL ) return 0;   /* indicate failure: no result buffer at all */
    if(outLen == NULL ) return 0;   /* indicate failure: no result length pointer */

    while (in < end) {
        unsigned char c = d[*in++];

        switch (c) {
        case WHITESPACE: continue;   /* skip whitespace */
        case INVALID:    return 1;   /* invalid input, return error */
        case EQUALS:                 /* pad character, end of data */
            in = end;
            continue;
        default:
            buf = buf << 6 | c;

            /* If the buffer is full, split it into bytes */
            if (buf & 0x1000000) {
                if ((len += 3) > *outLen) return 1; /* buffer overflow */
                *out++ = buf >> 16;
                *out++ = buf >> 8;
                *out++ = buf;
                buf = 1;
            }
        }
    }

    if (buf & 0x40000) {
        if ((len += 2) > *outLen) return 1; /* buffer overflow */
        *out++ = buf >> 10;
        *out++ = buf >> 2;
    }
    else if (buf & 0x1000) {
        if (++len > *outLen) return 1; /* buffer overflow */
        *out++ = buf >> 4;
    }

    *outLen = len; /* modify to reflect the actual output size */
    return 0;
}


/*! detect latitude and longitude from an ISO 6709 formatted string

  \param pchArg
  \param pdArgLatitude
  \param pdArgLongitude

  \return 1 in case of error
 */
int 
GetPositionISO6709(const char *pchArg,double *pdArgLatitude,double *pdArgLongitude)
{
  int iResult = 1; /* error by default */
  char *pchT;
  double dValue;
  int iDegree;
  int iMinute;
  double dMinute;
  double dSecond;

  if (pchArg != NULL && strlen(pchArg) > 4 && pdArgLatitude != NULL && pdArgLongitude != NULL) {

    *pdArgLatitude = 0.0f;
    *pdArgLongitude = 0.0f;

    /* Latitude */
    dValue = strtod(pchArg,&pchT);
    if (fabs(dValue) < 90.0) {
      *pdArgLatitude = dValue; /* "�DD.D" */
      iResult = 0;
    }
    else if (fabs(dValue) < 1.0e4) {
      iDegree = ((int)floor(dValue)) / 100; /* "�DDMM.M" */
      if (abs(iDegree) < 90) {
	dMinute = dValue - iDegree * 100;
	if (fabs(dMinute) < 60.0) {
	  *pdArgLatitude = iDegree + dMinute / 60.0;
	  iResult = 0;
	}
      }
    }
    else if (fabs(dValue) < 1.0e6) {
      iDegree = ((int)floor(dValue)) / 10000; /* "�DDMMSS.S" */
      if (abs(iDegree) < 90) {
	iMinute = ((int)floor(dValue - (iDegree * 10000)) / 100);
	if (abs(iMinute) < 60) {
	  dSecond = dValue - iDegree * 10000 - iMinute * 100;
	  if (fabs(dSecond) < 60.0) {
	    *pdArgLatitude = iDegree + iMinute / 60.0 + dSecond / 3600.0;
	    iResult = 0;
	  }
	}
      }
    }

    if (iResult == 0 && (*pchT == '+' || *pchT == '-')) {
      /* Longitude */
      dValue = strtod(pchT,&pchT);
      if (fabs(dValue) < 180.0) {
	*pdArgLongitude = dValue; /* "�DDD.D" */
	iResult = 0;
      }
      else if (fabs(dValue) < 1.0e5) {
	iDegree = ((int)floor(dValue)) / 100; /* "�DDDMM.M" */
	if (abs(iDegree) < 180) {
	  dMinute = dValue - iDegree * 100;
	  if (fabs(dMinute) < 60.0) {
	    *pdArgLongitude = iDegree + dMinute / 60.0;
	    iResult = 0;
	  }
	}
      }
      else if (fabs(dValue) < 1.0e7) {
	iDegree = ((int)floor(dValue)) / 10000; /* "�DDDMMSS.S" */
	if (abs(iDegree) < 180) {
	  iMinute = ((int)floor(dValue - (iDegree * 10000)) / 100);
	  if (abs(iMinute) < 60) {
	    dSecond = dValue - iDegree * 10000 - iMinute * 100;
	    if (fabs(dSecond) < 60.0) {
	      *pdArgLongitude = iDegree + iMinute / 60.0 + dSecond / 3600.0;
	      iResult = 0;
	    }
	  }
	}
      }
    }
  }

  return iResult;
}
/* end of GetPositionISO6709() */


/**
 * derived from xmlCopyCharMultiByte()

 * @val:  the char value
 *
 * Returns the pointer to an array of xmlChar
 */
xmlChar *
GetUTF8Bytes(int val)
{
  xmlChar *out;

  out = BAD_CAST xmlMalloc(10);
  if (out) {
    /*
     * We are supposed to handle UTF8, check it's valid
     * From rfc2044: encoding of the Unicode values on UTF-8:
     *
     * UCS-4 range (hex.)           UTF-8 octet sequence (binary)
     * 0000 0000-0000 007F   0xxxxxxx
     * 0000 0080-0000 07FF   110xxxxx 10xxxxxx
     * 0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx
     */
    if  (val >= 0x80) {
      xmlChar *savedout = out;
      int bits;

      if (val < 0x800) {
	*out++= (val >>  6) | 0xC0;
	bits=  0;
      }
      else if (val < 0x10000) {
	*out++= (val >> 12) | 0xE0;
	bits=  6;
      }
      else if (val < 0x110000) {
	*out++= (val >> 18) | 0xF0;
	bits=  12;
      }
      else {
	PrintFormatLog(1,"Internal error, GetUTF8Bytes(0x%X) out of bound\n",val);
	xmlFree(savedout);
	return NULL;
      }
      for ( ; bits >= 0; bits-= 6) {
	*out++= ((val >> bits) & 0x3F) | 0x80 ;
      }
      savedout[out - savedout] = (xmlChar) '\0'; /* string termination */
      out = savedout;
    }
    else {
      *out = (xmlChar) val;
      out[1] = (xmlChar) '\0';
    }
  }
  return out;
}
/* end of GetUTF8Bytes() */


/*
* Converts a ascii hex code into a binary character.
*/
static int
fromhex(char ch)
{
  if (ch >= '0' && ch <= '9')
    return ch - '0';
  if (ch >= 'a' && ch <= 'f')
    return ch - 'a' + 10;
  if (ch >= 'A' && ch <= 'F')
    return ch - 'A' + 10;
  return -1;
}

/*
*  rfc1738_unescape() - Converts escaped characters (%xy numbers) in
*  given the string.  %% is a %. %ab is the 8-bit hexadecimal number "ab"
*/
void
DecodeRFC1738(char *s)
{
  int i, j;			/* i is write, j is read */
  for (i = j = 0; s[j]; i++, j++) {
    s[i] = s[j];
    if (s[j] != '%') {
      /* normal case, nothing more to do */
    }
    else if (s[j + 1] == '%') {	/* %% case */
      j++;		/* Skip % */
    }
    else {
      /* decode */
      int v1, v2, x;
      v1 = fromhex(s[j + 1]);
      if (v1 < 0)
	continue;  /* non-hex or \0 */
      v2 = fromhex(s[j + 2]);
      if (v2 < 0)
	continue;  /* non-hex or \0 */
      x = v1 << 4 | v2;
      if (x > 0 && x <= 255) {
	s[i] = x;
	j += 2;
      }
    }
  }
  s[i] = '\0';
}


/**
* EncodeRFC1738:
* @doc:  the document containing the string
* @input:  A string to convert to XML.
*
* Derived from libxml2-2.6.26/entities.c: xmlEncodeEntitiesReentrant()
*
* Do a global encoding of a string, replacing the predefined entities
* and non ASCII values with their entities and CharRef counterparts.
* Contrary to xmlEncodeEntities, this routine is reentrant, and result
* must be deallocated.
*
* Returns A newly allocated string with the substitution done.

s. https://en.wikipedia.org/wiki/Percent-encoding
*/
xmlChar *
EncodeRFC1738(const xmlChar *input) {
  const xmlChar *cur = input;
  xmlChar *buffer = NULL;
  xmlChar *out = NULL;
  int buffer_size = 0;
  //int html = 0;

  if (input == NULL) return(NULL);

  assert(xmlStrlen(input) < BUFFER_LENGTH);
  
  /*
  * allocate an translation buffer.
  */
  buffer_size = BUFFER_LENGTH;
  buffer = (xmlChar *)xmlMalloc(buffer_size * sizeof(xmlChar));
  if (buffer) {

    out = buffer;

    while (*cur != '\0') {
      if (out - buffer > (buffer_size - BUFFER_LENGTH / 10)) {
	int indx = (int)(out - buffer);

	//growBufferReentrant();
	buffer_size *= 2;
	buffer = (xmlChar *)xmlRealloc(buffer, buffer_size * sizeof(xmlChar));
	if (buffer) {
	  out = &buffer[indx];
	}
	else {
	  break;
	}
      }

      /*
      *  RFC 1738 defines that these characters should be escaped, as well
      *  any non-US-ASCII character or anything between 0x00 - 0x1F.
      */

#if 1
      if ((*cur >= '0' && *cur <= '9')
	|| (*cur >= 'A' && *cur <= 'Z')
	|| (*cur >= 'a' && *cur <= 'z')
	|| *cur == ','
	|| *cur == '-'
	|| *cur == '/'
	|| *cur == '_'
	|| *cur == '.') {
	/*
	* default case, just copy !
	*/
	*out++ = *cur;
      }
#if 1
      else if (*cur == '\\') {
	/*
	* WORKAROUND: transform backslashes into slashes !!!
	*/
	*out++ = '/';
      }
#endif
      else if (cur[0] == '&' && cur[1] == 'a' && cur[2] == 'm' && cur[3] == 'p' && cur[4] == ';') {
	/*
	* WORKAROUND: transform &amp; entity !!!
	*/
	*out++ = '%';
	*out++ = '2';
	*out++ = '6';
	cur += 4;
      }
      else if (cur[0] == '&' && cur[1] == 'a' && cur[2] == 'p' && cur[3] == 'o' && cur[4] == 's' && cur[5] == ';') {
	/*
	* WORKAROUND: transform &apos; entity !!!
	*/
	*out++ = '%';
	*out++ = '2';
	*out++ = '7';
	cur += 5;
      }
      else if (cur[0] == '&' && cur[1] == 'q' && cur[2] == 'u' && cur[3] == 'o' && cur[4] == 't' && cur[5] == ';') {
	/*
	* WORKAROUND: transform &quot; entity !!!
	*/
	*out++ = '%';
	*out++ = '2';
	*out++ = '2';
	cur += 5;
      }
      else {
	char buf[11], *ptr;

#if 0
	if (*cur == '\\') {
	  /*
	  * WORKAROUND: transform backslashes like slashes !!!
	  */
	  xmlStrPrintf(buf, sizeof(buf), "%%%X", '/');
	}
	else {
	  xmlStrPrintf(buf, sizeof(buf), "%%%X", *cur);
	}
#else
	xmlStrPrintf((xmlChar *)buf, sizeof(buf), "%%%X", *cur);
#endif
	buf[sizeof(buf) - 1] = 0;

	for (ptr = buf; *ptr != '\0'; out++, ptr++) {
	  *out = *ptr;
	}
      }
#else
      /*
      RFC 1738 unsafe chars
      */
      if (*cur == 0x3C) { /* < */
	*out++ = '%';
	*out++ = '3';
	*out++ = 'C';
      }
      else if (*cur == 0x3E) { /* > */
	*out++ = '%';
	*out++ = '3';
	*out++ = 'E';
      }
      else if (*cur == 0x22) { /* " */
	*out++ = '%';
	*out++ = '2';
	*out++ = '2';
      }
      else if (*cur == 0x23) { /* # */
	*out++ = '%';
	*out++ = '2';
	*out++ = '3';
      }
      else if (*cur == 0x25) { /* % */
	*out++ = '%';
	*out++ = '2';
	*out++ = '5';
      }
      else if (*cur == 0x7B) { /* { */
	*out++ = '%';
	*out++ = '7';
	*out++ = 'B';
      }
      else if (*cur == 0x7D) { /* } */
	*out++ = '%';
	*out++ = '7';
	*out++ = 'D';
      }
      else if (*cur == 0x7C) { /* | */
	*out++ = '%';
	*out++ = '7';
	*out++ = 'C';
      }
      else if (*cur == 0x5C) { /* \ */
	*out++ = '%';
	*out++ = '5';
	*out++ = 'C';
      }
      else if (*cur == 0x5E) { /* ^ */
	*out++ = '%';
	*out++ = '5';
	*out++ = 'E';
      }
      else if (*cur == 0x7E) { /* ~ */
	*out++ = '%';
	*out++ = '7';
	*out++ = 'E';
      }
      else if (*cur == 0x5B) { /* [ */
	*out++ = '%';
	*out++ = '5';
	*out++ = 'B';
      }
      else if (*cur == 0x5D) { /* ] */
	*out++ = '%';
	*out++ = '5';
	*out++ = 'D';
      }
      else if (*cur == 0x60) { /* ` */
	*out++ = '%';
	*out++ = '6';
	*out++ = '0';
      }
      else if (*cur == 0x27) { /* ' */
	*out++ = '%';
	*out++ = '2';
	*out++ = '7';
      }
      else if (*cur == 0x20) { /* space */
	*out++ = '%';
	*out++ = '2';
	*out++ = '0';
      }
      /*
      RFC 1738 reserved chars
      */
      else if (*cur == 0x3b) { /* ; */
	*out++ = '%';
	*out++ = '3';
	*out++ = 'B';
#if 0
      }
      else if (*cur == 0x2f) { /* / */
	*out++ = '%';
	*out++ = '2';
	*out++ = 'F';
#endif
      }
      else if (*cur == 0x3f) { /* ? */
	*out++ = '%';
	*out++ = '3';
	*out++ = 'F';
      }
      else if (*cur == 0x3a) { /* : */
	*out++ = '%';
	*out++ = '3';
	*out++ = 'A';
      }
      else if (*cur == 0x40) { /* @ */
	*out++ = '%';
	*out++ = '4';
	*out++ = '0';
      }
      else if (*cur == 0x3d) { /* = */
	*out++ = '%';
	*out++ = '3';
	*out++ = 'D';
      }
      else if (*cur == 0x26) { /* & */
	*out++ = '%';
	*out++ = '2';
	*out++ = '6';
      }
      /*  */
      else if (((*cur >= 0x20) && (*cur < 0x80)) ||
	(*cur == '\n') || (*cur == '\t') || ((html) && (*cur == '\r'))) {
	/*
	* default case, just copy !
	*/
	*out++ = *cur;
      }
      else if (*cur >= 0x80) {
#if 0
	if (((doc != NULL) && (doc->encoding != NULL)) || (html)) {
	  /*
	  * Bjørn Reese <br@sseusa.com> provided the patch
	  xmlChar xc;
	  xc = (*cur & 0x3F) << 6;
	  if (cur[1] != 0) {
	  xc += *(++cur) & 0x3F;
	  *out++ = xc;
	  } else
	  */
	  *out++ = *cur;
	}
	else {
#endif
	  /*
	  * We assume we have UTF-8 input.
	  */
	  char buf[11], *ptr;
	  int val = 0, l = 1;

	  if (*cur < 0xC0) {
	    PrintFormatLog(1, "EncodeRFC1738(): input not UTF-8");
	    xmlStrPrintf(buf, sizeof(buf), "%%%d", *cur);
	    buf[sizeof(buf) - 1] = 0;
	    ptr = buf;
	    while (*ptr != 0) *out++ = *ptr++;
	    cur++;
	    continue;
	  }
	  else if (*cur < 0xE0) {
	    val = (cur[0]) & 0x1F;
	    val <<= 6;
	    val |= (cur[1]) & 0x3F;
	    l = 2;
	  }
	  else if (*cur < 0xF0) {
	    val = (cur[0]) & 0x0F;
	    val <<= 6;
	    val |= (cur[1]) & 0x3F;
	    val <<= 6;
	    val |= (cur[2]) & 0x3F;
	    l = 3;
	  }
	  else if (*cur < 0xF8) {
	    val = (cur[0]) & 0x07;
	    val <<= 6;
	    val |= (cur[1]) & 0x3F;
	    val <<= 6;
	    val |= (cur[2]) & 0x3F;
	    val <<= 6;
	    val |= (cur[3]) & 0x3F;
	    l = 4;
	  }
	  if ((l == 1) || (!IS_CHAR(val))) {
	    PrintFormatLog(1,"EncodeRFC1738(): char out of range\n");
	    xmlStrPrintf(buf, sizeof(buf), "%%%d", *cur);
	    buf[sizeof(buf) - 1] = 0;
	    ptr = buf;
	    while (*ptr != 0) *out++ = *ptr++;
	    cur++;
	    continue;
	  }
	  /*
	  * We could do multiple things here. Just save as a char ref
	  */
	  xmlStrPrintf(buf, sizeof(buf), "%%%X", val);
	  buf[sizeof(buf) - 1] = 0;
	  ptr = buf;
	  while (*ptr != 0) *out++ = *ptr++;
	  cur += l;
	  continue;
#if 0
	}
#endif
      }
      else if (IS_BYTE_CHAR(*cur)) {
	char buf[11], *ptr;

	xmlStrPrintf(buf, sizeof(buf), "%%%d", *cur);
	buf[sizeof(buf) - 1] = 0;
	ptr = buf;
	while (*ptr != 0) *out++ = *ptr++;
      }
#endif
      cur++;
    }
    *out++ = 0;
  }

  return buffer;
}
/* end of EncodeRFC1738() */


/*! \return
*/
xmlChar *
EncodeBase64(const xmlChar *pucArg)
{
  return NULL;
}
/* end of EncodeBase64() */


/*! \return
*/
xmlChar *
DecodeBase64(const xmlChar *pucArg)
{
  int ret;
  char *pchT;
  size_t inlen;
  size_t outlen;

  inlen = xmlStrlen(pucArg);
  pchT = (char*)xmlMalloc(inlen * sizeof(char*));
  outlen = inlen;

  ret = base64decode((char*)pucArg, inlen, BAD_CAST pchT, &outlen);
  if (ret == 0) {
    PrintFormatLog(3, "Decoded '%i' byte to '%i'", inlen, outlen);
  }
  else {
    return NULL;
  }
  pchT[outlen] = '\0';	/* string termination? */

  return BAD_CAST pchT;
}
/* end of DecodeBase64() */


/*
*  Converts Quoted-Printable encoding (=xy numbers) in given the string.
* 
*  https://en.wikipedia.org/wiki/Quoted-printable
* 
* \return number of errors as negative integer or 0 if success
*/
int
DecodeQuotedPrintable(xmlChar* pucArg)
{
  int iResult = 0;

  if (pucArg) {
    index_t i;
    index_t j = 0;

    for (i=j=0; pucArg[i] != '\0'; i++, j++) {
      if (pucArg[i] == '=' && isalnum(pucArg[i+1]) && isalnum(pucArg[i+2])) {
	xmlChar k = 0;

	if (isdigit(pucArg[i+1])) {
	  k += (pucArg[i+1] - '0') * 16;
	}
	else if (pucArg[i+1] > '@' && pucArg[i+1] < 'G') {
	  k += (pucArg[i+1] - '@' + 9) * 16;
	}
	else {
	  perror("Quoted-Printable encoding error\n");
	  pucArg[j] = '_';
	  i += 2;
	  iResult--;
	  continue;
	}

	if (isdigit(pucArg[i+2])) {
	  k += (pucArg[i+2] - '0');
	}
	else if (pucArg[i+2] > '@' && pucArg[i+2] < 'G') {
	  k += (pucArg[i+2] - '@' + 9);
	}
	else {
	  perror("Quoted-Printable encoding error\n");
	  pucArg[j] = '_';
	  i += 2;
	  iResult--;
	  continue;
	}

	if (k > 0x1F && k < 0xFF) {
	  pucArg[j] = k;
	}
	else {
	  perror("Quoted-Printable encoding error\n");
	  pucArg[j] = '_';
	  iResult--;
	}
	i += 2;
      }
      else {
	pucArg[j] = pucArg[i];
      }
    }
    pucArg[j] = '\0';
  }
  else {
    //perror("Quoted-Printable encoding error\n");
    iResult--;
  }
  return iResult;
} /* end of DecodeQuotedPrintable() */


#ifdef _MSC_VER
// http://cboard.cprogramming.com/windows-programming/53507-mfc-open-directory-dialog-box.html
//
static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
  // If the BFFM_INITIALIZED message is received
  // set the path to the start path.
  switch (uMsg) {
  case BFFM_INITIALIZED:
    if (NULL != lpData) {
      SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
    }
  }

  return 0; // The function should always return 0.
}
#endif


// http://www.codeproject.com/Articles/2604/Browse-Folder-dialog-search-folder-and-all-sub-fol
// http://www.codeproject.com/Articles/16069/SHBrowseForFolder-with-Explorer-style-file-open-di
/*! Browse to an existing folder
* \return selected directory path UTF-8 encoded
*/
xmlChar *
GetSelectedFolderName(xmlChar *pucArgMsg, xmlChar *pucArgPath)
{
  xmlChar *pucResult = NULL;

#ifdef _MSC_VER
  LPITEMIDLIST pidl;
  TCHAR path[MAX_PATH];
  BROWSEINFO bi;
  TCHAR szDisplayName[MAX_PATH];

  CoInitialize(NULL);
  ZeroMemory(&bi, sizeof(bi));
  strncpy(szDisplayName, (const char *)pucArgPath, MAX_PATH);
  bi.pszDisplayName = (LPSTR)szDisplayName;
  bi.hwndOwner = NULL;
  bi.pidlRoot = NULL;
  if (pucArgMsg) {
    bi.lpszTitle = (LPCSTR)pucArgMsg;
  }
  else {
    bi.lpszTitle = TEXT("Please select a Folder");
  }
  bi.ulFlags = BIF_EDITBOX | BIF_VALIDATE | BIF_USENEWUI | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
  bi.lpfn = BrowseCallbackProc;
  bi.lParam = (LPARAM)pucArgPath;
  bi.iImage = 0;
  pidl = SHBrowseForFolder(&bi);
  if (pidl) {
    path[0] = '\0';
    SHGetPathFromIDList(pidl, path);
    if (path[0] != '\0') {
      pucResult = xmlStrdup(BAD_CAST path);
    }
  }
  //bi.pszDisplayName = NULL;
  CoUninitialize();
#else
  PrintFormatLog(1, "No dialog '%s' possible", pucArgMsg);
  pucResult = xmlStrdup(pucArgPath);
#endif

  return pucResult;
}
/* end of GetSelectedFolderName() */


/*! Browse to an existing file
*
* \return selected file path UTF-8 encoded
*/
xmlChar *
GetSelectedFileName(xmlChar *pucArgMsg, xmlChar *pucArgPath)
{
  xmlChar *pucResult = NULL;

#ifdef _MSC_VER
  OPENFILENAME ofn;       // common dialog box structure
  char szFile[260];       // buffer for file name
  HWND hwnd;              // owner window
  //HANDLE hf;              // file handle

  // Initialize OPENFILENAME
  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  //ofn.hwndOwner = hwnd;
  ofn.lpstrFile = szFile;
  // Set lpstrFile[0] to '\0' so that GetOpenFileName does not
  // use the contents of szFile to initialize itself.
  ofn.lpstrFile[0] = '\0';
  ofn.nMaxFile = sizeof(szFile);
  ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
  ofn.nFilterIndex = 1;
  if (pucArgMsg) {
    ofn.lpstrFileTitle = (LPSTR)pucArgMsg;
  }
  else {
    ofn.lpstrFileTitle = TEXT("Please select a File");
  }
  ofn.nMaxFileTitle = 0;
  ofn.lpstrInitialDir = (LPSTR)pucArgPath;
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;

  // Display the Open dialog box.

  if (GetOpenFileName(&ofn)) {
    if (ofn.lpstrFile[0] != '\0') {
      pucResult = xmlStrdup(BAD_CAST ofn.lpstrFile);
    }
  }
#else
  PrintFormatLog(1, "No dialog '%s' possible", pucArgMsg);
  pucResult = xmlStrdup(pucArgPath);
#endif
  return pucResult;
}
/* end of GetSelectedFileName() */


/*! computes the number of days since 1970-01-01 as an absolute value
*/
long int
GetDayAbsolute(int year, int mon, int mday, int week, int wday)
{
  long int result = 0;

  if (year>1969) {
    struct tm t;
    t.tm_year = year - 1900;

    if (mon > 0 && mday > 0 && mday < 32) {
      t.tm_yday = 0;
      t.tm_mon = mon - 1;
      t.tm_mday = mday;
      t.tm_wday = 0;
      t.tm_hour = 0;
      t.tm_min = 0;
      t.tm_sec = 0;
      t.tm_isdst = 0;
      result = (long int)(mktime(&t) / (time_t)(60 * 60 * 24));
    }
    else if (week > -1 && week < 54 && wday > -1 && wday < 7) {

      /* starting with the very first day of year */
      t.tm_yday = 0;
      t.tm_mon = 0;
      t.tm_mday = 1;
      t.tm_wday = 0;
      t.tm_hour = 0;
      t.tm_min = 0;
      t.tm_sec = 0;
      t.tm_isdst = 0;
      result = (long int)(mktime(&t) / (time_t)(60 * 60 * 24));

      if (week == 0 && t.tm_wday > 0 && t.tm_wday < 5) {
	PrintFormatLog(1, "There is no week '0' in year '%i'", year);
	return -1;
      }

      /* offset for very first week of year */
      switch (t.tm_wday) {
      case 0: 		/* sun */
	result += 1;
	break;
      case 1: 		/* mon */
	break;
      case 2: 		/* tue */
	result -= 1;
	break;
      case 3: 		/* wed */
	result -= 2;
	break;
      case 4: 		/* thu */
	result -= 3;
	break;
      case 5: 		/* fri */
	result += 3;
	break;
      case 6: 		/* sat */
	result += 2;
	break;
      }

      /* add days of all complete weeks */
      result += (week - 1) * 7;

      /* add days of according week */
      switch (wday) {
      case 0: 		/* sun */
	result += 6;
	break;
      case 1: 		/* mon */
      case 2: 		/* tue */
      case 3: 		/* wed */
      case 4: 		/* thu */
      case 5: 		/* fri */
      case 6: 		/* sat */
	result += wday - 1;
	break;
      }
    }
  }
  return result;
}
/* end of GetDayAbsolute() */


/* compute the sequential number of a day in the year
(1-Jan = 1, 31-Dec = 365/366)
J.D. Robertson: Remark on Algorithm 398,
Comm. ACM 13, 10 (Oct. 1972), p. 918
*/
int GetDayOfYear(int day, int month, int year)
{
  int  lmon; /* derived from month */

  lmon = (month + 10) / 13;
  return (((long) 3055 * (long) (month + 2)) / 100 - 2 * lmon - 91
    + (1
    - (year - (year /   4) *   4 +   3) /   4
    + (year - (year / 100) * 100 +  99) / 100
    - (year - (year / 400) * 400 + 399) / 400
    ) * lmon + day);
}
/* end of GetDayOfYear() */


/* compute the day in the week,
1 = Monday, 7 = Sunday
J.D. Robertson: Remark on Algorithm 398,
Comm. ACM 13, 10 (Oct. 1972), p. 918
"Zeller's congruence"
*/
int GetDayOfWeek(int day, int month, int year)
{
  int  lmon; /* derived from month */
  int  mmon; /* derived from month */

  lmon = month + 10;
  mmon = (month - 14) / 12 + year;
  return ((((13 * (lmon - (lmon /  13) *  12) - 1) / 5 + day + 77
    + 5 * (mmon - (mmon / 100) * 100) / 4
    + mmon / 400 - (mmon / 100) *   2)
    - 1) % 7 + 1
    );
}
/* end of GetDayOfWeek() */


/* Compute the week number (0..52) for a given date.
The weeks start at monday, and some days at the beginning
of January may be in week "0"
which means week 52 of the previous year.
*/
int
GetWeekOfYear(int day, int month, int year)
{
#if 1
  // http://www.nord-com.net/h-g.mekelburg/kalender/kal-64.htm
  int Woche = 0;
  int Wchtag1Jan = GetDayOfWeek(1, 1, year) - 1;
  int Tage = GetDayOfYear(day, month, year) - 1;

  if (Wchtag1Jan > 3)
    Tage = Tage - (7 - Wchtag1Jan);
  else Tage = Tage + Wchtag1Jan;

  if (Tage < 0)
    if ((Wchtag1Jan == 4)
      || (GetDayOfWeek(1, 1, year - 1) - 1 == 3))
      Woche = 53;
    else Woche = 52;
  else Woche = (int)floor((float)Tage / 7.0f) + 1;

  if ((Tage > 360) && (Woche > 52)) {
    if (Wchtag1Jan == 3)
      Woche = 53;
    else if (GetDayOfWeek(1, 1, year + 1) - 1 == 4)
      Woche = 53;
    else Woche = 1;
  }
  return Woche;
#else
  int  wjan1; /* week day of January 1st */
  int  day_num; /* number of day in year */

  wjan1 = GetDayOfWeek(1, 1, year);
  day_num = GetDayOfYear(day, month, year);
  if (wjan1 > 1)
    day_num -= (8 - wjan1);
  if (day_num <= 0)
    return 0; /* before 1st Monday in January */
  else
    return (day_num - 1) / 7 + 1;
#endif
}
/* end of GetWeekOfYear() */


/*! \return the date of easter sunday in given year

  s.http://www.ptb.de/cms/fachabteilungen/abt4/fb-44/ag-441/darstellung-der-gesetzlichen-zeit/wann-ist-ostern.html

  (1)  K = INT(X / 100);

  (2)  M = 15 + INT((3K + 3) / 4) - INT((8K + 13) / 25);

  (3)  S = 2 - INT((3K + 3) / 4);

  (4)  A = MOD(X, 19);

  (5)  D = MOD(19A + M, 30);

  (6)  R = INT(D / 29) + (INT(D / 28) - INT(D / 29)) * INT(A / 11);

  (7)  OG = 21 + D - R;

  (8)  SZ = 7 - MOD(X + INT(X / 4) + S, 7);

  (9)  OE = 7 - MOD(OG - SZ, 7);

  OG ist das Märzdatum des Ostervollmonds.Dies entspricht dem 14. Tag des ersten Monats im Mondkalender, genannt Nisanu. SZ ist das Datum des ersten Sonntags im März.

  OS = OG + OE ist das Datum des Ostersonntags, als Datum im Monat März dargestellt. (Der 32. März entspricht also dem 1. April, usw.)

*/
long int
GetEasterSunday(int iArgYear, int *piArgMonth, int *piArgDay)
{
#if 1
  long int result = -1;
  struct tm t;

  int X = iArgYear;

  int K = (int)floor(X / 100.0f);

  int M = 15 + (int)floor((3 * K + 3) / 4.0f) - (int)floor((8 * K + 13) / 25.0f);

  int S = 2 - (int)floor((3 * K + 3) / 4.0f);

  int A = X % 19;

  int D = (19 * A + M) % 30;

  int R = (int)floor(D / 29.0f) + ((int)floor(D / 28.0f) - (int)floor(D / 29.0f)) * (int)floor(A / 11.0f);

  int OG = 21 + D - R;

  int SZ = 7 - ((X + (int)floor(X / 4.0f) + S) % 7);

  int OE = 7 - ((OG - SZ) % 7);

  t.tm_year = X - 1900;
  t.tm_yday = 0;
  t.tm_mon = 2;
  t.tm_mday = OG + OE;
  t.tm_wday = 0;
  t.tm_hour = 0;
  t.tm_min = 0;
  t.tm_sec = 0;
  t.tm_isdst = 0;
  result = (long int)(mktime(&t) / (time_t)(60 * 60 * 24));

  if (piArgMonth) {
    *piArgMonth = t.tm_mon + 1;
  }

  if (piArgDay) {
    *piArgDay = t.tm_mday;
  }

  return result;
#else
  int y, m, w, d, d_week;

  easter1(iArgYear, &d, &m);
  return GetDayAbsolute(iArgYear, m, d, -1, -1);
#endif
}
/* end of GetEasterSunday() */


/*! a subset of linux date command format

%d     day of month (e.g., 01)

%H     hour (00..23)

%j     day of year (001..366)

%m     month (01..12)

%M     minute (00..59)

%S     second (00..60)

%s     Unix epoch seconds

%V     ISO week number, with Monday as first day of week (01..53)

%Y     year

\param pucArgFormat pointer to a date like format string
\return pointer to a new allocated copy of pucArgFormat, format sequences replaced by its values
*/
xmlChar *
GetNowFormatStr(xmlChar *pucArgFormat)
{
  xmlChar *pucResult = NULL;

  if (pucArgFormat != NULL && xmlStrlen(pucArgFormat) > 0) {

    if (xmlStrcasecmp(pucArgFormat, BAD_CAST "ISO") == 0) {
      pucResult = GetNowFormatStr(BAD_CAST "%T");
    }
    else if (xmlStrcasecmp(pucArgFormat,BAD_CAST "Year") == 0) {
      pucResult = GetNowFormatStr(BAD_CAST "%Y");
    }
    else if (xmlStrcasecmp(pucArgFormat, BAD_CAST "NumOfMonth") == 0) {
      pucResult = GetNowFormatStr(BAD_CAST "%m");
    }
    else if (xmlStrcasecmp(pucArgFormat, BAD_CAST "DayOfMonth") == 0) {
      pucResult = GetNowFormatStr(BAD_CAST "%d");
    }
    else if (xmlStrcasecmp(pucArgFormat, BAD_CAST "NumOfWeek") == 0) {
      /*\bug substitute NumOfWeekIso and NumOfWeek */
      pucResult = GetNowFormatStr(BAD_CAST "%V");
    }
    else if (xmlStrcasecmp(pucArgFormat, BAD_CAST "NameOfMonth") == 0) {
      /*\bug substitute NameOfMonth */
      pucResult = GetNowFormatStr(BAD_CAST "%m");
    }
    else if (xmlStrcasecmp(pucArgFormat, BAD_CAST "Day") == 0) {
      pucResult = GetNowFormatStr(BAD_CAST "%d");
    }
    else if (xmlStrcasecmp(pucArgFormat, BAD_CAST "Hour") == 0) {
      pucResult = GetNowFormatStr(BAD_CAST "%H");
    }
    else if (xmlStrcasecmp(pucArgFormat, BAD_CAST "Minute") == 0) {
      pucResult = GetNowFormatStr(BAD_CAST "%M");
    }
    else if (xmlStrcasecmp(pucArgFormat, BAD_CAST "Second") == 0) {
      pucResult = GetNowFormatStr(BAD_CAST "%S");
    }
    else {
      /* a format string */
      struct tm *pTime;
      time_t nowTime;

      time(&nowTime);

      pTime = localtime(&nowTime);
      if (pTime) {

	pucResult = xmlStrdup(pucArgFormat);
	if (pucResult) {
	  int i;
	  xmlChar mucBuffer[BUFFER_LENGTH];

	  for (i = 0, mucBuffer[0] = 0; pucResult[i]; i++) {

	    if (pucResult[i] == (xmlChar)'%') {
	      xmlChar *pucT;

	      switch (pucResult[i + 1]) {

	      case 'T':
		xmlStrPrintf(mucBuffer,
		  BUFFER_LENGTH,
		  "%i-%02i-%02iT%02i:%02i:%02i",
		  pTime->tm_year + 1900,
		  pTime->tm_mon + 1,
		  pTime->tm_mday,
		  pTime->tm_hour,
		  pTime->tm_min,
		  pTime->tm_sec);
		break;

	      case 'Y':
		xmlStrPrintf(mucBuffer, BUFFER_LENGTH, "%04i", pTime->tm_year + 1900);
		break;

	      case 'm':
		xmlStrPrintf(mucBuffer, BUFFER_LENGTH, "%02i", pTime->tm_mon + 1);
		break;

	      case 'd':
		xmlStrPrintf(mucBuffer, BUFFER_LENGTH, "%02i", pTime->tm_mday);
		break;

	      case 'j':
		xmlStrPrintf(mucBuffer, BUFFER_LENGTH, "%03i", pTime->tm_yday + 1);
		break;

	      case 'V':
		xmlStrPrintf(mucBuffer, BUFFER_LENGTH, "%02i", GetWeekOfYear(pTime->tm_mday, pTime->tm_mon + 1, pTime->tm_year + 1900));
		break;

	      case 'H':
		xmlStrPrintf(mucBuffer, BUFFER_LENGTH, "%02i", pTime->tm_hour);
		break;

	      case 'M':
		xmlStrPrintf(mucBuffer, BUFFER_LENGTH, "%02i", pTime->tm_min);
		break;

	      case 'S':
		xmlStrPrintf(mucBuffer, BUFFER_LENGTH, "%02i", pTime->tm_sec);
		break;

	      case 's':
		xmlStrPrintf(mucBuffer, BUFFER_LENGTH, "%lli", (long long)nowTime);
		break;

	      default:
		/* not defined pattern */
		break;
	      }

	      if (mucBuffer[0]) { /* there is a value for current pattern */

		if (i > 0) { /* there is a prefix */
		  pucT = xmlStrndup(pucResult, i);
		  pucT = xmlStrcat(pucT, mucBuffer);
		}
		else {
		  pucT = xmlStrdup(mucBuffer);
		}

		if (xmlStrlen(pucResult + i) > 2) { /* there is a postfix */
		  pucT = xmlStrcat(pucT, pucResult + i + 2);
		}

		xmlFree(pucResult);
		pucResult = pucT;
		mucBuffer[0] = 0;
		i++; /* to skip pattern */
	      }
	    }
	  }
	}
      }
    }
  }
  return pucResult;
}
/* end of GetNowFormatStr() */


/*! \return a string according to "ISO 8601 Data elements and
interchange formats – Information interchange – Representation of
dates and times" (http://en.wikipedia.org/wiki/ISO_8601)

\todo append timezone id
*/
xmlChar *
GetDateIsoString(time_t ArgTime)
{
  xmlChar *pucResult = NULL;
  struct tm *pTime;

  if (ArgTime < 1) {
    time(&ArgTime);
  }

  pTime = localtime(&ArgTime);
  if (pTime) {
    pucResult = BAD_CAST xmlMalloc(BUFFER_LENGTH * sizeof(char));
    if (pucResult) {
      xmlStrPrintf(pucResult,
	BUFFER_LENGTH,
	"%i-%02i-%02iT%02i:%02i:%02i",
	pTime->tm_year + 1900,
	pTime->tm_mon + 1,
	pTime->tm_mday,
	pTime->tm_hour,
	pTime->tm_min,
	pTime->tm_sec);
    }
  }

  return pucResult;
}
/* end of GetDateIsoString() */


/*! \return true if *pucArg is a valid char for hastags
*/
int
ishashtag(xmlChar* pucArg, int* piArg)
{
  BOOL_T fResult = FALSE;

  if (pucArg == NULL) {
  }
  else if (isalnum(*pucArg) || *pucArg == (xmlChar)'-' || *pucArg == (xmlChar)'_' || *pucArg == (xmlChar)'#' || *pucArg == (xmlChar)'@') {
    /* allowed single byte char */
    if (piArg) {
      *piArg = 1;
    }
    fResult = TRUE;
  }
  else if (*pucArg == (xmlChar)0xc3
    && (*(pucArg + 1) == (xmlChar)0x84
      || *(pucArg + 1) == (xmlChar)0xa4
      || *(pucArg + 1) == (xmlChar)0xb6
      || *(pucArg + 1) == (xmlChar)0x96
      || *(pucArg + 1) == (xmlChar)0xbc
      || *(pucArg + 1) == (xmlChar)0x9c
      || *(pucArg + 1) == (xmlChar)0x9f)
    ) {
    /* allowed multi byte char */
    if (piArg) {
      *piArg = 2;
    }
    fResult = TRUE;
  }
  return fResult;
} /* End of ishashtag() */


/*! \return true if c is a valid char for a date string.
The separator chars '#' and ',' are handled separately in iscalx()
*/
BOOL_T
iscal(xmlChar c)
{
  if (isdigit((int)c)
    || (c == '*') || (c == ':') || (c == '.') || (c == '+') || (c == '-') || (c == '@')
    || (c == 'm') || (c == 'o') || (c == 'n')
    || (c == 't') || (c == 'u') || (c == 'e')
    || (c == 'w') || (c == 'd')
    || (c == 'h')
    || (c == 'f') || (c == 'r') || (c == 'i')
    || (c == 's') || (c == 'a')
    ) {
    return TRUE;
  }
  return FALSE;
}
/* end of iscal() */


/*! \return true if c is a extended valid char for a date string.
*/
BOOL_T
iscalx(xmlChar c)
{
  if ((c == ',') || (c == '#')) {
    return TRUE;
  }
  return FALSE;
}
/* end of iscal() */


/*! makes elements content XML-conformant and
\param ppeT element to use
\return a new node pointer or NULL if failed
*/
xmlNodePtr
StringNodeSubst(xmlChar* pucArg)
{
  xmlNodePtr pndResult = NULL;

  if (STR_IS_NOT_EMPTY(pucArg)) {
    int p;
    int i;
    int j;
    int k;
    BOOL_T fRegexp;
    xmlChar* puc0 = NULL;
    xmlChar* puc1 = NULL;
    xmlChar* pucAttrName = NULL;
    xmlChar ucQuot = '\''; /* quote char */

    for (i = 0; pucArg[i] != '(' && !isend(pucArg[i]); i++); /* find opening '(' */

    for (j = xmlStrlen(pucArg); j > i && pucArg[j] != ')'; j--); /* find closing ')' */

    for (k = i + 1; isspace(pucArg[k]); k++); /* skip space chars */

    if ((fRegexp = (pucArg[k] == 'r'))) {
      k++;
    }

    for (p = 0; k < j; k++) { /* separate two parameters */
      if (pucArg[k] == '\'' || (pucArg[k] == '\"' && (ucQuot = '\"'))) {
	int l;

	for (k++, l = k; k < j && pucArg[k] != ucQuot; k++); /* find closing char */
	if (pucArg[k] == ucQuot) {
	  if (p == 0) {
	    puc0 = xmlStrndup(BAD_CAST &pucArg[l], k - l);
	    p++;
	  }
	  else if (p == 1) {
	    puc1 = xmlStrndup(BAD_CAST &pucArg[l], k - l);
	    break;
	  }
	}
      }
      else if (pucArg[k] == '=' && p == 1) { /* named subst attribute */
	int m;

	for (m = k - 1; m > i && isalpha(pucArg[m]); m--); /* */
	pucAttrName = xmlStrndup(BAD_CAST & pucArg[m+1], k - m - 1);
      }
    }

    if (STR_IS_NOT_EMPTY(puc0) && STR_IS_NOT_EMPTY(puc1)) {
      pndResult = xmlNewNode(NULL, NAME_SUBST);
      xmlSetProp(pndResult, BAD_CAST(fRegexp ? "regexp" : "string"), puc0);
      xmlSetProp(pndResult, BAD_CAST (STR_IS_NOT_EMPTY(pucAttrName) ? pucAttrName : "to"), puc1);
#if 0
      if (ppeT->iDepthHidden > 0) {
	xmlSetProp(pndResult, BAD_CAST "valid", BAD_CAST"no");
      }
#endif
      xmlFree(puc1);
      xmlFree(puc0);
    }
    xmlFree(pucAttrName);
  }

  return pndResult;
} /* end of StringNodeSubst() */


#ifdef TESTCODE
#include "test/test_utils.c"
#endif
