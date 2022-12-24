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

#define NAME_DIR BAD_CAST "dir"

#define NAME_FILE BAD_CAST "file"

#define NAME_SYMLINK BAD_CAST "link"

#define IS_NODE_DIR(NODE) (IS_NODE(NODE,NAME_DIR))

#define IS_NODE_FILE(NODE) (IS_NODE(NODE,NAME_FILE))

#if (!defined(_WIN32)) && (!defined(WIN32)) && (!defined(__APPLE__))
#ifndef __USE_FILE_OFFSET64
#define __USE_FILE_OFFSET64
#endif
#ifndef __USE_LARGEFILE64
#define __USE_LARGEFILE64
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
#ifndef _FILE_OFFSET_BIT
#define _FILE_OFFSET_BIT 64
#endif
#endif

#ifndef FS_CONTEXT_H_INCLUDED
#define FS_CONTEXT_H_INCLUDED

#ifdef _MSC_VER
#include <io.h>
#elif defined WIN32
#include <io.h>
#include <dirent.h>
#else
#include <sys/dir.h>
#include <dirent.h>
#endif

#ifdef HAVE_PCRE2
#include <pcre2.h>
#endif

#ifdef HAVE_LIBCURL
#include <curl/curl.h>
#endif

#include <res_node/res_path.h>
#include <res_node/res_mime.h>

/*! option bits for duplicating of resource nodes
 */
#define RN_DUP_THIS    (0)

#define RN_DUP_CONTENT (1)

#define RN_DUP_CHILDS  (2)

#define RN_DUP_NEXT    (4)

#define RN_DUP_EXIST   (8)

#define RN_DUP_READ    (16 | RN_DUP_EXIST)

#define RN_DUP_WRITE   (32 | RN_DUP_EXIST)

 /*! option bits for parsing of resource nodes
 */
#define RN_INFO_MIN     (0)

#define RN_INFO_META    (1)

#define RN_INFO_INFO    (2)

#define RN_INFO_XML     (4)

#define RN_INFO_PIE     (8)

#define RN_INFO_STRUCT  (16)

#define RN_INFO_CONTENT (32)

/*
"PICTURE.JPG.TXT"
*/
#define RN_INFO_COMMENT (64)

/*
".index.txt"
".index.pie"
".index.json"
*/
#define RN_INFO_INDEX   (128)

/*
re-build full list of contextxts
*/
#define RN_INFO_LIST    (256)

#define RN_INFO_MAX     (RN_INFO_META | RN_INFO_INFO | RN_INFO_XML | RN_INFO_PIE | RN_INFO_STRUCT | RN_INFO_CONTENT | RN_INFO_COMMENT | RN_INFO_LIST)


typedef enum {
  rn_type_undef,
  rn_type_stdout,
  rn_type_stderr,
  rn_type_stdin,
  rn_type_memory,
  rn_type_root,
  rn_type_dir,
  rn_type_file,
  rn_type_file_compressed,
  rn_type_document_plain,
  rn_type_document_dom,
  rn_type_document_json,
  rn_type_image,
  rn_type_index,
  rn_type_archive,
  rn_type_dir_in_archive,
  rn_type_file_in_archive,
  rn_type_database,
  rn_type_file_in_database,
  rn_type_symlink,
  rn_type_url_http,
  rn_type_url_ftp,
  rn_type_url
} RN_TYPE; /*! types of resources */

typedef enum {
  mode_undef,
  mode_read,			/* was opened for reading */
  mode_write,			/*                writing */
  mode_append			/*                appending */
} RN_MODE; /*!  */

typedef enum {
  rn_access_undef,
  rn_access_file,			/* was opened as FILE */
  rn_access_directory,		/*               directory */
  rn_access_std,			/*               stdio|stderr|stderr */
#ifdef HAVE_LIBCURL
  rn_access_curl,			/*               CURL */
#endif
  rn_access_xmlio,			/*               libXml */
  rn_access_sqlite,			/*               SQLite */
  rn_access_image,			/*               image */
  rn_access_audio,			/*               audio */
  rn_access_archive			/*               archive */
} RN_ACCESS; /*! indicates the way it was opened (type of IO handle) */

typedef enum {
  rn_error_none,
  rn_error_undef,
  rn_error_argv,
  rn_error_memory,
  rn_error_path,
  rn_error_name,
  rn_error_owner,
  rn_error_access,
  rn_error_copy,
  rn_error_write,
  rn_error_busy,
  rn_error_parse,
  rn_error_archive,
  rn_error_encoding,
  rn_error_max_path,
  rn_error_find,
  rn_error_mkdir,
  rn_error_unlink,
  rn_error_link,
  rn_error_open,
  rn_error_read,
  rn_error_close,
  rn_error_xml,
  rn_error_ftp,
  rn_error_http,
  rn_error_sql,
  rn_error_stat
} RN_ERROR; /*! error types */


