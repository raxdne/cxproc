/*
  cxproc - Configurable Xml PROCessor

  Copyright (C) 2006..2024 by Alexander Tenbusch

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

#include <pie/pie_dtd.h>
#include <pie/pie_element.h>

#ifdef WITH_MARKDOWN
#include <pie/pie_cmark.h>
#endif

#ifdef PIE_STANDALONE
#else
#include "calendar_element.h"
#include "plain_text.h"
#endif

#include <pie/pie_text_tags.h>

#define CXP_PIE_URL (BAD_CAST "http://www.tenbusch.info/pie")

/*! \todo German Umlaute in UNC */
/* s. "Regular Expressions Cookbook" by Jan Goyvaerts and Steven Levithan, Chapter "Validate Windows Paths"
   Published by O’Reilly Media, Inc.

   s. https://www.regular-expressions.info/
*/
#define RE_UNC "(?:\\b[a-z]:\\\\|\\\\{1,2}[a-zäÄöÖüÜß0-9_.$\\-]+\\\\[a-zäÄöÖüÜß0-9_.$\\-]+)\\\\*(?:[^\\\\/:*?\"<>|\\r\\n]+\\\\)*[^\\\\/:*?\"<>|\\r\\n]*"
#define RE_URL "(tel|onenote|file|http|https|ftp|ftps|mailto|smb|rtp)(://+|%%3A%%2F%%2F|:|%%3A)([a-zäÄöÖüÜß0-9\\.\\-\\&\\#\\;\\:\\,\\+\\_%%\\~\\?\\!=\\@]+|[a-zäÄöÖüÜß0-9\\.\\-]+@)([/a-zäÄöÖüÜß0-9\\(\\)\\.\\-\\&\\#\\;\\,\\+\\:\\_%%\\~\\*\\?\\!=\\@\\{\\}]+)*"
#define RE_LINK_MD "!*\\[([^\\]]*)\\]\\(([^\\)]+)\\)"
#define RE_LINK_MD_AUTO "(<|&lt;|\\xE2\\x80\\x99)([^<> \\t]+)(>|&gt;|\\xE2\\x80\\x98)"

/*!\bug extend regexp to markdown
*/
#define RE_FIG "^[ \\t]*(FIG|ABB)[\\. \\t]+([^ \\t]+)[ \\t]*(.+)*$"

/*
*/
#define RE_SCRIPT "\\bSCRIPT=\\\"([^\\\"]+)\\\""

/*
*/
#define RE_INLINE "_{2,}[^_]+_{2,}|\\*{2,}[^\\*]+\\*{2,}|`[^`]+`"

/*
*/
#if 0
  #define RE_TASK "^(TODO|DONE|REQ|BUG|TARGET|TEST)[: \\t]+"
#else
  #define RE_TASK "^(TODO|DONE|REQ|BUG|TARGET|TEST)(:[ \\t]*)"
  /*!\todo add agile (EPIC|US|UC|FEATURE) */
#endif


/*! s https://en.wikipedia.org/wiki/ISO_8601
      https://ijmacd.github.io/rfc3339-iso8601/
      https://rgxdb.com/r/MD2234J
*/

#define RE_DATE_YEAR          "[12][09][0-9][0-9]"
#define RE_DATE_MONTH         "[01][0-9]"
#define RE_DATE_QUATER        "-Q[1-4]"
#define RE_DATE_DAY_OF_MONTH  "[0123][0-9]"
#define RE_DATE_ORD           "[0-9]{3}"
#define RE_DATE_WEEK          "W[0-5][0-9]"
#define RE_DATE_DAY_OF_WEEK   "[1-7]"
#define RE_DATE_UNIX          "[0-9]{10}(\\.[0-9]{0,3})*"
#define RE_DATE_GERMAN        "[0123]*[0-9]" "\\." "[01]*[0-9]" "\\." "[0-9]{2,4}"


#define RE_ISO_DAY \
     "(" RE_DATE_YEAR "-" RE_DATE_MONTH "-" RE_DATE_DAY_OF_MONTH "|"  RE_DATE_YEAR RE_DATE_MONTH RE_DATE_DAY_OF_MONTH \
     "|" RE_DATE_YEAR "-" RE_DATE_WEEK "-" RE_DATE_DAY_OF_WEEK "|" RE_DATE_YEAR RE_DATE_WEEK RE_DATE_DAY_OF_WEEK \
     "|" RE_DATE_YEAR "-" RE_DATE_ORD "|" RE_DATE_YEAR RE_DATE_ORD \
     ")"

