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

/* this module acts as a simple "Cache manager"

- reduce multiple XML parsings of static content

- store content without filesystem access (temporarily or in readonly mode)

*/


#ifdef HAVE_LIBPTHREAD
#include <pthread.h>
#elif defined(WITH_THREAD)
#endif

/*
*/
#include <libxml/parser.h>

/*
*/
#include "basics.h"
#include "utils.h"
#include <res_node/res_node_io.h>
#include <cxp/cxp.h>


/*!
\param pccArg -- pointer to context
\param fArg -- flag to enable or disable caching in this cxp context
\return TRUE if , FALSE in case of
*/
BOOL_T
cxpCtxtCacheEnable(cxpContextPtr pccArg, BOOL_T fArg)
{
  if (pccArg) {
    cxpCtxtLogPrint(pccArg, 2, (fArg ? "Enable caching" : "Disable caching"));
    pccArg->fCaching = fArg;
    return pccArg->fCaching;
  }
  return FALSE;
} /* end of cxpCtxtCacheEnable() */


/*! 

\param pccArg -- pointer to context
\param *prnArg -- context to cache
\return TRUE if successfully cached, FALSE else
*/
BOOL_T
cxpCtxtCacheAppendResNodeEat(cxpContextPtr pccArg, resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (pccArg == NULL) {
    resNodeFree(prnArg);
  }
  else if ( ! pccArg->fCaching) {
    resNodeFree(prnArg);
  }
  else if (prnArg == NULL) {
  }
  else {
    xmlChar *pucKey;
    resNodePtr prnI;

    if (resNodeGetNameAlias(prnArg)) {
      pucKey = resNodeGetNameAlias(prnArg);
    }
    else {
      pucKey = resNodeGetNameNormalized(prnArg);
    }
    
    cxpCtxtLogPrint(pccArg, 3, "Caching ressource node '%s'", (char *)pucKey);

    if (pccArg->prnCache == NULL) { /* prnArg will be the first node */
      pccArg->prnCache = prnArg;
      fResult = TRUE;
    }
    else if ((prnI = cxpCtxtCacheGetResNode(pccArg,pucKey)) == NULL) { /* there is no cache object with equal name */
      fResult = resNodeAddSibling(pccArg->prnCache, prnArg);
    }
    else { /* there is a cache object with equal name */
      fResult = resNodeSwapContent(prnI,prnArg);
      /*\todo reset counter etc */
      resNodeFree(prnArg);
    }
  }
  return fResult;
} /* end of cxpCtxtCacheAppendResNodeEat() */


/*! Copy a '\0'-terminated Buffer and append to cache list

\param pccArg -- pointer to context
\param *pucArg -- Buffer to cache
\param *pucArgName -- symbolic or file name for identification
\return TRUE if successfully cached, FALSE else
*/
BOOL_T
cxpCtxtCacheAppendBuffer(cxpContextPtr pccArg, xmlChar *pucArg, xmlChar *pucArgName)
{
  BOOL_T fResult = FALSE;

  if (pccArg != NULL && pccArg->fCaching == TRUE && STR_IS_NOT_EMPTY(pucArg) && STR_IS_NOT_EMPTY(pucArgName)) {
    xmlChar *pucNew;
    resNodePtr prnI;
    resNodePtr prnNew;

    cxpCtxtLogPrint(pccArg, 2, "Caching Buffer '%s'", (char *)pucArgName);

    if ((pucNew = xmlStrdup(pucArg)) == NULL) { /* copy the string */
    }
    else if ((prnI = cxpCtxtCacheGetResNode(pccArg,pucArgName)) != NULL) { /* check if there is a cache object with equal name */
      fResult = (resNodeSetContentPtr(prnI,(void *)pucNew,xmlStrlen(pucNew)) != NULL);
    }
    else if ((prnNew = resNodeAliasNew(pucArgName)) != NULL) { /* its an alias name (no file) */
      resNodeSetContentPtr(prnNew,(void *)pucNew,xmlStrlen(pucNew));
      fResult = cxpCtxtCacheAppendResNodeEat(pccArg,prnNew);
    }
    else {
      xmlFree(pucNew);
    }
  }
  return fResult;
} /* end of cxpCtxtCacheAppendBuffer() */


/*! Copy a DOM and append to cache list

\param pccArg -- pointer to context
\param pdocArg -- DOM to cache
\param *pucArgName -- symbolic or file name for identification
\return TRUE if successfully cached, FALSE else
*/
BOOL_T
cxpCtxtCacheAppendDoc(cxpContextPtr pccArg, xmlDocPtr pdocArg, xmlChar *pucArgName)
{
  BOOL_T fResult = FALSE;

  if (pccArg != NULL && pccArg->fCaching == TRUE
    && pdocArg != NULL && xmlDocGetRootElement(pdocArg) != NULL && STR_IS_NOT_EMPTY(pucArgName)) {
    xmlDocPtr pdocNew;
    resNodePtr prnI;
    resNodePtr prnNew;

    cxpCtxtLogPrint(pccArg, 2, "Caching DOM '%s'", (char *)pucArgName);

    if ((pdocNew = xmlCopyDoc(pdocArg, 1)) == NULL) { /* copy the DOM */
    }
    else if ((prnI = cxpCtxtCacheGetResNode(pccArg,pucArgName)) != NULL) { /* check if there is a cache object with equal name */
      fResult = (resNodeSetContentDocEat(prnI,pdocNew) != NULL);
    }
    else if ((prnNew = resNodeAliasNew(pucArgName)) != NULL) { /* its an alias name (no file) */
      resNodeSetContentDocEat(prnNew, pdocNew);
      fResult = cxpCtxtCacheAppendResNodeEat(pccArg,prnNew);
    }
    else {
      xmlFreeDoc(pdocNew);
    }
  }
  return fResult;
} /* end of cxpCtxtCacheAppendDoc() */


