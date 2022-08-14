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

#ifndef _TIMEZONE_H_
#include <cxp/cxp_timezone.h>
#endif

#include <c-dt/dt.h>

/*! structure
\todo iAnchor OR iYear,iMonth,iDay,...
 */
struct pieCalendarElement {
  xmlNodePtr pndEntry; /*!< pointer to anchor DOM node */
  xmlAttrPtr patAttr; /*!< pointer to descendant DOM attribute of pndEntry */
  xmlChar *pucColId; /*!< pointer to column id of Date anchor */
  xmlChar *pucId; /*!< pointer to ID string */
  xmlChar *pucDate; /*!< pointer to single date string */
  xmlChar *pucSep; /*!< pointer to iteration separator string */
  dt_t iAnchor; /*!< day index of Date anchor */
  int iStep; /*!< iterator increment of day steps */
  int iCount; /*!< iteration count */
  int iYear;  /*!< Date year anchor */
  int iMonth;  /*!< Date month anchor */
  int iDay;  /*!< Date day anchor */
  int iWeek; /*!< Date week anchor */
  int iDayWeek; /*!< Date day of week anchor */

  /* values of begin time */
  int iHourA;  /*!< Hour of Time */
  int iMinuteA;  /*!< Minute of Time */
  int iSecondA;  /*!< Second of Time */

  int iTimezone;	/*!< numerical ID for timezone (UTC by default) */
  int iTimezoneOffset; /*!< offset to UTC in minutes */

  /* values of end time */
  int iHourB;  /*!< Hour of Time */
  int iMinuteB;  /*!< Minute of Time */
  int iSecondB;  /*!< Second of Time */

  struct pieCalendarElement *pNext;
} ;

typedef struct pieCalendarElement pieCalendarElementType;

typedef pieCalendarElementType *pieCalendarElementPtr;


extern const xmlChar *dow[];

extern const xmlChar *dow_de[];

extern const xmlChar *moy[];

extern const xmlChar *moy_de[];

extern BOOL_T
ceInit(void);

extern pieCalendarElementPtr
CalendarElementNew(xmlChar *pucArg);

extern pieCalendarElementPtr
CalendarElementUpdate(pieCalendarElementPtr pceArg, xmlChar* pucArg);

extern BOOL_T
CalendarElementUpdateValues(pieCalendarElementPtr pceArg);

extern void
CalendarElementFree(pieCalendarElementPtr pceArg);

extern pieCalendarElementPtr
CalendarElementDup(pieCalendarElementPtr pceArg);

extern BOOL_T
CalendarElementListAdd(pieCalendarElementPtr pceArgList, pieCalendarElementPtr pceArg);

extern int
GetRunningTime();

extern dt_t
UpdateToday(xmlChar *pucArgToday);

extern long int
GetToday(void);

extern int
GetTodayYear(void);

extern int
GetTodayMonth(void);

extern int
GetTodayDayOfMonth(void);

extern int
GetTodayDayOfYear(void);

extern int
GetTodayWeek(void);

extern xmlChar *
GetTodayTag(void);

extern int
GetTodayTime(void);

extern int
GetTodayHour(void);

extern int
GetTodayMinute(void);

extern int
GetTodaySecond(void);

extern void
PrintCalendarElement(pieCalendarElementPtr pceArg);

extern BOOL_T
ScanCalendarElementDate(pieCalendarElementPtr pceArgResult);


#ifdef TESTCODE
extern int
ceTest(void);
#endif
