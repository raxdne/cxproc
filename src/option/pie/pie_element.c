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
#include <libxml/parserInternals.h>

/*
 */
#include "basics.h"
#include "utils.h"
#include "dom.h"
#include <res_node/res_node_io.h>
#include <cxp/cxp_context.h>
#include "plain_text.h"
#include <pie/pie_dtd.h>
#include <pie/pie_element.h>
#include <pie/calendar_element.h>

#define IS_CONTACT_CHAR_START(C) (C==(xmlChar)'|')
#define IS_CONTACT_CHAR_SEP(C)   (C==(xmlChar)' ' || C==(xmlChar)',' || C==(xmlChar)';' || C==(xmlChar)'\t')

/* https://unicode.org/charts/
*/

#define STR_UTF8_LEFT_SINGLE_QUOTATION_MARK "\xE2\x80\x98"

#define STR_UTF8_RIGHT_SINGLE_QUOTATION_MARK "\xE2\x80\x99"

#define STR_UTF8_SINGLE_LOW_9_QUOTATION_MARK "\xE2\x80\x9A"

#define STR_UTF8_SINGLE_HIGH_REVERSED_9_QUOTATION_MARK "\xE2\x80\x9B"

#define STR_UTF8_LEFT_DOUBLE_QUOTATION_MARK "\xE2\x80\x9C"

#define STR_UTF8_RIGHT_DOUBLE_QUOTATION_MARK "\xE2\x80\x9D"

#define STR_UTF8_DOUBLE_LOW_9_QUOTATION_MARK "\xE2\x80\x9E"

#define STR_UTF8_DOUBLE_HIGH_REVERSED_9_QUOTATION_MARK "\xE2\x80\x9F"

#define STR_UTF8_SINGLE_LEFT_POINTING_ANGLE_QUOTATIONMARK "\xE2\x80\xB9"

#define STR_UTF8_SINGLE_RIGHT_POINTING_ANGLE_QUOTATIONMARK "\xE2\x80\xBA"

#define STR_UTF8_LEFT_POINTING_DOUBLE_ANGLE_QUOTATIONMARK "\xC2\xAB"

#define STR_UTF8_RIGHT_POINTING_DOUBLE_ANGLE_QUOTATIONMARK "\xC2\xBB"

#define STR_UTF8_LEFTWARDS_ARROW "\xE2\x86\x90"

#define STR_UTF8_RIGHTWARDS_ARROW "\xE2\x86\x92"

#define STR_UTF8_LEFT_RIGHT_ARROW "\xE2\x86\x94"

#define STR_UTF8_LEFTWARDS_SINGLE_ARROW "\xE2\x87\x90"

#define STR_UTF8_RIGHTWARDS_SINGLE_ARROW "\xE2\x87\x92"

#define STR_UTF8_LEFT_RIGHT_SINGLE_ARROW "\xE2\x87\x94"

#define STR_UTF8_EN_DASH "\xE2\x80\x93"

#define STR_UTF8_EM_DASH "\xE2\x80\x94"


static xmlChar *
DuplicateNextLine(char *pchArg, index_t *piArg);

static BOOL_T
StrIncrementToNextLine(xmlChar *pucArg, int *piArgEnd);

#ifdef WITH_MARKDOWN

static BOOL_T
MdStrLineIsEmpty(xmlChar *pucArg, int *piArgEnd);

static BOOL_T
MdStrLineIsListEnum(xmlChar *pucArg, int *piArgEnd);

static BOOL_T
MdStrLineIsList(xmlChar *pucArg, int *piArgEnd);

static BOOL_T
MdStrLineIsBlockQuote(xmlChar *pucArg, int *piArgEnd);

static BOOL_T
MdStrLineIsBlockCode(xmlChar *pucArg, int *piArgEnd);

static BOOL_T
MdStrLineIsHeader(xmlChar *pucArg, int *piArgEnd);

static BOOL_T
MdStrLineIsHeaderLine(xmlChar *pucArg, int *piArgEnd);

static BOOL_T
MdStrLineIsRuler(xmlChar *pucArg, int *piArgEnd);

static int
MdStrCountTrailingSpaces(xmlChar *pucArg);

#endif

/*! constructor for pieTextElement
*/
pieTextElementPtr
pieElementNew(xmlChar *pucArg, rmode_t eModeArg, lang_t eLangArg)
{
  pieTextElementPtr ppeResult = NULL;

  if (STR_IS_NOT_EMPTY(pucArg)) {
    ppeResult = (pieTextElementPtr)xmlMalloc(sizeof(pieTextElement));
    if (ppeResult) {
      memset(ppeResult, 0, sizeof(pieTextElement));
      pieElementReset(ppeResult);
      ppeResult->pucSource = pucArg;
      ppeResult->iSourceLength = xmlStrlen(ppeResult->pucSource);
      ppeResult->eLang = eLangArg;
      ppeResult->eModeBefore = ppeResult->eMode = eModeArg;
      ppeResult->fMatchRegExp = TRUE;
    }
  }
  return ppeResult;
}
/* end of pieElementNew() */


/*! re-set all properties but pucSource and iBegin
*/
void
pieElementReset(pieTextElementPtr ppeArg)
{
  if (ppeArg) {
    if (ppeArg->pucContent) {
      xmlFree(ppeArg->pucContent);
      ppeArg->pucContent = NULL;
    }
    ppeArg->iLength = 0;
    ppeArg->iWeight = 0;
    ppeArg->iDepth = 0;
    ppeArg->iDepthHidden = 0;
    ppeArg->fEnum = FALSE;
    ppeArg->fValid = TRUE;
    ppeArg->eType = undefined;	     /* default: no special mode */
  }
}
/* end of pieElementReset() */


/*! destructor for pieTextElement
*/
void
pieElementFree(pieTextElementPtr ppeArg)
{
  if (ppeArg) {
    pieElementReset(ppeArg);
    if (ppeArg->pucSep) {
      xmlFree(ppeArg->pucSep);
      ppeArg->pucSep = NULL;
    }
    if (ppeArg->re_read) {
      pcre2_code_free(ppeArg->re_read);
    }
    xmlFree(ppeArg);
  }
}
/* end of pieElementFree() */


/*!
*/
xmlChar *
pieElementGetBeginPtr(pieTextElementPtr ppeArg)
{
  xmlChar *pucResult = NULL;

  if (ppeArg) {
    pucResult = ppeArg->pucContent;
  }

  return pucResult;
}
/* end of pieElementGetBeginPtr() */


/*!
*/
xmlChar *
pieElementSetSep(pieTextElementPtr ppeArg, xmlChar *pucArg)
{
  xmlChar *pucResult = NULL;

  if (STR_IS_NOT_EMPTY(pucArg)) {
    ppeArg->pucSep = xmlStrdup(pucArg);
    pucResult = ppeArg->pucSep;
  }
  else {
    PrintFormatLog(1, "Ignoring empty pie separator regexp");
  }

  return pucResult;
}
/* end of pieElementSetSep() */


/*!
*/
xmlChar *
pieElementGetSepPtr(pieTextElementPtr ppeArg)
{
  xmlChar *pucResult = NULL;

  if (ppeArg) {
    pucResult = ppeArg->pucSep;
  }

  return pucResult;
}
/* end of pieElementGetSepPtr() */


/*!
*/
rmode_t
pieElementGetMode(pieTextElementPtr ppeArg)
{
  if (ppeArg) {
    return ppeArg->eMode;
  }
  return RMODE_PAR;
}
/* end of pieElementGetMode() */


/*!
*/
BOOL_T
pieElementIsImport(pieTextElementPtr ppeArg)
{
  return (ppeArg != NULL && ppeArg->eType == import);
}
/* end of pieElementIsImport() */


/*!
*/
BOOL_T
pieElementIsSubst(pieTextElementPtr ppeArg)
{
  return (ppeArg != NULL && ppeArg->eType == subst);
}
/* end of pieElementIsSubst() */


/*!
*/
BOOL_T
pieElementIsBlock(pieTextElementPtr ppeArg)
{
  return (ppeArg != NULL && ppeArg->eType == import);
}
/* end of pieElementIsBlock() */


/*!
*/
BOOL_T
pieElementIsHeader(pieTextElementPtr ppeArg)
{
  return (ppeArg != NULL && ppeArg->eType == header);
}
/* end of pieElementIsHeader() */


/*!
*/
BOOL_T
pieElementIsPar(pieTextElementPtr ppeArg)
{
  return (ppeArg != NULL && ppeArg->eType == par);
}
/* end of pieElementIsPar() */


/*!
*/
BOOL_T
pieElementIsPre(pieTextElementPtr ppeArg)
{
  return (ppeArg != NULL && ppeArg->eType == pre);
}
/* end of pieElementIsPre() */


/*!
*/
BOOL_T
pieElementIsQuote(pieTextElementPtr ppeArg)
{
  return (ppeArg != NULL && ppeArg->eType == quote);
} /* end of pieElementIsQuote() */


