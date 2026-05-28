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

/* 

   http://www.rfc-editor.org/rfc/rfc5545.txt
   s. Draft http://www.rfc-editor.org/rfc/rfc6321.txt

   https://en.wikipedia.org/wiki/ICalendar#/media/File:ICalendarSpecification.png
   https://en.wikipedia.org/wiki/XCal

 */


/* 
 */
#include <libxml/parser.h>

/* 
 */
#include "basics.h"
#include <res_node/res_node_io.h>
#include "dom.h"
#include "utils.h"
#include "plain_text.h"
#include <ics/ics.h>

static BOOL_T fExtensions = TRUE; /*! flag to include 'X-' elements in DOM */

static BOOL_T
detectNextBlock(const char *pcArg, const int i0, const int i1, int *piArgOuterBegin, int *piArgInnerBegin, int *piArgInnerEnd, int *piArgOuterEnd);

static BOOL_T
detectNextLine(const char *pcArg, const int i0, const int i1, int *piArgNameBegin, int *piArgNameEnd, int *piArgValueBegin, int *piArgValueEnd);

static BOOL_T
icsParseComponent(xmlNodePtr pndArg, xmlChar *pucArgContent, const int d0, const int d1);

static BOOL_T
icsParseProperty(xmlNodePtr pndArg, const char *pchArg, const int i0, const int i1);

static BOOL_T
icsParseParameters(xmlNodePtr pndArg, const char *pchArg, const int i0, const int i1);

static BOOL_T
UnfoldLines(char *pchArg, int iArgLength);

/*! unfold long lines
 */
BOOL_T
UnfoldLines(char *pchArg, int iArgLength)
{
  BOOL_T fResult = FALSE;

  if (pchArg != NULL && iArgLength > 0) {
    int i;

    for (i = 0; i < iArgLength; i++) {

      if ((iArgLength - i) > 1 && pchArg[i] == '\n' && (pchArg[i + 1] == '\t' || pchArg[i + 1] == ' ')) {
	memmove((void *)(&pchArg[i]), (void *)(&pchArg[i + 2]), iArgLength - (i + 1));
	i += 1;
      }
      else if ((iArgLength - i) > 2 && pchArg[i] == '\r' && pchArg[i + 1] == '\n' && (pchArg[i + 2] == '\t' || pchArg[i + 2] == ' ')) {
	memmove(&pchArg[i], &pchArg[i + 3], iArgLength - (i + 2));
	i += 2;
      }
      else if ((iArgLength - i) > 3 && pchArg[i] == '\\' && pchArg[i + 1] == 'n' && pchArg[i + 2] == '\n' && (pchArg[i + 3] == '\t' || pchArg[i + 3] == ' ')) {
	/* symbolic newline */
	memmove(&pchArg[i + 2], &pchArg[i + 4], iArgLength - (i + 3));
	i += 2;
      }
      else if ((iArgLength - i) > 3 && (pchArg[i] == '\t' || pchArg[i] == ' ') && pchArg[i + 1] == '\\' && pchArg[i+2] == '\n' && (pchArg[i + 3] == '\t' || pchArg[i + 3] == ' ')) {
	memmove(&pchArg[i], &pchArg[i + 4], iArgLength - (i + 3));
	i += 3;
      }
    }
    fResult = TRUE;
  }
  return fResult;
} /* end of UnfoldLines() */

/*! 
* \todo process numeric encoded entities?
*/
BOOL_T
icsParseParameters(xmlNodePtr pndArg, const char *pchArg, const int i0, const int i1)
{
  BOOL_T fResult = FALSE;
  size_t l = i1 - i0;

  if (pndArg != NULL && pchArg != NULL && l > 0) {
    int i;
    int iS; /* index of separator */
    int iP; /* index of parameters */
    xmlNodePtr pndParameterList = NULL;

    for (i = iS = iP = i0; i < l; i++) {
      if (pchArg[i] == ';') {
	if (iS > i0) {
	  xmlNodePtr pndParameter = NULL;
	  xmlChar *pucNameElement;
	  xmlChar *pucValueElement;

	  pucNameElement = xmlStrndup(BAD_CAST & pchArg[iP + 1], iS - iP - 1);
	  if (StringBeginsWith((char *)pucNameElement, "X-")) {
	    /* ignoring extensions yet */
	  }
	  else {
	    if (pndParameterList == NULL) {
	      pndParameterList = xmlNewChild(pndArg, NULL, BAD_CAST "parameters", NULL);
	    }

	    StringToLower((char *)pucNameElement);
	    pucValueElement = xmlStrndup(BAD_CAST & pchArg[iS + 1], i - iS - 1);

	    /*!\todo split "REQUEST-STATUS:statcode;statdesc" */

	    pndParameter = xmlNewChild(pndParameterList, NULL, pucNameElement, NULL);
	    xmlNewChild(pndParameter, NULL, BAD_CAST "text", pucValueElement);
	  }
	  xmlFree(pucValueElement);
	  xmlFree(pucNameElement);
	}
	iS = iP = i;
      }
      else if (pchArg[i] == '=') {
	iS = i;
      }
    }
  }
  return fResult;
} /* end of icsParseParameters() */

