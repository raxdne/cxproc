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

#define LEVEL_MAX 4

/* 
   common header

*/

#define isend(C) (C == (xmlChar)'\0')

/****************************************************************************

   Declaration of public functions (prototypes)
*/

extern BOOL_T
ishexdigit(char chArg);

extern int
GetPositionISO6709(const char *pchArg,double *pdArgLatitude,double *pdArgLongitude);

extern int
base64encode(const void* data_buf, size_t dataLength, char* result, size_t resultSize);

extern int
base64decode(char *in, size_t inLen, unsigned char *out, size_t *outLen);

extern xmlChar *
ReadUTF8ToBufferNew(FILE* argin);

extern BOOL_T
StringToLower(char *pchArg);

extern BOOL_T
StringToUpper(char *pchArg);

extern BOOL_T
StringToId(char *pchArg);

extern BOOL_T
StringBeginsWith(char *pchArgBegin, const char *pchArgNeedle);

extern char*
StringEndsWith(char* pchArgBegin, const char* pchArgNeedle);

extern xmlChar *
StringReplaceUmlauteNew(const xmlChar *pucArg);

extern BOOL_T
StringRemovePairQuotes(xmlChar *pucArg);

extern BOOL_T
StringRemoveBackslashes(xmlChar* pucArg);

extern xmlChar *
StringAddPairQuotesNew(xmlChar *pucArg, xmlChar *pucArgChar);

extern BOOL_T
HasStringPairQuotes(xmlChar *pucArg);

extern xmlChar*
StringDecodeNumericCharsNew(xmlChar* pucArg);

extern xmlChar*
StringEncodeXmlDefaultEntitiesNew(xmlChar* pucArg);

extern xmlChar *
Strnstr(const xmlChar *str, const int l, const xmlChar *val);

extern xmlChar *
ReplaceStr(const xmlChar *haystack, const xmlChar *needle, const xmlChar *replace);

extern int
SetLogLevel(int level);

extern int
SetLogLevelStr(xmlChar *pucArg);

extern void
_printf__log(int level, const char *fmt, ...);

#define PrintFormatLog _printf__log


extern char *
NormalizeStringSpaces(char *pchArg);

extern char *
NormalizeStringNewLines(char *pchArg);

extern xmlChar *
EncodeRFC1738(const xmlChar *input);

extern void
DecodeRFC1738(char *s);

extern xmlChar *
EncodeBase64(const xmlChar *input);

extern xmlChar *
DecodeBase64(const xmlChar *input);

extern int
DecodeQuotedPrintable(xmlChar* pucArg);

extern xmlChar *
GetSelectedFolderName(xmlChar *pucArgMsg, xmlChar *pucArgPath);

extern xmlChar *
GetSelectedFileName(xmlChar *pucArgMsg, xmlChar *pucArgPath);


extern int
chomp(unsigned char *c);

extern xmlChar *
GetUTF8Bytes(int val);

extern long int
GetDayAbsoluteStr(xmlChar *pucGcal);

extern xmlChar *
GetDiffDaysStrNew(xmlChar *pucArgStart,xmlChar *pucArgEnd);

extern xmlChar *
GetDiffYearsStrNew(xmlChar *pucArgStart,xmlChar *pucArgEnd);

extern int
GetDayOfWeek(int day, int month, int year);

extern int
GetWeekOfYear(int day, int month, int year);

extern xmlChar *
GetNowFormatStr(xmlChar *pucArgFormat);

extern xmlChar *
GetDateIsoString(time_t ArgTime);

extern size_t
dt_parse_iso_recurrance(const char *str, size_t len, int* deltad);

extern size_t
dt_parse_iso_period(const char *str, size_t len, int *yp, int *mp, int *dp, int *wp);

extern int
ishashtag(xmlChar* pucArg, int* piArg);

extern BOOL_T
isiso8601(xmlChar c);

extern xmlChar*
StringConcatNextDate(xmlChar* pucArgGcal);

#ifdef TESTCODE
extern int
utilsTest(void);
#endif
