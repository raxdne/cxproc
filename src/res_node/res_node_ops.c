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

#include <libxml/parser.h>

#include "basics.h"
#include "utils.h"
#include <res_node/res_node_ops.h>


/* **********************************************************************************************

   filesystem operation functions (copy, move, ...)

   use of native functions
   - Microsoft "Directory Management Functions" and "File Management Functions"
   - Linux
 */

/*! the content of a single file 'pucFrom' to 'pucTo'. There is neither globbing nor
  directory handling (Use cxp:system instead).

  \param pucArgFrom source file name
  \param pucArgTo directory or file name
  \param fArgMove move flag
  \return TRUE if successful

  \bug handling of stdin, stdout
*/
BOOL_T
resNodeTransferStr(xmlChar *pucArgFrom, xmlChar *pucArgTo, BOOL_T fArgMove)
{
  BOOL_T fResult = FALSE;

#ifdef DEBUG
  PrintFormatLog(4,"resNodeTransferStr('%s','%s',%i)",pucArgFrom, pucArgTo, fArgMove);
#endif

  assert(resPathIsAbsolute(pucArgFrom));
  assert(resPathIsAbsolute(pucArgTo));
  
  if (STR_IS_EMPTY(pucArgFrom)) {
    PrintFormatLog(1,"No valid source name '%s'", pucArgFrom);
  }
  else if (STR_IS_EMPTY(pucArgTo)) {
    PrintFormatLog(1,"No valid destination name '%s'", pucArgTo);
  }
  else {
    resNodePtr prnFrom = NULL;
    resNodePtr prnTo   = NULL;

    prnFrom = resNodeDirNew(pucArgFrom);
    prnTo = resNodeDirNew(pucArgTo);
    fResult = resNodeTransfer(resNodeGetLastDescendant(prnFrom), prnTo, fArgMove);
    resNodeFree(prnTo);
    resNodeFree(prnFrom);
  }
  return fResult;
} /* end of resNodeTransferStr() */