/*! 
* \todo process numeric encoded entities?
*/
BOOL_T
icsParseProperty(xmlNodePtr pndArg, const char *pchArg, const int i0, const int i1)
{
  BOOL_T fResult = FALSE;
  size_t l = i1 - i0;

  if (pndArg != NULL && pchArg != NULL && l > 0) {
    int i;
    int iS; /* index of separator */
    int iP; /* index of parameters */
    xmlChar *pucA;
    xmlNodePtr pndElement;

    pucA = xmlStrndup(BAD_CAST & pchArg[i0], l);
    if (UnfoldLines((char *)pucA, l)) {

      for (i = iS = iP = 0; i < l && iS < 1; i++) {
	if (pucA[i] == ';') {
	  if (iP == 0) {
	    iP = i;
	  }
	}
	else if (pucA[i] == ':') {
	  iS = i;
	}
      }

      if (iS > 0) {
	xmlChar *pucNameElement;
	xmlChar *pucValueElement;

	pucNameElement = xmlStrndup(pucA, (iP > 0) ? iP : iS);
	StringToLower((char *)pucNameElement);
	pndElement = xmlNewChild(pndArg, NULL, pucNameElement, NULL);

	if (iP > 0) {
	  icsParseParameters(pndElement, BAD_CAST pucA, iP, iS);
	}

	pucValueElement = xmlEncodeSpecialChars(pndArg->doc, BAD_CAST &pucA[iS + 1]);

	if (xmlStrEqual(pucNameElement, BAD_CAST "dtstart") || xmlStrEqual(pucNameElement, BAD_CAST "dtend") ||
	    xmlStrEqual(pucNameElement, BAD_CAST "dtstamp") || xmlStrEqual(pucNameElement, BAD_CAST "created") ||
	    xmlStrEqual(pucNameElement, BAD_CAST "last-modified") || xmlStrEqual(pucNameElement, BAD_CAST "due")) {
	  dt_t dt;

	  xmlNewChild(pndElement, NULL, BAD_CAST "date-time", pucValueElement);
	  if (xmlStrEqual(pucNameElement, BAD_CAST "dtend") && xmlStrlen(pucValueElement) == 8 &&
	      dt_parse_iso_date((const char *)pucValueElement, 8, &dt) > 7) {
	    xmlChar mpucT[10];

	    dt--; /* add end of whole-day events as a dedicated attribute 'iso' */
	    xmlStrPrintf(mpucT, 10, "%04i%02i%02i", dt_year(dt), dt_month(dt), dt_dom(dt));
	    xmlSetProp(pndElement, BAD_CAST "iso", mpucT);
	  }
	}
	else if (xmlStrEqual(pucNameElement, BAD_CAST "version")) {
	  xmlNewChild(pndElement, NULL, BAD_CAST "text", pucValueElement);
	}
	else if (xmlStrEqual(pucNameElement, BAD_CAST "rrule")) {
	  xmlNewChild(pndElement, NULL, BAD_CAST "recur", pucValueElement);
	}
	else {
	  char *e;
	  long li;

	  li = strtol((const char *)pucValueElement, &e, 10);
	  if (e != NULL && *e == '\0') {
	    xmlNewChild(pndElement, NULL, BAD_CAST "integer", pucValueElement);
	  }
	  else {
	    double d;

	    d = strtod((const char *)pucValueElement, &e);
	    if (e != NULL && *e == '\0') {
	      xmlNewChild(pndElement, NULL, BAD_CAST "float", pucValueElement);
	    }
	    else {
	      xmlNewChild(pndElement, NULL, BAD_CAST "text", pucValueElement);
	    }
	  }
	}
	xmlFree(pucA);
	xmlFree(pucValueElement);
	xmlFree(pucNameElement);
      }
    }
  }
  return fResult;
} /* end of icsParseProperty() */


/*! parse an iCal formatted content buffer of prnArg and append the result to pndArg
 */
