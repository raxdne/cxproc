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

/*!\todo “URL -> Content” => `https://localhost/cxproc/exe` */

/*!\todo “URL -> Database -> SQL Query -> Content” => `https://localhost/Test/Database.db3`->Database Node->`?SELECT * FROM directory` */

/*!\todo “URL -> Archive Path -> Content”

- `https://localhost/Test/Arcive.zip`->Archive Node->`Test/Text.txt`

- `https://localhost/Test/Archive.zip`->Archive Node->`Test/Archive.zip`->Archive Node->`Test/Text.txt` */

/*!\todo “Directory -> Content” */

/*!\todo “Directory -> File -> Archive -> Archive Path -> Content” */

/*!\todo “Directory -> File -> Database -> SQL Query -> Content” */

/*!\todo deny direct access to 'resNodePtr' from extern /prn[a-z]+->[a-z]+/ */

/*!\todo improve speed by using index files 'NAME_FILE_INDEX' */

#ifdef HAVE_LIBCURL
#include <curl/curl.h>
#endif

#include <libxml/uri.h>
#include <libxml/parser.h>

#include "basics.h"
#include "utils.h"
#include <res_node/res_node_ops.h>
#include "plain_text.h"

#ifdef HAVE_PIE
#include <pie/pie_text_blocks.h>
#include <ics/ics.h>
#include <vcf/vcf.h>
#endif

#include "dom.h"
#include <cxp/cxp_dtd.h>

#ifdef HAVE_JSON
#include <json/json.h>
#endif
#ifdef HAVE_LIBSQLITE3
#include <database/database.h>
#endif
#ifdef HAVE_LIBID3TAG
#include <audio/audio.h>
#endif
#if defined(HAVE_LIBEXIF)
#include <image/image_exif.h>
#endif


const char *mpcTypeNames[] = {
	     "rn_type_undef",
	     "rn_type_stdout",
	     "rn_type_stderr",
	     "rn_type_stdin",
	     "rn_type_memory",
	     "rn_type_root",
	     "rn_type_dir",
	     "rn_type_file",
	     "rn_type_file_compressed",
	     "rn_type_document_plain",
	     "rn_type_document_dom",
	     "rn_type_document_json",
	     "rn_type_image",
	     "rn_type_index",
	     "rn_type_archive",
	     "rn_type_dir_in_archive",
	     "rn_type_file_in_archive",
	     "rn_type_database",
	     "rn_type_file_in_database",
	     "rn_type_symlink",
	     "rn_type_url_http",
	     "rn_type_url_ftp",
	     "rn_type_url"
};

static BOOL_T
resNodeSetExtension(resNodePtr prnArg);

static BOOL_T
resNodeSetNameBaseNative(resNodePtr prnArg, char *pcArgPath);

static BOOL_T
resNodeResetNameBase(resNodePtr prnArg);

static BOOL_T
resNodeReadOwner(resNodePtr prnArg);

/* **********************************************************************************************

   filesystem resource node functions

 */

/*! \return a pointer to a freshly allocated 'resNode', all values to NULL or undefined

  !!! NO DIRECT USE OUTSIDE 'res_node.c' !!!

  frontend is either resNodeRootNew() or resNodeDirNew()
 */
resNodePtr
resNodeNew(void)
{
  resNodePtr prnResult;

  prnResult = (resNodePtr)xmlMalloc(sizeof(resNode));
  if (prnResult) {
    memset(prnResult,0,sizeof(resNode));
  }
  else {
    PrintFormatLog(1,"Out of memory at resNodeNew()");
  }

  return prnResult;
} /* end of resNodeNew() */


/*! Duplicates an existing resource node into an new one (Path only!).

  \todo complete code for duplication of archive with childs

  \param prnArg pointer to resource node to duplicate
  \return a pointer to new 'resNode'
 */
resNodePtr
resNodeDup(resNodePtr prnArg, int iArgOptions)
{
  resNodePtr prnResult = NULL;

  if (prnArg) {

    if ((prnResult = resNodeDirNew(resNodeGetNameNormalized(prnArg))) == NULL) {
    }
    else if (iArgOptions & RN_DUP_EXIST && resNodeReadStatus(prnResult) == FALSE) { /* duplicate existing resource nodes only (file system) */
      /* not existing */
      resNodeFree(prnResult);
      prnResult = NULL;
    }
    else if (iArgOptions & RN_DUP_READ && resNodeIsReadable(prnResult) == FALSE) { /* duplicate readable resource nodes only (file system) */
      /* not readable */
      resNodeFree(prnResult);
      prnResult = NULL;
    }
    else {
      resNodePtr prnChild = NULL;

      resNodeSetType(prnResult, resNodeGetType(prnArg));
      resNodeSetRecursion(prnResult, resNodeIsRecursive(prnArg));

      if (iArgOptions & RN_DUP_CONTENT && resNodeGetContentPtr(prnArg) != NULL) {
	void *pT;
	
	assert(resNodeGetSize(prnArg) > 0);
	pT = xmlMalloc(resNodeGetSize(prnArg));
	memcpy(pT,resNodeGetContentPtr(prnArg),resNodeGetSize(prnArg));
	resNodeSetContentPtr(prnResult,pT,resNodeGetSize(prnArg));
      }
      
      for (prnChild = resNodeGetChild(prnArg); (iArgOptions & RN_DUP_CHILDS) && prnChild != NULL; prnChild = resNodeGetNext(prnChild)) {

	if (resNodeIsLink(prnChild)) {
	}
	else if (resNodeIsFileInArchive(prnChild)) {
#if 0
	  resNodePtr prnNew;

	  prnNew = resNodeNew();
	  memcpy(prnNew,prnChild,sizeof(resNode));

	  if (prnNew->pucNameBase) prnNew->pucNameBase = xmlStrdup(prnNew->pucNameBase);
	  if (prnNew->pcNameBaseNative) prnNew->pcNameBaseNative = (char *) xmlStrdup(BAD_CAST prnNew->pcNameBaseNative);
	  if (prnNew->pucNameNormalized) prnNew->pucNameNormalized = xmlStrdup(prnNew->pucNameNormalized);
	  if (prnNew->pcNameNormalizedNative) prnNew->pcNameNormalizedNative = (char *)xmlStrdup(BAD_CAST prnNew->pcNameNormalizedNative);
	  if (prnNew->pucNameBaseDir) prnNew->pucNameBaseDir = xmlStrdup(prnNew->pucNameBaseDir);
	  if (prnNew->pucExtension) prnNew->pucExtension = xmlStrdup(prnNew->pucExtension);
	  prnNew->pucNameShort = NULL;
	  if (prnNew->pucURI) prnNew->pucURI = xmlStrdup(prnNew->pucURI);
	  if (prnNew->pucOwner) prnNew->pucOwner = xmlStrdup(prnNew->pucOwner);
	  if (prnNew->pucObject) prnNew->pucObject = xmlStrdup(prnNew->pucObject);

	  if (prnArg->ppucProp) {
	    /* duplicate existing pairs + end marker */
	    size_t i;

	    for (i=0; ; i += 2) {
	      if (prnArg->ppucProp[i] == NULL) {
		size_t j;
		xmlChar **ppucT;

		ppucT = (xmlChar **)xmlMalloc(i * 2 * sizeof(xmlChar *));
		if (ppucT) {
		  for (j=0; j<i; j += 2) { /* copy existing pointers */
		    ppucT[j]   = xmlStrdup(prnArg->ppucProp[j]);
		    ppucT[j+1] = xmlStrdup(prnArg->ppucProp[j+1]);
		    ppucT[j+2] = NULL;
		  }
		  prnNew->ppucProp = ppucT;
		}
		else {
		}
	      }
	    }
	  }

	  //if (prnNew->pucOwner) prnNew->pucOwner = xmlStrdup(prnNew->pucOwner);
	  prnNew->pucAlias = NULL;
	  prnNew->pContent = NULL;
	  prnNew->pdocContent = NULL;
	  //prnNew->handleDir = NULL;
	  if ((prnChild = resNodeGetChild(prnNew))) {
	    resNodeAddChild(prnNew,resNodeDup(prnChild,iArgOptions));
	  }
	  resNodeAddChild(prnResult,prnNew);
#endif
	}
	else {
	  resNodeAddChild(prnResult, resNodeDup(prnChild, (iArgOptions ^ RN_DUP_NEXT)));
	}
      }
    }

    if (iArgOptions & RN_DUP_NEXT) {
      resNodePtr prnI;

      for (prnI = resNodeGetNext(prnArg); prnI; prnI = resNodeGetNext(prnI)) {
	resNodePtr prnNew;

	prnNew = resNodeDup(prnI, iArgOptions & ~RN_DUP_NEXT); /* duplicate without following siblings */
	if (prnNew) {
	  if (prnResult) {
	    resNodeAddSibling(prnResult, prnNew);
	  }
	  else {
	    prnResult = prnNew;
	  }
	}
      }
    }
  }

  return prnResult;
} /* end of resNodeDup() */


/*! Creates a new resource node according to pucArgPath, as descendant of root resource node only.

  \param prnRoot pointer to root resNode
  \param pucArgPath pointer to a filesystem path

  \return a pointer to a new 'resNode'
 */
resNodePtr
resNodeRootNew(resNodePtr prnRoot, xmlChar *pucArgPath)
{
  resNodePtr prnResult;

  if (prnRoot != NULL && prnRoot->fExist == TRUE) {
    if (resPathIsURL(pucArgPath) || resPathIsDescendant(prnRoot->pucNameNormalized,pucArgPath)) {
      prnResult = resNodeDirNew(pucArgPath);
    }
    else {
      prnResult = resNodeDup(prnRoot, RN_DUP_THIS);
      if (prnResult) {
	resNodeConcat(prnResult,pucArgPath);
      }
    }

    if (prnResult != NULL) {
      if (resPathIsDescendant(prnRoot->pucNameNormalized,prnResult->pucNameNormalized) == FALSE) {
	resNodeSetError(prnResult,rn_error_path,"Path '%s' is out of root resource node",prnResult->pucNameNormalized);
	resNodeFree(prnResult);
	prnResult = NULL;
      }
      else {
	resNodeSetRecursion(prnResult,resPathIsDirRecursive(pucArgPath));
      }
    }
  }
  else { /* no root resource node exists */
    prnResult = resNodeDirNew(pucArgPath);
  }
  return prnResult;
} /* end of resNodeRootNew() */


/*! Creates a new child resource node according to pucArgPath

\param prnArg parent resource node
\param pucArgPath pointer to a locator
\return a pointer to a new 'resNode' or NULL in case of error
*/
resNodePtr
resNodeAddChildNew(resNodePtr prnArg, xmlChar *pucArgPath)
{
  resNodePtr prnResult = NULL;

  if (prnArg != NULL && STR_IS_NOT_EMPTY(pucArgPath)) {
    xmlChar *pucT;

    if (resPathIsAbsolute(pucArgPath)) {
      pucT = resPathDiffPtr(resNodeGetNameNormalized(prnArg), pucArgPath);
      if (STR_IS_NOT_EMPTY(pucT)) {
	while (issep(*pucT)) pucT++;
	prnResult = resNodeAddChildNew(prnArg, pucT);
      }
      else {
      }
    }
    else {
      pucT = pucArgPath;
      if (isdot(pucT[0]) && issep(pucT[1])) pucT++; /* skip leading dot */
      while (issep(*pucT)) pucT++; /* skip leading separators */
      if (xmlStrlen(pucT) > 0) {
	xmlChar *pucSep;
	xmlChar *pucBase = NULL;

	pucSep = resPathGetNextSeparatorPtr(pucT);
	if (pucSep) {
	  if (isdot(pucT[0]) && isdot(pucT[1]) && (pucSep - pucT) == 2) {
	    resNodePtr prnParent;

	    if ((prnParent = resNodeGetParent(prnArg)) && STR_IS_NOT_EMPTY(pucSep)) {
	      /* one directory up */
	      while (issep(*pucSep)) pucSep++;
	      prnResult = resNodeAddChildNew(prnParent, pucSep);
	    }
	    else {
	      resNodeSetError(prnArg, rn_error_path, "Path '%s' is out of root resource node", pucT);
	    }
	  }
	  else if (resPathBeginIsArchive(pucT)) {
	    pucBase = xmlStrndup(pucT, (int)(pucSep - pucT)); /* duplicate without trailing separator */
	  }
	  else if (pucSep - pucT > 0) {
	    pucBase = xmlStrndup(pucT, (int)(pucSep + 1 - pucT)); /* duplicate including _one_ trailing separator */
	  }
	  while (issep(*pucSep)) pucSep++;
	}
	else {
	  pucBase = xmlStrdup(pucT);
	}

	if (STR_IS_NOT_EMPTY(pucBase)) {
	  if ((prnResult = resNodeListFindChild(prnArg, pucBase))) {
	    /* this child exists already */
	    xmlFree(pucBase);
	  }
	  else if ((prnResult = resNodeNew())) {
	    prnResult->pucNameBase = pucBase;
	    if (resPathBeginIsFileCompressed(prnResult->pucNameBase)) {
	      resNodeSetType(prnResult, rn_type_file_compressed);
	    }
#if 1
	    else if (resNodeIsFileInZip(prnArg)) {
	      resNodeSetType(prnResult, rn_type_file_in_zip);
	    }
	    else if (resNodeIsDirInZip(prnArg)) {
	      resNodeSetType(prnResult, rn_type_dir_in_zip);
	      // resPathCutTrailingChars(prnResult->pucNameBase);
	    }
#else
	    else if (resNodeIsZipDocument(prnArg)) {
	      if (resPathIsDir(prnResult->pucNameBase)) {
		resNodeSetType(prnResult, rn_type_dir_in_zip);
		resPathCutTrailingChars(prnResult->pucNameBase);
	      }
	      else {
		resNodeSetType(prnResult, rn_type_file_in_zip);
	      }
	    }
#endif
	    else if (resNodeIsArchive(prnArg) || resNodeIsDirInArchive(prnArg)) {
	      if (resPathIsDir(prnResult->pucNameBase)) {
		resNodeSetType(prnResult, rn_type_dir_in_archive);
		resPathCutTrailingChars(prnResult->pucNameBase);
	      }
	      else {
		resNodeSetType(prnResult, rn_type_file_in_archive);
	      }
	    }
	    else {
	      if (resPathBeginIsArchive(prnResult->pucNameBase)) {
		resNodeSetType(prnResult, rn_type_archive);
		//resNodeSetRecursion(prnResult, resPathIsDirRecursive(pucArgPath));
		resPathCutTrailingChars(prnResult->pucNameBase);
	      }
	      else if (resPathIsDir(prnResult->pucNameBase)) {
		resNodeSetType(prnResult, rn_type_dir);
		resNodeSetRecursion(prnResult, resPathIsDirRecursive(pucArgPath));
		resPathCutTrailingChars(prnResult->pucNameBase);
	      }
	      else {
		resNodeSetType(prnResult, rn_type_file);
	      }
	    }
	    resNodeAddChild(prnArg, prnResult);
	    resNodeGetNameNormalizedNative(prnResult); /* to update normalized path */
	    resNodeResetMimeType(prnArg);
	  }
	  else {
	    xmlFree(pucBase);
	  }

	  if (prnResult != NULL && xmlStrlen(pucSep) > 1) { /* there is a path after pucSep */
	    prnResult = resNodeAddChildNew(prnResult, pucSep);
	  }
	}
      }
    }
  }
  return prnResult;
} /* end of resNodeAddChildNew() */


/*! Creates a new resource node

  \return a pointer to a new 'resNode'
 */
resNodePtr
resNodeInMemoryNew(void)
{
  resNodePtr prnResult;

  prnResult = resNodeNew();
  if (prnResult) {
    resNodeSetType(prnResult,rn_type_memory);
    prnResult->pucNameNormalized = xmlStrdup(BAD_CAST ":memory:");
    prnResult->pcNameNormalizedNative = (char *)xmlStrdup(prnResult->pucNameNormalized);
    prnResult->fWrite = TRUE;
  }
  
  return prnResult;
} /* end of resNodeInMemoryNew() */


/*! Tests the readability of a directory with given name.

  \param pucArgPath relative or absolute path
  \return TRUE if the named directory is is readable
*/
BOOL_T
resNodeTestDirStr(xmlChar *pucArgPath)
{
  BOOL_T fResult = FALSE;

  if (STR_IS_NOT_EMPTY(pucArgPath)) {
    resNodePtr prnDir;

    prnDir = resNodeDirNew(pucArgPath);
    if (resNodeReadStatus(prnDir)) {
      fResult = resNodeIsDir(prnDir);
    }
    resNodeFree(prnDir);
  }
  return fResult;
} /* end of resNodeTestDirStr() */


/*! tests the readability of a file with given name

  \param pucArgPath relative or absolute path
  \return TRUE if the named file is is readable
*/
BOOL_T
resNodeTestFileStr(xmlChar *pucArgPath)
{
  BOOL_T fResult = FALSE;

  if (STR_IS_NOT_EMPTY(pucArgPath)) {
    resNodePtr prnFile;

    prnFile = resNodeDirNew(pucArgPath);
    if (resNodeReadStatus(prnFile)) {
      fResult = resNodeIsFile(prnFile);
    }
    resNodeFree(prnFile);
  }
  return fResult;
} /* end of resNodeTestFileStr() */


#if defined(_MSC_VER) && ! defined(__cplusplus)

