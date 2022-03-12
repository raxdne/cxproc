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

#include <libxml/tree.h>

#include "basics.h"
#include "utils.h"
#include <pie/calendar_element.h>

/*\todo s. http://unicode.org/reports/tr35/tr35-dates.html */

const xmlChar *dow[] = {
  BAD_CAST "sun",
  BAD_CAST "mon",
  BAD_CAST "tue",
  BAD_CAST "wed",
  BAD_CAST "thu",
  BAD_CAST "fri",
  BAD_CAST "sat",
  BAD_CAST "xxx", /* symbol for all days of week */
  NULL
};

const xmlChar *dow_de[] = {
  BAD_CAST "So",
  BAD_CAST "Mo",
  BAD_CAST "Di",
  BAD_CAST "Mi",
  BAD_CAST "Do",
  BAD_CAST "Fr",
  BAD_CAST "Sa",
  BAD_CAST "Xx", /* symbol for all days of week */
  NULL
};

const int miDayPerMonth[] = {-1,31,29,31,30,31,30,31,31,30,31,30,31,-1};

static time_t system_zeit = (time_t)0;

static struct tm timeNow;

long int number_of_today = 0;

pieCalendarElementType ceToday;

xmlChar *pucTodayTag = NULL;

xmlChar *pucTodayYear = NULL;

xmlChar *pucTodayMonth = NULL;

xmlChar *pucTodayWeek = NULL;

xmlChar *pucTodayDayYear = NULL;

xmlChar *pucTodayDayMonth = NULL;

xmlChar *pucTodayHour = NULL;

xmlChar *pucTodayMinute = NULL;

xmlChar *pucTodaySecond = NULL;

xmlChar *pucAbsoluteSecond = NULL;


static void
ceCleanup(void);

static BOOL_T
ScanDateIterationStepMultipy(pieCalendarElementPtr pceArg);

static BOOL_T
ScanDateIterationStepNext(pieCalendarElementPtr pceArg);

static BOOL_T
ScanDateIterationStepTo(pieCalendarElementPtr pceArg);

static BOOL_T
ScanDateIterationStepOffset(pieCalendarElementPtr pceArg);

static BOOL_T
UpdateCalendarElementDate(pieCalendarElementPtr pceArg);


/*!
*/
BOOL_T
ceInit(void)
{
  BOOL_T fResult = TRUE;
  
  /* mit Systemzeit initialisieren */
  time(&system_zeit);
  memcpy(&timeNow, localtime(&system_zeit), sizeof(struct tm));
  UpdateToday(NULL); /* update if CXP_DATE is set */

  /* register for exit() */
  if (atexit(ceCleanup) != 0) {
    exit(EXIT_FAILURE);
  }
  
  return fResult;
}
/* end of ceInit() */


/*! release allocated memory of static variables
*/
void
ceCleanup(void)
{
  /* release strings */
  xmlFree(pucTodayTag);
  xmlFree(pucTodayYear);
  xmlFree(pucTodayMonth);
  xmlFree(pucTodayWeek);
  xmlFree(pucTodayDayMonth);
  xmlFree(pucTodayDayYear);
  xmlFree(pucTodayHour);
  xmlFree(pucTodayMinute);
  xmlFree(pucTodaySecond);
  xmlFree(pucAbsoluteSecond);
}
/* end of utilsCleanup() */


/*!\return a new allocated calendar element
 */
pieCalendarElementPtr
CalendarElementNew(xmlChar *pucArg)
{
  pieCalendarElementPtr pceResult = NULL;

  pceResult = (pieCalendarElementPtr)xmlMalloc(sizeof(pieCalendarElementType));
  if (pceResult) {
    memset(pceResult, 0, sizeof(pieCalendarElementType));
    pceResult->iYear = -1;
    pceResult->iMonth = -1;
    pceResult->iDay = -1;
    pceResult->iWeek = -1;
    pceResult->iDayWeek = -1;
    pceResult->iStep = -1;

    pceResult->iHourA = -1;
    pceResult->iMinuteA = -1;
    pceResult->iSecondA = -1;

    pceResult->iHourB = -1;
    pceResult->iMinuteB = -1;
    pceResult->iSecondB = -1;

    if (STR_IS_NOT_EMPTY(pucArg) && EndOfDate(pucArg) > pucArg) {
      pceResult->pucDate = xmlStrdup(pucArg);
    }
  }
  return pceResult;
}
/* end of CalendarElementNew() */


/*! \return a freshly allocated copy of pceArg
 */
pieCalendarElementPtr
CalendarElementDup(pieCalendarElementPtr pceArg)
{
  pieCalendarElementPtr pceResult = NULL;

  if (pceArg) {
    pceResult = CalendarElementNew(NULL);
    if (pceResult) {
      memcpy(pceResult,pceArg,sizeof(pieCalendarElementType));
      if (pceArg->pucId) {
	pceResult->pucId = xmlStrdup(pceArg->pucId);
      }
      pceResult->pucDate = NULL;
      pceResult->pucSep = NULL;
      pceResult->pNext = NULL;
    }
  }
  return pceResult;
}
/* End of CalendarElementDup() */


/*! \return a freshly allocated copy of pceArg
 */
pieCalendarElementPtr
_CalendarElementReset(pieCalendarElementPtr pceArg)
{
  if (pceArg) {
    if (pceArg->pucId) {
      xmlFree(pceArg->pucId);
    }
    pceArg->pucDate = NULL;
    pceArg->pucSep = NULL;
    pceArg->pNext = NULL;
  }
  return pceArg;
}
/* End of CalendarElementReset() */


/*! \return a freshly allocated copy of pceArg
 */
BOOL_T
CalendarElementListAdd(pieCalendarElementPtr pceArgList, pieCalendarElementPtr pceArg)
{
  if (pceArg != NULL) {
    pieCalendarElementPtr pceT;

    for (pceT=pceArgList; pceT; pceT = pceT->pNext) {
      if (pceT->pNext == NULL) {
	pceT->pNext = pceArg;
	return TRUE;
      }
    }
  }
  return FALSE;
}
/* End of CalendarElementListAdd() */


/*! free a new allocated calendar element
 */
void
CalendarElementFree(pieCalendarElementPtr pceArg)
{
  xmlFree(pceArg->pucDate);
  xmlFree(pceArg->pucId);
  xmlFree(pceArg);
}
/* end of CalendarElementFree() */


/*! detect end of an usable date string

  \param pucArg pointer to start of a possible date string
  \return pointer to first character after the date string
 */
