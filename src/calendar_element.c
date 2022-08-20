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
#include "calendar_element.h"

/*\todo s. http://unicode.org/reports/tr35/tr35-dates.html */

const xmlChar *dow[] = {
  BAD_CAST "",
  BAD_CAST "mon",
  BAD_CAST "tue",
  BAD_CAST "wed",
  BAD_CAST "thu",
  BAD_CAST "fri",
  BAD_CAST "sat",
  BAD_CAST "sun",
  BAD_CAST "xxx", /* symbol for all days of week */
  NULL
};

static time_t system_zeit = (time_t)0;

static struct tm timeNow;

dt_t dtToday = 0;

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

static pieCalendarElementPtr
CalendarElementReset(pieCalendarElementPtr pceArg);


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
    if (CalendarElementUpdate(pceResult, pucArg)) {
    }
    else {
      CalendarElementFree(pceResult);
      pceResult = NULL;
    }
  }
  return pceResult;
} /* end of CalendarElementNew() */


/*!\return an updated calendar element
 */
pieCalendarElementPtr
CalendarElementUpdate(pieCalendarElementPtr pceArg, xmlChar* pucArg)
{
  if (pceArg) {
    CalendarElementReset(pceArg);
    if (STR_IS_NOT_EMPTY(pucArg)) {
      int i, j;

      for (i = 0; isspace(pucArg[i]); i++) {}

      for (j = 0; isiso8601(pucArg[i + j]); j++) {}

      if (j > 3) {
	pceArg->pucDate = xmlStrdup(&pucArg[i]);
      }
    }
  }
  return pceArg;
} /* end of CalendarElementUpdate() */


/*! \return a freshly allocated copy of pceArg
 */
pieCalendarElementPtr
CalendarElementDup(pieCalendarElementPtr pceArg)
{
  pieCalendarElementPtr pceResult = NULL;

  if (pceArg) {
    assert(pceArg->pNext == NULL);
    
    pceResult = CalendarElementNew(NULL);
    if (pceResult) {
      memcpy(pceResult,pceArg,sizeof(pieCalendarElementType));
      if (pceArg->pucId) {
	pceResult->pucId = xmlStrdup(pceArg->pucId);
      }
      if (pceArg->pucDate) {
	pceResult->pucDate = xmlStrdup(pceArg->pucDate);
      }
      pceResult->pucSep = NULL;
      pceResult->pNext = NULL;
    }
  }
  return pceResult;
} /* End of CalendarElementDup() */


/*! \return a freshly allocated copy of pceArg
 */
pieCalendarElementPtr
CalendarElementReset(pieCalendarElementPtr pceArg)
{
  if (pceArg) {

    if (pceArg->pucId) {
      xmlFree(pceArg->pucId);
      pceArg->pucId = NULL;
    }
    
    pceArg->pucSep = NULL;
    if (pceArg->pucDate) {
      xmlFree(pceArg->pucDate);
      pceArg->pucDate = NULL;
    }
    
    //memset(pceArg, 0, sizeof(pieCalendarElementType));

    pceArg->dtBegin = 0;
    pceArg->dtEnd   = pceArg->dtBegin;
    
    pceArg->period.y = 0;
    pceArg->period.m = 0;
    pceArg->period.d = 0;
    pceArg->period.w = 0;

    pceArg->iRecurrence = 0;
  }
  return pceArg;
} /* End of CalendarElementReset() */


/*! \return a freshly allocated copy of pceArg
 */
BOOL_T
CalendarElementListAdd(pieCalendarElementPtr pceArgList, pieCalendarElementPtr pceArg)
{
  if (pceArgList != NULL && pceArg != NULL) {
    if (pceArgList->pNext) {
      return CalendarElementListAdd(pceArgList->pNext,pceArg);
    }
    else {
      pceArgList->pNext = pceArg;
      return TRUE;
    }
  }
  return FALSE;
} /* End of CalendarElementListAdd() */


/*! free a new allocated calendar element
 */
void
CalendarElementFree(pieCalendarElementPtr pceArg)
{
  if (pceArg) {
    if (pceArg->pNext) {
      CalendarElementFree(pceArg->pNext);
    }
    CalendarElementReset(pceArg);
    xmlFree(pceArg);
  }
} /* end of CalendarElementFree() */