/*! resolves a Windows Shell link

see https://searchcode.com/codesearch/view/2103156/

\return HRESULT or -1 in case of error
*/
HRESULT
ResolveIt(HWND hwnd, LPCSTR lpszLinkFile, LPTSTR lpszPath, int iPathBufferSize)
{
  HRESULT hres;
  IShellLink* psl;
  IPersistFile* ppf;
  WORD wsz[MAX_PATH];
  char szGotPath[MAX_PATH];

  assert(lpszPath != NULL);
  assert(lpszLinkFile != NULL);

  *lpszPath = 0; // assume failure 

  hres = OleInitialize(NULL);
  if (hres != S_FALSE && hres != S_OK) {
    //      set_werrno;
    //		fprintf (stderr, "%s: Could not initialize OLE interface\n", prog_name);
    CoUninitialize();
    return(-1);
  }

  // Get a pointer to the IShellLink interface. 
  hres = CoCreateInstance((REFCLSID)&CLSID_ShellLink, NULL,
    CLSCTX_INPROC_SERVER, (REFIID)&IID_IShellLink, (LPVOID *)&psl);
  if (!SUCCEEDED(hres)) {
    //      set_werrno;
    //	    fprintf (stderr, "%s: CoCreateInstance failed\n", prog_name);
    CoUninitialize();
    return(-1);
  }

  hres = psl->lpVtbl->QueryInterface(psl, &IID_IPersistFile, (void **)&ppf);
  // Ensure that the string is Unicode. 
  hres = MultiByteToWideChar(AreFileApisANSI() ? CP_ACP : CP_OEMCP,
    0, lpszLinkFile, -1, (LPWSTR)wsz, MAX_PATH);
  if (!SUCCEEDED(hres)) {
    //      set_werrno;
    //		fprintf (stderr, "%s: Unicode translation failed%ld\n", prog_name, hres);
    CoUninitialize();
    return(-1);
  }

  // Load the shortcut. 
  hres = ppf->lpVtbl->Load(ppf, wsz, STGM_READ);
  if (!SUCCEEDED(hres)) {
    //      set_werrno;
    //	    fprintf (stderr, "%s: Could not load shortcut %s\n", prog_name, lpszLinkFile);
    CoUninitialize();
    return(-1);
  }

  // Resolve the link. 
  //    hres = psl->lpVtbl->Resolve(psl, NULL, 0);
  //			SLR_ANY_MATCH
  //			| SLR_NO_UI | SLR_NOLINKINFO | SLR_NOUPDATE | SLR_NOSEARCH
  //			| SLR_NOTRACK);
  //    if (!SUCCEEDED(hres)) { 
  //		set_werrno;
  ////	    fprintf (stderr, "%s: Could not resolve link %s\n", prog_name, lpszLinkFile);
  //		CoUninitialize();
  //  		return(-1);
  //	}

  // Get the path to the link target. 
  hres = psl->lpVtbl->GetPath(psl, szGotPath, MAX_PATH, NULL, 0);
  //		SLGP_UNCPRIORITY);
  //		    SLGP_SHORTPATH); 
  if (!SUCCEEDED(hres)) {
    //      set_werrno;
    //	    fprintf (stderr, "%s: Could not get path to link target %s\n", prog_name, lpszLinkFile);
    CoUninitialize();
    return(-1);
  }

  lstrcpy(lpszPath, szGotPath);

  // Release the pointer to the IPersistFile interface. 
  ppf->lpVtbl->Release(ppf);
  // Release the pointer to the IShellLink interface. 
  psl->lpVtbl->Release(psl);
  CoUninitialize();

  return hres;
}
#endif


/*! return a pointer to new resNode resolving symbolic link, append as child

\todo find a similar code for VC++

\param prnArg link resource node
\return pointer to new resNode or NULL in case of error
*/
resNodePtr
resNodeResolveLinkChildNew(resNodePtr prnArg)
{
  resNodePtr prnResult = NULL;

  if (prnArg != NULL && prnArg->pucNameNormalized != NULL) {
#ifdef _MSC_VER
    TCHAR achPath[MAX_PATH] ={ 0 };

#if defined(__cplusplus)

#if 0
    // https://www.c-plusplus.net/forum/topic/345713/realpath-gegenst%C3%BCck-f%C3%BCr-windows-pfad-auf-der-der-shortcut-zeigt/8
    size_t pathLen;
    CComPtr<IShellLink> psl;
    HWND hwnd;
    HRESULT hres = CoInitialize(NULL);

    if (SUCCEEDED(hres)) {
      hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, reinterpret_cast<void**>(&psl));

      if (SUCCEEDED(hres)) {
	CComPtr<IPersistFile> ppf;

	hres = psl->QueryInterface(IID_IPersistFile, reinterpret_cast<void**>(&ppf));
	if (SUCCEEDED(hres)) {

#ifdef UNICODE
	  const WCHAR* wsz = lnkFile;
#else
	  WCHAR wsz[MAX_PATH];
	  if (!MultiByteToWideChar(CP_ACP, 0, (TCHAR *)prnArg->pcNameNormalizedNative, -1, wsz, MAX_PATH))
	  {
	    // hier sollte reagiert werden...	
	  }
#endif

	  hres = ppf->Load(wsz, STGM_READ);
	  if (SUCCEEDED(hres)) {
	    hres = psl->Resolve(hwnd, 0);
	    if (SUCCEEDED(hres)) {
	      prnResult = resNodeConcatNew(resNodeGetNameBaseDir(prnArg), BAD_CAST psl->GetPath(achPath, pathLen, nullptr, 0));
	    }
	    else {
	      resNodeSetError(prnArg, rn_error_link,"link error");
	    }
	  }
	  else {
	    resNodeSetError(prnArg, rn_error_link,"link error");
	  }
	}
      }
      CoUninitialize();
    }
#endif

#else
    // https://msdn.microsoft.com/en-us/library/windows/desktop/bb774952.aspx IShellLink
    // http://www.cplusplus.com/forum/windows/64088/

    HRESULT hres = CoInitialize(NULL);
    if (SUCCEEDED(hres)) {

      hres = ResolveIt(NULL, prnArg->pcNameNormalizedNative, achPath, _countof(achPath));
      if (SUCCEEDED(hres)) {
	prnResult = resNodeConcatNew(resNodeGetNameBaseDir(prnArg), BAD_CAST achPath);
      }
      else {
	resNodeSetError(prnArg,rn_error_link,"link error");
      }

      CoUninitialize();
    }

#endif

#elif defined(_WIN32)

#else
    int ciLength;
    char mcT[MAX_PATH];

    // https://www.gnu.org/software/libc/manual/html_node/Symbolic-Links.html
    ciLength = readlink(resNodeGetNameNormalizedNative(prnArg), mcT, MAX_PATH);
    if (ciLength < 1) {
      resNodeSetError(prnArg,rn_error_link,"link error");
    }
    else {
      assert(ciLength < MAX_PATH);
      mcT[ciLength] = '\0';
      prnResult = resNodeConcatNew(resNodeGetNameBaseDir(prnArg),BAD_CAST mcT);
    }
#endif

    if (prnResult) {
      resNodeFree(resNodeGetChild(prnArg)); /* free old child */
      resNodeReadStatus(prnResult);
      resNodeAddChild(prnArg,prnResult);
    }
  }
  return prnResult;
} /* end of resNodeResolveLinkChildNew() */


/*! return a pointer to new resNode according to symbolic name

\param pucArg symbolic name
\return pointer to new resNode or NULL in case of error
*/
resNodePtr
resNodeAliasNew(xmlChar *pucArg)
{
  resNodePtr prnResult = NULL;

  if (STR_IS_NOT_EMPTY(pucArg)) {
    prnResult = resNodeNew();
    if (prnResult) {
      prnResult->pucAlias = xmlStrdup(pucArg);
    }
  }
  return prnResult;
} /* end of resNodeAliasNew() */


/*! Resource Node List Get Next

\return pointer to next resNode in list
*/
resNodePtr
resNodeGetNext(resNodePtr prnArg)
{
  resNodePtr prnResult = NULL;

  if (prnArg) {
    prnResult = prnArg->next;
  }
  return prnResult;
} /* end of resNodeGetNext() */


/*! appends a resource node list as siblings

\todo lock list while multithreading

\param prnArgList -- pointer to resNode list
\param prnArg -- resNode to append to list
\return TRUE if successful, FALSE in case of errors
*/
BOOL_T
resNodeAddSibling(resNodePtr prnArgList, resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (prnArgList != NULL && prnArg != NULL && prnArg != prnArgList) {
    resNodePtr prnLast;
    resNodePtr prnI;

    assert(prnArgList != prnArg);

    assert(prnArgList->parent != prnArg);
    assert(prnArgList->children != prnArg);
    assert(prnArgList->next != prnArg);
    assert(prnArgList->prev != prnArg);

    assert(prnArgList != prnArg->parent);
    assert(prnArgList != prnArg->children);
    assert(prnArgList != prnArg->next);
    assert(prnArgList != prnArg->prev);

    for (prnLast = prnArgList; prnLast->next != NULL; prnLast = prnLast->next) ;
    
    if (prnLast != NULL) {

      assert(prnLast->next == NULL);
	
      resNodeListUnlink(prnArg);

      prnLast->next = prnArg;
      prnArg->prev = prnLast;

      for (prnI = prnArg; prnI; prnI = prnI->next) {
	prnI->parent = prnLast->parent;
	if (prnLast->parent) {
	  prnLast->parent->last = prnI;
	}
	else {
	}
      }
      fResult = TRUE;
    }
  }
  return fResult;
} /* end of resNodeAddSibling() */


/*! appends a resource node list as childs

\param prnArgParent -- resNode to use as parent
\param prnArgChild -- resNode list to use as childs
\return TRUE if successful, FALSE in case of errors
*/
BOOL_T
resNodeAddChild(resNodePtr prnArgParent, resNodePtr prnArgChild)
{
  BOOL_T fResult = FALSE;

  if (prnArgParent != NULL && prnArgChild != NULL) {
    resNodePtr prnLast;
    resNodePtr prnI;

    assert(prnArgParent != prnArgChild);
    assert(prnArgParent->children != prnArgChild);
    assert(prnArgParent != prnArgChild->parent);

    prnLast = prnArgParent->last;

    for (prnI = prnArgChild; prnI; prnI = prnI->next) {
      prnI->parent = prnArgParent;
      prnArgParent->last = prnI;
    }

    if (prnArgParent->children) {
      prnLast->next = prnArgChild;
      prnArgChild->prev = prnLast;
    }
    else {
      prnArgParent->children = prnArgChild;
    }
    fResult = TRUE;
  }
  return fResult;
} /* end of resNodeAddChild() */


/*! Resource Node List Get Child

\return pointer to child resNode of prnArg
*/
resNodePtr
resNodeGetChild(resNodePtr prnArg)
{
  resNodePtr prnResult = NULL;

  if (prnArg) {
    prnResult = prnArg->children;
  }
  return prnResult;
} /* end of resNodeGetChild() */


/*! Resource Node List Get last resource node

\return pointer to last resNode of prnArg
*/
resNodePtr
resNodeGetLast(resNodePtr prnArg)
{
  resNodePtr prnResult = NULL;

  if (prnArg) {
    prnResult = prnArg->last;
  }
  return prnResult;
} /* end of resNodeGetLast() */


/*! Resource Node List Get parent

\return pointer to parent resNode of prnArg
*/
resNodePtr
resNodeGetParent(resNodePtr prnArg)
{
  resNodePtr prnResult = NULL;

  if (prnArg) {
    prnResult = prnArg->parent;
  }
  return prnResult;
} /* end of resNodeGetParent() */


/*! Resource Node List Get parent

\return pointer to root resNode of prnArg
*/
resNodePtr
resNodeGetRoot(resNodePtr prnArg)
{
  resNodePtr prnResult = NULL;

  if (prnArg) {
    resNodePtr prnParent;

    for (prnParent = prnArg; prnParent != NULL; prnParent = resNodeGetParent(prnParent)) {
      prnResult = prnParent;
    }
  }
  return prnResult;
} /* end of resNodeGetRoot() */


/*! Resource Node List Get parent

\return pointer to Ancestor archive resNode of prnArg
*/
resNodePtr
resNodeGetAncestorArchive(resNodePtr prnArg)
{
  resNodePtr prnResult = NULL;

  if (prnArg) {
    resNodePtr prnParent;

    for (prnParent = resNodeGetParent(prnArg); prnParent != NULL; prnParent = resNodeGetParent(prnParent)) {
      if (resNodeIsArchive(prnParent)) {
	prnResult = prnParent;
	break;
      }
    }
  }
  return prnResult;
} /* end of resNodeGetAncestorArchive() */


/*! Resource Node List Get parent

\return pointer to Ancestor zip resNode of prnArg
*/
resNodePtr
resNodeGetAncestorZip(resNodePtr prnArg)
{
  resNodePtr prnResult = NULL;

  if (prnArg) {
    resNodePtr prnParent;

    for (prnParent = resNodeGetParent(prnArg); prnParent != NULL; prnParent = resNodeGetParent(prnParent)) {
      if (resNodeIsZipDocument(prnParent)) {
	prnResult = prnParent;
	break;
      }
    }
  }
  return prnResult;
} /* end of resNodeGetAncestorZip() */


/*! \return pointer to a string containing ancestor basenames of prnArg
*/
xmlChar *
resNodeGetAncestorPathStr(resNodePtr prnArg)
{
  xmlChar* pucResult = NULL;

  if (prnArg) {
    resNodePtr prnT;

    for (prnT = prnArg; prnT != NULL; prnT = resNodeGetParent(prnT)) {
      xmlChar* pucT = NULL;

      if (pucResult) {
	pucT = xmlStrdup(resNodeGetNameBase(prnT));
	pucT = xmlStrcat(pucT,BAD_CAST"/");
	pucT = xmlStrcat(pucT, pucResult);
	xmlFree(pucResult);
	pucResult = pucT;
      }
      else {
	pucResult = xmlStrdup(resNodeGetNameBase(prnT));
      }
    }
  }
  return pucResult;
} /* end of resNodeGetAncestorPathStr() */


/*! for tests only

\return number of child descendant resource nodes in list
*/
int
resNodeGetLength(resNodePtr prnArg)
{
  int iResult;

  for (iResult=0; prnArg; prnArg = resNodeGetChild(prnArg)) {
    iResult++;
  }

  return iResult;
} /* end of resNodeGetLength() */


/*! Resource Node List Append

\param prnArgList -- pointer to resNode list
\param eArgType -- type of resNode to count
\return number of childs
*/
int
resNodeGetChildCount(resNodePtr prnArgList, RN_TYPE eArgType)
{
  int iResult = -1;

  if (prnArgList) {
    resNodePtr prnT;

    for (iResult = 0, prnT = resNodeGetChild(prnArgList); prnT; prnT = resNodeGetNext(prnT)) {
      if (resNodeGetType(prnT) == eArgType) {
	iResult++;
      }
    }
  }
  return iResult;
} /* end of resNodeGetChildCount() */


/*! 

\param prnArgList -- pointer to resNode list
\param eArgType -- type of resNode to count
\return number of siblings
*/
int
resNodeGetSiblingsCount(resNodePtr prnArgList, RN_TYPE eArgType)
{
  int iResult = -1;

  if (prnArgList) {
    resNodePtr prnT;

    for (iResult = 0, prnT = prnArgList->prev; prnT; prnT = prnT->prev) {
      if (resNodeGetType(prnT) == eArgType) {
	iResult++;
      }
    }

    for (prnT = prnArgList->next; prnT; prnT = prnT->next) {
      if (resNodeGetType(prnT) == eArgType) {
	iResult++;
      }
    }
  }
  return iResult;
} /* end of resNodeGetSiblingsCount() */


/*! Resource Node List Append

\param prnArgList -- pointer to resNode list
\param eArgType -- type of resNode to count
\return number of childs
*/
int
resNodeGetCountDescendants(resNodePtr prnArgList)
{
  int iResult = 0;

  if (prnArgList) {
    resNodePtr prnT;

    for (prnT = resNodeGetChild(prnArgList); prnT; prnT = resNodeGetNext(prnT)) {
      iResult += resNodeGetCountDescendants(prnT);
    }
    iResult++;
  }
  return iResult;
} /* end of resNodeGetCountDescendants() */


/*! \return last and only node on child axis or prnArgList if there is no child
*/
resNodePtr
resNodeGetLastDescendant(resNodePtr prnArgList)
{
  resNodePtr prnResult = prnArgList;

  if (prnArgList) {
    resNodePtr prnT;

    for (prnT = resNodeGetChild(prnArgList); prnT; prnT = resNodeGetChild(prnT)) {
      if (prnT->next) {
	/* there are other descendants of prnArgList */
	break;
      }
      else if (prnT->children == NULL) {
	/* this last is the result */
	prnResult = prnT;
      }
    }
  }
  return prnResult;
} /* end of resNodeGetLastDescendant() */


/*! Sets and returns the pucAlias of this resource node.

  \param prnArg a pointer to a resource node
  \return pucNameNormalized or NULL in case of errors
 */
xmlChar *
resNodeGetNameAlias(resNodePtr prnArg)
{
  if (prnArg) {
    return prnArg->pucAlias;
  }
  return NULL;
} /* end of resNodeGetNameAlias() */


/*! Sets and returns the pucAlias of this resource node.

  \param prnArg a pointer to a resource node
  \return pucNameShort or NULL in case of errors
 */
xmlChar*
resNodeGetNameShort(resNodePtr prnArg)
{
  if (prnArg) {
    return prnArg->pucNameShort;
  }
  return NULL;
} /* end of resNodeGetNameShort() */


/*! \return pointer to new resNode or NULL in case of error
*/
resNodePtr
resNodeCwdNew(void)
{
  resNodePtr prnResult = NULL;
  xmlChar *pucT;

  pucT = resPathGetCwdStr();
  if (STR_IS_NOT_EMPTY(pucT)) {
    prnResult = resNodeDirNew(pucT);
    resNodeSetType(prnResult, rn_type_dir);
    xmlFree(pucT);
  }
  return prnResult;
} /* end of resNodeCwdNew() */


