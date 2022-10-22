/*
  cxproc - Configurable Xml PROCessor

  Copyright (C) 2006..2022 by Alexander Tenbusch

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

/*!\todo display of database entries with one date formatted column */

/*!\todo     „20170719 script="Date() - Date(1989-11-12T11:00:00Z)" days old“ */

/*!\todo NameOfMonth in English, German ... (in calendar definition?) */


/*  */

#include <math.h>

#include <libxml/tree.h>

#ifdef LEGACY
#include <sunriset/sunriset.h>
#endif

/* 
 */
#include "basics.h"
#include "utils.h"
#include <res_node/res_node.h>
#include <cxp/cxp.h>
#include <cxp/cxp_dir.h>
#include "dom.h"

#include <pie/pie_text.h>

const char *mpucNumber[] = {
			    "00","01","02","03","04","05","06","07","08","09",
			    "10","11","12","13","14","15","16","17","18","19",
			    "20","21","22","23","24","25","26","27","28","29",
			    "30","31","32","33","34","35","36","37","38","39",
			    "40","41","42","43","44","45","46","47","48","49",
			    "50","51","52","53","54","55","56","57","58","59",
			    "60",NULL};
      

#define CXP_CALENDAR_YEAR_MIN (1970)

#define CXP_CALENDAR_YEAR_MAX (CXP_CALENDAR_YEAR_MIN + 100)

#define CXP_CALENDAR_SIZE ((CXP_CALENDAR_YEAR_MAX - CXP_CALENDAR_YEAR_MIN + 1) * 366 * sizeof(xmlNodePtr))

#define cxpCalendarIndex(I) (I - dt_from_yd(CXP_CALENDAR_YEAR_MIN, 1))

typedef enum {
  CXP_CALENDAR_MDAY, 
  CXP_CALENDAR_WDAY, 
  CXP_CALENDAR_YDAY,
  CXP_CALENDAR_MDAY_HOUR,
  CXP_CALENDAR_WDAY_HOUR, 
  CXP_CALENDAR_YDAY_HOUR,
  CXP_CALENDAR_WEEK,
  CXP_CALENDAR_MONTH, 
  CXP_CALENDAR_YEAR
} cxpCalendarType;

/*! structure
 */
typedef struct {
  xmlDocPtr pdocCalendar; /*!< pointer to DOM of result calendar DOM */
  xmlNodePtr pndCalendarRoot;
  //xmlNode **mpndDay; /*!< array of pointers to day nodes of pdocCalendar (performance vector) */
  xmlNodePtr mpndDay[CXP_CALENDAR_SIZE];
  ceElementPtr pceFirst;
  cxpCalendarType eType;
  
  unsigned int ciEntries;
  int *pmiYear; /*!< array of requiered years, derived from attributes in pdocSource */

  BOOL_T fCoordinate;
  double dLatitude;
  double dLongitude;

  int iTimezone;	/*!< numerical ID for timezone (UTC by default) */
  int iTimezoneOffset;  /*!< offset to UTC in minutes */
} cxpCalendar;

typedef cxpCalendar *cxpCalendarPtr;

/* Global 
 */
const xmlChar *moy[] = {
  BAD_CAST "",
  BAD_CAST "January",
  BAD_CAST "February",
  BAD_CAST "March",
  BAD_CAST "April",
  BAD_CAST "May",
  BAD_CAST "June",
  BAD_CAST "July",
  BAD_CAST "August",
  BAD_CAST "September",
  BAD_CAST "October",
  BAD_CAST "November",
  BAD_CAST "December",
  NULL
};

/*!\todo
BOOL_T
CalendarSwitchTimezone(cxpCalendarPtr pCalendarArg, pie_tz_t tzArg);
 */

static void
GetYearArray(cxpCalendarPtr pCalendarArg);

static BOOL_T
AddTreeYear(cxpCalendarPtr pCalendarArg, int year);

static void
GetYearMinMax(cxpCalendarPtr pCalendarArg, int *year_min, int *year_max);

static BOOL_T
AddYears(cxpCalendarPtr pCalendarArg);

static xmlNodePtr
FindCalendarElementCol(xmlNodePtr pndArgParent, xmlChar *pucArgIdCol, xmlNodePtr pndArgInsert);

static void
CalendarUpdate(cxpCalendarPtr pCalendarArg);

static BOOL_T
RegisterAndParseDateNodes(cxpCalendarPtr pCalendarArg, xmlChar *pucArg);

static BOOL_T
ProcessCalendarColumns(cxpCalendarPtr pCalendarArg, cxpContextPtr pccArg);

static void
CalendarColumnsFree(cxpCalendarPtr pCalendarArg);

static void
CalendarFree(cxpCalendarPtr pCalendarArg);

static cxpCalendarPtr
CalendarNew(void);

static cxpCalendarPtr
CalendarSetup(xmlNodePtr pndMakeCalendar, cxpContextPtr pccArg);

static xmlNodePtr
GetCalendarNodeYear(xmlNodePtr pndArg, unsigned int uiArgYear);

static xmlNodePtr
GetCalendarNodeMonth(xmlNodePtr pndArg, unsigned int uiArgYear, unsigned int uiArgMonth);

static xmlNodePtr
GetCalendarNodeDay(xmlNodePtr pndArg, unsigned int uiArgYear, unsigned int uiArgMonth, unsigned int uiArgDay);

static xmlNodePtr
GetCalendarNodeDayHour(xmlNodePtr pndArg, unsigned int uiArgYear, unsigned int uiArgMonth, unsigned int uiArgDay, unsigned int uiArgHour);

static xmlNodePtr
GetCalendarNodeWeek(xmlNodePtr pndArg, unsigned int uiArgYear, unsigned int uiArgWeek);

static xmlChar *
SubstituteFormatStr(xmlNodePtr pndContext, xmlChar *fmt);

static BOOL_T
SubstituteFormat(xmlNodePtr pndArg);

static xmlNodePtr
GetCalendarNodeNext(const xmlNodePtr pndI, int distance);

static BOOL_T
IsFullMoonConway(int year, int month, int day);

#if defined(DEBUG) || defined(TESTCODE)
BOOL_T
PrintCalendarSetup(cxpCalendarPtr pCalendarArg, cxpContextPtr pccArg);
#endif


/*! Searches in a CALENDAR DOM for the next node with same name like pndArg.

\param pndArg start node
\param distance between result and pndArg

Both directions are supported. Distance values lower than zero means
backward searching.
*/
xmlNodePtr
GetCalendarNodeNext(const xmlNodePtr pndArg, int distance)
{
  BOOL_T fForward = TRUE;
  xmlNodePtr pndN;

  if (pndArg && pndArg->type == XML_ELEMENT_NODE) {

    if (IS_NODE_CXP_WEEK(pndArg)) {
      /* pndArg is a WEEK element 
       */
      if (distance == 0) {
	/*! nearest distance is reached */
	return pndArg;
      }
      else if (distance > 0) {  /* search forward */
	distance--;
      }
      else {  /*!\todo search backward */
	fForward = FALSE;
	distance++;
      }
      if (fForward) {
	return GetCalendarNodeNext(domGetNextNode(pndArg,NULL),distance);
      }
      else {
	assert(FALSE);		/*!\todo backward movement */
      }
    }
    else if (IS_NODE_CXP_DAY(pndArg)) {
      /* pndArg is a DAY element 
       */
      if (distance == 0) {
	/*! nearest distance is reached */
	return pndArg;
      }
      else if (distance > 0) {  /* search forward */
	distance--;
      }
      else {  /*!\todo search backward */
	fForward = FALSE;
	distance++;
      }

      if (fForward) {
	if ((pndN = domGetNextNode(pndArg,NULL))) {
	  return GetCalendarNodeNext(pndN,distance);
	}
      }
      else {
	if (pndArg->prev) {	/*!\todo use prev_element(pndArg,NULL) */
	  /*!\bug when element in previuos period, pndArg->prev==NULL */
	  return GetCalendarNodeNext(pndArg->prev,distance);
 	}
      }
      /*
	no next/prev DAY node in this MONTH found
       */
      if (pndArg->parent) {
	xmlNodePtr pndPn;
	xmlNodePtr pndPnc;
	if (fForward) {
	  /* search for a pndArg->parent->next->children */
	  for (pndPn=pndArg->parent->next; pndPn; pndPn=pndPn->next) {
	    if ((pndPnc = domGetFirstChild(pndPn,BAD_CAST pndArg->name))) {
	      /* use first DAY in next MONTH */
	      return GetCalendarNodeNext(pndPnc,distance);
	    }
	  }
	}
	else {
	  for (pndPn=pndArg->parent->prev; pndPn; pndPn=pndPn->prev) {
	    if (pndPn->last) {
	      /* use last DAY in prev MONTH */
	      return GetCalendarNodeNext(pndPn->last,distance);
	    }
	  }
	}
	/*
	  no next/prev MONTH node in this YEAR found
	  */
	if (pndArg->parent->parent) {
	  xmlNodePtr pndPpn;
	  if (fForward) {
	    /* else search for a pndArg->parent->parent->next->children->children */
	    for (pndPpn = pndArg->parent->parent->next; pndPpn; pndPpn = pndPpn->next) {
	      if (pndPpn->children && pndPpn->children->children) {
		/* use first DAY in next MONTH */
		return GetCalendarNodeNext(pndPpn->children->children, distance);
	      }
	    }
	  }
	  else {
	    for (pndPpn = pndArg->parent->parent->prev; pndPpn; pndPpn = pndPpn->prev) {
	      if (pndPpn->last && pndPpn->last->last) {
		/* use last DAY in last MONTH in prev YEAR */
		return GetCalendarNodeNext(pndPpn->last->last, distance);
	      }
	    }
	  }
	}
      }
    }
    else {
      /* other elements from recursive calls */
    }
  }
  return NULL;
}
/* End of GetCalendarNodeNext() */