BOOL_T
icsParse(xmlNodePtr pndArg, resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (pndArg != NULL && (resNodeIsFile(prnArg) || resNodeIsURL(prnArg))) {
    xmlChar *pucContent;

    PrintFormatLog(3, "Read ics file '%s'", resNodeGetNameNormalized(prnArg));

    pucContent = plainGetContextTextEat(prnArg,1024);
    if (pucContent) {
      fResult = icsParseString(pndArg, pucContent);
      xmlFree(pucContent);
    }
    else {
      PrintFormatLog(1,"Empty input");
      xmlSetProp(pndArg, BAD_CAST "type", BAD_CAST "error/file");
    }
  }
  return fResult;
}
/* end of icsParse() */


/*! parse an iCal formatted buffer
 */
BOOL_T
icsParseString(xmlNodePtr pndArg, xmlChar *pucArgContent)
{
  BOOL_T fResult = FALSE;

  if (pndArg != NULL && STR_IS_NOT_EMPTY(pucArgContent)) {
    xmlNodePtr pndICalendar;

    pndICalendar = xmlNewChild(pndArg, NULL, BAD_CAST "icalendar", NULL);
    fResult = icsParseComponent(pndICalendar, pucArgContent, 0, xmlStrlen(pucArgContent));
  }
  return fResult;
} /* end of icsParseString() */


/*! parse an iCal formatted buffer
 */
BOOL_T
icsParseComponent(xmlNodePtr pndArg, xmlChar *pucArgContent, const int d0, const int d1)
{
  BOOL_T fResult = FALSE;

  if (pndArg != NULL && STR_IS_NOT_EMPTY(pucArgContent)) {
    xmlNodePtr pndProperties = NULL;
    xmlNodePtr pndComponentChilds = NULL;

    int i0, i1;
    int j0, j1;
    int k0, k1;

    for (i0 = d0, i1 = d1, j0 = j1 = 0, k0 = k1 = 0; i0 < d1;) {
      /* iteration */

      if (detectNextBlock((const char *)pucArgContent, i0, i1, &j0, &k0, &k1, &j1)) {
	xmlNodePtr pndComponent;
	xmlChar *pucName;

	pucName = xmlStrndup(&pucArgContent[j0 + 6], k0 - (j0 + 6));
	StringToLower((char *)pucName);

	if (xmlStrEqual(pndArg->name, BAD_CAST "icalendar")) {
	  /* top element */
	  pndComponentChilds = xmlNewChild(pndArg, NULL, pucName, NULL);
	  fResult = icsParseComponent(pndComponentChilds, pucArgContent, k0, k1);
	}
	else {
	  if (pndComponentChilds == NULL) {
	    pndComponentChilds = xmlNewChild(pndArg, NULL, BAD_CAST "components", NULL);
	  }
	  /* append all child components*/

	  pndComponent = xmlNewChild(pndComponentChilds, NULL, pucName, NULL);
	  fResult = icsParseComponent(pndComponent, pucArgContent, k0, k1);
	}
	xmlFree(pucName);

	i0 = j1;
	i1 = d1;
      }
      else if (detectNextLine((const char *)pucArgContent, i0, i1, &k0, &k1, &j0, &j1)) {
	if (pndProperties == NULL) {
	  pndProperties = xmlNewChild(pndArg, NULL, BAD_CAST "properties", NULL);
	}
	fResult = icsParseProperty(pndProperties, (const char *)pucArgContent, k0, j1);
	i0 = j1;
	i1 = d1;
      }
      else if (j1 == i0) {
	break;
      }
      else {
	i0 = j1;
      }
    }
  }
  else {
    PrintFormatLog(1, "Empty input");
    xmlSetProp(pndArg, BAD_CAST "type", BAD_CAST "error/file");
  }
  return fResult;
} /* end of icsParseComponent() */


#define DETECT_COMPONENT(C) \
      if (StringBeginsWith(pcBegin, BAD_CAST C)) { \
	pcBegin += strlen(C); \
	k0 = (int)(pcBegin - pcArg); \
	if ((pcEnd = (char *) Strnstr(BAD_CAST pcBegin, i1 - (int)(pcBegin - pcArg), BAD_CAST "END:" C))) { \
	  k1 = (int)(pcEnd - pcArg); \
	  j1 = k1 + strlen("END:" C); \
	} \
	else {j1 = i1;} \
      }

/*! \return TRUE if an ICS block was detected
 */
