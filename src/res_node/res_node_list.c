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

/* Build a List of resNode's (result of ZIP file parsing, list complete directory, ...)

 s. http://openbook.galileocomputing.de/c_von_a_bis_z/017_c_dateien_verzeichnisse_003.htm
    http://msdn.microsoft.com/en-us/library/windows/desktop/aa365522(v=vs.85).aspx
 */

#ifdef HAVE_LIBCURL
#include <curl/curl.h>
#endif

#include <libxml/uri.h>
#include <libxml/nanohttp.h>
#include <libxml/tree.h>

#include "basics.h"
#include "utils.h"
#include <res_node/res_node_ops.h>
#include <cxp/cxp.h>
#include "dom.h"
#include <cxp/cxp_dir.h>

#ifdef HAVE_LIBARCHIVE
#include <archive/archive.h>
#endif


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
    resNodePtr prnEntry;

    if (resNodeReadStatus(prnArg) == FALSE) {
      /* error */
    }
    else if (resNodeIsDir(prnArg)) {
      /*  */
      if (iArgDepth > 0 && resNodeDirAppendEntries(prnArg, re_match)) {
	for (prnEntry = resNodeGetChild(prnArg); prnEntry; prnEntry = resNodeGetNext(prnEntry)) {
	  if (resNodeReadStatus(prnEntry)) {
	    if (iArgDepth > 1 && (resNodeIsDir(prnEntry) || resNodeIsArchive(prnEntry))) {
	      if (resNodeListParse(prnEntry, iArgDepth - 1, re_match)) { /* recursion */
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
      prnArg->next = NULL;
    }
    
    if (prnArg->prev != NULL) {
      prnArg->prev->next = prnArg->next;
      prnArg->prev = NULL;
    }
    
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


/*! find resNode using pattern matching

\deprecated succeeded by resNodeListFindPath()

\todo use as iterator to find all Resource Nodes

\param prnArg -- tree of resNodes
\param pucArgPath
\param *pucPattern -- pointer to path matching pattern
\return pointer to next found resNode or NULL if failed
*/
resNodePtr
resNodeListFind(resNodePtr prnArg, xmlChar *pucArgPath, xmlChar *pucPattern)
{
  resNodePtr prnResult = NULL;

  if (prnArg != NULL && STR_IS_NOT_EMPTY(pucPattern)) {
      resNodePtr prnI;

    /*!\todo find resNode using pattern matching */

    /*!\todo include next occurance, return a list of resNodePtr??? */

    /*! compare siblings */
    prnI = prnArg;
    do {
      if (resPathIsEquivalent(resNodeGetNameBase(prnI), pucPattern)
	&& (pucArgPath == NULL || resPathIsMatchingEnd(resNodeGetNameBaseDir(prnI), pucArgPath))) {
	prnResult = prnI;
      }
      else {
	prnResult = resNodeListFind(resNodeGetChild(prnI), pucArgPath, pucPattern);
      }
    } while (prnResult == NULL && (prnI = resNodeGetNext(prnI)) != NULL);


#if 0
    resNodePtr prnChildFirst;

    if (resPathIsEquivalent(resNodeGetNameBase(prnArg), pucPattern)
	&& (pucArgPath == NULL || resPathIsMatchingEnd(resNodeGetNameBaseDir(prnArg), pucArgPath))) {
      prnResult = prnArg;
    }

    if (prnResult == NULL && (prnChildFirst = resNodeGetChild(prnArg)) != NULL) {
      resNodePtr prnNext;

      prnResult = resNodeListFind(prnChildFirst, pucArgPath, pucPattern);

      for (prnNext = resNodeGetNext(prnChildFirst); prnResult == NULL && prnNext != NULL; prnNext = resNodeGetNext(prnNext)) {
	prnResult = resNodeListFind(prnNext, pucArgPath, pucPattern);
      }
    }
#endif
  }

  return prnResult;
} /* end of resNodeListFind() */


/*! finds first self or descendant node according to pucArgPath, list directories and archives on-demand only!

\param prnArg start context
\param pucArgPath pointer to a locator
\return a pointer to an according 'resNode' or NULL else
*/
resNodePtr
resNodeListFindPath(resNodePtr prnArg, xmlChar *pucArgPath, int iArgOptions)
{
  resNodePtr prnResult = NULL;

  if (prnArg) {
    xmlChar *pucT;
    resNodePtr prnI;

#ifdef DEBUG
    PrintFormatLog(4, "resNodeListFindPath('%s','%s')", resNodeGetNameNormalized(prnArg), pucArgPath);
#endif

    if (resPathIsAbsolute(pucArgPath)) {
      xmlChar* pucTT;

      pucTT = resPathCollapse(pucArgPath, FS_PATH_FULL);
      pucT = resPathDiffPtr(resNodeGetNameNormalized(prnArg), pucTT);
      if (STR_IS_NOT_EMPTY(pucT)) {
	while (issep(*pucT)) pucT++;
	prnResult = resNodeListFindPath(prnArg, pucT, iArgOptions);
      }
      xmlFree(pucTT);
    }
    else if (resNodeIsHidden(prnArg)) {
      /* ignore this node and its childs */
    }
    else if (resPathIsMatchingEnd(resNodeGetNameNormalized(prnArg), pucArgPath)) { /* match or not? */
      if (((iArgOptions & RN_FIND_DIR)
	&& (resNodeGetType(prnArg) == rn_type_dir
	  || ((iArgOptions & RN_FIND_IN_ARCHIVE) && (resNodeGetType(prnArg) == rn_type_dir_in_archive))))
	|| 
	((iArgOptions & RN_FIND_FILE)
	  && (resNodeGetType(prnArg) == rn_type_file
	  || resNodeGetType(prnArg) == rn_type_archive)
	  || ((iArgOptions & RN_FIND_IN_ARCHIVE) && (resNodeGetType(prnArg) == rn_type_file_in_archive)))) {
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
#ifdef HAVE_LIBARCHIVE
    else if (prnResult == NULL && (iArgOptions & RN_FIND_IN_ARCHIVE) && resNodeIsArchive(prnArg)) {
      if (arcAppendEntries(prnArg, NULL, FALSE)) { /* there are no childs, append list of archive entries */
	for (prnI = resNodeGetChild(prnArg); prnI != NULL && prnResult == NULL; prnI = resNodeGetNext(prnI)) {
	  prnResult = resNodeListFindPath(prnI, pucArgPath, iArgOptions);
	}
      }
    }
#endif
  }
  return prnResult;
} /* end of resNodeListFindPath() */


/*! finds the next node according to pucArgPath, list directories and archives on-demand only!

\param prnArg start context
\param pucArgPath pointer to a locator
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

#if 0

/*! Creates a new index node according 'pucArgPath'

\param pucArgPath pointer to filesystem path
\return a freshly allocated 'resNode'
*/
resNodePtr
_resNodeIndexNew(xmlChar *pucArgPathDir)
{
  resNodePtr prnResult = NULL;

  if (STR_IS_NOT_EMPTY(pucArgPathDir) && resPathIsDir(pucArgPathDir)) {
    prnResult = resNodeDirNew(pucArgPathDir);
    if (prnResult) {
      xmlChar *pucT;
      xmlChar *pucTT;

      if (resNodeIsDir(prnResult)) {
	if (resNodeUpdate(prnResult, RN_INFO_CONTENT, NULL, NULL) == FALSE) {
	  printf("Error resNodeUpdate()\n");
	}
	resNodeSetType(prnResult,rn_type_index);
      }
#if 0
      if (resNodeIsDir(prnResult)) {

	resNodeConcat(prnResult,NAME_FILE_INDEX);
	resNodeReadStatus(prnResult);
	if (resNodeIsDir(prnResult)) {
	  /* directory with index filename exists already */
	}
	else if (resNodeIsFile(prnResult) && resNodeIsExist(prnResult)) {
	  /* index file exists already */
	  if (resNodeIsReadable(prnResult)) {
	    if (resNodeReadContent(prnResult, 1024) == FALSE) {
	      printf("Error resNodeReadContent(): there is no content\n");
	    }
	  }
	}
	else {
	  /* index file exists not */
	  resNodeSetType(prnResult,rn_type_index);

	  if (resNodeUpdate(prnResult, RN_INFO_CONTENT, NULL, NULL) == FALSE) {
	    printf("Error resNodeUpdate()\n");
	  }
	}
      }
#endif
      else {
      }
    }
  }
  return prnResult;
} /* end of resNodeIndexNew() */


/*! Resource Node List Append

\param prnArgList -- pointer to resNode list
\param eArgType -- type of resNode to count
\return number of childs
*/
int
_resNodeIndexGetScore(resNodePtr prnArgList)
{
  int iResult = -1;

  if (prnArgList != NULL) {
    resNodePtr prnT;

    for (iResult = 0, prnT = resNodeGetChild(prnArgList); prnT; prnT = resNodeGetNext(prnT)) {
      resNodePtr prnTT;

      switch (resNodeGetMimeType(prnT)) {

      case MIME_TEXT_PLAIN:
      case MIME_TEXT_PLAIN_CALENDAR:
      case MIME_TEXT_CSV:
      case MIME_TEXT_VCARD:
      {
	iResult++;
      }
      break;

      case MIME_TEXT_CALENDAR:
	iResult++;
        break;

      case MIME_APPLICATION_CXP_XML:
      case MIME_APPLICATION_VND_OASIS_OPENDOCUMENT_TEXT:
      case MIME_APPLICATION_VND_OASIS_OPENDOCUMENT_SPREADSHEET:
      case MIME_APPLICATION_VND_OASIS_OPENDOCUMENT_PRESENTATION:
      case MIME_APPLICATION_VND_SUN_XML_WRITER:
      case MIME_APPLICATION_VND_SUN_XML_CALC:
      case MIME_APPLICATION_VND_SUN_XML_IMPRESS:
      case MIME_APPLICATION_VND_STARDIVISION_WRITER:
      case MIME_APPLICATION_VND_STARDIVISION_CALC:
      case MIME_APPLICATION_VND_STARDIVISION_IMPRESS:
      case MIME_APPLICATION_MMAP_XML:
      case MIME_APPLICATION_MM_XML:
#ifdef HAVE_PIE
      case MIME_APPLICATION_PIE_XML:
#endif
      case MIME_APPLICATION_XMMAP_XML:
      case MIME_APPLICATION_XMIND_XML:
      case MIME_APPLICATION_XSPF_XML:
      case MIME_TEXT_HTML:
      case MIME_TEXT_XML:
      case MIME_TEXT_XSL:
      case MIME_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_WORDPROCESSINGML_DOCUMENT:
      case MIME_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_PRESENTATIONML_PRESENTATION:
      case MIME_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_SPREADSHEETML_SHEET:
      case MIME_APPLICATION_VND_MS_VISIO_DRAWING_MAIN_XML_2013:
      {
	iResult++;
      }
      break;

      case MIME_APPLICATION_ZIP:
      case MIME_APPLICATION_X_TAR:
      case MIME_APPLICATION_X_ISO9660_IMAGE:
  #ifdef HAVE_LIBARCHIVE
	iResult++;
  #endif
        break;

  #ifdef HAVE_JSON
      case MIME_APPLICATION_JSON:
	iResult++;
        break;
  #endif

      case MIME_APPLICATION_SLA:
        break;

  #ifdef HAVE_LIBSQLITE3
      case MIME_APPLICATION_X_SQLITE3:
	iResult++;
        break;
  #endif

  #ifdef HAVE_LIBID3TAG
      case MIME_AUDIO_MP3:
	iResult++;
        break;
  #endif

  #ifdef HAVE_LIBVORBIS
      case MIME_AUDIO_OGG:
	iResult++;
        break;
  #endif

      case MIME_IMAGE_GIF:
      case MIME_IMAGE_JPEG:
      case MIME_IMAGE_PNG:
      case MIME_IMAGE_TIFF:
      {
#ifdef HAVE_LIBEXIF
	iResult++;
#endif
      }
      break;

      default:
        /* no addtitional file information details */
        break;
      }
      for (prnTT = resNodeGetChild(prnT); prnTT; prnTT = resNodeGetNext(prnTT)) {
	iResult += _resNodeIndexGetScore(prnTT);
      }
    }
  }
  return iResult;
} /* end of _resNodeIndexGetScore() */


/*! Concatenates pucArgPath to existing context.

  \param prnArg the context
  \param pucArgPath pointer to a filesystem path
  \return TRUE if existing context concatenated 'pucArgPath'
 */
BOOL_T
_resNodeIndexSave(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (resNodeGetType(prnArg) == rn_type_index) {
    if (resNodeGetChild(prnArg)) { /* PATH is a tree of nodes */
      size_t i;
      xmlChar *pucContent = NULL;
      
      if (resNodeOpen(prnArg, "w") == FALSE) {
	printf("Error resNodeOpen()\n");
      }
      else if ((pucContent = resNodeListToPlain(prnArg, RN_INFO_INDEX)) == NULL || (i = resNodeGetSize(prnArg)) > 16) {
	printf("Error resNodeListToPlain()\n");
      }
      else if (resNodeSetContentPtr(prnArg,(void *)pucContent,i) != pucContent) {
	printf("Error resNodeSetContentPtr()\n");
      }
      else if (resNodePutContent(prnArg) == FALSE) {
	printf("Error resNodePutContent()\n");
      }
      else if (resNodeClose(prnArg) == FALSE) {
	printf("Error resNodeClose()\n");
      }
    }
    else { 			/*  */
    }    
  }
  
  return fResult;
} /* end of _resNodeIndexSave() */


/*! Concatenates pucArgPath to existing context.

  \param prnArg the context
  \param pucArgPath pointer to a filesystem path
  \return TRUE if existing context concatenated 'pucArgPath'
 */
xmlChar *
_resNodeIndexFind(resNodePtr prnArg, xmlChar *pucArgNeedle)
{
  xmlChar *pucResult = NULL;

  if (resNodeGetType(prnArg) == rn_type_index) {
    if (resNodeGetChild(prnArg)) { /* PATH is a tree of nodes */
    }
    else if (resNodeGetContentPtr(prnArg)) { /* PATH is a buffer of file paths */
    }
    else { 			/*  */
    }    
  }
  
  return pucResult;
} /* end of _resNodeIndexFind() */

#endif


/*! Resource Node List To DOM

\param prnArg -- resNode tree to build as DOM
\param iArgOptions
\return DOM tree representing prnArg
*/
xmlNodePtr
resNodeListToDOM(resNodePtr prnArg, int iArgOptions)
{
  xmlNodePtr pndResult = NULL;
  resNodePtr prnT;

  for (prnT = prnArg; prnT; prnT = resNodeGetNext(prnT)) {
    if (pndResult == NULL) {
      pndResult = resNodeToDOM(prnT, iArgOptions);
    }
    else {
      xmlAddSibling(pndResult, resNodeToDOM(prnT, iArgOptions));
    }
  }

  return pndResult;
} /* end of resNodeListToDOM() */


/*! Resource Node List To SQL

\param prnArg -- resNode tree to build as SQL INSERT statements
\return pointer to buffer with SQL INSERTs
*/
xmlChar *
resNodeListToSQL(resNodePtr prnArg, int iArgOptions)
{
  xmlChar *pucResult = NULL;
  resNodePtr prnT;

  for (prnT = prnArg; prnT; prnT = resNodeGetNext(prnT)) {
    resNodePtr prnChild;
    xmlChar *pucT;

    pucT = resNodeToSql(prnT,iArgOptions);
    pucResult = xmlStrcat(pucResult, pucT);
    pucResult = xmlStrcat(pucResult, BAD_CAST";");
    xmlFree(pucT);

    if ((prnChild = resNodeGetChild(prnT))) {
      xmlChar *pucTT;

      pucTT = resNodeListToSQL(prnChild,iArgOptions);
      pucResult = xmlStrcat(pucResult,pucTT);
      xmlFree(pucTT);
    }
  }
  return pucResult;
} /* end of resNodeListToSQL() */


/*! Resource Node List To JSON  

\param prnArg -- resNode tree to build as JSON string
\return pointer to buffer with JSON
*/
xmlChar *
resNodeListToJSON(resNodePtr prnArg, int iArgOptions)
{
  xmlChar *pucResult = NULL;
  resNodePtr prnT;

  pucResult = xmlStrdup(BAD_CAST"\n[");
  for (prnT = prnArg; prnT; prnT = resNodeGetNext(prnT)) {
    xmlChar *pucT = NULL;
    resNodePtr prnChild;

    pucResult = xmlStrcat(pucResult, BAD_CAST"{");
    pucT = resNodeToJSON(prnT,iArgOptions);
    pucResult = xmlStrcat(pucResult, pucT);
    xmlFree(pucT);

    if ((prnChild = resNodeGetChild(prnT))) {
      pucResult = xmlStrcat(pucResult, BAD_CAST", 'children' : ");
      pucT = resNodeListToJSON(prnChild,iArgOptions);
      pucResult = xmlStrcat(pucResult, pucT);
      xmlFree(pucT);
    }
    pucResult = xmlStrcat(pucResult, BAD_CAST"}");
  }
  pucResult = xmlStrcat(pucResult, BAD_CAST"]\n");
  
  return pucResult;
} /* end of resNodeListToJSON() */


/*! Resource Node List To Xml  

\param prnArg -- resNode tree to build as XML string
\return pointer to buffer with XML
*/
xmlChar *
resNodeListToXml(resNodePtr prnArg, int iArgOptions)
{
  xmlChar *pucResult = NULL;
  xmlNodePtr pndT = NULL;
  xmlDocPtr pdocT = NULL;

  if (prnArg) {
    if ((pndT = resNodeListToDOM(prnArg,iArgOptions)) != NULL && (pdocT = domDocFromNodeNew(pndT)) != NULL) {
      int iLength = 0;

      /*!\bug XML declaration is missing */
    
#if 0
      xmlDocDumpMemory(pdocT,&pucResult,&iLength);
      if (iLength > -1) {
	pucResult[iLength] = '\0';
      }
#else
      xmlBufferPtr buffer;

      buffer = xmlBufferCreate();
      if (buffer) {
	iLength = xmlNodeDump(buffer, pndT->doc, pndT, 0, 1);
	if (iLength > 0) {
	  pucResult = xmlBufferDetach(buffer);
	}
	xmlBufferFree(buffer);
      }
#endif
    }
    else {
      resNodeSetError(prnArg,rn_error_xml,"Error resNodeListToXml()");
    }

    xmlFreeNode(pndT);
    xmlFreeDoc(pdocT);
  }
  return pucResult;
} /* end of resNodeListToXml() */


/*! Resource Node List To Plain

\param prnArg -- resNode tree to build as plain string
\return pointer to buffer with plain output
*/
xmlChar *
resNodeListToPlain(resNodePtr prnArg, int iArgOptions)
{
  xmlChar *pucResult = NULL;

  if (prnArg) {
    resNodePtr prnT;

    for (prnT = prnArg; prnT; prnT = resNodeGetNext(prnT)) {
      xmlChar* pucT = NULL;
      resNodePtr prnChild;

      if ((prnChild = resNodeGetChild(prnT))) {
	if ((pucT = resNodeListToPlain(prnChild, iArgOptions))) {
	  if (pucResult) {
	    pucResult = xmlStrcat(pucResult, pucT);
	    xmlFree(pucT);
	  }
	  else {
	    pucResult = pucT;
	  }
	}
      }

      if ((pucT = resNodeToPlain(prnT, iArgOptions))) {
	if (pucResult) {
	  pucResult = xmlStrcat(pucResult, pucT);
	  xmlFree(pucT);
	}
	else {
	  pucResult = pucT;
	}
      }
    }
  }
  else {
    resNodeSetError(prnArg,rn_error_argv,"Error resNodeListToPlain()");
  }
  return pucResult;
} /* end of resNodeListToPlain() */


#ifdef TESTCODE
#include "test/test_res_node_list.c"
#endif

