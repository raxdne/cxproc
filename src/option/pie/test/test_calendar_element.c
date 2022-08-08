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


/*! 
 */
int
ceTest(void)
{
  xmlChar buffer[BUFFER_LENGTH];

  int n_ok;
  int i;

  n_ok=0;
  i=0;

  if (RUNTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"20091011.")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == TRUE) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }

  if (RUNTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"209")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == TRUE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_ERROR) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }

  if (RUNTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"2009")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_ISO) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2009 || pceT->iMonth != 1 || pceT->iDay != 1 || pceT->iStep != 1 || pceT->iCount != 364) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }

  if (RUNTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"200903")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_ISO) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2009 || pceT->iMonth != 3 || pceT->iDay != 1 || pceT->iStep != 1 || pceT->iCount != 30) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }


  if (RUNTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"200917")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == TRUE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_ERROR) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != -1 || pceT->iMonth != -1 || pceT->iDay != -1 || pceT->iWeek != -1 || pceT->iDayWeek !=-1) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }

  if (RUNTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"20091011")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_ISO) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2009 || pceT->iMonth != 10 || pceT->iDay != 11 || pceT->iWeek != 41 || pceT->iDayWeek != 7) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) ==  TRUE) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }

  if (RUNTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"200910119")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == TRUE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_ERROR) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != -1 || pceT->iMonth != -1 || pceT->iDay != -1 || pceT->iWeek != -1 || pceT->iDayWeek != -1) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }

  if (SKIPTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"1311186519")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_SYSTEM) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2011 || pceT->iMonth != 7 || pceT->iDay != 20 || pceT->iWeek != -1 || pceT->iDayWeek != 3) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }

  if (SKIPTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"1253131866347")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_SYSTEM_MSEC) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2009 || pceT->iMonth != 9 || pceT->iDay != 16 || pceT->iWeek != -1 || pceT->iDayWeek != 3) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }

  if (RUNTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"2009-10-11")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_ISO) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2009 || pceT->iMonth != 10 || pceT->iDay != 11 || pceT->iCount != -1) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }

  if (SKIPTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"11.10.2009")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_GERMAN) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2009 || pceT->iMonth != 10 || pceT->iDay != 11 || pceT->iWeek != -1 || pceT->iDayWeek != -1) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }

  if (SKIPTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"01.2.2009")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_GERMAN) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2009 || pceT->iMonth != 2 || pceT->iDay != 1 || pceT->iWeek != -1 || pceT->iDayWeek != -1) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }

  if (SKIPTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"1.03.2009")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_GERMAN) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2009 || pceT->iMonth != 3 || pceT->iDay != 1 || pceT->iWeek != -1 || pceT->iDayWeek != -1) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }

  if (SKIPTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"1.4.2009")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_GERMAN) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2009 || pceT->iMonth != 4 || pceT->iDay != 1 || pceT->iWeek != -1 || pceT->iDayWeek != -1) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }

  if (SKIPTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"30.2.2009")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == TRUE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_ERROR) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != -1 || pceT->iMonth != -1 || pceT->iDay != -1 || pceT->iWeek != -1 || pceT->iDayWeek != -1) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }

  if (SKIPTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"33.13.2009")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == TRUE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_ERROR) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != -1 || pceT->iMonth != -1 || pceT->iDay != -1 || pceT->iWeek != -1 || pceT->iDayWeek != -1) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }

  if (SKIPTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"2009*w33")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_WEEK) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2009 || pceT->iMonth != -1 || pceT->iDay != -1 || pceT->iWeek != 33 || pceT->iDayWeek != -1) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }

  if (RUNTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"2009-W33")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_ISO) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2009 || pceT->iWeek != 33 || pceT->iDayWeek != 1 || pceT->iCount != 6 || pceT->iStep != 1) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }

 
  if (SKIPTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"2009*w3wed")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_WEEK) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2009 || pceT->iMonth != -1 || pceT->iDay != -1 || pceT->iWeek != 3 || pceT->iDayWeek != 3) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }


  if (RUNTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"2009-W3-3")) == NULL) {
      printf("ERROR\n");
    }
    else if ((pceT = CalendarElementNew(BAD_CAST"2009-W03-3")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_ISO) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2009 || pceT->iWeek != 3 || pceT->iDayWeek != 3) {
      printf("ERROR\n");
    }
    else if ((pceT = CalendarElementUpdate(pceT, BAD_CAST"2009-W51-7")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_ISO) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2009 || pceT->iWeek != 51 || pceT->iDayWeek != 7) {
      printf("ERROR\n");
    }
    else if ((pceT = CalendarElementUpdate(pceT, BAD_CAST"2009-W55-8")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == TRUE) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != -1 || pceT->iWeek != -1 || pceT->iDayWeek != -1) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }

  
  if (SKIPTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"2009*w34tue")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_WEEK) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2009 || pceT->iMonth != -1 || pceT->iDay != -1 || pceT->iWeek != 34 || pceT->iDayWeek != 2) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }

 
  if (SKIPTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"2009*w55tue")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == TRUE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_ERROR) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != -1 || pceT->iMonth != -1 || pceT->iDay != -1 || pceT->iWeek != -1 || pceT->iDayWeek != -1) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }

  
  if (SKIPTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"0000-W99-2")) == NULL) {
      printf("ERROR: CalendarElementNew()\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR: ScanCalendarElementDate()\n");
    }
    else if (pceT->eTypeDate != DATE_WEEK_ISO) {
      printf("ERROR: eTypeDate\n");
    }
    else if (pceT->iYear != 0 || pceT->iMonth != -1 || pceT->iDay != -1 || pceT->iWeek != 99 || pceT->iDayWeek != 2 || pceT->iStep != -1) {
      //printf("%i %i %i %i %i %i \n", pceT->iYear, pceT->iMonth, pceT->iDay, pceT->iWeek, pceT->iDayWeek, pceT->iStep);
      printf("ERROR: value\n");
    }
    else if ((pceT->iYear = 2009) != 2009 || pceT->iMonth != -1 || pceT->iDay != -1 || pceT->iWeek != 99 || pceT->iDayWeek != 2 || pceT->iStep != 7) {
      //printf("%i %i %i %i %i %i \n", pceT->iYear, pceT->iMonth, pceT->iDay, pceT->iWeek, pceT->iDayWeek, pceT->iStep);
      printf("ERROR: value\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }

  
  if (RUNTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"20140301,20140401")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (xmlStrEqual(pceT->pucSep, BAD_CAST",20140401") == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_ISO) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2014 || pceT->iMonth != 3 || pceT->iDay != 1 || pceT->iWeek != 9 || pceT->iDayWeek != 6
      || pceT->iStep != -1 || pceT->iCount != -1) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2014 || pceT->iMonth != 4 || pceT->iDay != 1 || pceT->iWeek != 14 || pceT->iDayWeek != 2
      || pceT->iStep != -1 || pceT->iCount != -1) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }

  if (SKIPTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"2009@e")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_EASTER) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2009 || pceT->iMonth != 4 || pceT->iDay != 12 || pceT->iWeek != -1 || pceT->iDayWeek != -1
	|| pceT->iAnchor != 0 || pceT->iStep != -1 || pceT->iCount != -1) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == TRUE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_ERROR) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }

  if (SKIPTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"00000000")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_DAY) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 0 || pceT->iMonth != 0 || pceT->iDay != 0 || pceT->iWeek != -1 || pceT->iDayWeek != -1
	|| pceT->iAnchor != 0 || pceT->iStep != -1 || pceT->iCount != -1) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }


  if (RUNTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ", i, __FILE__, __LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"2013-12-11T10:11:00+05:45 ISO")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_ISO) {
      printf("ERROR\n");
    }
    else if (xmlStrEqual(pceT->pucSep, BAD_CAST" ISO") == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2013
      || pceT->iMonth != 12 || pceT->iDay != 11 || pceT->iWeek != -1 || pceT->iDayWeek != -1
      || pceT->iHourA != 10 || pceT->iMinuteA != 11 || pceT->iSecondA != 0
      || pceT->iHourB != -1 || pceT->iMinuteB != -1 || pceT->iSecondB != -1
      || pceT->iTimezoneOffset != 345) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }


  if (RUNTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ", i, __FILE__, __LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"2013-12-11T10:11:00CST ISO")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_ISO) {
      printf("ERROR\n");
    }
    else if (xmlStrEqual(pceT->pucSep, BAD_CAST" ISO") == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2013
      || pceT->iMonth != 12 || pceT->iDay != 11 || pceT->iWeek != -1 || pceT->iDayWeek != -1
      || pceT->iHourA != 10 || pceT->iMinuteA != 11 || pceT->iSecondA != 0
      || pceT->iHourB != -1 || pceT->iMinuteB != -1 || pceT->iSecondB != -1
      || pceT->iTimezoneOffset != -360) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }


  if (RUNTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ", i, __FILE__, __LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"2013-12-11/2014-03-11 ISO date range")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_ISO) {
      printf("ERROR\n");
    }
    else if (xmlStrEqual(pceT->pucSep, BAD_CAST" ISO date range") == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2013
      || pceT->iMonth != 12 || pceT->iDay != 11 || pceT->iWeek != -1 || pceT->iDayWeek != -1
      || pceT->iCount != 90 || pceT->iStep != 1) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }


  if (RUNTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ", i, __FILE__, __LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"2013-10-12T14:00:00/2013-10-14T14:00:00 ISO date range")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_ISO) {
      printf("ERROR\n");
    }
    else if (xmlStrEqual(pceT->pucSep, BAD_CAST" ISO date range") == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2013
      || pceT->iMonth != 12 || pceT->iDay != 11 || pceT->iWeek != -1 || pceT->iDayWeek != -1
      || pceT->iCount != 90 || pceT->iStep != 1) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }


  if (RUNTEST) {

    i++;
    printf("TEST %i in '%s:%i': update 'timeNow' calendar element = ", i, __FILE__, __LINE__);

    if (UpdateToday(NULL) < 0) {
      printf("ERROR UpdateToday()\n");
    }
    else if (UpdateToday(BAD_CAST"2013-12-11T10:11:00CST ISO") < 0) {
      printf("ERROR UpdateToday()\n");
    }
    else if (GetTodayYear() != 2013 || GetTodayMonth() != 12 || GetTodayDayOfMonth() != 11) {
      printf("ERROR GetToday*()\n");
    }
    else if (UpdateToday(BAD_CAST"2017") < 0) {
      printf("ERROR UpdateToday()\n");
    }
    else if (GetTodayYear() != 2017 || GetTodayMonth() != 12 || GetTodayDayOfMonth() != 11) {
      printf("ERROR GetToday*()\n");
    }
    else if (UpdateToday(BAD_CAST"201703") < 0) {
      printf("ERROR UpdateToday()\n");
    }
    else if (GetTodayYear() != 2017 || GetTodayMonth() != 3 || GetTodayDayOfMonth() != 11) {
      printf("ERROR GetToday*()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }


  printf("Result in '%s': %i/%i OK\n\n", __FILE__, n_ok, i);

  return (i - n_ok);
}
/* end of calTest() */
