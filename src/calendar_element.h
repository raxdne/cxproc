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

#include <c-dt/dt.h>

/*! structure
 */
struct ceElement {

  xmlNodePtr pndEntry; /*!< pointer to anchor DOM node */
  xmlAttrPtr patAttr; /*!< pointer to descendant DOM attribute of pndEntry */

  xmlChar *pucColId; /*!< pointer to column id of Date anchor */
  xmlChar *pucId; /*!< pointer to ID string */
  xmlChar *pucDate; /*!< pointer to single date string */
  xmlChar *pucSep; /*!< pointer to iteration separator string */

  dt_t dtBegin; /*!< day index of Interval Begin Date */
  int iSecBegin;

  dt_t dtEnd;   /*!< day index of Interval End Date */
  int iSecEnd;

  struct {
    int y;
    int m;
    int d;
    int w;
    int hour;
    int minute;
    int second;
  } period;

  int iRecurrence;	/*! recurrences */

  int iTimezone;	/*!< numerical ID for timezone (UTC by default) */
  int iTimezoneOffset; /*!< offset to UTC in minutes */

  struct ceElement *pNext;
} ;

typedef struct ceElement ceElementType;

typedef ceElementType *ceElementPtr;


extern const xmlChar *dow[];

extern const xmlChar *moy[];

extern BOOL_T
ceInit(void);

extern ceElementPtr
CalendarElementNew(xmlChar *pucArg);

extern ceElementPtr
CalendarElementUpdate(ceElementPtr pceArg, xmlChar* pucArg);

extern BOOL_T
CalendarElementUpdateValues(ceElementPtr pceArg);

extern void
CalendarElementFree(ceElementPtr pceArg);

extern ceElementPtr
CalendarElementDup(ceElementPtr pceArg);

extern BOOL_T
CalendarElementListAdd(ceElementPtr pceArgList, ceElementPtr pceArg);

extern dt_t
UpdateToday(xmlChar *pucArgToday);

extern dt_t
GetToday(void);

#if defined(DEBUG) || defined(TESTCODE)

extern void
PrintCalendarElement(ceElementPtr pceArg);

#endif

extern BOOL_T
ScanCalendarElementDate(ceElementPtr pceArgResult);

extern ceElementPtr
SplitCalendarElementRecurrences(ceElementPtr pceArg);

extern BOOL_T
AddNodeDateAttributes(xmlNodePtr pndArg, xmlChar* pucArg);

#ifdef TESTCODE
extern int
ceTest(void);
#endif
