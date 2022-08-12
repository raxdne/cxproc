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
#include <cxp/calendar_element.h>

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

      /*!\todo check use of dt_parse_iso_date_interval() to skip ScanCalendarElementDate() */

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
} /* End of CalendarElementDup() */


/*! \return a freshly allocated copy of pceArg
 */
pieCalendarElementPtr
CalendarElementReset(pieCalendarElementPtr pceArg)
{
  if (pceArg) {
    if (pceArg->pucId) {
      xmlFree(pceArg->pucId);
    }
    pceArg->pucId = NULL;
    pceArg->pucDate = NULL;
    pceArg->pucSep = NULL;
    pceArg->pNext = NULL;

    pceArg->iYear = -1;
    pceArg->iMonth = -1;
    pceArg->iDay = -1;
    pceArg->iWeek = -1;
    pceArg->iDayWeek = -1;
    pceArg->iStep = -1;

    pceArg->iHourA = -1;
    pceArg->iMinuteA = -1;
    pceArg->iSecondA = -1;

    pceArg->iHourB = -1;
    pceArg->iMinuteB = -1;
    pceArg->iSecondB = -1;

    pceArg->eTypeDate = DATE_ERROR;

  }
  return pceArg;
} /* End of CalendarElementReset() */


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
  if (pceArg->pNext) {
    CalendarElementFree(pceArg->pNext);
  }
  xmlFree(pceArg->pucDate);
  xmlFree(pceArg->pucId);
  xmlFree(pceArg);
}
/* end of CalendarElementFree() */


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
CalendarElementUpdateValues(pieCalendarElementPtr pceArg)
{
  BOOL_T fResult = FALSE;
  
  if (pceArg != NULL && pceArg->iAnchor > -1) {
    pceArg->iYear    = dt_year(pceArg->iAnchor);
    pceArg->iMonth   = dt_month(pceArg->iAnchor);
    pceArg->iDay     = dt_dom(pceArg->iAnchor);
    pceArg->iWeek    = dt_woy(pceArg->iAnchor);
    pceArg->iDayWeek = dt_dow(pceArg->iAnchor);
    fResult = TRUE;
  }
  return fResult;
} /* end of CalendarElementUpdateValues() */


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
    pceArgResult->eTypeDate = DATE_ERROR;

    if (pceArgResult->pucSep == NULL) {
      pceArgResult->pucSep = pceArgResult->pucDate;
    }

    if (pceArgResult->pucSep != NULL) {
      dt_t dt0, dt1;
      size_t l;
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
	pceArgResult->iAnchor = dt_from_struct_tm(localtime((const time_t*)(&tT)));
	pceArgResult->eTypeDate = DATE_SYSTEM;
	pceArgResult->iStep = -1;
	pceArgResult->iCount = -1;

	for (pceArgResult->pucSep += 10; isdigit(*(pceArgResult->pucSep)); pceArgResult->pucSep++) {
	  /* skip chars of millisecs */
	}

	fResult = TRUE;
      }
      else if ((l = dt_parse_iso_date_interval(pceArgResult->pucSep, xmlStrlen(pceArgResult->pucSep), &dt0, &dt1)) > 0) {
	if (dt0) {
	  pceArgResult->eTypeDate = DATE_ISO;
	  pceArgResult->iAnchor = dt0;

	  if (dt1 > dt0) {
	    /* an interval end */
	    pceArgResult->iStep = 1;
	    pceArgResult->iCount = dt1 - dt0;
	  }
	  else {
	    /* no interval end */
	    pceArgResult->iStep = -1;
	    pceArgResult->iCount = -1;
	  }
	  fResult = TRUE;
	}
	else {
	}
	pceArgResult->pucSep = &pceArgResult->pucSep[l-1];
      }

      if (fResult) {
	CalendarElementUpdateValues(pceArgResult);
      }
    }
  }
  
  return fResult;
} /* end of ScanCalendarElementDate() */


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