xmlChar *
EndOfDate(xmlChar *pucArg)
{
  xmlChar *pucResult = NULL;

  if (pucArg) {
    int ciChar;
    xmlChar *pucStart;
    xmlChar *pucEnd;

    for (pucStart = pucArg; isspace(*pucStart); pucStart++) { }

    for (ciChar = 0, pucEnd = pucStart; iscal(*pucEnd) || iscalx(*pucEnd); pucEnd++, ciChar++) { }

    if (ciChar > 3) {
      pucResult = pucEnd;
    }
  }
  return pucResult;
}
/* end of EndOfDate() */


/*! Prints

\param pceArg
*/
void
PrintCalendarElement(pieCalendarElementPtr pceArg)
{
  if (pceArg != NULL && pceArg->pndEntry != NULL) {
    PrintFormatLog(1, "Calendar element: %s/%s=%s, anchor=%li, count=%li, step=%li",
      pceArg->pndEntry->parent->name,
      pceArg->pndEntry->name,
      pceArg->pucDate,
      pceArg->iAnchor,
      pceArg->iCount,
      pceArg->iStep);
  }
} /* end of PrintCalendarElement() */


/*! updates all date fields in pceArg

\param pceArg
\return TRUE or FALSE in case of error
 */
BOOL_T
UpdateCalendarElementDate(pieCalendarElementPtr pceArg)
{
  BOOL_T fResult = FALSE;
  
  if (pceArg) {
    struct tm tmNew;
    time_t iTime;

    iTime = (((time_t)pceArg->iAnchor + 1) * 24 * 60 * 60);
    memcpy(&tmNew, localtime(&iTime), sizeof(struct tm));

    pceArg->iYear  = tmNew.tm_year + 1900;
    pceArg->iMonth = tmNew.tm_mon + 1;
    pceArg->iDay   = tmNew.tm_mday;

    pceArg->eTypeDate = DATE_DAY;
    fResult = TRUE;
  }
  return fResult;
} /* end of UpdateCalendarElementDate() */


/*! Scans 'pceArgResult' for a datum description and write the results to
  the given addresses.

  skips further scans if result pointer is NULL (speedup for
  calGetYearMinMax())

\param pceArgResult
\return NULL in case of error, else pointer to xmlChar after date expression string

\todo find date string in any position of pceArgResult->pucDate
 */
