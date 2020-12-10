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
xmlTest(void)
{
  int i;
  int n_ok;

  n_ok=0;
  i=0;

  if (RUNTEST) {
      /* TEST:
       */
      xmlDocPtr pdocResult = NULL;
      int options;

      i++;
      printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);

      options = XML_PARSE_RECOVER | XML_PARSE_NOENT | XML_PARSE_NOERROR | XML_PARSE_NOWARNING | XML_PARSE_NSCLEAN;
#ifdef _WIN32
      options |= XML_PARSE_NONET;
#endif

      if ((pdocResult = xmlReadFile((const char *) TESTPREFIX "xml/baustelle.pie", NULL, options)) == NULL) {
	printf("ERROR xmlReadFile()\n");
      }
      else {
	n_ok++;
	printf("OK\n");
      }
      //xmlSaveFileEnc("-",pdocResult,"UTF-8");
      xmlFreeDoc(pdocResult);
  }


  if (RUNTEST) {
    /* TEST:
     */
    xmlDocPtr pdocResult = NULL;
    xmlDocPtr pdocT = NULL;
    int options;
    xmlNodePtr pndT;
    xmlNodePtr pndPieRoot = NULL;
    xmlNodePtr pndBlock = NULL;
    xmlNsPtr* ppList = NULL;
    xmlNsPtr pNsI = NULL;
    xmlNsPtr pNsT = NULL;

    i++;
    printf("TEST %i in '%s:%i': ", i, __FILE__, __LINE__);

    options = XML_PARSE_RECOVER | XML_PARSE_NOENT | XML_PARSE_NOERROR | XML_PARSE_NOWARNING | XML_PARSE_NSCLEAN;
#ifdef _WIN32
    options |= XML_PARSE_NONET;
#endif

    if ((pdocResult = xmlNewDoc(BAD_CAST "1.0")) != NULL) {
      //pdocResult->encoding = xmlStrdup(BAD_CAST"UTF-8");

      pndPieRoot = xmlNewNode(NULL, BAD_CAST"root");
      xmlDocSetRootElement(pdocResult, pndPieRoot);
      xmlSetTreeDoc(pndPieRoot, pdocResult);

      if ((pdocT = xmlReadFile((const char*)TESTPREFIX "option/pie/text/xml/test-pie-ns.pie", NULL, options)) == NULL) {
	printf("ERROR xmlReadFile()\n");
      }
      else if ((ppList = xmlGetNsList(pdocT, xmlDocGetRootElement(pdocT))) == NULL) {
	printf("ERROR xmlGetNsList()\n");
      }
      else {
	n_ok++;
	printf("OK\n");
      }

      pndBlock = xmlNewChild(pndPieRoot, NULL, BAD_CAST"block", NULL);
      for (pNsI = *ppList; pNsI; pNsI = pNsI->next) {
	/* s. code in tree.c:xmlNewReconciliedNs() */

	pNsT = xmlNewNs(pndBlock, pNsI->href, pNsI->prefix);

	pndT = xmlNewChild(pndBlock, pNsT, BAD_CAST"p", pNsT->prefix);
      }
      pndT = xmlNewChild(pndBlock, NULL, BAD_CAST"p", NULL);

      //xmlSaveFileEnc("-", pdocResult, "UTF-8");

      //xmlFreeNsList(pList);
      xmlFreeDoc(pdocT);
      xmlFreeDoc(pdocResult);
    }
  }

  if (RUNTEST) {
    /* TEST:
     */
    int ciErrors = 0;
    xmlChar* pucAttr;
    xmlNodePtr pndT;
    xmlNodePtr pndTT;
    xmlNodePtr pndMeta;
    xmlNodePtr pndPieRoot = NULL;
    xmlNodePtr pndBlock = NULL;
    xmlDocPtr pdocResult = NULL;

    i++;
    printf("TEST %i in '%s:%i': ", i, __FILE__, __LINE__);

    if ((pdocResult = xmlNewDoc(BAD_CAST "1.0")) != NULL) {
      //pdocResult->encoding = xmlStrdup(BAD_CAST"UTF-8");

      pndPieRoot = xmlNewNode(NULL, BAD_CAST"root");
      xmlDocSetRootElement(pdocResult, pndPieRoot);
      xmlSetTreeDoc(pndPieRoot, pdocResult);

      pndBlock = xmlNewNode(NULL, BAD_CAST"block");

      pndT = xmlNewChild(pndBlock, NULL, BAD_CAST"child", BAD_CAST"ABC & DEF < as > ");
      if (pndT == NULL || pndT->children != NULL) {
	ciErrors++;
      }

      pndTT = xmlNewText(BAD_CAST"ABC & DEF < as > ");
      if (pndTT == NULL || STR_IS_EMPTY(pndTT->content)) {
	ciErrors++;
      }
      else {
	xmlAddChild(pndT, pndTT);
      }

      pndT = xmlNewChild(pndBlock, NULL, BAD_CAST"child", BAD_CAST"ABC &amp; DEF &lt; as &gt; ");
      if (pndT == NULL || pndT->children == NULL) {
	ciErrors++;
      }
      xmlAddChild(pndPieRoot, pndBlock);

      pndBlock = xmlCopyNode(pndBlock, 1); /* first copy for import processing */
      xmlNodeSetName(pndBlock, BAD_CAST"bblock");
      if (pndBlock == NULL || pndBlock->children == NULL) {
	ciErrors++;
      }
      xmlAddChild(pndPieRoot, pndBlock);

      pndMeta = xmlNewChild(pndPieRoot, NULL, NAME_META, NULL);

      // xmlSaveFileEnc("-",pdocResult,"UTF-8");
      xmlFreeDoc(pdocResult);
    }

    if (ciErrors > 0) {
      fprintf(stderr,"Errors %i", ciErrors);
    }
    else {
      n_ok++;
      printf("OK\n");
    }
  }

  return (i - n_ok);
}
/* end of xmlTest() */
