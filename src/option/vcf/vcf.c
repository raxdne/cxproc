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
    s. http://tools.ietf.org/html/rfc2426
    http://en.wikipedia.org/wiki/VCard

    transformation into xCard format
 */


/* 
 */
#include <libxml/parser.h>

/* 
 */
#include "basics.h"
#include "utils.h"
#include <cxp/cxp_context.h>
#include "dom.h"
#include <pie/pie_text.h>
#include <vcf/vcf.h>


static BOOL_T
RemoveLineBreaks(char *pchArg, int iArgLength);

static int
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
      else {
	*pchA = *pchB;
	pchA++;
      }
    }
    *pchA = '\0';

  return TRUE;
}
/* end of RemoveLineBreaks() */

#if 0
/* 
*/
BOOL_T
RemoveChar(char *pchArg, char chArg)
{
    char *pchA;
    char *pchB;

    for (pchB=pchA=pchArg; *pchB != '\0'; pchB++) {
      if (*pchB == chArg) {
	/* skip this char */
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
#endif


/*! \return the number of chars before '\n'
*/
int
getLineLength(char *pchArg)
{
  char *pchT;

  for (pchT=pchArg; *pchT != '\0' && *pchT != '\n' && *pchT != '\r'; pchT++) {}

  return (pchT - pchArg);
}
/* end of getLineLength() */


/*! 
*/
char *
getNextLine(char *pchArg)
{
  char *pchT;
  char *pchResult = NULL;
  int l;

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
  //int iLength;
  char mchBlockEndNeedle[1024];

  mchBlockEndNeedle[0] = 'E';
  mchBlockEndNeedle[1] = 'N';
  mchBlockEndNeedle[2] = 'D';
  mchBlockEndNeedle[3] = ':';

  //iLength = 1024 - 4;
  getBlockType(&mchBlockEndNeedle[4],pchArgBlockBegin,iArgLength);

  return (char *)Strnstr(BAD_CAST pchArgBlockBegin,iArgLength,BAD_CAST mchBlockEndNeedle);
}
/* end of getBlockEnd() */


/*! 
*/
BOOL_T
addLine(xmlNodePtr pndArg, char *pchArg)
{
  int l = getLineLength(pchArg);
  char *pchSep = (char *)Strnstr(BAD_CAST pchArg,l,BAD_CAST ":");

  if (pchSep > pchArg) {
    xmlChar *pucA;
    xmlChar *pucB;
    xmlChar *pucNameElement = NULL;
    xmlChar *pucT;

    pucA = xmlStrndup(BAD_CAST pchArg,(pchSep - pchArg)); /* property and parametres */
    pucB = xmlStrndup(BAD_CAST(pchSep + 1),(pchArg + l - pchSep - 1)); /* attributes */
/*!\todo decode 7-bit value */

    /* detect property and parametres
    */

    if (xmlStrEqual(pucA,BAD_CAST"N")) { /* name */

      char *pndAttributeBegin;
      char *pchT;
      int i;
      int ciAttribute;

      l = xmlStrlen(pucB);

      for (i=0, ciAttribute=0, pchSep=pchT=pndAttributeBegin=(char *)pucB; i<=l; i++, pchT++) {

	assert(*pchT != ':');
	//assert(*pchT != '\0');

	if (*pchT == ';' || i == l) {

	  xmlChar *pucText = xmlStrndup(BAD_CAST(pndAttributeBegin),(pchT - pndAttributeBegin));
	  ciAttribute++;

	  /* "Family Name, Given Name, Additional Names, Honorific Prefixes, and Honorific Suffixes" */
	  xmlNewChild(pndArg,NULL, BAD_CAST (
	    (ciAttribute == 1) ? "firstname" : ((ciAttribute == 2) ? "name" : ((ciAttribute == 3) ? "additional" : ((ciAttribute == 4) ? "prefix" : ((ciAttribute == 5) ? "suffix" : "na"))))
	    ), pucText);

	  xmlFree(pucText);
	  pndAttributeBegin = pchT + 1;
	}
      }
    }
    else if (xmlStrEqual(pucA,BAD_CAST"FN")) { /* formatted text corresponding to the name */
      /* this is a single text attribute */
      xmlNewChild(pndArg,NULL,BAD_CAST"display",pucB);
      StringToId((char *)pucB);
      xmlSetProp(pndArg,BAD_CAST"id",pucB);
    }
    else if (xmlStrEqual(pucA,BAD_CAST"BDAY")) { /* birth date */
      /* this is a single text attribute */
      xmlNewChild(pndArg,NULL,BAD_CAST"birthday",pucB);
    }
    else if (xmlStrstr(pucA,BAD_CAST"ADR")==pucA) { /* delivery address */
      /* this is a single text attribute */
      for (pucT=pucB; *pucT != '\0'; pucT++) {
	if (*pucT == ';') {
	  *pucT = '\n';
	}
      }
      if (xmlStrstr(pucA,BAD_CAST"WORK")) { /*  */
	xmlNewChild(pndArg,NULL,BAD_CAST"work",pucB);
      }
      else { /*  */
	xmlNewChild(pndArg,NULL,BAD_CAST"private",pucB);
      }
    }
    else if (xmlStrstr(pucA,BAD_CAST"EMAIL")==pucA) { /*  */
      /* this is a single text attribute */
      if (xmlStrstr(pucA,BAD_CAST"WORK")) { /*  */
	xmlNewChild(pndArg,NULL,BAD_CAST"work",pucB);
      }
      else { /*  */
	xmlNewChild(pndArg,NULL,BAD_CAST"private",pucB);
      }
    }
    else if (xmlStrstr(pucA,BAD_CAST"TEL")==pucA) { /*  */
      /* this is a single text attribute */
      if (xmlStrstr(pucA,BAD_CAST"CELL")) { /*  */
	xmlNewChild(pndArg,NULL,BAD_CAST"mobile",pucB);
      }
      else if (xmlStrstr(pucA,BAD_CAST"WORK")) { /*  */
	xmlNewChild(pndArg,NULL,BAD_CAST"work",pucB);
      }
      else { /* if (xmlStrstr(pucA,BAD_CAST"HOME"))  */
	xmlNewChild(pndArg,NULL,BAD_CAST"private",pucB);
      }
    }
    else { /* */
      /* this is a single text attribute */
      StringToLower((char *)pucA);
      xmlNewChild(pndArg,NULL,pucA,pucB);
    }
    
    xmlFree(pucNameElement);
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
    //char *pchEnd = pchArg + iArgLength; /* pointer to end of char range, not '\0' terminated */

    /*
    all block siblings
    */
    for (pchBlockBegin = pchArg; (pchBlockBegin = (char *)Strnstr(BAD_CAST pchBlockBegin,iArgLength,BAD_CAST"BEGIN:")); pchBlockBegin=pchBlockEnd) {

      pchBlockEnd = getBlockEnd(pchBlockBegin,iArgLength);
      if (pchBlockEnd) {
	int iBlockLength = pchBlockEnd - pchBlockBegin; /* set "return" value */
	if (iBlockLength > 0) {
	  char *pchLineNext;
	  char *pchBlockContent;
	  int ciDepth;
	  int iLengthName = getLineLength(pchBlockEnd) - 4;
	  xmlChar *pucName;
	  xmlNodePtr pndNew;
	  xmlNodePtr pndNewAdr;
	  xmlNodePtr pndNewTel;
	  xmlNodePtr pndNewMail;

	  pucName = xmlStrndup(BAD_CAST (pchBlockEnd + 4), iLengthName);
#if 1
	  PrintFormatLog(3,"Element %s %i Byte",pucName,iBlockLength);
#else
	  xmlChar *pucTT;
	  pucTT = xmlStrndup(pchBlockBegin,20);
	  PrintFormatLog(1,pucTT);
	  xmlFree(pucTT);
#endif
	  StringToLower((char *)pucName);
	  pndNew = xmlNewChild(pndArg,NULL,pucName,NULL);
	  pndNewAdr = xmlNewChild(pndNew,NULL,BAD_CAST"address",NULL);
	  pndNewTel = xmlNewChild(pndNew,NULL,BAD_CAST"phone",NULL);
	  pndNewMail = xmlNewChild(pndNew,NULL,BAD_CAST"mail",NULL);
	  xmlFree(pucName);
	  
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
	      if (pchLineNext[0]=='X' && pchLineNext[1]=='-') {
		/* ignore */
	      }
	      else if (xmlStrstr(BAD_CAST pchLineNext,BAD_CAST"VERSION") == BAD_CAST pchLineNext) {
		/* ignore this */
	      }
	      else if (xmlStrstr(BAD_CAST pchLineNext,BAD_CAST"REV") == BAD_CAST pchLineNext) {
		/* ignore this */
	      }
	      else if (xmlStrstr(BAD_CAST pchLineNext,BAD_CAST"LABEL") == BAD_CAST pchLineNext) {
		/* ignore this */
	      }
	      else if (xmlStrstr(BAD_CAST pchLineNext,BAD_CAST"TEL") == BAD_CAST pchLineNext) {
		addLine(pndNewTel,pchLineNext);
	      }
	      else if (xmlStrstr(BAD_CAST pchLineNext,BAD_CAST"ADR") == BAD_CAST pchLineNext) {
		addLine(pndNewAdr,pchLineNext);
	      }
	      else if (xmlStrstr(BAD_CAST pchLineNext,BAD_CAST"EMAIL") == BAD_CAST pchLineNext) {
		addLine(pndNewMail,pchLineNext);
	      }
	      else {
		addLine(pndNew,pchLineNext);
	      }
	    }
	  }
	  /* all block childs */
	  getNextBlock(pndNew,pchBlockContent,pchBlockEnd - pchBlockContent);
	}
      }
    }
  }
  return pchBlockBegin;
}
/* end of getNextBlock() */


/*! Append the plain vCard formatted text from filehandle to the given pndArgParent

\param pndArgParent
\param pndArgSource node of pie element with options
\param pchArgSource
\param lang for language specific defaults

if in is NULL, the single text child node of pndArgSource is used as single input paragraph

 */
xmlNodePtr
vcfParse(xmlNodePtr pndArgParent, xmlNodePtr pndArgSource, char *pchArgSource, lang_t lang)
{
  //PrintFormatLog(1,"Read vcf file '%s'", pchArgNameFile);

  if (pchArgSource) {
    int l = strlen(pchArgSource);

    if (RemoveLineBreaks((char *)pchArgSource,l)) {
      int i = strlen(pchArgSource);
      getNextBlock(pndArgParent,pchArgSource,i);
    }
    //xmlFree(pucContent);
  }
  else {
    PrintFormatLog(1,"Empty input");
    xmlSetProp(pndArgParent, BAD_CAST "type", BAD_CAST "error/file");
  }

  return pndArgParent;
}
/* end of vcfParse() */


#ifdef TESTCODE
#include <res_node/res_node_io.h>
#include "test/test_vcf.c"
#endif

