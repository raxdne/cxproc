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
RemoveLineBreaks(char *pchArg, int iArgLength);


/*! removes the linebreaks at long lines

!\todo handle non conformant linebreaks (simple '\n')
*/
BOOL_T
RemoveLineBreaks(char *pchArg, int iArgLength)
{
    char *pchA;
    char *pchB;
    char *pchEnd = pchArg + iArgLength;

    for (pchB=pchA=pchArg; pchB < pchEnd; pchB++) {
      if (pchB[0]=='\r' && pchB[1]=='\n' && (pchB[2]=='\t' || pchB[2]==' ')) {
	pchB++;
	pchB++;
      }
      else if (pchB[0]=='\n' && (pchB[1]=='\t' || pchB[1]==' ')) {
	pchB++;
      }
      else if (pchB[0]=='\\' && pchB[1]==',') {
	/* ignore this backslash */
      }
      else {
	*pchA = *pchB;
	pchA++;
      }
    }
    *pchA = '\0';

  return TRUE;
}
/* end of RemoveLineBreaks() */


/*! 
* \todo process numeric encoded entities?
*/
BOOL_T
icsParseProperty(xmlNodePtr pndArg, const char *pchArgT, const int i0, const int i1)
{
  char *pchArg = &pchArgT[i0];
  size_t l = i1 - i0;
  char* pchSep = (char*)Strnstr(BAD_CAST pchArg, (int)l, BAD_CAST ":");

  if (pchSep > pchArg) {
    xmlChar *pucA;
    xmlChar *pucB;
    xmlChar *pucNameElement;
    xmlChar *pucT;
    xmlNodePtr pndElement;

    pucA = xmlStrndup(BAD_CAST pchArg,(int)(pchSep - pchArg));
    pucB = xmlStrndup(BAD_CAST(pchSep + 1), (int)(pchArg + (int)l - pchSep - 1));

    pucT = BAD_CAST xmlStrchr(pucA,(xmlChar)';');
    if (pucA[0]=='X' && pucA[1]=='-' && fExtensions == FALSE) {
      /* ignore this */
      return FALSE;
    }
    else if (pucT) {
      xmlNodePtr pndParameter;
      char *pchParameterBegin;
      char *pchT;
      int i;

      pucNameElement = xmlStrndup(pucA, (int)(pucT - pucA));
      StringToLower((char *)pucNameElement);
      l = xmlStrlen(pucA);

      pndElement = xmlNewChild(pndArg,NULL,pucNameElement,NULL);
      pndParameter = xmlNewChild(pndElement,NULL,BAD_CAST"parameters",NULL);

      for (i=0, pchSep=pchT=pchParameterBegin=(char *)(pucT+1); i<l; i++, pchT++) {
	if (*pchT == '=') {
	  pchSep = pchT;
	}
	else if (*pchT == ';' || *pchT == ':' || *pchT == '\0') {
	  if (pchParameterBegin[0]=='X' && pchParameterBegin[1]=='-' && fExtensions == FALSE) {
	    /* ignore this */
	  }
	  else if (pchSep - pchParameterBegin > 0) {
	    xmlChar *pucText = xmlStrndup(BAD_CAST(pchSep+1), (int)(pchT - pchSep - 1));
	    xmlChar *pucNameElementParam = xmlStrndup(BAD_CAST pchParameterBegin, (int)(pchSep - pchParameterBegin));
	    StringToLower((char *)pucNameElementParam);

	    xmlNewChild(xmlNewChild(pndParameter,NULL,pucNameElementParam,NULL),NULL,BAD_CAST"text",pucText);	  
	    xmlFree(pucNameElementParam);
	    xmlFree(pucText);
	    pchParameterBegin = pchT + 1;
	  }
	  if (*pchT == ':' || *pchT == '\0') {
	    break;
	  }
	}
      }
      xmlFree(pucNameElement);
    }
    else {
      StringToLower((char *)pucA);
      pndElement = xmlNewChild(pndArg,NULL,pucA,NULL);
    }

    pucT = BAD_CAST xmlStrchr(pucB,(xmlChar)';');
    if (xmlStrstr(pndElement->name,BAD_CAST"dt")==pndElement->name
	|| xmlStrstr(pndElement->name,BAD_CAST"created")==pndElement->name
	|| xmlStrstr(pndElement->name,BAD_CAST"last-modified")==pndElement->name) {
      xmlNewChild(pndElement,NULL,BAD_CAST"date-time",pucB);
    }
    else if (pucT) {
      xmlNodePtr pndParameter;
      char *pchParameterBegin;
      char *pchT;
      int i;

      l = xmlStrlen(pucB);

      if (xmlStrstr(pndElement->name,BAD_CAST"rrule")==pndElement->name) {
	pndParameter = xmlNewChild(pndElement,NULL,BAD_CAST"recur",NULL);
      }
      else {
	pndParameter = xmlNewChild(pndElement,NULL,BAD_CAST"parameters",NULL);
      }

      for (i=0, pchSep=pchT=pchParameterBegin=(char *)pucB; i<l; i++, pchT++) {
	if (*pchT == '=') {
	  pchSep = pchT;
	}
	else if (*pchT == ';' || *pchT == ':' || *pchT == '\0') {
	  if (pchParameterBegin[0]=='X' && pchParameterBegin[1]=='-' && fExtensions == FALSE) {
	    /* ignore this */
	  }
	  else if (pchSep - pchParameterBegin > 0) {
	    xmlChar* pucText = xmlStrndup(BAD_CAST(pchSep+1), (int)(pchT - pchSep - 1));
	    xmlChar *pucNameElementParam = xmlStrndup(BAD_CAST pchParameterBegin, (int)(pchSep - pchParameterBegin));

	    StringToLower((char *)pucNameElementParam);
	    pucT = StringEncodeXmlDefaultEntitiesNew(pucText);

	    xmlNewChild(pndParameter,NULL,pucNameElementParam, pucT);
	    xmlFree(pucT);
	    xmlFree(pucNameElementParam);
	    xmlFree(pucText);
	    pchParameterBegin = pchT + 1;
	  }
	  if (*pchT == ':' || *pchT == '\0') {
	    break;
	  }
	}
      }
    }
    else {
      pucT = StringEncodeXmlDefaultEntitiesNew(pucB);
      xmlNewChild(pndElement, NULL, BAD_CAST"text", pucT);
      xmlFree(pucT);
    }

    xmlFree(pucB);
    xmlFree(pucA);
  }

  return TRUE;
}
/* end of icsParseProperty() */