/*!
*/
BOOL_T
pieElementIsCsv(pieTextElementPtr ppeArg)
{
  return (ppeArg != NULL && ppeArg->eType == csv);
}
/* end of pieElementIsCsv() */


/*!
*/
BOOL_T
pieElementIsScript(pieTextElementPtr ppeArg)
{
  return (ppeArg != NULL && ppeArg->eType == script);
}
/* end of pieElementIsScript() */


/*!
*/
BOOL_T
pieElementIsCxp(pieTextElementPtr ppeArg)
{
  return (ppeArg != NULL && ppeArg->eType == cxp);
}
/* end of pieElementIsCxp() */


/*!
*/
BOOL_T
pieElementIsListItem(pieTextElementPtr ppeArg)
{
  return (ppeArg != NULL && ppeArg->eType == listitem);
}
/* end of pieElementIsListItem() */


/*!
*/
BOOL_T
pieElementIsRuler(pieTextElementPtr ppeArg)
{
  return (ppeArg != NULL && ppeArg->eType == ruler);
}
/* end of pieElementIsRuler() */


/*!
*/
BOOL_T
pieElementIsEnum(pieTextElementPtr ppeArg)
{
  return (ppeArg != NULL && ppeArg->fEnum);
}
/* end of pieElementIsEnum() */


/*!
*/
BOOL_T
pieElementIsMetaTags(pieTextElementPtr ppeArg)
{
  return (ppeArg != NULL && StringBeginsWith((char *)pieElementGetBeginPtr(ppeArg),"TAGS: "));
}
/* end of pieElementIsMetaTags() */


/*!
*/
int
pieElementGetDepth(pieTextElementPtr ppeArg)
{
  if (ppeArg) {
    return ppeArg->iDepth;
  }
  return -1;
}
/* end of pieElementGetDepth() */


/*! detect trailing weight markup
*/
int
pieElementWeight(pieTextElementPtr ppeArg)
{
  int iResult = -1;

  if (ppeArg != NULL && STR_IS_NOT_EMPTY(ppeArg->pucContent)) {
    int i;
    xmlChar *pucT;

    for (pucT = ppeArg->pucContent, i = xmlStrlen(pucT)-1; pucT[i] == (xmlChar)'+'; pucT[i--] = (xmlChar)'\0') {
      ppeArg->iWeight++;
    }

    if (ppeArg->iWeight > 1) {
      for (; pucT[i] == (xmlChar)' '; pucT[i--] = (xmlChar)'\0'); /* skip all trailing spaces */
    }

    iResult = ppeArg->iWeight;
  }
  return iResult;
} /* end of pieElementWeight() */


/*!
*/
int
pieElementGetStrlen(pieTextElementPtr ppeArg)
{
  if (ppeArg) {
    return ppeArg->iLength;
  }
  return 0;
} /* end of pieElementGetStrlen() */


/*! \deprecated
*/
BOOL_T
pieElementReplace(pieTextElementPtr ppeArg, xmlChar *pucArgA, xmlChar *pucArgB)
{
  BOOL_T fResult = FALSE;
  xmlChar *pucT;

  pucT = ReplaceStr(ppeArg->pucContent, pucArgA, pucArgB);
  if (pucT) {
    xmlFree(ppeArg->pucContent);
    ppeArg->pucContent = pucT;
    ppeArg->iLength = xmlStrlen(ppeArg->pucContent);
    fResult = TRUE;
  }
  return fResult;
}
/* end of pieElementReplace() */

/*! 
*/
BOOL_T
StrIncrementToNextLine(xmlChar *pucArg, int *piArgEnd)
{
  BOOL_T fResult = FALSE;

  if (pucArg == NULL) {
  }
  else {
    index_t iLength;

    for (iLength=0; ; iLength++) {
      if (pucArg[iLength] == (xmlChar)'\0' || pucArg[iLength] == (xmlChar)'\n') {
	if (piArgEnd != NULL && *piArgEnd > -1) {
	  *piArgEnd += (pucArg[iLength] == (xmlChar)'\n') ? iLength + 1 : iLength;
	}
	fResult = TRUE;
	break;
      }
    }
  }
  return fResult;
} /* end of StrIncrementToNextLine() */


/*!
*/
BOOL_T
StrLineIsEmpty(xmlChar *pucArg, int *piArgEnd)
{
  BOOL_T fResult = TRUE;

  if (pucArg == NULL || *pucArg == (xmlChar)'\0') {
    if (piArgEnd != NULL) {
      *piArgEnd = 0;
    }
  }
  else {
    xmlChar *pucT;

    for (pucT = pucArg; *pucT != (xmlChar)'\0'; pucT++) {
      if (*pucT == (xmlChar)'\n' || *pucT == (xmlChar)'\r' || *pucT == (xmlChar)'\t' || *pucT == (xmlChar)' ') {
	/* char for empty */
      }
      else {
	fResult = FALSE;
	break;
      }
    }

    if (piArgEnd != NULL) {
      *piArgEnd = (int)(pucT - pucArg);
    }
  }
  return fResult;
} /* end of StrLineIsEmpty() */


#ifdef WITH_MARKDOWN

/*! 
*/
BOOL_T
MdStrLineIsEmpty(xmlChar *pucArg, int *piArgEnd)
{
  BOOL_T fResult = FALSE;

  if (pucArg == NULL) {
  }
  else if (*pucArg == (xmlChar)'\0') {
    if (piArgEnd != NULL) {
      *piArgEnd = 0;
    }
    fResult = TRUE;
  }
  else if (MdStrLineIsBlockCode(pucArg, piArgEnd)) {
  }
  else {
    xmlChar *pucT;

    for (fResult = TRUE, pucT=pucArg; fResult && *pucT != (xmlChar)'\0' && *pucT != (xmlChar)'\n'; pucT++) {
      if (*pucT == (xmlChar)'\n' || *pucT == (xmlChar)'\r' || *pucT == (xmlChar)'\t' || *pucT == (xmlChar)' ') {
	/* char for empty */
      }
      else {
	fResult = FALSE;
      }
    }

    if (fResult && piArgEnd != NULL) {
      *piArgEnd = (int)(pucT - pucArg);
    }
  }
  return fResult;
} /* end of MdStrLineIsEmpty() */


/*! 
*/
BOOL_T
MdStrLineIsListEnum(xmlChar *pucArg, int *piArgEnd)
{
  BOOL_T fResult = FALSE;

  if (STR_IS_NOT_EMPTY(pucArg)) {
    xmlChar *pucT;

    for (pucT=pucArg; *pucT != (xmlChar)'\0' && *pucT != (xmlChar)'\n'; pucT++) {
      if ((pucT[0] == (xmlChar)' ')) {
      }
      else if (isdigit(pucT[0]) && (pucT[1] == (xmlChar)'.' || pucT[1] == (xmlChar)')') && (pucT[2] == (xmlChar)' ')) {
	fResult = TRUE;
	if (piArgEnd) {
	  *piArgEnd = (int)(pucT - pucArg + 2);
	}
	break;
      }
      else {
	break;
      }
    }
  }
  return fResult;
} /* end of MdStrLineIsListEnum() */


/*! 
*/
BOOL_T
MdStrLineIsList(xmlChar *pucArg, int *piArgEnd)
{
  BOOL_T fResult = FALSE;

  if (STR_IS_NOT_EMPTY(pucArg)) {
    xmlChar *pucT;

    for (pucT=pucArg; *pucT != (xmlChar)'\0' && *pucT != (xmlChar)'\n'; pucT++) {
      if ((pucT[0] == (xmlChar)' ')) {
      }
      else if ((pucT[0] == (xmlChar)'*' || pucT[0] == (xmlChar)'-' || pucT[0] == (xmlChar)'+')) {
	if ((pucT[1] == (xmlChar)' ' || pucT[1] == (xmlChar)'\t')) {
	  fResult = TRUE;
	  if (piArgEnd) {
	    *piArgEnd = (int)(pucT - pucArg + 1);
	  }
	}
	break;
      }
      else {
	break;
      }
    }
  }
  return fResult;
} /* end of MdStrLineIsList() */


/*!
*/
BOOL_T
MdStrLineIsBlockQuote(xmlChar *pucArg, int *piArgEnd)
{
  BOOL_T fResult = FALSE;

  if (pucArg == NULL) {
  }
  else if (pucArg[0] == (xmlChar)'>') {
    fResult = TRUE;
    if (piArgEnd) {
      *piArgEnd = (pucArg[1] == (xmlChar)' ') ? 2 : 1;
    }
  }
  return fResult;
} /* end of MdStrLineIsBlockQuote() */


