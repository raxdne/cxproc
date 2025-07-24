/*
  cxproc - Configurable Xml PROCessor

  Copyright (C) 2006..2024 by Alexander Tenbusch

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

/* Build a List of resNode's (result of ZIP file parsing, list complete directory, ...)

 s. http://openbook.galileocomputing.de/c_von_a_bis_z/017_c_dateien_verzeichnisse_003.htm
    http://msdn.microsoft.com/en-us/library/windows/desktop/aa365522(v=vs.85).aspx
 */

#ifdef HAVE_LIBCURL
#include <curl/curl.h>
#endif

#include <libxml/tree.h>

#include "basics.h"
#include "utils.h"
#include <res_node/res_node_ops.h>
#include "dom.h"
#include <cxp/cxp_dtd.h>


/*! split a prnArg into its single ancestor resource nodes

\param prnArg a pointer to a resource node without a child node
\return TRUE as splitting result
*/
resNodePtr
resNodeListPathNew(xmlChar* pucArgPath)
{
  resNodePtr prnResult = NULL;

  if (STR_IS_NOT_EMPTY(pucArgPath)) { /* a trailing separator indicates a directory */
    xmlChar* pucSep;
    xmlChar* pucStart;
    xmlChar* pucT;
    xmlChar* pucTT;
    resNodePtr prnT = NULL;

    if ((pucSep = resPathGetNextPathPtr(pucArgPath)) == NULL) {
      pucSep = pucArgPath + xmlStrlen(pucArgPath);
    }
    pucTT = pucArgPath;
    
    do {
      if (pucSep - pucTT < 2) {
	/* ignore empty path */
      }
      else if ((pucT = xmlStrndup(pucTT, (int)(pucSep - pucTT)))) {

	if (resPathIsURL(pucT)) {
#ifdef HAVE_LIBCURL
	  prnT = resNodeCurlNew(pucT);
#else
	  prnT = resNodeDirNew(pucT);
#endif
	}
	else if (resPathIsAbsolute(pucT)) {
	  prnT = resNodeDirNew(pucT);
	}
	else if (resPathIsRelative(pucT)) {
	  prnT = resNodeDirNew(pucT);
	}
	else if ((prnT = resNodeNew())) {
	  resNodeReset(prnT,pucT);
	}
	else {
	  assert(TRUE);
	}
	resNodeSetType(prnResult, rn_type_root);

	if (prnResult) {
	  prnResult = prnT;
	}
	else {
	  resNodeAddChild(prnT, prnT);
	}
	  
	xmlFree(pucT);
      }
      else {
	assert(TRUE);
      }

      pucTT = pucSep;
      
    } while ((pucSep = resPathGetNextPathPtr(pucSep)) != NULL);

    
    prnResult = resNodeNew();
    if (prnResult) {

      pucTT = pucStart = resPathCollapseStr(pucArgPath, FS_PATH_FULL);

      if (resPathIsAbsolute(pucArgPath)) {
	if (resPathIsDosDrive(pucTT)) {
	  /* skip leading drive char */
	  pucTT += 2;
	}

	while (STR_IS_NOT_EMPTY(pucTT) && issep(*pucTT)) { /* ignore leading slash */
	  pucTT++;
	}

	pucT = xmlStrndup(pucStart, (int)(pucTT - pucStart)); /* root */
	resNodeReset(prnResult, pucT);
	resNodeSetType(prnResult, rn_type_root);
	xmlFree(pucT);
      }
      else if ((pucTT = resPathGetNextSeparatorPtr(pucStart))) {

	pucT = xmlStrndup(pucStart, (int)(pucTT - pucStart));
	resNodeSetNameBase(prnResult, pucT);
	xmlFree(pucT);

	while (STR_IS_NOT_EMPTY(pucTT) && issep(*pucTT)) { /* ignore slash */
	  pucTT++;
	  resNodeSetType(prnResult, rn_type_dir);
	}
      }
      else {
	resNodeSetNameBase(prnResult, pucStart);
	resNodeSetType(prnResult, rn_type_file); /* default */
      }

      if (xmlStrlen(pucTT) > 0) {
	prnT = resNodeAddChildNew(prnT, pucTT);
      }

      xmlFree(pucStart);
    }
  }
  return prnResult;
} /* end of resNodeListPathNew() */


