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
    ceElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan invalid calendar element = ", i, __FILE__, __LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == TRUE) {
      printf("ERROR\n");
    }
    else if (CalendarElementUpdate(pceT,BAD_CAST"209") == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == TRUE) {
      printf("ERROR\n");
    }
    else if (CalendarElementUpdate(pceT,BAD_CAST"200917") == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == TRUE) {
      printf("ERROR\n");
    }
    else if (CalendarElementUpdate(pceT, BAD_CAST"200910119") == NULL) {
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
    ceElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"20091011")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dtBegin != dt_from_ymd(2009, 10, 11)) {
      printf("ERROR\n");
    }
    else if (pceT->dtEnd != 0) {
      printf("ERROR\n");
    }
    else if (CalendarElementUpdate(pceT,BAD_CAST"2019-01-11") == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dtBegin != dt_from_ymd(2019, 1, 11)) {
      printf("ERROR\n");
    }
    else if (pceT->dtEnd != 0) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }

  if (RUNTEST) {
    ceElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a system time calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"1311186519")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dtBegin != dt_from_ymd(2011, 7, 20) || pceT->dtEnd != 0) {
      printf("ERROR\n");
    }
    else if (CalendarElementUpdate(pceT,BAD_CAST"1660333489490") == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dtBegin != dt_from_ymd(2022, 8, 12) || pceT->dtEnd != 0) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }

  if (RUNTEST) {
    ceElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a week calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"2009-W3-3")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == TRUE) {
      printf("ERROR\n");
    }
    else if (CalendarElementUpdate(pceT, BAD_CAST"2009-W03-3") == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dtBegin != dt_from_ywd(2009, 3, 3) || pceT->dtEnd != 0) {
      printf("ERROR\n");
    }
    else if (CalendarElementUpdate(pceT, BAD_CAST"2009-W51-7") == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dtBegin != dt_from_ywd(2009, 51, 7) || pceT->dtEnd != 0) {
      printf("ERROR\n");
    }
    else if (CalendarElementUpdate(pceT, BAD_CAST"2007W033") == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dtBegin != dt_from_ywd(2007, 3, 3) || pceT->dtEnd != 0) {
      printf("ERROR\n");
    }
    else if (CalendarElementUpdate(pceT, BAD_CAST"2009-W55-8") == NULL) {
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
    ceElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a day calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"2009-")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == TRUE) {
      printf("ERROR\n");
    }
    else if (CalendarElementUpdate(pceT, BAD_CAST"2009-099") == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dtBegin != dt_from_yd(2009, 99) || pceT->dtEnd != 0) {
      printf("ERROR\n");
    }
    else if (CalendarElementUpdate(pceT, BAD_CAST"2009199") == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dtBegin != dt_from_yd(2009, 199) || pceT->dtEnd != 0) {
      printf("ERROR\n");
    }
    else if (CalendarElementUpdate(pceT, BAD_CAST"2009-399") == NULL) {
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
    ceElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a list calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"20140301,20140401")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dtBegin != dt_from_ymd(2014, 3, 1) || pceT->dtEnd != 0) {
      printf("ERROR\n");
    }
    else if (xmlStrEqual(pceT->pucSep, BAD_CAST",20140401") == FALSE) {
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
    ceElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ", i, __FILE__, __LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"2013-12-11T10:11:00+05:45 ISO")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (xmlStrEqual(pceT->pucSep, BAD_CAST" ISO") == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dtBegin != dt_from_ymd(2013, 12, 11) || pceT->dtEnd != 0) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }

  /*\todo test time strings */
  
  if (RUNTEST) {
    ceElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ", i, __FILE__, __LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"2013-12-11T10:11:00CST ISO")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (xmlStrEqual(pceT->pucSep, BAD_CAST" ISO") == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dtBegin != dt_from_ymd(2013, 12, 11) || pceT->dtEnd != 0) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }

  /* ISO 8601 Time intervals */

  if (RUNTEST) {
    ceElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': ", i, __FILE__, __LINE__);

    if ((pceT = CalendarElementNew(NULL)) == NULL) {
      printf("ERROR CalendarElementUpdate()\n");
    }
    else if (CalendarElementUpdate(pceT, BAD_CAST"20110703/20110711") == NULL) {
      printf("ERROR CalendarElementUpdate()\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dtBegin != dt_from_ymd(2011, 7, 3) || pceT->dtEnd != dt_from_ymd(2011, 7, 11)) {
      printf("ERROR\n");
    }
    else if (CalendarElementUpdate(pceT,BAD_CAST"2013-12-11/2014-03-11 ISO date interval") == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (xmlStrEqual(pceT->pucSep, BAD_CAST" ISO date interval") == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dtBegin != dt_from_ymd(2013, 12, 11) || pceT->dtEnd != dt_from_ymd(2014, 3, 11)) {
      printf("ERROR\n");
    }
    else if (CalendarElementUpdate(pceT, BAD_CAST"20110703/2011 AAA") == NULL) {
      printf("ERROR CalendarElementUpdate()\n");
    }
    else if (ScanCalendarElementDate(pceT) == TRUE) {
      printf("ERROR\n");
    }
    else if (CalendarElementUpdate(pceT, BAD_CAST"20110703/20110703") == NULL) {
      printf("ERROR CalendarElementUpdate()\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dtBegin != dt_from_ymd(2011, 7, 3) || pceT->dtEnd != 0) {
      printf("ERROR\n");
    }
    else if (CalendarElementUpdate(pceT, BAD_CAST"20110711/20110703") == NULL) {
      printf("ERROR CalendarElementUpdate()\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dtBegin != dt_from_ymd(2011, 7, 3) || pceT->dtEnd != dt_from_ymd(2011, 7, 11)) {
      printf("ERROR\n");
    }
    else if (CalendarElementUpdate(pceT, BAD_CAST"20110703/P3Y6M4D TEST") == NULL) {
      printf("ERROR CalendarElementUpdate()\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dtBegin != dt_from_ymd(2011, 7, 3) || pceT->dtEnd != dt_from_ymd(2015, 1, 7)) {
      printf("ERROR\n");
    }
    else if (CalendarElementUpdate(pceT, BAD_CAST"P3W/20110722 TEST") == NULL) {
      printf("ERROR CalendarElementUpdate()\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dtBegin != dt_from_ymd(2011, 7, 1) || pceT->dtEnd != dt_from_ymd(2011, 7, 22)) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }

  if (RUNTEST) {
    ceElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ", i, __FILE__, __LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"2013-10-12T14:00:00/2013-10-14T14:00:00 ISO date interval")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (xmlStrEqual(pceT->pucSep, BAD_CAST" ISO date interval") == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dtBegin != dt_from_ymd(2013, 10, 12) || pceT->dtEnd != dt_from_ymd(2013, 10, 14)) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }


  if (RUNTEST) {
    ceElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a year interval calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"2009")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dtBegin != dt_from_ymd(2009,1,1) || pceT->dtEnd != dt_from_ymd(2009, 12, 31)) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }

  if (RUNTEST) {
    ceElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a month interval calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"200903")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dtBegin != dt_from_ymd(2009, 3, 1) || pceT->dtEnd != dt_from_ymd(2009, 3, 31)) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }


  if (RUNTEST) {
    ceElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a week interval calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"2009-W33")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dtBegin != dt_from_ywd(2009, 33, 1) || pceT->dtEnd != dt_from_ywd(2009, 33, 7)) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }

  /* ISO 8601 Repeating intervals */

  if (RUNTEST) {
    ceElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': ", i, __FILE__, __LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"R-21/")) == NULL) {
      printf("ERROR CalendarElementNew()\n");
    }
    else if (ScanCalendarElementDate(pceT) == TRUE) {
      printf("ERROR\n");
    }
    else if (CalendarElementUpdate(pceT, BAD_CAST"R3/20110703/P3M2DT12H30M5S TEST") == NULL) {
      printf("ERROR CalendarElementUpdate()\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dtBegin != dt_from_ymd(2011, 7, 3) || pceT->dtEnd != 0 || pceT->iRecurrence != 3) {
      printf("ERROR\n");
    }
    else if (CalendarElementUpdate(pceT, BAD_CAST"R7/P3M2DT12H30M5S/20110703 TEST") == NULL) {
      printf("ERROR CalendarElementUpdate()\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dtBegin != 0 || pceT->dtEnd != dt_from_ymd(2011, 7, 3) || pceT->iRecurrence != 7) {
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
    else if (dt_year(GetToday()) != 2013 || dt_month(GetToday()) != 12 || dt_dom(GetToday()) != 11) {
      printf("ERROR GetToday*()\n");
    }
    else if (UpdateToday(BAD_CAST"2017") < 0) {
      printf("ERROR UpdateToday()\n");
    }
    else if (dt_year(GetToday()) != 2017 || dt_month(GetToday()) != 12 || dt_dom(GetToday()) != 11) {
      printf("ERROR GetToday*()\n");
    }
    else if (UpdateToday(BAD_CAST"201703") < 0) {
      printf("ERROR UpdateToday()\n");
    }
    else if (dt_year(GetToday()) != 2017 || dt_month(GetToday()) != 3 || dt_dom(GetToday()) != 11) {
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
/* end of ceTest() */