/*!
*/
BOOL_T
MdStrLineIsBlockCode(xmlChar *pucArg, int *piArgEnd)
{
  BOOL_T fResult = FALSE;

  if (pucArg == NULL) {
  }
  else if (pucArg[0] == (xmlChar)'\t') {
    fResult = TRUE;
    if (piArgEnd) {
      *piArgEnd = 1;
    }
  }
  else if (pucArg[0] == (xmlChar)' ' && pucArg[1] == (xmlChar)' ' && pucArg[2] == (xmlChar)' ' && pucArg[3] == (xmlChar)' ') {
    fResult = TRUE;
    if (piArgEnd) {
      *piArgEnd = 4;
    }
  }
  return fResult;
} /* end of MdStrLineIsBlockCode() */


/*!
*/
BOOL_T
MdStrLineIsHeader(xmlChar *pucArg, int *piArgEnd)
{
  BOOL_T fResult = FALSE;

  if (STR_IS_NOT_EMPTY(pucArg)) {
    xmlChar *pucT;

    for (pucT=pucArg; *pucT != (xmlChar)'\0' && *pucT != (xmlChar)'\r' && *pucT != (xmlChar)'\n'; pucT++) {
      if (pucT[0] == (xmlChar)'#') {
	fResult = TRUE;
	if (piArgEnd) {
	  *piArgEnd = (int)(&pucT[1] - pucArg);
	  if (*piArgEnd > 6) {
	    /* markup is too long, reset result */
	    *piArgEnd = 0;
	    fResult = FALSE;
	  }
	}
      }
      else {
	break;
      }
    }
  }
  return fResult;
} /* end of MdStrLineIsHeader() */


/*!
*/
BOOL_T
MdStrLineIsHeaderLine(xmlChar *pucArg, int *piArgEnd)
{
  BOOL_T fResult = FALSE;

  if (STR_IS_NOT_EMPTY(pucArg)) {
    xmlChar *pucT;

    for (fResult = TRUE, pucT=pucArg; *pucT != (xmlChar)'\0' && *pucT != (xmlChar)'\r' && *pucT != (xmlChar)'\n'; pucT++) {
      if (pucT[0] == (xmlChar)'=' || pucT[0] == (xmlChar)'-') {
	/* char for header underline */
      }
      else {
	fResult = FALSE;
	break;
      }
    }

    if (fResult && piArgEnd != NULL) {
      *piArgEnd = (int)(pucT - pucArg + 1);
      if (*piArgEnd < 2) {
	/* underline is too short, reset result */
	*piArgEnd = 0;
	fResult = FALSE;
      }
    }
  }
  return fResult;
} /* end of MdStrLineIsHeaderLine() */


/*!
*/
BOOL_T
MdStrLineIsRuler(xmlChar *pucArg, int *piArgEnd)
{
  BOOL_T fResult = FALSE;

  if (STR_IS_NOT_EMPTY(pucArg)) {
    xmlChar *pucT;

    for (fResult = TRUE, pucT=pucArg; *pucT != (xmlChar)'\0' && *pucT != (xmlChar)'\r' && *pucT != (xmlChar)'\n'; pucT++) {
      if (pucT[0] == (xmlChar)'*' || pucT[0] == (xmlChar)'-' || pucT[0] == (xmlChar)'_') {
	/* char for ruler */
      }
      else {
	fResult = FALSE;
	break;
      }
    }

    if (fResult && piArgEnd != NULL) {
      *piArgEnd = (int)(pucT - pucArg + 1);
      if (*piArgEnd < 2) {
	/* underline is too short, reset result */
	*piArgEnd = 0;
	fResult = FALSE;
      }
    }
  }
  return fResult;
} /* end of MdStrLineIsRuler() */


/*! \return TRUE if pucArg has min 2 trailing spaces
*/
int
MdStrCountTrailingSpaces(xmlChar *pucArg)
{
  int iResult = 0;

  if (STR_IS_NOT_EMPTY(pucArg)) {
    int i, j;

    for (j = i = xmlStrlen(pucArg); i > 1 && pucArg[i-1] == (xmlChar)' '; i--);
    iResult = j - i;
  }
  return iResult;
} /* end of MdStrCountTrailingSpaces() */

#endif

