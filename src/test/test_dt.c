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

/*! s. "c-dt/API.pod"
*/
int
dtTest(void)
{
  int i;
  int n_ok;

  n_ok=0;
  i=0;

  if (RUNTEST) {
    dt_t dtp;
    
    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);

    if (dt_parse_iso_date("2022-03-01", 20, &dtp) < 1) {
      printf("ERROR dt_parse_string()\n");
    }
    else if (dt_year(dtp) != 2022 || dt_month(dtp) != 3 || dt_dom(dtp) != 1) {
      printf("ERROR dt_parse_string()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  
  if (RUNTEST) {
    int sod, nsec;
    
    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);

    if (dt_parse_iso_time_extended("09:11:00.55", 20, &sod, &nsec) < 1) {
      printf("ERROR dt_parse_iso_time_basic()\n");
    }
    else if (sod != 33060 || nsec != 550000000) {
      printf("ERROR dt_parse_iso_time_basic()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  
  if (RUNTEST) {
    dt_t dtp;
    int sod, nsec;
    int l;
    unsigned char *pucT = "2022-03-01T09:11:00.55 TEST";
    
    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);

    if ((l = dt_parse_iso_date(pucT, 20, &dtp)) != 10) {
      printf("ERROR dt_parse_iso_date()\n");
    }
    else if (dt_year(dtp) != 2022 || dt_month(dtp) != 3 || dt_dom(dtp) != 1) {
      printf("ERROR dt_parse_iso_date()\n");
    }
    else if ((l = dt_parse_iso_time_extended(&pucT[l+1], 20, &sod, &nsec)) != 11) {
      printf("ERROR dt_parse_iso_time_basic()\n");
    }
    else if (sod != 33060 || nsec != 550000000) {
      printf("ERROR dt_parse_iso_time_basic()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  
  if (RUNTEST) {
    dt_t dt;

    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);

    dt = dt_from_easter(2012, DT_WESTERN);

    if (dt_year(dt) != 2012 || dt_month(dt) != 3 || dt_dom(dt) != 1) {
      printf("ERROR dt_from_easter()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }
  
  return (i - n_ok);
}
/* end of dtTest() */