/*! sets the lower and upper values for year, according to registered calendar elements
 */
void
GetYearMinMax(cxpCalendarPtr pCalendarArg, int *year_min, int *year_max)
{
  ceElementPtr pceT;

  for (pceT = pCalendarArg->pceFirst; pceT; pceT = pceT->pNext) {
    int y1, y0;

    if (pceT->iRecurrence == -1) {
      /* this calendar element is resulting from a recurrence */
      continue;
    }

    y0 = dt_year(pceT->dt0.dt);
    if (y0 > 0) {
      if (y0 < *year_min) {
	*year_min = y0;
	if (*year_max == 0) {
	  *year_max = *year_min;
	}
      }
      if (y0 > *year_max) {
	*year_max = y0;
	if (*year_min == 2999) {
	  *year_min = *year_max;
	}
      }
    }

    y1 = dt_year(pceT->dt1.dt);
    if (y1 > 0) {
      if (y1 < *year_min) {
	*year_min = y1;
	if (*year_max == 0) {
	  *year_max = *year_min;
	}
      }
      if (y1 > *year_max) {
	*year_max = y1;
	if (*year_min == 2999) {
	  *year_min = *year_max;
	}
      }
    }

  }
}
/* End of GetYearMinMax() */


/*! \return an array of year numbers

syntax of \@YEAR

"2007"
"2005..2007"

*/
BOOL_T
AddYears(cxpCalendarPtr pCalendarArg)
{
  if (pCalendarArg != NULL && pCalendarArg->pdocCalendar != NULL) {
    GetYearArray(pCalendarArg);
    if (pCalendarArg->pmiYear) {
      int *piYear;
      
      for (piYear=pCalendarArg->pmiYear; *piYear != 0; piYear++) {
	if (dt_from_yd(*piYear,1) < dt_from_yd(CXP_CALENDAR_YEAR_MIN + 1, 0)) {
	  PrintFormatLog(1,"Ignore year '%i', value to low", *piYear);
	}
	else if (dt_end_of_year(*piYear,0) > dt_from_yd(CXP_CALENDAR_YEAR_MAX + 1, 0)) {
	  PrintFormatLog(1,"Ignore year '%i', value to high", *piYear);
	  break;
	}
	else if (AddTreeYear(pCalendarArg, *piYear)) {
	  /* OK */
	}
	else {
	  return FALSE;
	}
      }
    }
    else {
      /* add tree for current year if year array is empty */
      AddTreeYear(pCalendarArg, dt_year(GetToday()));
    }
  }
  return TRUE;
}
/* End of AddYears() */


/*! Find the according parent col element to pndArgInsert, check for redundancy or create a new one

 \param pndArgParent pointer to according date element in calendar
 \param pucArgIdCol pointer to ID string of the required col element
 \param pndArgInsert pointer to node to insert

 \return pointer to parent node
*/
xmlNodePtr
FindCalendarElementCol(xmlNodePtr pndArgParent, xmlChar *pucArgIdCol, xmlNodePtr pndArgInsert)
{
  xmlNodePtr pndResult = NULL;

  if (pndArgParent != NULL && pndArgInsert != NULL) {
    xmlNodePtr pndCol;
    xmlNodePtr pndHour;
    xmlNodePtr pndChild;
    xmlChar *pucHour;

#if 0
    if ((pndHour = domGetFirstChild(pndArgParent,NAME_CXP_HOUR))
	&& (pucHour = domGetPropValuePtr(pndArgInsert, BAD_CAST "hour"))) {
      /*  */
      for ( ; pndHour != NULL; pndHour = domGetNextNode(pndHour,NAME_CXP_HOUR)) {
	/* step trough all existing hour elements */
	if (xmlStrEqual(pucHour,domGetPropValuePtr(pndHour, BAD_CAST "nr"))) {
	  return FindCalendarElementCol(pndHour, pucArgIdCol, pndArgInsert);
	}
      }
    }
    else
#endif
      if (pucArgIdCol) {
      for (pndCol = domGetFirstChild(pndArgParent,NAME_COL);
	  pndCol != NULL && xmlStrcasecmp(pucArgIdCol,domGetPropValuePtr(pndCol,BAD_CAST "idref"));
	  pndCol = domGetNextNode(pndCol,NAME_COL)) {
	/* step trough all existing col elements */
      }
      if (pndCol == NULL) {
	/* create one before */
	pndResult = xmlNewChild(pndArgParent, NULL, NAME_COL, NULL);
	xmlSetProp(pndResult, BAD_CAST "idref", pucArgIdCol);
      }
      else {
	pndResult = pndCol;
      }
    }
    else {
      pndResult = pndArgParent;
    }

    /* check if there is a sibling with same content
     */
    if (pndResult) {
      for (pndChild = pndResult->children;
	pndChild;
	pndChild = pndChild->next) {
	if (domNodesAreEqual(pndChild,pndArgInsert)) {
	  PrintFormatLog(4,"Ignore redundant entry '%s'",(unsigned char*)pndArgInsert->name);
	  return NULL;
	}
      }
    }
  }
  return pndResult;
}
/* End of FindCalendarElementCol() */


/*! update calendar DOM with all registered calendar elements of 'pCalendarArg'
 \param pCalendarArg pointer to calendar
*/
void
CalendarUpdate(cxpCalendarPtr pCalendarArg)
{
  if (pCalendarArg != NULL && pCalendarArg->pdocCalendar != NULL) {
    /*! insert COL entities */
    ceElementPtr pceT;
    xmlChar mucAdress[128];

    for (pceT = pCalendarArg->pceFirst; pceT; pceT = pceT->pNext) {
      xmlNodePtr pndAdd;

      /* generate an id for origin node */
      xmlStrPrintf(mucAdress,sizeof(mucAdress),"%x",pceT->pndEntry);
      
      if ((pndAdd = pieGetSelfAncestorNodeList(pceT->pndEntry,mucAdress))) {
	xmlNodePtr pndCol;

	if (cxpCalendarIndex(pceT->dt0.dt) > 0
	  && pCalendarArg->mpndDay[cxpCalendarIndex(pceT->dt0.dt)] != NULL
	  && cxpCalendarIndex(pceT->dt1.dt) > 0
	  && pCalendarArg->mpndDay[cxpCalendarIndex(pceT->dt1.dt)] != NULL) {
	  /* an interval is specified */
	  dt_t dtI;

	  for (dtI = pceT->dt0.dt; dtI <= pceT->dt1.dt; dtI++) {
	    /* do iteration of this calendar interval */
	    xmlNodePtr pndAddCopy;

	    assert(pCalendarArg->mpndDay[cxpCalendarIndex(dtI)]);

	    if ((pndCol = FindCalendarElementCol(pCalendarArg->mpndDay[cxpCalendarIndex(dtI)], pceT->pucColId, pceT->pndEntry)) != NULL
	      && (pndAddCopy = xmlCopyNode(pndAdd, 1))) {
	      xmlAddChild(pndCol, pndAddCopy);
	    }

	  }
	  xmlFreeNode(pndAdd);
	}
	else if (cxpCalendarIndex(pceT->dt0.dt) > 0
	  && pCalendarArg->mpndDay[cxpCalendarIndex(pceT->dt0.dt)] != NULL) {
	  /* an anchor is specified */
	  dt_t dtI;

	  assert(pCalendarArg->mpndDay[cxpCalendarIndex(pceT->dt0.dt)]);

	  if ((pndCol = FindCalendarElementCol(pCalendarArg->mpndDay[cxpCalendarIndex(pceT->dt0.dt)], pceT->pucColId, pceT->pndEntry)) != NULL) {
	    xmlAddChild(pndCol, pndAdd);
	  }

	  //xmlFreeNode(pndAdd);
	}
	else {
	  xmlFreeNode(pndAdd);
	  //xmlFreeNode(pceT->pndEntry);
	}
      }
    }

    for (pceT = pCalendarArg->pceFirst; pceT; pceT = pceT->pNext) {
      /* add id to origin node */
      xmlStrPrintf(mucAdress,sizeof(mucAdress),"%x",pceT->pndEntry);

      xmlSetProp(pceT->pndEntry, BAD_CAST"id", mucAdress);
    }
  }
} /* end of CalendarUpdate() */