/*! generator for next element

\param ppeArg
\return TRUE if there is a next input element

\todo additional regexp argument for paragraph separator?
*/
BOOL_T
pieElementHasNext(pieTextElementPtr ppeArg)
{
  BOOL_T fResult = FALSE;

  if (ppeArg) {
    xmlChar *puc0;

    pieElementReset(ppeArg);

    puc0 = ppeArg->pucSource + ppeArg->iBegin;
    if (STR_IS_NOT_EMPTY(puc0)) {

      if (pieElementGetMode(ppeArg) == RMODE_PRE) {
	ppeArg->iLength = xmlStrlen(puc0);
	if (ppeArg->iLength > 0) {
	  ppeArg->pucContent = xmlStrdup(puc0);
	  ppeArg->iBegin += ppeArg->iLength;
	  ppeArg->eType = pre;
	  fResult = TRUE;
	}
	else {
	}
      }
      else if (StringBeginsWith((char *)puc0, "#begin_of_skip")) {
	/*
	handle skip markup in input file
	*/
	xmlChar *puc1;

	if ((puc1 = BAD_CAST xmlStrstr(puc0, BAD_CAST"#end_of_skip")) != NULL) {
	  /* skip string between markups */
	  assert(Strnstr(puc0 + 1, (puc1 - puc0), BAD_CAST"#begin_of_skip") == NULL); /* nested skip markup? */
	  ppeArg->iBegin = (index_t)(puc1 + xmlStrlen(BAD_CAST"#end_of_skip") - ppeArg->pucSource);
	  fResult = TRUE;
	}
	else {
	  /* no end markup found, end of string */
	  ppeArg->iBegin = xmlStrlen(ppeArg->pucSource);
	}

	ppeArg->eType = skip;
	ppeArg->pucContent = NULL;
	//ppeArg->iLength = 0;
      }
      else if (pieElementGetMode(ppeArg) == RMODE_PAR && StringBeginsWith((char *)puc0, "#begin_of_script")) {
	/*
	handle script formatted markup in input file
	*/
	xmlChar *puc1;
	xmlChar *pucT;

	if ((puc1 = BAD_CAST xmlStrstr(puc0, BAD_CAST"#end_of_script")) != NULL) {
	  /* copy string between markups */
	  assert(Strnstr(puc0 + 1, (puc1 - puc0), BAD_CAST"#begin_of_script") == NULL); /* nested script markup? */

	  ppeArg->pucContent = xmlStrndup(puc0 + xmlStrlen(BAD_CAST"#begin_of_script"), (int)(puc1 - (puc0 + xmlStrlen(BAD_CAST"#begin_of_script"))));
	  ppeArg->iBegin += (index_t)(puc1 + xmlStrlen(BAD_CAST"#end_of_script") - puc0);
	}
	else {
	  /* no end markup found, copy end of string */
	  ppeArg->pucContent = xmlStrdup(puc0 + xmlStrlen(BAD_CAST"#begin_of_script"));
	  ppeArg->iBegin = xmlStrlen(ppeArg->pucSource);
	}

	ppeArg->eType = script;
	ppeArg->iLength = xmlStrlen(ppeArg->pucContent);
	fResult = TRUE;
      }
      else if (pieElementGetMode(ppeArg) == RMODE_PAR && StringBeginsWith((char *)puc0, "#begin_of_pre")) {
	/*
	handle pre formatted markup in input file
	*/
	xmlChar *puc1;
	xmlChar *pucT;

	if ((puc1 = BAD_CAST xmlStrstr(puc0, BAD_CAST"#end_of_pre")) != NULL) {
	  /* copy string between markups */
	  index_t l;

	  assert(Strnstr(puc0 + 1, (puc1 - puc0), BAD_CAST"#begin_of_pre") == NULL); /* nested pre markup? */

	  for (l = xmlStrlen(BAD_CAST"#begin_of_pre"); puc0[l] == (xmlChar)'\n'; l++); /* skip leading empty lines */
	  ppeArg->pucContent = xmlStrndup(puc0 + l, (int)(puc1 - (puc0 + l)));
	  for (l = xmlStrlen(ppeArg->pucContent) - 1; l > 0 && (ppeArg->pucContent[l] == (xmlChar)'\n' || ppeArg->pucContent[l] == (xmlChar)'\r'); l--) {
	    ppeArg->pucContent[l] = (xmlChar)'\0'; /* cut trailing empty lines */
	  }

	  ppeArg->iBegin += (index_t)(puc1 + xmlStrlen(BAD_CAST"#end_of_pre") - puc0);
	}
	else {
	  /* no end markup found, copy end of string */
	  ppeArg->pucContent = xmlStrdup(puc0 + xmlStrlen(BAD_CAST"#begin_of_pre"));
	  ppeArg->iBegin = xmlStrlen(ppeArg->pucSource);
	}

	ppeArg->eType = pre;
	ppeArg->iLength = xmlStrlen(ppeArg->pucContent);
	fResult = TRUE;
      }
#ifdef WITH_MARKDOWN
      else if (pieElementGetMode(ppeArg) == RMODE_MD && StringBeginsWith((char *)puc0, "```")) {
	/*
	  handle pre formatted markup in input file
	*/
	index_t l;
	xmlChar *puc1;
	xmlChar *pucT;

	for (l = 3; puc0[l] != (xmlChar)'\n' && puc0[l] != (xmlChar)'\0'; l++); /* skip to end of line */
	  
	for ( ; puc0[l] == (xmlChar)'\n'; l++); /* skip leading empty lines */

	if ((puc1 = BAD_CAST xmlStrstr(puc0+l, BAD_CAST"```")) != NULL) {
	  /* copy string between markups */

	  ppeArg->pucContent = xmlStrndup(puc0 + l, (int)(puc1 - (puc0 + l)));
	  for (l = xmlStrlen(ppeArg->pucContent) - 1; l > 0 && (ppeArg->pucContent[l] == (xmlChar)'\n' || ppeArg->pucContent[l] == (xmlChar)'\r'); l--) {
	    ppeArg->pucContent[l] = (xmlChar)'\0'; /* cut trailing empty lines */
	  }

	  ppeArg->iBegin += (index_t)(puc1 + 3 - puc0);
	}
	else {
	  /* no end markup found, copy end of string */
	  ppeArg->pucContent = xmlStrdup(puc0 + l);
	  ppeArg->iBegin = xmlStrlen(ppeArg->pucSource);
	}

	ppeArg->eType = pre;
	ppeArg->iLength = xmlStrlen(ppeArg->pucContent);
	fResult = TRUE;
      }
      else if (pieElementGetMode(ppeArg) == RMODE_MD && StringBeginsWith((char *)puc0, "~~~")) {
	/*
	  handle pre formatted markup in input file
	*/
	index_t l;
	xmlChar *puc1;
	xmlChar *pucT;

	for (l = 3; puc0[l] != (xmlChar)'\n' && puc0[l] != (xmlChar)'\0'; l++); /* skip to end of line */
	  
	for ( ; puc0[l] == (xmlChar)'\n'; l++); /* skip leading empty lines */

	if ((puc1 = BAD_CAST xmlStrstr(puc0+l, BAD_CAST"~~~")) != NULL) {
	  /* copy string between markups */

	  ppeArg->pucContent = xmlStrndup(puc0 + l, (int)(puc1 - (puc0 + l)));
	  for (l = xmlStrlen(ppeArg->pucContent) - 1; l > 0 && (ppeArg->pucContent[l] == (xmlChar)'\n' || ppeArg->pucContent[l] == (xmlChar)'\r'); l--) {
	    ppeArg->pucContent[l] = (xmlChar)'\0'; /* cut trailing empty lines */
	  }

	  ppeArg->iBegin += (index_t)(puc1 + 3 - puc0);
	}
	else {
	  /* no end markup found, copy end of string */
	  ppeArg->pucContent = xmlStrdup(puc0 + l);
	  ppeArg->iBegin = xmlStrlen(ppeArg->pucSource);
	}

	ppeArg->eType = pre;
	ppeArg->iLength = xmlStrlen(ppeArg->pucContent);
	fResult = TRUE;
      }
#endif
      else if (pieElementGetMode(ppeArg) == RMODE_PAR && StringBeginsWith((char *)puc0, "#begin_of_cxp")) {
	/*
	handle cxp formatted markup in input file
	*/
	xmlChar *puc1;
	xmlChar *pucT;

	if ((puc1 = BAD_CAST xmlStrstr(puc0, BAD_CAST"#end_of_cxp")) != NULL) {
	  /* copy string between markups */
	  assert(Strnstr(puc0 + 1, (puc1 - puc0), BAD_CAST"#begin_of_cxp") == NULL); /* nested cxp markup? */

	  ppeArg->pucContent = xmlStrndup(puc0 + xmlStrlen(BAD_CAST"#begin_of_cxp"), (int)(puc1 - (puc0 + xmlStrlen(BAD_CAST"#begin_of_cxp"))));
	  ppeArg->iBegin += (index_t)(puc1 + xmlStrlen(BAD_CAST"#end_of_cxp") - puc0);
	}
	else {
	  /* no end markup found, copy end of string */
	  ppeArg->pucContent = xmlStrdup(puc0 + xmlStrlen(BAD_CAST"#begin_of_cxp"));
	  ppeArg->iBegin = xmlStrlen(ppeArg->pucSource);
	}

	ppeArg->eType = cxp;
	ppeArg->iLength = xmlStrlen(ppeArg->pucContent);
	fResult = TRUE;
      }
      else if (pieElementGetMode(ppeArg) == RMODE_LINE && StringBeginsWith((char *)puc0, "#end_of_line")) {
	/*
	switch to line-based parsing
	*/
	ppeArg->eType = skip;
	ppeArg->pucContent = NULL;
	ppeArg->iBegin += xmlStrlen(BAD_CAST"#end_of_line");
	ppeArg->eMode = ppeArg->eModeBefore;
	fResult = TRUE;
      }
      else if (pieElementGetMode(ppeArg) == RMODE_PAR && StringBeginsWith((char *)puc0, "#begin_of_line")) {
	/*
	switch to line-based parsing
	*/
	ppeArg->eType = skip;
	ppeArg->pucContent = NULL;
	ppeArg->iBegin += xmlStrlen(BAD_CAST"#begin_of_line");
	ppeArg->eModeBefore = ppeArg->eMode;
	ppeArg->eMode = RMODE_LINE;
	fResult = TRUE;
      }
      else if (pieElementGetMode(ppeArg) == RMODE_PAR && StringBeginsWith((char *)puc0, "#begin_of_csv")) {
	/*
	handle csv formatted markup in input file
	*/
	xmlChar *puc1;
	xmlChar *pucT;

	for (pucT = puc0 + xmlStrlen(BAD_CAST"#begin_of_csv"); *pucT == (xmlChar)'\r' || *pucT == (xmlChar)'\n'; pucT++) {}

	if ((puc1 = BAD_CAST xmlStrstr(puc0, BAD_CAST"#end_of_csv")) != NULL) {
	  /* copy string between markups */
	  assert(Strnstr(puc0 + 1, (puc1 - puc0), BAD_CAST"#begin_of_csv") == NULL); /* nested csv markup? */

	  ppeArg->pucContent = xmlStrndup(pucT, (int)(puc1 - pucT));
	  ppeArg->iBegin += (index_t)(puc1 + xmlStrlen(BAD_CAST"#end_of_csv") - puc0);
	}
	else {
	  /* no end markup found, copy end of string */
	  ppeArg->pucContent = xmlStrdup(pucT);
	  ppeArg->iBegin = xmlStrlen(ppeArg->pucSource);
	}

	ppeArg->eType = csv;
	ppeArg->iLength = xmlStrlen(ppeArg->pucContent);
	fResult = TRUE;
      }
      else {
	index_t iNextBegin;
	index_t iLine;
	index_t iLengthT;
	xmlChar *pucLine;
	xmlChar *pucContent = NULL;

	/*\todo restrict number of loops */
	for (iLine=0, iLengthT=0, iNextBegin=0, pucContent=NULL;
	  fResult == FALSE && (pucLine = DuplicateNextLine((char *)&(puc0[iNextBegin]), &iLengthT)) != NULL;
	  iLine++) {

	  /* pucLine contains the next line */

	  if (pieElementGetMode(ppeArg) == RMODE_TABLE) {
	    if (iLengthT < 1) {
	      /* respect empty line */
	      xmlFree(pucLine);
	    }
	    else if (StringBeginsWith((char *)pucLine, "sep=")) {
	      /*\todo handle this useful MS Excel convention, map some value to a valid regexp */
	      pieElementSetSep(ppeArg, pucLine + 4);
	      /*!\todo check pattern */
	      iNextBegin += iLengthT + 1;
	      iLengthT = 0;
	      xmlFree(pucLine);
	      continue;
	    }
	    else {
	      pucContent = pucLine;
	    }

	    for (iNextBegin += iLengthT; puc0[iNextBegin] == (xmlChar)'\r'; iNextBegin++) {
	      /* skip all following return chars */
	    }

	    if (puc0[iNextBegin] == (xmlChar)'\n') {
	      iNextBegin++;
	    }

	    fResult = TRUE;
	    break;
	  }
	  else if (pieElementGetMode(ppeArg) == RMODE_MD) {
#ifdef WITH_MARKDOWN
	    int iEnd;

	    if (MdStrLineIsEmpty(pucLine, &iEnd)) {
	      /* empty line */
	      xmlFree(pucLine);
	      StrIncrementToNextLine(&puc0[iNextBegin], &iNextBegin);
	      fResult = TRUE;
	    }
	    else if (MdStrLineIsRuler(pucLine, &iEnd)) { /* ruler */
	      xmlFree(pucLine);
	      StrIncrementToNextLine(&puc0[iNextBegin], &iNextBegin);
	      ppeArg->eType = ruler;
	      fResult = TRUE;
	    }
	    else if (MdStrLineIsListEnum(pucLine, &iEnd) || MdStrLineIsList(pucLine, &iEnd) || ppeArg->eType == listitem) { /* list */
	      if (pucContent == NULL) {
		/* first string in current pie element */
		pucContent = pucLine;
	      }
	      else {
		/* add space char */
		pucContent = xmlStrcat(pucContent, BAD_CAST"\n");
		pucContent = xmlStrcat(pucContent, pucLine);
		xmlFree(pucLine);
	      }

	      StrIncrementToNextLine(&puc0[iNextBegin], &iNextBegin);

	      if (MdStrCountTrailingSpaces(pucContent) > 1) {
		/*!\todo cut trailing spaces */
		fResult = TRUE;
	      }
	      else if (MdStrLineIsEmpty(&puc0[iNextBegin], &iEnd)) {
		/* next line is empty */
		iNextBegin += iEnd;
		fResult = TRUE;
	      }
	      else if (MdStrLineIsListEnum(&puc0[iNextBegin], NULL) || MdStrLineIsList(&puc0[iNextBegin], NULL)) {
		fResult = TRUE;
	      }
	      else if (MdStrLineIsHeader(&puc0[iNextBegin], NULL)) {
		fResult = TRUE;
	      }
	      else if (MdStrLineIsHeaderLine(&puc0[iNextBegin], NULL)) { /* header underline */
		fResult = TRUE;
	      }
	      else if (MdStrLineIsRuler(&puc0[iNextBegin], NULL)) { /* ruler */
		fResult = TRUE;
	      }
	      else if (MdStrLineIsBlockCode(&puc0[iNextBegin], NULL)) { /* pre-formatted */
		//fResult = TRUE;
	      }
	      else if (MdStrLineIsBlockQuote(&puc0[iNextBegin], NULL)) {
		fResult = TRUE;
	      }
	      else {
	      }

	      ppeArg->eType = listitem;
	    }
	    else if (MdStrLineIsBlockCode(pucLine, &iEnd) && ppeArg->eType != par) { /* '    ' or '\t' pre-formatted, but not in an simple paragraph */
	      if (pucContent == NULL) {
		/* first string in current pie element */
		pucContent = xmlStrdup(&pucLine[iEnd]);
		ppeArg->eType = pre;
	      }
	      else {
		/* add space char */
		pucContent = xmlStrcat(pucContent, BAD_CAST"\n");
		pucContent = xmlStrcat(pucContent, &pucLine[iEnd]);
	      }
	      xmlFree(pucLine);

	      StrIncrementToNextLine(&puc0[iNextBegin], &iNextBegin);

	      if (MdStrLineIsEmpty(&puc0[iNextBegin], &iEnd)) {
		/* next line is empty */
		pucContent = xmlStrcat(pucContent, BAD_CAST"\n");
		iNextBegin += iEnd;
		fResult = TRUE;
	      }
	      else if (MdStrLineIsHeader(&puc0[iNextBegin], NULL)) {
		fResult = TRUE;
	      }
	      else if (MdStrLineIsHeaderLine(&puc0[iNextBegin], NULL)) { /* header underline */
		fResult = TRUE;
	      }
	      else if (MdStrLineIsRuler(&puc0[iNextBegin], NULL)) { /* ruler */
		fResult = TRUE;
	      }
	      else if (MdStrLineIsBlockCode(&puc0[iNextBegin], NULL)) { /* pre-formatted */
	      }
	      else if (MdStrLineIsBlockQuote(&puc0[iNextBegin], NULL)) {
		fResult = TRUE;
	      }
	      else {
		fResult = TRUE;
	      }
	    }
	    else if (MdStrLineIsBlockQuote(pucLine, &iEnd)) { /* '>' blockquote */
	      if (pucContent == NULL) {
		/* first string in current pie element */
		pucContent = xmlStrdup(&pucLine[iEnd]);
		ppeArg->eType = quote;
	      }
	      else {
		/* add space char */
		pucContent = xmlStrcat(pucContent, BAD_CAST"\n");
		pucContent = xmlStrcat(pucContent, &pucLine[iEnd]);
	      }
	      xmlFree(pucLine);

	      StrIncrementToNextLine(&puc0[iNextBegin], &iNextBegin);

	      if (MdStrLineIsEmpty(&puc0[iNextBegin], &iEnd)) {
		/* next line is empty */
		iNextBegin += iEnd;
		fResult = TRUE;
	      }
	      else if (MdStrLineIsHeader(&puc0[iNextBegin], NULL)) {
		fResult = TRUE;
	      }
	      else if (MdStrLineIsHeaderLine(&puc0[iNextBegin], NULL)) { /* header underline */
	      }
	      else if (MdStrLineIsRuler(&puc0[iNextBegin], NULL)) { /* ruler */
		fResult = TRUE;
	      }
	      else if (MdStrLineIsBlockCode(&puc0[iNextBegin], NULL)) { /* pre-formatted */
		fResult = TRUE;
	      }
	      else if (MdStrLineIsBlockQuote(&puc0[iNextBegin], NULL)) {
		/* continue paragraph if blockquote follows */
	      }
	      else {
		fResult = TRUE;
	      }
	    }
	    else { /* paragraph, add line to buffer without linebreak */
	      if (pucContent == NULL) { /* first string in current pie element */
		ppeArg->eType = par;
		pucContent = pucLine;
		if (MdStrLineIsHeader(pucContent, &iEnd)) { /* single-line header */
		  iNextBegin += iEnd;
		  ppeArg->eType = header;
		  fResult = TRUE;
		}
	      }
	      else {
		/* concat space char */
		pucContent = xmlStrcat(pucContent, BAD_CAST" ");
		pucContent = xmlStrcat(pucContent, pucLine);
		xmlFree(pucLine);
	      }

	      StrIncrementToNextLine(&puc0[iNextBegin], &iNextBegin);

	      /*! preview next line to recognize an end of current paragraph, because of header, blockquote etc
	      */

	      if (MdStrCountTrailingSpaces(pucContent) > 1) {
		/*!\todo cut trailing spaces */
		fResult = TRUE;
	      }
	      else if (MdStrLineIsEmpty(&puc0[iNextBegin], &iEnd)) {
		/* next line is empty */
		iNextBegin += iEnd;
		fResult = TRUE;
	      }
	      else if (MdStrLineIsHeader(&puc0[iNextBegin], NULL)) {
		/* close paragraph if blockquote follows */
		/*!\bug paragraph to be closed if header with following line "===" or "---" */
		fResult = TRUE;
	      }
	      else if (MdStrLineIsHeaderLine(&puc0[iNextBegin], &iEnd)) { /* header underline */
		if (puc0[iNextBegin] == (xmlChar)'=') {
		  ppeArg->iDepth = 1;
		}
		else if (puc0[iNextBegin] == (xmlChar)'-') {
		  ppeArg->iDepth = 2;
		}
		else {
		  assert(TRUE);
		}
		ppeArg->eType = header;
		StrIncrementToNextLine(&puc0[iNextBegin], &iNextBegin);
		fResult = TRUE;
	      }
	      else if (MdStrLineIsRuler(&puc0[iNextBegin], NULL)) { /* ruler */
		fResult = TRUE;
	      }
	      else if (MdStrLineIsListEnum(&puc0[iNextBegin], NULL) || MdStrLineIsList(&puc0[iNextBegin], NULL)) {
		fResult = TRUE;
	      }
	      else if (MdStrLineIsBlockCode(&puc0[iNextBegin], NULL)) { /* pre-formatted */
	      }
	      else if (MdStrLineIsBlockQuote(&puc0[iNextBegin], NULL)) {
		/* close paragraph if blockquote follows */
		fResult = TRUE;
	      }
	      else {
	      }
	    }
#endif
	  }
	  else if (pieElementGetMode(ppeArg) == RMODE_LINE) {
	    if (iLengthT < 1) {
	      /* ignore empty line */
	      iNextBegin++;
	      xmlFree(pucLine);
	      continue;
	    }
	    else {
	      pucContent = pucLine;
	      for (iNextBegin += iLengthT; isspace(puc0[iNextBegin]); iNextBegin++) {
		/* before break, skip all following newline chars */
	      }
	      fResult = TRUE;
	      NormalizeStringSpaces((char *)pucContent); /* replace tabs and multiple spaces */
	      break;
	    }
	  }
	  else if (pieElementGetMode(ppeArg) == RMODE_PAR) {
	    /* add line to buffer without linebreak */
	    if (StrLineIsEmpty(pucLine, NULL)) {
	      /* empty line */
	      xmlFree(pucLine);
	      fResult = TRUE;
	    }
	    else if (pucContent == NULL) {
	      /* first string in current pie element */
	      pucContent = pucLine;
	      ppeArg->eType = par;
	    }
	    else {
	      /* add space char */
	      pucContent = xmlStrcat(pucContent, BAD_CAST" ");
	      pucContent = xmlStrcat(pucContent, pucLine);
	      xmlFree(pucLine);
	    }
	    StrIncrementToNextLine(&puc0[iNextBegin], &iNextBegin);
	    //NormalizeStringSpaces((char *)pucContent); /* replace tabs and multiple spaces */
	  }
	  else {
	    assert(FALSE);
	  }
	}
	ppeArg->iBegin += iNextBegin;

	if (xmlStrlen(pucContent) < 1) {
	  pieTextElementType eType;

	  eType = ppeArg->eType;
	  xmlFree(pucContent);
	  pieElementReset(ppeArg);
	  ppeArg->eType = eType;
	}
	else {
	  ppeArg->pucContent = pucContent;
	  ppeArg->iLength = xmlStrlen(ppeArg->pucContent);
	}
      }
    }
    else {
      /* no content */
    }
  }
  return fResult;
} /* end of pieElementHasNext() */


