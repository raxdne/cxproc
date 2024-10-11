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
RN_ERROR
resNodeTransferStr(xmlChar *pucArgFrom, xmlChar *pucArgTo, BOOL_T fArgMove)
{
  RN_ERROR eResult = rn_error_undef;

#ifdef DEBUG
  PrintFormatLog(4,"resNodeTransferStr('%s','%s',%i)",pucArgFrom, pucArgTo, fArgMove);
#endif

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
    eResult = resNodeTransfer(resNodeGetLastDescendant(prnFrom), prnTo, fArgMove);
    resNodeFree(prnTo);
    resNodeFree(prnFrom);
  }
  return eResult;
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
RN_ERROR
resNodeTransfer(resNodePtr prnArgFrom, resNodePtr prnArgTo, BOOL_T fArgMove)
{
  RN_ERROR eResult = rn_error_none;

  /********************************************************************************
     check source
  */
  if (eResult == rn_error_none) {
    if (prnArgFrom == NULL) {
      eResult = rn_error_copy;
    }
    else if (resNodeIsError(prnArgFrom)) {
      resNodeSetError(prnArgFrom,rn_error_copy,"Copy source contexts not usable");
      eResult = rn_error_undef;
    }
    else if (resNodeReadStatus(prnArgFrom)) { /* try to get some context info */
      if (resNodeIsURL(prnArgFrom)) {
	/* OK */
      }
      else if (resNodeIsFileInArchive(prnArgFrom)) {
#ifdef HAVE_LIBARCHIVE
#else
	PrintFormatLog(1, "un-archiving not compiled");
	eResult = rn_error_undef;
#endif
      }
      else if (resNodeIsArchive(prnArgFrom)) {
      }
      else if (resNodeGetType(prnArgFrom) != rn_type_file) {
	resNodeSetError(prnArgFrom,rn_error_copy,"Copy source context is not an existing file with content");
	eResult = rn_error_undef;
      }
      else if (fArgMove && resNodeParentIsWriteable(prnArgFrom) == FALSE) {
	resNodeSetError(prnArgFrom,rn_error_access,"Move of source context is not permitted");
	eResult = rn_error_access;
      }
    }
    else {
      resNodeSetError(prnArgFrom,rn_error_copy, "Cant copy source context");
      eResult = rn_error_undef;
    }
  }

  /********************************************************************************
     check destination
  */
  if (eResult == rn_error_none) {
    if (prnArgTo == NULL) {
      eResult = rn_error_copy;
    }
    else if (resNodeIsError(prnArgTo) && resNodeGetError(prnArgTo) != rn_error_stat) {
      resNodeSetError(prnArgTo,rn_error_copy,"Copy destination contexts not usable");
      eResult = rn_error_undef;
    }
    else if (resNodeIsStd(prnArgTo)) {
      if (fArgMove) {
	resNodeSetError(prnArgTo, rn_error_copy, "Moving to stdout is not permitted");
	eResult = rn_error_undef;
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
	/*!\todo move to filename with iteration suffix 'abc.txt.1' */
	eResult = rn_error_undef;
      }
    }
    else if (resNodeIsFile(prnArgTo) && resNodeParentIsWriteable(prnArgTo) == FALSE) { /* path is a file, but base directory not existant yet */
      if (resNodeMakeDirectoryStr(resNodeGetNameBaseDir(prnArgTo), MODE_DIR_CREATE) == rn_error_none) {
	resNodeReadStatus(prnArgTo); /* try to get some context info */
      }
      else {
	//resNodeSetError(prnArgTo,rn_error_mkdir,"Cant move file to an existing one");
	eResult = rn_error_mkdir;
      }
    }
    else if (resNodeIsDir(prnArgTo) || resPathIsDir(resNodeGetNameNormalized(prnArgTo))) { /* path is a directory, but not existant yet */
      if (resNodeMakeDirectoryStr(resNodeGetNameNormalized(prnArgTo), MODE_DIR_CREATE) == rn_error_none) {
	resNodeConcat(prnArgTo, resNodeGetNameBase(prnArgFrom));
	resNodeReadStatus(prnArgTo); /* try to get some context info */
      }
      else {
	eResult = rn_error_mkdir;
      }
    }
    else { /* prnArgTo is an non-existing file node */
    }

    if (resNodeIsReadable(prnArgTo) && fArgMove) {
      resNodeSetError(prnArgTo,rn_error_copy,"Cant move file to an existing one");
      eResult = rn_error_undef;
    }
  }

  /********************************************************************************
     check source to destination
  */
  if (eResult == rn_error_none) {
    if (resPathIsEquivalent(resNodeGetNameNormalized(prnArgTo),resNodeGetNameNormalized(prnArgFrom))) {
      resNodeSetError(prnArgTo,rn_error_copy,"Source and destination are the same file '%s'",
		      resNodeGetNameNormalized(prnArgFrom));
      eResult = rn_error_undef;
    }
  }

  /********************************************************************************
     ... and action ...
  */
  if (eResult == rn_error_none) {
    if (resNodeIsStd(prnArgTo)) {
      if (resNodeGetContent(prnArgFrom, 1024) != NULL && resNodeSwapContent(prnArgFrom, prnArgTo) == rn_error_none && resNodePutContent(prnArgTo)) {
	/* OK */
      }
      else {
	eResult = rn_error_undef;
      }
    }
    else if (resNodeMakeDirectoryStr(resNodeGetNameBaseDir(prnArgTo), MODE_DIR_CREATE) != rn_error_none) {
      eResult = rn_error_undef;
    }
    else if (fArgMove) {
      int e;

      PrintFormatLog(3, "Move '%s' to '%s'", resNodeGetNameNormalized(prnArgFrom), resNodeGetNameNormalized(prnArgTo));
#ifdef _MSC_VER
      e = MoveFile(resNodeGetNameNormalizedNative(prnArgFrom), resNodeGetNameNormalizedNative(prnArgTo));
      if (e == 0) {
	resNodeSetError(prnArgTo, rn_error_copy, "Move error '%i'", GetLastError());
      }
#else
      /* detect distinct volumes and combine copy() and unlink() */
      e = rename(resNodeGetNameNormalizedNative(prnArgFrom), resNodeGetNameNormalizedNative(prnArgTo));
      if (e) {
	if (errno == EROFS || errno == EXDEV) {
	  /* rename fails if different volumes */
	  resNodeSetError(prnArgFrom, rn_error_copy, "Cant move to an other filesystem, will copy instead");
	  eResult = (resNodeTransfer(prnArgFrom, prnArgTo, FALSE) && resNodeUnlink(prnArgFrom, FALSE));
	}
	else {
	  resNodeSetError(prnArgFrom, rn_error_copy, "Error moving '%i'", errno);
	}
	// prnArgTo->eError = error_move;
      }
#endif
      else {
	PrintFormatLog(3, "OK moving");
	eResult = rn_error_none;
      }
    }
    else if (resNodeSwapContent(prnArgFrom, prnArgTo) == rn_error_none) {
      if (resNodePutContent(prnArgTo)) {}
    }
  }
  return eResult;
} /* end of resNodeTransfer() */