#ifdef USE_ISO_TIME
  #define RE_ISO_TIME      "T[012]*[0-9](:*[0-5][0-9]){0,2}([\\.][0-9]{1,10})*" "(((\\+|\\-|" STR_UTF8_MINUS ")[0-9]{1,2}([:\\.]*[0-9]{1,2})*)" "|" "[A-Z]{3}" "|" "Z" ")*"
  #define RE_ISO_DAY_TIME  RE_ISO_DAY "(" RE_ISO_TIME ")*"
  #ifdef USE_ISO_EXTENSION
    #define RE_ISO_PERIOD    "P(-*[0-9\\.]+[YMDW])*" "(T(-*[0-9\\.]+[HMS]))*"
  #else
    #define RE_ISO_PERIOD    "[PO](-*[0-9\\.]+[YMDW])*" "(T(-*[0-9\\.]+[HMS]))*"
  #endif
#else
  #define RE_ISO_DAY_TIME  RE_ISO_DAY
  #ifdef USE_ISO_EXTENSION
    #define RE_ISO_PERIOD    "[PO](-*[0-9\\.]+[YMDW])+"
  #else
    #define RE_ISO_PERIOD    "P(-*[0-9\\.]+[YMDW])+"
  #endif
#endif

#define RE_ISO_WEEK RE_DATE_YEAR "-*" RE_DATE_WEEK
     
#define RE_ISO_MONTH RE_DATE_YEAR "-*" RE_DATE_MONTH

#define RE_ISO_YEAR RE_DATE_YEAR

#define RE_ISO_QUATER RE_DATE_YEAR RE_DATE_QUATER

#define RE_ISO_RECURRENCE   "R[0-9]{0,2}"

/*!\todo define simple sub-set of ISO 8601 (no recurrence ...) */

#define RE_ISO_8601 \
  "(" RE_ISO_RECURRENCE "/)(" RE_ISO_DAY_TIME ")/(" RE_ISO_PERIOD ")" \
  "|"									\
  \
  "(" RE_ISO_PERIOD ")/(" RE_ISO_DAY_TIME ")" \
  "|"									\
  "(" RE_ISO_DAY_TIME ")/(" RE_ISO_PERIOD ")" \
  "|"									\
  "(" RE_ISO_DAY_TIME ")/(" RE_ISO_DAY_TIME ")" \
  "|"									\
  \
  "(" RE_ISO_WEEK ")/(" RE_ISO_WEEK ")" \
  "|"									\
  "(" RE_ISO_WEEK ")/(" RE_ISO_PERIOD ")" \
  "|"									\
  "(" RE_ISO_PERIOD ")/(" RE_ISO_WEEK ")" \
  "|"									\
  \
  "(" RE_ISO_MONTH ")/(" RE_ISO_MONTH ")" \
  "|"									\
  "(" RE_ISO_MONTH ")/(" RE_ISO_PERIOD ")" \
  "|"									\
  "(" RE_ISO_PERIOD ")/(" RE_ISO_MONTH ")" \
  "|"									\
  \
  "(" RE_ISO_QUATER ")/(" RE_ISO_QUATER ")" \
  "|"									\
  "(" RE_ISO_QUATER ")" \
  "|"									\
  \
  "(" RE_ISO_YEAR ")/(" RE_ISO_YEAR ")" \
  "|"									\
  "(" RE_ISO_YEAR ")/(" RE_ISO_PERIOD ")" \
  "|"									\
  "(" RE_ISO_PERIOD ")/(" RE_ISO_YEAR ")" \
  "|"									\
  \
  "(" RE_ISO_DAY_TIME ")" \
  "|"									\
  "(" RE_ISO_WEEK ")" \
  "|"									\
  "(" RE_ISO_MONTH ")" \


#ifdef USE_ALT_DATETIME

// 2024-WEA-7 - Sunday of Easter week 2024
//#define RE_DATE_EASTER "\\@e([\\-+][0-9]+)*"

// Today??

// every last day of a month

