xmlNsPtr pnsxml;
xmlNsPtr pnsxsl;
xmlNsPtr pnscxp;
xmlNsPtr pnspie;
xmlChar *pucT;
xmlDocPtr pdocResult;
xmlNodePtr pnd1;
xmlNodePtr pnd2;
xmlNodePtr pnd3;
xmlNodePtr pnd4;
xmlNodePtr pnd5;
xmlNodePtr pnd6;
xmlNodePtr pnd7;
xmlNodePtr pnd8;
xmlNodePtr pnd9;
xmlNodePtr pnd10;
xmlNodePtr pnd11;
xmlNodePtr pnd12;
xmlNodePtr pnd13;
xmlNodePtr pnd14;
xmlNodePtr pnd15;
xmlNodePtr pnd16;
xmlNodePtr pnd17;
xmlNodePtr pnd18;
xmlNodePtr pnd19;
xmlNodePtr pnd20;
xmlNodePtr pnd21;
xmlNodePtr pnd22;
xmlNodePtr pnd23;
xmlNodePtr pnd24;
xmlNodePtr pnd25;
xmlNodePtr pnd26;
xmlNodePtr pnd27;
xmlNodePtr pnd28;
xmlNodePtr pnd29;
xmlNodePtr pnd30;
xmlNodePtr pnd31;
xmlNodePtr pnd32;
xmlNodePtr pnd33;
xmlNodePtr pnd34;
xmlNodePtr pnd35;
xmlNodePtr pnd36;
xmlNodePtr pnd37;
xmlNodePtr pnd38;
xmlNodePtr pnd39;
xmlNodePtr pnd40;
xmlNodePtr pnd41;
xmlNodePtr pnd42;
xmlNodePtr pnd43;
xmlNodePtr pnd44;
xmlNodePtr pnd45;
xmlNodePtr pnd46;
xmlNodePtr pnd47;
xmlNodePtr pnd48;
xmlNodePtr pnd49;
xmlNodePtr pnd50;
xmlNodePtr pnd51;
xmlNodePtr pnd52;
xmlNodePtr pnd53;
xmlNodePtr pnd54;
xmlNodePtr pnd55;
xmlNodePtr pnd56;
xmlNodePtr pnd57;
xmlNodePtr pnd58;
xmlNodePtr pnd59;
xmlNodePtr pnd60;
xmlNodePtr pnd61;
xmlNodePtr pnd62;
xmlNodePtr pnd63;
xmlNodePtr pnd64;
xmlNodePtr pnd65;
xmlNodePtr pnd66;
xmlNodePtr pnd67;
xmlNodePtr pnd68;
xmlNodePtr pnd69;
xmlNodePtr pnd70;
xmlNodePtr pnd71;
xmlNodePtr pnd72;
xmlNodePtr pnd73;
xmlNodePtr pnd74;
xmlNodePtr pnd75;
xmlNodePtr pnd76;
xmlNodePtr pnd77;
xmlNodePtr pnd78;
xmlNodePtr pnd79;
xmlNodePtr pnd80;
xmlNodePtr pnd81;
xmlNodePtr pnd82;
xmlNodePtr pnd83;
xmlNodePtr pnd84;
xmlNodePtr pnd85;
xmlNodePtr pnd86;
xmlNodePtr pnd87;
xmlNodePtr pnd88;
xmlNodePtr pnd89;
xmlNodePtr pnd90;

pdocResult = xmlNewDoc(BAD_CAST "1.0");

pnd1 = xmlNewDocNode(pdocResult, NULL, BAD_CAST "pie", NULL);
xmlDocSetRootElement(pdocResult, pnd1);
pnsxsl = xmlNewNs(pnd1,BAD_CAST "http://www.w3.org/1999/XSL/Transform",BAD_CAST "xsl");
pnscxp = xmlNewNs(pnd1,BAD_CAST "http://www.tenbusch.info/cxproc",BAD_CAST "cxp");
pnspie = xmlNewNs(pnd1,BAD_CAST "http://www.tenbusch.info/pie",BAD_CAST "pie");
xmlSetProp(pnd1, BAD_CAST "version", BAD_CAST "1.0");

pnd2 = xmlNewChild(pnd1,NULL,"section",NULL);

pnd3 = xmlNewChild(pnd2,NULL,"h",NULL);
xmlNodeAddContent(pnd3, BAD_CAST "PIE Specification");