/*! Prints

\param pceArg
*/
void
PrintCalendarElement(pieCalendarElementPtr pceArg)
{
  if (pceArg != NULL && pceArg->pndEntry != NULL) {
    PrintFormatLog(1, "Calendar element: %s/%s=%s, begin=%lu, end=%lu, repeat=%.1f",
      pceArg->pndEntry->parent->name,
      pceArg->pndEntry->name,
      pceArg->pucDate,
      pceArg->dtBegin,
      pceArg->dtEnd,
      pceArg->iRecurrence);
  }
} /* end of PrintCalendarElement() */



/*! Scans 'pceArgResult' for a datum description and write the results to
  the given addresses.

  skips further scans if result pointer is NULL (speedup for
  calGetYearMinMax())

\param pceArgResult
\return NULL in case of error, else pointer to xmlChar after date expression string
 */
pieCalendarElementPtr
SplitCalendarElementRecurrences(pieCalendarElementPtr pceArg)
{
  pieCalendarElementPtr pceResult = NULL;

  if (pceArg != NULL && pceArg->iRecurrence > 0) {
    pieCalendarElementPtr pceI;

    assert(pceArg->pNext == NULL);
    
    for (pceI = CalendarElementDup(pceArg); pceI != NULL && pceI->iRecurrence > 0; pceI->iRecurrence--) {
      pieCalendarElementPtr pceT;

      pceT = CalendarElementDup(pceI);
      if (pceT) {
	pceT->iRecurrence = 0;
	if (pceResult) {
	  CalendarElementListAdd(pceResult, pceT);
	}
	else {
	  pceResult = pceT;
	}
      }

      if (pceI->dtBegin > 0) {
	if (pceI->period.w != 0) {
	  /* date/week-period */
	  pceI->dtBegin += pceI->period.w * 7;
	}
	else {
	  pceI->dtBegin = dt_add_years(pceI->dtBegin, pceI->period.y, DT_EXCESS);
	  pceI->dtBegin = dt_add_months(pceI->dtBegin, pceI->period.m, DT_EXCESS);
	  pceI->dtBegin += pceI->period.d;
	}
      }
      else if (pceI->dtEnd > 0) {
	if (pceI->period.w != 0) {
	  /* week-period/date */
	  pceI->dtEnd -= pceI->period.w * 7;
	}
	else {
	  pceI->dtEnd = dt_add_years(pceI->dtEnd, - pceI->period.y, DT_EXCESS);
	  pceI->dtEnd = dt_add_months(pceI->dtEnd, - pceI->period.m, DT_EXCESS);
	  pceI->dtEnd -= pceI->period.d;
	}
      }
      else {
      }

    }
    CalendarElementFree(pceI);
  }

  return pceResult;
} /* end of SplitCalendarElementRecurrences() */



/*! Scans 'pceArgResult' for a datum description and write the results to
  the given addresses.

  skips further scans if result pointer is NULL (speedup for
  calGetYearMinMax())

\param pceArgResult
\return NULL in case of error, else pointer to xmlChar after date expression string
 */