/*! \return
*/
resNodePtr
resNodeStrNew(xmlChar *pucArgPath)
{
  resNodePtr prnResult = NULL;
  xmlChar *pucSep;

  if ((pucSep = BAD_CAST xmlStrchr(pucArgPath, PATHLIST_SEPARATOR)) != NULL
    || (pucSep = BAD_CAST xmlStrchr(pucArgPath, (xmlChar) '\n')) != NULL) {
    /* separator PATHLIST_SEPARATOR or newline found */
    xmlChar *puc0;
    xmlChar *pucPath;
    xmlChar ucC = *pucSep;

    pucPath = xmlStrdup(pucArgPath);

    /* handle multiple directory names, separated by ucC */
    for (puc0 = pucPath, (pucSep = (xmlChar *)xmlStrchr((xmlChar *)puc0, ucC));
	 puc0;
	 pucSep = (xmlChar *)xmlStrchr((xmlChar *)puc0, ucC)) {
      resNodePtr prnNew = NULL;

      if (pucSep == puc0) {
	puc0++;
	continue;
      }

      if (pucSep) {
	*pucSep = (xmlChar)'\0';
      }

      if (STR_IS_NOT_EMPTY(puc0)) {
	prnNew = resNodeDirNew(puc0);
	if (prnNew) {
	  if (prnResult == NULL) {
	    prnResult = prnNew; /* first element in result list */
	  }
	  else {
	    resNodeAddSibling(prnResult, prnNew); /* append new element to result list */
	  }
	}
      }

      if (pucSep != NULL && !isend(pucSep[1])) {
	puc0 = pucSep + 1;
      }
      else {
	break;
      }
    }
    xmlFree(pucPath);
  }
#ifdef HAVE_LIBCURL
  else if ((prnResult = resNodeCurlNew(pucArgPath)) != NULL) {
  }
#endif
  else if ((prnResult = resNodeDirNew(pucArgPath)) != NULL) {
  }

  return prnResult;
} /* end of resNodeStrNew() */


#ifdef HAVE_LIBCURL

/*! Creates a new resource node according to pucArgPath.

  \param pucArgPath pointer to a filesystem path
  \return a freshly allocated 'resNode', string pointers set according to 'pucArgPath'
*/
resNodePtr
resNodeCurlNew(xmlChar *pucArgPath)
{
  resNodePtr prnResult = NULL;

  if (STR_IS_NOT_EMPTY(pucArgPath)) { /* a trailing separator indicates a directory */
    xmlChar *pucPath = NULL;
    CURLU *curlURL = NULL;

    curlURL = curl_url(); /* get a handle to work with */
    if (curlURL == NULL) {
      PrintFormatLog(3, "This is not a valid URL '%s'\n", pucArgPath);
    }
    else if (curl_url_set(curlURL, CURLUPART_URL, (const char *)pucArgPath, 0) == CURLUE_OK) { /* parse a full URL */
      prnResult = resNodeNew();
      if (prnResult) {
	xmlChar *pucT = NULL;
	xmlChar *pucTT = NULL;

	if (curl_url_get(curlURL, CURLUPART_HOST, (char **)&pucTT, 0) == CURLUE_NO_HOST) { /* no host name from the parsed URL */
	  /* local file or directory, use pucURLPath but dont handle by curl */

	  if (curl_url_get(curlURL, CURLUPART_PATH, (char **)&pucT, 0) == CURLUE_OK && STR_IS_NOT_EMPTY(pucT)) { /* extract the path from the parsed URL */
	    xmlFree(pucPath);
	    pucPath = xmlStrdup(pucT);
	    DecodeRFC1738((char *)pucPath);

	    prnResult->pucNameNormalized = pucPath;
	  }
	  curl_free(pucT);
	  curl_url_cleanup(curlURL);
	}
	else {
	  char* scheme;

	  if (curl_url_get(curlURL, CURLUPART_URL, (char **)&pucT, 0) == CURLUE_OK) { /* extract the query from the parsed URL */
	    if (STR_IS_NOT_EMPTY(pucT)) {
	      prnResult->pucNameNormalized = xmlStrdup(pucT);
	    }
	  }
	  curl_free(pucT);

	  if (curl_url_get(curlURL, CURLUPART_QUERY, (char **)&pucT, 0) == CURLUE_OK) { /* extract the query from the parsed URL */
	    if (STR_IS_NOT_EMPTY(pucT)) {
	      prnResult->pucQuery = xmlStrdup(pucT);
	    }
	  }
	  curl_free(pucT);

	  if (curl_url_get(curlURL, CURLUPART_SCHEME, (char**)&pucT, 0) == CURLUE_OK) { /* extract the query from the parsed URL */
	    if (xmlStrEqual(BAD_CAST"http", pucT) || xmlStrEqual(BAD_CAST"https", pucT)) {
	      prnResult->eType = rn_type_url_http;
	    }
	    else {
	      prnResult->eType = rn_type_url;
	    }
	  }
	  curl_free(pucT);

	  prnResult->eMode = mode_read; /* by default*/
	  prnResult->curlURL = curlURL; /* use this handle */
	}
	curl_free(pucTT);
      }
      else {
	curl_url_cleanup(curlURL);
      }
    }
    else {
      curl_url_cleanup(curlURL);
    }
  }
  
  return prnResult;
} /* end of resNodeCurlNew() */

#endif

    
/*! Creates a new resource node according to pucArgPath.

  \param pucArgPath pointer to a filesystem path
  \return a freshly allocated 'resNode', string pointers set according to 'pucArgPath'
 */
resNodePtr
resNodeDirNew(xmlChar *pucArgPath)
{
  resNodePtr prnResult = NULL;

  prnResult = resNodeNew();
  if (prnResult) {
    RN_TYPE eType = rn_type_undef;

    if (STR_IS_EMPTY(pucArgPath)) {
      resNodeSetError(prnResult,rn_error_name,"name");
    }
    else if (resPathIsStd(pucArgPath)) {
      resNodeSetType(prnResult, rn_type_stdout);
      prnResult->eAccess = rn_access_std;
      prnResult->pucNameNormalized = xmlStrdup(pucArgPath);
      prnResult->pcNameNormalizedNative = (char *)xmlStrdup(prnResult->pucNameNormalized);
      prnResult->fWrite = TRUE;
      eType = rn_type_stdout;
    }
    else if (resPathIsRoot(pucArgPath)) { /* root of a filesystem */
      prnResult->pucNameNormalized = xmlStrdup(pucArgPath);
      prnResult->pcNameNormalizedNative = (char *)xmlStrdup(prnResult->pucNameNormalized);
      eType = rn_type_root;
    }
    else { /* a trailing separator indicates a directory */
      xmlChar *pucT = NULL;
      xmlChar *pucPath = NULL;
      xmlChar *pucNameArchive = NULL;
      xmlChar *pucNameInArchive = NULL;
      BOOL_T fRecursion = FALSE;

      pucPath = resPathCollapseStr(pucArgPath, FS_PATH_FULL);
#ifdef HAVE_LIBCURL
      CURLU *curlURL = NULL;

      curlURL = curl_url(); /* get a handle to work with */
      if (curlURL == NULL) {
	PrintFormatLog(3, "This is not a valid URL '%s'\n", pucArgPath);
      }
      else if (curl_url_set(curlURL, CURLUPART_URL, (const char *)pucArgPath, 0) == CURLUE_OK) { /* parse a full URL */
	xmlChar *pucTT = NULL;

	if (curl_url_get(curlURL, CURLUPART_HOST, (char **)&pucTT, 0) == CURLUE_NO_HOST) { /* no host name from the parsed URL */
	  /* local file or directory, use pucURLPath but dont handle by curl */
	  if (curl_url_get(curlURL, CURLUPART_PATH, (char **)&pucT, 0) == CURLUE_OK && STR_IS_NOT_EMPTY(pucT)) { /* extract the path from the parsed URL */
	    xmlFree(pucPath);
	    pucPath = xmlStrdup(pucT);
	    DecodeRFC1738((char *)pucPath);
	  }
	  curl_free(pucT);
	  curl_url_cleanup(curlURL);
	}
	else {
	  prnResult->curlURL = curlURL; /* use this handle */
	}
	curl_free(pucTT);
      }
      else {
	curl_url_cleanup(curlURL);
      }
#endif

      /*!\todo handle "mem:///tmp/test.txt" */

#ifdef HAVE_LIBARCHIVE
      pucNameArchive = resPathGetNameOfNextArchivePtr(pucPath);
      if ((pucNameInArchive = resPathGetPathInNextArchivePtr(pucPath)) != NULL && pucNameInArchive > pucPath) {
	/*  */
      }
      else if (pucNameInArchive != NULL) {	   /*  */
	if (resPathIsTrailingSeparator(pucPath)) { /*  */
	  eType = rn_type_dir_in_archive;
	}
	else {
	  eType = rn_type_file_in_archive;
	}
      }
      else if (pucNameArchive != NULL) { /*  */
	eType = rn_type_archive;
      }
      else 
#endif
      if (resPathIsHttpURL(pucPath)) { /*  */
	eType = rn_type_url_http;
      }
      else if (resPathIsFtpURL(pucPath)) { /*  */
	eType = rn_type_url_ftp;
      }
      else if (resPathIsTrailingSeparator(pucPath)) { /*  */
	eType = rn_type_dir;
      }
      else {
	eType = rn_type_file;
      }

      fRecursion = resPathIsDirRecursive(pucArgPath);

      if (STR_IS_EMPTY(pucPath)) {
#ifdef HAVE_CGI
	pucT = resPathGetCwdStr();
#else
	pucT = resPathGetCwdStr();
#endif
	resNodeReset(prnResult, pucT);
	xmlFree(pucT);
	eType = rn_type_dir;
      }
#ifdef HAVE_LIBCURL
      else if (prnResult->curlURL) {

	if (curl_url_get(prnResult->curlURL, CURLUPART_URL, (char **)&pucT, 0) == CURLUE_OK) { /* extract the query from the parsed URL */
	  if (STR_IS_NOT_EMPTY(pucT)) {
	    prnResult->pucNameNormalized = xmlStrdup(pucT);
	  }
	}
	curl_free(pucT);

	if (curl_url_get(prnResult->curlURL, CURLUPART_QUERY, (char **)&pucT, 0) == CURLUE_OK) { /* extract the query from the parsed URL */
	  if (STR_IS_NOT_EMPTY(pucT)) {
	    prnResult->pucQuery = xmlStrdup(pucT);
	  }
	}
	curl_free(pucT);

	if (pucNameArchive) { /* URL contains an archive name */
	  resNodePtr prnArchive;

	  /* cut URL before archive */
	  pucT = xmlStrndup(pucPath, pucNameArchive - pucPath - 1);
	  xmlFree(prnResult->pucNameNormalized);
	  prnResult->pucNameNormalized = pucT;
	  if (curl_url_set(prnResult->curlURL, CURLUPART_PATH, (const char *)prnResult->pucNameNormalized, 0) ==
	      CURLUE_OK) { /* update the path from the parsed URL */
	  }

	  /* add a new node for archive */
	  pucT = xmlStrndup(pucNameArchive, pucNameInArchive - pucNameArchive - 1);
	  prnArchive = resNodeAddChildNew(prnResult, pucT);
	  resNodeSetType(prnArchive, rn_type_archive);

	  /* add a node for archive path */
	  if (pucNameInArchive) { /* URL contains a name into archive also */
	    resNodeSetType(resNodeAddChildNew(prnArchive, pucNameInArchive), rn_type_file_in_archive);
	  }
	}
	/*\todo handle sqlite in URL */
	eType = rn_type_url;
      }
#endif
#ifdef HAVE_LIBARCHIVE
      else if (resPathIsInArchive(pucPath)) {
	xmlChar *pucZip = NULL;

#ifdef HAVE_CGI
	pucT = resPathGetCwdStr();
#else
	pucT = resPathGetCwdStr();
#endif

	pucZip = resPathGetPathOfArchiveStr(pucPath);
	if (pucZip) {
	  xmlChar *pucTT = NULL;

	  if (resPathIsRelative(pucZip)) {
	    resNodeReset(prnResult, pucT);
	    resNodeConcat(prnResult, pucZip);
	  }
	  else {
	    resNodeReset(prnResult, pucZip);
	  }
	  eType = rn_type_archive;

	  pucTT = resPathGetPathInNextArchivePtr(pucPath);
	  resNodeAddChildNew(prnResult, pucTT);
	  xmlFree(pucZip);
	}
	else {
	  resNodeReset(prnResult, pucT);
	  resNodeConcat(prnResult, pucPath);
	}
	xmlFree(pucT);
      }
#endif
      else if (resPathIsRelative(pucPath)) {
	pucT = resPathGetCwdStr();
	resNodeReset(prnResult, pucT);
	resNodeConcat(prnResult, pucPath);
	xmlFree(pucT);
	if (resPathIsTrailingSeparator(pucPath)) { /*  */
	  eType = rn_type_dir;
	}
      }
      else if (resPathIsFileURL(pucPath)) { /* without drive letter */
	int i;

	assert(xmlStrlen(pucPath) > 7);
#ifdef _MSC_VER
	for (i = 6; issep(pucPath[i]); i++);
#else
	for (i = 6; issep(pucPath[i + 1]); i++);
#endif
	prnResult->pucNameNormalized = xmlStrdup(&pucPath[i]);
	if (resPathIsTrailingSeparator(pucPath)) { /*  */
	  eType = rn_type_dir;
	}
      }
#ifdef _MSC_VER
      else if (resPathIsUNC(pucPath)) {
	prnResult->pucNameNormalized = xmlStrdup(pucPath);
	if (resPathIsTrailingSeparator(pucPath)) { /*  */
	  eType = rn_type_dir;
	}
      }
      else if (resPathIsLeadingSeparator(pucPath)) { /* without drive letter */
	pucT = resPathGetCwdStr();
	prnResult->pucNameNormalized = xmlStrndup(pucT, 3);
	resNodeConcat(prnResult, pucPath);
	xmlFree(pucT);
	if (resPathIsTrailingSeparator(pucPath)) { /*  */
	  eType = rn_type_dir;
	}
      }
#endif
      else {
	resNodeReset(prnResult, pucPath);
	if (resPathIsTrailingSeparator(pucPath)) { /*  */
	  resNodeSetRecursion(prnResult, fRecursion);
	// resPathCutTrailingChars(prnResult->pucNameNormalized);
	  eType = rn_type_dir;
	}
      }
      xmlFree(pucPath);
    }
    resNodeSetType(prnResult, eType);
  }
  return prnResult;
} /* end of resNodeDirNew() */


/*! split the multi-line buffer pucArg into single resource nodes

\param pucArg a pointer to buffer
\return new ancestor resource node
*/
resNodePtr
resNodeSplitLineBufferNew(xmlChar* pucArg)
{
  resNodePtr prnResult = NULL;

  if (STR_IS_NOT_EMPTY(pucArg)) {
    int i;
    int j;
    int l;

    for (i=0, j=0, l=0; ; ) {

      if (isend(pucArg[i]) || pucArg[i] == '\n' || pucArg[i] == '\r') {
	int k;
	xmlChar* pucPath;

	if (i == j) {
	  if (isend(pucArg[i])) {
	    break;
	  }

	  while (pucArg[i] == '\n' || pucArg[i] == '\r') {
	    i++; /* skip multiple line breaks */
	  }
	  j = i;
	  continue;
	}

	/* check if the string is of blank chars only */
	for (pucPath=NULL, k=j; k < i; k++) {
	  if (isspace(pucArg[k])) {
	  }
	  else {
	    pucPath = xmlStrndup(&(pucArg[j]), i - j);
	    break;
	  }
	}

	if (STR_IS_NOT_EMPTY(pucPath)) {
	  if (l > 0) {
	    assert(prnResult);
	    resNodeInsertStrNew(prnResult, pucPath);
	  }
	  else {
	    prnResult = resNodeSplitStrNew(pucPath);
	  }
	  l++;
	}
	xmlFree(pucPath);

	if (isend(pucArg[i])) {
	  break;
	}

	while (pucArg[i] == '\n' || pucArg[i] == '\r') {
	  i++; /* skip multiple line breaks */
	}
	j = i;
      }
      else {
	i++;
      }
    }
  }
  return prnResult;
} /* end of resNodeSplitLineBufferNew() */


/*! split a prnArg into its single ancestor resource nodes

\param prnArg a pointer to a resource node without a child node
\return TRUE as splitting result
*/
resNodePtr
resNodeSplitStrNew(xmlChar* pucArgPath)
{
  resNodePtr prnResult = NULL;

  if (STR_IS_NOT_EMPTY(pucArgPath)) { /* a trailing separator indicates a directory */

    prnResult = resNodeNew();
    if (prnResult) {
      xmlChar* pucSep;
      xmlChar* pucStart;
      resNodePtr prnT;
      xmlChar* pucT;
      xmlChar* pucTT;

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

      for (prnT = prnResult, pucSep = pucTT; (pucSep = resPathGetNextSeparatorPtr(pucSep)) != NULL; pucTT = pucSep) {
	while (issep(*pucSep) || isquot(*pucSep)) {
	  pucSep++;
	}

	if ((pucT = xmlStrndup(pucTT, (int)(pucSep - pucTT)))) {
	  prnT = resNodeAddChildNew(prnT, pucT);
	  xmlFree(pucT);
	}

	if (isend(*pucSep)) {
	  pucSep = NULL; /* empty */
	}
      }

      if (xmlStrlen(pucTT) > 0) {
	prnT = resNodeAddChildNew(prnT, pucTT);
      }

      xmlFree(pucStart);
    }
  }
  return prnResult;
} /* end of resNodeSplitStrNew() */


/*! inserts a new resource node for pucArgPath into the tree of prnArg

\param prnArgTree a pointer to a resource node tree
\param prnArg a pointer to an resource node list to insert into 'prnArgTree'

\return 
*/
resNodePtr
resNodeInsert(resNodePtr prnArgTree, resNodePtr prnArg)
{
  resNodePtr prnResult = prnArg;
  resNodePtr prnPath;
  resNodePtr prnTree;

  for (prnPath = prnArg, prnTree = prnArgTree; prnPath != NULL && prnTree != NULL; ) {
    BOOL_T fMatch;
    resNodePtr prnT;

    do {
      prnT = prnTree;
      fMatch = resPathIsEquivalent(resNodeGetNameBase(prnPath), resNodeGetNameBase(prnTree));
      if (fMatch) {
	if (prnPath->children) {
	  if (prnTree->children) {
	    prnPath = prnPath->children;
	    prnTree = prnTree->children;
	    break; /* step to childrens */
	  }
	  else {
	    /* that was the last node in 'prnTree', append childs of 'prnPath' to 'prnTree' */
	    prnResult = prnPath->children;
	    resNodeAddChild(prnTree, prnResult);
	    return prnResult;
	  }
	}
	else {
	  /* that was the last node in 'prnPath', it's included already */
	  prnResult = prnArgTree;
	  return prnResult;
	}
      }
    } while ((prnTree = prnTree->next) != NULL);

    if (fMatch) {
    }
    else {
      prnResult = prnPath;
      resNodeAddSibling(prnT, prnResult);
      break;
    }

  }
  
  return prnResult;
} /* end of resNodeInsert() */