typedef struct _resNode resNode;
typedef resNode *resNodePtr;
struct _resNode {
  xmlChar *pucNameBase;		/*! UTF-8 encoded basename of entry name */
  char    *pcNameBaseNative;	/*! native filesystem encoded name, eventually linked to 'pucNameNormalized' (Caution when xmlFree()!!) */

  xmlChar *pucNameNormalized;	/*! UTF-8 encoded normalized entry name */
  char    *pcNameNormalizedNative;	/*! native filesystem encoded name, eventually linked to 'pucNameNormalized' (Caution when xmlFree()!!) */

  xmlChar *pucNameBaseDir;	/*! UTF-8 encoded base directory of entry name */

  xmlChar *pucExtension;        /*! UTF-8 encoded entry extension */

  xmlChar *pucNameShort;	/*! UTF-8 encoded short path of entry name */

  xmlChar *pucQuery;	        /*! UTF-8 encoded entry query */

  xmlChar *pucURI;        	/*! UTF-8 encoded entry URI notation */

  xmlChar *pucOwner;		/*! UTF-8 encoded owner of entry name */

  xmlChar *pucObject;		/*! UTF-8 encoded object name of entry */

  xmlChar *pucAlias;		/*! UTF-8 encoded alias of entry */

  xmlChar **ppucProp;		/*! array of pointer pairs to context properties (author, title, resolution etc) */

  /*! flags 
   */
  BOOL_T fStat;			/*! TRUE if this context was stat'd already */
  BOOL_T fExist;
  BOOL_T fRead;
  BOOL_T fWrite;
  BOOL_T fExecute;
  BOOL_T fHidden;
  
  BOOL_T fRecursive;

  RN_ERROR eError;		/*! error code */
  xmlChar *pucMsg;		/*! UTF-8 encoded last error string of entry */

  RN_MODE eMode;		/*! IO mode of this resource */
  RN_TYPE eType;		/*! type of this resource */

  RN_MIME_TYPE eMimeType;	/*! numerical mime type of this resource */

  size_t liSize;			/*! size _info_ in Bytes (meta datum!) */
  size_t liRecursiveSize;

  time_t tMtime;            /*! modification time value of context */
  xmlChar *pucMtime;        /*! UTF-8 encoded entry mtime string */

  RN_ACCESS eAccess;        /*! access method for this context (default: direct) */

  /*! generic handle, distinction via RN_TYPE

  rn_type_undef			| NULL
  rn_type_stdout		| 
  rn_type_stderr		| 
  rn_type_stdin			| 
  rn_type_memory		| NULL
  rn_type_dir			| DIR * | HANDLE *
  rn_type_file			| FILE *
  fs_document			| 
  rn_type_image			| 
  rn_type_archive		| struct archive *
  rn_type_dir_in_archive	| struct archive_entry *
  rn_type_file_in_archive	| struct archive_entry *
  rn_type_database		| sqlite3 *
  rn_type_file_in_database	| 
  rn_type_symlink		| 
  rn_type_url_http		| 
  rn_type_url_ftp		| 
  rn_type_url			| 
   */
  void *handleIO;

#ifdef HAVE_LIBCURL
  CURLU *curlURL;
#endif
  
  /*! generic pointer to read content

  rn_type_undef			| NULL
  rn_type_stdout		| 
  rn_type_stderr		| 
  rn_type_stdin			| 
  rn_type_memory		| NULL
  rn_type_dir			| 
  rn_type_file			| 
  fs_document			| 
  rn_type_image			| ExifData *
  rn_type_archive		| 
  rn_type_dir_in_archive	| 
  rn_type_file_in_archive	| 
  rn_type_database		| 
  rn_type_file_in_database	| 
  rn_type_symlink		| 
  rn_type_url_http		| 
  rn_type_url_ftp		| 
  rn_type_url			| 
   */
  void *pContent;
  size_t liSizeContent;		/*! size of _read_ content at pContent */
  size_t liSizeBlock;  		/*! size of blocks while reading content */

  xmlDocPtr pdocContent;	/*! pointer to read DOM */

  int iCountUse;  		/*! usage counter for this context (caching) */

  struct _resNode *parent;   /*! parent context of list */
  struct _resNode *children; /*! children context in list */
  struct _resNode *last;     /*! last children context in list */
  struct _resNode *next;     /*! next context in list */
  struct _resNode *prev;     /*! next context in list */
};

extern resNodePtr
resNodeNew(void);

extern BOOL_T
resNodeReset(resNodePtr prnArg, xmlChar *pucArgPath);

extern BOOL_T
resNodeUpdate(resNodePtr prnArg, int iArgOptions, const pcre2_code *re_match, const pcre2_code *re_grep);

extern resNodePtr
resNodeRootNew(resNodePtr prnRoot, xmlChar *pucArgPath);

