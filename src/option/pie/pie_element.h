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

#ifdef HAVE_PCRE2
#include <pcre2.h>
#endif

typedef enum {
  RMODE_MD,     /*! markdown-formatted content */
  RMODE_PAR,    /*! paragraph content */
  RMODE_LINE,   /*! line oriented content */
  RMODE_PRE,    /*! pre-formatted content */
  RMODE_TABLE   /*! table oriented content */
} rmode_t; 	/*! read mode */


typedef enum {
  LANG_DEFAULT,
  LANG_DE,
  LANG_EN,
  LANG_FR
} lang_t;

#define LANG_DEFAULT LANG_DEFAULT

typedef enum {
  undefined,
  import,
  subst,
  header,
  listitem,
  ruler,
  quote,
  par,
  csv,
  pre,
  cxp,
  script,
  skip
} pieTextElementType;

/*! structure
 */
typedef struct {
  xmlChar *pucSource;		/*!< pointer to begin of source string */
  index_t iSourceLength;
  index_t iBegin;		/*!< */
  xmlChar *pucContent;		/*!< pointer to begin of element content string */
  index_t iDepthHidden;		/*!< hidden level */
  BOOL_T fValid;		/*!< marker flag */
  BOOL_T fEnum;		/*!<  flag */
  index_t iDepth;		/*!< */
  index_t iLength;
  index_t iWeight;
  pieTextElementType eType;
  rmode_t eMode;
  rmode_t eModeBefore;
  lang_t eLang;
  xmlChar *pucSep;		/*!< separator string */
  pcre2_code *re_read;	/*!< regular expression for filtered reading */
  BOOL_T fMatchRegExp;	/*!< flag for include or exclude elements matching regexp */
  /*!\todo encoding? */
} pieTextElement;

typedef pieTextElement *pieTextElementPtr;


extern pieTextElementPtr
pieElementNew(xmlChar *pucArg, rmode_t eModeArg, lang_t eLangArg);

extern xmlChar *
pieElementSetSep(pieTextElementPtr ppeArg, xmlChar *pucArg);

extern xmlChar *
pieElementGetSepPtr(pieTextElementPtr ppeArg);

extern xmlChar *
pieElementGetBeginPtr(pieTextElementPtr ppeArg);

extern xmlChar *
pieElementGetEndPtr(pieTextElementPtr ppeArg);

extern void
pieElementSetRegExp(pieTextElementPtr ppeArg, xmlChar *pucArgRegexp, BOOL_T fArg);

extern pcre2_code *
pieElementGetRegExp(pieTextElementPtr ppeArg);

extern void
pieElementFree(pieTextElementPtr ppeArg);

extern void
pieElementReset(pieTextElementPtr ppeArg);

extern BOOL_T
pieElementReplaceCharMarkup(pieTextElementPtr ppeArg);

extern BOOL_T
pieElementReplaceCharNumerics(pieTextElementPtr ppeArg);

extern BOOL_T
pieElementHasNext(pieTextElementPtr ppeArg);

extern BOOL_T
pieElementParse(pieTextElementPtr ppeArg);

extern BOOL_T
_pieElementUpdateMarkup(pieTextElementPtr ppeArg);

extern int
pieElementGetDepth(pieTextElementPtr ppeArg);

extern int
pieElementGetStrlen(pieTextElementPtr ppeArg);

extern rmode_t
pieElementGetMode(pieTextElementPtr ppeArg);

extern BOOL_T
pieElementIsImport(pieTextElementPtr ppeArg);

extern BOOL_T
pieElementIsSubst(pieTextElementPtr ppeArg);

extern BOOL_T
pieElementIsHeader(pieTextElementPtr ppeArg);

extern BOOL_T
pieElementIsPar(pieTextElementPtr ppeArg);

extern BOOL_T
pieElementIsPre(pieTextElementPtr ppeArg);

extern BOOL_T
pieElementIsQuote(pieTextElementPtr ppeArg);

extern BOOL_T
pieElementIsCsv(pieTextElementPtr ppeArg);

extern BOOL_T
pieElementIsScript(pieTextElementPtr ppeArg);

extern BOOL_T
pieElementIsCxp(pieTextElementPtr ppeArg);

extern BOOL_T
pieElementIsListItem(pieTextElementPtr ppeArg);

extern BOOL_T
pieElementIsRuler(pieTextElementPtr ppeArg);

extern BOOL_T
pieElementIsEnum(pieTextElementPtr ppeArg);

extern BOOL_T
pieElementIsMetaTags(pieTextElementPtr ppeArg);

extern BOOL_T
pieElementIsMetaOrigin(pieTextElementPtr ppeArg);

extern int
pieElementStrnlenEmpty(xmlChar *pucArg, int iArg);

extern xmlNodePtr
pieElementToDOM(pieTextElementPtr ppeT);

extern xmlChar *
pieElementToPlain(pieTextElementPtr ppeArg);


#ifdef TESTCODE
extern int
pieElementTest(void);
#endif