/*! creates a given path string as directories in filesystem

  \param path relative or absolute path and name of directory
  \param mode argument for mkdir()
  \return 
 */
RN_ERROR
resNodeMakeDirectoryStr(xmlChar *pucArgPath, int mode)
{
  RN_ERROR eResult = rn_error_undef;

  if (STR_IS_NOT_EMPTY(pucArgPath)) {
    resNodePtr prnDir;

    prnDir = resNodeSplitStrNew(pucArgPath);
    if (prnDir) {
      eResult = resNodeMakeDirectory(prnDir,mode);
      resNodeFree(prnDir);
    }
  }
  return eResult;
} /* end of resNodeMakeDirectoryStr() */


/*! creates a given path split node list as directories in filesystem

  \param prnArg pointer to node list of single directory names
  \param mode argument for mkdir()
  \return 
 */
RN_ERROR
resNodeMakeDirectory(resNodePtr prnArg, int mode)
{
  RN_ERROR eResult = rn_error_undef;

  if (prnArg) {
    resNodeReadStatus(prnArg);

    if (resNodeGetType(prnArg) == rn_type_root) {
      /* ignoring root node */
      eResult = rn_error_none;
    }
    else if (resNodeGetType(prnArg) == rn_type_stdout || resNodeGetType(prnArg) == rn_type_stdin || resNodeGetType(prnArg) == rn_type_stderr) {
      /* ignoring std* nodes */
      eResult = rn_error_none;
    }
    else if (resNodeIsFile(prnArg)) {
	resNodePtr prnParent;
      /* make only parent directory for this file */

      prnParent = resNodeDup(prnArg,RN_DUP_THIS);
      resNodeSetToParent(prnParent);
      eResult = resNodeMakeDirectory(prnParent, mode);
      /*!\todo check errors */
      resNodeFree(prnParent);
    }
    else if (resNodeIsExist(prnArg)) {
      /* OK */
      eResult = rn_error_none;
    }
    else {
      int i;

      PrintFormatLog(3, "MKDIR '%s'", resNodeGetNameNormalized(prnArg));
#ifdef _MSC_VER
      i = CreateDirectory(resNodeGetNameNormalizedNative(prnArg), NULL);
      if (i == 0) {
	DWORD dwErrorCode = GetLastError();
	if (dwErrorCode == ERROR_ALREADY_EXISTS) {
	  eResult = resNodeReadStatus(prnArg) ? rn_error_none : rn_error_undef;
	}
	else if (dwErrorCode == ERROR_PATH_NOT_FOUND) {
	  resNodePtr prnT;

	  prnT = resNodeDirNew(resNodeGetNameBaseDir(prnArg));
	  if (resNodeMakeDirectory(prnT, mode)) {
	    i = resNodeMakeDirectory(prnArg, mode);
	  }
	  resNodeFree(prnT);
	}
	else {
	  resNodeSetError(prnArg, rn_error_mkdir, "Cant create directory");
	}
      }
      else {
	eResult = rn_error_none;
      }
#else
      i = mkdir(resNodeGetNameNormalizedNative(prnArg), mode);
      if (i == 0 || errno == EEXIST) {
	eResult = resNodeReadStatus(prnArg) ? rn_error_none : rn_error_undef;
      }
      else if (errno == EACCES || errno == EPERM) {
	resNodeSetError(prnArg, rn_error_mkdir, "access error");
      }
      else if (errno == ENOENT) {
	resNodePtr prnT;

	prnT = resNodeDirNew(resNodeGetNameBaseDir(prnArg));
	if (resNodeMakeDirectory(prnT, mode) == rn_error_none) {
	  eResult = resNodeMakeDirectory(prnArg, mode);
	}
	resNodeFree(prnT);
      }
      else {
	resNodeSetError(prnArg, rn_error_mkdir, "misc error");
      }
#endif
    }

    if (eResult == rn_error_none && resNodeGetChild(prnArg) != NULL) {
      eResult = resNodeMakeDirectory(resNodeGetChild(prnArg), mode);
      // eResult = resNodeMakeDirectory(resNodeGetNext(prnArg), mode);
    }
  }
  return eResult;
} /* end of resNodeMakeDirectory() */


