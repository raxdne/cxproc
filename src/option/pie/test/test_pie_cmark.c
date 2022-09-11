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
pieCmarkTest(void)
{
  int n_ok;
  int i;

  n_ok=0;
  i=0;
  

  if (SKIPTEST) {
    cmark_parser *parser;
    cmark_node *document;
    xmlDocPtr pdocT = NULL;
    xmlNodePtr pndRoot;
    xmlChar* pucT = BAD_CAST"# TEST #\n\nThis is a Test!\n";
    int options = CMARK_OPT_DEFAULT | CMARK_OPT_SMART;

    i++;
    printf("TEST %i in '%s:%i': cmark parser = ", i, __FILE__, __LINE__);

    pdocT = xmlNewDoc(BAD_CAST "1.0");
    pndRoot = xmlNewDocNode(pdocT, NULL, NAME_PIE, NULL);
    xmlDocSetRootElement(pdocT, pndRoot);

    parser = cmark_parser_new(options);

    cmark_parser_feed(parser, (const char *)pucT, strlen((const char*)pucT));

    document = cmark_parser_finish(parser);

    cmark_parser_free(parser);

    if (_cmark_render_pie(pdocT, document, options) == NULL) {
      printf("Error _cmark_render_pie()");
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    domPutDocString(stderr, BAD_CAST "parse result", pdocT);
    cmark_node_free(document);
    xmlFreeDoc(pdocT);
  }
  
  if (SKIPTEST) {
    cmark_node* document;
    xmlDocPtr pdocT = NULL;
    xmlNodePtr pndRoot;
    xmlChar* pucT = BAD_CAST"# TEST #\n\nThis is a Test!\n";
    int options = CMARK_OPT_DEFAULT | CMARK_OPT_SMART;

    i++;
    printf("TEST %i in '%s:%i': cmark parser = ", i, __FILE__, __LINE__);

    pdocT = xmlNewDoc(BAD_CAST "1.0");
    pndRoot = xmlNewDocNode(pdocT, NULL, NAME_PIE, NULL);
    xmlDocSetRootElement(pdocT, pndRoot);

    if ((document = cmark_parse_document((const char*)pucT, strlen((const char*)pucT), options)) == NULL) {
      printf("Error cmark_parse_document()");
    }
    else if (_cmark_render_pie(pdocT, document, options) == NULL) {
      printf("Error _cmark_render_pie()");
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    domPutDocString(stderr, BAD_CAST "parse result", pdocT);
    cmark_node_free(document);
    xmlFreeDoc(pdocT);
  }

  if (RUNTEST) {
    xmlDocPtr pdocT = NULL;
    xmlChar* pucT = BAD_CAST"# TEST #\n\nThis is a Test!\n";
    int options = CMARK_OPT_DEFAULT | CMARK_OPT_SMART;

    i++;
    printf("TEST %i in '%s:%i': cmark parser = ", i, __FILE__, __LINE__);

    if ((pdocT = cmark_markdown_to_pie((const char*)pucT, strlen((const char*)pucT), options)) == NULL) {
      printf("Error cmark_markdown_to_pie()");
    }
    else {
      n_ok++;
      printf("OK\n");
    }

    domPutDocString(stderr, BAD_CAST "parse result", pdocT);
    xmlFreeDoc(pdocT);
  }

  printf("Result in '%s': %i/%i OK\n\n", __FILE__, n_ok, i);

  return (i - n_ok);
} /* end of pieCmarkTest() */
