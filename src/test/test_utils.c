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
utilsTest(void)
{
  int i;
  int n_ok;
  xmlChar *pucTest;
  xmlChar buffer[BUFFER_LENGTH];

  n_ok=0;
  i=0;

  if (RUNTEST) {
    char mchT[BUFFER_LENGTH];
    int dT0;
    int dT1;

    i++;
    printf("TEST %i in '%s:%i': ", i, __FILE__, __LINE__);
    sprintf(mchT, "  ABCDE    DERT\n\nGG\n\rjhskjhdfkj   \n\t\n");
    dT0 = strlen(mchT);
    NormalizeStringSpaces(mchT);
    dT1 = strlen(mchT);

    if (dT0 - dT1 == 13) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("Error NormalizeStringSpaces()\n");
    }
  }

  if (RUNTEST) {
    /* TEST: 
     */
    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);
    pucTest = ReplaceStr(BAD_CAST"ABC & DEF &#x263C; GHI",BAD_CAST"&",BAD_CAST"&amp;");
    if (xmlStrcmp(pucTest,BAD_CAST"ABC &amp; DEF &#x263C; GHI")==0) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("ERROR '%s'\n",pucTest);
    }
    xmlFree(pucTest);
  }

  if (RUNTEST) {
    /* TEST:
     */
    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);
    pucTest = ReplaceStr(BAD_CAST"ABCDEFGH",BAD_CAST"DEF",BAD_CAST"---");
    if (xmlStrcmp(pucTest,BAD_CAST"ABC---GH")==0) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("ERROR '%s'\n",pucTest);
    }
    xmlFree(pucTest);
  }

  if (RUNTEST) {
    /* TEST:
     */
    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);
    pucTest = ReplaceStr(BAD_CAST"Mon %d. 2011",BAD_CAST"%d",BAD_CAST"15");
    if (xmlStrcmp(pucTest,BAD_CAST"Mon 15. 2011")==0) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("ERROR '%s'\n",pucTest);
    }
    xmlFree(pucTest);
  }

  if (RUNTEST) {
    /* TEST:
     */
    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);
    pucTest = ReplaceStr(BAD_CAST"Mon %OO 2011",BAD_CAST"%OO",BAD_CAST"&#x263C;");
    if (xmlStrcmp(pucTest,BAD_CAST"Mon &#x263C; 2011")==0) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("ERROR '%s'\n",pucTest);
    }
    xmlFree(pucTest);
  }

  if (RUNTEST) {
    /* TEST: 
     */
    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);
    pucTest = xmlStrdup(BAD_CAST"ABC\tDEF\t\tGHI\n");
    if (chomp(pucTest)==11) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("ERROR '%s'\n",pucTest);
    }
    xmlFree(pucTest);
  }

  if (RUNTEST) {
    xmlChar mucBase64[BUFFER_LENGTH];
    xmlChar mucResult[BUFFER_LENGTH];
    size_t l = BUFFER_LENGTH;

    i++;
    printf("TEST %i in '%s:%i': base64 encoding and decoding = ",i,__FILE__,__LINE__);
    pucTest = xmlStrdup(BAD_CAST"ABCÜÖÄKKKK");

    if (base64encode(pucTest,strlen((char *)pucTest)+1,(char *)mucBase64,BUFFER_LENGTH) == 1 && strlen((char *)mucBase64) == 20) {
      if (base64decode((char *)mucBase64,strlen((char *)mucBase64),mucResult,&l)==0 && xmlStrEqual(mucResult,pucTest)) {
        n_ok++;
        printf("OK\n");
      }
      else {
        printf("ERROR '%s'\n",mucResult);
      }
    }
    else {
      printf("ERROR '%s'\n",mucBase64);
    }
    xmlFree(pucTest);
  }

  if (RUNTEST) {
    xmlChar mucBase64[BUFFER_LENGTH];
    xmlChar mucResult[BUFFER_LENGTH];
    size_t l = 10;

    i++;
    printf("TEST %i in '%s:%i': base64 error handling = ",i,__FILE__,__LINE__);

    if (base64encode(NULL,10,(char *)mucBase64,BUFFER_LENGTH) == 0 && base64decode(NULL,0,mucResult,&l) == 0) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("ERROR\n");
    }
  }


  if (RUNTEST) {
    xmlChar* pucError = xmlStrdup(BAD_CAST"=47=08=1=6H=7a=74 ");
    xmlChar* pucT = xmlStrdup(BAD_CAST"Der Almsee ist ein Bergsee im ober=F6sterreichischen Teil des Salzkammergutes im Gemeindegebiet von Gr=FCnau im Almtal, am Nordfu=DF des Toten Gebirges und liegt auf 589 m =FC. A.");

    i++;
    printf("TEST %i in '%s:%i': = ", i, __FILE__, __LINE__);

    if (DecodeQuotedPrintable(NULL) != -1) {
      printf("ERROR\n");
    }
    else if (DecodeQuotedPrintable(pucError) != -3) {
      printf("ERROR\n");
    }
    else if (DecodeQuotedPrintable(pucT) != 0) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    // puts(pucT);
    xmlFree(pucT);
    xmlFree(pucError);
  }


  if (RUNTEST) {

    i++;
    printf("TEST %i in '%s:%i': change string to lowercase chars = ", i, __FILE__, __LINE__);
    pucTest = xmlStrdup(BAD_CAST"AbCÜöÄKkKK");

    if (StringToLower(NULL) == TRUE) {
      printf("ERROR 1\n");
    }
    else if (StringToLower((char *)"") == TRUE) {
      printf("ERROR 2\n");
    }
    else if (StringToLower((char *)pucTest) == FALSE) {
      printf("ERROR 3\n");
    }
#if 0
    else if (xmlStrEqual(pucTest, BAD_CAST"abcüöäkkkk") == FALSE) {
      printf("ERROR 4\n");
    }
#endif
    else {
      n_ok++;
      printf("OK\n");
    }
    xmlFree(pucTest);
  }

  
  if (RUNTEST) {

    i++;
    printf("TEST %i in '%s:%i': string beginning with defined substring = ", i, __FILE__, __LINE__);

    if (StringBeginsWith(NULL,NULL) == TRUE) {
      printf("ERROR 1\n");
    }
    else if (StringBeginsWith((char *)"","") == TRUE) {
      printf("ERROR 2\n");
    }
    else if (StringBeginsWith((char *)"Abc def ","Ab") == FALSE) {
      printf("ERROR 3\n");
    }
    else if (StringBeginsWith((char *)" AAA Abc def ","Ab") == TRUE) {
      printf("ERROR 4\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }


  if (RUNTEST) {

    i++;
    printf("TEST %i in '%s:%i': string ending with defined substring = ", i, __FILE__, __LINE__);

    if (StringEndsWith(NULL,NULL) != NULL) {
      printf("ERROR 1\n");
    }
    else if (StringEndsWith((char *)"","") != NULL) {
      printf("ERROR 2\n");
    }
    else if (StringEndsWith((char *)"Abc def ","Ab") != NULL) {
      printf("ERROR 3\n");
    }
    else if (StringEndsWith((char *)" AAA Abc def"," def") == NULL) {
      printf("ERROR 4\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }


  if (RUNTEST) {

    i++;
    printf("TEST %i in '%s:%i': change string to uppercase chars = ", i, __FILE__, __LINE__);
    pucTest = xmlStrdup(BAD_CAST"abCüöäkKkk");

    if (StringToUpper(NULL) == TRUE) {
      printf("ERROR 1\n");
    }
    else if (StringToUpper((char *)"") == TRUE) {
      printf("ERROR 2\n");
    }
    else if (StringToUpper((char *)pucTest) == FALSE) {
      printf("ERROR 3\n");
    }
#if 0
    else if (xmlStrEqual(pucTest, BAD_CAST"ABCÜÖÄKKKK") == FALSE) {
      printf("ERROR 4\n");
    }
#endif
    else {
      n_ok++;
      printf("OK\n");
    }
    xmlFree(pucTest);
  }


  if (RUNTEST) {
    /* static references */
    xmlChar* pucTestA = BAD_CAST"invalid: &#; or &#x; or \\u \\U &#144;";
    xmlChar* pucTestB = BAD_CAST"valid: &#13; <= &#x21D2; <=> \\u21D2 => &#x2014; \\U2014";

    xmlChar* pucResult0 = NULL;
    xmlChar* pucResult1 = NULL;
    xmlChar* pucResult2 = NULL;
    xmlChar* pucResult3 = NULL;

    i++;
    printf("TEST %i in '%s:%i': StringDecodeNumericCharsNew() = ", i, __FILE__, __LINE__);

    if ((pucResult0 = StringDecodeNumericCharsNew(NULL)) != NULL) {
      printf("ERROR 1\n");
    }
    else if ((pucResult1 = StringDecodeNumericCharsNew(BAD_CAST"")) != NULL) {
      printf("ERROR 2\n");
    }
    else if ((pucResult2 = StringDecodeNumericCharsNew(pucTestA)) == NULL || xmlStrEqual(pucResult2, pucTestA) == FALSE) {
      printf("ERROR 3\n");
    }
    else if ((pucResult3 = StringDecodeNumericCharsNew(pucTestB)) == NULL || xmlStrEqual(pucResult3, pucTestB) == TRUE || xmlStrlen(pucResult3) > xmlStrlen(pucTestB)) {
      printf("ERROR 4\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    xmlFree(pucResult3);
    xmlFree(pucResult2);
    xmlFree(pucResult1);
    xmlFree(pucResult0);
  }


  if (RUNTEST) {
    xmlChar* pucResult0 = NULL;
    xmlChar* pucResult1 = NULL;
    xmlChar* pucResult2 = NULL;

    i++;
    printf("TEST %i in '%s:%i': StringEncodeXmlDefaultEntitiesNew() = ", i, __FILE__, __LINE__);

    if ((pucResult0 = StringEncodeXmlDefaultEntitiesNew(NULL)) != NULL) {
      printf("ERROR 1\n");
    }
    else if ((pucResult1 = StringEncodeXmlDefaultEntitiesNew(BAD_CAST"")) != NULL) {
      printf("ERROR 2\n");
    }
    else if ((pucResult2 = StringEncodeXmlDefaultEntitiesNew(BAD_CAST"valid: A & B > C < D >>EEE<<")) == NULL
      || xmlStrEqual(pucResult2, BAD_CAST "valid: A &amp; B &gt; C &lt; D &gt;&gt;EEE&lt;&lt;") == FALSE) {
      printf("ERROR 4\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    xmlFree(pucResult2);
    xmlFree(pucResult1);
    xmlFree(pucResult0);
  }


  if (RUNTEST) {
    xmlChar *pucTest0 = BAD_CAST"";
    xmlChar *pucTestA = BAD_CAST"''";
    xmlChar *pucTestB = BAD_CAST"Aäoöuüß";
    xmlChar *pucTestC = BAD_CAST" #aÄoÖuÜ ";
    xmlChar *pucT = NULL;
    xmlChar *pucTT = NULL;
    xmlChar *pucTTT = NULL;

    i++;
    printf("TEST %i in '%s:%i':  = ", i, __FILE__, __LINE__);

    if ((pucT = StringReplaceUmlauteNew(NULL)) != NULL) {
      printf("ERROR 1\n");
    }
    else if ((pucT = StringReplaceUmlauteNew(pucTest0)) != NULL) {
      printf("ERROR 2\n");
    }
    else if ((pucT = StringReplaceUmlauteNew(pucTestA)) != NULL) {
      printf("ERROR 3\n");
    }
    else if ((pucTT = StringReplaceUmlauteNew(pucTestB)) == NULL || xmlStrEqual(pucTT, BAD_CAST"Aaeooeuuess") == FALSE) {
      printf("ERROR 4\n");
    }
    else if ((pucTTT = StringReplaceUmlauteNew(pucTestC)) == NULL || xmlStrEqual(pucTTT, BAD_CAST" #aAeoOeuUe ") == FALSE) {
      printf("ERROR 5\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    xmlFree(pucTTT);
    xmlFree(pucTT);
    xmlFree(pucT);
  }


  if (RUNTEST) {
    xmlChar *pucTest0 = xmlStrdup(BAD_CAST"");
    xmlChar *pucTestA = xmlStrdup(BAD_CAST"''");
    xmlChar *pucTestB = xmlStrdup(BAD_CAST"'''");
    xmlChar *pucTestC = xmlStrdup(BAD_CAST"'abCüöä'kKkk\"");
    xmlChar *pucTestD = xmlStrdup(BAD_CAST"'abCüöäkKkk'");
    xmlChar *pucTestE = xmlStrdup(BAD_CAST"  abCüöäkKkk \t   ");
    xmlChar *pucTestF = xmlStrdup(BAD_CAST"   A   ");

    i++;
    printf("TEST %i in '%s:%i':  = ", i, __FILE__, __LINE__);

    if (StringRemovePairQuotes(NULL) == TRUE) {
      printf("ERROR 1\n");
    }
    else if (StringRemovePairQuotes(pucTest0) == TRUE) {
      printf("ERROR 2\n");
    }
    else if (StringRemovePairQuotes(pucTestA) == FALSE || xmlStrlen(pucTestA) > 0) {
      printf("ERROR 3\n");
    }
    else if (StringRemovePairQuotes(pucTestB) == FALSE || xmlStrEqual(pucTestB, BAD_CAST"'") == FALSE) {
      printf("ERROR 4\n");
    }
    else if (StringRemovePairQuotes(pucTestC) == TRUE) {
      printf("ERROR 5\n");
    }
    else if (StringRemovePairQuotes(pucTestD) == FALSE || xmlStrEqual(pucTestD, BAD_CAST"abCüöäkKkk") == FALSE) {
      printf("ERROR 6\n");
    }
    else if (StringRemovePairQuotes(pucTestE) == TRUE || xmlStrEqual(pucTestE, BAD_CAST"abCüöäkKkk") == FALSE) {
      printf("ERROR 7\n");
    }
    else if (StringRemovePairQuotes(pucTestF) == TRUE || xmlStrlen(pucTestF) != 1) {
      printf("ERROR 8\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    xmlFree(pucTestF);
    xmlFree(pucTestE);
    xmlFree(pucTestD);
    xmlFree(pucTestC);
    xmlFree(pucTestB);
    xmlFree(pucTestA);
    xmlFree(pucTest0);
  }


  if (RUNTEST) {
    xmlChar *pucTest0 = NULL;
    xmlChar *pucTestA = NULL;
    xmlChar *pucTestB = NULL;

    i++;
    printf("TEST %i in '%s:%i':  = ", i, __FILE__, __LINE__);

    if ((pucTest0 = EncodeRFC1738(NULL)) != NULL) {
      printf("ERROR 1\n");
    }
    else if ((pucTestA = EncodeRFC1738(BAD_CAST"")) == NULL || xmlStrlen(pucTestA) > 0) {
      printf("ERROR 2\n");
    }
    else if ((pucTestB = EncodeRFC1738(BAD_CAST"ABüößXZ")) == NULL || xmlStrEqual(pucTestB, BAD_CAST"'")) {
      printf("ERROR 3\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    xmlFree(pucTest0);
    xmlFree(pucTestB);
    xmlFree(pucTestA);
  }


  if (RUNTEST) {
    double latit;
    double longit;

    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);

    if (GetPositionISO6709("993911.2+1909029.3/",&latit,&longit) == 0) {
      printf("ERROR GetPositionISO6709(): range invalid\n");
    }
    else if (GetPositionISO6709("+4851+00221",&latit,&longit) != 0 || fabs(latit - 48.850) > 0.001 || fabs(longit - 2.350) > 0.001) {
      printf("ERROR GetPositionISO6709(): format %.3f %.3f\n", latit, longit);
    }
    else if (GetPositionISO6709("+4851-00221",&latit,&longit) != 0 || fabs(latit - 48.850) > 0.001 || fabs(longit + 2.350) > 0.001) {
      printf("ERROR GetPositionISO6709(): format %.3f %.3f\n", latit, longit);
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }


  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': ", i, __FILE__, __LINE__);

    xmlStrPrintf(buffer, BUFFER_LENGTH, "20091011,17,1122,33");

    if (StringConcatNextDate(buffer) == NULL || xmlStrEqual(buffer, BAD_CAST"20091017,1122,33") == FALSE) {
      printf("ERROR\n");
    }
    else if (StringConcatNextDate(buffer) == NULL || xmlStrEqual(buffer, BAD_CAST"20091122,33") == FALSE) {
      printf("ERROR\n");
    }
    else if (StringConcatNextDate(buffer) == NULL || xmlStrEqual(buffer, BAD_CAST"20091133") == FALSE) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': ", i, __FILE__, __LINE__);

    xmlStrPrintf(buffer, BUFFER_LENGTH, "20091011,20101011");

    if (xmlStrEqual(StringConcatNextDate(buffer), BAD_CAST"20101011") == FALSE) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': ", i, __FILE__, __LINE__);

    xmlStrPrintf(buffer, BUFFER_LENGTH, "20121015,20130613,0701,05");

    if (xmlStrEqual(StringConcatNextDate(buffer), BAD_CAST"20130613,0701,05") == FALSE) {
      printf("ERROR\n");
    }
    else if (xmlStrEqual(StringConcatNextDate(buffer), BAD_CAST"20130701,05") == FALSE) {
      printf("ERROR\n");
    }
    else if (xmlStrEqual(StringConcatNextDate(buffer), BAD_CAST"20130705") == FALSE) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': ", i, __FILE__, __LINE__);

    xmlStrPrintf(buffer, BUFFER_LENGTH, "20121208,201306");

    if (StringConcatNextDate(buffer) != NULL) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    i++;
    printf("TEST %i in '%s:%i': trailing separator ", i, __FILE__, __LINE__);

    xmlStrPrintf(buffer, BUFFER_LENGTH, "20091011,1112,");

    if (xmlStrEqual(StringConcatNextDate(buffer), BAD_CAST"20091112,") == FALSE) {
      printf("ERROR\n");
    }
    else if (StringConcatNextDate(buffer) != NULL) {
      printf("ERROR\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  /* ISO 8601 Durations */
  
  if (RUNTEST) {
    int y, m, d, w;
    
    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);

    if (dt_parse_iso_period(NULL, 20, NULL, NULL, NULL, NULL) != 0) {
      printf("ERROR dt_parse_iso_period()\n");
    }
    else if (dt_parse_iso_period("P1YM-4DT", 20, &y, &m, &d, NULL) != 0) {
      printf("ERROR dt_parse_iso_period()\n");
    }
    else if (dt_parse_iso_period("P3Y6M4DT12H30M5S", BUFFER_LENGTH, &y, &m, &d, NULL) != 16) {
      printf("ERROR dt_parse_iso_period()\n");
    }
    else if (y != 3 || m != 6 || d != 4) {
      printf("ERROR dt_parse_iso_period()\n");
    }
    else if (dt_parse_iso_period("P7Y", BUFFER_LENGTH, &y, &m, &d, NULL) != 3) {
      printf("ERROR dt_parse_iso_period()\n");
    }
    else if (y != 7 || m != 0 || d != 0) {
      printf("ERROR dt_parse_iso_period()\n");
    }
    else if (dt_parse_iso_period("P2W", BUFFER_LENGTH, &y, &m, &d, &w) != 3) {
      printf("ERROR dt_parse_iso_period()\n");
    }
    else if (w != 2 || m != 0 || d != 0) {
      printf("ERROR dt_parse_iso_period()\n");
    }
    else if (dt_parse_iso_period("P", BUFFER_LENGTH, &y, &m, &d, &w) != 1) {
      printf("ERROR dt_parse_iso_period()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

   /* ISO 8601 Time intervals */
 
  if (RUNTEST) {
    dt_t dt0, dt1;
    
    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);

    if (dt_parse_iso_date_interval(NULL, 0, &dt0, &dt1) != 0) {
      printf("ERROR dt_parse_iso_date_interval()\n");
    }
    else if (dt_parse_iso_date_interval("20110703/20110711", BUFFER_LENGTH, NULL, NULL) != 0) {
      printf("ERROR dt_parse_iso_date_interval()\n");
    }
    else if (dt_parse_iso_date_interval("20110703/2011 AAA", BUFFER_LENGTH, &dt0, &dt1) != 0) {
      printf("ERROR dt_parse_iso_date_interval()\n");
    }
    else if (dt_parse_iso_date_interval("20110703/20110711 AAA", BUFFER_LENGTH, &dt0, &dt1) != 17) {
      printf("ERROR dt_parse_iso_date_interval()\n");
    }
    else if (dt_year(dt0) != 2011 || dt_month(dt0) != 7 || dt_dom(dt0) != 3) {
      printf("ERROR dt_parse_iso_date_interval()\n");
    }
    else if (dt_year(dt1) != 2011 || dt_month(dt1) != 7 || dt_dom(dt1) != 11) {
      printf("ERROR dt_parse_iso_date_interval()\n");
    }
    else if (dt_parse_iso_date_interval("20110711/20110711 AAA", BUFFER_LENGTH, &dt0, &dt1) != 17) {
      printf("ERROR dt_parse_iso_date_interval()\n");
    }
    else if (dt0 != dt1) {
      printf("ERROR dt_parse_iso_date_interval()\n");
    }
    else if (dt_parse_iso_date_interval("20110711/20110703 AAA", BUFFER_LENGTH, &dt0, &dt1) != 17) {
      printf("ERROR dt_parse_iso_date_interval()\n");
    }
    else if (dt_year(dt0) != 2011 || dt_month(dt0) != 7 || dt_dom(dt0) != 3) {
      printf("ERROR dt_parse_iso_date_interval()\n");
    }
    else if (dt_year(dt1) != 2011 || dt_month(dt1) != 7 || dt_dom(dt1) != 11) {
      printf("ERROR dt_parse_iso_date_interval()\n");
    }
    else if (dt_parse_iso_date_interval("2011-399 AAA", BUFFER_LENGTH, &dt0, &dt1) != 4) {
      printf("ERROR dt_parse_iso_date_interval()\n");
    }
    else if (dt_year(dt0) != 2011 || dt_month(dt0) != 1 || dt_dom(dt0) != 1) {
      printf("ERROR dt_parse_iso_date_interval()\n");
    }
    else if (dt_parse_iso_date_interval("20110703/P3Y6M4D TEST", BUFFER_LENGTH, &dt0, &dt1) != 16) {
      printf("ERROR dt_parse_iso_date_interval()\n");
    }
    else if (dt_year(dt0) != 2011 || dt_month(dt0) != 7 || dt_dom(dt0) != 3) {
      printf("ERROR dt_parse_iso_date_interval()\n");
    }
    else if (dt_year(dt1) != 2015 || dt_month(dt1) != 1 || dt_dom(dt1) != 7) {
      printf("ERROR dt_parse_iso_date_interval()\n");
    }
    else if (dt_parse_iso_date_interval("P3W/20110703 TEST", BUFFER_LENGTH, &dt0, &dt1) != 12) {
      printf("ERROR dt_parse_iso_date_interval()\n");
    }
    else if (dt_year(dt1) != 2011 || dt_month(dt1) != 7 || dt_dom(dt1) != 3) {
      printf("ERROR dt_parse_iso_date_interval()\n");
    }
    else if (dt_year(dt0) != 2011 || dt_month(dt0) != 06 || dt_dom(dt0) != 12) {
      printf("ERROR dt_parse_iso_date_interval()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

   /* ISO 8601 Repeating intervals */
 
  if (RUNTEST) {
    int y, m, d, r;
    
    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);

    if (dt_parse_iso_recurrance(NULL, 20, NULL) != 0) {
      printf("ERROR dt_parse_iso_recurrance()\n");
    }
    else if (dt_parse_iso_recurrance("R-21/", 20, &r) != 0) {
      printf("ERROR dt_parse_iso_recurrance()\n");
    }
    else if (dt_parse_iso_recurrance("R3/20110703/P3M2DT12H30M5S", BUFFER_LENGTH, &r) != 2) {
      printf("ERROR dt_parse_iso_recurrance()\n");
    }
    else if (r != 3) {
      printf("ERROR dt_parse_iso_recurrance()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }


  if (RUNTEST) {

    i++;
    printf("TEST %i in '%s:%i': = ",i,__FILE__,__LINE__);

    if (GetWeekOfYear(3, 6, 2013) != 23) {
      printf("ERROR GetWeekOfYear()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  if (RUNTEST) {
    xmlChar *pucTest;

    i++;
    printf("TEST %i in '%s:%i': ", i, __FILE__, __LINE__);
    pucTest = GetDateIsoString((time_t)0);
    if (pucTest != NULL) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("ERROR '%s'\n", pucTest);
    }
    xmlFree(pucTest);
  }

  if (RUNTEST) {
    xmlChar *pucTest;

    i++;
    printf("TEST %i in '%s:%i': ", i, __FILE__, __LINE__);
    pucTest = GetDateIsoString((time_t)1295593140);
    if (pucTest != NULL && xmlStrEqual(pucTest, BAD_CAST"2011-01-21T07:59:00")) {
      n_ok++;
      printf("OK\n");
    }
    else {
      printf("ERROR '%s'\n", pucTest);
    }
    xmlFree(pucTest);
  }

  if (RUNTEST) {
    xmlChar *pucT;
    xmlChar *pucTT;

    i++;
    printf("TEST %i in '%s:%i': ", i, __FILE__, __LINE__);
    pucT = GetNowFormatStr(BAD_CAST"%Y-%m%-%dT%H:%M:%S CW%V/%Y (%j)");
    if (pucT != NULL) {
      pucTT = GetNowFormatStr(BAD_CAST"NumOfWeek");
      if (pucTT != NULL) {
	n_ok++;
	printf("OK\n");
      }
      else {
	printf("ERROR NumOfWeek '%s'\n", pucTT);
      }
      xmlFree(pucTT);
    }
    else {
      printf("ERROR GetNowFormatStr() '%s'\n", pucT);
    }
    xmlFree(pucT);
  }

  printf("Result in '%s': %i/%i OK\n\n",__FILE__,n_ok,i);

  return (i - n_ok);
}
/* end of utilsTest() */
