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
* https://www.pcre.org/current/doc/html/pcre2demo.html
*/
int
pcreTest(void)
{
  int i;
  int n_ok;

  n_ok=0;
  i=0;

#ifdef HAVE_PCRE2

  if (RUNTEST) {
    int errornumber = 0;
    size_t erroroffset;
    pcre2_code* preT = NULL;

    i++;
    printf("TEST %i in '%s:%i': libpcre2() = ", i, __FILE__, __LINE__);

    preT = pcre2_compile(
			 NULL, /* the pattern */
			 PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
			 PCRE2_UTF|PCRE2_CASELESS,        /* default options */
			 &errornumber,          /* for error number */
			 &erroroffset,          /* for error offset */
			 NULL);                 /* use default compile context */

    if (preT != NULL) {
      pcre2_code_free(preT);
      printf("Error 1\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }


  if (RUNTEST) {
    int errornumber = 0;
    size_t erroroffset;
    pcre2_code* preT = NULL;
    PCRE2_UCHAR buffer[256];

    i++;
    printf("TEST %i in '%s:%i': libpcre2() = ", i, __FILE__, __LINE__);

    preT = pcre2_compile(
			 (PCRE2_SPTR8)"][+\"", /* the pattern */
			 PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
			 PCRE2_UTF|PCRE2_CASELESS,        /* default options */
			 &errornumber,          /* for error number */
			 &erroroffset,          /* for error offset */
			 NULL);                 /* use default compile context */

    if (preT != NULL) {
      pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
      printf("PCRE2 compilation failed at offset %d: %s\n", (int)erroroffset, buffer);
      pcre2_code_free(preT);
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }


  if (RUNTEST) {
    int rc;
    int errornumber = 0;
    size_t erroroffset;
    pcre2_code* preT = NULL;
    pcre2_match_data *match_data = NULL;
    PCRE2_UCHAR buffer[256];
    xmlChar* pucT = BAD_CAST "A1B2C3D4E5F";

    i++;
    printf("TEST %i in '%s:%i': libpcre2() = ", i, __FILE__, __LINE__);

    preT = pcre2_compile(
			 (PCRE2_SPTR8)"[0-9]+", /* the pattern */
			 PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
			 PCRE2_UTF|PCRE2_CASELESS,        /* default options */
			 &errornumber,          /* for error number */
			 &erroroffset,          /* for error offset */
			 NULL);                 /* use default compile context */

    if (preT == NULL) {
      pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
      printf("PCRE2 compilation failed at offset %d: %s\n", (int)erroroffset, buffer);
      pcre2_code_free(preT);
    }
    else if ((match_data = pcre2_match_data_create_from_pattern(preT, NULL)) == NULL) {
      printf("Error 1\n");
    }
    else if ((rc = pcre2_match(preT, (PCRE2_SPTR8)pucT, xmlStrlen(pucT),0,0,match_data,NULL)) < 0) {
      printf("Error 2\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    pcre2_match_data_free(match_data);   /* Release memory used for the match */
    pcre2_code_free(preT);
  }


  if (RUNTEST) {
    int rc;
    int errornumber = 0;
    size_t erroroffset;
    pcre2_code* preT = NULL;
    pcre2_match_data* match_data = NULL;
    PCRE2_UCHAR buffer[256];
    xmlChar* pucT = BAD_CAST "A1B2C3 www D4E5F";
    xmlChar* pucTT = BAD_CAST "[$1](http://www.$1/$1)";

    PCRE2_UCHAR output[256];
    size_t output_length = 256;

    /* https://www.pcre.org/current/doc/html/pcre2_substitute.html
    https://cpp.hotexamples.com/examples/-/-/pcre2_substitute/cpp-pcre2_substitute-function-examples.html
    */

    i++;
    printf("TEST %i in '%s:%i': libpcre2() match + replace = ", i, __FILE__, __LINE__);

    preT = pcre2_compile(
      (PCRE2_SPTR8)"(www)", /* the pattern */
      PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
      PCRE2_UTF | PCRE2_CASELESS,        /* default options */
      &errornumber,          /* for error number */
      &erroroffset,          /* for error offset */
      NULL);                 /* use default compile context */

    if (preT == NULL) {
      pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
      printf("PCRE2 compilation failed at offset %d: %s\n", (int)erroroffset, buffer);
      pcre2_code_free(preT);
    }
    else if ((rc = pcre2_substitute(preT, (PCRE2_SPTR8)pucT, xmlStrlen(pucT), 0, PCRE2_SUBSTITUTE_GLOBAL, NULL, NULL,(PCRE2_SPTR8)pucTT, xmlStrlen(pucTT), output, &output_length)) < 0) {
      printf("Error 1\n");
      pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
      printf("PCRE2 compilation failed at offset %d: %s\n", (int)erroroffset, buffer);
      pcre2_code_free(preT);
    }
    else if (strcmp(output, "A1B2C3 [www](http://www.www/www) D4E5F")) {
      printf("Error 2: '%s'\n", output);
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    pcre2_code_free(preT);
  }

#endif
  
  printf("Result in '%s': %i/%i OK\n\n", __FILE__, n_ok, i);

  return (i - n_ok);
}
/* end of pcreTest() */
