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

#include <res_node/res_node.h>

typedef struct _cxpContext cxpContext;
typedef cxpContext *cxpContextPtr;
struct _cxpContext {

  /*! is a dedicated processing context for Threads etc
  */
  time_t system_zeit;

  /*! symbolic working directory (change) */
  resNodePtr prnLocation;

  /*! node to process (for new Thread e.g.) */
  xmlDocPtr pdocContextNode;
  xmlNodePtr pndContextNode;

  /*! NULL pointer terminated array of environment variables */
  char **ppcEnv;
  int iCountEnv;

  /*! pointer to array of program arguments */
  char **ppcArgv;
  int iCountArgv;

  /*! internal level for debug messages */
  int level_set;

  /*! internal flag for readonly mode */
  BOOL_T mode_readonly;

  /*! internal error counter as return code of executable while runtime */
  int iCountError;

  /*! internal runtime */
  int iTimeRun;

//  int tzOffsetToUTC = 9999; /*!> default Offset to UTC in Minutes while runtime */

  /*! internal node for log messages */
  xmlNodePtr pndLog;

  /*! list of search contexts */
  resNodePtr prnSearch;

  /*! Contexts of security

    - root of all path locators

    - a defined 'root directory' (env CXP_ROOT as 'prnRoot')

     - all filesystem operations are checked against this two contexts if they are defined

   */
  resNodePtr prnRoot;

  resNodePtr prnCache; /*! pointer to cache list of resNode's */

  BOOL_T fCaching; /*! flag to switch caching on/off */

  /* collection of conversion descriptors */
  struct {

    struct {
      iconv_t env;    /*! charset environment */
      iconv_t cgi;    /*! charset CGI */
      iconv_t argv;   /*! charset command line arguments */
      iconv_t fs;     /*! charset filesystem informations */
      iconv_t plain;  /*! charset plain text file content */

      iconv_t utf16be;  /*!  */
      iconv_t utf16le;  /*!  */
      iconv_t isolat1;  /*!  */
    } from;

    struct {
      iconv_t env;    /*! charset environment */
      iconv_t cgi;    /*! charset CGI */
      iconv_t argv;   /*! charset command line arguments */
      iconv_t fs;     /*! charset filesystem informations */
      iconv_t plain;  /*! charset plain text file content */

      iconv_t utf16be;  /*!  */
      iconv_t utf16le;  /*!  */
      iconv_t isolat1;  /*!  */
    } to;

  } cd;

#ifdef HAVE_PCRE2
  pcre2_code *re_each; /*! internal regular expression for separation of 'each' values */
#endif

  struct _cxpContext *next; /*! next context in list */
  struct _cxpContext *children; /*! children context in list */
  struct _cxpContext *parent; /*! parent context in list */
};


extern cxpContextPtr
cxpCtxtNew(void);

extern BOOL_T
cxpCtxtFree(cxpContextPtr pccArg);

extern cxpContextPtr
cxpCtxtDup(cxpContextPtr pccArg);

extern int
cxpCtxtOutputSaveFormat(cxpContextPtr pccArg,resNodePtr prnOut,xmlDocPtr pdocArgOutput,xmlChar *pucArgOutput,char *encoding,int format,BOOL_T fAppend);

extern BOOL_T
cxpCtxtSaveFileNode(cxpContextPtr pccArg,xmlNodePtr pndArg,xmlDocPtr pdocArgOutput,xmlChar *pucArgOutput);

extern BOOL_T
cxpCtxtLogAppend(cxpContextPtr pccArg, xmlChar *pucArgLog);

extern xmlNodePtr
cxpCtxtLogGetNode(cxpContextPtr pccArg);

extern BOOL_T
cxpCtxtLogInfo(cxpContextPtr pccArg);

extern void
cxpCtxtLogPutsExit(cxpContextPtr pccArg, int iArg, const char *pcArg);

extern BOOL_T
cxpCtxtLogPrint(cxpContextPtr pccArg, int level, const char *fmt, ...);