#define RE_DATE_EXTENSION \
  "(" RE_ISO_RECURRENCE "/)(" RE_ISO_PERIOD ")/(" RE_ISO_DAY_TIME ")/(" RE_ISO_PERIOD ")" \
  "|"									\
  "(" RE_ISO_RECURRENCE "/)(" RE_ISO_DAY_TIME ")/(" RE_ISO_DAY_TIME ")/(" RE_ISO_PERIOD ")" \
  "|"									\
  "(" RE_ISO_RECURRENCE ")/(" RE_ISO_DAY_TIME "/)(" RE_ISO_PERIOD ")/(" RE_ISO_PERIOD ")" \
  "|"									\
  "(" RE_ISO_RECURRENCE "/)(" RE_ISO_PERIOD ")/(" RE_ISO_DAY_TIME ")" \


#define RE_DATE ("\\b("	RE_ISO_8601 "|" RE_DATE_EXTENSION "|" RE_DATE_GERMAN "|" RE_DATE_UNIX "|" RE_ISO_YEAR ")\\b")

#else

#define RE_DATE ("\\b(" RE_ISO_8601 ")")

#endif

typedef enum {
  LANG_DEFAULT, 		/* == "en" */
  LANG_DE,
  LANG_FR
} lang_t;


extern xmlNsPtr
pieGetNs(void);

extern BOOL_T
CompileRegExpDefaults(void);

extern void
pieTextBlocksCleanup(void);

extern xmlNodePtr
RecognizeFigures(xmlNodePtr pndArg);

extern xmlNodePtr
RecognizeUrls(xmlNodePtr pndArg);

extern xmlNodePtr
ParsePlainBuffer(xmlNodePtr pndArgTop, xmlChar *pucArg, rmode_t eArgMode);

extern xmlNodePtr
SplitStringToScriptNode(const xmlChar *pucArg);

extern xmlNodePtr
RecognizeSymbols(xmlNodePtr pndArg, lang_t eLangArg);

extern BOOL_T
RecognizeScripts(xmlNodePtr pndArg);

extern xmlNodePtr
RecognizeTasks(xmlNodePtr pndArg);

extern xmlNodePtr
RecognizeDates(xmlNodePtr pndArg, RN_MIME_TYPE eMimeTypeArg);

extern xmlNodePtr
RecognizeSubsts(xmlNodePtr pndArg);

extern xmlNodePtr
RecognizeInserts(xmlNodePtr pndArg, int iArgMode);

#define RecognizeIncludes(N) RecognizeInserts(N,0)

#define RecognizeImports(N) RecognizeInserts(N,1)

extern xmlNodePtr
RecognizeInlines(xmlNodePtr pndArg);

extern xmlNodePtr
pieRemoveInvalidsFromTree(xmlNodePtr pndArg);

extern xmlNodePtr
CleanUpTree(xmlNodePtr pndArg);

extern xmlChar *
GetLinkDisplayNameNew(const xmlChar *pucArg);

extern xmlNodePtr
SplitTupelToLinkNodesMd(const xmlChar *pucArg);

extern xmlNodePtr
SplitStringToLinkNodes(const xmlChar *pucArg);

extern xmlChar *
TranslateUncToUrl(const xmlChar *pucArg);

extern BOOL_T
SplitNodeToTableDataNodes(xmlNodePtr pndArgParent, xmlChar *pucPatternSep);

extern xmlNodePtr
TransformToTable(xmlNodePtr pndArgParent, xmlNodePtr pndArg, xmlChar *pucPatternSep);

extern xmlNodePtr
FindElementNodeLast(xmlNodePtr pndArg);

#ifdef PIE_STANDALONE

#else

extern BOOL_T
IsImportCircularStr(xmlNodePtr pndArg, xmlChar *pucArgURI);

extern BOOL_T
IsImportCircular(xmlNodePtr pndArg, resNodePtr prnArg);

extern rmode_t
GetModeByMimeType(RN_MIME_TYPE tArg);

#endif

extern BOOL_T
IsImportFromAttr(xmlNodePtr pndArg);

extern BOOL_T
SetTypeAttr(xmlNodePtr pndArgImport, rmode_t eArgMode);

extern rmode_t 
GetModeByAttr(xmlNodePtr pndArgImport);

extern rmode_t 
GetModeByExtension(xmlChar *pucArgExt);

#ifdef TESTCODE
extern int
pieTextBlocksTest(void);
#endif