/*! set attribute today at calendar DOM of 'pCalendarArg'
*/
void
CalendarSetToday(cxpCalendarPtr pCalendarArg)
{
  if (pCalendarArg != NULL && pCalendarArg->pdocCalendar != NULL) {
    xmlNodePtr pndToday = NULL;
    
    if (pCalendarArg->eType == CXP_CALENDAR_YEAR) {      
      pndToday = GetCalendarNodeYear(xmlDocGetRootElement(pCalendarArg->pdocCalendar),dt_year(GetToday()));
    }
    else if (pCalendarArg->eType == CXP_CALENDAR_MONTH) {      
      pndToday = GetCalendarNodeMonth(xmlDocGetRootElement(pCalendarArg->pdocCalendar),dt_year(GetToday()),dt_month(GetToday()));
    }
    else if (pCalendarArg->eType == CXP_CALENDAR_WEEK) {      
      pndToday = GetCalendarNodeWeek(xmlDocGetRootElement(pCalendarArg->pdocCalendar),dt_year(GetToday()),dt_woy(GetToday()));
    }
    else {      
      pndToday = GetCalendarNodeDay(xmlDocGetRootElement(pCalendarArg->pdocCalendar),dt_year(GetToday()),dt_month(GetToday()),dt_dom(GetToday()));
    }
    
    if (pndToday) {
      xmlSetProp(pndToday, BAD_CAST "today", BAD_CAST "yes");
    }
  }
}
/* End of CalendarSetToday() */


/*! \return an array of year numbers

syntax of \@YEAR

"2007"
"2005..2007"

*/
void
GetYearArray(cxpCalendarPtr pCalendarArg)
{
  if (pCalendarArg != NULL && pCalendarArg->pdocCalendar != NULL) {
    int *result = NULL;
    xmlChar *pucYFirst;
    xmlChar *pucYear = domGetPropValuePtr(pCalendarArg->pndCalendarRoot,BAD_CAST "year");

    if (pucYear != NULL) {
      if ((pucYFirst = BAD_CAST xmlStrstr(pucYear, BAD_CAST "..")) != NULL) {
	/*
	string at pucYear contains a year range
	*/
	int y_0, yFirst;
	y_0 = atoi((const char *)pucYear);
	if (errno != ERANGE && y_0 > 1900) {
	  yFirst = atoi((const char *)(pucYFirst + 2));
	  if (errno != ERANGE) {
	    if (y_0 <= yFirst || (yFirst - y_0) < 100) {
	      int i;
	      result = (int *)xmlMemMalloc((size_t) (yFirst - y_0 + 2) * sizeof(int));
	      if (result) {
		for (i=0; y_0 + i <= yFirst; i++) {
		  result[i] = y_0 + i;
		}
		result[i] = 0;
	      }
	    }
	  }
	}
      }
      else if (xmlStrstr(pucYear,BAD_CAST " ") != NULL || xmlStrstr(pucYear,BAD_CAST ",") != NULL) {
	/*
	string at pucYear contains a list of years
	*/
	const int i_max = 10;
	int i;

	result = (int *)xmlMemMalloc((size_t) (i_max + 1) * sizeof(int));
	if (result) {
	  for (i=0; i<i_max && *pucYear != '\0'; i++) {
	    int y_0;

	    y_0 = (int)strtol((const char*)pucYear, (char**)&pucYear, 10);
	    if (errno != ERANGE && y_0 > 1900) {
	      result[i] = y_0;
	    }
	    if (*pucYear == ',') {
	      pucYear++;
	    }
	  }
	  result[i] = 0;
	}
      }
      else {
	/*
	string at pucYear contains a single year
	*/
	int y_0;
	y_0 = atoi((const char *)pucYear);
	if (errno != ERANGE && y_0 > 1900) {
	  result = (int *)xmlMemMalloc(2 * sizeof(int));
	  if (result) {
	    result[0] = y_0;
	    result[1] = 0;
	  }
	}
      }
    }

    if (result == NULL) {
      /*! detect the calendar years from content */
      int year_min = 2999;
      int year_max = 0;
      int year_delta = 0;
      int i;

      GetYearMinMax(pCalendarArg,&year_min,&year_max);

      if (year_min == 2999) {
	/* no defined year found */
	year_min = dt_year(GetToday());
      }

      if (year_max == 0) {
	/* no defined year found */
	year_max = dt_year(GetToday());
      }

      year_delta = year_max - year_min;
      if (year_delta < 1) {
	result = (int*)xmlMemMalloc((size_t)(2 * sizeof(int)));
	if (result) {
	  result[0] = year_min;
	  result[1] = 0;
	}
      }
      else if (year_delta < 20) {
	result = (int*)xmlMemMalloc((size_t)((year_delta + 2) * sizeof(int)));
	if (result) {
	  for (i=0; year_min+i <= year_max; i++) {
	    result[i] = year_min + i;
	  }
	  result[i] = 0;
	}
      }
      else {
      }
    }

    pCalendarArg->pmiYear = result;
  }
}
/* end of GetYearArray() */


/*! register all calendar related DOM attributes

\todo replace XPath implementation by recursive procedure
 */
BOOL_T
RegisterAndParseDateNodes(cxpCalendarPtr pCalendarArg, xmlChar *pucArg)
{
  BOOL_T fResult = FALSE;

#ifdef DEBUG
  PrintFormatLog(2,"RegisterAndParseDateNodes(pCalendarArg=%0x)",pCalendarArg);
#endif

  if (pCalendarArg != NULL && pCalendarArg->pdocCalendar != NULL) {
      int i;
      xmlNodeSetPtr nodeset;
      xmlXPathObjectPtr result;
      ceElementPtr pceNew = NULL;

      //domPutDocString(stderr, BAD_CAST"Document", pCalendarArg->pdocCalendar);

      if (STR_IS_NOT_EMPTY(pucArg)) {
	result = domGetXPathNodeset(pCalendarArg->pdocCalendar, pucArg);
      }
      else {
	result = domGetXPathNodeset(pCalendarArg->pdocCalendar, BAD_CAST"/calendar/col//*[name() = 'date' or @date or @mtime2 or @MODIFIED]");
      }

      if (result) {
	for (nodeset = result->nodesetval, i=0; i < nodeset->nodeNr; i++) {
	  xmlChar *pucT = NULL;

	  if (IS_NODE_PIE_DATE(nodeset->nodeTab[i])) {
	    if (IS_NODE_PIE_TD(nodeset->nodeTab[i]->parent) || IS_NODE_PIE_TH(nodeset->nodeTab[i]->parent)) {
	      pucT = domNodeGetContentPtr(nodeset->nodeTab[i]);
	    }
	    else if ((pucT = domGetPropValuePtr(nodeset->nodeTab[i], BAD_CAST "ref"))) {
	      /* use attribute value */
	    }
	    else {
	      pucT = domNodeGetContentPtr(nodeset->nodeTab[i]);
	    }
	  }
#ifdef LEGACY
	  else if ((pucT = domGetPropValuePtr(nodeset->nodeTab[i], BAD_CAST "date"))) {
	  }
	  else if ((pucT = domGetPropValuePtr(nodeset->nodeTab[i], BAD_CAST "done"))) {
	  }
	  else if ((pucT = domGetPropValuePtr(nodeset->nodeTab[i], BAD_CAST "mtime2"))) {
	    /* dir listing */
	  }
	  else if ((pucT = domGetPropValuePtr(nodeset->nodeTab[i], BAD_CAST "MODIFIED"))) {
	    /* Freemind XML Format */
	  }
	  else {
	  }
#endif

	  if (STR_IS_NOT_EMPTY(pucT) && (pceNew = CalendarElementNew(pucT)) != NULL) {
	    xmlNodePtr pndCol;

	    if (IS_NODE_PIE_DATE(nodeset->nodeTab[i])
	      && (IS_NODE_PIE_TD(nodeset->nodeTab[i]->parent) || IS_NODE_PIE_TH(nodeset->nodeTab[i]->parent))
	      && IS_NODE_PIE_TR(nodeset->nodeTab[i]->parent->parent)) {
	      pceNew->pndEntry = nodeset->nodeTab[i]->parent->parent; /* use complete table row */
	    }
	    else {
	      pceNew->pndEntry = nodeset->nodeTab[i];
	    }

	    /*!\bug avoid redundant entries */
	    
	    /*! use ancestor col element for pucColId
	    */
	    for (pndCol = pceNew->pndEntry; pndCol; pndCol = pndCol->parent) {
	      if (IS_NODE_CXP_COL(pndCol)) {
		pceNew->pucColId = domGetPropValuePtr(pndCol, BAD_CAST "id");
		break;
	      }
	    }

	    if (ScanCalendarElementDate(pceNew)) {
	      ceElementPtr pceList;

	      if ((pceList = SplitCalendarElementRecurrences(pceNew))) {
		CalendarElementFree(pceNew);
		pceNew = pceList;
	      }

	      if (pCalendarArg->pceFirst) {
		CalendarElementListAdd(pCalendarArg->pceFirst, pceNew);
	      }
	      else {
		pCalendarArg->pceFirst = pceNew;
	      }
	    }
	    else {
	      CalendarElementFree(pceNew);
	    }
	  }
	}
	xmlXPathFreeObject(result);
	fResult = TRUE;
      }

#ifdef DEBUG
      //PrintCalendarSetup(pCalendarArg,NULL);
#endif
  }

  return fResult;
} /* end of RegisterAndParseDateNodes() */