/*! update legacy markup in ppeArg->pucContent

\param ppeArg
\return TRUE or FALSE in case of errors
*/
BOOL_T
_pieElementUpdateMarkup(pieTextElementPtr ppeArg)
{
  BOOL_T fResult = FALSE;

  if (ppeArg != NULL && STR_IS_NOT_EMPTY(ppeArg->pucContent) && pieElementGetMode(ppeArg) != RMODE_PRE) {
    int i, j, k;
    xmlChar *pucA;

    pucA = ppeArg->pucContent;

    /* try to find backwards an 'ending' separator */
    for (j=xmlStrlen(pucA); j>0 && IS_CONTACT_CHAR_START(pucA[j]) == FALSE; j--);

    /* count separators backwards */
    for (k = j; k > 0 && IS_CONTACT_CHAR_START(pucA[k]); k--);

    if (k < j && isspace(pucA[k])) { /* there is more than one separator and a space char before */

      xmlChar *pucB;

      ppeArg->iWeight = j - k;

      pucB = BAD_CAST xmlMalloc((size_t) ppeArg->iLength * 2);
      k++; /* to include space char too */
      memcpy(pucB, pucA, k);
      pucB[k] = '\0';

      for (i=j+1; pucA[i]; ) {
	int l;

	for (; isspace(pucA[i]); i++);
	
	for (j=i; ishashtag(&pucA[i],&l); i += l);

	if (i > j) { /* there are hashtag chars */
	  xmlChar *pucT;

	  pucB[k++] = (xmlChar)' ';
	  if (pucA[j] == (xmlChar)'#' || pucA[j] == (xmlChar)'@') {
	    /* there is hashtag markup already */
	  }
	  else {
	    pucB[k++] = (xmlChar)'@';
	  }
	  memcpy(&pucB[k], &pucA[j], (size_t) i - j);

#ifdef EXPERIMENTAL
	  if ((pucT = Strnstr(&pucB[k], i - j, BAD_CAST"_org")) != NULL
	    || (pucT = Strnstr(&pucB[k], i - j, BAD_CAST"_do")) != NULL
	    || (pucT = Strnstr(&pucB[k], i - j, BAD_CAST"_review")) != NULL
	    || (pucT = Strnstr(&pucB[k], i - j, BAD_CAST"_rejected")) != NULL) {
	    /* split GTD categories '_org', '_do', '_review', '_rejected' */
	    pucT[0] = (xmlChar)'#';
	  }
#endif

	  k += i - j;
	  pucB[k] = (xmlChar)'\0';
	}
	else if (isend(pucA[i])) { /* neither space nor hashtag chars */
	  break;
	}
	else {
	  i++;
	}
      }
      xmlFree(ppeArg->pucContent);
      ppeArg->pucContent = pucB;
    }
    fResult = TRUE;
  }
  return fResult;
} /* end of _pieElementUpdateMarkup() */


