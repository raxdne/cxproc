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
pieCmarkTest(void)
{
  int n_ok;
  int i;

  n_ok=0;
  i=0;

#ifdef WITH_MARKDOWN

  if (RUNTEST) {
    xmlNodePtr pndPie;

    i++;
    printf("TEST %i in '%s:%i': parse empty markdown text = ", i, __FILE__, __LINE__);

    if ((pndPie = xmlNewNode(NULL, NAME_PIE_PIE)) == NULL) {
      printf("Error xmlNewNode()\n");
    }
    else if (ParseMarkdownBuffer(pndPie, NULL) != NULL) {
      printf("Error 1 ParseMarkdownBuffer()\n");
    }
    else if (ParseMarkdownBuffer(pndPie, BAD_CAST"") != NULL) {
      printf("Error 2 ParseMarkdownBuffer()\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    //domPutNodeString(stderr, BAD_CAST"import result", pndPie);
    xmlFreeNode(pndPie);
  }


  if (RUNTEST) {
    xmlNodePtr pndPie;
    xmlNodePtr pndBlock;
    xmlChar* pucContent = BAD_CAST
      "# ABCDE #\n"
      "## FGHI ##\n"
      "### JKLM\n"
      "AAA BBB\n\n"
      "- 1\n"
      "- 2\n"
      "- 3\n\n"
      "CCC  \n\r\n"
      /* "fig. abc.png: picture\n\n" */
      "NPQR\r\n"
      "====\n"
      "1) A\n"
      "   1) A.1\n"
      "   1) A.2\n"
      "2) B\n"
      "3) C\n\n"
      "##### STUV\n\r\n"
      "TODO: task markup\n\n"
      "WXYZ\n\n"
      "    QQQ\n"
      "    QQQ\n"
      "    QQQ\n"
      "WXYZ `edoc` post\n\n"
      "WXYZ __hpme__ post\n\n"
      "WXYZ **gnorts** post\n\n"
      "WXYZ [knil](https://) post\n\n"
      "~~~ WXYZ post\n"
      "jsflfkjsl\n"
      "~~~\n"
      //"----"
      ;

    i++;
    printf("TEST %i in '%s:%i': parse markdown text and build list of import elements = ", i, __FILE__, __LINE__);

    if ((pndPie = xmlNewNode(NULL, NAME_PIE_PIE)) == NULL) {
      printf("Error xmlNewNode()\n");
    }
    else if (ParseMarkdownBuffer(pndPie, pucContent) == NULL) {
      printf("Error 1 ParseMarkdownBuffer()\n");
    }
    else if ((pndBlock = pndPie->children) == NULL || IS_NODE_PIE_BLOCK(pndBlock) == FALSE) {
      printf("Error 2 ParseMarkdownBuffer()\n");
    }
#if 0 
    else if (domNumberOfChild(pndBlock, NAME_PIE_SECTION) != 2) {
      printf("Error 3 ParseMarkdownBuffer()\n");
    }
#endif
    else {
      n_ok++;
      printf("OK\n");
    }
#ifdef DEBUG
    //puts(pucContent);
    //domPutNodeString(stderr, BAD_CAST"import result", pndPie);
#endif
    
    xmlFreeNode(pndPie);
  }

#endif

  printf("Result in '%s': %i/%i OK\n\n", __FILE__, n_ok, i);

  return (i - n_ok);
} /* end of pieCmarkTest() */