#if defined(DEBUG) || defined(TESTCODE)
/*! process all column definition elements of /calendar/meta/calendar/ and insert result at /calendar/
 */
BOOL_T
PrintCalendarSetup(cxpCalendarPtr pCalendarArg, cxpContextPtr pccArg)
{
  BOOL_T fResult = TRUE;

  if (pCalendarArg) {
    ceElementPtr pceT;
    unsigned int i;

    PrintFormatLog(1, "Calendar");
    for (pceT = pCalendarArg->pceFirst, i=0; pceT != NULL; pceT = pceT->pNext, i++) {
      assert((pceT->patAttr != NULL && pceT->patAttr->parent != NULL) || (pceT->pndEntry != NULL && pceT->pndEntry->parent != NULL));
      PrintCalendarElement(pceT);
    }
  }
  return fResult;
} /* end of PrintCalendarSetup() */
#endif


/*! process all column definition elements of /calendar/meta/calendar/ and insert result at /calendar/
 */
BOOL_T
ProcessCalendarColumns(cxpCalendarPtr pCalendarArg, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;
  xmlNodePtr pndMeta;
  xmlNodePtr pndCalendarConfiguration;

#ifdef DEBUG
  PrintFormatLog(1, "ProcessCalendarColumns(pCalendarArg=%0x)", pCalendarArg);
#endif

  if (IS_NODE_CXP_CALENDAR(pCalendarArg->pndCalendarRoot)
    && (pndMeta = domGetFirstChild(pCalendarArg->pndCalendarRoot, NAME_META)) != NULL
    && (pndCalendarConfiguration = domGetFirstChild(pndMeta, NAME_CALENDAR)) != NULL) {
    xmlNodePtr pndCol;

    /*! preprocess COL entities */
    for (pndCol = domGetFirstChild(pndCalendarConfiguration, NAME_COL);
      pndCol;
      pndCol = domGetNextNode(pndCol, NAME_COL)) {
      xmlChar *pucIdCol;
      xmlChar *pucNameCol;
      xmlNodePtr pndColchild;
      xmlNodePtr pndColchildNext;
      xmlNodePtr pndColNew;

      pndColNew = xmlNewChild(pCalendarArg->pndCalendarRoot, NULL, NAME_COL, NULL);
      pucIdCol = domGetPropValuePtr(pndCol, BAD_CAST "id");
      xmlSetProp(pndColNew, BAD_CAST "id", pucIdCol);
      pucNameCol = domGetPropValuePtr(pndCol, BAD_CAST "name");
      xmlSetProp(pndColNew, BAD_CAST "name", pucNameCol);

      for (pndColchild = pndCol->children; pndColchild; pndColchild = pndColchildNext) {
	pndColchildNext = pndColchild->next;
	if (IS_NODE_XML(pndColchild)) {
	  xmlDocPtr pdocInput;

	  pdocInput = cxpProcessXmlNode(pndColchild, pccArg);
	  if (pdocInput) {
	    xmlNodePtr pndRoot = xmlDocGetRootElement(pdocInput);
	    if (pndRoot) {
	      domFreeNodeByName(pndRoot, NAME_META);
	      PrintFormatLog(2, "Insert XML DOM into calendar");
	      /*!\todo avoid copy of DOM, replace instead (respect the used namespaces) */
#if 0
	      xmlUnlinkNode(pndRoot);
	      xmlSetTreeDoc(pndRoot, pCalendarArg->pdocCalendar);
	      xmlAddChild(pndColNew, pndRoot);
#else
	      /*!\todo use domReplaceNodeList() */
	      xmlAddChild(pndColNew,xmlCopyNode(pndRoot,1));
#endif
	    }
	    else {
	      PrintFormatLog(1, "This DOM is empty");
	    }
	    xmlFreeDoc(pdocInput);
	  }
	}
      }
    }
    fResult = TRUE;
  }
  else {
    PrintFormatLog(1, "Wrong structure");
  }
  return fResult;
} /* end of ProcessCalendarColumns() */


/*! free all calendar related DOM attributes
 */
void
CalendarFree(cxpCalendarPtr pCalendarArg)
{
  if (pCalendarArg) {
    ceElementPtr pceT;

#ifdef DEBUG
    PrintFormatLog(1,"CalendarFree(pCalendarArg=%0x)",pCalendarArg);
#endif

    CalendarElementFree(pCalendarArg->pceFirst);
    xmlMemFree(pCalendarArg->pmiYear);
    xmlFreeDoc(pCalendarArg->pdocCalendar);
  
#ifdef DEBUG
    memset(pCalendarArg->mpndDay, 0, CXP_CALENDAR_SIZE);
    memset(pCalendarArg,0,sizeof(cxpCalendar));
#endif

    xmlFree(pCalendarArg);
  }
} /* end of CalendarFree() */


/*! free all calendar column content
 */
void
CalendarColumnsFree(cxpCalendarPtr pCalendarArg)
{
  if (pCalendarArg != NULL && pCalendarArg->pndCalendarRoot != NULL) {
    xmlNodePtr pndCol;

    pndCol = domGetFirstChild(pCalendarArg->pndCalendarRoot,NAME_COL);
    while (pndCol) {
      xmlNodePtr pndNext = domGetNextNode(pndCol,NAME_COL);

      xmlUnlinkNode(pndCol);
      xmlFreeNode(pndCol);
      pndCol = pndNext;
    }
  }
}
/* end of CalendarColumnsFree() */


/*!\return a new allocated calendar
 */
cxpCalendarPtr
CalendarNew(void)
{
  cxpCalendarPtr pCalendarResult = NULL;
  
  pCalendarResult = (cxpCalendarPtr) xmlMalloc(sizeof(cxpCalendar));
  if (pCalendarResult) {
    memset(pCalendarResult,0,sizeof(cxpCalendar));
    memset(pCalendarResult->mpndDay, 0, CXP_CALENDAR_SIZE);
    pCalendarResult->eType = CXP_CALENDAR_MDAY;
  }
  return pCalendarResult;
}
/* end of CalendarNew() */


/*!\return a new allocated calendar
 */