BOOL_T
ScanCalendarElementDate(pieCalendarElementPtr pceArgResult)
{
  BOOL_T fResult = FALSE;

  if (pceArgResult != NULL && pceArgResult->pucDate != NULL) {

    if (pceArgResult->pucSep == NULL) {
      pceArgResult->pucSep = pceArgResult->pucDate;
    }

    if (pceArgResult->pucSep != NULL) {
      xmlChar *pucT = pceArgResult->pucSep; /* shortcut */
      
      //for (; pceArgResult->pucSep[0] == ','; pceArgResult->pucSep++);

      if (isdigit(pucT[0]) && isdigit(pucT[1]) && isdigit(pucT[2]) && isdigit(pucT[3])
	&& isdigit(pucT[4]) && isdigit(pucT[5]) && isdigit(pucT[6]) && isdigit(pucT[7])
	&& isdigit(pucT[8]) && isdigit(pucT[9])) {

	/* system time "1311186519" */
	unsigned long iT;
	time_t tT;
	xmlChar* pucSepEnd;

	pucT = xmlStrndup(pucT, 10);
	iT = strtoul((const char*)pucT, NULL, 10);
	xmlFree(pucT);

	tT = (time_t)iT;
	pceArgResult->dtBegin = dt_from_struct_tm(localtime((const time_t*)(&tT)));
	pceArgResult->dtEnd = pceArgResult->dtBegin;

	for (pceArgResult->pucSep += 10; isdigit(*(pceArgResult->pucSep)); pceArgResult->pucSep++) {
	  /* skip chars of millisecs */
	}

	fResult = TRUE;
      }
      else {
	size_t j;
	size_t n = 0;
	int y, m, d, w, r;

	if ((n = dt_parse_iso_recurrance(pucT, xmlStrlen(pceArgResult->pucSep), &pceArgResult->iRecurrence)) > 1) {

	  if (pucT[n] == '/') {
	    n++;
	  }

	}

	if ((j = dt_parse_iso_date(&pucT[n], xmlStrlen(&pucT[n]), &pceArgResult->dtBegin)) > 3) {
	  n += j;
	  if (pucT[n] == '/') { /* */
	    n++;
	    if ((j = dt_parse_iso_date(&pucT[n], xmlStrlen(&pucT[n]), &pceArgResult->dtEnd)) > 3) {
	      /* date/date */
	      assert(pceArgResult->iRecurrence < 1);
	      n += j;
	    }
	    else if ((j = dt_parse_iso_period(&pucT[n], xmlStrlen(&pucT[n]), &y, &m, &d, &w)) > 0) {

	      if (pceArgResult->iRecurrence > 0) {
		/* recurrance/date/period */
		pceArgResult->period.y = y;
		pceArgResult->period.m = m;
		pceArgResult->period.d = d;
		pceArgResult->period.w = w;
	      }
	      else {
		/* date/period */
		pceArgResult->dtEnd = pceArgResult->dtBegin;
		if (w != 0) {
		  /* date/week-period */
		  pceArgResult->dtEnd += w * 7;
		}
		else {
		  pceArgResult->dtEnd = dt_add_years(pceArgResult->dtEnd, y, DT_EXCESS);
		  pceArgResult->dtEnd = dt_add_months(pceArgResult->dtEnd, m, DT_EXCESS);
		  pceArgResult->dtEnd += d;
		}
	      }
	      n += j;
	    }
	    else {
	      n = 0;
	    }
	  }
#if 0
	  else if (pucT[n] == 'T') {
	    int s;
	    n++;
	    if ((j = dt_parse_iso_time_extended(&pucT[n], len - n, &s, NULL)) > 3) {
	      n += j;
	    }
	  }
#endif
	  else {
	    /* no interval */
	  }

	}
	else if ((j = dt_parse_iso_period(&pucT[n], xmlStrlen(&pucT[n]), &y, &m, &d, &w)) > 0) {
	  n += j;

	  if (pucT[n] == '/') {
	    n++;

	    if ((j = dt_parse_iso_date(&pucT[n], xmlStrlen(&pucT[n]), &pceArgResult->dtEnd)) > 3) {

	      if (pceArgResult->iRecurrence > 0) {
		/* recurrance/date/period */
		pceArgResult->period.y = y;
		pceArgResult->period.m = m;
		pceArgResult->period.d = d;
		pceArgResult->period.w = w;
	      }
	      else {
		/* period/date */
		pceArgResult->dtBegin = pceArgResult->dtEnd;
		if (w != 0) {
		  /* week-period/date */
		  pceArgResult->dtBegin -= w * 7;
		}
		else {
		  pceArgResult->dtBegin = dt_add_years(pceArgResult->dtBegin, -y, DT_EXCESS);
		  pceArgResult->dtBegin = dt_add_months(pceArgResult->dtBegin, -m, DT_EXCESS);
		  pceArgResult->dtBegin -= d;
		}
	      }
	      n += j;
	    }
	    else {
	      n = 0;
	    }
	  }
	  else {
	    n = 0;
	  }

	}
	else if (isdigit(pucT[n]) && isdigit(pucT[n+1]) && isdigit(pucT[n+2]) && isdigit(pucT[n+3])) {

	  y = 1000 * (pucT[n] - '0') + 100 * (pucT[n+1] - '0') + 10 * (pucT[n+2] - '0') + (pucT[n+3] - '0');

	  assert(y > -1 && y < 2999);

	  n += 4;

	  /*! implicit intervals (whole year, month, week) */

	  if (pucT[n] == '/' && isdigit(pucT[n + 1]) && isdigit(pucT[n + 2]) && isdigit(pucT[n + 3]) && isdigit(pucT[n + 4]) && !isiso8601(pucT[n + 5])) {
	    /* ISO 8601 YYYY/YYYY */
	    int z;

	    z = 1000 * (pucT[n + 1] - '0') + 100 * (pucT[n + 2] - '0') + 10 * (pucT[n + 3] - '0') + (pucT[n + 4] - '0');
	    if (z > -1 && z < 2999) {
	      pceArgResult->dtBegin = dt_from_yd(y, 1);
	      pceArgResult->dtEnd = dt_from_yd(z+1, 0);
	      n += 5;
	    }
	    else {
	      n = 0;
	    }
	  }
	  else if (pucT[n] == '-' && isdigit(pucT[n + 1]) && isdigit(pucT[n + 2]) && !isdigit(pucT[n + 3])) {
	    /* ISO 8601 YYYY-MM */
	    m = 10 * (pucT[n + 1] - '0') + (pucT[n + 2] - '0');
	    if (m > 0 && m < 13) {
	      n += 3;
	      pceArgResult->dtBegin = dt_from_ymd(y, m, 1);
	      pceArgResult->dtEnd = dt_end_of_month(pceArgResult->dtBegin, 0);
	    }
	    else {
	      n = 0;
	    }
	  }
	  else if (isdigit(pucT[n]) && isdigit(pucT[n + 1]) && !isdigit(pucT[n + 2])) {
	    /* ISO 8601 YYYYMM */
	    m = 10 * (pucT[n] - '0') + (pucT[n + 1] - '0');
	    if (m > 0 && m < 13) {
	      n += 2;
	      pceArgResult->dtBegin = dt_from_ymd(y, m, 1);
	      pceArgResult->dtEnd = dt_end_of_month(pceArgResult->dtBegin, 0);
	    }
	    else {
	      n = 0;
	    }
	  }
	  else if (pucT[n] == '-' && pucT[n + 1] == 'W' && isdigit(pucT[n + 2]) && isdigit(pucT[n + 3]) && !isdigit(pucT[n + 4])) {
	    /* ISO 8601 YYYY-Wnn */
	    w = 10 * (pucT[n + 2] - '0') + (pucT[n + 3] - '0');
	    if (w > -1 && w < 54) {
	      n += 4;
	      pceArgResult->dtBegin = dt_from_ywd(y, w, 1);
	      pceArgResult->dtEnd = dt_end_of_week(pceArgResult->dtBegin, 1);
	    }
	    else {
	      n = 0;
	    }
	  }
	  else if (!isiso8601(pucT[n])) {
	    /* ISO 8601 YYYY */
	    pceArgResult->dtBegin = dt_from_ymd(y, 1, 1);
	    pceArgResult->dtEnd = dt_end_of_year(pceArgResult->dtBegin, 0);
	  }
	  else {
	    n = 0;
	  }

	}
	else {
	  n = 0;
	}

	if (pceArgResult->dtEnd > 0) {
	  if (pceArgResult->dtEnd > pceArgResult->dtBegin) {
	    /* expected */
	  }
	  else if (pceArgResult->dtEnd < pceArgResult->dtBegin) {
	    /* swap pceArgResult->dtEnd and pceArgResult->dtBegin */
	    dt_t dt0;

	    dt0 = pceArgResult->dtEnd;
	    pceArgResult->dtEnd = pceArgResult->dtBegin;
	    pceArgResult->dtBegin = dt0;
	  }
	  else {
	    /* it's not an interval */
	    pceArgResult->dtEnd = 0;
	  }
	}

	pceArgResult->pucSep += n;
	fResult = n > 0;
      }
    }
  }
  
  return fResult;
} /* end of ScanCalendarElementDate() */