BOOL_T
ScanCalendarElementDate(pieCalendarElementPtr pceArgResult)
{
  char b[BUFFER_LENGTH];
  date_t eTypeDate;
  int y, m, w, d, d_week;
  xmlChar *pucGcal;
  xmlChar *pucDow = NULL;
  xmlChar *pucSepT   = NULL;
  xmlChar *pucSepFirst = NULL;
  xmlChar *pucSepEnd   = NULL;
  struct tm *ptm = NULL;

  y = m = w = d = d_week = -1;

  if (pceArgResult == NULL) {
    return FALSE;
  }
  pceArgResult->eTypeDate = DATE_ERROR;

  if (pceArgResult->pucDate == NULL || xmlStrlen(pceArgResult->pucDate) < 4) {
    PrintFormatLog(2,"Ignore '%s', too short", pceArgResult->pucDate);
    return FALSE;
  }

  if (pceArgResult->pucSep != NULL && calConcatNextDate(pceArgResult->pucDate) == NULL) {
    PrintFormatLog(4,"No next date found '%s'", pceArgResult->pucDate);
    return FALSE;
  }

  for (pucGcal = pceArgResult->pucDate; isspace(*pucGcal);pucGcal++);

  if (isdigit(pucGcal[0]) && isdigit(pucGcal[1]) && isdigit(pucGcal[2]) && isdigit(pucGcal[3])) {
    /*!\todo check chars of year string [12][90][0-9][0-9] */
    /*!\todo check end chars string */

    if (isdigit(pucGcal[4]) && isdigit(pucGcal[5]) && isdigit(pucGcal[6]) && isdigit(pucGcal[7]) && isdigit(pucGcal[8]) && isdigit(pucGcal[9])) {
      /* system time "1311186519" */
      if (isdigit(pucGcal[10]) && isdigit(pucGcal[11]) && isdigit(pucGcal[12])) {
        /* system time "1311186519" */
        pucSepEnd = &pucGcal[13];
        eTypeDate = DATE_SYSTEM_MSEC;
      }
      else {
	pucSepEnd = &pucGcal[10];
	eTypeDate = DATE_SYSTEM;
      }
    }
    else if (pucGcal[4]=='-' && isdigit(pucGcal[5]) && isdigit(pucGcal[6])
	&& pucGcal[7]=='-' && isdigit(pucGcal[8]) && isdigit(pucGcal[9])
	&& !isdigit(pucGcal[10])) {
      /* ISO-Date "2006-10-22" */
      pucSepT   = &pucGcal[4];
      pucSepFirst   = &pucGcal[7];
      pucSepEnd = &pucGcal[10];
      eTypeDate = DATE_ISO;
    }
    else if (isdigit(pucGcal[4]) && isdigit(pucGcal[5])) {

      if (isdigit(pucGcal[6]) && isdigit(pucGcal[7])
	  && !isdigit(pucGcal[8])) {
	/* Day Date "20060603" */
	pucSepEnd = &pucGcal[8];
	eTypeDate = DATE_DAY;
      }
      else if (!isdigit(pucGcal[6])) {
	/* Month Date "200606" */
	pucSepEnd = &pucGcal[6];
	eTypeDate = DATE_MONTH;
      }
      else {
	/* default */
	return FALSE;
      }
    }
    else if (pucGcal[4]=='*'
	     && (pucGcal[5]=='w' || pucGcal[5]=='W')
	     && isdigit(pucGcal[6])) {
      /* Week Date "2006*w22" */
      eTypeDate = DATE_WEEK;
    }
    else if (pucGcal[4]=='@' && (pucGcal[5]=='e')) {
      /* Easter Date "2006@e+3" */
      eTypeDate = DATE_EASTER;
    }
    else {
      /* default */
      eTypeDate = DATE_YEAR;
    }
  }
  /*!\todo check chars of year string [12][90][0-9][0-9] */
  /*!\todo check end chars string */
  else if (isdigit(pucGcal[0])
	   && pucGcal[1]=='.'
	   && isdigit(pucGcal[2])
	   && pucGcal[3]=='.'
	   && isdigit(pucGcal[4]) && isdigit(pucGcal[5]) && isdigit(pucGcal[6]) && isdigit(pucGcal[7])
	   && !isdigit(pucGcal[8])) {
    /* german date: "1.1.2009" */
    pucSepT   = &pucGcal[1];
    pucSepFirst   = &pucGcal[3];
    pucSepEnd = &pucGcal[8];
    eTypeDate = DATE_GERMAN;
  }
  else if (pucGcal[0] >= '0' && pucGcal[0] < '4' && isdigit(pucGcal[1])
	   && pucGcal[2]=='.'
	   && isdigit(pucGcal[3])
	   && pucGcal[4]=='.'
	   && isdigit(pucGcal[5]) && isdigit(pucGcal[6]) && isdigit(pucGcal[7]) && isdigit(pucGcal[8])
	   && !isdigit(pucGcal[9])) {
    /* german: "01.1.2009" */
    pucSepT   = &pucGcal[2];
    pucSepFirst   = &pucGcal[4];
    pucSepEnd = &pucGcal[9];
    eTypeDate = DATE_GERMAN;
  }
  else if (isdigit(pucGcal[0])
	   && pucGcal[1]=='.'
	   && pucGcal[2] >= '0' && pucGcal[2] < '2' && isdigit(pucGcal[3])
	   && pucGcal[4]=='.'
	   && isdigit(pucGcal[5]) && isdigit(pucGcal[6]) && isdigit(pucGcal[7]) && isdigit(pucGcal[8])
	   && !isdigit(pucGcal[9])) {
    /* german: "1.01.2009" */
    pucSepT   = &pucGcal[1];
    pucSepFirst   = &pucGcal[4];
    pucSepEnd = &pucGcal[9];
    eTypeDate = DATE_GERMAN;
  }
  else if (pucGcal[0] >= '0' && pucGcal[0] < '4' && isdigit(pucGcal[1])
	   && pucGcal[2]=='.'
	   && pucGcal[3] >= '0' && pucGcal[3] < '2' && isdigit(pucGcal[4])
	   && pucGcal[5]=='.'
	   && isdigit(pucGcal[6]) && isdigit(pucGcal[7]) && isdigit(pucGcal[8]) && isdigit(pucGcal[9])
	   && !isdigit(pucGcal[10])) {
    /* german: "01.01.2009" */
    pucSepT   = &pucGcal[2];
    pucSepFirst   = &pucGcal[5];
    pucSepEnd = &pucGcal[10];
    eTypeDate = DATE_GERMAN;
  }
  else {
    /* default */
    return FALSE;
  }

  if (eTypeDate==DATE_SYSTEM || eTypeDate==DATE_SYSTEM_MSEC) {
    time_t tT;

    assert(xmlStrlen(pucGcal) == 10 || xmlStrlen(pucGcal) == 13);
    strncpy(b,(const char *)pucGcal,10);
    b[10] = '\0';
    assert(strlen(b) == 10);

    tT = (time_t)atol((const char *)b);
    ptm = localtime((const time_t *)(&tT));

    assert(ptm);
    y = ptm->tm_year + 1900;
    m = ptm->tm_mon + 1;
    d = ptm->tm_mday;
    d_week = ptm->tm_wday;
  }
  else {
    /*
      eval year string
    */
    if (eTypeDate==DATE_GERMAN) {
      strncpy(b,(const char *)pucSepFirst+1,pucSepEnd - pucSepFirst);
    }
    else if (eTypeDate==DATE_ISO) {
      strncpy(b,(const char *)pucGcal,pucSepT - pucGcal);
    }
    else {
      strncpy(b,(const char *)&pucGcal[0],4);
    }

    b[4]='\0';
    y = atoi(b);
    if (y==0 || (y>1900 && y<2032)) {
    }
    else {
      PrintFormatLog(2,"Ignore '%s': invalid year '%s'", pucGcal, b);
      return FALSE;
    }

    if (eTypeDate==DATE_YEAR) {
      /* year only */
      pucSepEnd = &pucGcal[4];
    }
    else if (eTypeDate==DATE_EASTER) {
      if (y>1900 && y<2032) {
	GetEasterSunday(y,&m,&d);
	pucSepEnd = &pucGcal[6];
      }
      else {
	pucSepEnd = &pucGcal[4];
      }
    }
    else if (eTypeDate==DATE_WEEK) {
      /*
	eval year/week string
      */
      xmlChar *pucE;

      pucE = (pucGcal[4] == '*') ? &pucGcal[6] : &pucGcal[5];

      w = (int) strtol((char *)pucE,(char **)&pucE,10);
      if (pceArgResult) {
	if (w>-1 && w<54) {
	}
	else if (w == 99) {
	}
	else {
	  PrintFormatLog(2,"Ignore '%s': invalid week '%s'", pucGcal, pucE);
	  return FALSE;
	}
      }
      else {
	/* this result isn't needed */
      }

      for (pucSepEnd=pucE; isalpha(*pucSepEnd); pucSepEnd++) {}
      if (pucSepEnd != pucE) {
	/* entry for a specified day of week found
	 */
	pucDow = xmlStrndup(pucE, (int)(pucSepEnd - pucE));
	d_week = GetDayOfWeekInt(pucDow);
	xmlFree(pucDow);
      }
    }
    else if (pceArgResult) {
      /*
	eval month string
      */
      if (eTypeDate==DATE_ISO) {
	strncpy(b,(const char *)&pucGcal[5],2);
	b[2]='\0';
      }
      else if (eTypeDate==DATE_GERMAN) {
	strncpy(b,(const char *)pucSepT+1,pucSepFirst - pucSepT - 1);
	b[pucSepFirst - pucSepT - 1]='\0';
      }
      else {
	strncpy(b,(const char *)&pucGcal[4],2);
	b[2]='\0';
      }
      m = atoi(b);
      if (m==0 || (m>0 && m<13)) {

	if (eTypeDate==DATE_MONTH) {
	  /* ready */
	  pucSepEnd = &pucGcal[6];
	}
	else if (pceArgResult) {
	  /*
	    eval day string
	  */
	  if (eTypeDate==DATE_ISO) {
	    strncpy(b,(const char *)&pucGcal[8],2);
	    pucSepEnd = &pucGcal[10];
	    b[2]='\0';
	  }
	  else if (eTypeDate==DATE_GERMAN) {
	    strncpy(b,(const char *)pucGcal,pucSepT - pucGcal);
	    b[pucSepT - pucGcal]='\0';
	  }
	  else {
	    strncpy(b,(const char *)&pucGcal[6],2);
	    pucSepEnd = &pucGcal[8];
	    b[2]='\0';
	  }
	  d = atoi(b);
	  if (d == 0 || (d > 0 && m == 0 && d <= miDayPerMonth[1]) || (d > 0 && d <= miDayPerMonth[m])) {
	  }
	  else {
	    PrintFormatLog(2,"Ignore '%s': invalid day '%s'", pucGcal, b);
	    return FALSE;
	  }
	}
      }
      else {
	PrintFormatLog(2,"Ignore '%s': invalid month '%s'", pucGcal, b);
	return FALSE;
      }
    }
  }

  /* Values for {y, (w,pucDow) | (m,d)} are set and checked */

  /* set results if not NULL pointer */
  if (pceArgResult) {
    pceArgResult->pucSep = pucSepEnd;

    pceArgResult->iAnchor =  0;
    pceArgResult->iStep   = -1;
    pceArgResult->iCount  = -1;

    pceArgResult->eTypeDate = eTypeDate;
    pceArgResult->iYear  = y;
    pceArgResult->iMonth = m;
    pceArgResult->iDay   = d;
    pceArgResult->iWeek  = w;
    pceArgResult->iDayWeek = d_week;

    if (ScanDateIterationStepOffset(pceArgResult)) {
      UpdateCalendarElementDate(pceArgResult);
    }
  }

  return (pceArgResult->eTypeDate != DATE_ERROR);
} /* end of ScanCalendarElementDate() */