cxpCalendarPtr
CalendarSetup(xmlNodePtr pndArg, cxpContextPtr pccArg)
{
  cxpCalendarPtr pCalendarResult = NULL;

  pCalendarResult = CalendarNew();
  if (pCalendarResult) {
    /*! create DOM
     */
    xmlChar mpucT[BUFFER_LENGTH];
    xmlNodePtr pndCalendarCopy = NULL;
    xmlNodePtr pndMeta;
    xmlNodePtr pndT;
    xmlNodePtr pndTT;
    xmlNodePtr pndTimezone;
    xmlChar *pucAttrYear;
    xmlChar *pucAttrType;
    xmlChar *pucAttrCoordinate;
    xmlChar *pucAttrTimezone;

    pCalendarResult->pdocCalendar = xmlNewDoc(BAD_CAST "1.0");
    if (IS_NODE_CALENDAR(pndArg) && pndArg->doc != NULL && STR_IS_NOT_EMPTY(pndArg->doc->URL)) {
      pCalendarResult->pdocCalendar->URL = xmlStrdup(pndArg->doc->URL);
    }
    pCalendarResult->pndCalendarRoot = xmlNewDocNode(pCalendarResult->pdocCalendar, NULL, NAME_CXP_CALENDAR, NULL);
    xmlDocSetRootElement(pCalendarResult->pdocCalendar, pCalendarResult->pndCalendarRoot);

    if (IS_NODE_CALENDAR(pndArg) && (pucAttrYear = domGetPropValuePtr(pndArg, BAD_CAST "year"))) {
      xmlSetProp(pCalendarResult->pndCalendarRoot, BAD_CAST "year", pucAttrYear);
    }

    if (IS_NODE_CALENDAR(pndArg) && (pucAttrType = domGetPropValuePtr(pndArg, BAD_CAST "type"))) {
      
      if (xmlStrEqual(pucAttrType, BAD_CAST"week")) {
	pCalendarResult->eType = CXP_CALENDAR_WEEK;
      }
      else if (xmlStrEqual(pucAttrType, BAD_CAST"wday")) {
	pCalendarResult->eType = CXP_CALENDAR_WDAY;
      }
      else if (xmlStrEqual(pucAttrType, BAD_CAST"month")) {
	pCalendarResult->eType = CXP_CALENDAR_MONTH;
      }
      else if (xmlStrEqual(pucAttrType, BAD_CAST"mday")) {
	pCalendarResult->eType = CXP_CALENDAR_MDAY;
      }
#if 0
      else if (xmlStrEqual(pucAttrType, BAD_CAST"hour")) {
	pCalendarResult->eType = CXP_CALENDAR_MDAY_HOUR;
      }
#endif
      else if (xmlStrEqual(pucAttrType, BAD_CAST"year")) {
	pCalendarResult->eType = CXP_CALENDAR_YEAR;
      }
      else if (xmlStrEqual(pucAttrType, BAD_CAST"yday")) {
	pCalendarResult->eType = CXP_CALENDAR_YDAY;
      }
    }

    pCalendarResult->fCoordinate = FALSE;
    if ((pucAttrCoordinate = domGetPropValuePtr(pndArg, BAD_CAST "coordinate"))
      || (pucAttrCoordinate = cxpCtxtEnvGetValueByName(pccArg, BAD_CAST "CXP_COORDINATE"))) {
      xmlSetProp(pCalendarResult->pndCalendarRoot, BAD_CAST "coordinate", pucAttrCoordinate);
      if (GetPositionISO6709((const char*)pucAttrCoordinate, &(pCalendarResult->dLatitude), &(pCalendarResult->dLongitude))) {
	/* error */
      }
      else {
	PrintFormatLog(2, "Calendar Coordinate %f/%f", pCalendarResult->dLatitude, pCalendarResult->dLongitude);
	pCalendarResult->fCoordinate = TRUE;
      }
    }

    pndMeta = xmlNewChild(pCalendarResult->pndCalendarRoot, NULL, NAME_META, NULL);
    cxpInfoProgram(pndMeta, pccArg);
    if (IS_NODE_CALENDAR(pndArg)) {
      pndCalendarCopy = xmlCopyNode(pndArg, 1);
      //pieValidateTree(pndCalendarCopy);
    }
    else if ((pndCalendarCopy = xmlNewNode(NULL, NAME_CXP_CALENDAR)) == NULL
	     || xmlSetProp(pndCalendarCopy, BAD_CAST"subst", BAD_CAST "no") == NULL) {
      PrintFormatLog(1, "Cannot copy to new calendar");
    }
    else if ((pndT = xmlNewChild(pndCalendarCopy, NULL, NAME_CXP_COL, NULL)) == NULL
      || xmlSetProp(pndT, BAD_CAST"id", BAD_CAST "content") == NULL
      || xmlSetProp(pndT, BAD_CAST"name", BAD_CAST"Content") == NULL) {
      PrintFormatLog(1, "Cannot create new calendar");
    }
    else if ((pndTT = xmlCopyNode(pndArg, 1)) != NULL) {
      //pieValidateTree(pndTT);
      xmlAddChild(pndT, pndTT);
    }
    xmlAddChild(pndMeta, pndCalendarCopy);

    /* Get the current time. */
    domSetPropEat(pndMeta, BAD_CAST "ctime", GetNowFormatStr(BAD_CAST "%s"));
    domSetPropEat(pndMeta, BAD_CAST "ctime2", GetDateIsoString(0));
#if 0
    /* output timezone of XML */
    if ((pucAttrTimezone = domGetPropValuePtr(pndArg, BAD_CAST "timezone"))) {
      pCalendarResult->iTimezone = tzGetNumber(pucAttrTimezone);
      pCalendarResult->iTimezoneOffset = tzGetOffset(pCalendarResult->iTimezone);
    }
    pndTimezone = xmlNewChild(pndMeta, NULL, BAD_CAST"timezone", tzGetName(pCalendarResult->iTimezone));
    xmlStrPrintf(mpucT, BUFFER_LENGTH, "%+.2f", (pCalendarResult->iTimezoneOffset / 60.0f));
    xmlSetProp(pndTimezone, BAD_CAST "offset", mpucT);
    xmlSetProp(pndTimezone, BAD_CAST "id", tzGetId(pCalendarResult->iTimezone));
    PrintFormatLog(2, "Calendar timezone '%s' = %sh", tzGetName(pCalendarResult->iTimezone), domGetPropValuePtr(pndTimezone, BAD_CAST "offset"));
#endif
  }
  return pCalendarResult;
} /* end of CalendarSetup() */


/*! process the required calendar files
 */
xmlDocPtr
calProcessCalendarNode(xmlNodePtr pndArg, cxpContextPtr pccArg)
{
  xmlDocPtr pdocResult = NULL;

  if (IS_NODE_CALENDAR(pndArg)) {
    cxpCalendarPtr pCalendarResult;

    if ((pCalendarResult = CalendarSetup(pndArg, pccArg)) != NULL) {

      if (ProcessCalendarColumns(pCalendarResult, pccArg)
	  && RegisterAndParseDateNodes(pCalendarResult, NULL)
	  && AddYears(pCalendarResult)) {
      
	CalendarUpdate(pCalendarResult);
	CalendarSetToday(pCalendarResult);
	if (domGetPropFlag(pndArg, BAD_CAST"subst", FALSE)) {
	  /* do time-consuming format substitution by explicit demand only */
	  SubstituteFormat(pCalendarResult->pndCalendarRoot);
	}
	if (domGetPropFlag(pndArg, BAD_CAST"columns", TRUE) == FALSE) {
	  CalendarColumnsFree(pCalendarResult);
	}
      }
      else if (AddYears(pCalendarResult)) {
	CalendarUpdate(pCalendarResult);
	CalendarSetToday(pCalendarResult);
      }
      
      pdocResult = pCalendarResult->pdocCalendar;
      pCalendarResult->pdocCalendar = NULL;
      CalendarFree(pCalendarResult);
    }
  }
  return pdocResult;
} /* end of calProcessCalendarNode() */


/*! process the required calendar files
\deprecated ???
 */
xmlDocPtr
calProcessDoc(xmlDocPtr pdocArg, cxpContextPtr pccArg)
{
  xmlDocPtr pdocResult = NULL;
  cxpCalendarPtr pCalendarResult = NULL;

  if ((pCalendarResult = CalendarSetup(xmlDocGetRootElement(pdocArg), pccArg)) != NULL
    && ProcessCalendarColumns(pCalendarResult, pccArg)
    && RegisterAndParseDateNodes(pCalendarResult, NULL)
    && AddYears(pCalendarResult)) {
    CalendarUpdate(pCalendarResult);
    CalendarSetToday(pCalendarResult);
    SubstituteFormat(pCalendarResult->pndCalendarRoot);
#if 0
    if (domGetPropFlag(pndMakeCalendar, BAD_CAST"columns", FALSE) == FALSE) {
      CalendarColumnsFree(pCalendarResult);
    }
#endif
    pdocResult = pCalendarResult->pdocCalendar;
    pCalendarResult->pdocCalendar = NULL;
  }
  CalendarFree(pCalendarResult);
  
  return pdocResult;
} /* end of calProcessDoc() */


/*! \return 

\param 
*/
xmlNodePtr
GetCalendarNodeYear(xmlNodePtr pndArg, unsigned int uiArgYear)
{
  xmlNodePtr pndYear;
  xmlChar mpucCompare[BUFFER_LENGTH];

  xmlStrPrintf(mpucCompare,BUFFER_LENGTH, "%02i",uiArgYear);

  for (pndYear = domGetFirstChild(pndArg,NAME_CXP_YEAR); pndYear; pndYear = domGetNextNode(pndYear,NAME_CXP_YEAR)) {
    if (xmlStrcasecmp(mpucCompare,domGetPropValuePtr(pndYear,BAD_CAST "ad"))==0) {
      return pndYear;
    }
  }
  return NULL;
}
/* end of GetCalendarNodeYear() */