/*! Resource Node List Parse  list different types of contexts

\param prnArg -- new resNode to parse (directory, share, archive, file)
\param iArgDepth -- integer value for recursion
\param re_match -- regexp

\return TRUE if successful, FALSE in case of errors
*/
BOOL_T
resNodeListParse(resNodePtr prnArg, int iArgDepth, const pcre2_code *re_match)
{
  BOOL_T fResult = FALSE;

  if (iArgDepth > -1) { /* recursion depth reached? */
    resNodePtr prnI;

    if (resNodeReadStatus(prnArg) == FALSE) {
      /* error */
    }
    else if (resNodeIsDir(prnArg)) {
      /*  */
      if (iArgDepth > 0 && resNodeDirAppendEntries(prnArg, re_match)) {
	for (prnI = resNodeGetChild(prnArg); prnI; prnI = resNodeGetNext(prnI)) {
	  if (resNodeReadStatus(prnI)) {
	    if (iArgDepth > 1 && (resNodeIsDir(prnI) || resNodeIsArchive(prnI))) {
	      if (resNodeListParse(prnI, iArgDepth - 1, re_match)) { /* recursion */
	      }
	    }
	  }
	}
      }
      fResult = TRUE;
    }
#ifdef HAVE_LIBARCHIVE
    else if (resNodeIsArchive(prnArg)) {
      if (iArgDepth > 1) {
	fResult = arcAppendEntries(prnArg, re_match, FALSE);
      }
    }
    else if (resNodeIsFileInArchive(prnArg)) {
      /*  */
    }
#endif
    else if (resNodeIsURL(prnArg)) {
      /*  */
      fResult = TRUE;
    }
    else if (resNodeIsLink(prnArg)) {
      /*  */
      fResult = (resNodeResolveLinkChildNew(prnArg) != NULL);
    }
    else if (resNodeIsFile(prnArg)) {
      /*  */
      fResult = resNodeReadStatus(prnArg);
    }
    else {
    }

    /*! list different types of contexts */
  }
  return fResult;
} /* end of resNodeListParse() */


/*! frees tree of prnArg

  \param prnArg a pointer to a resource node tree to free
 */
void
resNodeListFree(resNodePtr prnArg)
{
  if (prnArg) {
    resNodePtr prnRelease;
    resNodePtr prnReleaseNext;

    for (prnRelease = prnArg; prnRelease != NULL; prnRelease = prnReleaseNext) {
      prnReleaseNext = resNodeGetNext(prnRelease);
      resNodeFree(prnRelease);
    }
  }
} /* end of resNodeListFree() */


/*! remove link from its parent resource node

\param prnArg -- resNode to unlink from list
\return TRUE if successful, FALSE in case of errors
*/
BOOL_T
resNodeListUnlink(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (prnArg) {
    /*! remove link from parent resource node */
    if (prnArg->parent != NULL) {
      if (prnArg->parent->children == prnArg) {
	prnArg->parent->children = prnArg->next;
      }
      if (prnArg->parent->last == prnArg) {
	prnArg->parent->last = prnArg->prev;
      }
      prnArg->parent = NULL;
    }

    /*! remove link from sibling resource nodes */
    if (prnArg->next != NULL) {
      prnArg->next->prev = prnArg->prev;
    }
    
    if (prnArg->prev != NULL) {
      prnArg->prev->next = prnArg->next;
    }

    prnArg->prev = NULL;
    prnArg->next = NULL;

    fResult = TRUE;
  }
  return fResult;
} /* end of resNodeListUnlink() */


/*! remove link from its parent resource node

\param prnArg -- resNode to unlink from list
\return TRUE if successful, FALSE in case of errors
*/
BOOL_T
resNodeListUnlinkDescendants(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (prnArg) {
    /*! remove link to and from child resource nodes */
    resNodePtr prnI;

    for (prnI = resNodeGetChild(prnArg); prnI != NULL; prnI = resNodeGetNext(prnI)) {
      prnI->parent = NULL;
    }
    
    prnArg->children = NULL;
    prnArg->last = NULL;
    
    fResult = TRUE;
  }
  return fResult;
} /* end of resNodeListUnlinkDescendants() */