/*! insert a day diff attribute to pndArg
*/
BOOL_T
AddNodeDateAttributes(xmlNodePtr pndArg, xmlChar* pucArg)
{
  BOOL_T fResult = FALSE;
  xmlChar* pucDate;

  if (pndArg != NULL
      && ((STR_IS_NOT_EMPTY(pucArg) && (pucDate = pucArg))
	  || ((pndArg->children != NULL && (pucDate = pndArg->children->content) != NULL)))) {
    pieCalendarElementPtr pceT;

    if ((pceT = CalendarElementNew(pucDate))) {
      time_t t;
      dt_t iDayToday;
      long int iDayDiff = 0;
      pieCalendarElementPtr pceList;

      time(&t); /*!\todo reduce number of calls of time()/localtime() */
      iDayToday = dt_from_struct_tm(localtime(&t));

      if (ScanCalendarElementDate(pceT)) {
	xmlChar mpucT[BUFFER_LENGTH];
	xmlChar* pucDisplay = NULL;
	pieCalendarElementPtr pceI;

	if ((pceList = SplitCalendarElementRecurrences(pceT))) {
	  mpucT[0] = '\0';
	  for (pceI = pceList; pceI; pceI = pceI->pNext) {
	    xmlStrPrintf(mpucT, BUFFER_LENGTH, "%04i-%02i-%02i",
	      dt_year(pceI->dtBegin > 0 ? pceI->dtBegin : pceI->dtEnd), 
	      dt_month(pceI->dtBegin > 0 ? pceI->dtBegin : pceI->dtEnd), 
	      dt_dom(pceI->dtBegin > 0 ? pceI->dtBegin : pceI->dtEnd));

	    if (pucDisplay) {
	      if (pceI->dtBegin > 0) {
		pucDisplay = xmlStrcat(pucDisplay, ",");
		pucDisplay = xmlStrcat(pucDisplay, mpucT);
	      }
	      else {
		xmlChar* pucT;

		pucT = xmlStrdup(mpucT);
		pucT = xmlStrcat(pucT, ",");
		pucDisplay = xmlStrcat(pucT, pucDisplay);
		// xmlFree(pucDisplay);
	      }
	    }
	    else {
	      pucDisplay = xmlStrdup(mpucT);
	    }
	  }
	  CalendarElementFree(pceList);
	  domSetPropEat(pndArg, BAD_CAST"iso", pucDisplay);
	}
	else {
	  xmlStrPrintf(mpucT, BUFFER_LENGTH, "%04i-%02i-%02i", dt_year(pceT->dtBegin), dt_month(pceT->dtBegin), dt_dom(pceT->dtBegin));
	  xmlSetProp(pndArg, BAD_CAST"iso", mpucT);
	}



	//xmlStrPrintf(mpucT, BUFFER_LENGTH, "%li", pceT->dtBegin);
	//xmlSetProp(pndArg, BAD_CAST"abs", mpucT);


	if (pceT->dtEnd > pceT->dtBegin) {
	  /* an interval end */

	  xmlStrPrintf(mpucT, BUFFER_LENGTH, "%04i-%02i-%02i", dt_year(pceT->dtEnd), dt_month(pceT->dtEnd), dt_dom(pceT->dtEnd));
	  xmlSetProp(pndArg, BAD_CAST"b", mpucT);

	  xmlStrPrintf(mpucT, BUFFER_LENGTH, "%li", pceT->dtEnd - pceT->dtBegin + 1);
	  xmlSetProp(pndArg, BAD_CAST"interval", mpucT);

	  if (pceT->dtEnd < iDayToday) {
	    /* date interval ends before today */
	    iDayDiff = pceT->dtEnd - iDayToday;
	  }
	  else if (iDayToday < pceT->dtBegin) {
	    /* date interval begins after today */
	    iDayDiff = pceT->dtBegin - iDayToday;
	  }
	  else {
	    /* today is in date interval */
	  }
	}
	else {
	  /* no interval end, single date */
	  iDayDiff = pceT->dtBegin - iDayToday;
	}

	xmlStrPrintf(mpucT, BUFFER_LENGTH, "%li", iDayDiff);
	xmlSetProp(pndArg, BAD_CAST"diff", mpucT);

	fResult = TRUE;
      }
    }
    
#if 0

      if (pceT->iMonth > 0 && pceT->iDay > 0) {

	if (pceT->iHourA > -1 && pceT->iMinuteA > -1 && pceT->iSecondA > -1) {
	  xmlStrPrintf(mpucT, BUFFER_LENGTH,
	    "%04i-%02i-%02iT%02i:%02i:%02i",
	    pceT->iYear,
	    pceT->iMonth,
	    pceT->iDay,
	    pceT->iHourA,
	    pceT->iMinuteA,
	    pceT->iSecondA
	  );

	  if (pceT->iTimezoneOffset) {
	    xmlChar* pucT = NULL;

	    if (pceT->iTimezoneOffset < 0) {
	      pucT = xmlStrncatNew(mpucT, BAD_CAST STR_UTF8_MINUS, -1);
	    }
	    else if (pceT->iTimezoneOffset > 0) {
	      pucT = xmlStrncatNew(mpucT, BAD_CAST "+", -1);
	    }

	    xmlStrPrintf(mpucT, BUFFER_LENGTH - 1, "%s%02i:%02i", pucT, (abs(pceT->iTimezoneOffset) / 60), (abs(pceT->iTimezoneOffset) % 60));
	    xmlFree(pucT);
	  }

	  if (pceT->iTimezone) {
	    xmlSetProp(pndArg, BAD_CAST "tz", tzGetId(pceT->iTimezone));
	  }
	}
	else {
	}
      }

    if (STR_IS_NOT_EMPTY(mpucT)) {

      if (pceT->iHourA > -1) {
	xmlStrPrintf(mpucT, BUFFER_LENGTH - 1, "%02i", (pceT->iHourA > -1) ? pceT->iHourA : 0);
	xmlSetProp(pndArg, BAD_CAST "hour", mpucT);

	xmlStrPrintf(mpucT, BUFFER_LENGTH - 1, "%02i", (pceT->iMinuteA > -1) ? pceT->iMinuteA : 0);
	xmlSetProp(pndArg, BAD_CAST "minute", mpucT);

	xmlStrPrintf(mpucT, BUFFER_LENGTH - 1, "%02i", (pceT->iSecondA > -1) ? pceT->iSecondA : 0);
	xmlSetProp(pndArg, BAD_CAST "second", mpucT);

	if (pceT->iHourB > -1) {
	  xmlStrPrintf(mpucT, BUFFER_LENGTH - 1, "%02i", (pceT->iHourB > -1) ? pceT->iHourB : 0);
	  xmlSetProp(pndArg, BAD_CAST "hour-end", mpucT);

	  xmlStrPrintf(mpucT, BUFFER_LENGTH - 1, "%02i", (pceT->iMinuteB > -1) ? pceT->iMinuteB : 0);
	  xmlSetProp(pndArg, BAD_CAST "minute-end", mpucT);

	  xmlStrPrintf(mpucT, BUFFER_LENGTH - 1, "%02i", (pceT->iSecondB > -1) ? pceT->iSecondB : 0);
	  xmlSetProp(pndArg, BAD_CAST "second-end", mpucT);
	}
      }
    }
#endif

    return fResult;
  } 
} /* End of AddNodeDateAttributes() */

  
/*!
*/
dt_t
UpdateToday(xmlChar *pucArgToday)
{
  xmlChar mpucT[BUFFER_LENGTH];
  xmlChar *pucEnvDate;

  if (STR_IS_NOT_EMPTY(pucArgToday)) {
    pieCalendarElementPtr pceNew;

    dt_parse_iso_date(pucArgToday, xmlStrlen(pucArgToday), &dtToday);

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
  else if ((pucEnvDate = BAD_CAST getenv("CXP_DATE")) != NULL) {
    UpdateToday(pucEnvDate);
  }
  else {
    PrintFormatLog(4,"Use system time as today");
    memcpy(&timeNow,localtime(&system_zeit),sizeof(struct tm));
    dtToday = dt_from_struct_tm(localtime(&system_zeit));
  }

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

  return dtToday;
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
  return dtToday;
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


#ifdef TESTCODE
#include "test/test_calendar_element.c"
#endif