BOOL_T
detectNextBlock(const char *pcArg, const int i0, const int i1, int *piArgOuterBegin, int *piArgInnerBegin, int *piArgInnerEnd, int *piArgOuterEnd)
{
  BOOL_T fResult = FALSE;

  if (STR_IS_NOT_EMPTY(pcArg) && i0 > -1 && i1 > i0 && piArgOuterBegin != NULL && piArgInnerBegin != NULL && piArgInnerEnd != NULL && piArgOuterEnd != NULL) {
    char *pcBegin;
    char *pcEnd;
    int j0, j1;
    int k0, k1;

    for (j0 = i0; j0 < i1 && (pcArg[j0] == '\n' || pcArg[j0] == '\r' || pcArg[j0] == ' ' || pcArg[j0] == '\t'); j0++);

    if (StringBeginsWith((char *)&pcArg[j0], BAD_CAST "BEGIN:")) {
      pcBegin = (char *)&pcArg[j0 + 6];

      DETECT_COMPONENT("VCALENDAR")
      else DETECT_COMPONENT("VEVENT") else DETECT_COMPONENT("DAYLIGHT") else DETECT_COMPONENT("STANDARD") else DETECT_COMPONENT("VALARM") else DETECT_COMPONENT(
	  "VFREEBUSY") else DETECT_COMPONENT("VJOURNAL") else DETECT_COMPONENT("VTIMEZONE") else DETECT_COMPONENT("VTODO")

      /*!\todo handle incomplete blocks ? */

      if (j0 < k0 && j0 < k1 && j0 < j1 && k0 < k1 && k0 < j1 && k1 < j1) {
	fResult = TRUE;
	*piArgOuterBegin = j0;
	*piArgInnerBegin = k0;
	*piArgInnerEnd = k1;
	*piArgOuterEnd = j1;
      }
    }
#if 0
    else if (StringBeginsWith((char *)&pcArg[j0], BAD_CAST "X-")) {
   int n0, n1; /* name indexes */
    int v0, v1; /* value indexes */

    detectNextLine(pcArg, j0, i1, &n0, &n1, &v0, &v1);
      fResult = detectNextBlock(pcArg,v1, i1 - v1, piArgOuterBegin, piArgInnerBegin, piArgInnerEnd, piArgOuterEnd);
    }
    else {}
#endif
  }
  return fResult;
} /* end of detectNextBlock() */


/*! \return TRUE if an usable ICS block was detected
 */
BOOL_T
detectNextLine(const char *pcArg, const int i0, const int i1, int *piArgNameBegin, int *piArgNameEnd, int *piArgValueBegin, int *piArgValueEnd)
{
  BOOL_T fResult = FALSE;

  if (STR_IS_NOT_EMPTY(pcArg) && i0 > -1 && i1 > i0 && piArgNameBegin != NULL && piArgNameEnd != NULL && piArgValueBegin != NULL && piArgValueEnd != NULL) {
    int n0, n1; /* name indexes */
    int v0, v1; /* value indexes */

    for (n0 = i0; n0 < i1 && (pcArg[n0] == '\n' || pcArg[n0] == '\r'); n0++);

    if (pcArg[n0 + 0] == 'B' && pcArg[n0 + 1] == 'E' && pcArg[n0 + 2] == 'G' && pcArg[n0 + 3] == 'I' && pcArg[n0 + 4] == 'N' && pcArg[n0 + 5] == ':') {
      /* BEGIN: block */
    }
    else if (pcArg[n0 + 0] == 'E' && pcArg[n0 + 1] == 'N' && pcArg[n0 + 2] == 'D' && pcArg[n0 + 3] == ':') {
      /* END: block */
    }
    else {
      for (v0 = v1 = n1 = n0; v1 < i1; v1++) {
	if (pcArg[v1] == ':') {
	  if (n1 == n0) {
	    /* first ':' */
	    n1 = v1 - 1;
	    v0 = v1 + 1;
	  }
	}
	else if (pcArg[v1] == '\n' && (pcArg[v1 + 1] == ' ' || pcArg[v1 + 1] == '\t')) {
	  /* folded line */
	  v1 += 2;
	}
	else if (pcArg[v1] == '\n' || pcArg[v1] == '\r' || pcArg[v1] == '\0') {
	  break;
	}
      }

      *piArgNameBegin = n0;
      *piArgValueEnd = v1;
      if (n1 > n0 && v0 > n1 && v1 > v0) {
	fResult = TRUE;
	*piArgNameEnd = n1;
	*piArgValueBegin = v0;
      }
      else {}
    }
  }
  return fResult;
} /* end of detectNextLine() */


#ifdef TESTCODE
#include "test/test_ics.c"
#endif