/*! macro for string to time value.

\param P pointer to string
\return integer value
 */
#define strtotime(P) ((int) strtol((char *)P,(char **)&P,10))

 /*! Scans 'pucGcal' for a time description and write the results to
   the given addresses.

 \param
 \return NULL in case of error, else pointer to xmlChar after Time expression string
  */
xmlChar*
ScanTimeTripeString(xmlChar* pucArgGcal, int *piArgHour, int *piArgMinute, int *piArgSecond)
{
  xmlChar *pucResult = NULL;

  if (pucArgGcal != NULL && xmlStrlen(pucArgGcal) > 2) {
    xmlChar *pucT;
    int j;

    pucT = pucArgGcal;
    if (pucT[0] == 'T' || pucT[0] == '-') {
      pucT++;
    }
    else {
      while(pucT[0] == ' ') {
	pucT++;
      }
    }
    
    if (isdigit(pucT[0]) && isdigit(pucT[1])
	&& isdigit(pucT[2]) && isdigit(pucT[3])
	&& isdigit(pucT[4]) && isdigit(pucT[5])
	&& ! isdigit(pucT[6])) {
      /* 6 digits without separator chars */
      
      if (piArgHour != NULL && (j = (pucT[0] - '0') * 10 + (pucT[1] - '0')) < 24) {
	*piArgHour = j;

	if (piArgMinute != NULL && (j = (pucT[2] - '0') * 10 + (pucT[3] - '0')) < 60) {
	  *piArgMinute = j;

	  if (piArgSecond != NULL && (j = (pucT[4] - '0') * 10 + (pucT[5] - '0')) < 60) {
	    *piArgSecond = j;

	    pucResult = pucT + 6;
	  }
	}
      }
    }
    else if (isdigit(pucT[0]) && isdigit(pucT[1])
	     && pucT[2] == ':' && isdigit(pucT[3]) && isdigit(pucT[4])
	     && pucT[5] == ':' && isdigit(pucT[6]) && isdigit(pucT[7])
	     && ! isdigit(pucT[8])) {
      /* 6 digits ':' separator chars */
      
      if (piArgHour != NULL && (j = (pucT[0] - '0') * 10 + (pucT[1] - '0')) < 24) {
	*piArgHour = j;

	if (piArgMinute != NULL && (j = (pucT[3] - '0') * 10 + (pucT[4] - '0')) < 60) {
	  *piArgMinute = j;

	  if (piArgSecond != NULL && (j = (pucT[6] - '0') * 10 + (pucT[7] - '0')) < 60) {
	    *piArgSecond = j;

	    pucResult = pucT + 8;
	  }
	}
      }
    }
    else if (isdigit(pucT[0]) && isdigit(pucT[1])
	     && (pucT[2] == '.' || pucT[2] == ':') && isdigit(pucT[3]) && isdigit(pucT[4])
	     && ! isdigit(pucT[5])) {
      /* 4 digits '.' separator chars */
      
      if (piArgHour != NULL && (j = (pucT[0] - '0') * 10 + (pucT[1] - '0')) < 24) {
	*piArgHour = j;

	if (piArgMinute != NULL && (j = (pucT[3] - '0') * 10 + (pucT[4] - '0')) < 60) {
	  *piArgMinute = j;

	  if (piArgSecond != NULL) {
	    *piArgSecond = 0;
	  }

	  pucResult = pucT + 5;
	}
      }
    }
    else if (isdigit(pucT[0])
	     && pucT[1] == '.' && isdigit(pucT[2]) && isdigit(pucT[3])
	     && ! isdigit(pucT[4])) {
      /* 3 digits '.' separator chars */
      
      if (piArgHour != NULL && (j = (pucT[0] - '0')) < 24) {
	*piArgHour = j;

	if (piArgMinute != NULL && (j = (pucT[2] - '0') * 10 + (pucT[3] - '0')) < 60) {
	  *piArgMinute = j;

	  if (piArgSecond != NULL) {
	    *piArgSecond = 0;
	  }

	  pucResult = pucT + 4;
	}
      }
    }
  }

  if (pucResult == NULL) {	/* reset values */
    if (piArgHour) {
      *piArgHour = -1;
    }

    if (piArgMinute) {
      *piArgMinute = -1;
    }

    if (piArgSecond) {
      *piArgSecond = -1;
    }
  }
  
  return pucResult;
} /* end of ScanTimeTripeString() */


/*! Scans 'pucGcal' for a time description and write the results to
  the given addresses.

 \param
 \return NULL in case of error, else pointer to xmlChar after Time expression string
  */