pnd4 = xmlNewChild(pnd2,NULL,"section",NULL);

pnd5 = xmlNewChild(pnd4,NULL,"h",NULL);
xmlNodeAddContent(pnd5, BAD_CAST "Outline Nodes");

pnd6 = xmlNewChild(pnd4,NULL,"p",NULL);
xmlNodeAddContent(pnd6, BAD_CAST "Par");

pnd7 = xmlNewChild(pnd4,NULL,"list",NULL);

pnd8 = xmlNewChild(pnd7,NULL,"p",NULL);
xmlNodeAddContent(pnd8, BAD_CAST "A");

pnd9 = xmlNewChild(pnd7,NULL,"p",NULL);
xmlNodeAddContent(pnd9, BAD_CAST "B");

pnd10 = xmlNewChild(pnd7,NULL,"p",NULL);
xmlNodeAddContent(pnd10, BAD_CAST "C");

pnd11 = xmlNewChild(pnd4,NULL,"task",NULL);

pnd12 = xmlNewChild(pnd11,NULL,"h",NULL);
xmlNodeAddContent(pnd12, BAD_CAST "Demo Task");

pnd13 = xmlNewChild(pnd11,NULL,"p",NULL);
xmlNodeAddContent(pnd13, BAD_CAST "details");

pnd14 = xmlNewChild(pnd4,NULL,"table",NULL);

pnd15 = xmlNewChild(pnd14,NULL,"thead",NULL);

pnd16 = xmlNewChild(pnd15,NULL,"tr",NULL);

pnd17 = xmlNewChild(pnd16,NULL,"th",NULL);
xmlSetProp(pnd17, BAD_CAST "colspan", BAD_CAST "2");
xmlNodeAddContent(pnd17, BAD_CAST "Z");

pnd18 = xmlNewChild(pnd14,NULL,"tbody",NULL);

pnd19 = xmlNewChild(pnd18,NULL,"tr",NULL);

pnd20 = xmlNewChild(pnd19,NULL,"td",NULL);
xmlNodeAddContent(pnd20, BAD_CAST "Y");

pnd21 = xmlNewChild(pnd19,NULL,"td",NULL);
xmlNodeAddContent(pnd21, BAD_CAST "X");

pnd22 = xmlNewChild(pnd4,NULL,"section",NULL);

pnd23 = xmlNewChild(pnd22,NULL,"h",NULL);
xmlNodeAddContent(pnd23, BAD_CAST "Sub Section");

pnd24 = xmlNewChild(pnd22,NULL,"p",NULL);
xmlNodeAddContent(pnd24, BAD_CAST "AAA");

pnd25 = xmlNewChild(pnd2,NULL,"section",NULL);

pnd26 = xmlNewChild(pnd25,NULL,"h",NULL);
xmlNodeAddContent(pnd26, BAD_CAST "Inline Nodes");

pnd27 = xmlNewChild(pnd25,NULL,"p",NULL);
xmlNodeAddContent(pnd27, BAD_CAST "_em_ __bold__");

pnd28 = xmlNewChild(pnd25,NULL,"hr",NULL);

pnd29 = xmlNewChild(pnd25,NULL,"p",NULL);
xmlNodeAddContent(pnd29, BAD_CAST "http://www.abcd.de/");

pnd30 = xmlNewChild(pnd25,NULL,"section",NULL);

pnd31 = xmlNewChild(pnd30,NULL,"h",NULL);
xmlNodeAddContent(pnd31, BAD_CAST "Date Nodes");

pnd32 = xmlNewChild(pnd30,NULL,"p",NULL);
xmlNodeAddContent(pnd32, BAD_CAST "Date 2025-11-22");

pnd33 = xmlNewChild(pnd25,NULL,"section",NULL);

pnd34 = xmlNewChild(pnd33,NULL,"h",NULL);
xmlNodeAddContent(pnd34, BAD_CAST "Tag Nodes");

pnd35 = xmlNewChild(pnd33,NULL,"p",NULL);
xmlNodeAddContent(pnd35, BAD_CAST "Tag @test #haha");

pnd36 = xmlNewChild(pnd2,NULL,"section",NULL);

pnd37 = xmlNewChild(pnd36,NULL,"h",NULL);
xmlNodeAddContent(pnd37, BAD_CAST "Markup Nodes");

pnd38 = xmlNewChild(pnd36,NULL,"p",NULL);
xmlNodeAddContent(pnd38, BAD_CAST "TODO: to be done");

