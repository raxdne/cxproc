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
    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);

    xmlStrPrintf(buffer,BUFFER_LENGTH, "20091011,17,1122,33");
    
    if (calConcatNextDate(buffer) == NULL || xmlStrEqual(buffer, BAD_CAST"20091017,1122,33") == FALSE) {
      printf("ERROR\n");
    }
    else if (calConcatNextDate(buffer) == NULL || xmlStrEqual(buffer, BAD_CAST"20091122,33") == FALSE) {
      printf("ERROR\n");
    }
    else if (calConcatNextDate(buffer) == NULL || xmlStrEqual(buffer, BAD_CAST"20091133") == FALSE) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': find end of date string = ",i,__FILE__,__LINE__);

    if (xmlStrEqual(EndOfDate(BAD_CAST"20091011.;END"),BAD_CAST";END") == FALSE) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    pieCalendarElementType ceT;

    i++;
    printf("TEST %i in '%s:%i': GetWeekOfYear() = ",i,__FILE__,__LINE__);
    ;
    if ((ceT.iWeek = GetWeekOfYear(3, 6, 2013)) != 23) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);

    xmlStrPrintf(buffer,BUFFER_LENGTH, "20091011,w33mon");
    
    if (calConcatNextDate(buffer) !=NULL) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);

    xmlStrPrintf(buffer,BUFFER_LENGTH, "20091011,20101011");
    
    if (xmlStrEqual(calConcatNextDate(buffer),BAD_CAST"20101011") == FALSE) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);

    xmlStrPrintf(buffer,BUFFER_LENGTH, "20121015,20130613,0701,05");
    
    if (xmlStrEqual(calConcatNextDate(buffer),BAD_CAST"20130613,0701,05") == FALSE) {
      printf("ERROR\n");
    }
    else if (xmlStrEqual(calConcatNextDate(buffer), BAD_CAST"20130701,05") == FALSE) {
      printf("ERROR\n");
    }
    else if (xmlStrEqual(calConcatNextDate(buffer), BAD_CAST"20130705") == FALSE) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);
    
    xmlStrPrintf(buffer,BUFFER_LENGTH, "20121208,201306");

    if (calConcatNextDate(buffer) != NULL) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': trailing separator ",i,__FILE__,__LINE__);

    xmlStrPrintf(buffer,BUFFER_LENGTH, "20091011,1112,");
    
    if (xmlStrEqual(calConcatNextDate(buffer),BAD_CAST"20091112,") == FALSE) {
      printf("ERROR\n");
    }
    else if (calConcatNextDate(buffer) != NULL) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {

    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);
    
    xmlStrPrintf(buffer,BUFFER_LENGTH, "2009*24wed,2010*w25wed");

    if (xmlStrEqual(calConcatNextDate(buffer),BAD_CAST"2010*w25wed") == FALSE) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);

    if (GetDayAbsolute(2008,1,1,-1,-1) - GetDayAbsolute(2008,-1,-1,1,2) != 0) {
      printf("ERROR\n");
    }
    else if (GetDayAbsolute(2010, 1, 1, -1, -1) - GetDayAbsolute(2010, -1, -1, 0, 5) != 0) {
      printf("ERROR\n");
    }
    else if (GetDayAbsolute(2012, 1, 1, -1, -1) - GetDayAbsolute(2012, -1, -1, 0, 0) != 0) {
      printf("ERROR\n");
    }
    else if (GetDayAbsolute(2012, 1, 10, -1, -1) - GetDayAbsolute(2012, -1, -1, 2, 2) != 0) {
      printf("ERROR\n");
    }
    else if (GetDayAbsolute(2012, 1, 10, -1, -1) - GetDayAbsolute(2012, -1, -1, 0, 0) != 9) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    int h, m, s;
    xmlChar *pucT;
      
    i++;
    printf("TEST %i in '%s:%i': ScanTimeTripeString()",i,__FILE__,__LINE__);

    if ((pucT = ScanTimeTripeString(NULL, NULL, NULL, NULL)) != NULL) {
      printf("ERROR\n");
    }
    else if ((pucT = ScanTimeTripeString(BAD_CAST"1.14", &h, &m, &s)) == NULL
	     || h != 1 || m != 14 || s != 00) {
      printf("ERROR\n");
    }
    else if ((pucT = ScanTimeTripeString(BAD_CAST" 04:44:44", &h, &m, &s)) == NULL
	     || h != 4 || m != 44 || s != 44) {
      printf("ERROR\n");
    }
    else if ((pucT = ScanTimeTripeString(BAD_CAST"T055555", &h, &m, &s)) == NULL
	     || h != 5 || m != 55 || s != 55) {
      printf("ERROR\n");
    }
    else if ((pucT = ScanTimeTripeString(BAD_CAST"T056655", &h, &m, &s)) != NULL) {
      printf("ERROR\n");
    }
    else if ((pucT = ScanTimeTripeString(BAD_CAST"T02:24:24Z", &h, &m, &s)) == NULL
	     || h != 2 || m != 24 || s != 24) {
      printf("ERROR\n");
    }
    else if ((pucT = ScanTimeTripeString(BAD_CAST" 03:33:00-5.55", &h, &m, &s)) == NULL
	     || h != 3 || m != 33 || s != 00) {
      printf("ERROR\n");
    }
    else if ((pucT = ScanTimeTripeString(pucT, &h, &m, &s)) == NULL
	     || h != 5 || m != 55 || s != 00) {
      printf("ERROR\n");
    }
    else if ((pucT = ScanTimeTripeString(pucT, &h, &m, &s)) != NULL
	     || h != -1 || m != -1 || s != -1) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    int m,d;
    long int liEasterAbs;

    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);

    if ((liEasterAbs = GetEasterSunday(2031, &m, &d)) != 22381 || m != 4 || d != 13) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

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
    else if (pceT->eTypeDate != DATE_YEAR) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2009 || pceT->iMonth != -1 || pceT->iDay != -1 || pceT->iWeek != -1 || pceT->iDayWeek != -1) {
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
    else if (pceT->eTypeDate != DATE_MONTH) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2009 || pceT->iMonth != 3 || pceT->iDay != -1 || pceT->iWeek != -1 || pceT->iDayWeek != -1) {
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
    else if (pceT->eTypeDate != DATE_DAY) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2009 || pceT->iMonth != 10 || pceT->iDay != 11 || pceT->iWeek != -1 || pceT->iDayWeek != -1) {
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

  if (RUNTEST) {
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

  if (RUNTEST) {
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
    else if (pceT->iYear != 2009 || pceT->iMonth != 10 || pceT->iDay != 11 || pceT->iWeek != -1 || pceT->iDayWeek != -1) {
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

  if (RUNTEST) {
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

  if (RUNTEST) {
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

  if (RUNTEST) {
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

  if (RUNTEST) {
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

  if (RUNTEST) {
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

  if (RUNTEST) {
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

  if (RUNTEST) {
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

  if (RUNTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"0000*w99tue")) == NULL) {
      printf("ERROR: CalendarElementNew()\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR: ScanCalendarElementDate()\n");
    }
    else if (pceT->eTypeDate != DATE_WEEK) {
      printf("ERROR: eTypeDate\n");
    }
    else if (pceT->iYear != 0 || pceT->iMonth != -1 || pceT->iDay != -1 || pceT->iWeek != 99 || pceT->iDayWeek != 2 || pceT->iStep != -1) {
      //printf("%i %i %i %i %i %i \n", pceT->iYear, pceT->iMonth, pceT->iDay, pceT->iWeek, pceT->iDayWeek, pceT->iStep);
      printf("ERROR: value\n");
    }
    else if ((pceT->iYear = 2009) != 2009 || ScanDateIteration(pceT) == FALSE
	     || pceT->iMonth != -1 || pceT->iDay != -1 || pceT->iWeek != 99 || pceT->iDayWeek != 2 || pceT->iStep != 7) {
      //printf("%i %i %i %i %i %i \n", pceT->iYear, pceT->iMonth, pceT->iDay, pceT->iWeek, pceT->iDayWeek, pceT->iStep);
      printf("ERROR: value\n");
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

    if ((pceT = CalendarElementNew(BAD_CAST"20090911.3x10")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (xmlStrEqual(pceT->pucSep, BAD_CAST".3x10") == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_DAY) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2009 || pceT->iMonth != 9 || pceT->iDay != 11 || pceT->iWeek != -1 || pceT->iDayWeek != -1
	|| pceT->iAnchor != 0 || pceT->iStep != -1 || pceT->iCount != -1) {
      printf("ERROR\n");
    }
    else if (ScanDateIteration(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2009 || pceT->iMonth != 9 || pceT->iDay != 11 || pceT->iWeek != -1 || pceT->iDayWeek != -1
      || pceT->iAnchor != 14497 || pceT->iStep != 3 || pceT->iCount != 10) {
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

    if ((pceT = CalendarElementNew(BAD_CAST"2009*w34tue.7x3")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (xmlStrEqual(pceT->pucSep, BAD_CAST".7x3") == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_WEEK) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2009 || pceT->iMonth != -1 || pceT->iDay != -1 || pceT->iWeek != 34 || pceT->iDayWeek != 2
	|| pceT->iAnchor != 0 || pceT->iStep != -1 || pceT->iCount != -1) {
      printf("ERROR\n");
    }
    else if (ScanDateIteration(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2009 || pceT->iMonth != -1 || pceT->iDay != -1 || pceT->iWeek != 34 || pceT->iDayWeek != 2
      || pceT->iAnchor != 14473 || pceT->iStep != 7 || pceT->iCount != 3) {
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

    if ((pceT = CalendarElementNew(BAD_CAST"2009*w34tue:30.4")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (xmlStrEqual(pceT->pucSep, BAD_CAST":30.4") == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_WEEK) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2009 || pceT->iMonth != -1 || pceT->iDay != -1 || pceT->iWeek != 34 || pceT->iDayWeek != 2
	|| pceT->iAnchor != 0 || pceT->iStep != -1 || pceT->iCount != -1) {
      printf("ERROR\n");
    }
    else if (ScanDateIteration(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2009 || pceT->iMonth != -1 || pceT->iDay != -1 || pceT->iWeek != 34 || pceT->iDayWeek != 2
      || pceT->iAnchor != 14473 || pceT->iStep != 4 || pceT->iCount != 7) {
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

    if ((pceT = CalendarElementNew(BAD_CAST"20140301#20140401")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (xmlStrEqual(pceT->pucSep, BAD_CAST"#20140401") == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_DAY) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2014 || pceT->iMonth != 3 || pceT->iDay != 1 || pceT->iWeek != -1 || pceT->iDayWeek != -1
      || pceT->iAnchor != 0 || pceT->iStep != -1 || pceT->iCount != -1) {
      printf("ERROR\n");
    }
    else if (ScanDateIteration(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2014 || pceT->iMonth != 3 || pceT->iDay != 1 || pceT->iWeek != -1 || pceT->iDayWeek != -1
      || pceT->iAnchor != 16129 || pceT->iStep != 1 || pceT->iCount != 31) {
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

    if ((pceT = CalendarElementNew(BAD_CAST"2012*w39tue#w41wed")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (xmlStrEqual(pceT->pucSep, BAD_CAST"#w41wed") == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_WEEK) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2012 || pceT->iMonth != -1 || pceT->iDay != -1 || pceT->iWeek != 39 || pceT->iDayWeek != 2
	|| pceT->iAnchor != 0 || pceT->iStep != -1 || pceT->iCount != -1) {
      printf("ERROR\n");
    }
    else if (ScanDateIteration(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2012 || pceT->iMonth != -1 || pceT->iDay != -1 || pceT->iWeek != 39 || pceT->iDayWeek != 2
      || pceT->iAnchor != 15607 || pceT->iStep != 1 || pceT->iCount != 15) {
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

    if ((pceT = CalendarElementNew(BAD_CAST"20140301,20140401")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (xmlStrEqual(pceT->pucSep, BAD_CAST",20140401") == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_DAY) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2014 || pceT->iMonth != 3 || pceT->iDay != 1 || pceT->iWeek != -1 || pceT->iDayWeek != -1
	|| pceT->iAnchor != 0 || pceT->iStep != -1 || pceT->iCount != -1) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2014 || pceT->iMonth != 4 || pceT->iDay != 1 || pceT->iWeek != -1 || pceT->iDayWeek != -1
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
    printf("TEST %i in '%s:%i': scan a calendar element = ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"20140301+14")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_DAY) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == TRUE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_ERROR) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2014 || pceT->iMonth != 3 || pceT->iDay != 15 || pceT->iWeek != -1 || pceT->iDayWeek != -1
	|| pceT->iAnchor != 16143 || pceT->iStep != -1 || pceT->iCount != -1) {
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

    if ((pceT = CalendarElementNew(BAD_CAST"20140301-14")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_DAY) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2014 || pceT->iMonth != 2 || pceT->iDay != 15 || pceT->iWeek != -1 || pceT->iDayWeek != -1
      || pceT->iAnchor != 16115 || pceT->iStep != -1 || pceT->iCount != -1) {
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

  if (RUNTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': scan a calendar element = ", i, __FILE__, __LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"2009@e+17")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->eTypeDate != DATE_DAY) {
      printf("ERROR\n");
    }
    else if (pceT->iYear != 2009 || pceT->iMonth != 4 || pceT->iDay != 29 || pceT->iWeek != -1 || pceT->iDayWeek != -1
      || pceT->iAnchor != 14362 || pceT->iStep != -1 || pceT->iCount != -1) {
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
    printf("TEST %i in '%s:%i': ScanCalendarElementTime ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"  TEST")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == TRUE) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementTime(pceT) == TRUE) {
      printf("ERROR\n");
    }
    else if (pceT->iHourA == -1 && pceT->iMinuteA == -1 && pceT->iSecondA == -1
	     && pceT->iHourB == -1 && pceT->iMinuteB == -1 && pceT->iSecondB == -1) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("ERROR\n");
    }
    CalendarElementFree(pceT);
  }

  if (RUNTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': ScanCalendarElementTime ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST" 11.73.21 ABC")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == TRUE) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementTime(pceT) == TRUE) {
      printf("ERROR\n");
    }
    else if (pceT->iHourA == -1 && pceT->iMinuteA == -1 && pceT->iSecondA == -1
	     && pceT->iHourB == -1 && pceT->iMinuteB == -1 && pceT->iSecondB == -1) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("ERROR\n");
    }
    CalendarElementFree(pceT);
  }

  if (RUNTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': ScanCalendarElementTime ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST"10.14 Essen")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == TRUE) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementTime(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->iHourA == 10 && pceT->iMinuteA == 14 && pceT->iSecondA == 0
	     && pceT->iHourB == -1 && pceT->iMinuteB == -1 && pceT->iSecondB == -1) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("ERROR\n");
    }
    CalendarElementFree(pceT);
  }

  if (RUNTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': ScanCalendarElementTime ",i,__FILE__,__LINE__);

    if ((pceT = CalendarElementNew(BAD_CAST" 18.33-23.55 DDDD")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == TRUE) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementTime(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->iHourA == 18 && pceT->iMinuteA == 33 && pceT->iSecondA == 0
	     && pceT->iHourB == 23 && pceT->iMinuteB == 55 && pceT->iSecondB == 0) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("ERROR\n");
    }
    CalendarElementFree(pceT);
  }

  if (RUNTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': ScanCalendarElementTime ",i,__FILE__,__LINE__);
    
    if ((pceT = CalendarElementNew(BAD_CAST" 11.12.2013 10.11 HaHa")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementTime(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->iYear == 2013
	     && pceT->iMonth == 12 && pceT->iDay== 11      && pceT->iWeek == -1 && pceT->iDayWeek==-1
	     && pceT->iHourA == 10 && pceT->iMinuteA == 11 && pceT->iSecondA == 0
	     && pceT->iHourB == -1 && pceT->iMinuteB == -1 && pceT->iSecondB == -1) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("ERROR\n");
    }
    CalendarElementFree(pceT);
  }


  if (SKIPTEST) {
    pieCalendarElementPtr pceT;

    i++;
    printf("TEST %i in '%s:%i': ScanCalendarElementTime ", i, __FILE__, __LINE__);
    
    if ((pceT = CalendarElementNew(BAD_CAST"\t20091011,17,1122,33 11.11-13.00 Test")) == NULL) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementDate(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (ScanCalendarElementTime(pceT) == FALSE) {
      printf("ERROR\n");
    }
    else if (pceT->iYear == 2013
	     && pceT->iMonth == 12 && pceT->iDay == 11 && pceT->iWeek == -1 && pceT->iDayWeek == -1
	     && pceT->iHourA == 10 && pceT->iMinuteA == 11 && pceT->iSecondA == 0
	     && pceT->iHourB == -1 && pceT->iMinuteB == -1 && pceT->iSecondB == -1) {
      n_ok++;
      printf("OK\n");
    }
    else {
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
    else if (ScanCalendarElementTime(pceT) == FALSE) {
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
    else if (ScanCalendarElementTime(pceT) == FALSE) {
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


  printf("Result in '%s': %i/%i OK\n\n", __FILE__, n_ok, i);

  return (i - n_ok);
}
/* end of calTest() */