/*! \return 

\param 
*/
xmlNodePtr
GetCalendarNodeMonth(xmlNodePtr pndArg, unsigned int uiArgYear, unsigned int uiArgMonth)
{
  xmlNodePtr pndYear;

  pndYear = GetCalendarNodeYear(pndArg, uiArgYear);
  if (pndYear) {
    xmlNodePtr pndMonth;
    xmlChar mpucCompare[BUFFER_LENGTH];

    xmlStrPrintf(mpucCompare,BUFFER_LENGTH, "%02i",uiArgMonth);

    for (pndMonth = domGetFirstChild(pndYear,NAME_CXP_MONTH); pndMonth; pndMonth = domGetNextNode(pndMonth,NAME_CXP_MONTH)) {
      if (xmlStrEqual(mpucCompare,domGetPropValuePtr(pndMonth,BAD_CAST "nr"))) {
          return pndMonth;
      }
    }
  }
  return NULL;
}
/* end of GetCalendarNodeMonth() */


/*! \return 

\param 
*/
xmlNodePtr
GetCalendarNodeWeek(xmlNodePtr pndArg, unsigned int uiArgYear, unsigned int uiArgWeek)
{
  xmlNodePtr pndYear;

  pndYear = GetCalendarNodeYear(pndArg, uiArgYear);
  if (pndYear) {
    xmlNodePtr pndWeek;
    xmlChar mpucCompare[BUFFER_LENGTH];

    xmlStrPrintf(mpucCompare,BUFFER_LENGTH, "%02i",uiArgWeek);

    for (pndWeek = domGetFirstChild(pndYear,BAD_CAST "week"); pndWeek; pndWeek = domGetNextNode(pndWeek,BAD_CAST "week")) {
      if (xmlStrEqual(mpucCompare,domGetPropValuePtr(pndWeek,BAD_CAST "nr"))) {
	return pndWeek;
      }
    }
  }
  return NULL;
}
/* end of GetCalendarNodeWeek() */


/*! \return 

\param 
*/
xmlNodePtr
GetCalendarNodeDay(xmlNodePtr pndArg, unsigned int uiArgYear, unsigned int uiArgMonth, unsigned int uiArgDay)
{
  xmlNodePtr pndMonth;

  pndMonth = GetCalendarNodeMonth(pndArg, uiArgYear, uiArgMonth);
  if (pndMonth) {
    xmlChar mpucCompare[BUFFER_LENGTH];
    xmlNodePtr pndDay;

    xmlStrPrintf(mpucCompare,BUFFER_LENGTH, "%02i",uiArgDay);

    for (pndDay = domGetFirstChild(pndMonth,NAME_CXP_DAY); pndDay; pndDay = domGetNextNode(pndDay,NAME_CXP_DAY)) {
      if (xmlStrEqual(mpucCompare,domGetPropValuePtr(pndDay,BAD_CAST "om"))) {
	return pndDay;
      }
    }
  }
  return NULL;
}
/* end of GetCalendarNodeDay() */


/*! \return 

\param 
*/
xmlNodePtr
GetCalendarNodeDayHour(xmlNodePtr pndArg, unsigned int uiArgYear, unsigned int uiArgMonth, unsigned int uiArgDay, unsigned int uiArgHour)
{
  xmlNodePtr pndDay;

  pndDay = GetCalendarNodeDay(pndArg, uiArgYear, uiArgMonth, uiArgDay);
  if (pndDay) {
    xmlChar mpucCompare[BUFFER_LENGTH];
    xmlNodePtr pndHour;

    xmlStrPrintf(mpucCompare,BUFFER_LENGTH, "%02i",uiArgHour);

    for (pndHour = domGetFirstChild(pndDay,NAME_CXP_DAY); pndHour; pndHour = domGetNextNode(pndHour,NAME_CXP_HOUR)) {
      if (xmlStrEqual(mpucCompare,domGetPropValuePtr(pndHour,BAD_CAST "om"))) {
	return pndHour;
      }
    }
  }
  return NULL;
}
/* end of GetCalendarNodeDayHour() */


