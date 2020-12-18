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

   http://www.rfc-editor.org/rfc/rfc5545.txt
   s. Draft http://www.rfc-editor.org/rfc/rfc6321.txt

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
RemoveLineBreaks(char *pchArg, int iArgLength);

static size_t
getLineLength(char *pchArg);

static char *
getNextLine(char *pchArg);

static char *
getBlockType(char *pchArgTarget, char *pchArgBlockBegin, int iArgLength);

static char *
getBlockEnd(char *pchArgBlockBegin, int iArgLength);

static BOOL_T
addLine(xmlNodePtr pndArg, char *pchArg);

static char *
getNextBlock(xmlNodePtr pndArg, char *pchArg, int iArgLength);


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


/*! \return the number of chars before '\n'
*/
size_t
getLineLength(char *pchArg)
{
  char *pchT;

  for (pchT=pchArg; *pchT != '\0' && *pchT != '\n' && *pchT != '\r'; pchT++) {}

  return (size_t)(pchT - pchArg);
} /* end of getLineLength() */


/*! 
*/
char *
getNextLine(char *pchArg)
{
  char *pchT;
  char *pchResult = NULL;
  size_t l;

  for (pchT=pchArg; *pchT == '\n' || *pchT == '\r'; pchT++) {} /* skip leading linebreaks */

  l = getLineLength(pchT);
  if (l > 0) {
    for (pchResult=pchT + l; *pchResult == '\n' || *pchResult == '\r'; pchResult++) {}
  }

  return pchResult;
}
/* end of getNextLine() */


/*! 
*/
char *
getBlockType(char *pchArgTarget, char *pchArgBlockBegin, int iArgLength)
{
  char *pchT;
  int i;

  if (   pchArgBlockBegin[0]=='B'
      && pchArgBlockBegin[1]=='E'
      && pchArgBlockBegin[2]=='G'
      && pchArgBlockBegin[3]=='I'
      && pchArgBlockBegin[4]=='N'
      && pchArgBlockBegin[5]==':') {
    pchT = pchArgBlockBegin + 6;
  }
  else {
    pchT = pchArgBlockBegin;
  }

  for (i=0; i < iArgLength && isalpha(pchT[i]); i++) {
    pchArgTarget[i] = pchT[i];
  }
  pchArgTarget[i] = '\0';

  return pchArgTarget;
}
/* end of getBlockType() */


/*! 
*/
char *
getBlockEnd(char *pchArgBlockBegin, int iArgLength)
{
  char mchBlockEndNeedle[1024];

  mchBlockEndNeedle[0] = 'E';
  mchBlockEndNeedle[1] = 'N';
  mchBlockEndNeedle[2] = 'D';
  mchBlockEndNeedle[3] = ':';

  getBlockType(&mchBlockEndNeedle[4],pchArgBlockBegin,iArgLength);

  return (char *)Strnstr(BAD_CAST pchArgBlockBegin,iArgLength,BAD_CAST mchBlockEndNeedle);
}
/* end of getBlockEnd() */


/*! 
* \todo process numeric encoded entities?
*/
BOOL_T
addLine(xmlNodePtr pndArg, char *pchArg)
{
  size_t l = getLineLength(pchArg);
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

	    xmlNewChild(pndParameter,NULL,pucNameElementParam,pucText);	  
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
/* end of addLine() */



/*! \return a pointer to the begin of first next block
*/
char *
getNextBlock(xmlNodePtr pndArg, char *pchArg, int iArgLength)
{
  char *pchBlockBegin = NULL;

  if (iArgLength > 10) {
    char *pchBlockEnd;

    /*
    all block siblings
    */
    for (pchBlockBegin = pchArg; (pchBlockBegin = (char *)Strnstr(BAD_CAST pchBlockBegin,iArgLength,BAD_CAST"BEGIN:")); pchBlockBegin=pchBlockEnd) {

      pchBlockEnd = getBlockEnd(pchBlockBegin,iArgLength);
      if (pchBlockEnd) {
	int iBlockLength = (int) (pchBlockEnd - pchBlockBegin); /* set "return" value */
	if (iBlockLength > 0) {
	  char *pchLineNext;
	  char *pchBlockContent;
	  int ciDepth;
	  size_t iLengthName = getLineLength(pchBlockEnd) - 4;
	  xmlChar *pucName;
	  xmlNodePtr pndNew;
	  xmlNodePtr pndProperties;
	  xmlNodePtr pndComponents;

	  pucName = xmlStrndup(BAD_CAST (pchBlockEnd + 4), (int)iLengthName);
#if 1
	  PrintFormatLog(3,"Element %s %i Byte",pucName,iBlockLength);
#else
	  xmlChar *pucTT;
	  pucTT = xmlStrndup(pchBlockBegin,20);
	  fprintf(stderr,pucTT);
	  xmlFree(pucTT);
#endif
	  StringToLower((char *)pucName);
	  pndNew = xmlNewChild(pndArg,NULL,pucName,NULL);
	  xmlFree(pucName);
	  pndProperties = xmlNewChild(pndNew,NULL,BAD_CAST"properties",NULL);
	  pndComponents = xmlNewChild(pndNew,NULL,BAD_CAST"components",NULL);
	  
	  pchBlockContent = pchBlockBegin + getLineLength(pchBlockBegin);
	  while (*pchBlockContent == '\n' || *pchBlockContent == '\r') {
	    pchBlockContent++; /* skip leading linebreaks */
	  }
	  /*
	    all lines of current block
	  */
	  for (ciDepth=0, pchLineNext=pchBlockContent;
	       pchLineNext != NULL && pchLineNext < pchBlockEnd;
	       (pchLineNext = getNextLine(pchLineNext))) {
	    if (   pchLineNext[0]=='B'
		&& pchLineNext[1]=='E'
		&& pchLineNext[2]=='G'
		&& pchLineNext[3]=='I'
		&& pchLineNext[4]=='N'
		&& pchLineNext[5]==':') {
	      ciDepth++;
	    }
	    else if (   pchLineNext[0]=='E'
		     && pchLineNext[1]=='N'
		     && pchLineNext[2]=='D'
		     && pchLineNext[3]==':') {
	      ciDepth--;
	    }
	    else if (ciDepth==0) {
	      if (pchLineNext[0]=='X' && pchLineNext[1]=='-' && fExtensions == FALSE) {
		/* ignore */
	      }
	      else {
		addLine(pndProperties,pchLineNext);
	      }
	    }
	  }
	  /* all block childs */
	  getNextBlock(pndComponents,pchBlockContent, (int)(pchBlockEnd - pchBlockContent));
	}
      }
    }
  }
  return pchBlockBegin;
}
/* end of getNextBlock() */


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
	xmlNodePtr pndICalendar;
	int i = xmlStrlen(pucContent);
	pndICalendar = xmlNewChild(pndArg,NULL,BAD_CAST "iCalendar",NULL); 
	getNextBlock(pndICalendar,(char *)pucContent,i);
	fResult = TRUE;
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


#ifdef TESTCODE
#include "test/test_ics.c"
#endif