pnd39 = xmlNewChild(pnd36,NULL,"p",NULL);
xmlNodeAddContent(pnd39, BAD_CAST "DONE: already done");

pnd40 = xmlNewChild(pnd36,NULL,"p",NULL);
xmlNodeAddContent(pnd40, BAD_CAST "TEST: to be tested");

pnd41 = xmlNewChild(pnd36,NULL,"p",NULL);
xmlNodeAddContent(pnd41, BAD_CAST "REQ: required");

pnd42 = xmlNewChild(pnd36,NULL,"p",NULL);
xmlNodeAddContent(pnd42, BAD_CAST "BUG: to be fixed");

pnd43 = xmlNewChild(pnd36,NULL,"p",NULL);
xmlNodeAddContent(pnd43, BAD_CAST "Fig: Image");

pnd44 = xmlNewChild(pnd36,NULL,"p",NULL);
xmlNodeAddContent(pnd44, BAD_CAST "data:image/png;base64,CiMgQSAoQylvbmZpZ3VyYWJsZSAoWCltbCAoUFJPQyll");

pnd45 = xmlNewChild(pnd2,NULL,"section",NULL);

pnd46 = xmlNewChild(pnd45,NULL,"h",NULL);
xmlNodeAddContent(pnd46, BAD_CAST "HTML Nodes");

pnd47 = xmlNewChild(pnd45,NULL,"html",NULL);

pnd48 = xmlNewChild(pnd47,NULL,"div",NULL);

pnd49 = xmlNewChild(pnd48,NULL,"strong",NULL);
xmlNodeAddContent(pnd49, BAD_CAST "STRONG");

pnd50 = xmlNewChild(pnd48,NULL,"a",NULL);
xmlSetProp(pnd50, BAD_CAST "href", BAD_CAST "http://www.abcd.de/");
xmlNodeAddContent(pnd50, BAD_CAST "Link");

pnd51 = xmlNewChild(pnd2,NULL,"section",NULL);

pnd52 = xmlNewChild(pnd51,NULL,"h",NULL);
xmlNodeAddContent(pnd52, BAD_CAST "Inline Imports");

pnd53 = xmlNewChild(pnd51,NULL,"p",NULL);

pnd54 = xmlNewChild(pnd53,NULL,"import",NULL);
xmlNodeAddContent(pnd54, BAD_CAST "A");

pnd55 = xmlNewChild(pnd51,NULL,"p",NULL);

pnd56 = xmlNewChild(pnd55,NULL,"import",NULL);
xmlSetProp(pnd56, BAD_CAST "type", BAD_CAST "markdown");
xmlNodeAddContent(pnd56, BAD_CAST "A _B_");

pnd57 = xmlNewChild(pnd51,NULL,"p",NULL);
xmlNodeAddContent(pnd57, BAD_CAST "inline calc: ");

pnd58 = xmlNewChild(pnd57,NULL,"import",NULL);
xmlSetProp(pnd58, BAD_CAST "type", BAD_CAST "script");
xmlNodeAddContent(pnd58, BAD_CAST "5*5");

pnd59 = xmlNewChild(pnd2,NULL,"section",NULL);

pnd60 = xmlNewChild(pnd59,NULL,"h",NULL);
xmlNodeAddContent(pnd60, BAD_CAST "Node Imports");

pnd61 = xmlNewChild(pnd59,NULL,"import",NULL);
xmlNodeAddContent(pnd61, BAD_CAST "A");

pnd62 = xmlNewChild(pnd59,NULL,"import",NULL);
xmlSetProp(pnd62, BAD_CAST "type", BAD_CAST "markdown");
xmlNodeAddContent(pnd62, BAD_CAST "A _B_");

pnd63 = xmlNewChild(pnd59,NULL,"import",NULL);
xmlSetProp(pnd63, BAD_CAST "type", BAD_CAST "csv");
xmlNodeAddContent(pnd63, BAD_CAST "A,B;C\nD;E,F");

pnd64 = xmlNewChild(pnd59,NULL,"import",NULL);
xmlSetProp(pnd64, BAD_CAST "type", BAD_CAST "script");
xmlNodeAddContent(pnd64, BAD_CAST "5*5");

pnd65 = xmlNewChild(pnd59,NULL,"import",NULL);
xmlSetProp(pnd65, BAD_CAST "type", BAD_CAST "cxp");

