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

#define FS_LENGTH_EXT 11 /* maximum length of file extension */

#ifdef _MSC_VER
#define PATHLIST_SEPARATOR (xmlChar)';'
#else
#define PATHLIST_SEPARATOR (xmlChar)':'
#endif

#define isdot(C) (C == (xmlChar)'.')

#ifdef _MSC_VER
/* double quote only, single quote is allowed in names */
#define isquot(C) (C == (xmlChar)'\"')
#else
/* double quote and single quote allowed */
#define isquot(C) (C == (xmlChar)'\"' || C == (xmlChar)'\'')
#endif

#define issep(C) (C == (xmlChar)'/' || C == (xmlChar)'\\')

#define isseporend(C) (issep(C) || isend(C))

extern BOOL_T
resPathSetNativeEncoding(const char *pcArg);

extern void
resPathCleanup(void);

extern xmlChar *
resPathEncode(const char *pchArg);

extern char *
resPathDecode(xmlChar *pucArg);

extern xmlChar *
resPathGetQuoted(xmlChar *pucArgNameFile);

extern int
resPathGetDepth(xmlChar *pucArgPath);

extern xmlChar *
resPathGetExtension(xmlChar *pucArg);

extern xmlChar *
resPathConcat(xmlChar *pucArgA, xmlChar *pucArgB);

extern xmlChar *
resPathConcatNormalized(xmlChar *pucArgA, xmlChar *pucArgB);

extern xmlChar *
resPathNormalize(xmlChar *pucArg);

#define FS_PATH_NUL  (0)
#define FS_PATH_SELF (1)
#define FS_PATH_UP   (2)
#define FS_PATH_SEP  (4)

#define FS_PATH_FULL (FS_PATH_SELF | FS_PATH_UP | FS_PATH_SEP)

extern xmlChar *
resPathCollapse(xmlChar *pucArg, int iArgOpts);

extern xmlChar *
resPathGetCwd(void);

extern xmlChar *
resPathRemoveQuotes(xmlChar *pucArg);

extern xmlChar *
resPathGetBasedir(xmlChar *str_filename);

extern xmlChar *
resPathGetRootname(xmlChar *pucArgNameFile);

extern BOOL_T
resPathIsMatchingBegin(xmlChar *pucArgPath, xmlChar *pucArgNameFile);

extern BOOL_T
resPathIsMatchingEnd(xmlChar *pucArgPath, xmlChar *pucArgNameFile);

extern BOOL_T
resPathIsDescendant(xmlChar *pucArgPath, xmlChar *pucArgPathDescendant);

extern xmlChar *
resPathSkipPrefixFilePtr(xmlChar *pucArgPath);

extern xmlChar *
resPathDiffPtr(xmlChar *pucArgPath, xmlChar *pucArgPathDescendant);

extern xmlChar *
resPathGetBasename(xmlChar *pucArgNameFile);

extern xmlChar *
resPathCutTrailingChars(xmlChar *pucArgPath);

extern xmlChar *
resPathChangeToSlashes(xmlChar *pucArgPath);

extern xmlChar *
resPathGetNextPathPtr(xmlChar *pucArgPath);

extern xmlChar *
resPathGetNextSeparator(xmlChar *pucArgPath);

extern xmlChar *
resPathGetDirFind(xmlChar *pucArgPath, xmlChar *pucArgNeedle);

extern BOOL_T
resPathIsDir(xmlChar *pucArg);

extern BOOL_T
resPathIsDirRecursive(xmlChar *pucArg);

extern BOOL_T
resPathIsStd(xmlChar *pucArg);

extern BOOL_T
resPathIsInMemory(xmlChar *pucArg);

extern BOOL_T
resPathIsRoot(xmlChar *pucArg);

extern BOOL_T
resPathIsURL(xmlChar *pucArg);

extern BOOL_T
resPathIsHttpURL(xmlChar *pucArg);

extern BOOL_T
resPathIsFtpURL(xmlChar *pucArg);

extern BOOL_T
resPathIsFileURL(xmlChar *pucArg);

extern BOOL_T
resPathBeginIsFileCompressed(xmlChar *pucArg);

extern BOOL_T
resPathBeginIsArchive(xmlChar *pucArg);

extern xmlChar *
resPathGetNameOfNextArchivePtr(xmlChar *pucArg);

extern xmlChar *
resPathGetPathInNextArchivePtr(xmlChar *pucArg);

#define resPathIsInArchive(P) (resPathGetPathInNextArchivePtr(P) != NULL)

extern xmlChar *
resPathGetPathOfArchive(xmlChar *pucArg);

extern BOOL_T
resPathIsRelative(xmlChar *pucArg);

extern BOOL_T
resPathIsEquivalent(xmlChar *pucArgA, xmlChar *pucArgB);

#define resPathIsAbsolute(P) (resPathIsRelative(P) == FALSE)

extern BOOL_T
resPathIsDosDrive(xmlChar *pucArg);

extern BOOL_T
resPathIsLeadingSeparator(xmlChar *pucArg);

extern BOOL_T
resPathIsTrailingSeparator(xmlChar *pucArg);

extern BOOL_T
resPathIsUNC(xmlChar *pucArg);

#ifdef TESTCODE
extern int
resNodeTestString(void);
#endif