/*! inserts a new resource node for pucArgPath into the tree of prnArg

\param prnArg a pointer to an existing resource node
\return pointer to a freshly allocated 'resNode'
*/
resNodePtr
resNodeInsertStrNew(resNodePtr prnArg, xmlChar* pucArgPath)
{
  resNodePtr prnResult = NULL;

  if (STR_IS_NOT_EMPTY(pucArgPath)) { /* a trailing separator indicates a directory */
    resNodePtr prnT;

    prnT = resNodeSplitStrNew(pucArgPath);
    if (prnT) {
      prnResult = resNodeInsert(prnArg,prnT);
      if (prnResult == prnArg || prnResult != prnT) {
	resNodeFree(prnT);
      }
    }
  }
  return prnResult;
} /* end of resNodeInsertStrNew() */


/*! Creates a new resource node according to concatenated 'pucArgPathA' and 'pucArgPathB'

\param pucArgPathA pointer to first filesystem path
\param pucArgPathB pointer to second filesystem path
\return pointer to a freshly allocated 'resNode'
*/
resNodePtr
resNodeConcatNew(xmlChar *pucArgPathA, xmlChar *pucArgPathB)
{
  resNodePtr prnResult = NULL;

  if (STR_IS_NOT_EMPTY(pucArgPathB)) {
    prnResult = resNodeNew();
    if (prnResult) {
      xmlChar *pucT = NULL;
      xmlChar *pucTT;

      if (resPathIsFileURL(pucArgPathB)) {
	pucT = resPathCollapseStr(pucArgPathB, FS_PATH_FULL);
      }
      else if (resPathIsURL(pucArgPathB)) {
	pucT = xmlStrdup(pucArgPathB);
      }
      else if (resPathIsUNC(pucArgPathB)) {
	pucT = resPathCollapseStr(pucArgPathB, FS_PATH_FULL);
      }
      else if (resPathIsAbsolute(pucArgPathB)) {
	/* value of pucArgPathB is absolute */
	pucT = resPathCollapseStr(pucArgPathB, FS_PATH_FULL);
      }
      else if (STR_IS_NOT_EMPTY(pucArgPathA)) {
	pucT = resPathConcatNormalizedStr(pucArgPathA, pucArgPathB);
      }

      if ((pucTT = resPathGetPathOfArchiveStr(pucT))) {
	xmlChar *pucTTT;

	pucTTT = resPathGetPathInNextArchivePtr(pucT);
	resNodeReset(prnResult, pucTT);
	if (STR_IS_NOT_EMPTY(pucTTT) && prnResult != NULL) {
	  resNodeAddChildNew(prnResult, pucTTT);
	}
	xmlFree(pucTTT);
      }
      else {
	//resPathCutTrailingChars(pucT);
	resNodeReset(prnResult, pucT);
	if (resPathIsDirRecursive(pucArgPathB)) {
	  resNodeSetRecursion(prnResult, TRUE);
	  resNodeSetType(prnResult, rn_type_dir);
	}
	else if (resPathIsDir(pucArgPathB)) { /* value of pucArgPathB is has a trailing separator */
	  resNodeSetType(prnResult, rn_type_dir);
	}
      }
      xmlFree(pucT);
    }
  }
  return prnResult;
} /* end of resNodeConcatNew() */


/*! Creates a new comment resource node (plain text file to store file
    comments) according to 'prnArg'

  \param prnArg a pointer to a resource node
  \return a freshly allocated 'resNode'
 */
resNodePtr
resNodeCommentNew(resNodePtr prnArg)
{
  resNodePtr prnResult = NULL;

  if (prnArg) {
    prnResult = resNodeDup(prnArg, RN_DUP_THIS);
    if (prnResult) {
      /*
	derive name of comment file from image file
       */
      xmlChar *pucNameFileComment;
      xmlChar *pucT;

      pucNameFileComment = xmlStrdup(resNodeGetNameNormalized(prnArg));
      for ( pucT = pucNameFileComment + xmlStrlen(pucNameFileComment); pucT > pucNameFileComment; pucT-- ) {
	if (*pucT == (xmlChar)'.') {
	  *pucT = (xmlChar)'_';
	  break;
	}
      }

      pucNameFileComment = xmlStrcat(pucNameFileComment,BAD_CAST ".txt");
      resNodeReset(prnResult,pucNameFileComment);
      xmlFree(pucNameFileComment);
    }
  }
  return prnResult;
} /* end of resNodeCommentNew() */


/*! Concatenates pucArgPath to existing resource node.

  \param prnArg a pointer to a resource node
  \param pucArgPath pointer to a filesystem path
  \return TRUE if existing resource node concatenated 'pucArgPath'
 */
BOOL_T
resNodeConcat(resNodePtr prnArg, xmlChar *pucArgPath)
{
  BOOL_T fResult = FALSE;
  xmlChar *pucT;

  if (prnArg != NULL && resNodeGetNameNormalized(prnArg) != NULL && pucArgPath != NULL) {
    if (resPathIsInArchive(pucArgPath)) {
      xmlChar *pucRelease;

      pucRelease = resPathGetPathOfArchiveStr(pucArgPath);
      if (pucRelease) {
	xmlChar *pucTT;

	pucT = resPathConcatNormalizedStr(resNodeGetNameNormalized(prnArg),pucRelease);
	resNodeReset(prnArg,pucT);

	pucTT = pucArgPath + xmlStrlen(pucRelease);
	while (STR_IS_NOT_EMPTY(pucTT) && issep(*pucTT)) pucTT++;
	resNodeAddChildNew(prnArg,pucTT);
	/*!\todo recursion in archives? */
	xmlFree(pucT);
	xmlFree(pucRelease);
      }
    } 
    else {
      pucT = resPathConcatNormalizedStr(resNodeGetNameNormalized(prnArg),pucArgPath);
      resNodeReset(prnArg,pucT);
      if (resPathIsDir(pucArgPath)) { /* value of pucArgPath is has a trailing separator */
	resNodeSetType(prnArg, rn_type_dir);
	resNodeSetRecursion(prnArg, resPathIsDirRecursive(pucArgPath));
      }
      xmlFree(pucT);
    }
  }
  return fResult;
} /* end of resNodeConcat() */


/*! Concatenates pucArgPath to existing resource node.

  \param prnArg a pointer to a resource node
  \param pucArgPath pointer to a filesystem path
  \return a freshly allocated 'resNode'
  */
resNodePtr
resNodeFromNodeNew(resNodePtr prnArg, xmlChar *pucArgPath)
{
  resNodePtr prnResult = NULL;

  if (pucArgPath) {

    if (resPathIsStd(pucArgPath)) {
      prnResult = resNodeDirNew(pucArgPath);
    }
    else if (resPathIsInMemory(pucArgPath)) {
      prnResult = resNodeInMemoryNew();
    }
    else if (resPathIsInArchive(pucArgPath)) {
      xmlChar *pucRelease;
      
      pucRelease = resPathGetPathOfArchiveStr(pucArgPath);
      if (STR_IS_NOT_EMPTY(pucRelease)) {
	xmlChar *pucT;

	pucT = resPathGetPathInNextArchivePtr(pucArgPath);
	prnResult = resNodeFromNodeNew(prnArg, pucRelease);
	if (STR_IS_NOT_EMPTY(pucT) && prnResult != NULL) {
	  resNodeAddChildNew(prnResult, pucT);
	}
	xmlFree(pucT);
	xmlFree(pucRelease);
      }
    }
    else if (resPathIsRelative(pucArgPath)) {
      if (prnArg) {
	prnResult = resNodeDup(prnArg, RN_DUP_THIS);
	if (resNodeIsFile(prnResult)) {
	  resNodeSetToParent(prnResult);
	}
      }
      else {
	prnResult = resNodeCwdNew();
      }

      resNodeConcat(prnResult,pucArgPath);
    }
    else {
      prnResult = resNodeDirNew(pucArgPath);
    }
  }
  return prnResult;
} /* end of resNodeFromNodeNew() */


/*! Sets pcNameBaseNative of an existing resource node to pcArg
  (especially for directory iterator resNodeDirAppendEntries())

  \param prnArg a pointer to a resource node
  \param pcArg pointer to a new native basename
  \return TRUE if existing resource node is set to new value (without encoding errors)
 */
BOOL_T
resNodeSetNameBaseNative(resNodePtr prnArg, char *pcArg)
{
  BOOL_T fResult = FALSE;

  if (prnArg && pcArg != NULL && strlen(pcArg) > 0) {
    char *pcBaseNativeNew;

    pcBaseNativeNew = (char *)xmlStrdup(BAD_CAST pcArg);
    if (STR_IS_NOT_EMPTY(pcBaseNativeNew)) {
      xmlChar *pucBaseNew;

      pucBaseNew = resPathEncodeStr(pcBaseNativeNew);
      if (STR_IS_NOT_EMPTY(pucBaseNew)) {
	resNodeSetNameBase(prnArg,pucBaseNew);
	fResult = (resNodeGetError(prnArg) == rn_error_none);
      }
      else {
	resNodeSetError(prnArg,rn_error_encoding,"encoding");
      }
      xmlFree(pucBaseNew);
    }
    else {
      resNodeSetError(prnArg,rn_error_name,"name");
    }
    xmlFree(pcBaseNativeNew);
  }
  return fResult;
} /* end of resNodeSetNameBaseNative() */


/*! re-sets pucAlias of an existing resource node to pucArg

  \param prnArg a pointer to a resource node
  \param pucArg pointer to a new native basename
  \return TRUE if existing resource node is set to new value (without encoding errors)
 */
BOOL_T
resNodeSetNameAlias(resNodePtr prnArg, xmlChar *pucArg)
{
  BOOL_T fResult = FALSE;

  if (prnArg) {
    prnArg->pucAlias = xmlStrdup(pucArg);
    fResult = (prnArg->pucAlias != NULL);
  }
  return fResult;
} /* end of resNodeSetNameAlias() */


/*! Sets pucNameBase of an existing resource node to pucArgPath and keep prnArg->pucNameBaseDir etc.

\param prnArg a pointer to a resource node
\param pucArgPath pointer to a new native basename
\return TRUE if existing resource node is set to new value (without encoding errors)
*/
BOOL_T
resNodeSetNameBase(resNodePtr prnArg, xmlChar *pucArgPath)
{
  BOOL_T fResult = FALSE;

  if (prnArg) {
    xmlChar *pucPathNew;

    pucPathNew = resPathConcatStr(resNodeGetNameBaseDir(prnArg), pucArgPath);
    resNodeReset(prnArg, pucPathNew);
    xmlFree(pucPathNew);
    prnArg->pucNameBase = xmlStrdup(pucArgPath);

    fResult = TRUE;
  }
  return fResult;
} /* end of resNodeSetNameBase() */


/*! Sets pucNameBase of an existing resource node to pucArgPath and keep prnArg->pucNameBaseDir etc.

\param prnArg a pointer to a resource node
\param pucArgPath pointer to a new native short name
\return TRUE if existing resource node is set to new value (without encoding errors)
*/
BOOL_T
resNodeSetNameShort(resNodePtr prnArg, xmlChar* pucArgShort)
{
  BOOL_T fResult = FALSE;

  if (prnArg) {
    xmlFree(prnArg->pucNameShort);
    prnArg->pucNameShort = xmlStrdup(pucArgShort);
    //assert(StringEndsWith(resNodeGetNameNormalized(prnArg),prnArg->pucNameShort));
    fResult = (prnArg->pucNameShort != NULL);
  }
  return fResult;
} /* end of resNodeSetNameShort() */


/*! Sets pucNameBase of an existing resource node to pucArgPath and keep prnArg->pucNameBaseDir etc.

\param prnArg a pointer to a resource node
\param pucArgPath pointer to a new native basename
\return TRUE if existing resource node is set to new value (without encoding errors)
*/
BOOL_T
resNodeSetNameBaseDir(resNodePtr prnArg, xmlChar* pucArgPath)
{
  BOOL_T fResult = FALSE;

  if (prnArg) {
    if (resNodeIsDir(prnArg) || resNodeIsFile(prnArg)) {
      resNodeResetNameBase(prnArg);
      xmlFree(prnArg->pcNameNormalizedNative);
      prnArg->pcNameNormalizedNative = NULL;
      xmlFree(prnArg->pucNameNormalized);
      prnArg->pucNameNormalized = NULL;
      xmlFree(prnArg->pucNameBaseDir);
      prnArg->pucNameBaseDir = resPathCollapseStr(pucArgPath, FS_PATH_FULL);
      resNodeSetNameBaseDir(resNodeGetChild(prnArg), NULL);
    }
    resNodeSetNameBaseDir(resNodeGetNext(prnArg), pucArgPath);
    fResult = TRUE;
  }
  return fResult;
} /* end of resNodeSetNameBaseDir() */


/*! Resets all values depending on pucNameNormalized.

  \param prnArg a pointer to a resource node
  \return TRUE if successful
 */
BOOL_T
resNodeResetNameBase(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (prnArg != NULL && STR_IS_NOT_EMPTY(prnArg->pucNameNormalized)) {
    /*
      UTF-8 normalized path is leading, build basenames
    */
    xmlChar *pucT;

    xmlFree(prnArg->pucNameBaseDir);
    prnArg->pucNameBaseDir = resPathGetBasedirStr(prnArg->pucNameNormalized);

    xmlFree(prnArg->pucNameBase);
    prnArg->pucNameBase = NULL;

    xmlFree(prnArg->pcNameBaseNative);
    prnArg->pcNameBaseNative = NULL;

    if (resNodeIsRoot(prnArg)) {
      fResult = TRUE;
    }
    else if ((pucT = resPathGetBasenameStr(prnArg->pucNameNormalized)) != NULL) {
      if (xmlStrlen(pucT) > 0) {
	prnArg->pucNameBase = pucT;
	prnArg->pcNameBaseNative = resPathDecodeStr(prnArg->pucNameBase);
	if (STR_IS_NOT_EMPTY(prnArg->pcNameBaseNative)) {
	  fResult = TRUE;
	}
	else {
	  resNodeSetError(prnArg,rn_error_encoding,"encoding");
	}
      }
      else {
	xmlFree(pucT);
	resNodeSetError(prnArg,rn_error_name,"name");
      }
    }
    else {
      resNodeSetError(prnArg,rn_error_name,"name");
    }
    
#ifdef _MSC_VER
    /*! check length of prnResult->pucNameBase against MAX_PATH */
    if (xmlStrlen(prnArg->pucNameBaseDir) < MAX_PATH && xmlStrlen(prnArg->pucNameBase) < MAX_PATH) {
    }
    else {
      resNodeSetError(prnArg,rn_error_max_path,"rn_error_max_path");
    }
#endif
  }

  return fResult;
} /* end of resNodeResetNameBase() */


/*! Changes resource node to its parent directory.

  \param prnArg a pointer to a resource node
  \return TRUE if successful
 */
BOOL_T
resNodeSetToParent(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (prnArg) {
    fResult = resNodeReset(prnArg, resNodeGetNameBaseDir(prnArg));
    prnArg->eType = rn_type_dir; /*\bug if rn_type_dir_in_archive */
  }
  return fResult;
} /* end of resNodeSetToParent() */


/*! resets an existing prnArg according to pucArgPath

  \param prnArg a pointer to a resource node to reset
  \param pucArgPath pointer to a filesystem path
  \return TRUE if successful
 */