extern void
cxpCtxtLogPrintNode(cxpContextPtr pccArg, int level, const char *pucArgLabel, xmlNodePtr pndArg);

extern void
cxpCtxtLogPrintDoc(cxpContextPtr pccArg, int level, const char *pucArg, xmlDocPtr pdocArg);

extern int
cxpCtxtLogSetLevel(cxpContextPtr pccArg, int level);

extern int
cxpCtxtLogSetLevelStr(cxpContextPtr pccArg, xmlChar *pucArg);

extern int
cxpCtxtLogGetLevel(cxpContextPtr pccArg);

extern BOOL_T
cxpCtxtLogSetFile(cxpContextPtr pccArg, resNodePtr prnArg);

extern BOOL_T
cxpCtxtLogCloseFile(cxpContextPtr pccArg);

extern int
cxpCtxtGetRunningTime(cxpContextPtr pccArg);

extern BOOL_T
cxpCtxtSetReadonly(cxpContextPtr pccArg, BOOL_T mode);

extern BOOL_T
cxpCtxtIsReadonly(cxpContextPtr pccArg);

extern BOOL_T
cxpContextRunmodeIsCgi(cxpContextPtr pccArg);

extern int
cxpCtxtGetExitCode(cxpContextPtr pccArg);

extern int
cxpCtxtIncrExitCode(cxpContextPtr pccArg, int iArg);


extern BOOL_T
cxpCtxtSearchSet(cxpContextPtr pccArg, resNodePtr prnArg);

extern resNodePtr
cxpCtxtSearchGet(cxpContextPtr pccArg);

extern xmlChar *
cxpCtxtLocationGetStr(cxpContextPtr pccArg);

extern resNodePtr
cxpCtxtLocationGet(cxpContextPtr pccArg);

extern BOOL_T
cxpCtxtLocationSet(cxpContextPtr pccArg,resNodePtr prnArg);


extern cxpContextPtr
cxpCtxtFromAttr(cxpContextPtr pccArg, xmlNodePtr pndArg);

extern resNodePtr
cxpCtxtLocationDup(cxpContextPtr pccArg);


extern BOOL_T
cxpCtxtProcessSetNodeCopy(cxpContextPtr pccArg, xmlNodePtr pndArg);

extern xmlNodePtr
cxpCtxtProcessGetNode(cxpContextPtr pccArg);

extern xmlChar *
cxpCtxtProcessDump(cxpContextPtr pccArg);


extern BOOL_T
cxpCtxtLocationUpdate(cxpContextPtr pccArg, xmlChar *pucArg);

extern BOOL_T
cxpCtxtRootSet(cxpContextPtr pccArg, resNodePtr prnArg);

extern resNodePtr
cxpCtxtRootGet(cxpContextPtr pccArg);

extern BOOL_T
cxpCtxtAccessIsPermitted(cxpContextPtr pccArg, resNodePtr prnArg);


extern int
cxpContextProcess(cxpContextPtr pccArg);


extern cxpContextPtr
cxpCtxtGetNext(cxpContextPtr pccArg);

extern cxpContextPtr
cxpCtxtGetTop(cxpContextPtr pccArg);

extern cxpContextPtr
cxpCtxtGetParent(cxpContextPtr pccArg);

extern cxpContextPtr
cxpCtxtGetChild(cxpContextPtr pccArg);

extern BOOL_T
cxpCtxtUnlinkChild(cxpContextPtr pccArgList, cxpContextPtr pccArg);

extern cxpContextPtr
cxpCtxtAddChild(cxpContextPtr pccArgParent, cxpContextPtr pccArgChild);

extern xmlChar *
cxpCtxtGetHostValueNamed(cxpContextPtr pccArgParent, const xmlChar *pucName);


#ifdef TESTCODE
extern int
cxpContextTest(cxpContextPtr pccArg);
#endif

#include <cxp/cxp_context_cli.h>

#ifdef TESTCODE
extern int
cxpCtxtTest(cxpContextPtr pccArg);
#endif