/*! remove a file or directory with given name 

  \param pucArgPath relative or absolute path and name of file
  \return true if the named file is is removed
*/
RN_ERROR
resNodeUnlinkStr(xmlChar *pucArgPath)
{
  RN_ERROR eResult = rn_error_undef;

  if (STR_IS_NOT_EMPTY(pucArgPath)) {
    resNodePtr prnUnlink;

    /*\todo unlink non-empty subdirs too */
    
    prnUnlink = resNodeDirNew(pucArgPath);
    eResult = resNodeUnlink(prnUnlink,FALSE);
    resNodeFree(prnUnlink);
  }
  return eResult;
} /* end of resNodeUnlinkStr() */


/*! remove a directory with given name recursively

  \param pucArgPath relative or absolute path of directory
  \return true if the named directory is removed
*/
RN_ERROR
resNodeUnlinkRecursivelyStr(xmlChar *pucArgPath)
{
  RN_ERROR eResult = rn_error_undef;

  if (STR_IS_NOT_EMPTY(pucArgPath)) {
    resNodePtr prnUnlink;

    /*\todo unlink non-empty subdirs too */
    
    prnUnlink = resNodeDirNew(pucArgPath);
    eResult = resNodeUnlink(prnUnlink,TRUE);
    resNodeFree(prnUnlink);
  }
  
  return eResult;
} /* end of resNodeUnlinkRecursivelyStr() */


/*! remove a file or directory with given name 

  \param pucArgPath relative or absolute path and name of file
  \return true if the named file is is removed
*/
RN_ERROR
resNodeUnlink(resNodePtr prnArg, BOOL_T fRecursively)
{
  int e;
  RN_ERROR eResult = rn_error_undef;

  if (resNodeReadStatus(prnArg)) {
    resNodeClose(prnArg);    
    if (resNodeIsFile(prnArg)) {
      PrintFormatLog(4,"Delete file '%s'", resNodeGetNameNormalized(prnArg));
#ifdef _MSC_VER
      e = DeleteFile(resNodeGetNameNormalizedNative(prnArg));
      if (e == 0) {
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
      e = unlink(resNodeGetNameNormalizedNative(prnArg));
      if (e) {
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
	eResult = rn_error_none;
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
	  eResult |= resNodeUnlink(prnT,TRUE);
	}
      }
      
      PrintFormatLog(1,"Delete empty directory '%s'", resNodeGetNameNormalized(prnArg));
#ifdef _MSC_VER
      e = RemoveDirectory(resNodeGetNameNormalizedNative(prnArg));
      if (e == 0) {
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
	eResult = rn_error_none;
      }
#else
      e = rmdir(resNodeGetNameNormalizedNative(prnArg));
      if (e) {
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
	eResult = rn_error_none;
      }
#endif
    }
    else {
      resNodeSetError(prnArg,rn_error_unlink, "Non-existing context '%s'", resNodeGetNameNormalized(prnArg));
    }
  }
  return eResult;
} /* end of resNodeUnlink() */


#ifdef TESTCODE
#include "test/test_res_node_ops.c"
#endif