/*! \return a new allocated substituted string for 'fmt', 
    placeholders replaced by according attribute values of pndContext.

\param pndContext the col parent node
\param fmt
*/
xmlChar *
SubstituteFormatStr(xmlNodePtr pndContext, xmlChar *fmt)
{
  xmlChar *puc0;
  xmlChar *puc1;
  xmlChar *pucResult = NULL;
  xmlNodePtr pndYear = NULL;
  xmlNodePtr pndMonth = NULL;
  xmlNodePtr pndWeek = NULL;
  xmlNodePtr pndDay = NULL;

  if (fmt==NULL || pndContext==NULL) {
    return NULL;
  }

  for (puc0=fmt; *puc0=='\n' || *puc0=='\r'; puc0++) {}
  if (*puc0 == '\0') {
    /* ignore empty string */
    return NULL;
  }

  for (puc1=puc0, pucResult=NULL; ; puc1++) {

    if (puc1[0] == '%' && (puc1[1] == 'Y' || puc1[1] == 'm' || puc1[1] == 'd' || puc1[1] == 'a' || puc1[1] == 'j' || puc1[1] == 'M' || puc1[1] == 'V'
			   || puc1[1] == 'S' || puc1[1] == 'B' || puc1[1] == 'D')) {
      /* maybe a format char? */
      xmlChar *pucValue = NULL;

      if (pndYear == NULL) {
	/* context nodes not yet initialized */
	pndDay   = NULL;
	if (IS_NODE_CXP_YEAR(pndContext->parent)) {
	  /* node for whole year */
	  pndWeek  = NULL;
	  pndMonth = NULL;
	  pndYear  = pndContext->parent;
	}
	else if (IS_NODE_CXP_MONTH(pndContext->parent)) {
	  pndWeek  = NULL;
	  pndMonth = pndContext->parent;
	  pndYear  = pndContext->parent->parent;
	}
	else if (IS_NODE_CXP_WEEK(pndContext->parent)) {
	  pndWeek  = pndContext->parent;
	  pndMonth = NULL;
	  pndYear  = pndContext->parent->parent;
	}
	else if (IS_NODE_CXP_DAY(pndContext->parent)) {
	  pndDay   = pndContext->parent;
	  if (IS_NODE_CXP_WEEK(pndContext->parent->parent)) {
	    pndWeek  = pndContext->parent->parent;
	    pndMonth = NULL;
	    pndYear  = pndContext->parent->parent->parent;
	  }
	  else if (IS_NODE_CXP_MONTH(pndContext->parent->parent)) {
	    pndWeek  = NULL;
	    pndMonth = pndContext->parent->parent;
	    pndYear  = pndContext->parent->parent->parent;
	  }
	  else {
	    pndWeek  = NULL;
	    pndMonth = NULL;
	    pndYear  = pndContext->parent->parent;
	  }
	}
	else {
	}
      }

      if (puc1[1] == 'Y' || puc1[1] == 'm' || puc1[1] == 'd' || puc1[1] == 'a' || puc1[1] == 'j' || puc1[1] == 'V') {
	/* one char formats */
	if (puc1[1] == 'Y') { /* year */
	  pucValue = domGetPropValuePtr(pndYear,BAD_CAST "ad");
	}
	else if (puc1[1] == 'm') { /* month of year */
	  pucValue = domGetPropValuePtr(pndDay,BAD_CAST "mon");
	}
	else if (puc1[1] == 'd') { /* day of month */
	  pucValue = domGetPropValuePtr(pndDay,BAD_CAST "om");
	}
	else if (puc1[1] == 'a') { /* day of week */
	  pucValue = domGetPropValuePtr(pndDay,BAD_CAST "own");
	}
	else if (puc1[1] == 'j') { /* day of year */
	  pucValue = domGetPropValuePtr(pndDay,BAD_CAST "oy");
	}
	else if (puc1[1] == 'V') { /* ISO week of year */
	  pucValue = domGetPropValuePtr(pndDay,BAD_CAST "cw");
	}

	if (pucValue) {
	  if (puc1 - puc0 > 0) {
	    pucResult = xmlStrncat(pucResult, puc0, (int)(puc1 - puc0));
	  }

	  if (pucResult) {
	    pucResult = xmlStrcat(pucResult,pucValue);
	  }
	  else {
	    pucResult = xmlStrdup(pucValue);
	  }

	  puc1++;
	  puc0 = puc1 + 1;
	}
      }
      else if (puc1[1] == 'M') {
	/* multiple char formats */
	if (puc1[2] == 'O' && puc1[3] == 'O' && puc1[4] == 'N') {
	  if (puc1 - puc0 > 0) {
	    pucResult = xmlStrncat(pucResult,puc0,puc1 - puc0);
	  }

	  pucValue = domGetPropValuePtr(pndDay,BAD_CAST "moon");
	  if (pucValue) {
	    xmlChar *pucMoon = GetUTF8Bytes(0x274D);
	    if (pucResult) {
	      pucResult = xmlStrcat(pucResult,pucMoon);
	      xmlFree(pucMoon);
	    }
	    else {
	      pucResult = xmlStrdup(pucMoon);
	    }
	  }
	  puc1 += 4;
	  puc0 = puc1 + 1;
	}
      }
#ifdef LEGACY
      else if (puc1[1] == 'S') { /* sun */
	if (puc1[2] == 'R') {
	  pucValue = domGetPropValuePtr(pndDay,BAD_CAST "sunrise");
	}
	else if (puc1[2] == 'S') {
	  pucValue = domGetPropValuePtr(pndDay,BAD_CAST "sunset");
	}

	if (pucValue) {
	  if (puc1 - puc0 > 0) {
	    pucResult = xmlStrncat(pucResult,puc0,puc1 - puc0);
	  }

	  if (pucResult) {
	    pucResult = xmlStrcat(pucResult,pucValue);
	  }
	  else {
	    pucResult = xmlStrdup(pucValue);
	  }

	  puc1 += 2;
	  puc0 = puc1 + 1;
	}
      }
#endif
      else if (puc1[1] == 'B') {
	if (isdigit(puc1[2]) && isdigit(puc1[3]) && isdigit(puc1[4]) && isdigit(puc1[5]) && ! isdigit(puc1[6])) {
	  /* difference in years */
	  if (puc1 - puc0 > 0) {
	    pucResult = xmlStrncat(pucResult, puc0, (int)(puc1 - puc0));
	  }

	  pucValue = GetDiffYearsStrNew(domGetPropValuePtr(pndYear,BAD_CAST "ad"),&puc1[2]);
	  if (pucResult) {
	    pucResult = xmlStrcat(pucResult,pucValue);
	    xmlFree(pucValue);
	  }
	  else {
	    pucResult = pucValue;
	  }
	  puc1 += 6;
	  for (puc0 = puc1; *puc0 == ' '; puc0++) ; /* trailing spaces */
	  if (*puc0 == '.') {
	  }
	  else {
	    puc0 = puc1;
	  }
	}
      }
      else if (puc1[1] == 'D') {
	if (isdigit(puc1[2]) && isdigit(puc1[3]) && isdigit(puc1[4]) && isdigit(puc1[5])
	  && isdigit(puc1[6]) && isdigit(puc1[7]) && isdigit(puc1[8]) && isdigit(puc1[9]) && isspace(puc1[10])) {
	  /* difference in days */
	  if (puc1 - puc0 > 0) {
	    pucResult = xmlStrncat(pucResult, puc0, (int)(puc1 - puc0));
	  }

	  pucValue = GetDiffDaysStrNew(domGetPropValuePtr(pndDay,BAD_CAST "abs"),&puc1[2]);
	  if (pucResult) {
	    pucResult = xmlStrcat(pucResult,pucValue);
	    xmlFree(pucValue);
	  }
	  else {
	    pucResult = pucValue;
	  }
	  puc1 += 9;
	  puc0 = puc1 + 1;
	}
      }
      /*!\todo substitute '%N' */
#if 0
      /* non-@free day of year */
      pucT = pucResult;
      if ((pucFormating = BAD_CAST xmlStrstr(pucT,BAD_CAST "%N"))) {
	/* search for all preceeding siblings in same COL in same
	YEAR
	*/
	int iDayCount = 0;
	xmlNodePtr pndTDay;
	xmlNodePtr pndCurrent;
	xmlNodePtr pndPi;
	xmlChar *pucFree;
	xmlChar *pucHoliday;
	xmlChar *pucColId;

	pucColId = domGetPropValuePtr(pndContext,BAD_CAST "name");

	if (pndDay && pndYear) {
	  for (pndTDay = domGetFirstChild(domGetFirstChild(pndYear,NAME_CXP_MONTH),NAME_CXP_DAY);
	    pndTDay;
	    pndTDay = GetCalendarNodeNext(pndTDay,+1)) {
	    /* step over all day nodes */
	    /*!\todo set pndArg for every col separately, perfomance? */
	    for (pndCurrent = domGetFirstChild(pndTDay,NAME_COL);
	      pndCurrent;
	      pndCurrent = pndCurrent->next) {
	      /* step over all col nodes */
	      for (pndPi = domGetFirstChild(pndCurrent,NAME_CXP_PAR);
		pndPi;
		pndPi = pndPi->next) {
		/* step over all p nodes */
		if (((pucFree = domGetPropValuePtr(pndPi,BAD_CAST "free"))
		  && !xmlStrcasecmp(pucFree,BAD_CAST "yes")
		  && !xmlStrcasecmp(domGetPropValuePtr(pndCurrent,BAD_CAST "name"),pucColId))
		  ||
		  ((pucHoliday = domGetPropValuePtr(pndPi,BAD_CAST "holiday"))
		  && !xmlStrcasecmp(pucHoliday,BAD_CAST "yes"))) {
		  /* a p[@free='yes'] in a col[@id=pucColId] found */
		  /* or */
		  /* a p[@holiday='yes'] found */
		  /* dont count this day */
		  goto skip_day;
		}
	      }
	    }
	    iDayCount++;
	  skip_day:
	    if (pndTDay == pndDay) {
	      /* day node itself found */
	      break;
	    }
	  }
	}

	if (iDayCount > 0) {
	  xmlStrPrintf(mpucT,BUFFER_LENGTH, "%i",iDayCount);
	  pucResult = ReplaceStr(pucT,BAD_CAST "%N",mpucT);
	}
	xmlFree(pucT);
      }
#endif
    }
    else if (puc1[0] == '\0') {
      /* end of string */
      if (pucResult) {
	pucResult = xmlStrcat(pucResult,puc0);
      }
      else {
	pucResult = xmlStrdup(puc0);
      }
      break;
    }
  }
  return pucResult;
}
/* end of SubstituteFormatStr() */


/*! Substitutes the format strings in tree of pndArg.

\param pndArg parent node
\return TRUE
*/
BOOL_T
SubstituteFormat(xmlNodePtr pndArg)
{
  xmlNodePtr pndCol;
  xmlNodePtr pndPar;
  xmlNodePtr pndPchild;

  if (pndArg==NULL || pndArg->type!=XML_ELEMENT_NODE) {
    return FALSE;
  }
  else if (IS_NODE_META(pndArg)) {
    return FALSE;
  }
  else if (IS_NODE_CXP_COL(pndArg)) {
    /*
    all COL children nodes
    */

    if (IS_NODE_CXP_CALENDAR(pndArg->parent)) {
      /* this is the calendar definition */
      return FALSE;
    }

    pndCol = pndArg;

    for (pndPar = pndCol->children; pndPar; pndPar = pndPar->next) {
      if (IS_NODE_PIE_LINK(pndPar) || IS_NODE_PIE_PAR(pndPar)) {
	/* all P children nodes */
	for (pndPchild=pndPar->children; pndPchild; pndPchild=pndPchild->next) {
	  xmlChar *pucNew;
	  if (IS_NODE_PIE_LINK(pndPchild)) {
	    if (pndPchild->children!=NULL && pndPchild->children->type == XML_TEXT_NODE) {
	      /* substitute format in text node content of link node */
	      pucNew = SubstituteFormatStr(pndCol,domNodeGetContentPtr(pndPchild));
	      if (pucNew) {
 		PrintFormatLog(4,"Substituted '%s' to '%s'",domNodeGetContentPtr(pndPchild),pucNew);
		/*!\todo encode pucNew to UTF-8 */
		xmlNodeSetContent(pndPchild->children,pucNew);
		xmlFree(pucNew);
	      }
	    }
	  }
	  else if (pndPchild->type == XML_TEXT_NODE) {
	    pucNew = SubstituteFormatStr(pndCol,pndPchild->content);
	    if (pucNew) {
	      PrintFormatLog(4,"Substituted '%s' to '%s'",pndPchild->content,pucNew);
	      /*!\todo encode pucNew to UTF-8 */
	      xmlNodeSetContent(pndPchild,pucNew);
	      xmlFree(pucNew);
	    }
	  }
	}
      }
    }
  }
  else {
    /*!\todo other elements than p ? */
    for (pndPar = pndArg->children; pndPar; pndPar = pndPar->next) {
      SubstituteFormat(pndPar);
    }
  }
  return TRUE;
}
/* end of SubstituteFormat() */