BOOL_T
ScanCalendarElementTime(pieCalendarElementPtr pceArg)
{
  BOOL_T fResult = FALSE;

  if (pceArg) {
    int j;
    xmlChar *pucT;
    
    pucT = (pceArg->pucSep != NULL) ? pceArg->pucSep : pceArg->pucDate;

    if ((pucT = ScanTimeTripeString(pucT, &(pceArg->iHourA), &(pceArg->iMinuteA), &(pceArg->iSecondA)))) {

      /* detect time zone */
      if (pucT[0] == 'Z') {
	/* UTC time is also known as "Zulu" time */
      }
      else if (pucT[0] == '+') {
	/* time zone as offset */
	xmlChar *pucTT;

	for (pucTT = pucT + 1; isdigit(*pucTT); pucTT++);

	if (pucTT - pucT < 5) {
	  j = strtotime(pucT);
	  if (j > -13 && j < 15) {
	    if (pceArg) pceArg->iTimezoneOffset = j * 60;
	    if (pucTT[0] == ':') {
	      for (pucTT++, pucT = pucTT; isdigit(*pucTT); pucTT++);

	      if (pucTT - pucT < 3) {
		j = strtotime(pucT);
		if (j > -1 && j < 60) {
		  if (pceArg) pceArg->iTimezoneOffset += j;
		  pucT = pucTT;
		}
	      }
	    }
	  }
	}
      }
      else if (pucT[0] == '\xE2' && pucT[1] == '\x88' && pucT[2] == '\x92') { /*!\bug handle UTF-8 'Minus' */
	/* time zone as offset */
	xmlChar *pucTT;

	for (pucTT = pucT + 3; isdigit(*pucTT); pucTT++);

	if (pucTT - pucT < 5) {
	  j = strtotime(pucT);
	  if (j > -13 && j < 15) {
	    if (pceArg) pceArg->iTimezoneOffset = j * 60;
	    if (pucTT[0] == ':') {
	      for (pucTT++, pucT = pucTT; isdigit(*pucTT); pucTT++);

	      if (pucTT - pucT < 3) {
		j = strtotime(pucT);
		if (j > -1 && j < 60) {
		  if (pceArg) pceArg->iTimezoneOffset += j;
		  pucT = pucTT;
		}
	      }
	    }
	  }
	}
      }
#ifdef HAVE_PIE
      else if (isalpha(pucT[0])) {
	/* time zone as non-standardized Abbreviations */
	int tzT;

	tzT = tzGetNumber(pucT);
	if (tzT == PIE_TZ_END) {
	  tzT = PIE_TZ_UTC;
	}
	
	if (pceArg) {
	  pceArg->iTimezone = tzT;
	  pceArg->iTimezoneOffset = tzGetOffset(tzT);
	}
	
	while (isalpha(*pucT)) pucT++;
      }
#endif
      else if (pucT[0] == '-') {
	pucT = ScanTimeTripeString(pucT+1, &(pceArg->iHourB), &(pceArg->iMinuteB), &(pceArg->iSecondB));
      }
      else {
#if 0
	pucT++;
	j = strtotime(pucT);
	if (j > -1 && j < 60) {

	  if (pceArg) pceArg->iMinuteA = j;

	  if (pucT[0] == '-' && isdigit(pucT[1])) {
	    pucT = ScanCalendarElementTime(pucT+1,&ceT);
	    if (pceArg) {
	      pceArg->iHourB   = ceT.iHourA;
	      pceArg->iMinuteB = ceT.iMinuteA;
	      pceArg->iSecondB = ceT.iSecondA;
	    }
	  }
	  else if (pucT[0] == '.' && isdigit(pucT[1])) {
	    pucT++;
	    j = strtotime(pucT);
	    if (j > -1 && j < 60) {
	      if (pceArg) pceArg->iSecondA = j;
	    }
	  }
	}
#endif
      }
      pceArg->pucSep = pucT;
      fResult = (pceArg->pucSep != NULL);
    }
  }
  return fResult;
} /* end of ScanCalendarElementTime() */


/*!
\param
 */
BOOL_T
ScanDateIterationStepMultipy(pieCalendarElementPtr pceArg)
{
  if (pceArg) {
    xmlChar *pucSep = pceArg->pucSep;

    if (pceArg->iAnchor > 0 && pucSep != NULL && pucSep[0] == '.' && isdigit(pucSep[1])) {
      int i;
      char *pcEnd = NULL;

      i = strtol((char *)pucSep + 1,(char **)&pcEnd,10);
      if (i > 1 && i < 367) {
	if (pcEnd[0] == 'x' && isdigit(pcEnd[1])) {
	  int j;

	  j = strtol(pcEnd + 1,&pcEnd,10);
	  pceArg->iStep = i;
	  pceArg->iCount = j;
	}
	else {
	  //              pceArg->iStep = 1;
	  //              pceArg->iCount = i - 1;
	}
      }
      return TRUE;
    }
  }
  return FALSE;
}
/* end of ScanDateIterationStepMultipy() */


/*!
\param
 */
BOOL_T
ScanDateIterationStepNext(pieCalendarElementPtr pceArg)
{
  if (pceArg) {
    xmlChar *pucSep = pceArg->pucSep;

    if (pceArg->iAnchor > 0 && pucSep != NULL && pucSep[0] == ':' && isdigit(pucSep[1])) {
      /* add the next days to anchor */
      int i;
      char *pcEnd = NULL;

      i = strtol((char *)pucSep + 1,(char **)&pcEnd,10);
      if (i > 1 && i < 367) {
	if (pcEnd[0] == '.' && isdigit(pcEnd[1])) {
	  int j;

	  j = strtol(pcEnd + 1,&pcEnd,10);
	  pceArg->iStep = j;
	  pceArg->iCount = ((i-1) / j);
	}
	else {
	  pceArg->iStep = 1;
	  pceArg->iCount = i - 1;
	}
      }
      return TRUE;
    }
  }
  return FALSE;
}
/* end of ScanDateIterationStepNext() */


/*!
\param
 */
BOOL_T
ScanDateIterationStepTo(pieCalendarElementPtr pceArg)
{
  if (pceArg) {
    xmlChar *pucSep = pceArg->pucSep;

    if (pceArg->iAnchor > 0 && pucSep != NULL
	&& pucSep[0] == '#'
	&& (((pucSep[1]=='w' || pucSep[1]=='W') && isdigit(pucSep[2]))
	    || (isdigit(pucSep[1])))) {
      /* add the day range to anchor */
      int iDelta;
      xmlChar *pucDateB = xmlStrdup(pceArg->pucDate);
      xmlChar *pucDateNext;

      pucDateNext = calConcatNextDate(pucDateB);
      if (pucDateNext) {
	iDelta = GetDayAbsoluteStr(pucDateNext) - pceArg->iAnchor;
	if (iDelta > 0) {
	  pceArg->iStep = 1;
	  pceArg->iCount = iDelta;
	  /*!\todo add step size detection "#21.3" */
	}
	else {
	  PrintFormatLog(1,"Date range error: '%s'",pceArg->pucDate);
	}
      }
      xmlFree(pucDateB);
      return TRUE;
    }
  }
  return FALSE;
}
/* end of ScanDateIterationStepTo() */


/*!
\param
 */
