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

#include <math.h>
#include <float.h>

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

dt_t dtToday = 0; /* might be manipulated by CXP_DATE etc */


static ceElementPtr
CalendarElementReset(ceElementPtr pceArg);

xmlChar*
GetISO8601DateStr(iso8601DateType *pdtArg);

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
    CalendarElementUpdate(pceResult, pucArg);
  }
  return pceResult;
} /* end of CalendarElementNew() */


/*!\return an updated calendar element
 */
ceElementPtr
CalendarElementUpdate(ceElementPtr pceArg, xmlChar* pucArg)
{
  size_t l;

  if (pceArg != NULL && (l = xmlStrlen(pucArg)) > 3) {
    CalendarElementReset(pceArg);

    pceArg->pucIntern = BAD_CAST xmlMalloc(l + 1);
    if (pceArg->pucIntern) {
      int i, j, k;
      BOOL_T fTime;
      BOOL_T fOffset;
      BOOL_T fPeriod;

      for (i = 0; isspace(pucArg[i]); i++) {}

      for (fTime = fOffset = fPeriod = FALSE, k = j = 0; (isiso8601(pucArg[i + j]) || isalnum(pucArg[i + j]) || pucArg[i + j] == 0xE2) && k < l; j++) {
	switch (pucArg[i + j]) {
	case ':':
	  /* ignoring chars */
	  break;

	case '-':
	  if (fPeriod || fOffset) {
	    /* negative periods/offsets are not permitted */
	    CalendarElementReset(pceArg);
	    return NULL;
	  }
	  else if (fTime) { /* minus char is time zone offset now */
	    pceArg->pucIntern[k++] = pucArg[i + j];
	  }
	  break;

	case 0xE2:
	  if (pucArg[i + j + 1] == 0x88 && pucArg[i + j + 2] == 0x92) {
	    pceArg->pucIntern[k++] = '-'; /* reduce multi-byte char to ASCII */
	    j += 2;
	  }
	  break;

	case '/':
	  fTime = fOffset = fPeriod = FALSE; /* reset flag for next date/time */
	  pceArg->pucIntern[k++] = pucArg[i + j];
	  break;

	case 'O':
	  /* following Offset must not be modified, but copied */
	  fOffset = TRUE;
	  pceArg->pucIntern[k++] = pucArg[i + j];
	  break;

	case 'P':
	  /* following Period must not contain '-', but copied */
	  fPeriod = TRUE;
	  pceArg->pucIntern[k++] = pucArg[i + j];
	  break;

	case 'T':
	  /* following Time must not be modified, but copied */
	  fTime = TRUE;
	  pceArg->pucIntern[k++] = pucArg[i + j];
	  break;

	default:
	  pceArg->pucIntern[k++] = pucArg[i + j];
	}
      }
      pceArg->pucIntern[k] = '\0';

      if (k > 3) {
	pceArg->pucDate = xmlStrdup(&pucArg[i]);
	pceArg->pucSep = &pucArg[i + j];
      }
      else {
	xmlFree(pceArg->pucIntern);
	pceArg->pucIntern = NULL;
      }
    }
  }
  return pceArg;
} /* end of CalendarElementUpdate() */


/*!\return an ISO string for calendar element
  \todo handle time zone
 */
xmlChar*
GetISO8601DateStr(iso8601DateType *pdtArg)
{
  xmlChar *pucResult = NULL;
  
  if (pdtArg != NULL && pdtArg->dt > 0) {
    xmlChar mpucT[BUFFER_LENGTH];

    if (pdtArg->iSec > 0) {
      xmlStrPrintf(mpucT, BUFFER_LENGTH, "%04i-%02i-%02iT%02i:%02i:%02iZ",
		   dt_year(pdtArg->dt),
		   dt_month(pdtArg->dt),
		   dt_dom(pdtArg->dt),
		   pdtArg->iSec / 3600 ,
		   (pdtArg->iSec % 3600) / 60,
		   pdtArg->iSec % 60);
    }
    else {
      xmlStrPrintf(mpucT, BUFFER_LENGTH, "%04i-%02i-%02i",
		   dt_year(pdtArg->dt), 
		   dt_month(pdtArg->dt), 
		   dt_dom(pdtArg->dt));
    }
    pucResult = xmlStrdup(mpucT);
  }
  return pucResult;
} /* end of GetISO8601DateStr() */


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
      if (pceArg->pucIntern) {
	pceResult->pucIntern = xmlStrdup(pceArg->pucIntern);
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
    
    if (pceArg->pucIntern) {
      xmlFree(pceArg->pucIntern);
      pceArg->pucIntern = NULL;
    }
    
    if (pceArg->pucFormatted) {
      xmlFree(pceArg->pucFormatted);
      pceArg->pucFormatted = NULL;
    }
    //memset(pceArg, 0, sizeof(ceElementType));

    pceArg->dt0.dt = 0;
    pceArg->dt1.dt   = pceArg->dt0.dt;
    
    pceArg->dt0.iSec = 0;
    pceArg->dt1.iSec = pceArg->dt0.iSec;

    pceArg->period.y = 0;
    pceArg->period.m = 0;
    pceArg->period.d = 0;
    pceArg->period.w = 0;
    pceArg->period.hour = 0;
    pceArg->period.minute = 0;
    pceArg->period.second = 0;

    pceArg->iRecurrence = 0;
    pceArg->iRecurrenceIndex = 0;
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
    CalendarElementFree(pceArg->pNext);
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
      pceArg->pucIntern,
      pceArg->pucDate,
      pceArg->pucFormatted,
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

  if (pceArg != NULL && pceArg->pNext == NULL && (pceArg->dt0.dt > 0 || pceArg->dt1.dt > 0) && pceArg->iRecurrence > 0) {
    int i;
    int o = 0;
    ceElementPtr pceI;

    pceResult = CalendarElementDup(pceArg);
    assert(pceResult);
    if (pceResult->dt0.dt > 0 && pceResult->dt1.dt > 0) {
      o = (pceResult->dt1.dt - pceResult->dt0.dt);
    }

    for (i=1, pceI = CalendarElementDup(pceResult); pceI != NULL && pceI->iRecurrence > 0; i++, pceI = CalendarElementDup(pceI)) {

      pceI->iRecurrenceIndex = i;
      pceI->iRecurrence--; /* this calendar element is resulting from a recurrence */

      /*!\bug time value is ignored */

      if (o > 0) {
	pceI->dt0.dt += o;
      }
      else if (pceI->dt0.dt > 0 && pceI->dt1.dt > 0) {
	/* date/date/ */
	if (pceI->period.w != 0) {
	  /* date/date/week-period */
	  pceI->dt0.dt += pceI->period.w * 7;
	  pceI->dt1.dt += pceI->period.w * 7;
	}
	else {
	  pceI->dt0.dt = dt_add_months(pceI->dt0.dt, pceI->period.y * 12 + pceI->period.m, DT_EXCESS) + pceI->period.d;
	  pceI->dt1.dt = dt_add_months(pceI->dt1.dt, pceI->period.y * 12 + pceI->period.m, DT_EXCESS) + pceI->period.d;
	}
	/*!\bug handle time values */
      }
      else if (pceI->dt0.dt > 0) {
	if (pceI->period.w != 0) {
	  /* date/week-period */
	  pceI->dt0.dt += pceI->period.w * 7;
	}
	else {
	  pceI->dt0.dt = dt_add_months(pceI->dt0.dt, pceI->period.y * 12 + pceI->period.m, DT_EXCESS) + pceI->period.d;
	}
	/*!\bug handle time values */
      }
      else if (pceI->dt1.dt > 0) {
	if (pceI->period.w != 0) {
	  /* week-period/date */
	  pceI->dt1.dt -= pceI->period.w * 7;
	}
	else {
	  pceI->dt1.dt = dt_add_months(pceI->dt1.dt, - (pceI->period.y * 12 + pceI->period.m), DT_EXCESS) - pceI->period.d;
	}
	/*!\bug handle time values */
      }
      else {
      }

      CalendarElementListAdd(pceResult, pceI);
    }
    CalendarElementFree(pceI);
  }

  return pceResult;
} /* end of SplitCalendarElementRecurrences() */