pnd66 = xmlNewChild(pnd65,pnscxp,"info",NULL);

pnd67 = xmlNewChild(pnd2,NULL,"section",NULL);

pnd68 = xmlNewChild(pnd67,NULL,"h",NULL);
xmlNodeAddContent(pnd68, BAD_CAST "Structure-In-Node Imports");

pnd69 = xmlNewChild(pnd67,NULL,"import",NULL);
xmlNodeAddContent(pnd69, BAD_CAST "\n\
* A\n\
\n\
** B\n\
\n\
DEF\n\
\n\
** C\n\
\n\
*** CC\n\
\n\
**** CCC\n\
      ");

pnd70 = xmlNewChild(pnd67,NULL,"import",NULL);
xmlSetProp(pnd70, BAD_CAST "type", BAD_CAST "markdown");
xmlNodeAddContent(pnd70, BAD_CAST "\n\
# A\n\
\n\
## _B_\n\
\n\
DEF\n\
     \n\
## C\n\
     \n\
### CC\n\
     \n\
#### CCC\n\
");

pnd71 = xmlNewChild(pnd67,NULL,"import",NULL);
xmlSetProp(pnd71, BAD_CAST "type", BAD_CAST "csv");
xmlNodeAddContent(pnd71, BAD_CAST "A,B;C\nD;E,F");

pnd72 = xmlNewChild(pnd67,NULL,"import",NULL);
xmlSetProp(pnd72, BAD_CAST "type", BAD_CAST "script");
xmlNodeAddContent(pnd72, BAD_CAST "5*5");

pnd73 = xmlNewChild(pnd67,NULL,"import",NULL);
xmlSetProp(pnd73, BAD_CAST "type", BAD_CAST "cxp");

pnd74 = xmlNewChild(pnd73,pnscxp,"info",NULL);

pnd75 = xmlNewChild(pnd2,NULL,"section",NULL);

pnd76 = xmlNewChild(pnd75,NULL,"h",NULL);
xmlNodeAddContent(pnd76, BAD_CAST "File Imports");

pnd77 = xmlNewChild(pnd75,NULL,"import",NULL);
xmlSetProp(pnd77, BAD_CAST "name", BAD_CAST "test_none.txt");

pnd78 = xmlNewChild(pnd75,NULL,"import",NULL);
xmlSetProp(pnd78, BAD_CAST "name", BAD_CAST "test_pie.pie");

pnd79 = xmlNewChild(pnd75,NULL,"import",NULL);
xmlSetProp(pnd79, BAD_CAST "name", BAD_CAST "test_pie.txt");

pnd80 = xmlNewChild(pnd75,NULL,"import",NULL);
xmlSetProp(pnd80, BAD_CAST "name", BAD_CAST "test_pie.md");

pnd81 = xmlNewChild(pnd75,NULL,"import",NULL);
xmlSetProp(pnd81, BAD_CAST "name", BAD_CAST "test_pie.csv");

pnd82 = xmlNewChild(pnd75,NULL,"import",NULL);
xmlSetProp(pnd82, BAD_CAST "name", BAD_CAST "test_pie.cxp");

pnd83 = xmlNewChild(pnd75,NULL,"import",NULL);
xmlSetProp(pnd83, BAD_CAST "name", BAD_CAST "test_pie.js");

pnd84 = xmlNewChild(pnd2,NULL,"section",NULL);

pnd85 = xmlNewChild(pnd84,NULL,"h",NULL);
xmlNodeAddContent(pnd85, BAD_CAST "File Includes");

pnd86 = xmlNewChild(pnd84,NULL,"include",NULL);
xmlSetProp(pnd86, BAD_CAST "name", BAD_CAST "test_none.txt");

pnd87 = xmlNewChild(pnd84,NULL,"include",NULL);
xmlSetProp(pnd87, BAD_CAST "name", BAD_CAST "test_pie.pie");

pnd88 = xmlNewChild(pnd84,NULL,"include",NULL);
xmlSetProp(pnd88, BAD_CAST "name", BAD_CAST "test_pie.html");

pnd89 = xmlNewChild(pnd84,NULL,"include",NULL);
xmlSetProp(pnd89, BAD_CAST "name", BAD_CAST "test_pie.txt");

pnd90 = xmlNewChild(pnd84,NULL,"include",NULL);
xmlSetProp(pnd90, BAD_CAST "name", BAD_CAST "test_pie.js");

// xmlFreeDoc(pdocResult);