/*! copies the content of a single filesystem context 'prnArgFrom' to 'prnArgTo'.

  \param prnArgFrom source filesystem context
  \param prnArgTo directory or file filesystem context
  \param fArgMove move flag

  \return TRUE if successful

  the source file must exist

  the destination may be

  - stdout

  - an existing or non-existing file or

  - directory

    \todo accept stdin and http://
    \todo also other types of URI
*/
BOOL_T
resNodeTransfer(resNodePtr prnArgFrom, resNodePtr prnArgTo, BOOL_T fArgMove)
{
  BOOL_T fResult = TRUE;

  //  assert(prnArgFrom != NULL);

  /********************************************************************************
     check source
  */
  if (fResult) {
    if (resNodeIsError(prnArgFrom)) {
      resNodeSetError(prnArgFrom,rn_error_copy,"Copy source contexts not usable");
      fResult = FALSE;
    }
    else if (resNodeReadStatus(prnArgFrom)) { /* try to get some context info */
      if (resNodeIsURL(prnArgFrom)) {
	/* OK */
      }
      else if (resNodeIsFileInArchive(prnArgFrom)) {
#ifdef HAVE_LIBARCHIVE
#else
	PrintFormatLog(1, "un-archiving not compiled");
	fResult = FALSE;
#endif
      }
      else if (resNodeIsArchive(prnArgFrom)) {
      }
      else if (resNodeGetType(prnArgFrom) != rn_type_file) {
	resNodeSetError(prnArgFrom,rn_error_copy,"Copy source context is not an existing file with content");
	fResult = FALSE;
      }
    }
    else {
      resNodeSetError(prnArgFrom,rn_error_copy, "Cant copy source context");
      fResult = FALSE;
    }
  }

  /********************************************************************************
     check destination
  */
  if (fResult) {
    if (resNodeIsError(prnArgTo) && resNodeGetError(prnArgTo) != rn_error_stat) {
      resNodeSetError(prnArgTo,rn_error_copy,"Copy destination contexts not usable");
      fResult = FALSE;
    }
    else if (resNodeIsStd(prnArgTo)) {
      if (fArgMove) {
	resNodeSetError(prnArgTo, rn_error_copy, "Moving to stdout is not permitted");
	fResult = FALSE;
      }
    }
    else if (resNodeReadStatus(prnArgTo)) { /* try to get some context info */
      if (resNodeIsDir(prnArgTo)) {
	/* existing directory */
	resNodeConcat(prnArgTo, resNodeGetNameBase(prnArgFrom));
	resNodeReadStatus(prnArgTo); /* try to get some context info */
      }
      else if (resNodeIsFile(prnArgTo) && fArgMove) {
	resNodeSetError(prnArgTo,rn_error_copy,"Cant move file to an existing one");
	fResult = FALSE;
      }
    }
    else if (resNodeIsDir(prnArgTo) || resPathIsDir(resNodeGetNameNormalized(prnArgTo))) { /* path is a directory, but not existant yet */
      if (resNodeMakeDirectoryStr(resNodeGetNameNormalized(prnArgTo), MODE_DIR_CREATE) == TRUE) {
	resNodeConcat(prnArgTo, resNodeGetNameBase(prnArgFrom));
	resNodeReadStatus(prnArgTo); /* try to get some context info */
      }
      else {
	fResult = FALSE;
      }
    }
    else { /* prnArgTo is an non-existing file node */
    }

    if (resNodeIsReadable(prnArgTo) && fArgMove) {
      resNodeSetError(prnArgTo,rn_error_copy,"Cant move file to an existing one");
      fResult = FALSE;
    }
  }

  /********************************************************************************
     check source to destination
  */
  if (fResult) {
    if (resPathIsEquivalent(resNodeGetNameNormalized(prnArgTo),resNodeGetNameNormalized(prnArgFrom))) {
      resNodeSetError(prnArgTo,rn_error_copy,"Source and destination are the same file '%s'",
		      resNodeGetNameNormalized(prnArgFrom));
      fResult = FALSE;
    }
  }

  /********************************************************************************
     ... and action ...
  */
  if (fResult) {
    if (resNodeIsStd(prnArgTo)) {
      fResult = (resNodeGetContent(prnArgFrom,1024) && resNodeSwapContent(prnArgFrom, prnArgTo) && resNodePutContent(prnArgTo));
    }
    else if (resNodeMakeDirectoryStr(resNodeGetNameBaseDir(prnArgTo),MODE_DIR_CREATE) == FALSE) {
      fResult = FALSE;
    }
    else if (fArgMove) {
      int err;

      PrintFormatLog(3,"Move '%s' to '%s'",
		     resNodeGetNameNormalized(prnArgFrom),
		     resNodeGetNameNormalized(prnArgTo));
#ifdef _MSC_VER
      err = MoveFile(resNodeGetNameNormalizedNative(prnArgFrom),
		     resNodeGetNameNormalizedNative(prnArgTo));
      if (err == 0) {
	resNodeSetError(prnArgTo,rn_error_copy,"Move error '%i'", GetLastError());
      }
#else
      /* detect distinct volumes and combine copy() and unlink() */
      err = rename(resNodeGetNameNormalizedNative(prnArgFrom),
		   resNodeGetNameNormalizedNative(prnArgTo));
      if (err) {
	if (errno==EROFS || errno==EXDEV) {
	  /* rename fails if different volumes */
	  resNodeSetError(prnArgFrom,rn_error_copy,"Cant move to an other filesystem, will copy instead");
	  fResult = (resNodeTransfer(prnArgFrom,prnArgTo,FALSE)
		     && resNodeUnlink(prnArgFrom,FALSE));
	}
	else {
	  resNodeSetError(prnArgFrom,rn_error_copy,"Error moving '%i'",errno);
	}
	//prnArgTo->eError = error_move;
      }
#endif
      else {
	PrintFormatLog(3,"OK moving");
	fResult = TRUE;
      }
    }
    else {
      fResult = resNodeSwapContent(prnArgFrom,prnArgTo) && resNodePutContent(prnArgTo);
    }
  }
  return fResult;
} /* end of resNodeTransfer() */


/*! creates a given path string as directories in filesystem

  \param path relative or absolute path and name of directory
  \param mode argument for mkdir()
  \return 
 */
BOOL_T
resNodeMakeDirectoryStr(xmlChar *pucArgPath, int mode)
{
  BOOL_T fResult = FALSE;

  if (STR_IS_NOT_EMPTY(pucArgPath)) {
    resNodePtr prnDir;

    prnDir = resNodeSplitStrNew(pucArgPath);
    if (prnDir) {
      fResult = resNodeMakeDirectory(prnDir,mode);
      resNodeFree(prnDir);
    }
  }
  return fResult;
} /* end of resNodeMakeDirectoryStr() */


/*! creates a given path split node list as directories in filesystem

  \param prnArg pointer to node list of single directory names
  \param mode argument for mkdir()
  \return 
 */
