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


/*! cxp ThreadPool Test  

\param pccArg -- pointer to context to duplicate
\return return count of passed tests
*/
int
cxpThreadPoolTest(cxpContextPtr pccArg)
{
  int n_ok;
  int i;

  n_ok = 0;
  i = 0;


  if (RUNTEST) {
    cxpThreadPoolPtr pcxpThreadPool = NULL;
    cxpContextPtr pccT;
    cxpContextPtr pccTT;

    i++;
    printf("TEST %i in '%s:%i': cxpThreadPoolNew() = ", i, __FILE__, __LINE__);

    pccT = cxpCtxtDup(pccArg);
    pccTT = cxpCtxtDup(pccT);
    if (pccT != NULL && pccTT != NULL) {
      xmlNodePtr pndXml;
      xmlNodePtr pndChild;

      pndXml = xmlNewNode(NULL,NAME_XML);
      xmlSetProp(pndXml,BAD_CAST"name",BAD_CAST"-");
      xmlNewChild(pndXml,NULL,NAME_INFO,NULL);
      cxpCtxtProcessSetNodeCopy(pccT,pndXml);
      xmlFreeNode(pndXml);

      pndXml = xmlNewNode(NULL,NAME_XML);
      xmlSetProp(pndXml,BAD_CAST"name",BAD_CAST"-");
      pndChild = xmlNewChild(pndXml,NULL,NAME_XML,NULL);
      xmlSetProp(pndChild,BAD_CAST"name",BAD_CAST TESTPREFIX "xml/baustelle.pie");
      cxpCtxtProcessSetNodeCopy(pccTT,pndXml);
      xmlFreeNode(pndXml);

      pcxpThreadPool = cxpThreadPoolNew();
      if (pcxpThreadPool) {
	if (cxpThreadPoolAppend(pcxpThreadPool,pccT) && cxpThreadPoolAppend(pcxpThreadPool,pccTT)) {
	  cxpThreadPoolJoin(pcxpThreadPool);
	  n_ok++;
	  printf("OK\n");
	}
	else {
	  printf("Error cxpThreadPoolAppend()\n");
	}
      }
      else {
	printf("Error cxpThreadPoolNew()\n");
      }
    }
  }


  printf("Result in '%s': %i/%i OK\n\n", __FILE__, n_ok, i);

  return (i - n_ok);
}
/* end of cxpThreadPoolTest() */