extern resNodePtr
resNodeAddChildNew(resNodePtr prnArg, xmlChar *pucArgPath);

extern resNodePtr
resNodeInMemoryNew(void);

extern resNodePtr
resNodeDup(resNodePtr prnArg, int iArgOptions);

extern resNodePtr
resNodeStrNew(xmlChar *pucArgPath);

#ifdef HAVE_LIBCURL

resNodePtr
resNodeCurlNew(xmlChar *pucArgPath);

#endif

extern resNodePtr
resNodeDirNew(xmlChar *pucArgPath);

extern resNodePtr
resNodeSplitStrNew(xmlChar *pucArgPath);

extern resNodePtr
resNodeSplitLineBufferNew(xmlChar* pucArg);

extern resNodePtr
resNodeInsert(resNodePtr prnArgTree, resNodePtr prnArg);

extern resNodePtr
resNodeInsertStrNew(resNodePtr prnArg, xmlChar* pucArgPath);

extern resNodePtr
resNodeConcatNew(xmlChar *pucArgPath, xmlChar *pucArgPathB);

extern resNodePtr
resNodeFromNodeNew(resNodePtr prnArg, xmlChar *pucArgPath);

extern resNodePtr
resNodeCwdNew(void);

extern resNodePtr
resNodeAliasNew(xmlChar *pucArg);

extern int
resNodeGetLength(resNodePtr prnArg);

extern BOOL_T
resNodeAddSibling(resNodePtr prnArgList, resNodePtr prnArg);

extern resNodePtr
resNodeGetNext(resNodePtr prnArg);

extern BOOL_T
resNodeAddChild(resNodePtr prnArgParent, resNodePtr prnArgChild);

extern resNodePtr
resNodeGetChild(resNodePtr prnArg);

extern resNodePtr
resNodeGetLast(resNodePtr prnArg);

extern resNodePtr
resNodeGetParent(resNodePtr prnArg);

extern resNodePtr
resNodeGetRoot(resNodePtr prnArg);

extern resNodePtr
resNodeGetAncestorArchive(resNodePtr prnArg);

extern xmlChar*
resNodeGetAncestorPathStr(resNodePtr prnArg);

extern resNodePtr
resNodeGetLastDescendant(resNodePtr prnArgList);

extern int
resNodeGetChildCount(resNodePtr prnArgList, RN_TYPE eArgType);

extern int
resNodeGetSiblingsCount(resNodePtr prnArgList, RN_TYPE eArgType);

extern int
resNodeGetCountDescendants(resNodePtr prnArgList);

extern xmlChar *
resNodeGetNameAlias(resNodePtr prnArg);

extern xmlChar*
resNodeGetNameShort(resNodePtr prnArg);

extern resNodePtr
resNodeResolveLinkChildNew(resNodePtr prnArg);

extern resNodePtr
resNodeCommentNew(resNodePtr prnArg);

extern BOOL_T
resNodeConcat(resNodePtr prnArg, xmlChar *pucArgPath);

extern BOOL_T
resNodeSetNameAlias(resNodePtr prnArg, xmlChar *pucArg);

extern BOOL_T
resNodeSetNameBase(resNodePtr prnArg, xmlChar *pucArgPath);

extern BOOL_T
resNodeSetNameBaseDir(resNodePtr prnArg, xmlChar *pucArgPath);

extern BOOL_T
resNodeSetNameShort(resNodePtr prnArg, xmlChar* pucArgShort);

extern void
resNodeFree(resNodePtr prnArg);

extern BOOL_T
resNodeIsStd(resNodePtr prnArg);

extern BOOL_T
resNodeIsDir(resNodePtr prnArg);

extern BOOL_T
resNodeIsRecursive(resNodePtr prnArg);

extern BOOL_T
resNodeIsURL(resNodePtr prnArg);

extern BOOL_T
resNodeIsMemory(resNodePtr prnArg);

extern BOOL_T
resNodeIsFile(resNodePtr prnArg);

extern BOOL_T
resNodeIsLargeFile(resNodePtr prnArg);

extern BOOL_T
resNodeIsLink(resNodePtr prnArg);

extern BOOL_T
resNodeIsSqlite(resNodePtr prnArg);

extern BOOL_T
resNodeIsDatabase(resNodePtr prnArg);

extern BOOL_T
resNodeIsArchive(resNodePtr prnArg);

extern BOOL_T
resNodeIsFileInArchive(resNodePtr prnArg);

extern BOOL_T
resNodeIsDirInArchive(resNodePtr prnArg);

extern BOOL_T
resNodeIsExist(resNodePtr prnArg);

extern BOOL_T
resNodeIsReadable(resNodePtr prnArg);

extern BOOL_T
resNodeIsMatching(resNodePtr prnArg, const pcre2_code *re_match);

