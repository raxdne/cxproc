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

/*! bits for finding of resource nodes in resNodeListFindPath()
 */
#define RN_FIND_MIN        (0)

#define RN_FIND_FILE       (1)

#define RN_FIND_DIR        (2)

#define RN_FIND_IN_SUBDIR  (4)

#define RN_FIND_IN_ARCHIVE (8)

#define RN_FIND_NEXT	  (16)

#define RN_FIND_ALL        (RN_FIND_FILE | RN_FIND_DIR | RN_FIND_IN_SUBDIR | RN_FIND_IN_ARCHIVE | RN_FIND_NEXT)

extern BOOL_T
resNodeListParse(resNodePtr prnArg, int iArgDepth, const pcre2_code *re_match);

extern void
resNodeListFree(resNodePtr prnArg);

extern BOOL_T
resNodeListUnlink(resNodePtr prnArg);

extern BOOL_T
resNodeListUnlinkDescendants(resNodePtr prnArg);

extern resNodePtr
resNodeListFindChild(resNodePtr prnArg, xmlChar *pucName);

extern resNodePtr
resNodeListFindPath(resNodePtr prnArg, xmlChar *pucArgPath, int iArgOptions);

extern resNodePtr
resNodeListFindPathNext(resNodePtr prnArg, xmlChar *pucArgPath, int iArgOptions);

extern xmlNodePtr
resNodeListToDOM(resNodePtr prnArg, int iLevelVerboseArg);

extern xmlChar *
resNodeListToSQL(resNodePtr prnArg, int iLevelVerboseArg);

extern xmlChar *
resNodeListToJSON(resNodePtr prnArg, int iLevelVerboseArg);

extern xmlChar *
resNodeListToXml(resNodePtr prnArg, int iLevelVerboseArg);

extern xmlChar *
resNodeListToPlain(resNodePtr prnArg, int iLevelVerboseArg);

extern xmlChar *
resNodeListToGraphviz(resNodePtr prnArg, int iArgOptions);

extern BOOL_T
resNodeListDumpRecursively(FILE *argout, resNodePtr prnArg, xmlChar *(*pfArg)(resNodePtr, int));

extern xmlChar*
resNodeListToPlainTree(resNodePtr prnArg, xmlChar* pucArgPrefix, int iArgOptions);

#ifdef TESTCODE
int
resNodeTestList(void);
#endif