BOOL_T
resNodeReset(resNodePtr prnArg, xmlChar *pucArgPath)
{
  BOOL_T fResult = FALSE;

  if (prnArg) {
    xmlChar *pucArgPathCopy = NULL;
    resNodePtr prnChild;
    resNodePtr prnChildNext;
    
    if (STR_IS_NOT_EMPTY(pucArgPath)) {
      pucArgPathCopy = xmlStrdup(pucArgPath); /* if pucArgPath is prnArg->pucNameBaseDir ... */
    }

#ifdef HAVE_LIBCURL
     /*! - free url handle */
    if (prnArg->curlURL != NULL) {
      curl_url_cleanup(prnArg->curlURL);
    }
#endif

    /*! - close file and directory handle */
    if (prnArg->handleIO != NULL) {
      resNodeClose(prnArg);
    }

    /*! - release child nodes */
    for (prnChild = resNodeGetChild(prnArg); prnChild != NULL; prnChild = prnChildNext) {
      prnChildNext = resNodeGetNext(prnChild);
      resNodeFree(prnChild);
    }

    /*! - release node properties */
    if (prnArg->ppucProp) {
      size_t i;

      for (i=0; prnArg->ppucProp[i]; i += 2) {
	xmlFree((prnArg->ppucProp[i]));
	xmlFree((prnArg->ppucProp[i+1]));
      }
      xmlFree(prnArg->ppucProp);
    }

    /*! - release allocated memory of this resource node */

    xmlFree(prnArg->pucMsg);
    xmlFree(prnArg->pucAlias);
    xmlFree(prnArg->pucMtime);
    xmlFree(prnArg->pucOwner);
    xmlFree(prnArg->pucQuery);
    xmlFree(prnArg->pucObject);
    xmlFree(prnArg->pContent);
    xmlFreeDoc(prnArg->pdocContent);
    xmlFree(prnArg->pucNameBase);
    xmlFree(prnArg->pcNameBaseNative);
    xmlFree(prnArg->pucNameBaseDir);
    xmlFree(prnArg->pucNameShort);
    xmlFree(prnArg->pucURI);
    xmlFree(prnArg->pucExtension);
    xmlFree(prnArg->pcNameNormalizedNative);
    xmlFree(prnArg->pucNameNormalized);

    /*! - set all pointer to NULL and other values to 0 */
    memset(prnArg,0,sizeof(resNode));

    /*! - set new normalized path */
    if (STR_IS_NOT_EMPTY(pucArgPathCopy)) {
      int iLength = 0;
      xmlChar *pucCwd;

      iLength = xmlStrlen(pucArgPathCopy);
      if (resPathIsURL(pucArgPathCopy)) {
	/*!\todo use new fsURLNormalize(pucArgPathCopy,NULL) */
	prnArg->pucNameNormalized = xmlStrdup(pucArgPathCopy);
      }
      else if (resPathIsUNC(pucArgPathCopy)) {
	prnArg->pucNameNormalized = resPathCollapseStr(pucArgPathCopy, FS_PATH_FULL);
      }
      else if (resPathIsRelative(pucArgPathCopy)) {
	if ((pucCwd = resPathGetCwdStr())) {
	  prnArg->pucNameNormalized = resPathConcatNormalizedStr(pucCwd, pucArgPathCopy);
	  xmlFree(pucCwd);
	}
      }
#ifdef _MSC_VER
      else if (issep(pucArgPathCopy[0])) { /* DOS path without drive letter */
	if ((pucCwd = resPathGetCwdStr())) {
          xmlChar *pucT;

	  pucT = xmlStrndup(pucCwd, 3); /* drive letter + ':' + '\' */
	  prnArg->pucNameNormalized = resPathConcatNormalizedStr(pucT, pucArgPathCopy);
	  xmlFree(pucT);
	  xmlFree(pucCwd);
	}
      }
#endif
      else {
	prnArg->pucNameNormalized = resPathCollapseStr(pucArgPathCopy, FS_PATH_FULL);
      }

#ifdef _MSC_VER
	/*! - check length of path */
      if (prnArg->pucNameNormalized != NULL && xmlStrlen(prnArg->pucNameNormalized) < MAX_PATH) {
      }
      else {
	resNodeSetError(prnArg,rn_error_max_path,"max_path");
	return fResult;
      }
#endif

      resNodeSetRecursion(prnArg,resPathIsDirRecursive(pucArgPathCopy));

      /*! - convert UTF-8 name (is leading) into native, build normalized path */
      if ((prnArg->pcNameNormalizedNative = resPathDecodeStr(prnArg->pucNameNormalized)) == NULL) {
	resNodeSetError(prnArg,rn_error_encoding,"encoding");
      }

#ifdef _MSC_VER
      if (prnArg->pcNameNormalizedNative != NULL && (iLength = (int)strlen(prnArg->pcNameNormalizedNative)) > 1) {
	xmlChar *pucEnd = BAD_CAST &(prnArg->pcNameNormalizedNative[iLength]);
	/*! - cut all redundant trailing path separators */
	for (pucEnd--; pucEnd > BAD_CAST prnArg->pcNameNormalizedNative && issep(*pucEnd) && issep(*(pucEnd - 1)); pucEnd--) {
	  *pucEnd = (xmlChar)'\0';
	}

	/*! - check length of prnResult->pcNameNormalizedNative against MAX_PATH */
	if (strlen(prnArg->pcNameNormalizedNative) < MAX_PATH) {
	}
	else {
	  resNodeSetError(prnArg,rn_error_max_path,"max_path");
	  return fResult;
	}
      }
#endif
      xmlFree(pucArgPathCopy);
    }
    resNodeResetDetails(prnArg);
    fResult = TRUE;
  }
  return fResult;
} /* end of resNodeReset() */


/*! frees tree of prnArg

  \param prnArg a pointer to a resource node tree to free
 */
void
resNodeFree(resNodePtr prnArg)
{
  if (prnArg) {
    resNodeListUnlink(prnArg);
    resNodeListFree(resNodeGetChild(prnArg));
    resNodeReset(prnArg, NULL);
    xmlFree(prnArg);
  }
} /* end of resNodeFree() */


/*! \return TRUE if prnArg is a directory
 */
BOOL_T
resNodeIsDir(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (prnArg) {
    if (resNodeGetType(prnArg) == rn_type_undef) {
      if (resNodeGetMimeType(prnArg) == MIME_INODE_DIRECTORY) {
	if (resNodeGetType(resNodeGetParent(prnArg)) == rn_type_archive) {
	  resNodeSetType(prnArg, rn_type_dir_in_archive);
	}
	if (resNodeGetType(resNodeGetParent(prnArg)) == rn_type_zip) {
	  resNodeSetType(prnArg, rn_type_dir_in_zip);
	}
	else {
	  resNodeSetType(prnArg, rn_type_dir);
	}
      }
      else {
	resNodeReadStatus(prnArg);
      }
    }
    fResult = (resNodeGetType(prnArg) == rn_type_root || resNodeGetType(prnArg) == rn_type_dir || resNodeGetType(prnArg) == rn_type_dir_in_archive || resNodeGetType(prnArg) == rn_type_dir_in_zip); //  || resNodeGetChild(prnArg) != NULL
  }
  return fResult;
} /* end of resNodeIsDir() */


/*! \return TRUE if prnArg is root of a file system
*/
BOOL_T
resNodeIsRoot(resNodePtr prnArg)
{
  return resNodeGetType(prnArg) == rn_type_root;
} /* end of resNodeIsRoot() */


/*! \return TRUE if prnArg is a directory
 */
BOOL_T
resNodeIsRecursive(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (prnArg) {
    fResult = ((resNodeGetType(prnArg) == rn_type_dir || resNodeGetType(prnArg) == rn_type_dir_in_archive || resNodeGetType(prnArg) == rn_type_archive)
	       && prnArg->fRecursive);
  }
  return fResult;
} /* end of resNodeIsRecursive() */


/*! \return TRUE if prnArg is a URL
 */
BOOL_T
resNodeIsURL(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (prnArg) {
    if (resNodeGetType(prnArg) == rn_type_undef) {
      if (resPathIsHttpURL(prnArg->pucNameNormalized)) {
	resNodeSetType(prnArg,rn_type_url_http);
      }
      else if (resPathIsFtpURL(prnArg->pucNameNormalized)) {
	resNodeSetType(prnArg,rn_type_url_ftp);
      }
    }
    fResult = (resNodeGetType(prnArg) == rn_type_url || resNodeGetType(prnArg) == rn_type_url_ftp || resNodeGetType(prnArg) == rn_type_url_http);
#ifdef HAVE_LIBCURL
    fResult |= (prnArg->curlURL != NULL);
#endif
  }
  return fResult;
} /* end of resNodeIsURL() */


/*! \return TRUE if prnArg is stdout, stdin
 */
BOOL_T
resNodeIsStd(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (prnArg != NULL && resNodeGetError(prnArg) == rn_error_none) {
    if (resNodeGetType(prnArg) == rn_type_undef) {
      resNodeResetMimeType(prnArg);
    }
    fResult = (resNodeGetType(prnArg) == rn_type_stdin || resNodeGetType(prnArg) == rn_type_stdout || resNodeGetType(prnArg) == rn_type_stderr);
  }
  return fResult;
} /* end of resNodeIsStd() */


/*! \return TRUE if prnArg is a symbolic filesystem link
 */
BOOL_T
resNodeIsLink(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (prnArg) {
    if (resNodeGetType(prnArg) == rn_type_undef) {
      resNodeReadStatus(prnArg);
    }
    fResult = (resNodeGetType(prnArg) == rn_type_symlink);
  }
  return fResult;
} /* end of resNodeIsLink() */


/*! \return TRUE if prnArg is a memory
 */
BOOL_T
resNodeIsMemory(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (prnArg) {
    fResult = (prnArg->pdocContent != NULL || prnArg->pContent != NULL || prnArg->eType == rn_type_memory);
  }
  return fResult;
} /* end of resNodeIsMemory() */


/*! \return TRUE if prnArg is a file
 */
BOOL_T
resNodeIsFile(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (prnArg) {
    if (resNodeGetType(prnArg) == rn_type_undef) {
      resNodeReadStatus(prnArg);
    }
    fResult = (resNodeGetType(prnArg) == rn_type_file || resNodeGetType(prnArg) == rn_type_archive || resNodeGetType(prnArg) == rn_type_file_in_archive || resNodeGetType(prnArg) == rn_type_file_in_zip);
  }
  return fResult;
} /* end of resNodeIsFile() */


/*! \return TRUE if prnArg is a LARGE file

\todo use fopen64(), ftello64() etc (s. minizip.c)
*/
BOOL_T
resNodeIsLargeFile(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (resNodeIsFile(prnArg)) {
    FILE* pFile;

    pFile = fopen(resNodeGetNameNormalizedNative(prnArg),"rb");
    if (pFile) {
      fResult = (fseek(pFile,0,SEEK_END) == 0) && (ftell(pFile) >= 0xffffffff);
      fclose(pFile);
    }
  }
  return fResult;
} /* end of resNodeIsLargeFile() */


/*! \return TRUE if prnArg is a sqlite database file
 */
BOOL_T
resNodeIsSqlite(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (prnArg) {
    if (resNodeGetType(prnArg) == rn_type_memory) {
      /*!\todo improve sqlite detection */
      fResult = TRUE;
    }
    else {
      if (resNodeGetType(prnArg) == rn_type_undef) {
	resNodeReadStatus(prnArg);
	resNodeResetMimeType(prnArg);
      }
      fResult = (resNodeIsFile(prnArg) && resNodeGetMimeType(prnArg) == MIME_APPLICATION_X_SQLITE3);
    }
  }
  return fResult;
} /* end of resNodeIsSqlite() */


/*! \return TRUE if prnArg is a processable database file
 */
BOOL_T
resNodeIsDatabase(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (prnArg) {
    fResult = resMimeIsDatabase(resNodeGetMimeType(prnArg));
  }
  return fResult;
} /* end of resNodeIsDatabase() */


/*! \return TRUE if prnArg is a processable picture file
*/
BOOL_T
resNodeIsPicture(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (prnArg) {
    fResult = resMimeIsPicture(resNodeGetMimeType(prnArg));
  }
  return fResult;
} /* end of resNodeIsPicture() */


/*! \return TRUE if prnArg is a processable shortcut file
*/
BOOL_T
resNodeIsShortcut(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;
  xmlChar *pucT;

  if (resNodeIsFile(prnArg) && (pucT = resNodeGetNameBase(prnArg)) != NULL && xmlStrlen(pucT) > 0) {
    fResult = resPathIsEquivalent(pucT, BAD_CAST NAME_FILE_SHORTCUTS ".pie") || resPathIsEquivalent(pucT, BAD_CAST NAME_FILE_SHORTCUTS ".md") ||
	      resPathIsEquivalent(pucT, BAD_CAST NAME_FILE_SHORTCUTS ".txt") || resPathIsEquivalent(pucT, BAD_CAST NAME_FILE_SHORTCUTS ".html");
  }
  return fResult;
} /* end of resNodeIsShortcut() */


/*! \return TRUE if prnArg is a processable shortcut file
*/
BOOL_T
resNodeIsIndex(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (resNodeIsFile(prnArg)) {
    fResult = resPathIsEquivalent(resNodeGetNameBase(prnArg), BAD_CAST NAME_FILE_INDEX);
  }
  return fResult;
} /* end of resNodeIsIndex() */


/*! \return TRUE if prnArg is a processable video file
*/
BOOL_T
resNodeIsVideo(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (prnArg) {
    fResult = resMimeIsVideo(resNodeGetMimeType(prnArg));
  }
  return fResult;
} /* end of resNodeIsVideo() */


/*! \return TRUE if prnArg is a processable archive file
*/
BOOL_T
resNodeIsArchive(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (prnArg) {
    fResult = resMimeIsArchive(resNodeGetMimeType(prnArg));
  }
  return fResult;
} /* end of resNodeIsArchive() */


/*! \return TRUE if prnArg is a file in a parent archive file
 */
BOOL_T
resNodeIsFileInArchive(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (prnArg) {
    if (prnArg->eType == rn_type_file_in_archive || prnArg->eType == rn_type_file_in_zip) {
      fResult = TRUE;
    }
    else if (prnArg->eType == rn_type_file) {
      resNodePtr prnI = NULL;

      for (prnI = resNodeGetParent(prnArg); prnI; prnI = resNodeGetParent(prnI)) {
	if (prnI->eType == rn_type_archive || prnI->eType == rn_type_dir_in_archive || prnI->eType == rn_type_zip || prnI->eType == rn_type_dir_in_zip) {
	  fResult = TRUE;
	  break;
	}
      }
    }
  }
  return fResult;
} /* end of resNodeIsFileInArchive() */


/*! \return TRUE if prnArg is a file in a parent archive file
*/
BOOL_T
resNodeIsDirInArchive(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (prnArg) {
    if (prnArg->eType == rn_type_dir_in_archive || prnArg->eType == rn_type_dir_in_zip) {
      fResult = TRUE;
    }
    else if (prnArg->eType == rn_type_dir) {
      resNodePtr prnI = NULL;

      for (prnI = resNodeGetParent(prnArg); prnI; prnI = resNodeGetParent(prnI)) {
	if (prnI->eType == rn_type_archive || prnI->eType == rn_type_dir_in_archive || prnI->eType == rn_type_zip || prnI->eType == rn_type_dir_in_zip) {
	  fResult = TRUE;
	  break;
	}
      }
    }
  }
  return fResult;
} /* end of resNodeIsDirInArchive() */


/*! \return TRUE if prnArg is a processable zip file
*/
BOOL_T
resNodeIsZipDocument(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (prnArg) {
    fResult = resMimeIsZipDocument(resNodeGetMimeType(prnArg));
  }

  return fResult;
} /* end of resNodeIsZipDocument() */


/*! \return TRUE if prnArg is a file in a parent zip file
 */
BOOL_T
resNodeIsFileInZip(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (prnArg) {
    if (prnArg->eType == rn_type_file_in_zip) {
      fResult = TRUE;
    }
    else if (prnArg->eType == rn_type_file) {
      resNodePtr prnI = NULL;

      for (prnI = resNodeGetParent(prnArg); prnI; prnI = resNodeGetParent(prnI)) {
	if (prnI->eType == rn_type_zip || prnI->eType == rn_type_dir_in_zip) {
	  fResult = TRUE;
	  break;
	}
      }
    }
  }
  return fResult;
} /* end of resNodeIsFileInZip() */


/*! \return TRUE if prnArg is a file in a parent zip file
*/
BOOL_T
resNodeIsDirInZip(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (prnArg) {
    if (prnArg->eType == rn_type_dir_in_zip) {
      fResult = TRUE;
    }
    else if (prnArg->eType == rn_type_dir) {
      resNodePtr prnI = NULL;

      for (prnI = resNodeGetParent(prnArg); prnI; prnI = resNodeGetParent(prnI)) {
	if (prnI->eType == rn_type_zip || prnI->eType == rn_type_dir_in_zip) {
	  fResult = TRUE;
	  break;
	}
      }
    }
  }
  return fResult;
} /* end of resNodeIsDirInZip() */


/*! \return TRUE if prnArg exists
*/
BOOL_T
resNodeIsExist(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  /*!\bug check for ROOT */
  if (prnArg) {
    if (resNodeGetType(prnArg) == rn_type_undef || resNodeHasDetails(prnArg,RN_INFO_STAT) == FALSE) {
      resNodeReadStatus(prnArg);
    }
    fResult = prnArg->fExist;
  }
  return fResult;
} /* end of resNodeIsExist() */


/*! \return TRUE if prnArg is readable
 */
BOOL_T
resNodeIsReadable(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  /*!\bug check for ROOT */
  if (prnArg) {
    if (resNodeGetType(prnArg) == rn_type_undef || resNodeHasDetails(prnArg,RN_INFO_STAT) == FALSE) {
      resNodeReadStatus(prnArg);
    }
    fResult = prnArg->fRead;
  }
  return fResult;
} /* end of resNodeIsReadable() */


#ifdef HAVE_PCRE2
/*! \return TRUE if prnArg is matching the regular expression
*/
BOOL_T
resNodeIsMatching(resNodePtr prnArg, const pcre2_code *re_match)
{
  BOOL_T fResult = TRUE;

  if (resNodeIsError(prnArg)) {
    fResult = FALSE;
  }
  else if (re_match) {
    int rc;
    char *pcNameBase;
    pcre2_match_data *match_data;

    if ((pcNameBase = (char *)resNodeGetNameBase(prnArg))) {
      match_data = pcre2_match_data_create_from_pattern(re_match, NULL);
      rc = pcre2_match(
	re_match,        /* result of pcre2_compile() */
	(PCRE2_SPTR8)pcNameBase,  /* the subject string */
	strlen((const char*)pcNameBase),             /* the length of the subject string */
	0,              /* start at offset 0 in the subject */
	0,              /* default options */
	match_data,        /* vector of integers for substring information */
	NULL);            /* number of elements (NOT size in bytes) */

      pcre2_match_data_free(match_data);   /* Release memory used for the match */

      if (rc < 0) {
	fResult = FALSE;
      }
    }
    else {
      fResult = FALSE;
    }
  }
  return fResult;
} /* end of resNodeIsMatching() */
#endif


/*! \return TRUE if prnArg is writeable
 */
BOOL_T
resNodeIsWriteable(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  /*!\bug check for ROOT */
  if (prnArg) {
    if (resNodeGetType(prnArg) == rn_type_undef || resNodeHasDetails(prnArg,RN_INFO_STAT) == FALSE) {
      resNodeReadStatus(prnArg);
    }

    if (resNodeIsStd(prnArg)) {
      fResult = TRUE;
    }
    else if (resNodeIsExist(prnArg)) {
      fResult = prnArg->fWrite;
    }
    else { 
      fResult = resNodeIsCreateable(prnArg);
    }
  }
  return fResult;
} /* end of resNodeIsWriteable() */


/*! \return TRUE if prnArg check if parent directory is writeable
 */
BOOL_T
resNodeParentIsWriteable(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (prnArg) {
    resNodePtr prnT;

    if (resNodeIsLink(prnArg) || resNodeIsRoot(prnArg) || resNodeIsStd(prnArg)) {
      /* exceptions */
    }
    else if (resNodeGetNameBaseDir(prnArg) != NULL && (prnT = resNodeDup(prnArg, RN_DUP_THIS)) != NULL) {
      resNodeSetToParent(prnT);
      fResult = (resNodeReadStatus(prnT) && resNodeIsWriteable(prnT) && prnT->fWrite);
      resNodeFree(prnT);
    }
  }
  return fResult;
} /* end of resNodeParentIsWriteable() */


