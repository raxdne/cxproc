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
xslTest(void)
{
  int i;
  int n_ok;

  n_ok=0;
  i=0;

  if (RUNTEST) {
      /* TEST:
       */
      xmlDocPtr pdocXml = NULL;
      xmlDocPtr pdocXsl = NULL;
      xmlDocPtr pdocResult = NULL;
      xsltStylesheetPtr pxslT = NULL;
      int options;

      i++;
      printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);

      options = XML_PARSE_RECOVER | XML_PARSE_NOENT | XML_PARSE_NOERROR | XML_PARSE_NOWARNING | XML_PARSE_NSCLEAN;
#ifdef _WIN32
      options |= XML_PARSE_NONET;
#endif

      if ((pdocXml = xmlReadFile((const char *) TESTPREFIX  "xml/baustelle.pie", NULL, options)) == NULL) {
	printf("ERROR 'Cant read XML file'\n");
      }
      else if ((pdocXsl = xmlReadFile((const char *) TESTPREFIX "xsl/TestValidate.xsl", NULL, options)) == NULL) {
	printf("ERROR 'Cant read XSL file'\n");
      }              
      else if ((pxslT = xsltParseStylesheetDoc(pdocXsl)) == NULL) {
	printf("ERROR 'Cant parse this Stylesheet'\n");
      }
      else if ((pdocResult = xsltApplyStylesheet(pxslT, pdocXml, NULL)) == NULL) {
	printf("ERROR 'Cant apply this Stylesheet'\n");
      }
      else {
	n_ok++;
	printf("OK\n");
      }
      xmlFreeDoc(pdocResult);
      xsltFreeStylesheet(pxslT);  /* xsltFreeStylesheet() releases the DOM also */
      xmlFreeDoc(pdocXml);
  }

  return (i - n_ok);
}
/* end of xslTest() */
