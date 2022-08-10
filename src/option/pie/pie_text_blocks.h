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

#include <pie/pie_dtd.h>
#include <cxp/calendar_element.h>
#include <pie/pie_element.h>

#ifdef PIE_STANDALONE
#else
#include "plain_text.h"
#endif

#include <pie/pie_text_tags.h>

#define CXP_PIE_URL (BAD_CAST "http://www.tenbusch.info/pie")

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
GetParentElement(pieTextElementPtr ppeArg, xmlNodePtr pndArgParent);

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
RecognizeIncludes(xmlNodePtr pndArg);

extern xmlNodePtr
RecognizeImports(xmlNodePtr pndArg);

extern xmlNodePtr
RecognizeInlines(xmlNodePtr pndArg);

extern xmlNodePtr
pieValidateTree(xmlNodePtr pndArg);

extern xmlNodePtr
CleanUpTree(xmlNodePtr pndArg);

extern xmlChar *
GetLinkDisplayNameNew(const xmlChar *pucArg);

extern xmlNodePtr
SplitTupelToLinkNodes(const xmlChar *pucArg);

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