/*! \return TRUE if prnArg is existing or createable
 */
BOOL_T
resNodeIsCreateable(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  /*!\bug check for ROOT */
  if (prnArg) {
    if (resNodeGetType(prnArg) == rn_type_undef || resNodeHasDetails(prnArg,RN_INFO_STAT) == FALSE) {
      resNodeReadStatus(prnArg);
    }

    if (resNodeIsStd(prnArg)) {
      fResult = TRUE;
    }
    else if (resNodeIsExist(prnArg)) {
      fResult = prnArg->fWrite;
    }
   else { /* check if parent directory is writeable */
      resNodePtr prnT;

      prnT = resNodeDup(prnArg, RN_DUP_THIS);
      if (prnT) {
	resNodeSetToParent(prnT);
	fResult = resNodeIsWriteable(prnT);
	resNodeFree(prnT);
      }
    }
  }
  return fResult;
} /* end of resNodeIsWriteable() */


/*! \return TRUE if prnArg is executable
 */
BOOL_T
resNodeIsExecuteable(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  /*!\bug check for ROOT */
  if (prnArg) {
    if (resNodeGetType(prnArg) == rn_type_undef || resNodeHasDetails(prnArg,RN_INFO_STAT) == FALSE) {
      resNodeReadStatus(prnArg);
    }
    fResult = prnArg->fExecute;
  }
  return fResult;
} /* end of resNodeIsExecuteable() */


/*! \return TRUE if prnArg is hidden
 */
BOOL_T
resNodeIsHidden(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  /*!\bug check for ROOT */
  if (prnArg) {
    if (resNodeGetType(prnArg) == rn_type_undef || resNodeHasDetails(prnArg,RN_INFO_STAT) == FALSE) {
      if (prnArg->pucNameBase == NULL) {
	resNodeResetNameBase(prnArg); /* leading dot in basename */
      }
      resNodeReadStatus(prnArg);
    }
    fResult = prnArg->fHidden;
  }
  return fResult;
} /* end of resNodeIsHidden() */


/*! sprintf's log messages
 */
RN_ERROR
resNodeSetError(resNodePtr prnArg, RN_ERROR eArg, const char *fmt, ...)
{
  if (fmt) {
    char mcMessage[BUFFER_LENGTH];
    va_list ap;
    
    va_start(ap,fmt);

#ifdef _MSC_VER
    vsprintf_s(mcMessage, BUFFER_LENGTH, fmt, ap);
#else
    vsnprintf(mcMessage, BUFFER_LENGTH, fmt, ap);
#endif
    
    va_end(ap);

    if (prnArg) {
      xmlFree(prnArg->pucMsg);
      prnArg->pucMsg = xmlStrdup(BAD_CAST mcMessage);
      prnArg->eError = eArg;
    }
    else {
      fprintf(stderr,"%s",mcMessage);
    }
  }
  return eArg;
} /* end of resNodeSetError() */


/*! \return error status of prnArg
*/
xmlChar *
resNodeGetErrorMsg(resNodePtr prnArg)
{
  if (prnArg != NULL) {
    return prnArg->pucMsg;
  }
  return NULL;
} /* end of resNodeGetErrorMsg() */


/*! \return error status of prnArg
*/
RN_ERROR
resNodeGetError(resNodePtr prnArg)
{
  if (prnArg != NULL) {
    return prnArg->eError;
  }
  return rn_error_undef;
} /* end of resNodeGetError() */


/*! \return TRUE if an error occures while processing of prnArg
 */
BOOL_T
resNodeIsError(resNodePtr prnArg)
{
  BOOL_T fResult = TRUE;

  if (prnArg != NULL) {
    fResult = (prnArg->eError != rn_error_none);
  }
  return fResult;
} /* end of resNodeIsError() */


/*! \return TRUE if an error occures while processing of prnArg
 */
BOOL_T
resNodeResetError(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (prnArg != NULL) {
    prnArg->eError = rn_error_none;
    if (prnArg->pucMsg != NULL) {
      xmlFree(prnArg->pucMsg);
      prnArg->pucMsg = NULL;
    }
    fResult = TRUE;
  }
  return fResult;
} /* end of resNodeResetError() */


/*! Detects and sets "owner" string of the resource node.

  \param prnArg a pointer to a resource node
  \return TRUE if an according filesystem entry exists
*/
BOOL_T
resNodeReadOwner(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (prnArg != NULL
      && prnArg->pcNameNormalizedNative != NULL
      && resNodeIsError(prnArg) == FALSE
      && (resNodeIsDir(prnArg) || resNodeIsFile(prnArg) || resNodeIsLink(prnArg))) {

#ifdef _MSC_VER
    // s. http://stackoverflow.com/questions/11384220/getnamedsecurityinfo-returns-error-access-denied5-when-writting-owner-of-a-rem
    // http://www.installsetupconfig.com/win32programming/accesscontrollistaclexample4.html

    PSID pSidOwner;
    PSECURITY_DESCRIPTOR pSecurityDescriptor;
    SID_IDENTIFIER_AUTHORITY SIDAuthNT = SECURITY_NT_AUTHORITY;
    DWORD dwRes;

    // Get the original owner in the object's security descriptor.
    dwRes = GetNamedSecurityInfo((LPCSTR) prnArg->pcNameNormalizedNative,	     // name of the object
      SE_FILE_OBJECT,		  // type of object
      OWNER_SECURITY_INFORMATION,  // change only the object's owner
      &pSidOwner,		 // SID of Administrator group
      NULL,
      NULL,
      NULL,
      &pSecurityDescriptor);

    if (dwRes == ERROR_SUCCESS) {
      LPTSTR AcctName = (LPTSTR)"";
      LPTSTR DomainName = (LPTSTR)"";
      DWORD dwAcctName = 1;
      DWORD dwDomainName = 1;
      SID_NAME_USE eUse = SidTypeUnknown;

      // First call to LookupAccountSid() to get the buffer size AcctName.
      fResult = LookupAccountSid(NULL,	     // Local computer
	pSidOwner,	// Pointer to the SID to lookup for
	AcctName,	 // The account name of the SID (pSIDOwner)
	(LPDWORD)&dwAcctName,   // Size of the AcctName in TCHAR
	DomainName,       // Pointer to the name of the Domain where the account name was found
	(LPDWORD)&dwDomainName, // Size of the DomainName in TCHAR
	&eUse);		 // Value of the SID_NAME_USE enum type that specify the SID type

      // Allocate memory for the AcctName.
      AcctName = (LPTSTR)GlobalAlloc(GMEM_FIXED, dwAcctName);

      if(AcctName == NULL)    {
	resNodeSetError(prnArg,rn_error_memory, "GlobalAlloc() - Failed to allocate buffer for AcctName, error %u", GetLastError());
	return FALSE;
      }

      DomainName = (LPTSTR)GlobalAlloc(GMEM_FIXED, dwDomainName);
      if(DomainName == NULL)     {
	resNodeSetError(prnArg,rn_error_memory,"GlobalAlloc() failed to allocate buffer for DomainName, error %u", GetLastError());
	GlobalFree(AcctName);
	return FALSE;
      }

      // Second call to LookupAccountSid() to get the account name.
      fResult = LookupAccountSid(NULL,       // name of local or remote computer
	pSidOwner,  // security identifier, SID
	AcctName,   // account name buffer
	(LPDWORD)&dwAcctName,	 // size of account name buffer
	DomainName, // domain name
	(LPDWORD)&dwDomainName,       // size of domain name buffer
	&eUse);	   // SID type

      xmlFree(prnArg->pucOwner);
      prnArg->pucOwner = NULL;

      if(fResult) {
	prnArg->pucOwner = xmlStrdup(BAD_CAST DomainName);
	prnArg->pucOwner = xmlStrcat(prnArg->pucOwner,BAD_CAST"\\");
	prnArg->pucOwner = xmlStrcat(prnArg->pucOwner,BAD_CAST AcctName);
      }
      else {
	DWORD dwErrorCode = GetLastError();
	if(dwErrorCode == ERROR_NONE_MAPPED) {
	  resNodeSetError(prnArg,rn_error_owner,"Account owner not found for specified SID.");
	}
	else {
	  resNodeSetError(prnArg,rn_error_owner,"LookupAccountSid() failed, error %u", GetLastError());
	}
      }
      GlobalFree(DomainName);
      GlobalFree(AcctName);
    }
    /*!\bug Freeing of pSidOwner cause to crash */
    //LocalFree(pSidOwner);
    //LocalFree(pSecurityDescriptor);
#elif defined(_WIN32)
    /*!\todo owner detection on MinGW */
#else
    struct passwd *pOwner;

    xmlFree(prnArg->pucOwner);
    prnArg->pucOwner = NULL;

    if ((pOwner = getpwuid(prnArg->s.st_uid)) != NULL && pOwner->pw_name != NULL && pOwner->pw_name[0] != '\0') {
      prnArg->pucOwner = xmlStrdup(BAD_CAST pOwner->pw_name);
      fResult = TRUE;
    }
    else {
      resNodeSetError(prnArg,rn_error_owner,"owner");
    }
#endif
  }
  else {
    resNodeSetError(prnArg,rn_error_path,"No valid directory path");
  }
  resNodeAddDetails(prnArg, RN_INFO_OWNER);

  return fResult;
} /* end of resNodeReadOwner() */


/*! \return TRUE if an error occures while processing of prnArg
 */
BOOL_T
resNodeResetDetails(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (prnArg != NULL) {
    prnArg->iDetails = RN_INFO_MIN;
    fResult = TRUE;
  }
  return fResult;
} /* end of resNodeResetDetails() */


/*! \return TRUE if an error occures while processing of prnArg
 */
BOOL_T
resNodeAddDetails(resNodePtr prnArg, int iArgOptions)
{
  BOOL_T fResult = FALSE;

  if (prnArg != NULL) {
    prnArg->iDetails |= iArgOptions;
    time(&(prnArg->tAtime));
    fResult = TRUE;
  }
  return fResult;
} /* end of resNodeAddDetails() */


/*! \return TRUE if prnArg is up to date
*/
BOOL_T
resNodeHasDetails(resNodePtr prnArg, int iArgOptions)
{
  BOOL_T fResult = FALSE;

  if (prnArg) {

    fResult = TRUE;

    if (fResult && IS_OPTION_STAT(iArgOptions)) {
      fResult = (prnArg->iDetails & RN_INFO_STAT) != 0;
    }

    if (fResult && IS_OPTION_INFO(iArgOptions)) {
      fResult = (prnArg->iDetails & RN_INFO_INFO) != 0;
    }

    if (fResult && IS_OPTION_STRUCT(iArgOptions)) {
      fResult = (prnArg->iDetails & RN_INFO_STRUCT) != 0;
    }

    if (fResult && IS_OPTION_OWNER(iArgOptions)) {
      fResult = (prnArg->iDetails & RN_INFO_OWNER) != 0;
    }

    if (fResult && IS_OPTION_CONTENT(iArgOptions)) {
      fResult = (prnArg->iDetails & RN_INFO_CONTENT) != 0 && (prnArg->pContent != NULL || prnArg->pdocContent != NULL);
    }
  }
  return fResult;
} /* end of resNodeHasDetails() */


/*! Resource Node List Parse  list different types of resource nodes

\param prnArg -- new resNode to parse (directory, share, archive, file)
\param iArgOptions --
\param re_match -- regexp
\param re_grep -- regexp

\return TRUE if successful, FALSE in case of errors
*/
BOOL_T
resNodeUpdate(resNodePtr prnArg, int iArgOptions, const pcre2_code *re_match, const pcre2_code *re_grep)
{
  BOOL_T fResult = TRUE;

  if (resNodeIsHidden(prnArg)) {
    /* empty result */
  }
  else if (resNodeReadStatus(prnArg)) {
    resNodePtr prnI;

    if (IS_OPTION_INDEX(iArgOptions) && resNodeHasDetails(prnArg, RN_INFO_INDEX)) {
      for (prnI = resNodeGetChild(prnArg); prnI; prnI = resNodeGetNext(prnI)) {
	if (resNodeIsDir(prnI) && resNodeIsRecursive(prnArg)) {
	  resNodeSetRecursion(prnI, resNodeIsRecursive(prnArg));
	}
	resNodeUpdate(prnI, iArgOptions, re_match, re_grep);
      }
    }
    else {
      if (resNodeHasDetails(prnArg, RN_INFO_INFO) == FALSE) {
	resNodeResetMimeType(prnArg);
      }

      if (resNodeHasDetails(prnArg, RN_INFO_STRUCT) == FALSE || resNodeGetChild(prnArg) == NULL) {
	if (resNodeIsURL(prnArg)) {
	}
#ifdef HAVE_LIBSQLITE3
	else if (resNodeIsDatabase(prnArg)) {
	  /*! database is without child nodes */
	  //fResult = dbAppendScheme(prnArg, re_match, TRUE);
	}
#endif
	else if (resNodeIsZipDocument(prnArg)) {
	  fResult = zipDocumentRead(prnArg,iArgOptions); /* update archive nodes */
	}
#ifdef HAVE_LIBARCHIVE
	else if (resNodeIsArchive(prnArg)) {
	  fResult = arcAppendEntries(prnArg, re_match, FALSE);
	}
#endif
	else if (resNodeIsDir(prnArg)) {
	  /* append a non-recursive list of directory */
	  fResult = resNodeDirAppendEntries(prnArg, re_match);

	  for (prnI = resNodeGetChild(prnArg); prnI; prnI = resNodeGetNext(prnI)) {
	    if (resNodeIsDir(prnI)) {
	      resNodeSetRecursion(prnI, resNodeIsRecursive(prnArg));
	    }
	    fResult &= resNodeUpdate(prnI, iArgOptions, re_match, re_grep);
	  }
	}
	else if (resNodeIsLink(prnArg)) {
	  //fResult = (resNodeResolveLinkChildNew(prnArg) != NULL);
	}
	else if (resNodeIsFile(prnArg)) {
	}
	else {
	}
      }

      if (resNodeHasDetails(prnArg, RN_INFO_OWNER) == FALSE) {
	resNodeGetOwner(prnArg);
      }

      if (resNodeHasDetails(prnArg, RN_INFO_CONTENT) == FALSE) {
	if (resNodeIsURL(prnArg)) {
	  if (resNodeGetContent(prnArg, 1024) != NULL && resNodeIsMemory(prnArg)) { /* content fetched from URL */
#ifdef HAVE_LIBARCHIVE
	    if (resNodeIsArchive(prnArg)) {
	      fResult = arcAppendEntries(prnArg, re_match, TRUE); /* update achive nodes */
	    }
	    else {
	      /* content itself */
	    }
#endif
	  }
	  else {
	    fResult = FALSE;
	  }
	}
#ifdef HAVE_LIBSQLITE3
	else if (resNodeIsDatabase(prnArg)) {
	  /*! database is without child nodes */
	  fResult = dbAppendEntries(prnArg, re_match, TRUE);
	}
#endif
	else if (resNodeIsZipDocument(prnArg)) {
	  fResult = zipDocumentRead(prnArg,iArgOptions); /* update archive nodes */
	}
#ifdef HAVE_LIBARCHIVE
	else if (resNodeIsArchive(prnArg)) {
	  fResult = arcAppendEntries(prnArg, re_match, TRUE); /* update achive nodes */
	}
#endif
	else if (resNodeIsLink(prnArg)) {
	}
	else if (resNodeIsFile(prnArg)) {
	  if (resMimeIsXml(resNodeGetMimeType(prnArg))) {
	    fResult = (resNodeGetContentDoc(prnArg) != NULL);
	  }
	  else {
	    fResult = (resNodeGetContent(prnArg, 1024) != NULL);
	  }
	}
	else {
	}
      }

      if (resNodeHasDetails(prnArg, RN_INFO_COMMENT) == FALSE) {
	/*\todo insert comment text from file */
      }
    }

    if (fResult) {
      resNodeAddDetails(prnArg, iArgOptions);
    }
  }
  return fResult;
} /* end of resNodeUpdate() */