/*!
*/
BOOL_T
pieElementParse(pieTextElementPtr ppeArg)
{
  if (ppeArg == NULL) {
  }
  else if (STR_IS_EMPTY(ppeArg->pucContent)) {
  }
  else if (pieElementGetMode(ppeArg) == RMODE_PRE || pieElementIsPre(ppeArg)) {
    /*! pre-formatted content */
  }
  else if (pieElementIsCsv(ppeArg)) {
    /*! CSV-formatted content */
  }
  else if (pieElementGetMode(ppeArg) == RMODE_MD) { /* https://daringfireball.net/projects/markdown/syntax */
#ifdef WITH_MARKDOWN
    int i;
    xmlChar *pucA;

    pucA=ppeArg->pucContent;

    while (isspace(*pucA)) pucA++; /* skip leading spaces */

    switch (*pucA) {
    case '#':
      ppeArg->eType = header;
      for (ppeArg->iDepth=0; *pucA == (xmlChar)'#'; pucA++, ppeArg->iDepth++) {}
      for (i=ppeArg->iLength - 1; i > 0 && ppeArg->pucContent[i] == (xmlChar)'#'; i--) {
	ppeArg->pucContent[i] = (xmlChar)'\0';
      }
      break;

    case '_':
      if (StringBeginsWith((char *)pucA, "___")) {
	ppeArg->eType = ruler;
      }
      else {
      }
      break;

    case '*':
    case '+':
    case '-':
      if (StringBeginsWith((char *)pucA, "* * *") || StringBeginsWith((char *)pucA, "***") || StringBeginsWith((char *)pucA, "---") || StringBeginsWith((char *)pucA, "- - -")) {
	ppeArg->eType = ruler;
	for (i=0; pucA[i] == *pucA || pucA[i] == (xmlChar)' '; i++); /*   */
      }
      else {
	ppeArg->eType = listitem;
	ppeArg->iDepth = (index_t)(pucA - ppeArg->pucContent) / 2 + 1; /* https://spec.commonmark.org/0.29/#example-277 */
	pucA++;
      }
      break;

    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      if (pucA[1] == (xmlChar)'.' || pucA[1] == (xmlChar)')') {
	ppeArg->eType = listitem;
	ppeArg->iDepth = 1; /* there are no nested enumerations */
	/*!\todo handle enumeration start value */
	ppeArg->fEnum = TRUE;
	pucA += 2;
      }
      else {
      }
      break;
    }

    while (isspace(*pucA)) pucA++; /* skip spaces between markup and content */

    if (ppeArg->eType == ruler) {
      xmlFree(ppeArg->pucContent);
      ppeArg->pucContent = NULL;
      ppeArg->iLength = 0;
    }
    else if (pucA != ppeArg->pucContent) {
      xmlChar *pucRelease = ppeArg->pucContent;

      if (*pucA == (xmlChar)'\0') {
	/* there is no content after Markup */
	ppeArg->pucContent = NULL;
	ppeArg->iLength = 0;
      }
      else {
	ppeArg->pucContent = xmlStrdup(pucA);
	ppeArg->iLength = xmlStrlen(ppeArg->pucContent);
      }
      xmlFree(pucRelease);
    }
    pieElementWeight(ppeArg);
#endif
  }
  else {
    xmlChar *pucA;

    pucA=ppeArg->pucContent;

    while (isspace(*pucA)) pucA++; /* skip leading spaces */

    if (*pucA== (xmlChar)';') {
      if (pieElementGetMode(ppeArg) == RMODE_TABLE) {
	/* dont use ';' as "hidden" in CSV mode */
      }
      else {
	for (; *pucA== (xmlChar)';'; pucA++) {
	  ppeArg->iDepthHidden++;
	}
	while (isspace(*pucA)) pucA++; /* skip following spaces */
      }
    }

    switch (*pucA) {
    case '#':
      if (StringBeginsWith((char *)pucA, "#import")) { /* starts with import instruction */
	ppeArg->eType = import;
	pucA += xmlStrlen(BAD_CAST"#import");
      }
      else if (StringBeginsWith((char *)pucA, "#subst")) { /* starts with subst instruction */
	ppeArg->eType = subst;
	pucA += xmlStrlen(BAD_CAST"#subst");
      }
      break;

    case '*':
      ppeArg->eType = header;
      for (ppeArg->iDepth=0; *pucA == (xmlChar)'*'; pucA++, ppeArg->iDepth++) {}
      break;

    case '%':
      ppeArg->eType = header;
      ppeArg->iDepthHidden = 1;
      for (ppeArg->iDepth=0; *pucA == (xmlChar)'%'; pucA++, ppeArg->iDepth++) {}
      break;

    case '-':
      if (xmlStrEqual(pucA, BAD_CAST"----")) {
	ppeArg->eType = ruler;
      }
      else {
	ppeArg->eType = listitem;
	for (ppeArg->iDepth=0; *pucA == (xmlChar)'-'; pucA++, ppeArg->iDepth++) {}
      }
      break;

    case '+':
      ppeArg->eType = listitem;
      ppeArg->fEnum = TRUE;
      for (ppeArg->iDepth=0; *pucA == (xmlChar)'+'; pucA++, ppeArg->iDepth++) {}
      break;
    }

    while (isspace(*pucA)) pucA++; /* skip spaces between markup and content */

    if (pucA != ppeArg->pucContent) {
      xmlChar *pucRelease;

      assert(pucA != NULL);
      assert(ppeArg->pucContent != NULL);
      
      pucRelease = ppeArg->pucContent;
      if (*pucA == (xmlChar)'\0') {
	/* there is no content after Markup */
	ppeArg->pucContent = NULL;
      }
      else {
	ppeArg->pucContent = xmlStrdup(pucA);
      }
      xmlFree(pucRelease);
    }
    ppeArg->iLength = xmlStrlen(ppeArg->pucContent);
    pieElementWeight(ppeArg);
  }
  return TRUE;
} /* end of pieElementParse() */


