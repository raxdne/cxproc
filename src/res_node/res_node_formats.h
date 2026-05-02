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

/*! flags for output of details
*/

#define RN_OUT_NAME    (1) /*! name only */

#define RN_OUT_ATTR    (RN_OUT_NAME * 2) /*! stat() */

#define RN_OUT_SIZE    (RN_OUT_ATTR * 2) /*! size of resource */

#define RN_OUT_TIME    (RN_OUT_SIZE * 2) /*! modification time */

#define RN_OUT_TYPE    (RN_OUT_TIME * 2) /*! MIME type of resource */

#define RN_OUT_OWNER   (RN_OUT_TYPE * 2) /*! owner of resource */

#define RN_OUT_PATH    (RN_OUT_OWNER * 2) /*! path to resource */

#define RN_OUT_XML     (RN_OUT_PATH * 2) /*! read a DOM if able */

#define RN_OUT_STRUCT  (RN_OUT_XML * 2) /*! childs of directory */

#define RN_OUT_CONTENT (RN_OUT_STRUCT * 2) /*! read content to buffer */

#define RN_OUT_BASE64  (RN_OUT_CONTENT * 2) /*! read binary content to buffer */


#define RN_OUT_MIN     (RN_OUT_NAME)

#define RN_OUT_MAX     (RN_OUT_NAME | RN_OUT_ATTR | RN_OUT_SIZE | RN_OUT_TIME | RN_OUT_TYPE | RN_OUT_OWNER | RN_OUT_PATH | RN_OUT_XML | RN_OUT_STRUCT | RN_OUT_CONTENT | RN_OUT_BASE64)


#define IS_OUT_NAME(I)    ((I & RN_OUT_NAME) != 0)

#define IS_OUT_ATTR(I)    ((I & RN_OUT_ATTR) != 0)

#define IS_OUT_SIZE(I)    ((I & RN_OUT_SIZE) != 0)

#define IS_OUT_TIME(I)    ((I & RN_OUT_TIME) != 0)

#define IS_OUT_TYPE(I)    ((I & RN_OUT_TYPE) != 0)

#define IS_OUT_OWNER(I)   ((I & RN_OUT_OWNER) != 0)

#define IS_OUT_PATH(I)    ((I & RN_OUT_PATH) != 0)

#define IS_OUT_XML(I)     ((I & RN_OUT_XML) != 0)

#define IS_OUT_STRUCT(I)  ((I & RN_OUT_STRUCT) != 0)

#define IS_OUT_CONTENT(I) ((I & RN_OUT_CONTENT) != 0)



extern resNodePtr 
resNodeFromDOM(xmlNodePtr pndArg, int iArgOptions);

extern xmlNodePtr
resNodeToDOM(resNodePtr prnArg, int iArgOptions);

extern xmlChar *
resNodeToPlain(resNodePtr prnArg, int iArgOptions);

extern xmlChar*
resNodeCSVHeaderStr(int iArgOptions);

extern xmlChar*
resNodeToCSV(resNodePtr prnArg, int iArgOptions);

extern xmlChar*
resNodeToGraphviz(resNodePtr prnArg, int iArgOptions);

extern xmlChar *
resNodeToJSON(resNodePtr prnArg, int iArgOptions);

extern xmlChar*
resNodeDatabaseSchemaStr(int iArgOptions);

extern xmlChar *
resNodeToSQL(resNodePtr prnArg, int iArgOptions);

extern BOOL_T
resNodeContentToDOM(xmlNodePtr pndArg, resNodePtr prnArg);

extern resNodePtr 
resNodeListFromDOM(xmlNodePtr pndArg, int iArgOptions);

extern xmlNodePtr
resNodeListToDOM(resNodePtr prnArg, int iLevelVerboseArg);

extern xmlChar *
resNodeListToSQL(resNodePtr prnArg, int iLevelVerboseArg);

extern xmlChar *
resNodeListToJSON(resNodePtr prnArg, int iLevelVerboseArg);

extern xmlChar *
resNodeListToXmlStr(resNodePtr prnArg, int iLevelVerboseArg);

extern xmlChar *
resNodeListToPlain(resNodePtr prnArg, int iLevelVerboseArg);

extern xmlChar *
resNodeListToGraphviz(resNodePtr prnArg, int iArgOptions);

extern BOOL_T
resNodeListDumpRecursively(FILE *argout, resNodePtr prnArg, int iArgDetails, xmlChar *(*pfArg)(resNodePtr, int));

extern xmlChar*
resNodeListToPlainTree(resNodePtr prnArg, xmlChar* pucArgPrefix, int iArgOptions);

#ifdef TESTCODE
int
resNodeTestFormats(void);
#endif