/*! 
\param pccArg -- pointer to context
\param *pucArgName -- symbolic or file name for identification
\return pointer to cached Buffer
*/
xmlChar *
cxpCtxtCacheGetBuffer(cxpContextPtr pccArg, xmlChar *pucArgName)
{
  xmlChar *pucResult = NULL;

  if (pccArg != NULL && STR_IS_NOT_EMPTY(pucArgName)) {
    resNodePtr prnFound;

    prnFound = cxpCtxtCacheGetResNode(pccArg, pucArgName);
    if (prnFound != NULL) {
      cxpCtxtLogPrint(pccArg, 3, "Found a cached Buffer of '%s'",
	(resNodeGetNameAlias(prnFound) ? resNodeGetNameAlias(prnFound) : resNodeGetNameNormalized(prnFound)));
      pucResult = BAD_CAST resNodeGetContentPtr(prnFound);
    }
  }
  return pucResult;
} /* end of cxpCtxtCacheGetBuffer() */


/*! 
\param pccArg -- pointer to context
\param *pucArgName -- symbolic or file name for identification
\return pointer to cached DOM
*/
xmlDocPtr
cxpCtxtCacheGetDoc(cxpContextPtr pccArg, xmlChar *pucArgName)
{
  xmlDocPtr pdocResult = NULL;

  if (pccArg != NULL && STR_IS_NOT_EMPTY(pucArgName)) {
    resNodePtr prnFound;

    prnFound = cxpCtxtCacheGetResNode(pccArg, pucArgName);
    if (prnFound != NULL) {
      cxpCtxtLogPrint(pccArg, 3, "Found a cached DOM of '%s'",
	(resNodeGetNameAlias(prnFound) ? resNodeGetNameAlias(prnFound) : resNodeGetNameNormalized(prnFound)));
      pdocResult = resNodeGetContentDoc(prnFound);
    }
  }
  return pdocResult;
} /* end of cxpCtxtCacheGetDoc() */


/*! 
\param pccArg -- pointer to context
\param *pucArgName -- symbolic or file name for identification
\return pointer to cached resNode
*/
resNodePtr
cxpCtxtCacheGetResNode(cxpContextPtr pccArg, xmlChar *pucArgName)
{
  resNodePtr prnResult = NULL;

  if (pccArg != NULL && STR_IS_NOT_EMPTY(pucArgName)) {
    resNodePtr prnT;
    resNodePtr prnI;
    cxpContextPtr pccT;

    /* check if there is a cache object with equal name */
    prnT = resNodeConcatNew(cxpCtxtLocationGetStr(pccArg), pucArgName);

    for (pccT = pccArg; prnResult == NULL && pccT != NULL; pccT = pccT->parent) { /*!\todo all cxp contexts */
      /* find according cache element in list */
      for (prnI = pccT->prnCache; prnResult == NULL && prnI != NULL; prnI = resNodeGetNext(prnI)) {
	if (resPathIsEquivalent(pucArgName, resNodeGetNameAlias(prnI))
	    || resPathIsEquivalent(resNodeGetNameBase(prnT), resNodeGetNameBase(prnI))
	    || resPathIsEquivalent(resNodeGetNameNormalized(prnT), resNodeGetNameNormalized(prnI))) {
	  prnResult = prnI;
	}
      }
    }

    resNodeFree(prnT);
  }
  return prnResult;
} /* end of cxpCtxtCacheGetResNode() */


/*! cxp Ctxt Cache GetNew Buffer

\param pccArg -- pointer to context
\param *pucArgName -- symbolic or file name for identification
\return 
*/
BOOL_T
cxpCtxtCacheUnlink(cxpContextPtr pccArg, xmlChar *pucArgName)
{
  BOOL_T fResult = FALSE;

  if (pccArg != NULL && STR_IS_NOT_EMPTY(pucArgName)) {
    resNodePtr prnFound;

    prnFound = cxpCtxtCacheGetResNode(pccArg, pucArgName);
    if (prnFound != NULL) {
      cxpCtxtLogPrint(pccArg, 3, "Found a cached Buffer of '%s'",
	(resNodeGetNameAlias(prnFound) ? resNodeGetNameAlias(prnFound) : resNodeGetNameNormalized(prnFound)));
      resNodeListUnlink(prnFound);
      resNodeFree(prnFound);
      fResult = TRUE;
    }
  }
  return fResult;
} /* end of cxpCtxtCacheUnlink() */


/*! cxp Ctxt Cache Print

\param pccArg -- pointer to context
*/
void
cxpCtxtCachePrint(cxpContextPtr pccArg)
{
  if (pccArg) {
    cxpCtxtLogPrint(pccArg, 2, "Stat cached DOMs and Strings");

    if (pccArg->prnCache != NULL) {
      int i;
      resNodePtr prnI;

      for (i = 0, prnI = pccArg->prnCache; prnI; prnI = resNodeGetNext(prnI), i++) {
	cxpCtxtLogPrint(pccArg, 2,
	  "%3i. %i x %7s '%s'",
	  i,
	  resNodeGetUsageCount(prnI),
	  (resNodeGetContentDoc(prnI) ? "DOM" : resNodeGetContentPtr(prnI) ? "String" : "empty"),
	  (resNodeGetNameAlias(prnI) ? resNodeGetNameAlias(prnI) : resNodeGetNameNormalized(prnI)));
      }
    }

    if ( ! pccArg->fCaching) {
      cxpCtxtLogPrint(pccArg, 2, "Caching is currently disabled");
    }
  }
} /* end of cxpCtxtCachePrint() */


#ifdef TESTCODE
#include "test/test_cxp_context_cache.c"
#endif