/*!
Length
Empty line
EOF
*/
xmlChar *
DuplicateNextLine(char *pchArg, index_t *piArg)
{
  xmlChar *pucResult = NULL;
  index_t iLength;

  for (iLength=0; pchArg != NULL; iLength++) {
    if (pchArg[iLength] == (xmlChar)'\0' || pchArg[iLength] == (xmlChar)'\n' || pchArg[iLength] == (xmlChar)'\r') {
      pucResult = xmlStrndup(BAD_CAST pchArg, iLength);
      if (piArg) {
	*piArg = iLength;
      }
      break;
    }
  }
  return pucResult;
} /* end of DuplicateNextLine() */


/*! substitutions with accurate UTF-8/XML strings

https://en.wikipedia.org/wiki/UTF-8
https://www.duden.de/sprachwissen/rechtschreibregeln/anfuehrungszeichen
*/
BOOL_T
pieElementReplaceCharMarkup(pieTextElementPtr ppeArg)
{
  if (ppeArg != NULL && STR_IS_NOT_EMPTY(ppeArg->pucContent)) {
    int i, k, l;
    xmlChar* pucResult;
    xmlChar* pucC = ppeArg->pucContent; /* shortcut only */

    pucResult = BAD_CAST xmlMalloc((size_t) ppeArg->iLength * 2);

    for (k=i=0; pucC[i]; ) {
      int iCode;

      iCode = -1;
      l = 0;

      if (pucC[i] == (xmlChar)'<') {
	l++;
	if (pucC[i+l] == (xmlChar)'=') {
	  l++;
	  if (pucC[i+l] == (xmlChar)'>') { // STR_UTF8_LEFT_RIGHT_SINGLE_ARROW
	    l++;
	    iCode = 0x21D4;
	  }
	  else { // STR_UTF8_LEFTWARDS_SINGLE_ARROW
	    iCode = 0x21D0;
	  }
	}
	else if (pucC[i+l] == (xmlChar)'-') {
	  l++;
	  if (pucC[i+l] == (xmlChar)'>') { // STR_UTF8_LEFT_RIGHT_ARROW
	    l++;
	    iCode = 0x2194;
	  }
	  else { // STR_UTF8_LEFTWARDS_ARROW
	    iCode = 0x2190;
	  }
	}
	else if (pucC[i+l] == (xmlChar)'<') {
	  l++;
	  switch (ppeArg->eLang) {
	  case LANG_DE: // STR_UTF8_LEFT_DOUBLE_QUOTATION_MARK
	    iCode = 0x201C;
	    break;
	  case LANG_FR: //  STR_UTF8_RIGHT_POINTING_DOUBLE_ANGLE_QUOTATIONMARK
	    iCode = 0x00BB;
	    break;
	  default:		/* is LANG_EN, STR_UTF8_RIGHT_DOUBLE_QUOTATION_MARK */
	    iCode = 0x201D;
	  }
	}
      }
      else if (pucC[i] == (xmlChar)'>') {
	l++;
	if (pucC[i+l] == (xmlChar)'>') {
	  l++;
	  switch (ppeArg->eLang) {
	  case LANG_DE: // STR_UTF8_DOUBLE_LOW_9_QUOTATION_MARK
	    iCode = 0x201E;
	    break;
	  case LANG_FR: // STR_UTF8_LEFT_POINTING_DOUBLE_ANGLE_QUOTATIONMARK
	    iCode = 0x00AB;
	    break;
	  default:		/* is LANG_EN, STR_UTF8_LEFT_DOUBLE_QUOTATION_MARK */
	    iCode = 0x201C;
	  }
	}
      }
      else if (pucC[i] == (xmlChar)'=') {
	l++;
	if (pucC[i+l] == (xmlChar)'>') { // STR_UTF8_RIGHTWARDS_SINGLE_ARROW
	  l++;
	  iCode = 0x21D2;
	}
      }
      else if (pucC[i] == (xmlChar)'-') {
	l++;
	if (pucC[i+l] == (xmlChar)'>') { // STR_UTF8_RIGHTWARDS_ARROW
	  l++;
	  iCode = 0x2192;
	}
	else if (pucC[i+l] == (xmlChar)'-') {
	  l++;
	  if (pucC[i+l] == (xmlChar)'-') { // STR_UTF8_EM_DASH
	    l++;
	    iCode = 0x2014;
	  }
	  else { // STR_UTF8_EN_DASH
	    iCode = 0x2013;
	  }
	}
      }

      if (iCode > -1 && l > 0) {
	/* numeric character reference detected */
	int j;

	j = xmlCopyCharMultiByte(&pucResult[k], iCode);
	assert(j > 0 && j < 8);
	//assert(j <= l);

	k += j;
	i += l;
      }
      else {
	pucResult[k] = pucC[i];
	k++;
	i++;
      }
    }
    pucResult[k] = (xmlChar)'\0';
    xmlFree(ppeArg->pucContent);
    ppeArg->pucContent = pucResult;
  }
  return TRUE;
} /* end of pieElementReplaceCharMarkup() */


/*! wrapper code for StringDecodeNumericCharsNew()
*/
BOOL_T
pieElementReplaceCharNumerics(pieTextElementPtr ppeArg)
{
  if (ppeArg != NULL && STR_IS_NOT_EMPTY(ppeArg->pucContent)) {
    xmlChar* pucT;

    if ((pucT = StringDecodeNumericCharsNew(ppeArg->pucContent))) {
      xmlFree(ppeArg->pucContent);
      ppeArg->pucContent = pucT;
    }
  }
  return TRUE;
} /* end of pieElementEncodeEntities() */


/*! wrapper code for xmlEncodeEntitiesReentrant() substitutions with accurate UTF-8/XML strings

\deprecated not required
*/
BOOL_T
pieElementEncodeEntities(pieTextElementPtr ppeArg)
{
  if (ppeArg != NULL && STR_IS_NOT_EMPTY(ppeArg->pucContent)) {
    xmlChar* pucT;

    if ((pucT = StringEncodeXmlDefaultEntitiesNew(ppeArg->pucContent))) {
      xmlFree(ppeArg->pucContent);
      ppeArg->pucContent = pucT;
    }
  }
  return TRUE;
} /* end of pieElementEncodeEntities() */


/*!
*/
int
pieElementStrnlenEmpty(xmlChar *pucArg, int iArg)
{
  if (pucArg) {
    int i;

    for (i = 0; iArg < 0 || i < iArg; i++) {
      switch (pucArg[i]) {
      case '\n':
      case '\r':
      case '\t':
      case ' ':
	break;
      case '\0':
	return i;
	break;
      default:
	return -1; /* its not empty */
	break;
      }
    }
  }

  return 0;
}
/* end of pieElementStrnlenEmpty() */