BOOL_T
ScanDateIterationStepOffset(pieCalendarElementPtr pceArg)
{
  if (pceArg) {
    xmlChar *pucSep = pceArg->pucSep;

    if (pucSep != NULL && (pucSep[0] == '-' || pucSep[0] == '+') && isdigit(pucSep[1])
      && (pceArg->iAnchor > 0 || (pceArg->iAnchor = GetDayAbsolute(pceArg->iYear, pceArg->iMonth, pceArg->iDay, pceArg->iWeek, pceArg->iDayWeek)) > 0)) {
      /* add the day offset to anchor */
      pceArg->iAnchor += atoi((char *)pucSep);
      for (pceArg->pucSep += 2; isdigit(*pceArg->pucSep); pceArg->pucSep++);
      return TRUE;
    }
  }
  return FALSE;
}
/* end of ScanDateIterationStepOffset() */


/*!
  \param
  \return
 */
BOOL_T
DateIterationFollows(pieCalendarElementPtr pceArg)
{
  if (pceArg) {
    xmlChar *pucSep = pceArg->pucSep;

    return (pceArg->iAnchor > 0 && pucSep != NULL && (pucSep[0] == '-' || pucSep[0] == '+' || pucSep[0] == '.' || pucSep[0] == '#' || pucSep[0] == ':'));
  }
  return FALSE;
}
/* end of DateIterationFollows() */


/*! Scans 'pucGcal' for a datum iteration and write the results to
  the given addresses.

  \param
  \return
 */
BOOL_T
ScanDateIteration(pieCalendarElementPtr pceArg)
{
  BOOL_T fResult = FALSE;
  xmlChar *pucSep;

  if (pceArg == NULL) {
    return FALSE;
  }

  if (pceArg->pucSep == NULL && pceArg->eTypeDate != DATE_ERROR) {
    ScanCalendarElementDate(pceArg);
  }
  pucSep = pceArg->pucSep;

  if (pucSep==NULL || pceArg->eTypeDate == DATE_ERROR) {
    PrintFormatLog(1,"Date error: '%s'",pceArg->pucDate);
  }
  else if (pceArg->iYear < 0) {
    /*! no valid year */
  }
  else if (pceArg->iYear == 0) {
    /*! all years */
  }
  else {
    /*! valid year */

    if (pceArg->iMonth < 0) {
      /*! valid year but no valid month, test week */
      if (pceArg->iWeek < 0) {
	/*! valid year and no valid week */
      }
      else if (pceArg->iWeek == 99) {
	/*! valid year and all weeks */
	if (pceArg->iDayWeek < 0) {
	  /*! no valid day of week */
	}
	else if (pceArg->iDayWeek == 7) {
	  /*! valid year, valid week and all days of week */
	  pceArg->iAnchor = GetDayAbsolute(pceArg->iYear, 1, 1, -1, -1);
	  pceArg->iStep = 1;
	  pceArg->iCount = GetDayAbsolute(pceArg->iYear+1, 1, 1, -1, -1) - pceArg->iAnchor - 1;
	  fResult = TRUE;
	}
	else {
	  /*! valid year, valid week and valid days of week */
	  pceArg->iAnchor = GetDayAbsolute(pceArg->iYear, 1, 1, -1, -1) + (7 + pceArg->iDayWeek - GetDayOfWeek(1,1,pceArg->iYear));
	  pceArg->iStep = 7;
	  pceArg->iCount = 53;
	  fResult = TRUE;
	}
      }
      else {
	/*! valid year and valid week */
	if (pceArg->iDayWeek < 0) {
	  /*! no valid day of week */
	}
	else if (pceArg->iDayWeek == 7) {
	  /*! valid year, valid week and all days of week */
	  pceArg->iAnchor = GetDayAbsolute(pceArg->iYear, -1, -1, pceArg->iWeek, 0);
	  pceArg->iStep = -1;
	  pceArg->iCount = 6;
	  fResult = TRUE;
	}
	else {
	  /*! valid year, valid week and valid day of week */
	  pceArg->iAnchor = GetDayAbsolute(pceArg->iYear, -1, -1, pceArg->iWeek, pceArg->iDayWeek);
	  if (DateIterationFollows(pceArg)) {
	    if (ScanDateIterationStepOffset(pceArg)
		|| ScanDateIterationStepTo(pceArg)
		|| ScanDateIterationStepNext(pceArg)
		|| ScanDateIterationStepMultipy(pceArg)) {
	      fResult = TRUE;
	    }
	  }
	}
      }
    }
    else if (pceArg->iMonth == 0) {
      /*! valid year and all months */
      if (pceArg->iDay < 0) {
	/*! no valid day of month */
      }
      else if (pceArg->iDay == 0) {
	/*! valid year and all months and all days of month */
        pceArg->iAnchor = GetDayAbsolute(pceArg->iYear, 1, 1, -1, -1);
        pceArg->iStep = 1;
        pceArg->iCount = GetDayAbsolute(pceArg->iYear+1, 1, 1, -1, -1) - pceArg->iAnchor - 1;
	fResult = TRUE;
      }
      else {
	/*! valid year, all months and valid days of month */
      }
    }
    else {
      /*! valid year, valid month */
      if (pceArg->iDay < 0) {
	/*! no valid day of month */
      }
      else if (pceArg->iDay == 0) {
	/*! valid year and valid month and all days of month */
        pceArg->iAnchor = GetDayAbsolute(pceArg->iYear, pceArg->iMonth, 1, -1, -1);
        pceArg->iStep = 1;
        pceArg->iCount = GetDayAbsolute(pceArg->iYear, pceArg->iMonth+1, 1, -1, -1) - pceArg->iAnchor - 1;
	fResult = TRUE;
      }
      else {
	/*! valid year and valid month and valid day of month */
	pceArg->iAnchor = GetDayAbsolute(pceArg->iYear, pceArg->iMonth, pceArg->iDay, -1, -1);
	if (DateIterationFollows(pceArg)) {
	  if (ScanDateIterationStepOffset(pceArg)
	      || ScanDateIterationStepTo(pceArg)
	      || ScanDateIterationStepNext(pceArg)
	      || ScanDateIterationStepMultipy(pceArg)) {
	    fResult = TRUE;
	  }
	}
      }
    }
  }

  return fResult;
}
/* end of ScanDateIteration() */