BOOL_T
resNodeMakeDirectory(resNodePtr prnArg, int mode)
{
  BOOL_T fResult = FALSE;

  if (prnArg) {
    resNodeReadStatus(prnArg);

    if (resNodeGetType(prnArg) == rn_type_root) {
      /* ignoring root node */
      fResult = TRUE;
    }
    else if (resNodeIsExist(prnArg)) {
      /* OK */
      fResult = TRUE;
    }
    else {
      int iResult;

      PrintFormatLog(1, "MKDIR '%s'", resNodeGetNameNormalized(prnArg));
#ifdef _MSC_VER
      iResult = CreateDirectory(resNodeGetNameNormalizedNative(prnArg), NULL);
      if (iResult == 0) {
	DWORD dwErrorCode = GetLastError();
	if (dwErrorCode == ERROR_ALREADY_EXISTS) {
	  resNodeSetError(prnArg, rn_error_mkdir, "Directory exists already");
	}
	else if (dwErrorCode == ERROR_PATH_NOT_FOUND) {
	  resNodePtr prnT;

	  prnT = resNodeDirNew(resNodeGetNameBaseDir(prnArg));
	  if (resNodeMakeDirectory(prnT, mode)) {
	    fResult = resNodeMakeDirectory(prnArg, mode);
	  }
	  resNodeFree(prnT);
	}
	else {
	  resNodeSetError(prnArg, rn_error_mkdir, "Cant create directory");
	}
	//break;
      }
#else
      iResult = mkdir(resNodeGetNameNormalizedNative(prnArg), mode);
      if (iResult) {
	resNodeSetError(prnArg, rn_error_mkdir, "Cant create directory '%s'", resNodeGetNameNormalized(prnArg));
	fResult = FALSE;
	break;
      }
#endif
      else {
	resNodeResetError(prnArg);
	fResult = resNodeReadStatus(prnArg);
      }
    }

    fResult = resNodeMakeDirectory(resNodeGetChild(prnArg), mode);
    fResult = resNodeMakeDirectory(resNodeGetNext(prnArg), mode);
    fResult = TRUE;
  }
  return fResult;
} /* end of resNodeMakeDirectory() */


/*! remove a file or directory with given name 

  \param pucArgPath relative or absolute path and name of file
  \return true if the named file is is removed
*/
BOOL_T
resNodeUnlinkStr(xmlChar *pucArgPath)
{
  BOOL_T fResult = FALSE;

  if (STR_IS_NOT_EMPTY(pucArgPath)) {
    resNodePtr prnUnlink;

    /*\todo unlink non-empty subdirs too */
    
    prnUnlink = resNodeDirNew(pucArgPath);
    fResult = resNodeUnlink(prnUnlink,FALSE);
    resNodeFree(prnUnlink);
  }
  return fResult;
} /* end of resNodeUnlinkStr() */


/*! remove a directory with given name recursively

  \param pucArgPath relative or absolute path of directory
  \return true if the named directory is removed
*/
BOOL_T
resNodeUnlinkRecursivelyStr(xmlChar *pucArgPath)
{
  BOOL_T fResult = FALSE;

#ifdef EXPERIMENTAL
  if (STR_IS_NOT_EMPTY(pucArgPath)) {
    resNodePtr prnUnlink;

    /*\todo unlink non-empty subdirs too */
    
    prnUnlink = resNodeDirNew(pucArgPath);
    fResult = resNodeUnlink(prnUnlink,TRUE);
    resNodeFree(prnUnlink);
  }
#else
  PrintFormatLog(1,"Recursive directory delete is an experimental feature yet ('%s')", pucArgPath);
#endif
  
  return fResult;
} /* end of resNodeUnlinkRecursivelyStr() */