/*! Formats 'pceArgResult' for a datum (similar to GetISO8601DateStr())

\param pceArg
\return NULL in case of error, else pointer to xmlChar after date expression string
 */
xmlChar*
FormatCalendarElementDateStr(ceElementPtr pceArg)
{
  xmlChar *pucResult = NULL;

  if (pceArg) {
    int l = 0;
    xmlChar mpucT[BUFFER_LENGTH];

    xmlFree(pceArg->pucFormatted);

    if (pceArg->dt0.dt > 0) {
      if (pceArg->dt0.iSec > 0) {
	l = xmlStrPrintf(mpucT, BUFFER_LENGTH, "%04i-%02i-%02iT%02i:%02i:%02iZ", dt_year(pceArg->dt0.dt), dt_month(pceArg->dt0.dt), dt_dom(pceArg->dt0.dt),
			 pceArg->dt0.iSec / 3600, (pceArg->dt0.iSec % 3600) / 60, pceArg->dt0.iSec % 60);
      }
      else {
	l = xmlStrPrintf(mpucT, BUFFER_LENGTH, "%04i-%02i-%02i", dt_year(pceArg->dt0.dt), dt_month(pceArg->dt0.dt), dt_dom(pceArg->dt0.dt));
      }
    }

    if (pceArg->dt1.dt > 0) {
      mpucT[l++] = '/';

      if (pceArg->dt1.iSec > 0) {
	xmlStrPrintf(&mpucT[l], BUFFER_LENGTH - l, "%04i-%02i-%02iT%02i:%02i:%02iZ", dt_year(pceArg->dt1.dt), dt_month(pceArg->dt1.dt), dt_dom(pceArg->dt1.dt),
		     pceArg->dt1.iSec / 3600, (pceArg->dt1.iSec % 3600) / 60, pceArg->dt1.iSec % 60);
      }
      else {
	xmlStrPrintf(&mpucT[l], BUFFER_LENGTH - l, "%04i-%02i-%02i", dt_year(pceArg->dt1.dt), dt_month(pceArg->dt1.dt), dt_dom(pceArg->dt1.dt));
      }
    }
    pucResult = pceArg->pucFormatted = xmlStrdup(mpucT);
  }

  return pucResult;
} /* end of FormatCalendarElementDateStr() */


/*! Scans 'pceArgResult' for a datum description and write the results to
  the given addresses.

  skips further scans if result pointer is NULL (speedup for
  calGetYearMinMax())

  check "doc/ISO 8601.mm"

\todo reorder if's, starting with YYYY

\param pceArgResult
\return false in case of error
 */