/*! Assembles the next date part.

  \param pucArgGcal pointer to comma separated list of date strings
  \return pointer to a the modified pucGcal or NULL if no date string follows,
*/
xmlChar *
calConcatNextDate(xmlChar *pucArgGcal)
{
  xmlChar *pucE;
  xmlChar *pucB;		/* begin of next string */
  xmlChar *pucTmp;
  int l_new;
  int l_old;
  BOOL_T fWeek = FALSE;

  for (pucB=pucArgGcal, l_old=0; iscal(*pucB); pucB++, l_old++) {
    /*
      skip all chars in first calendar string
    */
    if (*pucB=='*')
      fWeek = TRUE;
  }

  if (*pucB == ',' || *pucB == '#') {

    for (pucB++, pucE=pucB; iscal(*pucE); pucE++) {
      /* skip all chars in second calendar string */
    }

    l_new = (int) (pucE - pucB);

    if (l_new > 0) {
      if (fWeek) {
	int c_b = tolower((int)*pucB);	/* speedup */
	if (*pucB == 'w' && isdigit(*(pucB+1))) {
	  /*
	    next date descriptor is a week number beginning with '*w'
	   */
	  pucTmp = xmlStrndup(pucArgGcal, 5);
	}
	else if (c_b == tolower((int)dow[0][0])
		 || c_b == tolower((int)dow[1][0])
		 || c_b == tolower((int)dow[2][0])
		 || c_b == tolower((int)dow[3][0])
		 || c_b == tolower((int)dow[4][0])
		 || c_b == tolower((int)dow[5][0])
		 || c_b == tolower((int)dow[6][0]) ) {
	  /*
	    next date descriptor is a day of week string
	   */
	  pucTmp = xmlStrndup(pucArgGcal, l_old - l_new);
	}
	else if (isdigit(pucB[0]) && isdigit(pucB[1]) && isdigit(pucB[2]) && isdigit(pucB[3])
		 && pucB[4]=='*' && pucB[5]=='w') {
	  /*
	    next date descriptor is a year number
	  */
	  int y = (int) strtol((char *)pucB,(char **)&pucE,10);
	  int w = (int) strtol((char *)(pucB+6),(char **)&pucE,10);
	  if ((y>1900 && y<3000) && (w>-1 && w<54)) {
	    pucTmp = xmlStrndup(pucArgGcal, l_old - l_new);
	  }
	  else {
	    PrintFormatLog(2,"No valid pattern '%s' in '%s'", pucB, pucArgGcal);
	    return NULL;
	  }
	}
	else if (isdigit(c_b)) {
	  /*
	    next date descriptor is a week number
	  */
	  int w = (int) strtol((char *)pucB,(char **)&pucE,10);
	  if (w>-1 && w<54 && isalpha((int)*pucE)) {
	    pucTmp = xmlStrndup(pucArgGcal, l_old - l_new);
	  }
	  else {
	    PrintFormatLog(2,"No valid pattern '%s' in '%s'", pucB, pucArgGcal);
	    return NULL;
	  }
	}
	else {
	  PrintFormatLog(2,"No valid pattern '%s' in '%s'", pucB, pucArgGcal);
	  return NULL;
	}
      }
      else if (isdigit(*pucB)) {
	if (l_new < 3) {
	  pucTmp = xmlStrndup(pucArgGcal, 6);
	  if (l_new < 2)  /* insert leading zero */
	    pucTmp = xmlStrcat(pucTmp, BAD_CAST "0");
	}
	else if (l_new < 5) {
	  pucTmp = xmlStrndup(pucArgGcal, 4);
	  if (l_new < 4)  /* insert leading zero */
	    pucTmp = xmlStrcat(pucTmp, BAD_CAST "0");
	}
	else if (l_new > 7) {
	  /* this is a complete new 'yyyymmdd' */
	  /* 	memmove(pucArgGcal,pucB,xmlStrlen(pucB)+1); */
	  pucTmp = xmlStrndup(pucArgGcal, 0);
	}
	else {
	  PrintFormatLog(2,"no valid extension '%s' in '%s'", pucB, pucArgGcal);
	  return NULL;
	}
      }
      else {
	PrintFormatLog(2,"Different types between '%s' in '%s'", pucB, pucArgGcal);
	return NULL;
      }
      pucTmp = xmlStrcat(pucTmp, pucB);
      memcpy(pucArgGcal,pucTmp,(size_t)xmlStrlen(pucTmp)+1);
      xmlFree(pucTmp);
      return pucArgGcal;
    }
    else {
      return NULL;
    }
  }

  return NULL;
}
/* end of calConcatNextDate() */


/*!
*/
long int
UpdateToday(xmlChar *pucArgToday)
{
  xmlChar mpucT[BUFFER_LENGTH];
  xmlChar *pucEnvDate;

  if (STR_IS_NOT_EMPTY(pucArgToday)) {
    pieCalendarElementPtr pceNew;

    pceNew = CalendarElementNew(pucArgToday);
    if (ScanCalendarElementDate(pceNew)) {
      PrintFormatLog(2,"Use '%s' as today",pucArgToday);
      if (pceNew->iYear > 1900) {
	timeNow.tm_year = pceNew->iYear - 1900;
	if (pceNew->iMonth > -1) {
	  timeNow.tm_mon  = pceNew->iMonth - 1;
	  if (pceNew->iDay > 0) {
	    timeNow.tm_mday = pceNew->iDay;
	  }
	}
      }
      mktime(&timeNow);

      /*!\todo handle time zone */
#if 0

    if (tzOffsetToUTC == 9999) {
      xmlChar *pucEnv;

      /* default timezone of runtime environment (shell, HTTP Server) */
      if ((pucEnv = cxpCtxtEnvGetValueByName(pccArg, BAD_CAST "CXP_TZ")) || (pucEnv = cxpCtxtEnvGetValueByName(pccArg, BAD_CAST "TZ"))) {
	tzOffsetToUTC = (int)(60.0f * tzGetOffset(tzGetNumber(pucEnv)));
      }
      else {
#ifdef _MSC_VER
	// s. http://stackoverflow.com/questions/12112419/getting-windows-time-zone-information-c-mfc
	// Get the timezone info.
	//    TIME_ZONE_INFORMATION TimeZoneInfo;
	//    GetTimeZoneInformation(&TimeZoneInfo);

	// https://msdn.microsoft.com/en-us/library/windows/desktop/ms724318(v=vs.85).aspx
	//PDYNAMIC_TIME_ZONE_INFORMATION pTimeZoneInformation;
	DYNAMIC_TIME_ZONE_INFORMATION TimeZoneInformation;
	GetDynamicTimeZoneInformation(&TimeZoneInformation);

	tzOffsetToUTC = -TimeZoneInformation.Bias;
#else
	tzOffsetToUTC = 0;
#endif
      }
    }
#endif
      
    }
    CalendarElementFree(pceNew);
  }
  else if ((pucEnvDate = BAD_CAST getenv("CXP_DATE")) != NULL) {
    UpdateToday(pucEnvDate);
  }
  else {
    PrintFormatLog(4,"Use system time as today");
    memcpy(&timeNow,localtime(&system_zeit),sizeof(struct tm));
  }

  number_of_today = GetDayAbsolute(timeNow.tm_year + 1900,timeNow.tm_mon + 1,timeNow.tm_mday,-1,-1);

  /*
  initialize strings
  */
  xmlFree(pucTodayTag);
  pucTodayTag = GetDateIsoString(system_zeit);

  xmlFree(pucTodayYear);
  xmlStrPrintf(mpucT,BUFFER_LENGTH, "%i",GetTodayYear());
  pucTodayYear = xmlStrdup(mpucT);

  xmlFree(pucTodayMonth);
  xmlStrPrintf(mpucT,BUFFER_LENGTH, "%02i",GetTodayMonth());
  pucTodayMonth = xmlStrdup(mpucT);

  xmlFree(pucTodayWeek);
  xmlStrPrintf(mpucT,BUFFER_LENGTH, "%02i",GetTodayWeek());
  pucTodayWeek = xmlStrdup(mpucT);

  xmlFree(pucTodayDayYear);
  xmlStrPrintf(mpucT,BUFFER_LENGTH, "%02i",GetTodayDayOfYear());
  pucTodayDayYear = xmlStrdup(mpucT);

  xmlFree(pucTodayDayMonth);
  xmlStrPrintf(mpucT,BUFFER_LENGTH, "%02i",GetTodayDayOfMonth());
  pucTodayDayMonth = xmlStrdup(mpucT);

  xmlFree(pucTodayHour);
  xmlStrPrintf(mpucT,BUFFER_LENGTH, "%02i",GetTodayHour());
  pucTodayHour = xmlStrdup(mpucT);

  xmlFree(pucTodayMinute);
  xmlStrPrintf(mpucT,BUFFER_LENGTH, "%02i",GetTodayMinute());
  pucTodayMinute = xmlStrdup(mpucT);

  xmlFree(pucTodaySecond);
  xmlStrPrintf(mpucT,BUFFER_LENGTH, "%02i",GetTodaySecond());
  pucTodaySecond = xmlStrdup(mpucT);

  xmlFree(pucAbsoluteSecond);
  xmlStrPrintf(mpucT,BUFFER_LENGTH, "%lli",(long long)system_zeit);
  pucAbsoluteSecond = xmlStrdup(mpucT);

  return number_of_today;
}
/* end of UpdateToday() */