/*! parse an iCal formatted buffer
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
      int l = xmlStrlen(pucContent);

      if (RemoveLineBreaks((char *)pucContent,l)) {
	fResult = icsParseString(pndArg, pucContent);
      }
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

      if (detectNextBlock(pucArgContent, i0, i1, &j0, &k0, &k1, &j1)) {
	xmlNodePtr pndComponent;
	xmlChar *pucName;
	xmlChar *pucContent;

	pucName = xmlStrndup(&pucArgContent[j0 + 6], k0 - (j0 + 6));
	StringToLower(pucName);

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
      else if (detectNextLine(pucArgContent, i0, i1, &k0, &k1, &j0, &j1)) {
	if (pndProperties == NULL) {
	  pndProperties = xmlNewChild(pndArg, NULL, BAD_CAST "properties", NULL);
	}
	fResult = icsParseProperty(pndProperties, pucArgContent, k0, j1);
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
	if ((pcEnd = Strnstr(BAD_CAST pcBegin, i1 - (int)(pcBegin - pcArg), BAD_CAST "END:"C))) { \
	  k1 = (int)(pcEnd - pcArg); \
	  j1 = k1 + strlen("END:"C); \
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

    for (j0 = i0; j0 < i1 && (pcArg[j0] == '\n' || pcArg[j0] == '\r' || pcArg[j0] == ' '); j0++);

    if (StringBeginsWith(&pcArg[j0], BAD_CAST "BEGIN:")) {
      pcBegin = &pcArg[j0 + 6];
      PrintFormatLog(3, "ICS Block '%s'", pcBegin);

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
    else {}
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

    for (n0 = i0; n0 < i1 && (pcArg[n0] == '\n' || pcArg[n0] == '\r' || pcArg[n0] == ' '); n0++);

    if (pcArg[n0 + 0] == 'B' && pcArg[n0 + 1] == 'E' && pcArg[n0 + 2] == 'G' && pcArg[n0 + 3] == 'I' && pcArg[n0 + 4] == 'N' && pcArg[n0 + 5] == ':') {
      /* BEGIN: block */
    }
    else if (pcArg[n0 + 0] == 'E' && pcArg[n0 + 1] == 'N' && pcArg[n0 + 2] == 'D' && pcArg[n0 + 3] == ':') {
      /* END: block */
    }
    else {
      for (v0 = v1 = n1 = n0; v1 < i1 && pcArg[v1] != '\n' && pcArg[v1] != '\r' && pcArg[v1] != '\0'; v1++) {
	if (pcArg[v1] == ':') {
	  n1 = v1 - 1;
	  v0 = v1 + 1;
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

