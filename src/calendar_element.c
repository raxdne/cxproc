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
#include "dom.h"
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


static ceElementPtr
CalendarElementReset(ceElementPtr pceArg);


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
  
  return fResult;
}
/* end of ceInit() */


/*!\return a new allocated calendar element
 */
ceElementPtr
CalendarElementNew(xmlChar *pucArg)
{
  ceElementPtr pceResult = NULL;

  pceResult = (ceElementPtr)xmlMalloc(sizeof(ceElementType));
  if (pceResult) {
    memset(pceResult, 0, sizeof(ceElementType));
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
ceElementPtr
CalendarElementUpdate(ceElementPtr pceArg, xmlChar* pucArg)
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
ceElementPtr
CalendarElementDup(ceElementPtr pceArg)
{
  ceElementPtr pceResult = NULL;

  if (pceArg) {
    assert(pceArg->pNext == NULL);
    
    pceResult = CalendarElementNew(NULL);
    if (pceResult) {
      memcpy(pceResult,pceArg,sizeof(ceElementType));
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
ceElementPtr
CalendarElementReset(ceElementPtr pceArg)
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
    
    //memset(pceArg, 0, sizeof(ceElementType));

    pceArg->dt0.dt = 0;
    pceArg->dt1.dt   = pceArg->dt0.dt;
    
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
CalendarElementListAdd(ceElementPtr pceArgList, ceElementPtr pceArg)
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
CalendarElementFree(ceElementPtr pceArg)
{
  if (pceArg) {
    if (pceArg->pNext) {
      CalendarElementFree(pceArg->pNext);
    }
    CalendarElementReset(pceArg);
    xmlFree(pceArg);
  }
} /* end of CalendarElementFree() */


#if defined(DEBUG) || defined(TESTCODE)

/*! Prints

\param pceArg
*/
void
PrintCalendarElement(ceElementPtr pceArg)
{
  if (pceArg != NULL && pceArg->pndEntry != NULL) {
    PrintFormatLog(1, "Calendar element: %s/%s=%s, begin=%lu, end=%lu, repeat=%.1f",
      pceArg->pndEntry->parent->name,
      pceArg->pndEntry->name,
      pceArg->pucDate,
      pceArg->dt0.dt,
      pceArg->dt1.dt,
      pceArg->iRecurrence);
  }
} /* end of PrintCalendarElement() */

#endif


/*! Scans 'pceArgResult' for a datum description and write the results to
  the given addresses.

  skips further scans if result pointer is NULL (speedup for
  calGetYearMinMax())

\param pceArgResult
\return NULL in case of error, else pointer to xmlChar after date expression string
 */
ceElementPtr
SplitCalendarElementRecurrences(ceElementPtr pceArg)
{
  ceElementPtr pceResult = NULL;

  if (pceArg != NULL && pceArg->iRecurrence > 0) {
    ceElementPtr pceI;

    assert(pceArg->pNext == NULL);
    
    for (pceI = CalendarElementDup(pceArg); pceI != NULL && pceI->iRecurrence > 0; pceI->iRecurrence--) {
      ceElementPtr pceT;

      pceT = CalendarElementDup(pceI);
      if (pceT) {
	pceT->iRecurrence = -1; /* this calendar element is resulting from a recurrence */
	if (pceResult) {
	  CalendarElementListAdd(pceResult, pceT);
	}
	else {
	  pceResult = pceT;
	}
      }

      if (pceI->dt0.dt > 0) {
	if (pceI->period.w != 0) {
	  /* date/week-period */
	  pceI->dt0.dt += pceI->period.w * 7;
	}
	else {
	  pceI->dt0.dt = dt_add_months(pceI->dt0.dt, pceI->period.y * 12 + pceI->period.m, DT_EXCESS);
	  pceI->dt0.dt += pceI->period.d;
	}
      }
      else if (pceI->dt1.dt > 0) {
	if (pceI->period.w != 0) {
	  /* week-period/date */
	  pceI->dt1.dt -= pceI->period.w * 7;
	}
	else {
	  pceI->dt1.dt = dt_add_months(pceI->dt1.dt, - (pceI->period.y * 12 + pceI->period.m), DT_EXCESS);
	  pceI->dt1.dt -= pceI->period.d;
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
ScanCalendarElementDate(ceElementPtr pceArgResult)
{
  BOOL_T fResult = FALSE;

  if (pceArgResult != NULL && pceArgResult->pucDate != NULL) {

    if (pceArgResult->pucSep == NULL) {
      pceArgResult->pucSep = pceArgResult->pucDate;
    }

    if (pceArgResult->pucSep != NULL) {
      xmlChar *pucT = pceArgResult->pucSep; /* shortcut */
      
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
	pceArgResult->dt0.dt = dt_from_struct_tm(localtime((const time_t*)(&tT)));
	pceArgResult->dt1.dt = pceArgResult->dt0.dt;

	for (pceArgResult->pucSep += 10; isdigit(*(pceArgResult->pucSep)); pceArgResult->pucSep++) {
	  /* skip chars of millisecs */
	}

	fResult = TRUE;
      }
      else {
	size_t j;
	size_t n = 0;
	int y, m, d, w, r;

	if ((n = dt_parse_iso_recurrance((const char *)pucT, xmlStrlen(pceArgResult->pucSep), &pceArgResult->iRecurrence)) > 0) {

	  if (pucT[n] == '/') {
	    n++;
	  }

	}

	if ((j = dt_parse_iso_date_time_zone((const char*)&pucT[n], xmlStrlen(&pucT[n]), &pceArgResult->dt0.dt, &pceArgResult->dt0.iSec)) > 3) {
	  n += j;

	  if (pucT[n] == '/') { /* */
	    n++;
	    if ((j = dt_parse_iso_date_time_zone((const char*)&pucT[n], xmlStrlen(&pucT[n]), &pceArgResult->dt1.dt, &pceArgResult->dt1.iSec)) > 3) {
	      /* date/date */
	      assert(pceArgResult->iRecurrence < 1);
	      n += j;
	    }
	    else if ((j = dt_parse_iso_period((const char*)&pucT[n], xmlStrlen(&pucT[n]), &y, &m, &d, &w)) > 0) {

	      if (pceArgResult->iRecurrence > 0) {
		/* recurrance/date/period */
		pceArgResult->period.y = y;
		pceArgResult->period.m = m;
		pceArgResult->period.d = d;
		pceArgResult->period.w = w;
	      }
	      else {
		/* date/period */
		pceArgResult->dt1.dt = pceArgResult->dt0.dt;
		if (w != 0) {
		  /* date/week-period */
		  pceArgResult->dt1.dt += w * 7;
		}
		else {
		  pceArgResult->dt1.dt = dt_add_years(pceArgResult->dt1.dt, y, DT_EXCESS);
		  pceArgResult->dt1.dt = dt_add_months(pceArgResult->dt1.dt, m, DT_EXCESS);
		  pceArgResult->dt1.dt += d;
		}
	      }
	      n += j;
	    }
	    else {
	      n = 0;
	    }
	  }
	  else {
	    /* no interval */
	  }

	}
	else if ((j = dt_parse_iso_period((const char*)&pucT[n], xmlStrlen(&pucT[n]), &y, &m, &d, &w)) > 0) {
	  n += j;

	  if (pucT[n] == '/') {
	    n++;

	    if ((j = dt_parse_iso_date_time_zone((const char*)&pucT[n], xmlStrlen(&pucT[n]), &pceArgResult->dt1.dt, &pceArgResult->dt1.iSec)) > 3) {

	      if (pceArgResult->iRecurrence > 0) {
		/* recurrance/date/period */
		pceArgResult->period.y = y;
		pceArgResult->period.m = m;
		pceArgResult->period.d = d;
		pceArgResult->period.w = w;
	      }
	      else {
		/* period/date */
		pceArgResult->dt0.dt = pceArgResult->dt1.dt;
		if (w != 0) {
		  /* week-period/date */
		  pceArgResult->dt0.dt -= w * 7;
		}
		else {
		  pceArgResult->dt0.dt = dt_add_years(pceArgResult->dt0.dt, -y, DT_EXCESS);
		  pceArgResult->dt0.dt = dt_add_months(pceArgResult->dt0.dt, -m, DT_EXCESS);
		  pceArgResult->dt0.dt -= d;
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
	      pceArgResult->dt0.dt = dt_from_yd(y, 1);
	      pceArgResult->dt1.dt = dt_from_yd(z+1, 0);
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
	      pceArgResult->dt0.dt = dt_from_ymd(y, m, 1);
	      pceArgResult->dt1.dt = dt_end_of_month(pceArgResult->dt0.dt, 0);
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
	      pceArgResult->dt0.dt = dt_from_ymd(y, m, 1);
	      pceArgResult->dt1.dt = dt_end_of_month(pceArgResult->dt0.dt, 0);
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
	      pceArgResult->dt0.dt = dt_from_ywd(y, w, 1);
	      pceArgResult->dt1.dt = dt_end_of_week(pceArgResult->dt0.dt, DT_MON);
	    }
	    else {
	      n = 0;
	    }
	  }
	  else if (!isiso8601(pucT[n])) {
	    /* ISO 8601 YYYY */
	    pceArgResult->dt0.dt = dt_from_ymd(y, 1, 1);
	    pceArgResult->dt1.dt = dt_end_of_year(pceArgResult->dt0.dt, 0);
	  }
	  else {
	    n = 0;
	  }

	}
	else {
	  n = 0;
	}

	if (pceArgResult->dt1.dt > 0) {
	  if (pceArgResult->dt1.dt > pceArgResult->dt0.dt) {
	    /* expected */
	  }
	  else if (pceArgResult->dt1.dt < pceArgResult->dt0.dt) {
	    /* swap pceArgResult->dt1.dt and pceArgResult->dt0.dt */
	    dt_t dt0;

	    dt0 = pceArgResult->dt1.dt;
	    pceArgResult->dt1.dt = pceArgResult->dt0.dt;
	    pceArgResult->dt0.dt = dt0;
	  }
	  else {
	    /* it's not an interval */
	    pceArgResult->dt1.dt = 0;
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
    ceElementPtr pceT;

    if ((pceT = CalendarElementNew(pucDate))) {
      time_t t;
      dt_t iDayToday;
      long int iDayDiff = 0;
      ceElementPtr pceList;

      time(&t); /*!\todo reduce number of calls of time()/localtime() */
      iDayToday = dt_from_struct_tm(localtime(&t));

      if (ScanCalendarElementDate(pceT)) {
	xmlChar mpucT[BUFFER_LENGTH];
	xmlChar* pucDisplay = NULL;
	ceElementPtr pceI;

	if ((pceList = SplitCalendarElementRecurrences(pceT))) {
	  mpucT[0] = '\0';
	  for (pceI = pceList; pceI; pceI = pceI->pNext) {
	    xmlStrPrintf(mpucT, BUFFER_LENGTH, "%04i-%02i-%02i",
	      dt_year(pceI->dt0.dt > 0 ? pceI->dt0.dt : pceI->dt1.dt), 
	      dt_month(pceI->dt0.dt > 0 ? pceI->dt0.dt : pceI->dt1.dt), 
	      dt_dom(pceI->dt0.dt > 0 ? pceI->dt0.dt : pceI->dt1.dt));

	    if (pucDisplay) {
	      if (pceI->dt0.dt > 0) {
		pucDisplay = xmlStrcat(pucDisplay, BAD_CAST",");
		pucDisplay = xmlStrcat(pucDisplay, mpucT);
	      }
	      else {
		xmlChar* pucT;

		pucT = xmlStrdup(mpucT);
		pucT = xmlStrcat(pucT, BAD_CAST",");
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
	else if (pceT->dt1.dt > pceT->dt0.dt) {
	  /* an interval end */

	  xmlStrPrintf(mpucT, BUFFER_LENGTH, "%04i-%02i-%02i", dt_year(pceT->dt0.dt), dt_month(pceT->dt0.dt), dt_dom(pceT->dt0.dt));
	  xmlSetProp(pndArg, BAD_CAST"begin", mpucT);

	  xmlStrPrintf(mpucT, BUFFER_LENGTH, "%04i-%02i-%02i", dt_year(pceT->dt1.dt), dt_month(pceT->dt1.dt), dt_dom(pceT->dt1.dt));
	  xmlSetProp(pndArg, BAD_CAST"end", mpucT);

	  xmlStrPrintf(mpucT, BUFFER_LENGTH, "%li", pceT->dt1.dt - pceT->dt0.dt + 1);
	  xmlSetProp(pndArg, BAD_CAST"interval", mpucT);
	}
	else {
	  xmlStrPrintf(mpucT, BUFFER_LENGTH, "%04i-%02i-%02i", dt_year(pceT->dt0.dt), dt_month(pceT->dt0.dt), dt_dom(pceT->dt0.dt));
	  xmlSetProp(pndArg, BAD_CAST"iso", mpucT);
	}

	//xmlStrPrintf(mpucT, BUFFER_LENGTH, "%li", pceT->dt0.dt);
	//xmlSetProp(pndArg, BAD_CAST"abs", mpucT);

	if (pceT->dt1.dt > pceT->dt0.dt) {
	  if (pceT->dt1.dt < iDayToday) {
	    /* date interval ends before today */
	    iDayDiff = pceT->dt1.dt - iDayToday;
	  }
	  else if (iDayToday < pceT->dt0.dt) {
	    /* date interval begins after today */
	    iDayDiff = pceT->dt0.dt - iDayToday;
	  }
	  else {
	    /* today is in date interval */
	  }
	}
	else {
	  /* no interval end, single date */
	  iDayDiff = pceT->dt0.dt - iDayToday;
	}

	xmlStrPrintf(mpucT, BUFFER_LENGTH, "%li", iDayDiff);
	xmlSetProp(pndArg, BAD_CAST"diff", mpucT);

	fResult = TRUE;
      }
      CalendarElementFree(pceT);
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

  } 
  return fResult;
} /* End of AddNodeDateAttributes() */

  
/*!
*/
dt_t
GetToday(void)
{
  if (dtToday < 1) {
    UpdateToday(NULL);
  }
  return dtToday;
} /* End of GetToday() */


/*!
*/
dt_t
UpdateToday(xmlChar *pucArgToday)
{
  xmlChar mpucT[BUFFER_LENGTH];
  xmlChar *pucEnvDate;

  if (STR_IS_NOT_EMPTY(pucArgToday)) {
    ceElementPtr pceNew;
    int s;

    if (dt_parse_iso_date_time_zone((const char*)pucArgToday, xmlStrlen(pucArgToday), &dtToday, &s) < 1) {
      return -1;
    }
  }
  else if ((pucEnvDate = BAD_CAST getenv("CXP_DATE")) != NULL) {
    UpdateToday(pucEnvDate);
  }
  else {
    PrintFormatLog(4,"Use system time as today");
    memcpy(&timeNow,localtime(&system_zeit),sizeof(struct tm));
    dtToday = dt_from_struct_tm(localtime(&system_zeit));
  }

  return dtToday;
} /* end of UpdateToday() */


#ifdef TESTCODE
#include "test/test_calendar_element.c"
#endif