/*! find child resNode 

\todo find resNode using pattern matching??

\param prnArg -- tree of resNodes
\param pucName -- pointer to path matching pattern
\return pointer to next child resNode or NULL if failed
*/
resNodePtr
resNodeListFindChild(resNodePtr prnArg, xmlChar *pucName)
{
  resNodePtr prnResult = NULL;

  if (prnArg != NULL && STR_IS_NOT_EMPTY(pucName)) {
    resNodePtr prnI;

    for (prnI = resNodeGetChild(prnArg); prnResult == NULL && prnI != NULL; prnI = resNodeGetNext(prnI)) {
      if (resPathIsEquivalent(resNodeGetNameBase(prnI), pucName)) {
	prnResult = prnI;
      }
    }
  }
  return prnResult;
} /* end of resNodeListFindChild() */


/*! finds first self or descendant node according to pucArgPath, list directories and archives on-demand only!

\param prnArg start context
\param pucArgPath pointer to a locator
\param iArgOptions bits for options 
\return a pointer to an according 'resNode' or NULL else
*/
resNodePtr
resNodeListFindPath(resNodePtr prnArg, xmlChar *pucArgPath, int iArgOptions)
{
  resNodePtr prnResult = NULL;

  if (prnArg && STR_IS_NOT_EMPTY(pucArgPath)) {
    xmlChar *pucT;
    resNodePtr prnI;

#ifdef HAVE_PCRE2
    pcre2_match_data *match_data = NULL;
    pcre2_code *re_match = NULL;

    if (iArgOptions & RN_FIND_REGEXP) {
      size_t erroroffset;
      int errornumber;
      int opt_match_pcre = PCRE2_UTF | PCRE2_CASELESS;

      re_match = pcre2_compile(
			       (PCRE2_SPTR8)pucArgPath, /* the pattern */
			       PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
			       opt_match_pcre,         /* default options */
			       &errornumber,          /* for error number */
			       &erroroffset,          /* for error offset */
			       NULL);                 /* use default compile context */

      if (re_match) {
	match_data = pcre2_match_data_create_from_pattern(re_match, NULL);
      }
      else {
	PrintFormatLog(1, "error regexp: '%s'", pucArgPath);
	return prnResult;
      }
    }
#endif

    if (resPathIsAbsolute(pucArgPath)) {
      xmlChar* pucTT;

      pucTT = resPathCollapseStr(pucArgPath, FS_PATH_FULL);
      pucT = resPathDiffPtr(resNodeGetNameNormalized(prnArg), pucTT);
      if (STR_IS_NOT_EMPTY(pucT)) {
	while (issep(*pucT)) pucT++;
	prnResult = resNodeListFindPath(prnArg, pucT, iArgOptions);
      }
      xmlFree(pucTT);
    }
#ifdef EXPERIMENTAL
    else if ((iArgOptions & RN_FIND_IN_TEMP) == 0 && resPathIsTemp(resNodeGetNameNormalized(prnArg))) {
      /* ignore this node and its childs */
    }
#endif
    else if (resNodeIsHidden(prnArg)) {
      /* ignore this node and its childs */
    }
    else if (
#ifdef HAVE_PCRE2
      ((iArgOptions & RN_FIND_REGEXP)
	      &&
	      pcre2_match(re_match,
			  (PCRE2_SPTR8)resNodeGetNameNormalized(prnArg),
			  strlen((const char*)resNodeGetNameNormalized(prnArg)),
			  0,
			  0,
			  match_data,
			  NULL) > -1)
	     ||
#endif
	     resPathIsMatchingEnd(resNodeGetNameNormalized(prnArg), pucArgPath)) { /* match or not? */
      if (((iArgOptions & RN_FIND_DIR) && resNodeGetType(prnArg) == rn_type_dir)
          ||
          ((iArgOptions & RN_FIND_IN_ARCHIVE) && resNodeGetType(prnArg) == rn_type_dir_in_archive)
	  ||
	  ((iArgOptions & RN_FIND_FILE) && (resNodeGetType(prnArg) == rn_type_file || resNodeGetType(prnArg) == rn_type_archive))
	  ||
	  ((iArgOptions & RN_FIND_IN_ARCHIVE) && resNodeGetType(prnArg) == rn_type_file_in_archive)) {
	prnResult = prnArg;
      }
    }
    else if (prnResult == NULL && resNodeGetChild(prnArg) != NULL) { /* there are childs already */
      for (prnI = resNodeGetChild(prnArg); prnI != NULL && prnResult == NULL; prnI = resNodeGetNext(prnI)) {
	prnResult = resNodeListFindPath(prnI, pucArgPath, iArgOptions);
      }
    }
    else if (prnResult == NULL && (iArgOptions & RN_FIND_IN_SUBDIR) && resNodeGetType(prnArg) == rn_type_dir) {
      if (resNodeDirAppendEntries(prnArg, NULL)) { /* there are no childs, append list of sub-directories and files */
	for (prnI = resNodeGetChild(prnArg); prnI != NULL && prnResult == NULL; prnI = resNodeGetNext(prnI)) {
	  prnResult = resNodeListFindPath(prnI, pucArgPath, iArgOptions);
	}
      }
    }
#if 0
    else if (prnResult == NULL && (iArgOptions & RN_FIND_IN_ARCHIVE) && resNodeIsArchive(prnArg)) {
      if (arcAppendEntries(prnArg, NULL, FALSE)) { /* there are no childs, append list of archive entries */
	for (prnI = resNodeGetChild(prnArg); prnI != NULL && prnResult == NULL; prnI = resNodeGetNext(prnI)) {
	  prnResult = resNodeListFindPath(prnI, pucArgPath, iArgOptions);
	}
      }
    }
#endif
    
    if (prnResult == NULL && prnArg->parent == NULL && (iArgOptions & RN_FIND_NEXT)) {
      for (prnI = resNodeGetNext(prnArg); prnI != NULL && prnResult == NULL; prnI = resNodeGetNext(prnI)) {
	prnResult = resNodeListFindPath(prnI, pucArgPath, iArgOptions & ~RN_FIND_NEXT);
      }
    }

#ifdef HAVE_PCRE2
    if (re_match) {
      pcre2_match_data_free(match_data);   /* Release memory used for the match */
      pcre2_code_free(re_match);
    }    
#endif
  }
  return prnResult;
} /* end of resNodeListFindPath() */


