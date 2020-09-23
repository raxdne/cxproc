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

#include <libxml/globals.h>

#include "basics.h"
#include "utils.h"
#include <res_node/res_node.h>


/*! Sets the file property of this context.

\param prnArg the context
\return pointer to new property value
*/
xmlChar *
resNodeSetProp(resNodePtr prnArg, xmlChar *pucArgKey, xmlChar *pucArgValue)
{
  xmlChar *pucResult = NULL;

  if (prnArg != NULL && STR_IS_NOT_EMPTY(pucArgKey) && STR_IS_NOT_EMPTY(pucArgValue)) {

    if (prnArg->ppucProp == NULL) { /* no properties yet */
      prnArg->ppucProp = (xmlChar **)xmlMalloc(3 * sizeof(xmlChar *));
      if (prnArg->ppucProp) {
	prnArg->ppucProp[0] = xmlStrdup(pucArgKey);
	prnArg->ppucProp[1] = xmlStrdup(pucArgValue);
	prnArg->ppucProp[2] = NULL;
	pucResult = prnArg->ppucProp[1];
      }
      else {
      }
    }
    else {
      size_t i;

      for (i=0; ; i += 2) {
	if (prnArg->ppucProp[i] == NULL) {
	  /* end marker reached, add a new pair */
	  size_t j;
	  xmlChar **ppucT;

	  ppucT = (xmlChar **)xmlMalloc((i * 2 + 1) * sizeof(xmlChar *));
	  if (ppucT) {
	    for (j=0; j<i; j += 2) { /* copy existing pointers */
	      ppucT[j] = prnArg->ppucProp[j];
	      ppucT[j+1] = prnArg->ppucProp[j+1];
	    }
	    ppucT[j] = xmlStrdup(pucArgKey);
	    ppucT[j+1] = xmlStrdup(pucArgValue);
	    ppucT[j+2] = NULL;

	    xmlFree(prnArg->ppucProp);
	    prnArg->ppucProp = ppucT;
	    pucResult = prnArg->ppucProp[j+1];
	  }
	  else {
	  }
	  break;
	}
	else if (xmlStrEqual(prnArg->ppucProp[i], pucArgKey)) {
	  /* key exists already, replace value */
	  xmlFree(prnArg->ppucProp[i+1]);
	  prnArg->ppucProp[i+1] = xmlStrdup(pucArgValue);
	  pucResult = prnArg->ppucProp[i+1];
	  break;
	}
      }
    }
  }
  return pucResult;
}
/* end of resNodeSetProp() */


/*! \return a pointer to the extension string of context, derived from basename
*/
xmlChar *
resNodeGetProp(resNodePtr prnArg, xmlChar *pucArgKey)
{
  if (prnArg != NULL && prnArg->ppucProp != NULL && STR_IS_NOT_EMPTY(pucArgKey)) {
    size_t i;

    for (i=0; prnArg->ppucProp[i]; i += 2) {
      if (xmlStrEqual(prnArg->ppucProp[i], pucArgKey)) {
	return prnArg->ppucProp[i+1];
      }
    }
  }

  return NULL;
}
/* end of resNodeGetProp() */



#ifdef TESTCODE
#include "test/test_res_node_properties.c"
#endif