/*! remove a file or directory with given name 

  \param pucArgPath relative or absolute path and name of file
  \return true if the named file is is removed
*/
BOOL_T
resNodeUnlink(resNodePtr prnArg, BOOL_T fRecursively)
{
  int err;
  BOOL_T fResult = FALSE;

  if (resNodeReadStatus(prnArg)) {
    resNodeClose(prnArg);    
    if (resNodeIsFile(prnArg)) {
      PrintFormatLog(4,"Delete file '%s'", resNodeGetNameNormalized(prnArg));
#ifdef _MSC_VER
      err = DeleteFile(resNodeGetNameNormalizedNative(prnArg));
      if (err == 0) {
	int iErrCode = GetLastError();
	switch (iErrCode) {
	case ERROR_ACCESS_DENIED:
	case ERROR_INVALID_ACCESS:
	  resNodeSetError(prnArg,rn_error_access,"access");
	  break;
	case ERROR_FILE_NOT_FOUND:
	case ERROR_PATH_NOT_FOUND:
	  resNodeSetError(prnArg,rn_error_find,"find");
	  break;
	default:
	  resNodeSetError(prnArg,rn_error_undef,"Delete error '%i'", iErrCode);
	  break;
	}
      }
#else
      err = unlink(resNodeGetNameNormalizedNative(prnArg));
      if (err) {
	switch (errno) {
	case EACCES:
	  resNodeSetError(prnArg,rn_error_access,"access");
	  break;
	case EBUSY:
	  resNodeSetError(prnArg,rn_error_busy,"busy");
	  break;
	case EFAULT:
	  resNodeSetError(prnArg,rn_error_path,"path");
	  break;
	case EIO:
	  resNodeSetError(prnArg,rn_error_undef,"undef");
	  break;
	case EISDIR:
	  resNodeSetError(prnArg,rn_error_undef,"undef");
	  break;
	case ELOOP:
	  resNodeSetError(prnArg,rn_error_undef,"undef");
	  break;
	case ENAMETOOLONG:
	  resNodeSetError(prnArg,rn_error_max_path,"max_path");
	  break;
	case ENOENT:
	  resNodeSetError(prnArg,rn_error_find,"find");
	  break;
	case ENOMEM:
	  resNodeSetError(prnArg,rn_error_undef,"undef");
	  break;
	case ENOTDIR:
	  resNodeSetError(prnArg,rn_error_undef,"undef");
	  break;
	case EPERM:
	  resNodeSetError(prnArg,rn_error_access,"access");
	  break;
	case EROFS:
	  resNodeSetError(prnArg,rn_error_undef,"undef");
	  break;
	default:
	  resNodeSetError(prnArg,rn_error_undef,"undef");
	  break;
	}
	//PrintFormatLog(1,"Delete error '%i'", errno);
      }
#endif
      else {
	resNodeResetError(prnArg);
	resNodeSetType(prnArg,rn_type_undef);
	fResult = TRUE;
      }
    }
    else if (resNodeIsDir(prnArg)) {

      if (fRecursively == FALSE) {
      }
      else if (resNodeListParse(prnArg, 1, NULL) == FALSE) { /*! read Resource Node as list of childs */
      }
      else if (resNodeUpdate(prnArg, RN_INFO_MIN, NULL, NULL) == FALSE) { /*! read Resource Node as list of childs */
      }
      else {
	resNodePtr prnT;

	for (prnT = resNodeGetChild(prnArg); prnT; prnT = resNodeGetNext(prnT)) {
	  fResult |= resNodeUnlink(prnT,TRUE);
	}
      }
      
      PrintFormatLog(1,"Delete empty directory '%s'", resNodeGetNameNormalized(prnArg));
#ifdef _MSC_VER
      err = RemoveDirectory(resNodeGetNameNormalizedNative(prnArg));
      if (err == 0) {
	int iErrCode = GetLastError();
	switch (iErrCode) {
	case ERROR_ACCESS_DENIED:
	case ERROR_INVALID_ACCESS:
	  resNodeSetError(prnArg,rn_error_access,"Delete error '%i'", iErrCode);
	  break;
	case ERROR_FILE_NOT_FOUND:
	case ERROR_PATH_NOT_FOUND:
	  resNodeSetError(prnArg,rn_error_find,"Delete error '%i'", iErrCode);
	  break;
	default:
	  resNodeSetError(prnArg,rn_error_undef,"Delete error '%i'", iErrCode);
	  break;
	}
      }
      else {
	resNodeResetError(prnArg);
	resNodeSetType(prnArg,rn_type_undef);
	fResult = TRUE;
      }
#else
      err = rmdir(resNodeGetNameNormalizedNative(prnArg));
      if (err) {
	  switch (errno) {
	  case EACCES:
	  case EPERM:
	    resNodeSetError(prnArg,rn_error_access,"access");
	    break;
	  case EBUSY:
	    resNodeSetError(prnArg,rn_error_busy,"busy");
	    break;
	  case EFAULT:
	    resNodeSetError(prnArg,rn_error_path,"path");
	    break;
	  case EIO:
	    resNodeSetError(prnArg,rn_error_undef,"undef");
	    break;
	  case EISDIR:
	    resNodeSetError(prnArg,rn_error_undef,"undef");
	    break;
	  case ELOOP:
	    resNodeSetError(prnArg,rn_error_undef,"undef");
	    break;
	  case ENAMETOOLONG:
	    resNodeSetError(prnArg,rn_error_max_path,"max_path");
	    break;
	  case ENOENT:
	    resNodeSetError(prnArg,rn_error_find,"find");
	    break;
	  case ENOMEM:
	    resNodeSetError(prnArg,rn_error_undef,"undef");
	    break;
	  case ENOTDIR:
	    resNodeSetError(prnArg,rn_error_undef,"undef");
	    break;
	  case EROFS:
	    resNodeSetError(prnArg,rn_error_undef,"undef");
	    break;
	  default:
	    resNodeSetError(prnArg,rn_error_undef,"Delete error '%i'", errno);
	    break;
	  }
      }
      else {
	resNodeResetError(prnArg);
	resNodeSetType(prnArg,rn_type_undef);
	fResult = TRUE;
      }
#endif
    }
    else {
      resNodeSetError(prnArg,rn_error_unlink, "Non-existing context '%s'", resNodeGetNameNormalized(prnArg));
    }
  }
  return fResult;
} /* end of resNodeUnlink() */


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


#ifdef TESTCODE
#include "test/test_res_node_ops.c"
#endif