/*! finds the next node according to pucArgPath, list directories and archives on-demand only!

\param prnArg start context
\param pucArgPath pointer to a locator
\param iArgOptions bits for options 
\return a pointer to an according 'resNode' or NULL else
*/
resNodePtr
resNodeListFindPathNext(resNodePtr prnArg, xmlChar *pucArgPath, int iArgOptions)
{
  resNodePtr prnResult = NULL;

#ifdef DEBUG
  PrintFormatLog(4, "resNodeListFindPathNext('%s','%s')", resNodeGetNameNormalized(prnArg), pucArgPath);
#endif

  if (prnArg) {
    resNodePtr prnA;
    resNodePtr prnI;
    
    if (prnResult == NULL && resNodeGetChild(prnArg) != NULL) { /* there are childs already */
      prnResult = resNodeListFindPath(resNodeGetChild(prnArg), pucArgPath, iArgOptions);
    }

    if (prnResult == NULL && resNodeGetNext(prnArg) != NULL) { /* there are nexts already */
      prnResult = resNodeListFindPath(resNodeGetNext(prnArg), pucArgPath, iArgOptions);
    }      

    for (prnA = resNodeGetParent(prnArg); prnA != NULL && prnResult == NULL; prnA = resNodeGetParent(prnA)) {
      for (prnI = resNodeGetNext(prnA); prnI != NULL && prnResult == NULL; prnI = resNodeGetNext(prnI)) {
	prnResult = resNodeListFindPath(prnI, pucArgPath, iArgOptions);
      }
    }      
  }
  return prnResult;
} /* end of resNodeListFindPathNext() */


#ifdef TESTCODE
#include "test/test_res_node_list.c"
#endif