BOOL_T
ScanCalendarElementDate(ceElementPtr pceArgResult)
{
  BOOL_T fResult = FALSE;

  if (pceArgResult != NULL && pceArgResult->pucIntern != NULL) {

    if (pceArgResult->pucSep == NULL) {
      pceArgResult->pucSep = pceArgResult->pucIntern;
    }

    if (pceArgResult->pucSep != NULL) {
      xmlChar *pucT = pceArgResult->pucIntern; /* shortcut */
      size_t j;
      size_t n = 0;
      int r;
      int ss;
      int iYear, iMonth, iWeek, iQuarter, iDay, iHour, iMinute, iSecond;
      int iYear1, iMonth1, iWeek1, iQuarter1, iDay1, iHour1, iMinute1, iSecond1;
      double dYear, dMonth, dDay, dWeek, dHour, dMinute, dSecond;

#ifdef USE_ALT_DATETIME
      if ((j = dt_parse_unix((const char *)pucT, xmlStrlen(pucT), &pceArgResult->dt0.dt, &pceArgResult->dt0.iSec)) == 10) {
	n += j;
      }
      else  if ((j = dt_parse_german_date((const char *)pucT, xmlStrlen(pucT), &pceArgResult->dt0.dt)) > 5) {
	n += j;
      }
      else 
#endif
      if ((j = dt_parse_iso_recurrence((const char *)pucT, xmlStrlen(pucT), &r)) > 0) {
	/* recurrence/ */

	n += j;
	pceArgResult->iRecurrence = r;

	if (pucT[n] == '/') {
	  n++;

	  if ((j = dt_parse_iso_date_time_zone((const char *)&pucT[n], xmlStrlen(&pucT[n]), &pceArgResult->dt0.dt, &pceArgResult->dt0.iSec)) > 3) {
	    /* recurrence/date */
	    n += j;

	    if (pucT[n] == '/') { /* */
	      n++;

	      if ((j = dt_parse_iso_period((const char *)&pucT[n], xmlStrlen(&pucT[n]), &dYear, &dMonth, &dDay, &dWeek, &dHour, &dMinute, &dSecond)) > 0) {
		/* recurrence/date/period */

		pceArgResult->period.y = (int)dYear;
		pceArgResult->period.m = (int)dMonth;
		pceArgResult->period.d = (int)dDay;
		pceArgResult->period.w = (int)dWeek;
#ifdef USE_ISO_TIME
		pceArgResult->period.hour = (int)dHour;
		pceArgResult->period.minute = (int)dMinute;
		pceArgResult->period.second = (int)dSecond;
		pceArgResult->iRecurrence = r;
#endif

		n += j;
	      }
	      else if ((j = dt_parse_iso_date_time_zone((const char *)&pucT[n], xmlStrlen(&pucT[n]), &pceArgResult->dt1.dt, &pceArgResult->dt1.iSec)) > 3) {
		/* recurrence/date/date */
		n += j;

		if (pucT[n] == '/') { /* */
		  n++;

		  if ((j = dt_parse_iso_period((const char *)&pucT[n], xmlStrlen(&pucT[n]), &dYear, &dMonth, &dDay, &dWeek, &dHour, &dMinute, &dSecond)) > 0) {
		    /* recurrence/date/date/period */

		    pceArgResult->period.y = (int)dYear;
		    pceArgResult->period.m = (int)dMonth;
		    pceArgResult->period.d = (int)dDay;
		    pceArgResult->period.w = (int)dWeek;
#ifdef USE_ISO_TIME
		    pceArgResult->period.hour = (int)dHour;
		    pceArgResult->period.minute = (int)dMinute;
		    pceArgResult->period.second = (int)dSecond;
		    pceArgResult->iRecurrence = r;
#endif

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
	      else {
		n = 0;
	      }
	    }
	    else {
	      /* no interval */
	      n = 0;
	    }
	  }
	  else if ((j = dt_parse_iso_period((const char *)&pucT[n], xmlStrlen(&pucT[n]), &dYear, &dMonth, &dDay, &dWeek, &dHour, &dMinute, &dSecond)) > 0) {
	    /* recurrence/period */

	    n += j;

	    if (pucT[n] == '/') { /* */
	      n++;

	      if ((j = dt_parse_iso_date_time_zone((const char *)&pucT[n], xmlStrlen(&pucT[n]), &pceArgResult->dt1.dt, &pceArgResult->dt1.iSec)) > 3) {
		/* recurrence/period/date */
		n += j;

		if (fabs(dWeek) > DBL_EPSILON) {
		  /* week-period */
		  pceArgResult->dt0.dt = pceArgResult->dt1.dt - (int)(dWeek * 7.0f - 1.0f);
		}
		else {
		  pceArgResult->dt0.dt = dt_add_months(pceArgResult->dt1.dt, -(int)(dYear * 12.0f + dMonth), DT_EXCESS) - 1;
		}

		if (fabs(dDay) > DBL_EPSILON) {
		  pceArgResult->dt0.dt -= (int)(dDay)-1;
		}

#ifdef USE_ISO_TIME
		ss = (int)(dHour * 3600.0f + dMinute * 60.0f + dSecond);
		if (ss > 0) {
		  pceArgResult->dt0.dt -= ss / (24 * 60 * 60); /* time in previous day */
		  pceArgResult->dt0.iSec = pceArgResult->dt1.iSec - ((int)ss % (24 * 60 * 60));
		  //assert(pceArgResult->dt0.iSec > 0);
		}
#endif

		if (pucT[n] == '/') { /* */
		  n++;

		  if ((j = dt_parse_iso_period((const char *)&pucT[n], xmlStrlen(&pucT[n]), &dYear, &dMonth, &dDay, &dWeek, &dHour, &dMinute, &dSecond)) > 0) {
		    /* recurrence/period/date/period */

		    pceArgResult->period.y = (int)dYear;
		    pceArgResult->period.m = (int)dMonth;
		    pceArgResult->period.d = (int)dDay;
		    pceArgResult->period.w = (int)dWeek;
#ifdef USE_ISO_TIME
		    pceArgResult->period.hour = (int)dHour;
		    pceArgResult->period.minute = (int)dMinute;
		    pceArgResult->period.second = (int)dSecond;
		    pceArgResult->iRecurrence = r;
#endif

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
	      else {
		/* no interval */
		n = 0;
	      }
	    }
	    else {
	      n = 0;
	    }
	  }
	  else {
	    n = 0;
	  }
	}
      }
      else if ((j = dt_parse_iso_period((const char *)pucT, xmlStrlen(pucT), &dYear, &dMonth, &dDay, &dWeek, &dHour, &dMinute, &dSecond)) > 0) {
	/* prefix period/ */

	n += j;

	if (pucT[n] == '/') {
	  n++;

	  if ((j = dt_parse_iso_date_time_zone((const char *)&pucT[n], xmlStrlen(&pucT[n]), &pceArgResult->dt1.dt, &pceArgResult->dt1.iSec)) > 3) {

	    if (pucT[0] == 'O') {
	      /* offset/date */

	      if (fabs(dWeek) > DBL_EPSILON) {
		/* week-period */
		pceArgResult->dt0.dt = pceArgResult->dt1.dt - (int)(dWeek * 7.0f + dDay);
	      }
	      else if (fabs(dYear) > DBL_EPSILON || fabs(dMonth) > DBL_EPSILON) {
		pceArgResult->dt0.dt = dt_add_months(pceArgResult->dt1.dt, -(int)(dYear * 12.0f + dMonth), DT_EXCESS);
		pceArgResult->dt0.dt -= (int)(dWeek * 7.0f + dDay);
	      }
	      else if (fabs(dDay) > DBL_EPSILON) {
		pceArgResult->dt0.dt = pceArgResult->dt1.dt - (int)(dDay);
	      }
	      else {}

#ifdef USE_ISO_TIME
	      ss = (int)(dHour * 3600.0f + dMinute * 60.0f + dSecond);
	      if (ss > 0) {
		pceArgResult->dt0.dt -= ss / (24 * 60 * 60); /* time in next day */
		pceArgResult->dt0.iSec = pceArgResult->dt1.iSec - (int)ss % (24 * 60 * 60);
		// assert(pceArgResult->dt1.iSec < (24 * 60 * 60));
	      }
#endif
	      pceArgResult->dt1.dt = 0;
	      pceArgResult->dt1.iSec = 0;
	    }
	    else {
	      /* period/date */

	      if (fabs(dWeek) > DBL_EPSILON) {
		/* week-period */
		pceArgResult->dt0.dt = pceArgResult->dt1.dt - (int)(dWeek * 7.0f - 1.0f);
	      }
	      else {
		pceArgResult->dt0.dt = dt_add_months(pceArgResult->dt1.dt, -(int)(dYear * 12.0f + dMonth), DT_EXCESS) - 1;
	      }

	      if (fabs(dDay) > DBL_EPSILON) {
		pceArgResult->dt0.dt -= (int)(dDay);
	      }

#ifdef USE_ISO_TIME
	      ss = (int)(dHour * 3600.0f + dMinute * 60.0f + dSecond);
	      if (ss > 0) {
		pceArgResult->dt0.dt -= ss / (24 * 60 * 60); /* time in previous day */
		pceArgResult->dt0.iSec = pceArgResult->dt1.iSec - ((int)ss % (24 * 60 * 60));
		assert(pceArgResult->dt0.iSec > 0);
	      }
#endif
	    }
	    n += j;
	  }
	  else if (isdigit(pucT[n]) && isdigit(pucT[n + 1]) && isdigit(pucT[n + 2]) && isdigit(pucT[n + 3]) && pucT[n + 4] == 'W' && isdigit(pucT[n + 5]) &&
		   isdigit(pucT[n + 6]) && !isdigit(pucT[n + 7])) {
	    /* P/YYYYWmm */

	    iYear = 1000 * (pucT[n] - '0') + 100 * (pucT[n + 1] - '0') + 10 * (pucT[n + 2] - '0') + (pucT[n + 3] - '0');
	    if (iYear > -1 && iYear < 2999) {
	      n += 4;

	      iWeek = 10 * (pucT[n + 1] - '0') + (pucT[n + 2] - '0');
	      if (iWeek > -1 && iWeek < 54) {
		n += 3;

		pceArgResult->dt1.dt = dt_from_ywd(iYear, iWeek + 1, 1);

		if (fabs(dWeek) > DBL_EPSILON) {
		  /* week-period */
		  pceArgResult->dt0.dt = pceArgResult->dt1.dt - (int)(dWeek * 7.0f);
		}
		else if (fabs(dYear) > DBL_EPSILON || fabs(dMonth) > DBL_EPSILON) {
		  pceArgResult->dt0.dt = dt_add_months(pceArgResult->dt1.dt, -(int)(dYear * 12.0f + dMonth), DT_EXCESS);
		}

		pceArgResult->dt1.dt--;
	      }
	      else {
		n = 0;
	      }
	    }
	    else {
	      n = 0;
	    }
	  }
	  else if (isdigit(pucT[n]) && isdigit(pucT[n + 1]) && isdigit(pucT[n + 2]) && isdigit(pucT[n + 3]) && isdigit(pucT[n + 4]) && isdigit(pucT[n + 5]) &&
		   !isdigit(pucT[n + 6])) {
	    /* ISO 8601 P/YYYYMM */
	    iYear = 1000 * (pucT[n] - '0') + 100 * (pucT[n + 1] - '0') + 10 * (pucT[n + 2] - '0') + (pucT[n + 3] - '0');
	    if (iYear > -1 && iYear < 2999) {
	      n += 4;
	      iMonth = 10 * (pucT[n] - '0') + (pucT[n + 1] - '0');
	      if (iMonth > 0 && iMonth < 13) {
		n += 3;

		pceArgResult->dt1.dt = dt_from_ymd(iYear, iMonth + 1, 1);

		if (fabs(dYear) > DBL_EPSILON || fabs(dMonth) > DBL_EPSILON) {
 		  pceArgResult->dt0.dt = dt_add_months(pceArgResult->dt1.dt, - (int)(dYear * 12.0f + dMonth), DT_EXCESS);
		}

		pceArgResult->dt1.dt--;
	      }
	      else {
		n = 0;
	      }
	    }
	    else {
	      n = 0;
	    }
	  }
	  else if (isdigit(pucT[n]) && isdigit(pucT[n + 1]) && isdigit(pucT[n + 2]) && isdigit(pucT[n + 3]) && !isiso8601(pucT[n + 4])) {
	    /* ISO 8601 P/YYYY */
	    iYear = 1000 * (pucT[n] - '0') + 100 * (pucT[n + 1] - '0') + 10 * (pucT[n + 2] - '0') + (pucT[n + 3] - '0');
	    if (iYear > -1 && iYear < 2999) {
	      pceArgResult->dt1.dt = dt_from_yd(iYear + 1, 1);
	      if (fabs(dYear) > DBL_EPSILON || fabs(dMonth) > DBL_EPSILON) {
		pceArgResult->dt0.dt = dt_add_months(pceArgResult->dt1.dt, -(int)(dYear * 12.0f + dMonth), DT_EXCESS);
	      }
	      pceArgResult->dt1.dt--;
	      n += 5;
	    }
	    else {
	      n = 0;
	    }
	  }
	  else {
	    n = 0;
	  }
	}
	else {
	  n = 0;
	}
      }
      else if ((j = dt_parse_iso_date_time_zone((const char *)pucT, xmlStrlen(pucT), &pceArgResult->dt0.dt, &pceArgResult->dt0.iSec)) > 3) {
	/* date */
	n += j;

	if (pucT[n] == '/') { /* */
	  n++;

	  if ((j = dt_parse_iso_period((const char *)&pucT[n], xmlStrlen(&pucT[n]), &dYear, &dMonth, &dDay, &dWeek, &dHour, &dMinute, &dSecond)) > 0) {

	    if (pucT[n] == 'O') {
	      /* date/offset */

	      if (fabs(dWeek) > DBL_EPSILON) {
		/* week-period */
		pceArgResult->dt0.dt += (int)(dWeek * 7.0f + dDay);
	      }
	      else if (fabs(dYear) > DBL_EPSILON || fabs(dMonth) > DBL_EPSILON) {
		pceArgResult->dt0.dt = dt_add_months(pceArgResult->dt0.dt, (int)(dYear * 12.0f + dMonth), DT_EXCESS);
		pceArgResult->dt0.dt += (int)(dWeek * 7.0f + dDay);
	      }
	      else if (fabs(dDay) > DBL_EPSILON) {
		pceArgResult->dt0.dt += (int)(dDay);
	      }
	      else {}

#ifdef USE_ISO_TIME
	      ss = (int)(dHour * 3600.0f + dMinute * 60.0f + dSecond);
	      if (ss > 0) {
		pceArgResult->dt0.dt += ss / (24 * 60 * 60); /* time in next day */
		pceArgResult->dt0.iSec += (int)ss % (24 * 60 * 60);
		// assert(pceArgResult->dt1.iSec < (24 * 60 * 60));
	      }
#endif
	    }
	    else {
	      /* date/period */

	      if (fabs(dWeek) > DBL_EPSILON) {
		/* week-period */
		pceArgResult->dt1.dt = pceArgResult->dt0.dt + (int)(dWeek * 7.0f + dDay) - 1;
	      }
	      else if (fabs(dYear) > DBL_EPSILON || fabs(dMonth) > DBL_EPSILON) {
		pceArgResult->dt1.dt = dt_add_months(pceArgResult->dt0.dt, (int)(dYear * 12.0f + dMonth), DT_EXCESS) - 1;
		pceArgResult->dt1.dt += (int)(dWeek * 7.0f + dDay);
	      }
	      else if (fabs(dDay) > DBL_EPSILON) {
		pceArgResult->dt1.dt = pceArgResult->dt0.dt + (int)(dDay)-1;
	      }
	      else {
		pceArgResult->dt1.dt = pceArgResult->dt0.dt;
	      }

#ifdef USE_ISO_TIME
	      ss = (int)(dHour * 3600.0f + dMinute * 60.0f + dSecond);
	      if (ss > 0) {
		pceArgResult->dt1.dt += ss / (24 * 60 * 60); /* time in next day */
		pceArgResult->dt1.iSec = pceArgResult->dt0.iSec + ((int)ss % (24 * 60 * 60));
		// assert(pceArgResult->dt1.iSec < (24 * 60 * 60));
	      }
#endif
	    }
	    n += j;
	  }
	  else if ((j = dt_parse_iso_date_time_zone((const char *)&pucT[n], xmlStrlen(&pucT[n]), &pceArgResult->dt1.dt, &pceArgResult->dt1.iSec)) > 3) {
	    /* date/date */
	    n += j;
	  }
	  else {
	    // n = 0; /* keep valid values */
	  }
	}
	else {
	  /* no interval, simple <date>T<time> */
	}
      }
      else if (isdigit(pucT[n]) && isdigit(pucT[n + 1]) && isdigit(pucT[n + 2]) && isdigit(pucT[n + 3])) {
	/* YYYY */
	iYear = 1000 * (pucT[n] - '0') + 100 * (pucT[n + 1] - '0') + 10 * (pucT[n + 2] - '0') + (pucT[n + 3] - '0');
	if (iYear > -1 && iYear < 2999) {
	  n += 4;
	  if (pucT[n] == '/') {
	    n++;

	    if (isdigit(pucT[n]) && isdigit(pucT[n + 1]) && isdigit(pucT[n + 2]) && isdigit(pucT[n + 3]) && !isiso8601(pucT[n + 4])) {
	      /* ISO 8601 YYYY/YYYY */
	      iYear1 = 1000 * (pucT[n] - '0') + 100 * (pucT[n + 1] - '0') + 10 * (pucT[n + 2] - '0') + (pucT[n + 3] - '0');
	      if (iYear1 > -1 && iYear1 < 2999) {
		pceArgResult->dt0.dt = dt_from_yd(iYear, 1);
		pceArgResult->dt1.dt = dt_from_yd(iYear1 + 1, 0);
		n += 5;
	      }
	      else {
		n = 0;
	      }
	    }
	    else if ((j = dt_parse_iso_period((const char *)&pucT[n], xmlStrlen(BAD_CAST & pucT[n]), &dYear, &dMonth, NULL, NULL, NULL, NULL, NULL)) > 0) {
	      /* YYYY/P */
	      n += j;

	      pceArgResult->dt0.dt = dt_from_yd(iYear, 1);
	      if (fabs(dYear) > DBL_EPSILON || fabs(dMonth) > DBL_EPSILON) {
		pceArgResult->dt1.dt = dt_add_months(pceArgResult->dt0.dt, (int)(dYear * 12.0f + dMonth), DT_EXCESS) - 1;
	      }
	    }
	    else {
	      // n = 0;
	    }
	  }
	  /*! implicit intervals (whole year, month, week)
	   */
	  else if (isdigit(pucT[n]) && isdigit(pucT[n + 1]) && !isdigit(pucT[n + 2])) {
	    /* ISO 8601 YYYYMM */
	    iMonth = 10 * (pucT[n] - '0') + (pucT[n + 1] - '0');
	    if (iMonth > 0 && iMonth < 13) {
	      n += 2;

	      if (pucT[n] == '/') {
		n++;

		if ((j = dt_parse_iso_period((const char *)&pucT[n], xmlStrlen(&pucT[n]), &dYear, &dMonth, NULL, NULL, NULL, NULL, NULL)) > 0) {
		  /* YYYYMM/P */
		  n += j;

		  pceArgResult->dt0.dt = dt_from_ymd(iYear, iMonth, 1);
		  pceArgResult->dt1.dt = dt_add_months(pceArgResult->dt0.dt, (int)(dYear * 12.0f + dMonth), DT_EXCESS) - 1;
		}
		else if (isdigit(pucT[n]) && isdigit(pucT[n + 1]) && isdigit(pucT[n + 2]) && isdigit(pucT[n + 3]) && isdigit(pucT[n + 4]) &&
			 isdigit(pucT[n + 5]) && !isdigit(pucT[n + 6])) {
		  /* ISO 8601 YYYYMM/YYYYMM */

		  iYear1 = 1000 * (pucT[n] - '0') + 100 * (pucT[n + 1] - '0') + 10 * (pucT[n + 2] - '0') + (pucT[n + 3] - '0');
		  if (iYear1 > -1 && iYear1 < 2999) {
		    n += 4;
		    iMonth1 = 10 * (pucT[n] - '0') + (pucT[n + 1] - '0');
		    if (iMonth1 > 0 && iMonth1 < 13) {
		      n += 2;
		      pceArgResult->dt0.dt = dt_from_ymd(iYear, iMonth, 1);
		      pceArgResult->dt1.dt = dt_from_ymd(iYear1, iMonth1 + 1, 0);
		    }
		    else {
		      n = 0;
		    }
		  }
		}
		else {
		  n = 0;
		}
	      }
	      else {
		/* YYYYMM */
		pceArgResult->dt0.dt = dt_from_ymd(iYear, iMonth, 1);
		pceArgResult->dt1.dt = dt_end_of_month(pceArgResult->dt0.dt, 0);
	      }
	    }
	    else {
	      n = 0;
	    }
	  }
	  else if (pucT[n] == 'W' && isdigit(pucT[n + 1]) && isdigit(pucT[n + 2]) && !isdigit(pucT[n + 3])) {
	    /* ISO 8601 YYYYWnn */
	    iWeek = 10 * (pucT[n + 1] - '0') + (pucT[n + 2] - '0');
	    if (iWeek > -1 && iWeek < 54) {
	      n += 3;

	      if (pucT[n] == '/') {
		n++;
		/* YYYYWnn/ */

		if ((j = dt_parse_iso_period((const char *)&pucT[n], xmlStrlen(&pucT[n]), &dYear, &dMonth, &dDay, &dWeek, NULL, NULL, NULL)) > 0) {
		  /* YYYYWnn/P */

		  n += j;
		  pceArgResult->dt0.dt = dt_from_ywd(iYear, iWeek, 1);
		  pceArgResult->dt1.dt = dt_add_months(pceArgResult->dt0.dt, (int)(dYear * 12.0f + dMonth), DT_EXCESS);
		  pceArgResult->dt1.dt += (int)(dWeek * 7.0f + dDay) - 1;
		}
		else if (isdigit(pucT[n]) && isdigit(pucT[n + 1]) && isdigit(pucT[n + 2]) && isdigit(pucT[n + 3]) && pucT[n + 4] == 'W' &&
			 isdigit(pucT[n + 5]) && isdigit(pucT[n + 6]) && !isdigit(pucT[n + 7])) {
		  /* YYYYWnn/YYYYWmm */

		  iYear1 = 1000 * (pucT[n] - '0') + 100 * (pucT[n + 1] - '0') + 10 * (pucT[n + 2] - '0') + (pucT[n + 3] - '0');
		  if (iYear1 > -1 && iYear1 < 2999) {
		    n += 4;

		    iWeek1 = 10 * (pucT[n + 1] - '0') + (pucT[n + 2] - '0');
		    if (iWeek1 > -1 && iWeek1 < 54) {
		      n += 3;
		      pceArgResult->dt0.dt = dt_from_ywd(iYear, iWeek, 1);
		      pceArgResult->dt1.dt = dt_from_ywd(iYear1, iWeek1, 7);
		    }
		    else {
		      n = 0;
		    }
		  }
		  else {
		    n = 0;
		  }
		}
		else {
		  n = 0;
		}
	      }
	      else {
		pceArgResult->dt0.dt = dt_from_ywd(iYear, iWeek, 1);
		pceArgResult->dt1.dt = dt_end_of_week(pceArgResult->dt0.dt, DT_MON);
	      }
	    }
	    else {
	      n = 0;
	    }
	  }
	  else if (pucT[n] == 'Q' && isdigit(pucT[n + 1]) && !isdigit(pucT[n + 2])) {
	    /* ISO 8601 YYYYQn */
	    iQuarter = (pucT[n + 1] - '0');
	    if (iQuarter > 0 && iQuarter < 5) {
	      n += 2;

	      if (pucT[n] == '/') {
		n++;
		/* YYYYQn/ */

		if (isdigit(pucT[n]) && isdigit(pucT[n + 1]) && isdigit(pucT[n + 2]) && isdigit(pucT[n + 3]) && pucT[n + 4] == 'Q' &&
			 isdigit(pucT[n + 5]) && !isdigit(pucT[n + 6])) {
		  /* YYYYQn/YYYYQm */

		  iYear1 = 1000 * (pucT[n] - '0') + 100 * (pucT[n + 1] - '0') + 10 * (pucT[n + 2] - '0') + (pucT[n + 3] - '0');
		  if (iYear1 > -1 && iYear1 < 2999) {
		    n += 4;

		    iQuarter1 = (pucT[n + 1] - '0');
		    if (iQuarter1 > 0 && iQuarter1 < 5) {
		      n += 2;
		      pceArgResult->dt0.dt = dt_from_ymd(iYear, ((iQuarter - 1) * 3) + 1, 1);
		      pceArgResult->dt1.dt = dt_from_ymd(iYear1, (iQuarter1 * 3) + 1, 0);
		    }
		    else {
		      n = 0;
		    }
		  }
		  else {
		    n = 0;
		  }
		}
		else {
		  n = 0;
		}
	      }
	      else {
		pceArgResult->dt0.dt = dt_from_ymd(iYear, ((iQuarter - 1) * 3) + 1, 1);
		pceArgResult->dt1.dt = dt_from_ymd(iYear, (iQuarter * 3) + 1, 0);
	      }
	    }
	    else {
	      n = 0;
	    }
	  }
	  else if (!isiso8601(pucT[n])) {
	    /* ISO 8601 YYYY */
	    pceArgResult->dt0.dt = dt_from_ymd(iYear, 1, 1);
	    pceArgResult->dt1.dt = dt_end_of_year(pceArgResult->dt0.dt, 0);
	  }
	  else {
	    n = 0;
	  }
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
	else if (pceArgResult->dt0.iSec > 0 || pceArgResult->dt1.iSec > 0) {
	  /* same day, different time */
	}
	else {
	  /* it's not an interval */
	  pceArgResult->dt1.dt = 0;
	}
      }

      //pceArgResult->pucSep += n;
      fResult = n > 0;
    }
  }

  return fResult;
} /* end of ScanCalendarElementDate() */


/*! insert a day diff attribute to pndArg
*/
BOOL_T
AddNodeDateAttributes(xmlNodePtr pndArg, ceElementPtr pceArg)
{
  BOOL_T fResult = FALSE;

  if (pndArg) {

    if (pceArg) {
      xmlChar mpucT[BUFFER_LENGTH];
      dt_t iDayToday;
      dt_t iDayEnd;
      long int iDayDiff = 0;

      iDayToday = GetToday();

      if ((pceArg->dt0.dt > 0 && pceArg->dt1.dt > pceArg->dt0.dt) || (pceArg->dt0.iSec > 0 && pceArg->dt1.iSec > 0)) {
	/* an interval with begin and end */

	xmlStrPrintf(mpucT, BUFFER_LENGTH, "%04i-%02i-%02i",
	  dt_year(pceArg->dt0.dt), dt_month(pceArg->dt0.dt), dt_dom(pceArg->dt0.dt));
	xmlSetProp(pndArg, BAD_CAST"begin", mpucT);

	xmlStrPrintf(mpucT, BUFFER_LENGTH, "%04i-%02i-%02i",
	  dt_year(pceArg->dt1.dt), dt_month(pceArg->dt1.dt), dt_dom(pceArg->dt1.dt));
	xmlSetProp(pndArg, BAD_CAST"end", mpucT);

	/*!\todo use an ics namespace for these attributes */

	if (pceArg->dt0.iSec > 0) {
	  xmlStrPrintf(mpucT, BUFFER_LENGTH, "%04i%02i%02iT%02i%02i%02iZ",
	    dt_year(pceArg->dt0.dt), dt_month(pceArg->dt0.dt), dt_dom(pceArg->dt0.dt),
	    pceArg->dt0.iSec / 3600, (pceArg->dt0.iSec % 3600) / 60, pceArg->dt0.iSec % 60);
	}
	else {
	  xmlStrPrintf(mpucT, BUFFER_LENGTH, "%04i%02i%02i",
	    dt_year(pceArg->dt0.dt), dt_month(pceArg->dt0.dt), dt_dom(pceArg->dt0.dt));
	}
	xmlSetProp(pndArg, BAD_CAST"DTSTART", mpucT);

	if (pceArg->dt1.iSec > 0) {
	  xmlStrPrintf(mpucT, BUFFER_LENGTH, "%04i%02i%02iT%02i%02i%02iZ",
	    dt_year(pceArg->dt1.dt), dt_month(pceArg->dt1.dt), dt_dom(pceArg->dt1.dt),
	    pceArg->dt1.iSec / 3600, (pceArg->dt1.iSec % 3600) / 60, pceArg->dt1.iSec % 60);
	}
	else if (pceArg->dt0.iSec > 0) {
	  /*! use a 15 minutes "dummy" duration to display event in calendar */
	  xmlStrPrintf(mpucT, BUFFER_LENGTH, "%04i%02i%02iT%02i%02i%02iZ",
	    dt_year(pceArg->dt0.dt), dt_month(pceArg->dt0.dt), dt_dom(pceArg->dt0.dt),
	    pceArg->dt0.iSec / 3600, (pceArg->dt0.iSec % 3600) / 60 + 15, pceArg->dt0.iSec % 60);
	}
	else if (pceArg->dt1.dt > pceArg->dt0.dt) {
	  iDayEnd = pceArg->dt1.dt + 1;
	  xmlStrPrintf(mpucT, BUFFER_LENGTH, "%04i%02i%02i",
	    dt_year(iDayEnd), dt_month(iDayEnd), dt_dom(iDayEnd));
	}
	else {
	  iDayEnd = pceArg->dt0.dt + 1;
	  xmlStrPrintf(mpucT, BUFFER_LENGTH, "%04i%02i%02i",
	    dt_year(iDayEnd), dt_month(iDayEnd), dt_dom(iDayEnd));
	}
	xmlSetProp(pndArg, BAD_CAST"DTEND", mpucT);

	xmlStrPrintf(mpucT, BUFFER_LENGTH, "%i", pceArg->dt1.dt - pceArg->dt0.dt + 1);
	xmlSetProp(pndArg, BAD_CAST"interval", mpucT);
      }
      else if (pceArg->dt1.dt > 0 || pceArg->dt0.dt > 0) {
	/* single date only */

	iso8601DateType *pdti; /* */

	pdti = (pceArg->dt1.dt > 0) ? &pceArg->dt1 : &pceArg->dt0;

	if (pceArg->iRecurrenceIndex > 0) {
	  xmlStrPrintf(mpucT, BUFFER_LENGTH, "%i", pceArg->iRecurrenceIndex);
	  xmlSetProp(pndArg, BAD_CAST "i", mpucT);
	}

	xmlStrPrintf(mpucT, BUFFER_LENGTH, "%04i-%02i-%02i",
	  dt_year(pdti->dt), dt_month(pdti->dt), dt_dom(pdti->dt));
	xmlSetProp(pndArg, BAD_CAST"iso", mpucT);

	if (pdti->iSec > 0) {
	  xmlStrPrintf(mpucT, BUFFER_LENGTH, "%04i%02i%02iT%02i%02i%02iZ",
	    dt_year(pdti->dt), dt_month(pdti->dt), dt_dom(pdti->dt),
	    pdti->iSec / 3600, (pdti->iSec % 3600) / 60, pdti->iSec % 60);
	}
	else {
	  xmlStrPrintf(mpucT, BUFFER_LENGTH, "%04i%02i%02i",
	    dt_year(pdti->dt), dt_month(pdti->dt), dt_dom(pdti->dt));
	}
	xmlSetProp(pndArg, BAD_CAST"DTSTART", mpucT);

	if (pdti->iSec > 0) {
	  xmlStrPrintf(mpucT, BUFFER_LENGTH, "%04i%02i%02iT%02i%02i%02iZ",
	    dt_year(pdti->dt), dt_month(pdti->dt), dt_dom(pdti->dt),
	    pdti->iSec / 3600, (pdti->iSec % 3600) / 60 + 15, pdti->iSec % 60);
	}
	else {
	  iDayEnd = pdti->dt + 1;
	  xmlStrPrintf(mpucT, BUFFER_LENGTH, "%04i%02i%02i",
	    dt_year(iDayEnd), dt_month(iDayEnd), dt_dom(iDayEnd));
	}
	xmlSetProp(pndArg, BAD_CAST"DTEND", mpucT);

	/*!\todo process reccurancing dates in ICS */
      }

      //xmlStrPrintf(mpucT, BUFFER_LENGTH, "%i", pceArg->dt0.dt);
      //xmlSetProp(pndArg, BAD_CAST"abs", mpucT);

      if (pceArg->dt1.dt > pceArg->dt0.dt) {
	if (pceArg->dt1.dt < iDayToday) {
	  /* date interval ends before today */
	  iDayDiff = pceArg->dt1.dt - iDayToday;
	}
	else if (iDayToday < pceArg->dt0.dt) {
	  /* date interval begins after today */
	  iDayDiff = pceArg->dt0.dt - iDayToday;
	}
	else {
	  /* today is in date interval */
	}
      }
      else {
	/* no interval end, single date */
	iDayDiff = pceArg->dt0.dt - iDayToday;
      }

#ifdef DEBUG
      xmlSetProp(pndArg, BAD_CAST "src", pceArg->pucIntern);
#endif

      xmlStrPrintf(mpucT, BUFFER_LENGTH, "%li", iDayDiff);
      xmlSetProp(pndArg, BAD_CAST"diff", mpucT);

    }
    else if (pndArg->children != NULL && STR_IS_NOT_EMPTY(pndArg->children->content)) {
      xmlChar* pucDate;
      ceElementPtr pceT;

      pucDate = pndArg->children->content;

      if ((pceT = CalendarElementNew(pucDate))) {
	if (ScanCalendarElementDate(pceT)) {
	  xmlChar mpucT[BUFFER_LENGTH];
	  ceElementPtr pceList;

	  /*! add separate date elements with own DTSTART/DTEND attributes */
	  if ((pceList = SplitCalendarElementRecurrences(pceT))) {
	    xmlChar* pucDisplay = NULL;
	    ceElementPtr pceI;

	    for (pceI = pceList; pceI; pceI = pceI->pNext) {
	      xmlChar* pucDateNew;

	      pucDateNew = GetISO8601DateStr(pceI->dt0.dt > 0 ? &pceI->dt0 : &pceI->dt1);
	      if (pucDateNew) {
		xmlNodePtr pndDateNew;

		if ((pndDateNew = xmlNewNode(NULL, pndArg->name)) != NULL) {
		  AddNodeDateAttributes(pndDateNew, pceI); /* recursion */
		  xmlAddSibling(pndArg, pndDateNew);
		}

		if (pucDisplay) {
		  /* concat at the end of pucDisplay */
		  pucDisplay = xmlStrcat(pucDisplay, BAD_CAST", ");
		  pucDisplay = xmlStrcat(pucDisplay, pucDateNew);
		  xmlFree(pucDateNew);
		}
		else {
		  pucDisplay = pucDateNew;
		}
	      }
	      else {
		/* ignore empty value */
	      }
	    }

	    if (STR_IS_NOT_EMPTY(pucDisplay)) {
	      domSetPropEat(pndArg, BAD_CAST"iso", pucDisplay);
	    }

	    CalendarElementFree(pceList);
	  }
	  else {
	    AddNodeDateAttributes(pndArg, pceT); /* recursion */
	  }

	  fResult = TRUE;
	}
	else {
#ifdef DEBUG
#else
	  /* invalid date string, replace node by its text children */
	  xmlNodePtr pndTT = pndArg;
	  xmlNodePtr pndT = pndTT->children;

	  xmlUnlinkNode(pndT);
	  xmlReplaceNode(pndTT,pndT);
	  xmlFreeNode(pndTT);
#endif
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
  xmlChar *pucEnvDate;

  if (STR_IS_NOT_EMPTY(pucArgToday)) {
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
    dtToday = dt_today_date();
  }

  return dtToday;
} /* end of UpdateToday() */


#ifdef TESTCODE
#include "test/test_calendar_element.c"
#endif