extern BOOL_T
resNodeIsExecuteable(resNodePtr prnArg);

extern BOOL_T
resNodeIsWriteable(resNodePtr prnArg);

extern BOOL_T
resNodeIsCreateable(resNodePtr prnArg);

extern BOOL_T
resNodeIsHidden(resNodePtr prnArg);

extern RN_ERROR
resNodeSetError(resNodePtr prnArg, RN_ERROR eArg, const char *fmt, ...);

extern BOOL_T
resNodeResetError(resNodePtr prnArg);

extern RN_ERROR
resNodeGetError(resNodePtr prnArg);

extern xmlChar *
resNodeGetErrorMsg(resNodePtr prnArg);

extern BOOL_T
resNodeIsError(resNodePtr prnArg);

extern BOOL_T
resNodeReadStatus(resNodePtr prnArg);

extern BOOL_T
resNodeSetOwner(resNodePtr prnArg);

extern xmlChar *
resNodeGetURI(resNodePtr prnArg);

extern xmlChar *
resNodeGetNameBase(resNodePtr prnArg);

extern xmlChar *
resNodeGetNameBaseDir(resNodePtr prnArg);

extern xmlChar *
resNodeGetNameLink(resNodePtr prnArg);

extern xmlChar *
resNodeGetNameRelative(resNodePtr prnArgBase, resNodePtr prnArg);

extern xmlChar *
resNodeGetExtension(resNodePtr prnArg);

extern xmlChar *
resNodeGetOwner(resNodePtr prnArg);

extern xmlChar *
resNodeGetNameObject(resNodePtr prnArg);

extern void
resNodeResetMimeType(resNodePtr prnArg);

extern void
resNodeSetMimeType(resNodePtr prnArg, RN_MIME_TYPE eArgMimeType);

extern RN_MIME_TYPE
resNodeGetMimeType(resNodePtr prnArg);

extern const char *
resNodeGetMimeTypeStr(resNodePtr prnArg);

extern void
resNodeSetType(resNodePtr prnArg, RN_TYPE eArgType);

extern RN_TYPE
resNodeGetType(resNodePtr prnArg);

extern xmlChar *
resNodeGetTypeStr(resNodePtr prnArg);

extern xmlChar *
resNodeGetNameNormalized(resNodePtr prnArg);

extern char *
resNodeGetNameNormalizedNative(resNodePtr prnArg);

extern BOOL_T
resNodeSetCacheFlag(resNodePtr prnArg, BOOL_T fArg);

extern struct tm *
resNodeGetLocalTime(resNodePtr prnArg);

extern time_t
resNodeGetMtime(resNodePtr prnArg);

extern xmlChar *
resNodeGetMtimeStr(resNodePtr prnArg);

extern long
resNodeGetMtimeDiff(resNodePtr prnArg);

extern size_t
resNodeGetSize(resNodePtr prnArg);

extern size_t
resNodeSetSize(resNodePtr prnArg, size_t iArg);

extern size_t
resNodeGetRecursiveSize(resNodePtr prnArg);

extern size_t
resNodeIncrRecursiveSize(resNodePtr prnArg, size_t iArg);

extern BOOL_T
resNodeSetRecursion(resNodePtr prnArg, BOOL_T fArgFlag);

extern BOOL_T
resNodeSetWrite(resNodePtr prnArg, BOOL_T fArgFlag);

extern BOOL_T
resNodeSetToParent(resNodePtr prnArg);

extern BOOL_T
resNodeStepInTo(resNodePtr prnArg);

extern BOOL_T
resNodeDirAppendEntries(resNodePtr prnArgDir, const pcre2_code *re_match);

extern xmlNodePtr
resNodeToDOM(resNodePtr prnArg, int iArgOptions);

extern xmlChar *
resNodeToPlain(resNodePtr prnArg, int iArgOptions);

extern xmlChar*
resNodeToCSV(resNodePtr prnArg, int iArgOptions);

extern xmlChar*
resNodeToGraphviz(resNodePtr prnArg, int iArgOptions);

extern xmlChar *
resNodeToJSON(resNodePtr prnArg, int iArgOptions);

extern xmlChar *
resNodeToSQL(resNodePtr prnArg, int iArgOptions);

extern xmlChar *
resNodeSetProp(resNodePtr prnArg, xmlChar *pucArgKey, xmlChar *pucArgValue);

extern xmlChar *
resNodeGetProp(resNodePtr prnArg, xmlChar *pucArgKey);

extern BOOL_T
resNodeContentToDOM(xmlNodePtr pndArg, resNodePtr prnArg);

extern xmlChar*
resNodeDatabaseSchemaStr(void);

#ifdef TESTCODE
extern int
resNodeTest(void);

extern int
resNodeTestProp(void);
#endif

#include <res_node/res_node_list.h>

#endif