/*! Reads status of an URL, file or directory with given resource
    node. Sets all file stat attributes to the resource node.

  \param prnArg a pointer to a resource node
  \return TRUE if an according filesystem entry exists
*/
BOOL_T
resNodeReadStatus(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (resNodeResetError(prnArg)) {
    if (resNodeHasDetails(prnArg, RN_INFO_STAT)) {
      /* this resource node was stat'd already */
      fResult = prnArg->fExist;
    }
    else if (prnArg->liSize > 0 && prnArg->pContent != NULL) {
      fResult = TRUE;
    }
    else if (resNodeIsDirInArchive(prnArg) || resNodeIsFileInArchive(prnArg)) {
      fResult = (resNodeGetAncestorArchive(prnArg) != NULL); /* stat archive */
    }
    else if (resNodeIsError(prnArg) == FALSE) {
      resNodePtr prnAncestor;

      switch (resNodeGetType(prnArg)) {
      case rn_type_stdin:
	prnArg->fRead = TRUE;
	prnArg->fWrite = FALSE;
	return TRUE;
	break;

      case rn_type_stdout:
      case rn_type_stderr:
	prnArg->fRead = FALSE;
	prnArg->fWrite = TRUE;
	return TRUE;
	break;

      case rn_type_url:
      case rn_type_url_ftp:
      case rn_type_url_http:
	prnArg->fRead = TRUE;
	prnArg->fWrite = FALSE;
	break;

      default:
	break;
      }

      if (resNodeGetNameNormalized(prnArg) != NULL && resNodeGetNameNormalizedNative(prnArg) != NULL) {
	if (resNodeIsURL(prnArg)) {
	  prnArg->liSize = 0;

	  if (resNodeIsMemory(prnArg)) {
	    /* content was fetched already */
	    prnArg->liSize = prnArg->liSizeContent;
	    prnArg->fRead = TRUE;
	    prnArg->fExist = TRUE;
	  }
	  else if (resNodeIsOpen(prnArg) || resNodeOpen(prnArg,"r")) {
#ifdef HAVE_LIBCURL
	    CURLcode res;

	    curl_easy_setopt((CURL *)prnArg->handleIO, CURLOPT_CONNECT_ONLY, 1L);
	    res = curl_easy_perform((CURL *)prnArg->handleIO);
	    if (res == CURLE_OK) { /* only connected! */
	      prnArg->fRead = TRUE;
	      prnArg->fExist = TRUE;
	    }
	    else {
	      prnArg->fRead = FALSE;
	      prnArg->fExist = FALSE;
	    }
	    curl_easy_setopt((CURL *)prnArg->handleIO, CURLOPT_CONNECT_ONLY, 0L);
#else
	    //prnArg->liSize = xmlNanoHTTPContentLength(resNodeGetHandleIO(prnArg));
	    //localtime(&(prnArg->tMtime));
	    //prnArg->tMtime = 1;
	    //prnArg->fExist = (prnArg->liSize > -1);
	    prnArg->fExist = TRUE;
#endif
	  }
	}
	else {
	  /*! local file */
	  int err;
	  char *pcName;

	  resPathCutTrailingChars(BAD_CAST prnArg->pcNameNormalizedNative);
	  pcName = prnArg->pcNameNormalizedNative; /* use 'pcName' as shortcut only */

#ifdef _MSC_VER
	  err = _stat(pcName, &(prnArg->s));
#elif defined(_WIN32)
	  err = stat(pcName, &(prnArg->s));
#else
	  err = lstat(pcName, &(prnArg->s));
#endif
	  if (err == -1) {
	    resNodeSetError(prnArg,rn_error_stat,"Cant stat() '%s'",pcName);
	    prnArg->fExist = FALSE;
	    //resNodeSetType(prnArg,rn_type_undef);
	  }
	  else {
	    int attr;

	    prnArg->fExist = TRUE;
	    prnArg->liSize = (long int)prnArg->s.st_size;
	    if (prnArg->liSize == 0) {
	      prnArg->eMimeType = MIME_EMPTY;
	    }
	    prnArg->tMtime = prnArg->s.st_mtime;

#ifdef _MSC_VER
	    attr = GetFileAttributes(pcName);
	    if (resNodeGetType(prnArg) == rn_type_root) {
	      /* keep this type */
	    }
	    else if (attr > 0 && (attr & FILE_ATTRIBUTE_REPARSE_POINT)) {
	      resNodeSetType(prnArg,rn_type_symlink);
	    }
	    else if (prnArg->s.st_mode & _S_IFDIR) {
	      resNodeSetType(prnArg,rn_type_dir);
	    }
	    else if (prnArg->s.st_mode & _S_IFREG) {
	      if (xmlStrEqual(resNodeGetExtension(prnArg), BAD_CAST"lnk")) {
		// Windows shell link (handle like a symbolic link)
		resNodeSetType(prnArg,rn_type_symlink);
		fResult = (resNodeResolveLinkChildNew(prnArg) != NULL);
	      }
	      else if (resNodeGetType(prnArg) == rn_type_undef) {
		resNodeSetType(prnArg,rn_type_file);
	      }
	    }
#elif defined(_WIN32)
	    /* no symlink on MinGW */
	    if (resNodeGetType(prnArg) == rn_type_root) {
	      /* keep this type */
	    }
	    else if (S_ISDIR(prnArg->s.st_mode)) {
	      resNodeSetType(prnArg,rn_type_dir);
	    }
	    else if (S_ISREG(prnArg->s.st_mode)) {
	      resNodeSetType(prnArg,rn_type_file);
	    }
#else
	    if (resNodeGetType(prnArg) == rn_type_root) {
	      /* keep this type */
	    }
	    else if (S_ISLNK(prnArg->s.st_mode)) {
	      resNodeSetType(prnArg,rn_type_symlink);
	      fResult = (resNodeResolveLinkChildNew(prnArg) != NULL);
	    }
	    else if (S_ISDIR(prnArg->s.st_mode)) {
	      resNodeSetType(prnArg,rn_type_dir);
	    }
	    else if (resNodeGetType(prnArg) == rn_type_undef) {
	      resNodeSetType(prnArg,rn_type_file);
	    }
	    else if (S_ISREG(prnArg->s.st_mode)) {
	    }
#endif

	    /* check permissions of path */
#ifdef _MSC_VER
	    prnArg->fRead    = (prnArg->s.st_mode & _S_IREAD) ? TRUE : FALSE;
	    prnArg->fWrite   = (prnArg->s.st_mode & _S_IWRITE) ? TRUE : FALSE;
	    prnArg->fExecute = (prnArg->s.st_mode & _S_IEXEC) ? TRUE : FALSE;
	    if (resNodeGetMimeType(prnArg) == MIME_APPLICATION_X_BAT || resNodeGetMimeType(prnArg) == MIME_APPLICATION_VND_MICROSOFT_PORTABLE_EXECUTABLE) {
	      prnArg->fExecute = TRUE;
	    }
	    prnArg->fHidden  = (attr > 0 && (attr & FILE_ATTRIBUTE_HIDDEN)) ? TRUE : FALSE;
#elif 0
	    prnArg->fRead    = prnArg->s.st_mode & (S_IRUSR | S_IRGRP | S_IROTH);
	    prnArg->fWrite   = prnArg->s.st_mode & (S_IWUSR | S_IWGRP | S_IWOTH);
	    prnArg->fExecute = prnArg->s.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH);
#else
	    /*!\todo OPTIMIZE without access() */
	    prnArg->fRead    = (access((const char*)prnArg->pcNameNormalizedNative,R_OK)==0);
	    prnArg->fWrite   = (access((const char*)prnArg->pcNameNormalizedNative,W_OK)==0);
	    prnArg->fExecute = (access((const char*)prnArg->pcNameNormalizedNative,X_OK)==0);
#endif
	    if (prnArg->pucNameBase != NULL && (prnArg->pucNameBase[0] == '.')) {
	      prnArg->fHidden = TRUE; /* leading dot overrules filesystem attribbute */
	    }
	  }
	}
      }
      resNodeAddDetails(prnArg, RN_INFO_STAT);

      /* set all parent directories too */
      for (prnAncestor = resNodeGetParent(prnArg); prnAncestor != NULL; prnAncestor = resNodeGetParent(prnAncestor)) {
	prnAncestor->fExist = TRUE;
      }
	    
      fResult = prnArg->fExist;
    }
    else {
      resNodeSetError(prnArg,rn_error_path, "No valid directory path");
    }
  }
  return fResult;
} /* end of resNodeReadStatus() */


/*! Sets the file extension attribute of this resource node.

  \param prnArg a pointer to a resource node
  \return TRUE if a extension was found at prnArg 
 */
BOOL_T
resNodeSetExtension(resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (prnArg != NULL && prnArg->pucExtension == NULL) {
    switch (prnArg->eType) {
    case rn_type_undef:
    case rn_type_file:
    case rn_type_file_compressed:
    case rn_type_image:
    case rn_type_archive:
    case rn_type_file_in_archive:
    case rn_type_file_in_zip:
    case rn_type_database:
    case rn_type_file_in_database:
    case rn_type_symlink:
      prnArg->pucExtension = resPathGetExtensionStr(resNodeGetNameBase(prnArg));
      break;
    default:
      break;
    }
    fResult = (prnArg->pucExtension != NULL);
  }
  return fResult;
} /* end of resNodeSetExtension() */


/*! \return a pointer to the extension string of resource node, derived from basename
*/
xmlChar *
resNodeGetExtension(resNodePtr prnArg)
{
  xmlChar *pucResult = NULL;

  if (prnArg) {
    switch (prnArg->eType) {
    case rn_type_undef:
    case rn_type_file:
    case rn_type_file_compressed:
    case rn_type_image:
    case rn_type_archive:
    case rn_type_file_in_archive:
    case rn_type_zip:
    case rn_type_file_in_zip:
    case rn_type_database:
    case rn_type_file_in_database:
    case rn_type_symlink:
      if (prnArg->pucExtension == NULL) {
	resNodeSetExtension(prnArg);
      }
      pucResult = prnArg->pucExtension;
      break;
    default:
      break;
    }
  }

  return pucResult;
} /* end of resNodeGetExtension() */


/*! \return a pointer to the owner string of resource node
*/
xmlChar *
resNodeGetOwner(resNodePtr prnArg)
{
  if (prnArg) {
    if (prnArg->pucOwner == NULL && (resNodeIsFile(prnArg) || resNodeIsDir(prnArg))) {
      resNodeReadOwner(prnArg);
    }
    return prnArg->pucOwner;
  }
  return NULL;
} /* end of resNodeGetOwner() */


/*! Sets the type of this resource node.

  \param prnArg a pointer to a resource node
  \param eArgType the resource node
  */
void
resNodeSetType(resNodePtr prnArg, RN_TYPE eArgType)
{
  if (prnArg) {
    prnArg->eType = eArgType;
  }
} /* end of resNodeSetType() */


/*! Sets and returns the type of this resource node.

\param prnArg a pointer to a resource node
\return value of eType or -1 in case of errors
*/
xmlChar *
resNodeGetTypeStr(resNodePtr prnArg)
{
  xmlChar *pucResult = BAD_CAST mpcTypeNames[rn_type_undef];

  if (prnArg != NULL && prnArg->eType >= rn_type_undef && prnArg->eType <= rn_type_url) {
    pucResult = BAD_CAST mpcTypeNames[prnArg->eType];
  }

  return pucResult;
} /* end of resNodeGetTypeStr() */


/*! Sets and returns the type of this resource node.

\param prnArg a pointer to a resource node
\return value of eType or -1 in case of errors
*/
RN_TYPE
resNodeGetType(resNodePtr prnArg)
{
  RN_TYPE eResult = rn_type_undef;

  if (prnArg) {
    xmlChar *pucT;

    if (prnArg->eType == rn_type_undef && (pucT = resNodeGetNameNormalized(prnArg)) != NULL) {
      if (resPathIsStd(pucT)) {	/* stdin */
	resNodeSetType(prnArg,rn_type_stdin);
      }
      else if (resPathIsHttpURL(pucT)) { /* web access */
	resNodeSetType(prnArg,rn_type_url_http);
      }
      else if (resPathIsFtpURL(pucT)) { /* ftp access */
	resNodeSetType(prnArg,rn_type_url_ftp);
      }
      /*!\todo also other types of URI */
      else if (resPathIsDir(pucT)) { /* directory */
	resNodeSetType(prnArg, rn_type_dir);
      }
      else if (resPathIsUNC(pucT)) { /* UNC */
	resNodeSetType(prnArg, rn_type_dir);
      }
#ifdef HAVE_LIBARCHIVE
      else if (resNodeGetAncestorArchive(prnArg) != NULL
	|| resPathGetNameOfNextArchivePtr(pucT) > pucT) { /*  */
	if (resPathGetPathInNextArchivePtr(pucT) == NULL) { /* archive only */
	  resNodeSetType(prnArg, rn_type_archive);
	}
	else if (resNodeGetExtension(prnArg) == NULL) { /*  */
	  resNodeSetType(prnArg, rn_type_dir_in_archive);
	}
	else { /*  */
	  resNodeSetType(prnArg, rn_type_file_in_archive);
	}
      }
#endif
      else {
	resNodeSetType(prnArg,rn_type_file);
      }
    }
    eResult = prnArg->eType;
  }
  return eResult;
} /* end of resNodeGetType() */


/*! returns the mtime of this resource node.

  \param prnArg a pointer to a resource node
  \return value of tMtime or -1 in case of errors
*/
struct tm *
resNodeGetLocalTime(resNodePtr prnArg)
{
  struct tm *pResult = NULL;

  if (prnArg) {
    pResult = (struct tm *)xmlMalloc(sizeof(struct tm));
    if (pResult) {
      struct tm *pT;

      pT = localtime(&(prnArg->tMtime));
      if (pT) {
	memcpy(pResult,pT,sizeof(struct tm));
      }
      else {
	xmlFree(pResult);
	pResult = NULL;
      }
    }
  }
  return pResult;
} /* end of resNodeGetLocalTime() */


/*! returns the mtime of this resource node.

\param prnArg a pointer to a resource node
\return value of tMtime or -1 in case of errors
*/
time_t
resNodeGetMtime(resNodePtr prnArg)
{
  if (prnArg) {
    if (prnArg->tMtime < 1) {
      resNodeReadStatus(prnArg);
    }
    return prnArg->tMtime;
  }
  return -1;
} /* end of resNodeGetMtime() */


/*! Sets and returns the mtime string of this resource node.

  \param prnArg a pointer to a resource node
  \return string value of tMtime or NULL in case of errors
*/
xmlChar *
resNodeGetMtimeStr(resNodePtr prnArg)
{
  xmlChar *pucResult = NULL;

  if (prnArg) {
    if (prnArg->pucMtime == NULL) {
      if (prnArg->tMtime < 1) {
	resNodeReadStatus(prnArg);
      }
      prnArg->pucMtime = GetDateIsoString(prnArg->tMtime);
    }

    pucResult = prnArg->pucMtime;
  }
  return pucResult;
} /* end of resNodeGetMtimeStr() */


/*! Sets and returns the mtime string of this resource node.

  \param prnArg a pointer to a resource node
  \return 
*/
long
resNodeGetMtimeDiff(resNodePtr prnArg)
{
  long liResult = -1;

  if (resNodeGetMtime(prnArg) != -1 && prnArg->tAtime > 0) {
    liResult = (long) (prnArg->tAtime - prnArg->tMtime);
  }
  return liResult;
} /* end of resNodeGetMtimeDiff() */


/*! Sets and returns the current number of child nodes of this resource node.

  \param prnArg a pointer to a resource node
  \return recursive value of liSize or -1 in case of errors
*/
size_t
resNodeIncrChilds(resNodePtr prnArg, size_t iArg)
{
  if (prnArg) {
    prnArg->liChilds += iArg;
    return prnArg->liChilds;
  }
  return (size_t) -1;
} /* end of resNodeIncrChilds() */


/*! Sets and returns the current number of child nodes of this resource node.

  \param prnArg a pointer to a resource node
*/
size_t
resNodeGetCountChilds(resNodePtr prnArg)
{
  if (prnArg) {
    return prnArg->liChilds;
  }
  return (size_t) -1;
} /* end of resNodeGetCountChilds() */


/*! Sets and returns the liSize of this resource node.

  \param prnArg a pointer to a resource node
  \return value of liSize or -1 in case of errors
*/
size_t
resNodeGetSize(resNodePtr prnArg)
{
  size_t liResult = 0;

  if (prnArg) {
    if (prnArg->liSize == 0) {
      resNodeReadStatus(prnArg);
    }
    if (prnArg->liSizeContent > 0) {
      liResult = prnArg->liSizeContent; /* size of read content */
    }
    else {
      liResult = prnArg->liSize; /* size of file etc */
    }
  }
  return liResult;
} /* end of resNodeGetSize() */


/*! Sets and returns the liSize of this resource node.

  \param prnArg a pointer to a resource node
  \return value of liSize or -1 in case of errors
*/
size_t
resNodeSetSize(resNodePtr prnArg, size_t iArg)
{
  if (prnArg) {
    prnArg->liSize = iArg;
    return prnArg->liSize;
  }
  return 0;
} /* end of resNodeSetSize() */


/*! Sets and returns the liSize of this resource node.

  \param prnArg a pointer to a resource node
  \return recursive value of liSize or -1 in case of errors
*/
size_t
resNodeIncrSize(resNodePtr prnArg, size_t iArg)
{
  if (prnArg) {
    prnArg->liSize += iArg;
    return prnArg->liSize;
  }
  return 0;
} /* end of resNodeIncrSize() */


/*! Sets and returns the liSize of this resource node.

  \param prnArg a pointer to a resource node
  \return recursive value of liSize or -1 in case of errors
*/
size_t
resNodeGetRecursiveSize(resNodePtr prnArg)
{
  if (prnArg) {
    return prnArg->liRecursiveSize;
  }
  return 0;
} /* end of resNodeGetRecursiveSize() */


/*! Sets and returns the liSize of this resource node.

  \param prnArg a pointer to a resource node
  \return recursive value of liSize or -1 in case of errors
*/
size_t
resNodeIncrRecursiveSize(resNodePtr prnArg, size_t iArg)
{
  if (prnArg) {
    prnArg->liRecursiveSize += iArg;
    return prnArg->liRecursiveSize;
  }
  return 0;
} /* end of resNodeIncrRecursiveSize() */


/*! Sets and returns the fRecursive flag of this resource node.

  \param prnArg a pointer to a resource node
  \return value of fRecursive or FALSE in case of errors
*/
BOOL_T
resNodeSetRecursion(resNodePtr prnArg, BOOL_T fArgFlag)
{
  BOOL_T fResult = FALSE;

  if (prnArg) {
    prnArg->fRecursive = fArgFlag;
    fResult = prnArg->fRecursive;
  }
  return fResult;
} /* end of resNodeSetRecursion() */


/*! Sets and returns the fRead flag of this resource node.

  \param prnArg a pointer to a resource node
  \return value of fRecursive or FALSE in case of errors
*/
BOOL_T
resNodeSetWrite(resNodePtr prnArg, BOOL_T fArgFlag)
{
  BOOL_T fResult = FALSE;

  if (prnArg) {
    prnArg->fWrite = fArgFlag;
    prnArg->fRead  = ! prnArg->fWrite;
    fResult = prnArg->fWrite;
  }
  return fResult;
} /* end of resNodeSetWrite() */


/*! Sets and returns the path of this resource node.in URI notation

  \param prnArg a pointer to a resource node
  \return resource node in URL notation or NULL in case of errors
 */