/*! creates a DOM for a whole 'year'
*/
BOOL_T
AddTreeYear(cxpCalendarPtr pCalendarArg, int year)
{

#ifdef DEBUG
  PrintFormatLog(1,"AddTreeYear(pCalendarArg=%0x,year=%i)",pCalendarArg,year);
#endif

  if (pCalendarArg) {
    //int week_current;
    //struct tm t;
    //struct tm tFirstFirst;
    dt_t dt0, dt1, dti;
    xmlChar buffer[BUFFER_LENGTH];
    xmlChar bufferYear[BUFFER_LENGTH];
    xmlNodePtr pndYear;
    xmlNodePtr pndParent = pCalendarArg->pndCalendarRoot;
    //unsigned int iDayAbsolute;
    unsigned int iDayToday = GetToday();
    xmlNodePtr pndMonth = NULL;
    xmlNodePtr pndWeek = NULL;
    int iDaysDiff;
    int i;

    if (pCalendarArg->mpndDay[cxpCalendarIndex(0)] == NULL) {
    }

    domSetPropEat(pndParent, BAD_CAST"today", GetNowFormatStr(BAD_CAST"%Y%m%d"));

    pndYear = xmlNewChild(pndParent, NULL, NAME_CXP_YEAR, NULL);
    xmlStrPrintf( buffer,BUFFER_LENGTH, "%i", year);
    xmlSetProp(pndYear, BAD_CAST "ad", buffer);
    xmlStrPrintf( bufferYear,BUFFER_LENGTH, " %s ", buffer);

    for( dti = dt0 = dt_from_yd(year,1), dt1 = dt_add_years(dt0,1, DT_EXCESS); dti < dt1; dti++) {

      if (pCalendarArg->eType == CXP_CALENDAR_MDAY || pCalendarArg->eType == CXP_CALENDAR_MDAY_HOUR || pCalendarArg->eType == CXP_CALENDAR_MONTH) {
	if (dt_dom(dti) == 1) {
	  pndMonth = xmlNewChild(pndYear, NULL, NAME_CXP_MONTH, NULL);
	  xmlSetProp(pndMonth, BAD_CAST "nr", BAD_CAST mpucNumber[dt_month(dti)]);
	  xmlSetProp(pndMonth, BAD_CAST "name", moy[dt_month(dti)]);
	  //xmlAddChild(pndMonth, xmlNewComment(bufferYear));
	}
      }
      else if (pCalendarArg->eType == CXP_CALENDAR_WDAY || pCalendarArg->eType == CXP_CALENDAR_WDAY_HOUR || pCalendarArg->eType == CXP_CALENDAR_WEEK) {
	if (pndWeek == NULL || dt_dow(dti) == 1) { /* New week on Monday */
	  pndWeek = xmlNewChild(pndYear, NULL, NAME_CXP_WEEK, NULL);
	  xmlSetProp(pndWeek, BAD_CAST "nr", BAD_CAST mpucNumber[dt_woy(dti)]);
	}
      }

      if (pCalendarArg->eType == CXP_CALENDAR_YEAR) {
	pCalendarArg->mpndDay[cxpCalendarIndex(dti)] = pndYear;
      }
      else if (pCalendarArg->eType == CXP_CALENDAR_MONTH) {
	pCalendarArg->mpndDay[cxpCalendarIndex(dti)] = pndMonth;
      }
      else if (pCalendarArg->eType == CXP_CALENDAR_WEEK) {
	pCalendarArg->mpndDay[cxpCalendarIndex(dti)] = pndWeek;
      }
      else {
	xmlNodePtr pndDay = NULL;

	if (pCalendarArg->eType == CXP_CALENDAR_MDAY || pCalendarArg->eType == CXP_CALENDAR_MDAY_HOUR) {
	  pndDay = xmlNewChild(pndMonth, NULL, NAME_CXP_DAY, NULL);
	}
	else if (pCalendarArg->eType == CXP_CALENDAR_WDAY || pCalendarArg->eType == CXP_CALENDAR_WDAY_HOUR) {
	  pndDay = xmlNewChild(pndWeek, NULL, NAME_CXP_DAY, NULL);
	}
	else {
	  pndDay = xmlNewChild(pndYear, NULL, NAME_CXP_DAY, NULL);
	}
	  
	if (pCalendarArg->eType == CXP_CALENDAR_MDAY_HOUR || pCalendarArg->eType == CXP_CALENDAR_WDAY_HOUR) {
	  for (i = 0; i < 24; i++) {
	    xmlNodePtr pndHour;

	    pndHour = xmlNewChild(pndDay, NULL, NAME_CXP_HOUR, NULL);
	    xmlSetProp(pndHour, BAD_CAST "nr", BAD_CAST mpucNumber[i]);
	  }
	}

	xmlSetProp(pndDay, BAD_CAST "mon", BAD_CAST mpucNumber[dt_month(dti)]);

	xmlSetProp(pndDay, BAD_CAST "om", BAD_CAST mpucNumber[dt_dom(dti)]);

	xmlStrPrintf(buffer, BUFFER_LENGTH, "%i", dt_doy(dti));
	xmlSetProp(pndDay, BAD_CAST "oy", buffer);

	xmlStrPrintf(buffer, BUFFER_LENGTH, "%i", dt_dow(dti));
	xmlSetProp(pndDay, BAD_CAST "ow", buffer);
	/* attribute is used in calGetWeekNode() */
	xmlSetProp(pndDay, BAD_CAST "own", dow[dt_dow(dti)]);

	xmlSetProp(pndDay, BAD_CAST "cw", BAD_CAST mpucNumber[dt_woy(dti)]);

	xmlStrPrintf(buffer, BUFFER_LENGTH, "%i", dti);
	xmlSetProp(pndDay, BAD_CAST "abs", buffer);

#if 0
	if (pCalendarArg->fCoordinate) {
	  double dHourUTCSunrise;
	  double dHourUTCSunset;
	  int iMinute;

	  sun_rise_set(t.tm_year + 1900, dt_mon(dti) + 1, dt_dom(dti),
		       pCalendarArg->dLongitude, pCalendarArg->dLatitude,
		       &dHourUTCSunrise,
		       &dHourUTCSunset);

	  // Sunrise
	  dHourUTCSunrise += pCalendarArg->iTimezoneOffset / 60.0f;
	  if (t.tm_isdst) {
	    dHourUTCSunrise += 1.0f;
	  }
	  iMinute = RoundToInt(dHourUTCSunrise * 60.0);
	  xmlStrPrintf(buffer, BUFFER_LENGTH, "%i:%02i", iMinute / 60, iMinute % 60);
	  xmlSetProp(pndDay, BAD_CAST "sunrise", buffer);

	  // Sunset
	  dHourUTCSunset += pCalendarArg->iTimezoneOffset / 60.0f;
	  if (t.tm_isdst) {
	    dHourUTCSunset += 1.0f;
	  }
	  iMinute = RoundToInt(dHourUTCSunset * 60.0);
	  xmlStrPrintf(buffer, BUFFER_LENGTH, "%i:%02i", iMinute / 60, iMinute % 60);
	  xmlSetProp(pndDay, BAD_CAST "sunset", buffer);

	  // Moon
	  if (IsFullMoonConway(t.tm_year + 1900, dt_mon(dti) + 1, dt_dom(dti))) {
	    xmlSetProp(pndDay, BAD_CAST "moon", BAD_CAST"full");
	  }

	  xmlSetProp(pndDay, BAD_CAST "dst", BAD_CAST(t.tm_isdst ? "yes" : "no"));
	}
#endif

	/*
	  add the time difference from today in days
	*/
	pCalendarArg->mpndDay[cxpCalendarIndex(dti)] = pndDay;
	xmlStrPrintf(buffer, BUFFER_LENGTH, "%i", dti - iDayToday);
	xmlSetProp(pndDay, BAD_CAST "diffy", buffer);
      }
    }
    return TRUE;
  }
  return FALSE;
}
/* end of AddTreeYear() */


/*! This is based on a 'do it in your head' algorithm by John Conway. In its current form, it's only valid for the 20th and 21st centuries, but I'm sure John's got refinements. :)

http://www.faqs.org/faqs/astronomy/faq/part3/section-15.html
http://www.ben-daglish.net/moon.shtml

  \return 1 in case of error
 */
BOOL_T
IsFullMoonConway(int year, int month, int day)
{
  int r;
  double d;

  r = year % 100;
  r %= 19;
  if (r>9) {
    r -= 19;
  }
  r = ((r * 11) % 30) + month + day;
  if (month<3) {
    r += 2;
  }
  d =  r - ((year<2000) ? 4.0 : 8.3);

  r = (int)floor(d + 0.5) % 30;
  if (r < 0) {
    r += 30;
  }

  return (r == 15);
}
/* end of IsFullMoonConway() */


#ifdef TESTCODE
#include "test/test_cxp_calendar.c"
#endif
