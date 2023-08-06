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
    else if (pceT->dt0.dt != dt_from_ymd(2009, 10, 11)) {
      printf("ERROR\n");
    }
    else if (pceT->dt1.dt != 0) {
      printf("ERROR\n");
    }
    else if (CalendarElementUpdate(pceT,BAD_CAST"2019-01-11") == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dt0.dt != dt_from_ymd(2019, 1, 11)) {
      printf("ERROR\n");
    }
    else if (pceT->dt1.dt != 0) {
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
    else if (pceT->dt0.dt != dt_from_ymd(2011, 7, 20) || pceT->dt1.dt != 734338) {
      printf("ERROR\n");
    }
    else if (CalendarElementUpdate(pceT,BAD_CAST"1660333489490") == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dt0.dt != dt_from_ymd(2022, 8, 12) || pceT->dt1.dt != 738379) {
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
    else if (pceT->dt0.dt != dt_from_ywd(2009, 3, 3) || pceT->dt1.dt != 0) {
      printf("ERROR\n");
    }
    else if (CalendarElementUpdate(pceT, BAD_CAST"2009-W51-7") == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dt0.dt != dt_from_ywd(2009, 51, 7) || pceT->dt1.dt != 0) {
      printf("ERROR\n");
    }
    else if (CalendarElementUpdate(pceT, BAD_CAST"2007W033") == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dt0.dt != dt_from_ywd(2007, 3, 3) || pceT->dt1.dt != 0) {
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
    else if (pceT->dt0.dt != dt_from_yd(2009, 99) || pceT->dt1.dt != 0) {
      printf("ERROR\n");
    }
    else if (CalendarElementUpdate(pceT, BAD_CAST"2009199") == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dt0.dt != dt_from_yd(2009, 199) || pceT->dt1.dt != 0) {
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
    else if (pceT->dt0.dt != dt_from_ymd(2014, 3, 1) || pceT->dt1.dt != 0) {
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

    if ((pceT = CalendarElementNew(BAD_CAST"2013-12-11T10:00:00-09:00 Anchorage/Alaska")) == NULL) {
      printf("ERROR 1\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR 2\n");
    }
    else if (xmlStrEqual(pceT->pucSep, BAD_CAST" Anchorage/Alaska") == FALSE) {
      printf("ERROR 3\n");
    }
    else if (pceT->dt0.dt != dt_from_ymd(2013, 12, 11) || pceT->dt0.iSec != 68400 || pceT->dt1.dt != 0 || pceT->dt1.iSec != 0) {
      printf("ERROR 4\n");
    }
    else if (CalendarElementUpdate(pceT, BAD_CAST"2013-12-11T04:00:00+07:00 Bangkok/Thailand") == NULL) {
      printf("ERROR CalendarElementUpdate()\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR 2\n");
    }
    else if (xmlStrEqual(pceT->pucSep, BAD_CAST" Bangkok/Thailand") == FALSE) {
      printf("ERROR 3\n");
    }
    else if (pceT->dt0.dt != dt_from_ymd(2013, 12, 10) || pceT->dt0.iSec != 75600 || pceT->dt1.dt != 0 || pceT->dt1.iSec != 0) {
      printf("ERROR 4\n");
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

    if ((pceT = CalendarElementNew(BAD_CAST"2013-12-11T10:11:00CST ISO")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (xmlStrEqual(pceT->pucSep, BAD_CAST" ISO") == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dt0.dt != dt_from_ymd(2013, 12, 11) || pceT->dt0.iSec != 7860 || pceT->dt1.dt != 0 || pceT->dt1.iSec != 0) {
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
    else if (pceT->dt0.dt != dt_from_ymd(2011, 7, 3) || pceT->dt1.dt != dt_from_ymd(2011, 7, 11)) {
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
    else if (pceT->dt0.dt != dt_from_ymd(2013, 12, 11) || pceT->dt1.dt != dt_from_ymd(2014, 3, 11)) {
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
    else if (pceT->dt0.dt != dt_from_ymd(2011, 7, 3) || pceT->dt1.dt != 0) {
      printf("ERROR\n");
    }
    else if (CalendarElementUpdate(pceT, BAD_CAST"20110711/20110703") == NULL) {
      printf("ERROR CalendarElementUpdate()\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dt0.dt != dt_from_ymd(2011, 7, 3) || pceT->dt1.dt != dt_from_ymd(2011, 7, 11)) {
      printf("ERROR\n");
    }
    else if (CalendarElementUpdate(pceT, BAD_CAST"20110703/P3Y6M4D TEST") == NULL) {
      printf("ERROR CalendarElementUpdate()\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dt0.dt != dt_from_ymd(2011, 7, 3) || pceT->dt1.dt != dt_from_ymd(2015, 1, 6)) {
      printf("ERROR\n");
    }
    else if (CalendarElementUpdate(pceT, BAD_CAST"P3W/20110722 TEST") == NULL) {
      printf("ERROR CalendarElementUpdate()\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dt0.dt != dt_from_ymd(2011, 7, 2) || pceT->dt1.dt != dt_from_ymd(2011, 7, 22)) {
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

    if ((pceT = CalendarElementNew(BAD_CAST"2013-10-12T14:00:00+02:00/2013-10-14T14:00:00Z ISO date interval")) == NULL) {
      printf("ERROR 1\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR 2\n");
    }
    else if (xmlStrEqual(pceT->pucSep, BAD_CAST" ISO date interval") == FALSE) {
      printf("ERROR 3\n");
    }
    else if (pceT->dt0.dt != dt_from_ymd(2013, 10, 12) || (pceT->dt0.iSec != 43200 && pceT->dt0.iSec != 43200 - 3600)
      || pceT->dt1.dt != dt_from_ymd(2013, 10, 14) || (pceT->dt1.iSec != 50400 && pceT->dt1.iSec != 50400 - 3600)) {
      printf("ERROR 4\n");
    }
    else if (CalendarElementUpdate(pceT, BAD_CAST"2012-10-15T08:00:00/2012-10-15T10:00:00 TEST") == NULL) {
      printf("ERROR CalendarElementUpdate()\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR 2\n");
    }
    else if (xmlStrEqual(pceT->pucSep, BAD_CAST" TEST") == FALSE) {
      printf("ERROR 3\n");
    }
    else if (pceT->dt0.dt != dt_from_ymd(2012, 10, 15) || (pceT->dt0.iSec != 25200 && pceT->dt0.iSec != 25200 - 3600)
      || pceT->dt1.dt != pceT->dt0.dt || (pceT->dt1.iSec != 32400 && pceT->dt1.iSec != 32400 - 3600)) {
      printf("ERROR 4\n");
    }
    else if (CalendarElementUpdate(pceT, BAD_CAST"2012-10-15T08:00:00/PT2H30M0S TEST") == NULL) {
      printf("ERROR CalendarElementUpdate()\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR 2\n");
    }
    else if (xmlStrEqual(pceT->pucSep, BAD_CAST" TEST") == FALSE) {
      printf("ERROR 3\n");
    }
    else if (pceT->dt0.dt != dt_from_ymd(2012, 10, 15) || (pceT->dt0.iSec != 25200 && pceT->dt0.iSec != 25200 - 3600)
      || pceT->dt1.dt != 734791 || (pceT->dt1.iSec != 34200 && pceT->dt1.iSec != 34200 - 3600)) {
      printf("ERROR 4\n");
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
    else if (pceT->dt0.dt != dt_from_ymd(2009,1,1) || pceT->dt1.dt != dt_from_ymd(2009, 12, 31)) {
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
    else if (pceT->dt0.dt != dt_from_ymd(2009, 3, 1) || pceT->dt1.dt != dt_from_ymd(2009, 3, 31)) {
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
    else if (pceT->dt0.dt != dt_from_ywd(2009, 33, 1) || pceT->dt1.dt != dt_from_ywd(2009, 33, 7)) {
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
    ceElementPtr pceI;
    ceElementPtr pceList = NULL;

    i++;
    printf("TEST %i in '%s:%i': ", i, __FILE__, __LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"R-21/")) == NULL) {
      printf("ERROR CalendarElementNew()\n");
    }
    else if (ScanCalendarElementDate(pceT) == TRUE) {
      printf("ERROR\n");
    }
    else if ((pceList = SplitCalendarElementRecurrences(pceT)) != NULL) {
      printf("ERROR\n");
    }
    else if (CalendarElementUpdate(pceT, BAD_CAST"R2/20110703/P3M TEST") == NULL) {
      printf("ERROR CalendarElementUpdate()\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dt0.dt != dt_from_ymd(2011, 7, 3) || pceT->dt1.dt != 0 || pceT->iRecurrence != 2) {
      printf("ERROR\n");
    }
    else if ((pceList = SplitCalendarElementRecurrences(pceT)) == NULL) {
      printf("ERROR\n");
    }
    else if ((pceI = pceList) == NULL     || pceI->dt0.dt != dt_from_ymd(2011, 7, 3) || pceI->dt1.dt != 0 || pceI->iRecurrence != -1) {
      printf("ERROR\n");
    }
    else if ((pceI = pceI->pNext) == NULL || pceI->dt0.dt != dt_from_ymd(2011, 10, 3) || pceI->dt1.dt != 0 || pceI->iRecurrence != -1) {
      printf("ERROR\n");
    }
    else if ((pceI = pceI->pNext) == NULL || pceI->dt0.dt != dt_from_ymd(2012, 1, 3) || pceI->dt1.dt != 0 || pceI->iRecurrence != -1) {
      printf("ERROR\n");
    }
    else if ((pceI = pceI->pNext) != NULL) {
      printf("ERROR\n");
    }
    else if (CalendarElementUpdate(pceT, BAD_CAST"R2/2022-02-06T14:00:00/2022-02-06T16:00:00/P4D TEST") == NULL) {
      printf("ERROR CalendarElementUpdate()\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dt0.dt != dt_from_ymd(2022, 2, 6) || (pceT->dt0.iSec != 46800 && pceT->dt0.iSec != 46800 - 3600)
      || pceT->dt1.dt != pceT->dt0.dt || (pceT->dt1.iSec != 54000 && pceT->dt1.iSec != 54000 - 3600)
      || pceT->period.d != 4 || pceT->iRecurrence != 2) {
      printf("ERROR\n");
    }
    else if ((pceList = SplitCalendarElementRecurrences(pceT)) == NULL) {
      printf("ERROR\n");
    }
    else if ((pceI = pceList) == NULL || pceI->dt0.dt != dt_from_ymd(2022, 2, 6)
      || pceI->dt1.dt != pceI->dt0.dt || (pceI->dt0.iSec != 46800 && pceI->dt0.iSec != 46800 - 3600)
      || (pceI->dt1.iSec != 54000 && pceI->dt1.iSec != 54000 - 3600) || pceI->iRecurrence != -1) {
      printf("ERROR\n");
    }
    else if ((pceI = pceI->pNext) == NULL || pceI->dt0.dt != dt_from_ymd(2022, 2, 10)
      || pceI->dt1.dt != pceI->dt0.dt || (pceI->dt0.iSec != 46800 && pceI->dt0.iSec != 46800 - 3600)
      || (pceI->dt1.iSec != 54000 && pceI->dt1.iSec != 54000 - 3600) || pceI->iRecurrence != -1) {
      printf("ERROR\n");
    }
    else if ((pceI = pceI->pNext) == NULL || pceI->dt0.dt != dt_from_ymd(2022, 2, 14)
      || pceI->dt1.dt != pceI->dt0.dt || (pceI->dt0.iSec != 46800 && pceI->dt0.iSec != 46800 - 3600)
      || (pceI->dt1.iSec != 54000 && pceI->dt1.iSec != 54000 - 3600) || pceI->iRecurrence != -1) {
      printf("ERROR\n");
    }
    else if ((pceI = pceI->pNext) != NULL) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceList);
    CalendarElementFree(pceT);
  }


  if (RUNTEST) {
    ceElementPtr pceT;
    ceElementPtr pceI;
    ceElementPtr pceList = NULL;

    i++;
    printf("TEST %i in '%s:%i': ", i, __FILE__, __LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"R-21/")) == NULL) {
      printf("ERROR CalendarElementNew()\n");
    }
    else if (ScanCalendarElementDate(pceT) == TRUE) {
      printf("ERROR\n");
    }
    else if (CalendarElementUpdate(pceT, BAD_CAST"R2/20110703/P3M2DT12H30M5S TEST") == NULL) {
      printf("ERROR CalendarElementUpdate()\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dt0.dt != dt_from_ymd(2011, 7, 3) || pceT->dt1.dt != 0 || pceT->iRecurrence != 2) {
      printf("ERROR\n");
    }
    else if ((pceList = SplitCalendarElementRecurrences(pceT)) == NULL) {
      printf("ERROR\n");
    }
    else if ((pceI = pceList) == NULL || pceI->dt0.dt != dt_from_ymd(2011, 7, 3) || pceI->dt1.dt != 0 || pceI->iRecurrence != -1) {
      printf("ERROR\n");
    }
    else if ((pceI = pceI->pNext) == NULL || pceI->dt0.dt != dt_from_ymd(2011, 10, 5) || pceI->dt1.dt != 0 || pceI->iRecurrence != -1) {
      printf("ERROR\n");
    }
    else if ((pceI = pceI->pNext) == NULL || pceI->dt0.dt != dt_from_ymd(2012, 1, 7) || pceI->dt1.dt != 0 || pceI->iRecurrence != -1) {
      printf("ERROR\n");
    }
    else if ((pceI = pceI->pNext) != NULL) {
      printf("ERROR\n");
    }
    else if (CalendarElementUpdate(pceT, BAD_CAST"R2/P3M2DT12H30M5S/20110703 TEST") == NULL) {
      printf("ERROR CalendarElementUpdate()\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->dt1.dt != dt_from_ymd(2011, 7, 3) || pceT->dt0.dt != 0 || pceT->iRecurrence != 2) {
      printf("ERROR\n");
    }
    else if ((pceList = SplitCalendarElementRecurrences(pceT)) == NULL) {
      printf("ERROR\n");
    }
    else if ((pceI = pceList) == NULL || pceI->dt0.dt != 0 || pceI->dt1.dt != dt_from_ymd(2011, 7, 3) || pceI->iRecurrence != -1) {
      printf("ERROR\n");
    }
    else if ((pceI = pceI->pNext) == NULL || pceI->dt0.dt != 0 || pceI->dt1.dt != dt_from_ymd(2011, 4, 1) || pceI->iRecurrence != -1) {
      printf("ERROR\n");
    }
    else if ((pceI = pceI->pNext) == NULL || pceI->dt0.dt != 0 || pceI->dt1.dt != dt_from_ymd(2010, 12, 30) || pceI->iRecurrence != -1) {
      printf("ERROR\n");
    }
    else if ((pceI = pceI->pNext) != NULL) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    CalendarElementFree(pceT);
  }


  if (RUNTEST) {
    xmlNodePtr pndBlock;
    xmlNodePtr pndDate = NULL;
    xmlChar* pucT = NULL;

    i++;
    printf("TEST %i in '%s:%i':  = ", i, __FILE__, __LINE__);

    if ((pndBlock = xmlNewNode(NULL, BAD_CAST"block")) == NULL
      || (pndDate = xmlNewChild(pndBlock, NULL, BAD_CAST"date", BAD_CAST"R2/2022-02-06T14:00:00/2022-02-06T16:00:00/P4D")) == NULL) {
      printf("Error xmlNewNode()\n");
    }
    else if (AddNodeDateAttributes(pndDate, NULL) == FALSE) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST"date result", pndBlock);
    xmlFreeNode(pndBlock);
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
    else if (UpdateToday(BAD_CAST"2017") > -1) {
      printf("ERROR UpdateToday()\n");
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