xmlChar *
resNodeGetURI(resNodePtr prnArg)
{
  xmlChar *pucResult = NULL;

  if (prnArg != NULL && prnArg->pucNameNormalized != NULL) {
    if ((resNodeGetChild(prnArg))) {
      pucResult = resNodeGetNameNormalized(prnArg);
    }
    else if (prnArg->pucURI == NULL) {
      xmlURIPtr pURI;
      xmlChar *pucT;
      xmlChar *pucTT;

      if (resPathIsURL(prnArg->pucNameNormalized)) {
	pucTT = xmlStrdup(prnArg->pucNameNormalized);
      }
      else {
#ifdef _WIN32
	pucTT = xmlStrcat(xmlStrdup(BAD_CAST"file:///"),prnArg->pucNameNormalized);
	for (pucT=pucTT; *pucT; pucT++) {
	  if (issep(*pucT)) {
	    *pucT = (xmlChar)'/';
	  }
	}
#else
	pucTT = xmlStrcat(xmlStrdup(BAD_CAST"file://"),prnArg->pucNameNormalized);
#endif
      }

      pucT = xmlPathToURI(pucTT);
      pURI = xmlParseURI((const char*)pucT);
      if (pURI) {
	prnArg->pucURI = xmlSaveUri(pURI);
	pucResult = prnArg->pucURI;
      }
      xmlFreeURI(pURI);
      xmlFree(pucT);
      xmlFree(pucTT);
    }
    else {
      pucResult = prnArg->pucURI;
    }
  }
  return pucResult;
} /* end of resNodeGetURI() */


/*! \return the pucNameBase of resource node
*/
xmlChar *
resNodeGetNameBase(resNodePtr prnArg)
{
  xmlChar *pucResult = NULL;

  if (prnArg) {
    if (prnArg->pucNameBase == NULL) {
      resNodeResetNameBase(prnArg);
    }
    pucResult = prnArg->pucNameBase;
  }
  return pucResult;
} /* end of resNodeGetNameBase() */


/*! Sets and returns the path relative to root directory of this resource node.

  \param prnArg a pointer to a resource node
  \return pucNameRelative or NULL in case of errors
*/
xmlChar *
resNodeGetNameRelative(resNodePtr prnArgBase, resNodePtr prnArg)
{
  xmlChar *pucResult = NULL;

  if (prnArg) {
    if (prnArgBase) {
      pucResult = resPathDiffPtr(resNodeGetNameNormalized(prnArgBase), resNodeGetNameNormalized(prnArg));
      while (STR_IS_NOT_EMPTY(pucResult) && issep(*pucResult)) {
        pucResult++;
      }
    }
    else {
#ifdef HAVE_CGI
      /* no absolute path in CGI mode */
#else
      pucResult = resNodeGetNameNormalized(prnArg);
#endif
    }
  }
  return pucResult;
} /* end of resNodeGetNameRelative() */


/*! Sets and returns the pucNameBaseDir of this resource node.

  \param prnArg a pointer to a resource node
  \return pucNameBaseDir or NULL in case of errors
*/
xmlChar *
resNodeGetNameBaseDir(resNodePtr prnArg)
{
  xmlChar *pucResult = NULL;

  if (prnArg) {
    if (prnArg->pucNameBaseDir == NULL) {
      resNodeResetNameBase(prnArg);
    }
    pucResult = prnArg->pucNameBaseDir;
  }
  return pucResult;
} /* end of resNodeGetNameBaseDir() */


/*! Sets and returns the pucNameNormalized of this resource node.

  \param prnArg a pointer to a resource node
  \return pucNameNormalized or NULL in case of errors
 */
xmlChar *
resNodeGetNameNormalized(resNodePtr prnArg)
{
  xmlChar *pucResult = NULL;

  if (prnArg) {
    if (prnArg->pucNameNormalized == NULL) {
      if (prnArg->pcNameNormalizedNative != NULL) {
	prnArg->pucNameNormalized = resPathEncodeStr(prnArg->pcNameNormalizedNative);
      }
      else if (prnArg->pucNameBaseDir != NULL) {
	prnArg->pucNameNormalized = resPathConcatNormalizedStr(prnArg->pucNameBaseDir, prnArg->pucNameBase);
      }
      else if (prnArg->parent != NULL) {
	prnArg->pucNameNormalized = resPathConcatNormalizedStr(resNodeGetNameNormalized(prnArg->parent), prnArg->pucNameBase);
      }
      else {
      }
    }
    else if (resNodeIsMemory(prnArg)) {
    }
    else if (resPathIsRelative(prnArg->pucNameNormalized)) {
      prnArg->pucNameNormalized = resPathConcatNormalizedStr(resNodeGetNameNormalized(prnArg->parent), prnArg->pucNameNormalized);
    }
    pucResult = prnArg->pucNameNormalized;
  }
  return pucResult;
} /* end of resNodeGetNameNormalized() */


/*! Sets and returns the pcNameNormalizedNative of this resource node.

  \param prnArg a pointer to a resource node
  \return pcNameNormalizedNative or NULL in case of errors
 */
char *
resNodeGetNameNormalizedNative(resNodePtr prnArg)
{
  if (prnArg) {
    if (prnArg->pcNameNormalizedNative == NULL) {
      prnArg->pcNameNormalizedNative = resPathDecodeStr(resNodeGetNameNormalized(prnArg));
    }
    return prnArg->pcNameNormalizedNative;
  }
  return NULL;
} /* end of resNodeGetNameNormalizedNative() */


/*! Read and sets the file MIME type of this resource node.

\param prnArg a pointer to a resource node
*/
void
resNodeResetMimeType(resNodePtr prnArg)
{
  if (prnArg) {
    resNodePtr prnChild;

    if (resNodeGetType(prnArg) == rn_type_dir || resNodeGetType(prnArg) == rn_type_dir_in_archive) {
      /*  */
      resNodeSetMimeType(prnArg,MIME_INODE_DIRECTORY);
    }
    else if (resNodeGetType(prnArg) == rn_type_symlink) {
      /*  */
      resNodeSetMimeType(prnArg,MIME_INODE_SYMLINK);
    }
    else if (resNodeGetType(prnArg) == rn_type_file_compressed) {
      /* set uncompressed MIME type for child */
      resNodeSetMimeType(prnArg,resMimeGetTypeFromExt(resNodeGetExtension(prnArg)));
      if ((prnChild = resNodeGetChild(prnArg)) != NULL) {
	xmlChar *pucE;

	pucE = resPathGetExtensionStr(resNodeGetNameObject(prnArg));
	prnChild->eMimeType = resMimeGetTypeFromExt(pucE);
	xmlFree(pucE);
      }
    }
    else if (resNodeGetType(prnArg) == rn_type_url) {
      /*!\todo detect MIME type by fetching Content-type */
      resNodeSetMimeType(prnArg,resMimeGetTypeFromExt(resNodeGetExtension(prnArg)));
    }
    else if (resNodeGetType(prnArg) == rn_type_stdout) {
      /*  */
      resNodeSetMimeType(prnArg,MIME_PIPE_STDOUT);
    }
    else if (resPathIsStd(prnArg->pucNameNormalized)) {
      /*  */
      resNodeSetType(prnArg,rn_type_stdout);
      resNodeSetMimeType(prnArg,MIME_PIPE_STDOUT);
    }
    else if (resNodeIsIndex(prnArg)) {
#ifdef HAVE_PIE
      resNodeSetMimeType(prnArg,MIME_APPLICATION_PIE_XML_INDEX);
#else
      resNodeSetMimeType(prnArg,MIME_TEXT_XML);
#endif
    }
    else {
      resNodeSetMimeType(prnArg,resMimeGetTypeFromExt(resNodeGetExtension(prnArg)));
    }
  }
} /* end of resNodeResetMimeType() */


/*! \return the index of mime attribute
*/
RN_MIME_TYPE
resNodeGetMimeType(resNodePtr prnArg)
{
  RN_MIME_TYPE eResult = MIME_UNDEFINED;
  
  if (prnArg) {
    if (prnArg->eMimeType == MIME_UNDEFINED) {
#if 1
      resNodeResetMimeType(prnArg);
      eResult = prnArg->eMimeType;
#else
      resNodePtr prnT;

      if ((prnT = resNodeGetParent(prnArg)) && resNodeIsFile(prnT)) {
	eResult = resNodeGetMimeType(prnT);
      }
      else {
	resNodeResetMimeType(prnArg);
	eResult = prnArg->eMimeType;
      }
#endif
    }
    else {
      eResult = prnArg->eMimeType;
    }
  }

  return eResult;
} /* end of resNodeGetMimeType() */


/*! \return a pointer to string of mime attribute or NULL in case of errors

  \todo handle symlinks and in-archive content
*/
const char *
resNodeGetMimeTypeStr(resNodePtr prnArg)
{
  if (prnArg) {
    return resMimeGetTypeStr(resNodeGetMimeType(prnArg));
  }
  return NULL;
} /* end of resNodeGetMimeTypeStr() */


/*! set MIME type
*/
void
resNodeSetMimeType(resNodePtr prnArg, RN_MIME_TYPE eArgMimeType)
{
  if (prnArg) {
    prnArg->eMimeType = eArgMimeType;
  }
} /* end of resNodeSetMimeType() */


/*! \return a pointer to an application specific interpretation string or NULL in case of errors
*/
xmlChar *
resNodeGetNameObject(resNodePtr prnArg)
{
  xmlChar *pucResult = NULL;
  
  if (prnArg) {
    if (prnArg->pucObject == NULL) {
      int i;
      xmlChar *pucT = NULL;

      switch (prnArg->eMimeType) {
      case MIME_APPLICATION_VND_PTC_CAD_2D:
      case MIME_APPLICATION_VND_PTC_CAD_3D:
      case MIME_APPLICATION_VND_PTC_CAD_CONFIGURATION:
	/* add a property for internal object name, without iteration suffix */

	pucT = xmlStrdup(prnArg->pucNameBase);
	for (i = xmlStrlen(pucT) - 1; i>0 && (isdigit(pucT[i]) || pucT[i] == '.'); i--) {
	  if (pucT[i] == '.') {
	    if (xmlStrlen(&(pucT[i])) > 1 && isdigit(pucT[i + 1]) && xmlStrlen(&(pucT[i + 1])) < 5) {
	    }
	    pucT[i] = (xmlChar)'\0';
	    break;
	  }
	}
	StringToUpper((char *)pucT);
	break;

      case MIME_APPLICATION_X_BZIP:
      case MIME_APPLICATION_GZIP:
	/* add a property for internal object name, without iteration suffix */
	pucT = resPathGetRootnameStr(prnArg->pucNameBase);
	/*!\todo handle ".tgz" */
	break;

      case MIME_APPLICATION_VND_DASSAULT_CATIA_V4_2D:
      case MIME_APPLICATION_VND_DASSAULT_CATIA_V4_3D:
      case MIME_APPLICATION_VND_DASSAULT_CATIA_V5_2D:
      case MIME_APPLICATION_VND_DASSAULT_CATIA_V5_3D:
	pucT = xmlStrdup(prnArg->pucNameBase);
	StringToUpper((char *)pucT);
	break;

      case MIME_APPLICATION_CAD_3D:
	pucT = xmlStrdup(prnArg->pucNameBase);
	StringToUpper((char *)pucT);
	break;

      default:
	/* no addtitional file information details */
	break;
      }

      if (STR_IS_EMPTY(pucT)) {
	xmlFree(pucT);
      }
      else {
	prnArg->pucObject = pucT;
      }
    }
    pucResult = prnArg->pucObject;
  }
  return pucResult;
} /* end of resNodeGetNameObject() */


#ifdef _MSC_VER

/*! wrapper for Windows FindNextFile(), append all directory entries as childs

\param prnArgDir the resource node
\param re_match -- regexp

\return the string of mime attribute
*/
BOOL_T
resNodeDirAppendEntries(resNodePtr prnArgDir, const pcre2_code *re_match)
{
  BOOL_T fResult = FALSE;

#ifdef DEBUG
  PrintFormatLog(4, "parse dir '%s'", resNodeGetNameNormalized(prnArgDir));
#endif

    /*!\bug test permission to this directory first */

  if (resNodeReadStatus(prnArgDir) && resNodeIsDir(prnArgDir)) {
    int iResultErr = -1;
    WIN32_FIND_DATA FindFileData;
    HANDLE phFind;
    xmlChar mpucPattern[BUFFER_LENGTH];

    xmlStrPrintf(mpucPattern, sizeof(mpucPattern), "%s\\*.*", prnArgDir->pcNameNormalizedNative);

    /*  Find first file in current directory */
    phFind = FindFirstFile((LPCSTR)mpucPattern, &FindFileData);
    if (phFind == INVALID_HANDLE_VALUE) {
      iResultErr = GetLastError();
      resNodeSetError(prnArgDir, rn_error_find,"Cant open directory '%s' %i", prnArgDir->pcNameNormalizedNative, iResultErr);
      //warning empty dir ???
    }
    else {
      for (fResult = TRUE;;) {
	xmlChar *pucName;

	if (strlen(FindFileData.cFileName) < 1 || strcmp(FindFileData.cFileName, ".")==0 || strcmp(FindFileData.cFileName, "..")==0) {
	  /* ignoring empty or symbolic entries */
	}
	else if ((pucName = resPathEncodeStr(FindFileData.cFileName))) {
	  int rc = 1; /* default: append node */
	  resNodePtr prnI;

	  if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
	    /* append all directories */
	    prnI = resNodeAddChildNew(prnArgDir, pucName);
	    resNodeSetType(prnI, rn_type_dir);
	    rc = -1; /* to avoid redundancy */
	  }
#ifdef HAVE_PCRE2
	  else if (re_match) { /* filename matching */
	    pcre2_match_data *match_data;

	    match_data = pcre2_match_data_create_from_pattern(re_match, NULL);
	    rc = pcre2_match(re_match,			    /* result of pcre2_compile() */
			     (PCRE2_SPTR8)pucName,	    /* the subject string */
			     strlen((const char *)pucName), /* the length of the subject string */
			     0,				    /* start at offset 0 in the subject */
			     0,				    /* default options */
			     match_data,		    /* vector of integers for substring information */
			     NULL);			    /* number of elements (NOT size in bytes) */

	    pcre2_match_data_free(match_data); /* Release memory used for the match */
	  }
#endif

	  if (rc < 0) {
	    //PrintFormatLog(4, "%s ignore '%s'", NAME_FILE, pucName);
	  }
	  else {
	    prnI = resNodeAddChildNew(prnArgDir, pucName);
	    resNodeIncrSize(prnArgDir, resNodeGetSize(prnI));
	  }
	  xmlFree(pucName);
	}
	else {
	  /*!\todo handle encoding errors */
	  fResult = FALSE;
	}

	if (FindNextFile(phFind, &FindFileData) == 0) {
	  iResultErr = GetLastError();
	  if (iResultErr == ERROR_NO_MORE_FILES) {
	    iResultErr = ENOENT;
	  }
	  else {
	    resNodeSetError(prnArgDir, rn_error_find, "find");
	    fResult = FALSE;
	  }
	  break;
	}
      }
    }
    FindClose(phFind);
  }
  return fResult;
} /* end of resNodeDirAppendEntries() */

#elif defined(_WIN32)

#error "readdir() to be implemented"

#else

/*! wrapper for readdir(), append all directory entries as childs

\param prnArgDir the resource node
\param re_match -- regexp

\return FALSE if there are any errors
*/
BOOL_T
resNodeDirAppendEntries(resNodePtr prnArgDir, const pcre2_code *re_match)
{
  BOOL_T fResult = FALSE;

#ifdef DEBUG
  PrintFormatLog(4, "parse dir '%s'", resNodeGetNameNormalized(prnArgDir));
#endif

  /*!\bug test permission to this directory first */

  if (resNodeIsExist(prnArgDir) && resNodeIsDir(prnArgDir)) {

    //assert(prnArgDir->eType == rn_type_dir);
    if (prnArgDir->handleIO == NULL) {
      prnArgDir->handleIO = opendir(prnArgDir->pcNameNormalizedNative);
      if(prnArgDir->handleIO == NULL) {
	//iResultErr = errno;
	resNodeSetError(prnArgDir,rn_error_find,"Cant open directory '%s'", prnArgDir->pucNameNormalized);
      }
    }

    if (prnArgDir->handleIO) {
      struct dirent *pEntity;

      for (fResult = TRUE; (pEntity = readdir((DIR *)prnArgDir->handleIO));) { /* CAUTION: readdir() is not thread safe */
	xmlChar *pucName;

	if (errno == EBADF) {
	  resNodeSetError(prnArgDir, rn_error_find, "find");
	  fResult = FALSE;
	}
	else if (pEntity->d_ino == 0 || strcmp(pEntity->d_name, ".") == 0 || strcmp(pEntity->d_name, "..") == 0) {
	  /* ignoring empty or symbolic entries */
	}
	else if ((pucName = resPathEncodeStr(pEntity->d_name))) {
	  int rc = 1; /* default: append node */
	  resNodePtr prnI;

	  if (pEntity->d_type & DT_DIR) {
	    /* append all directories */
	    prnI = resNodeAddChildNew(prnArgDir, pucName);
	    resNodeSetType(prnI, rn_type_dir);
	    rc = -1; /* to avoid redundancy */
	  }
#ifdef HAVE_PCRE2
	  else if (re_match) { /* filename matching */
	    pcre2_match_data *match_data;

	    match_data = pcre2_match_data_create_from_pattern(re_match, NULL);
	    rc = pcre2_match(re_match,		   /* result of pcre2_compile() */
			     (PCRE2_SPTR8)pucName, /* the subject string */
			     xmlStrlen(pucName),   /* the length of the subject string */
			     0,			   /* start at offset 0 in the subject */
			     0,			   /* default options */
			     match_data,	   /* vector of integers for substring information */
			     NULL);		   /* number of elements (NOT size in bytes) */

	    pcre2_match_data_free(match_data); /* Release memory used for the match */
	  }
#endif

	  if (rc < 0) {
	    // PrintFormatLog(4, "%s ignore '%s'", NAME_FILE, pcNameBase);
	  }
	  else {
	    prnI = resNodeAddChildNew(prnArgDir, pucName);
	    resNodeIncrSize(prnArgDir, resNodeGetSize(prnI));
	  }
	  xmlFree(pucName);
	}
	else {
	  /*!\todo handle encoding errors */
	  fResult = FALSE;
	}
      }

      closedir((DIR *)prnArgDir->handleIO);
      prnArgDir->handleIO = NULL;
    }
  }
  return fResult;
} /* end of resNodeDirAppendEntries() */

#endif


#ifdef TESTCODE
#include "version.h"
#include <cxp/cxp_dtd.h>
#include "test/test_res_node.c"
#endif