/*! makes elements content XML-conformant and
\param ppeT element to use
\return a new node pointer or NULL if failed
*/
xmlNodePtr
pieElementToDOM(pieTextElementPtr ppeT)
{
  xmlNodePtr pndResult = NULL;

  if (ppeT) {
    xmlChar* pucC;

    pucC = pieElementGetBeginPtr(ppeT); /* shortcut */

    /*! build to result DOM */
    if (pieElementGetMode(ppeT) == RMODE_PRE || pieElementIsPre(ppeT)) {
      pndResult = xmlNewNode(NULL, NAME_PIE_PRE);
      xmlAddChild(pndResult, xmlNewText(pucC));
    }
    else if (pieElementGetMode(ppeT) == RMODE_TABLE) {
      pndResult = xmlNewNode(NULL, (pieElementIsHeader(ppeT) ? NAME_PIE_HEADER : NAME_PIE_PAR));
      if (ppeT->iWeight > 1) {
	xmlSetProp(pndResult, BAD_CAST"impact", BAD_CAST((ppeT->iWeight > 2) ? "1" : "2"));
      }
      xmlAddChild(pndResult, xmlNewText(pucC));
    }
    else {
      if (pieElementIsImport(ppeT)) {
	pndResult = xmlNewNode(NULL, NAME_PIE_IMPORT);
	if (STR_IS_NOT_EMPTY(pucC)) {
	  int i;
	  int j;
	  char* pchT;
	  char delimiter[] = ",;";

	  for (i=j=0; ! isend(pucC[i]); i++) { /* clean content string */
	    switch (pucC[i]) {
	    case '(':
	    case ')':
	      break;
	    default:
	      pucC[j] = pucC[i];
	      j++;
	    }
	  }
	  pucC[j] = (xmlChar)'\0';

	  for (i=0, pchT = strtok((char*)pucC, delimiter); pchT != NULL; i++) {
	    switch (i) {
	    case 0:
	      resPathRemoveQuotes(BAD_CAST pchT);
	      if (STR_IS_NOT_EMPTY(pchT)) {
		xmlSetProp(pndResult, BAD_CAST "name", BAD_CAST pchT);
	      }
	      break;
	    case 1:
	      if (STR_IS_NOT_EMPTY(pchT)) {
		xmlSetProp(pndResult, BAD_CAST "type", BAD_CAST pchT);
	      }
	      break;
	    case 2:
	      if (STR_IS_NOT_EMPTY(pchT)) {
		xmlSetProp(pndResult, BAD_CAST "base", BAD_CAST pchT);
	      }
	      break;
	    default:
	      break;
	    }
	    pchT = strtok(NULL, delimiter);
	  }
	}
      }
      else if (pieElementIsSubst(ppeT)) {
	pndResult = xmlNewNode(NULL, NAME_SUBST);
	if (STR_IS_NOT_EMPTY(pucC)) {
	  int i;
	  int j;
	  char* pchT;
	  char delimiter[] = ",;=";

	  for (i=j=0; ! isend(pucC[i]); i++) { /* clean content string */
	    switch (pucC[i]) {
	    case '(':
	    case ')':
	      break;
	    default:
	      pucC[j] = pucC[i];
	      j++;
	    }
	  }
	  pucC[j] = (xmlChar)'\0';

	  for (i=0, pchT = strtok((char*)pucC, delimiter); pchT != NULL; i++) {
	    char* pcName = "name";

	    switch (i) {
	    case 0:
	      StringRemovePairQuotes(BAD_CAST pchT);
	      if (STR_IS_NOT_EMPTY(pchT)) {
		xmlSetProp(pndResult, BAD_CAST "string", BAD_CAST pchT);
	      }
	      break;
	    case 1:
	      if (STR_IS_NOT_EMPTY(pchT)) {
		pcName = pchT;
	      }
	      break;
	    case 2:
	      StringRemovePairQuotes(BAD_CAST pchT);
	      if (STR_IS_NOT_EMPTY(pchT)) {
		xmlSetProp(pndResult, BAD_CAST pcName, BAD_CAST pchT);
	      }
	      break;
	    default:
	      break;
	    }
	    pchT = strtok(NULL, delimiter);
	  }
	}
      }
      else if (pieElementIsHeader(ppeT)) {
	pndResult = xmlNewNode(NULL, NAME_PIE_SECTION);
	if (STR_IS_NOT_EMPTY(pucC)) {
	  xmlNodePtr pndH = NULL;

	  pndH = xmlNewNode(NULL, NAME_PIE_HEADER);
	  if (ppeT->iWeight > 1) {
	    xmlSetProp(pndResult, BAD_CAST"impact", BAD_CAST((ppeT->iWeight > 2) ? "1" : "2"));
	  }
	  xmlAddChild(pndH, xmlNewText(pucC));
	  xmlAddChild(pndResult, pndH);
	  if (ppeT->iDepthHidden > 0) {
	    xmlSetProp(pndH, BAD_CAST "hidden", BAD_CAST"1");
	  }
	}
      }
      else {
	if (pieElementIsListItem(ppeT)) {
	  if (STR_IS_NOT_EMPTY(pucC)) {
	    pndResult = xmlNewNode(NULL, NAME_PIE_PAR);
	    xmlAddChild(pndResult, xmlNewText(pucC));
	  }
	}
	else if (pieElementIsRuler(ppeT)) {
	  pndResult = xmlNewNode(NULL, NAME_PIE_RULER);
	}
	else if (pieElementIsPre(ppeT)) {
	  if (STR_IS_NOT_EMPTY(pucC)) {
	    pndResult = xmlNewNode(NULL, NAME_PIE_PRE);
	    xmlAddChild(pndResult, xmlNewText(pucC));
	  }
	}
	else if (pieElementIsCsv(ppeT)) {
	  if (STR_IS_NOT_EMPTY(pucC)) {
	    pndResult = xmlNewNode(NULL, NAME_PIE_IMPORT);
	    xmlSetProp(pndResult, BAD_CAST "type", BAD_CAST"csv");
	    xmlAddChild(pndResult, xmlNewText(pucC));
	  }
	}
	else if (pieElementIsScript(ppeT)) {
	  if (STR_IS_NOT_EMPTY(pucC)) {
	    pndResult = xmlNewNode(NULL, NAME_PIE_IMPORT);
	    xmlSetProp(pndResult, BAD_CAST "type", BAD_CAST"script");
	    xmlAddChild(pndResult, xmlNewText(pucC));
	  }
	}
	else if (pieElementIsCxp(ppeT)) {
	  if (STR_IS_NOT_EMPTY(pucC)) {
	    xmlDocPtr pdocInclude = NULL;

	    pdocInclude = xmlParseMemory((const char*)pucC, xmlStrlen(pucC));
	    if (pdocInclude) {
	      xmlNodePtr pndRootInclude;

	      pndRootInclude = xmlDocGetRootElement(pdocInclude);
	      if (pndRootInclude) {
		/* replace pndArg with a copy of pndRootInclude */

		xmlUnlinkNode(pndRootInclude);

		pndResult = xmlNewNode(NULL, NAME_PIE_IMPORT);
		xmlSetProp(pndResult, BAD_CAST "type", BAD_CAST "cxp");
		xmlAddChild(pndResult, pndRootInclude);
	      }
	      xmlFreeDoc(pdocInclude);
	    }
	    else {
	      pndResult = xmlNewComment(BAD_CAST" XML parser error ");
	    }
	  }
	}
	else if (STR_IS_NOT_EMPTY(pucC)) {
	  pndResult = xmlNewNode(NULL, NAME_PIE_PAR);
	  if (pndResult) {
	    xmlChar* pucSep;

	    if (pieElementGetMode(ppeT) == RMODE_LINE
	      && (pucSep = EndOfDate(pucC)) != NULL
	      && (pucSep - pucC) > 3) { /* log format */

	      int l;
	      xmlChar* pucDate;

	      l = (int)(pucSep - pucC);
	      for (; isspace(*pucSep); pucSep++);

	      pucDate = xmlStrndup(pucC, l);
	      if (pucDate) {
		xmlAddChild(pndResult, xmlNewText(pucSep));
		domSetPropEat(pndResult, BAD_CAST "date", pucDate);
	      }
	    }
	    else {
	      xmlAddChild(pndResult, xmlNewText(pucC));
	    }
	  }
	}
	else {
	  //pndResult = xmlNewPI(BAD_CAST"error", BAD_CAST"encoding");
	}

	if (pndResult != NULL && ppeT->iDepthHidden > 0) {
	  xmlChar mpucHidden[BUFFER_LENGTH];

	  xmlStrPrintf(mpucHidden, BUFFER_LENGTH, "%i", ppeT->iDepthHidden);
	  xmlSetProp(pndResult, BAD_CAST "hidden", mpucHidden);
	}
      }

      if (ppeT->iWeight > 1) {
	xmlSetProp(pndResult, BAD_CAST"impact", BAD_CAST((ppeT->iWeight > 2) ? "1" : "2"));
      }
    }
    /*\todo AddChildLogNew(pndHeader,pucC); */
  }

  return pndResult;
} /* end of pieElementToDOM() */


/*!
*/
xmlChar *
pieElementToPlain(pieTextElementPtr ppeArg)
{
  xmlChar *pucResult = NULL;

  if (ppeArg) {
    pucResult = BAD_CAST xmlMalloc((BUFFER_LENGTH + 1) * sizeof(xmlChar));

    switch (ppeArg->eType) {
    case par:
      xmlStrPrintf(pucResult, BUFFER_LENGTH, "Paragraph :: %s\n", (char *)ppeArg->pucContent);
      break;

    case header:
      xmlStrPrintf(pucResult, BUFFER_LENGTH, "Header :: %s\n", (char *)ppeArg->pucContent);
      break;

    case listitem:
      xmlStrPrintf(pucResult, BUFFER_LENGTH, "Item :: %s\n", (char *)ppeArg->pucContent);
      break;

    case quote:
      xmlStrPrintf(pucResult, BUFFER_LENGTH, "Quote :: %s\n", (char *)ppeArg->pucContent);
      break;

     case pre:
      xmlStrPrintf(pucResult, BUFFER_LENGTH, "Pre :: %s\n", (char *)ppeArg->pucContent);
      break;

   case ruler:
      xmlStrPrintf(pucResult, BUFFER_LENGTH, "Ruler :: ----\n");
      break;

    default:
      xmlStrPrintf(pucResult, BUFFER_LENGTH, "Undefined :: %s\n", (char *)ppeArg->pucContent);
    }
  }

  return pucResult;
}
/* end of pieElementToPlain() */


#ifdef TESTCODE
#include "test/test_pie_element.c"
#endif