/*!
*/
int
GetTodayTime(void)
{
  return (int)system_zeit;
}
/* end of GetTodayTime() */


/*!
*/
xmlChar *
GetTodayTag(void)
{
  return pucTodayTag;
}
/* end of GetTodayTag() */


/*!
*/
long int
GetToday(void)
{
  return number_of_today;
}
/* end of calGetToday() */


/*!
*/
int
GetTodayYear(void)
{
  return timeNow.tm_year + 1900;
}
/* end of GetTodayYear() */


/*!
*/
int
GetTodayMonth(void)
{
  return timeNow.tm_mon + 1;
}
/* end of GetTodayMonth() */


/*!
*/
int
GetTodayDayOfMonth(void)
{
  return timeNow.tm_mday;
}
/* end of GetTodayDayOfMonth() */


/*!
*/
int
GetTodayDayOfYear(void)
{
  return timeNow.tm_yday + 1;
}
/* end of GetTodayDayOfYear() */


/*!
*/
int
GetTodayWeek(void)
{
  return GetWeekOfYear(GetTodayDayOfMonth(),GetTodayMonth(),GetTodayYear());
}
/* end of GetTodayWeek() */


/*!
*/
int
GetTodayHour(void)
{
  return timeNow.tm_hour;
}
/* end of GetTodayHour() */


/*!
*/
int
GetTodayMinute(void)
{
  return timeNow.tm_min;
}
/* end of GetTodayMinute() */


/*!
*/
int
GetTodaySecond(void)
{
  return timeNow.tm_sec;
}
/* end of GetTodaySecond() */


/*! Decodes the given string and returns the according result of GetDayAbsolute()

\param pucGcal date string
*/
long int
GetDayAbsoluteStr(xmlChar *pucGcal)
{
  long int liResult = -1;

  if (pucGcal != NULL && xmlStrlen(pucGcal) > 4) {
    pieCalendarElementPtr pceT;

    pceT = CalendarElementNew(pucGcal);
    if (pceT) {
      ScanCalendarElementDate(pceT);
      liResult = GetDayAbsolute(pceT->iYear,pceT->iMonth,pceT->iDay,pceT->iWeek,pceT->iDayWeek);
      CalendarElementFree(pceT);
    }
  }
  return liResult;
}
/* end of GetDayAbsoluteStr() */


/*! \return the index number
*/
int
GetDayOfWeekInt(xmlChar *pucDow)
{
  int i;

  for (i=0; dow[i]; i++) {
    assert(dow_de[i]);
    if (xmlStrcasecmp(pucDow,dow[i]) == 0 || xmlStrcasecmp(pucDow,dow_de[i]) == 0) {
      return i;
    }
  }

  return -1;
}
/* end of GetDayOfWeekInt() */


/*! \return a string 
*/
xmlChar *
GetDiffDaysStrNew(xmlChar *pucArgAbs,xmlChar *pucArgDate)
{
  xmlChar *pucResult = NULL;

  if (pucArgAbs != NULL && pucArgDate != NULL) {
    xmlChar mpucT[BUFFER_LENGTH];

    int iDayStart = (int)GetDayAbsoluteStr(pucArgDate);
    int iDayEnd   = (int)strtol((char *)pucArgAbs,NULL,10);

    xmlStrPrintf(mpucT,BUFFER_LENGTH, "%i", iDayEnd - iDayStart);
    pucResult = xmlStrdup(mpucT);
  }

  return pucResult;
}
/* end of GetDiffDaysStrNew() */


/*! \return a string
*/
xmlChar *
GetDiffYearsStrNew(xmlChar *pucArgStart,xmlChar *pucArgEnd)
{
  xmlChar *pucResult = NULL;

  if (pucArgStart != NULL && pucArgEnd != NULL) {

    int iYearStart = (int)strtol((char *)pucArgStart,NULL,10);
    int iYearEnd   = (int)strtol((char *)pucArgEnd,NULL,10);

    if (iYearStart - iYearEnd > 0) {
      xmlChar mpucT[BUFFER_LENGTH];

      xmlStrPrintf(mpucT,BUFFER_LENGTH, "%i",iYearStart - iYearEnd);
      pucResult = xmlStrdup(mpucT);
    }
  }

  return pucResult;
}
/* end of GetDiffYearsStrNew() */



#ifdef TESTCODE
#include "test/test_calendar_element.c"
#endif

