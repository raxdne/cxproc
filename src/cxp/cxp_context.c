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

/*!\todo write errormessages (+ logmessages?) into the result DOM (like meta element)
    multiple result DOMs possible (only in CGI mode not)
    also plain and xhtml results
*/

/* 
 */
#include <libxml/xmlversion.h>
#include <libxml/HTMLtree.h>
#include <libxml/parser.h>
#include <libxml/uri.h>

#include <libxslt/xslt.h>
#include <libxslt/xsltutils.h>
#include <libxslt/transform.h>
#include <libxslt/variables.h>

#ifdef LIBXML_THREAD_ENABLED
#include <libxml/globals.h>
#include <libxml/threads.h>
#ifdef HAVE_LIBPTHREAD
#include <pthread.h>
#elif defined(WITH_THREAD)
#endif
#endif

/*
 */
#include "basics.h"
#include "utils.h"
#include <res_node/res_node_ops.h>
#include <cxp/cxp.h>
#include "plain_text.h"
#include <cxp/cxp_dir.h>
#include "dom.h"

#define NAME_ROOT (BAD_CAST "CXP_ROOT")

/*! creates a new empty cxproc Context

\return pointer to new allocated context
*/
cxpContextPtr
cxpCtxtNew(void)
{
  cxpContextPtr pccResult = NULL;

#ifdef DEBUG
  //cxpCtxtLogPrint(2, "cxpCtxtNew(void)");
#endif

  pccResult = (cxpContextPtr)xmlMalloc(sizeof(cxpContext));
  if (pccResult) {
    /*! set */
    memset(pccResult, 0, sizeof(cxpContext));

    pccResult->level_set = -1;
    cxpCtxtCacheEnable(pccResult,FALSE);
    time(&(pccResult->system_zeit));
  }
  else {
    //cxpCtxtLogPrint(pccArg, 0, "Out of memory at cxpCtxtDup()");
  }

  return pccResult;
} /* end of cxpCtxtNew() */


/*! cxp Ctxt Cli Dup

\param pccArg -- pointer to context to duplicate
\return a copy of pccArg or an freshly allocated empty context if pccArg is NULL
*/
cxpContextPtr
cxpCtxtDup(cxpContextPtr pccArg)
{
  cxpContextPtr pccResult = NULL;

  /*! if pccArg is NULL create a new empty context */

  pccResult = cxpCtxtNew();
  if (pccResult) {
    if (pccArg) {
      cxpCtxtRootSet(pccResult,cxpCtxtRootGet(pccArg)); /*! dup list of root context */
      cxpCtxtLocationSet(pccResult,cxpCtxtLocationGet(pccArg));
      cxpCtxtSearchSet(pccResult,cxpCtxtSearchGet(pccArg)); /*! dup list of search contexts */
      cxpCtxtLogSetLevel(pccResult,cxpCtxtLogGetLevel(pccArg));
      cxpCtxtEncSetDefaults(pccResult);
      cxpCtxtEnvDup(pccResult,pccArg->ppcEnv);
      cxpCtxtCacheEnable(pccResult,pccArg->fCaching);
    }
  }

  return pccResult;
} /* end of cxpCtxtDup() */


/*! wrapper fuction for xmlSaveFormatFileEnc() with filename decoding
*/
int
cxpCtxtOutputSaveFormat(cxpContextPtr pccArg,resNodePtr prnOut,xmlDocPtr pdocArgOutput,xmlChar *pucArgOutput,char *encoding,int format,BOOL_T fAppend)
{
  int iResult = -1;

  if (prnOut) {
    /*!\todo use encoding settings from DOM (s. pdocSave->encoding ) */

    if (cxpCtxtIsReadonly(pccArg) && resNodeIsStd(prnOut)==FALSE) {
      cxpCtxtLogPrint(pccArg,1,"Cant save '%s' in readonly mode",resNodeGetNameNormalized(prnOut));
    }
    else if (cxpCtxtAccessIsPermitted(pccArg,prnOut) == FALSE) {
      cxpCtxtLogPrint(pccArg,1,"No access permitted");
    }
    else if (resNodeReadStatus(prnOut) == TRUE && resNodeIsWriteable(prnOut) == FALSE) {
      cxpCtxtLogPrint(pccArg,1,"Exists but no write permission");
    }
    else if (pdocArgOutput) {
      /*
      there is a doc pointer
      */
      if (domDocIsHtml(pdocArgOutput)) {
	/* workaround because htmlSaveFileFormat() skips the DTD nodes */
	/*!\todo HTML Doctype fputs("<!DOCTYPE html>\n", stdout); problems with XHTTP request CgiPiejQEditor.cxp */
#ifdef HAVE_CGI
	if (resNodeIsStd(prnOut)) {
	  fflush(stdout);
	}
#else
#endif
	iResult = htmlSaveFileFormat(resNodeGetNameNormalizedNative(prnOut),pdocArgOutput,encoding,format);
      }
      else if (encoding) {
	iResult = xmlSaveFormatFileEnc(resNodeGetNameNormalizedNative(prnOut),pdocArgOutput,encoding,format);
      }
      else {
	iResult = xmlSaveFormatFile(resNodeGetNameNormalizedNative(prnOut),pdocArgOutput,format);
      }
    }
    else if (pucArgOutput) {
      size_t iLenOutput;

      iLenOutput = xmlStrlen(pucArgOutput);
      if (iLenOutput > 0) {
	if (resNodeOpen(prnOut,(char *)(fAppend ? "w+" : "w"))) {
	  if (encoding != NULL && strlen(encoding) > 0 && xmlStrcasecmp(BAD_CAST encoding,BAD_CAST"UTF-8") != 0) {
	    iconv_t cdOutput;

	    cdOutput = iconv_open(encoding,"UTF-8");
	    if (cdOutput == (iconv_t)-1) {
	      /* Something went wrong.  */
	      cxpCtxtLogPrint(pccArg,1,"conversion to '%s' to UTF-8 not possible",encoding);
	      /* fallback */
	      fputs((char *)pucArgOutput,(FILE *)resNodeGetHandleIO(prnOut));
	    }
	    else {
	      void *pContent;
	      char *pchIn;
	      size_t int_out;
	      size_t int_outleft;
	      size_t int_in;

	      cxpCtxtLogPrint(pccArg,2,"Output conversion '%s' to '%s'","UTF-8",encoding);

	      int_in = iLenOutput;
	      int_outleft = int_out = int_in * 2; /*!\bug DANGEROUS!!! */

	      pchIn  = (char *)pucArgOutput;
	      pContent = xmlMalloc(int_out * sizeof(char));
	      if (pContent) {
		char *pchOut;
		size_t nconv;

		pchOut = (char *)pContent;
		nconv = iconv(cdOutput,(char **)&pchIn,&int_in,&pchOut,&int_outleft);
		if (nconv == (size_t)-1) {
		  //		      cxpCtxtEncError(pccArg,errno,nconv);
		}
		/*!\bug UTF BOM required? */
		fwrite(pContent,1,(int_out - int_outleft),(FILE *)resNodeGetHandleIO(prnOut));
		xmlFree(pContent);
	      }
	      else {
		cxpCtxtLogPrint(pccArg,1,"Not enough memory for conversion (%i Byte)",int_out);
		/* fallback */
		fputs((const char *)pucArgOutput,(FILE *)resNodeGetHandleIO(prnOut));
	      }
	      iconv_close(cdOutput);
	    }
	  }
	  else {
	    cxpCtxtLogPrint(pccArg,3,"No conversion necessary (%i Byte)",iLenOutput);
	    /*  */
	    fputs((char *)pucArgOutput,(FILE *)resNodeGetHandleIO(prnOut));
	  }
	  resNodeClose(prnOut);
	  iResult = xmlStrlen(pucArgOutput);
	}
      }
    }
  }
  return iResult;
}
/* end of cxpCtxtOutputSaveFormat() */


/*! Save a copy of the result DOM or string to a file. Register the
copy for caching and leave pdocResult untouched.

calls cxpCtxtOutputSaveFormat() with according arguments

\param pndArg contains all attributes for saving

*/
BOOL_T
cxpCtxtSaveFileNode(cxpContextPtr pccArg,xmlNodePtr pndArg,xmlDocPtr pdocArgOutput,xmlChar *pucArgOutput)
{
  xmlChar *pucAttrMode;
  xmlChar *pucAttrEncoding;
  xmlChar *pucAttrResponse;
  xmlChar *pucAttrName;
  xmlChar *pucAttrNameCached;
  BOOL_T fAppend = FALSE;
  resNodePtr prnOutput = NULL;
  int iContentLength = -1;

  if (pccArg == NULL || pndArg == NULL) {
    return FALSE;
  }

  if (pdocArgOutput == NULL && pucArgOutput == NULL) {
    cxpCtxtLogPrint(pccArg,3,"Empty result");
    return FALSE;
  }

  if (domGetPropFlag(pndArg,BAD_CAST "dump",FALSE)) {
    if (pdocArgOutput) {
      cxpCtxtLogPrintDoc(pccArg,1,NULL,pdocArgOutput);
    }
    else if (pucArgOutput) {
      cxpCtxtLogPrint(pccArg,1,NULL,pucArgOutput);
    }
  }

  /*! cache DOM if required */
  pucAttrNameCached = domGetPropValuePtr(pndArg,BAD_CAST "cacheas");
  if (STR_IS_NOT_EMPTY(pucAttrNameCached)) {
    /* symbol is ID, ready for later transformations */
    if (pdocArgOutput) {
      cxpCtxtCacheAppendDoc(pccArg,pdocArgOutput,pucAttrNameCached);
    }
    else if (pucArgOutput) {
      cxpCtxtCacheAppendBuffer(pccArg,pucArgOutput,pucAttrNameCached);
    }
  }
  else {
    pucAttrNameCached = NULL;
  }

  pucAttrName = domGetPropValuePtr(pndArg,BAD_CAST "name");
  if (STR_IS_NOT_EMPTY(pucAttrName)) {
    if (resPathIsStd(pucAttrName)) {
      /* stdout */
      prnOutput = resNodeDirNew(pucAttrName);
    }
    else {
      prnOutput = resNodeFromNodeNew(cxpCtxtLocationGet(pccArg),pucAttrName);

      if (domGetPropFlag(pndArg,BAD_CAST "cache",FALSE)) {
	/* filename is ID, ready for later transformations */
	if (pdocArgOutput) {
	  cxpCtxtCacheAppendDoc(pccArg,pdocArgOutput,resNodeGetNameNormalized(prnOutput));
	}
	else if (pucArgOutput) {
	  cxpCtxtCacheAppendBuffer(pccArg,pucArgOutput,resNodeGetNameNormalized(prnOutput));
	}
      }
    }
  }
  else {
    pucAttrName = NULL;
  }

  pucAttrResponse = domGetPropValuePtr(pndArg,BAD_CAST "response");

  pucAttrEncoding = domGetPropValuePtr(pndArg,BAD_CAST "encoding");

  fAppend = (pucAttrMode = domGetPropValuePtr(pndArg,BAD_CAST "mode")) && xmlStrcasecmp(pucAttrMode,BAD_CAST "append")==0;

  /*!\todo respect the other \@mode values (s. DTD) */

#ifdef HAVE_CGI
  resNodeReadStatus(prnOutput);
  if (resNodeGetType(prnOutput) == rn_type_file) {
    if (cxpCtxtAccessIsPermitted(pccArg,prnOutput) == FALSE) {
      printf("Status: 503 No access permitted\r\n"
	"Content-Type: text/plain\r\n\r\n"
	"Cxproc access error %s\r\n",resNodeGetNameNormalized(prnOutput));
      cxpCtxtLogPrint(pccArg,1,"File access error");
      return FALSE;
    }
    else if (resNodeIsWriteable(prnOutput) == FALSE) {
      printf("Status: 503 No write permission\r\n"
	"Content-Type: text/plain\r\n\r\n"
	"Cxproc write permission error %s\r\n",resNodeGetNameNormalized(prnOutput));
      cxpCtxtLogPrint(pccArg,1,"File write permission error");
      return FALSE;
    }
  }
  else if (resNodeGetType(prnOutput) == rn_type_stdout) {
    /* s. RFC 1806 */
    xmlChar *pucAttrType = domGetPropValuePtr(pndArg,BAD_CAST "type");
    xmlChar *pucAttrDisposition = domGetPropValuePtr(pndArg,BAD_CAST "disposition");
    printf("Content-Type: ");
    if (pucAttrType && xmlStrlen(pucAttrType)>5) {
      printf("%s;",pucAttrType);
    }
    else if (IS_NODE_XML(pndArg)) {
      printf(domDocIsHtml(pdocArgOutput) ? "text/html;" : "text/xml;");
    }
    else {
      printf("text/plain;");
    }
    printf(" charset=UTF-8\n");

    if (pucAttrDisposition && xmlStrlen(pucAttrDisposition)>5) {
      printf("Content-Disposition: %s\n",pucAttrDisposition);
    }
    printf("Content-Description: Dynamic cxproc content\n\n");
  }
  fflush(stdout); /*! because problems with VC++ (reverse order in stdout) */
#else
#endif

  if (prnOutput) { /* Save the result only if a \@name is given. */
    if (pdocArgOutput) {
      /* this is DOM content */
      iContentLength = cxpCtxtOutputSaveFormat(pccArg,prnOutput,pdocArgOutput,NULL,(char *)((pucAttrEncoding != NULL) ? pucAttrEncoding : BAD_CAST "UTF-8"),1,FALSE);
    }
    else if (pucArgOutput) {
      /* this is string content only */
      iContentLength = cxpCtxtOutputSaveFormat(pccArg,prnOutput,NULL,pucArgOutput,(char *)((pucAttrEncoding != NULL) ? pucAttrEncoding : BAD_CAST "UTF-8"),0,fAppend);
    }
  }
  else {
#ifdef DEBUG
    cxpCtxtLogPrint(pccArg,4,"Anonymous result");
#endif
  }

#ifdef HAVE_CGI
  if (resNodeGetType(prnOutput) == rn_type_file) {
    if (iContentLength < 0) {
      cxpCtxtLogPrint(pccArg,1,"Save error '%s'",resNodeGetNameNormalized(prnOutput));
      printf("Status: 503 Save error\r\n"
	"Content-Type: text/plain\r\n\r\n"
	"Cxproc save error %s\r\n",
	resNodeGetNameNormalized(prnOutput));
    }
    else {
      cxpCtxtLogPrint(pccArg,2,"Saved '%i' Bytes to '%s' successfully",iContentLength,resNodeGetNameNormalized(prnOutput));
      fflush(stdout); /*! because problems with VC++ (reverse order in stdout) */
      if (STR_IS_NOT_EMPTY(pucAttrResponse)) {
	printf("Status: 200 OK\r\n"
	  "Content-Type: text/plain;\r\n\r\n"
	  "%s",
	  pucAttrResponse);
      }
    }
  }
#else
#endif

  resNodeFree(prnOutput);

  return TRUE;
}
/* end of cxpCtxtSaveFileNode() */


/*! cxp Ctxt Log Append

\param pccArg -- pointer to context
\param *pucArgLog -- pointer to log string
\return TRUE if , FALSE in case of
*/
BOOL_T
cxpCtxtLogAppend(cxpContextPtr pccArg, xmlChar *pucArgLog)
{
  BOOL_T fResult = FALSE;

  /*! create a log node if there is not one */
  if (pccArg->pndLog == NULL) {
    pccArg->pndLog = xmlNewNode(NULL, BAD_CAST"log");
  }

  /*! append log string to log node */
  if (pccArg->pndLog != NULL && STR_IS_NOT_EMPTY(pucArgLog)) {
    xmlNewTextChild(pccArg->pndLog, NULL, BAD_CAST"p", BAD_CAST pucArgLog);
    fResult = TRUE;
  }

  return fResult;
} /* end of cxpCtxtLogAppend() */


/*! cxp Ctxt Log GetNode

\param pccArg -- pointer to context
\return pointer to the internal log tree
*/
xmlNodePtr
cxpCtxtLogGetNode(cxpContextPtr pccArg)
{
  xmlNodePtr pndResult = NULL;

  if (pccArg) {
    pndResult = pccArg->pndLog;
  }
  return pndResult;
} /* end of cxpCtxtLogGetNode() */


/*! puts 'pucArg' and exits with code 'iArg'

\param pccArg -- pointer to context
*/
void
cxpCtxtLogPutsExit(cxpContextPtr pccArg, int iArg, const char* pcArg)
{
  if (pccArg != NULL && STR_IS_NOT_EMPTY(pcArg)) {
    cxpCtxtLogPrint(pccArg, 1, pcArg);
  }
  exit(iArg);
} /* end of cxpCtxtLogPutsExit() */


/*! cxp Ctxt fprintf's log messages

\param pccArg -- pointer to context 
\bug restrict length of output content to size of mBuffer

\return TRUE if , FALSE in case of 
*/
BOOL_T
cxpCtxtLogPrint(cxpContextPtr pccArg, int level, const char *fmt, ...)
{
  BOOL_T fResult = FALSE;

  if (pccArg) {
    va_list ap;
    char mBuffer[BUFFER_LENGTH];

    assert(fmt != NULL);

    if (level < 1)
      cxpCtxtIncrExitCode(pccArg,1);

    if (level > cxpCtxtLogGetLevel(pccArg))
      return fResult;

    va_start(ap, fmt);

#ifdef _MSC_VER
    vsprintf_s(mBuffer, BUFFER_LENGTH, fmt, ap);
#else
    vsnprintf(mBuffer, BUFFER_LENGTH, fmt, ap);
#endif

    va_end(ap);

    cxpCtxtLogAppend(pccArg, BAD_CAST mBuffer);

    //\todo cxpCtxtGetRunningTime();
#ifdef DEBUG
    fprintf(stderr, "0x%8p: %s\n", (void *)pccArg, mBuffer);
#else
    fputs(mBuffer,stderr);
    fputs("\n",stderr);
#endif

#if defined(DEBUG) && defined(_WIN32)
    fflush(stderr);
#endif
  }
  return fResult;
} /* end of cxpCtxtLogPrint() */


/*! prints a document tree as XML into log file or log DOM if pdocArg is NULL
 */
void
cxpCtxtLogPrintNode(cxpContextPtr pccArg, int level, const char *pucArgLabel, xmlNodePtr pndArg)
{
  if (pccArg != NULL && level > pccArg->level_set) {
    /* ignoring output */
  }
  else {
    xmlDocPtr pdocT;
  
    pdocT = domDocFromNodeNew(pndArg);
    cxpCtxtLogPrintDoc(pccArg,level,pucArgLabel,pdocT);
    xmlFreeDoc(pdocT);
  }
} /* end of cxpCtxtLogPrintNode() */


/*! prints a document tree as XML into log file or log DOM if pdocArg is NULL
 */
void
cxpCtxtLogPrintDoc(cxpContextPtr pccArg, int level, const char *pucArgLabel, xmlDocPtr pdocArg)
{
  if (pccArg) {
    if (level > pccArg->level_set) {
      return; 
    }

    if (pucArgLabel) {
      cxpCtxtLogPrint(pccArg,level, pucArgLabel);
    }
    else if (pdocArg != NULL && pdocArg->URL != NULL) {
      cxpCtxtLogPrint(pccArg,level, (const char *)pdocArg->URL);
    }
#ifdef DEBUG
    domPutDocString(stderr,NULL,pdocArg); /*!\todo use pccArg->fhLog for output */
#endif
  }
  else {
#ifdef DEBUG
    domPutDocString(stderr,BAD_CAST pucArgLabel,pdocArg);
#endif
  }
} /* end of cxpCtxtLogPrintDoc() */


/*! detect readonly, log, searchpath attributes

\param pccArg -- pointer to context
\param pndArg -- node to check
\return a new cxp context if a node attribute "context" exists or DOM URL
*/
cxpContextPtr
cxpCtxtFromAttr(cxpContextPtr pccArg, xmlNodePtr pndArg)
{
  xmlChar *pucAttr;
  int iAttr = -1;
  cxpContextPtr pccResult = pccArg;
  resNodePtr prnT;

  prnT = cxpAttributeLocatorResNodeNew(pccArg, pndArg, NULL);
  if (prnT) { /* new locator context found */
    if (resPathIsEquivalent(resNodeGetNameNormalized(cxpCtxtLocationGet(pccArg)), resNodeGetNameNormalized(prnT)) == FALSE) {
      pccResult = cxpCtxtNew();
      cxpCtxtLocationSet(pccResult, prnT);
    }
    resNodeFree(prnT);
  }

  pucAttr = domGetPropValuePtr(pndArg, BAD_CAST "log");
  if (STR_IS_NOT_EMPTY(pucAttr)) {
    iAttr = atoi((const char *)pucAttr);
    if (iAttr == cxpCtxtLogGetLevel(pccArg) || iAttr > 5) {
      /* equal or invalid value */
    }
    else {
      if (pccResult == pccArg) {
	pccResult = cxpCtxtNew();
      }
      cxpCtxtLogSetLevel(pccResult, iAttr);
    }
  }

#ifdef EXPERIMENTAL

  pucAttr = domGetPropValuePtr(pndArg, BAD_CAST "searchpath");
  if (STR_IS_NOT_EMPTY(pucAttr)) {
    /* detect additional searchpath */

#ifdef HAVE_CGI
    cxpCtxtLogPrint(pccArg, 1, "ERROR: '%s' NOT to be used in CGI mode (CXP_PATH only) !!!", pucAttr);
#else
    if ((prnT = resNodeStrNew(pucAttr))) {
      cxpCtxtSearchSet(pccArg, prnT);
      resNodeListFree(prnT);
    }
#endif
  }
#endif

  //    cxpCtxtSetReadonly(pccResult,domGetPropFlag(pndArg,BAD_CAST "readonly",FALSE));

  if (pccResult != pccArg) {
    cxpCtxtCacheEnable(pccResult, domGetPropFlag(pndArg, BAD_CAST "cache", (pccArg ? pccArg->fCaching : TRUE)));
    cxpCtxtAddChild(pccArg,pccResult);
  }

  return pccResult;
} /* end of cxpCtxtFromAttr() */


/*!\return a copy of pccArg's location context
*/
resNodePtr
cxpCtxtLocationDup(cxpContextPtr pccArg)
{
  resNodePtr prnResult = NULL;

  if (pccArg) {
    cxpContextPtr pccT;

    for (pccT=pccArg; pccT != NULL; pccT = pccT->parent) {
      if (pccT->prnLocation) {
	prnResult = resNodeDup(pccT->prnLocation, RN_DUP_THIS);
	break;
      }
    }
  }
  return prnResult;
} /* end of cxpCtxtLocationDup() */


/*! Frees pccArg.

\param pccArg the context to free
\return a TRUE if successful
*/
BOOL_T
cxpCtxtFree(cxpContextPtr pccArg)
{
  if (pccArg) {
    cxpContextPtr pccT;

    if ((pccT = cxpCtxtGetNext(pccArg))) {
      cxpCtxtFree(pccT);
    }

    if ((pccT = cxpCtxtGetChild(pccArg))) {
      cxpCtxtFree(pccT);
    }

    if (pccArg->ppcEnv) {
      int i;

      for ( i=0; pccArg->ppcEnv[i]; i++) {
	xmlFree(pccArg->ppcEnv[i]);
      }
      xmlFree(pccArg->ppcEnv);
    }

    resNodeFree(pccArg->prnRoot);

    resNodeFree(pccArg->prnLocation);
    xmlFreeNodeList(pccArg->pndContextNode);

    /*! free list of search contexts */
    resNodeListFree(pccArg->prnSearch);
    /*! free list of cached contexts */
    cxpCtxtCacheEnable(pccArg, FALSE);
    cxpCtxtCachePrint(pccArg);
    resNodeListFree(pccArg->prnCache);

    if (pccArg->pdocContextNode) {
      xmlFreeDoc(pccArg->pdocContextNode);
    }

    if (pccArg->pndLog) {
      //domPutNodeString(stderr, BAD_CAST "LOG", pccArg->pndLog);
      xmlFreeNode(pccArg->pndLog);
    }

#ifdef HAVE_PCRE2
    if (pccArg->re_each) {
      pcre2_code_free(pccArg->re_each);
    }
#endif

    cxpCtxtEncFree(pccArg);

    memset(pccArg, 0, sizeof(cxpContext));
    xmlFree(pccArg);
    return TRUE;
  }
  return FALSE;
}
/* end of cxpCtxtFree() */


/*! set the internal readonly flag
*/
BOOL_T
cxpCtxtSetReadonly(cxpContextPtr pccArg, BOOL_T mode)
{
  BOOL_T fResult = FALSE;

  if (pccArg) {
    cxpCtxtLogPrint(pccArg, 1,"Set mode to '%s'", (mode ? "readonly" : "readwrite"));
    pccArg->mode_readonly = mode;
    fResult = pccArg->mode_readonly;
  }
  return fResult;
}
/* end of cxpCtxtSetReadonly() */


/*! \return the internal readonly flag
*/
BOOL_T
cxpCtxtIsReadonly(cxpContextPtr pccArg)
{
  BOOL_T fResult;

#ifdef HAVE_CGI
  fResult = TRUE;
#else
  fResult = FALSE;
#endif

  if (pccArg) {
    fResult = pccArg->mode_readonly;
  }
  return fResult;
}
/* end of cxpCtxtIsReadonly() */


/*! set the internal resNode of root directory. In CGI mode access is
restricted to descendant directories.

\param pucArgPath path string to root directory
\return TRUE if root setting is OK
*/
BOOL_T
cxpCtxtRootSet(cxpContextPtr pccArg, resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  if (pccArg) {
    resNodePtr prnRoot = NULL;
    resNodePtr prnDocumentRoot = NULL;
    xmlChar *pucDocumentRoot = NULL;
    xmlChar *pucRoot = NULL;

    if (pccArg->prnRoot) {
      resNodeFree(pccArg->prnRoot);
      pccArg->prnRoot = NULL;
    }

    pucRoot = cxpCtxtEnvGetValueByName(pccArg,NAME_ROOT);
#ifdef HAVE_CGI
    if (((pucDocumentRoot = cxpCtxtEnvGetValueByName(pccArg, BAD_CAST"DOCUMENT_ROOT")) == NULL || xmlStrlen(pucDocumentRoot) < 1)
      && ((pucDocumentRoot = cxpCtxtEnvGetValueByName(pccArg, NAME_ROOT)) == NULL || xmlStrlen(pucDocumentRoot) < 1)) {
      cxpCtxtLogPrint(pccArg, 1, "No usable value of '%s' '%s'", BAD_CAST"DOCUMENT_ROOT", pucDocumentRoot);
    }
    else if (resPathIsRelative(pucDocumentRoot)) {
      cxpCtxtLogPrint(pccArg, 2, "Value of '%s' is not an absolute path", pucDocumentRoot);
    }
    else if (resPathIsURL(pucDocumentRoot)) {
      cxpCtxtLogPrint(pccArg, 2, "Value of '%s' is an URL path", pucDocumentRoot);
    }
    else if ((prnDocumentRoot = resNodeRootNew(NULL,pucDocumentRoot)) == NULL) {
      cxpCtxtLogPrint(pccArg, 2, "No usable value '%s'", pucDocumentRoot);
    }
    else if (resNodeReadStatus(prnDocumentRoot) == FALSE || resNodeIsDir(prnDocumentRoot) == FALSE) {
      cxpCtxtLogPrint(pccArg, 2, "Value of '%s' is no existing directory", pucDocumentRoot);
      resNodeFree(prnDocumentRoot);
      prnDocumentRoot = NULL;
    }
    else if (STR_IS_EMPTY(pucRoot)) {
      cxpCtxtLogPrint(pccArg, 4, "No usable value of '%s' using value of '%s'", NAME_ROOT, BAD_CAST"DOCUMENT_ROOT");
      prnRoot = resNodeDup(prnDocumentRoot, RN_DUP_THIS);
    }
    else if ((prnRoot = resNodeRootNew(NULL,pucRoot)) == NULL) {
      cxpCtxtLogPrint(pccArg,2,"No usable value '%s'",pucRoot);
    }
    else if (resNodeReadStatus(prnRoot) == FALSE || resNodeIsDir(prnRoot) == FALSE) {
      cxpCtxtLogPrint(pccArg,2,"Value of '%s' is no existing directory",pucRoot);
    }
    else if (resPathIsDescendant(resNodeGetNameNormalized(prnDocumentRoot),resNodeGetNameNormalized(prnRoot))) {
      cxpCtxtLogPrint(pccArg,1,"Value '%s' is a descendant directory of '%s'",resNodeGetNameNormalized(prnRoot),resNodeGetNameNormalized(prnDocumentRoot));
    }
    /*!\bug handle additional cases */
    /*!\todo handle single file CXP_ROOT */
    resNodeFree(prnDocumentRoot);
#else
    if (STR_IS_EMPTY(pucRoot)) {
      cxpCtxtLogPrint(pccArg, 3, "No usable value of '%s'", NAME_ROOT);
      prnRoot = prnArg;
    }
    else if (resPathIsRelative(pucRoot)) {
      cxpCtxtLogPrint(pccArg, 2, "Value of '%s' is not an absolute path", pucRoot);
    }
    else if (resPathIsURL(pucRoot)) {
      cxpCtxtLogPrint(pccArg, 2, "Value of '%s' is an URL path", pucRoot);
    }
    else if ((prnRoot = resNodeRootNew(NULL,pucRoot)) == NULL) {
      cxpCtxtLogPrint(pccArg, 2, "No usable value '%s'", pucRoot);
    }
    else if (resNodeReadStatus(prnRoot) == FALSE || resNodeIsDir(prnRoot) == FALSE) {
      cxpCtxtLogPrint(pccArg, 2, "Value of '%s' is no existing directory", pucRoot);
    }
    else if (prnArg == NULL) {
      cxpCtxtLogPrint(pccArg, 1, "Using value '%s' of '%s'", resNodeGetNameNormalized(prnRoot), NAME_ROOT);
    }
    else if (resPathIsDescendant(resNodeGetNameNormalized(prnRoot), resNodeGetNameNormalized(prnArg))) {
      /*! value if 'pucArgRootDir' is descendant of 'pucEnvRootCxpDir' */
      cxpCtxtLogPrint(pccArg, 1, "Value '%s' is a descendant directory of '%s'", resNodeGetNameNormalized(prnArg), resNodeGetNameNormalized(prnRoot));
      prnRoot = prnArg;
    }
#endif
    xmlFree(pucRoot);
    xmlFree(pucDocumentRoot);

    if (resNodeReadStatus(prnRoot)) {
      if (resNodeIsDir(prnRoot)) {
	cxpCtxtLogPrint(pccArg, 2, "Value of '%s' is an existing directory '%s'", NAME_ROOT, resNodeGetNameNormalized(prnRoot));
	pccArg->prnRoot = resNodeDup(prnRoot, RN_DUP_THIS);
	fResult = TRUE;
      }
      else if (resNodeIsFile(prnRoot)) {
	cxpCtxtLogPrint(pccArg, 2, "Value of '%s' is an existing file '%s'", NAME_ROOT, resNodeGetNameNormalized(prnRoot));
	pccArg->prnRoot = resNodeDup(prnRoot, RN_DUP_THIS);
	fResult = TRUE;
      }
      else {
	cxpCtxtLogPrint(pccArg, 1, "Value of '%s' is neither an existing file nor directory '%s'", NAME_ROOT, resNodeGetNameNormalized(prnRoot));
      }
    }
    else {
      cxpCtxtLogPrint(pccArg, 1, "Value '%s' is not existing", resNodeGetNameNormalized(prnRoot));
    }
    resNodeFree(prnRoot);
  }
  return fResult;
} /* end of cxpCtxtRootSet() */


/*! \return a copy of the internal root path
*/
resNodePtr
cxpCtxtRootGet(cxpContextPtr pccArg)
{
  resNodePtr prnResult = NULL;

  if (pccArg) {
    cxpContextPtr pccParent;

    if (resNodeReadStatus(pccArg->prnRoot)) {
      prnResult = pccArg->prnRoot;
    }
    else if ((pccParent = cxpCtxtGetParent(pccArg))) {
      prnResult = cxpCtxtRootGet(pccParent);
    }
    else {
      // NULL
    }
  }
  return prnResult;
}
/* end of cxpCtxtRootGet() */


/*! \return TRUE if prnArg is a descendant of Root context OR Search context
\todo change direct access to resNode fields
\todo handle symbolic link targets
*/
BOOL_T
cxpCtxtAccessIsPermitted(cxpContextPtr pccArg, resNodePtr prnArg)
{
  if (prnArg) {
    resNodePtr prnT;
    cxpContextPtr pccI;

    switch (resNodeGetType(prnArg)) {
    case rn_type_stdout:
    case rn_type_stderr:
      cxpCtxtLogPrint(pccArg,3, "Printing to stdout/stderr allowed in all modes");
      return TRUE;
      break;

    case rn_type_stdin:
      cxpCtxtLogPrint(pccArg,3, "Reading from stdin allowed in all modes");
      return TRUE;
      break;

    case rn_type_url:
    case rn_type_url_ftp:
    case rn_type_url_http:
      cxpCtxtLogPrint(pccArg,3, "Reading from external URLs allowed in all modes");
      return TRUE;
      break;

    default:
      break;
    }

    /* check for root directory context */
    if (cxpCtxtRootGet(pccArg)) {
      if (resPathIsDescendant(resNodeGetNameNormalized(cxpCtxtRootGet(pccArg)), resNodeGetNameNormalized(prnArg))) {
#ifdef DEBUG
	cxpCtxtLogPrint(pccArg,3, "Access to '%s' as subdir of root allowed", resNodeGetNameNormalized(prnArg));
#endif
	return TRUE;
      }
#ifdef DEBUG
      else {
	cxpCtxtLogPrint(pccArg,1, "Access to '%s' denied (root='%s')", resNodeGetNameNormalized(prnArg), resNodeGetNameNormalized(cxpCtxtRootGet(pccArg)));
      }
#endif
    }
#ifdef HAVE_CGI
    /* not in CGI mode */
#else
    else {
      prnT = cxpCtxtLocationGet(pccArg);
      if (prnT) {
	if (resPathIsDescendant(resNodeGetNameNormalized(prnT), resNodeGetNameNormalized(prnArg))) {
	  return TRUE;
	}
	else {
	  return TRUE;
	}
      }
#ifdef DEBUG
      cxpCtxtLogPrint(pccArg,4, "No access restrictions at root");
#endif
      return TRUE;
    }
#endif

    /* check for search directory context */

    for (pccI = pccArg; pccI; pccI = cxpCtxtGetParent(pccI)) {
      for (prnT = cxpCtxtSearchGet(pccI); prnT; prnT = resNodeGetNext(prnT)) {
	if (resNodeIsDir(prnT)
	  && resPathIsDescendant(resNodeGetNameNormalized(prnT), resNodeGetNameNormalized(prnArg))) {
	  /* OK */
#ifdef DEBUG
	  cxpCtxtLogPrint(pccArg, 1, "Access to '%s' as descendant of search directory '%s' allowed", resNodeGetNameNormalized(prnArg), resNodeGetNameNormalized(prnT));
#endif
	  return TRUE;
	}
#ifdef DEBUG
	else {
	  cxpCtxtLogPrint(pccArg, 1, "Access to '%s' as descendant of search directory '%s' denied", resNodeGetNameNormalized(prnArg), resNodeGetNameNormalized(prnT));
	}
#endif
      }
    }
    resNodeSetError(prnArg, rn_error_access,"access"); /* by default */
  }
  return FALSE;
} /* end of cxpCtxtAccessIsPermitted() */


/*! returns pointer to location of pccArg.

\param pccArg the context to free
\return pointer to location path string if successful
*/
xmlChar *
cxpCtxtLocationGetStr(cxpContextPtr pccArg)
{
  xmlChar *pucResult = NULL;

  if (pccArg) {
    pucResult = resNodeGetNameNormalized(cxpCtxtLocationGet(pccArg));
  }
  return pucResult;
}
/* end of cxpCtxtLocationGetStr() */


/*! returns pointer to location of pccArg.

\param pccArg the context to free
\return pointer to location path string if successful
*/
resNodePtr
cxpCtxtLocationGet(cxpContextPtr pccArg)
{
  resNodePtr prnResult = NULL;

  if (pccArg) {
    cxpContextPtr pccParent;

    if ((prnResult = pccArg->prnLocation)) {
    }
    else if ((pccParent = cxpCtxtGetParent(pccArg)) && (prnResult = cxpCtxtLocationGet(pccParent))) {
    }
    else {
    }
  }
  return prnResult;
}
/* end of cxpCtxtLocationGet() */


/*! set location of pccArg.

\param pccArg the context to set
\return TRUE if successful
*/
BOOL_T
cxpCtxtLocationSet(cxpContextPtr pccArg,resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  assert(resNodeGetNext(prnArg) == NULL);
  assert(resNodeGetChild(prnArg) == NULL);
  
  if (pccArg) {
    if (pccArg->prnLocation) {
      resNodeFree(pccArg->prnLocation);
    }

    if (prnArg) {
      pccArg->prnLocation = resNodeDup(prnArg, RN_DUP_THIS);
    }
    else {
      pccArg->prnLocation = resNodeCwdNew();
    }
#ifdef DEBUG
    cxpCtxtLogPrint(pccArg, 1,"Processing context changed to = %s",cxpCtxtLocationGetStr(pccArg));
#endif
    fResult = TRUE;
  }
  return fResult;
} /* end of cxpCtxtLocationSet() */


/*! set processing node of pccArg.

\param pccArg -- pointer to context
\param pndArg -- pointer to node to process later
\return TRUE if , FALSE in case of
*/
BOOL_T
cxpCtxtProcessSetNodeCopy(cxpContextPtr pccArg, xmlNodePtr pndArg)
{
  BOOL_T fResult = FALSE;

  if (pccArg) {
    if (pccArg->pndContextNode) {
      xmlFreeNodeList(pccArg->pndContextNode);
    }

    if (pndArg) {
      pccArg->pndContextNode = xmlCopyNodeList(pndArg);
    }
    else {
      pccArg->pndContextNode = NULL;
    }
#ifdef DEBUG
    cxpCtxtLogPrint(pccArg, 1, "Processing Node set copy");
#endif
    fResult = TRUE;
  }
  return fResult;
} /* end of cxpCtxtProcessSetNode() */


/*! cxp Ctxt Process Get Node

\param pccArg -- pointer to context
\return pointer to the internal process node
*/
xmlNodePtr
cxpCtxtProcessGetNode(cxpContextPtr pccArg)
{
  xmlNodePtr pndResult = NULL;

  if (pccArg) {
    pndResult = pccArg->pndContextNode;
  }
  return pndResult;
} /* end of cxpCtxtProcessGetNode() */


/*! cxp Ctxt Process Get Node

\param pccArg -- pointer to context
\return pointer to the internal process node
*/
xmlChar*
cxpCtxtProcessDump(cxpContextPtr pccArg)
{
  xmlChar* pucResult = NULL;

  if (pccArg) {
    xmlNodePtr pndT = NULL;

    if (pccArg->pndContextNode) {
      pndT = pccArg->pndContextNode;
    }
    else if (pccArg->pdocContextNode) {
      pndT = xmlDocGetRootElement(pccArg->pdocContextNode);
    }

    if (pndT) {
      xmlBufferPtr buffer;

      buffer = xmlBufferCreate();
      if (buffer) {
        int size;

        size = xmlNodeDump(buffer, NULL, pndT, 0, 1);
        if (size > 0) {
	  pucResult = xmlBufferDetach(buffer);
        }
        xmlBufferFree(buffer);
      }
    }
    else {
      printf("Error cxpCtxtProcessDump() ...\n");
    }
  }

  return pucResult;
} /* end of cxpCtxtProcessDump() */


/*! 

\param pccArg -- pointer to context
\param *pucArg -- pointer to path string to update location
\return TRUE if , FALSE in case of
*/
BOOL_T
cxpCtxtLocationUpdate(cxpContextPtr pccArg, xmlChar *pucArg)
{
  BOOL_T fResult = FALSE;

  if (pccArg != NULL && STR_IS_NOT_EMPTY(pucArg)) {
    if (pccArg->prnLocation) {
      if (resPathIsRelative(pucArg)) {
	fResult = resNodeConcat(pccArg->prnLocation, pucArg);
      }
      else {
	fResult = resNodeReset(pccArg->prnLocation, pucArg);
      }
    }
  }
  return fResult;
} /* end of cxpCtxtLocationUpdate() */


/*! \return the value of context exit code
*/
int
cxpCtxtGetExitCode(cxpContextPtr pccArg)
{
  int iResult = 0;

  if (pccArg) {
    iResult = pccArg->iCountError;
  }
  return iResult;
} /* end of cxpCtxtGetExitCode() */


/*! \return the incremented value of context exit code
*/
int
cxpCtxtIncrExitCode(cxpContextPtr pccArg, int iArg)
{
  int iResult = 0;

  if (pccArg != NULL && iArg > 0) {
    pccArg->iCountError += iArg;
    iResult = pccArg->iCountError;
  }
  return iResult;
} /* end of cxpCtxtIncrExitCode() */


/*! prints info about all data of pccArg.

\param pccArg the context to free
\return a TRUE if successful
*/
BOOL_T
cxpCtxtLogInfo(cxpContextPtr pccArg)
{
  if (pccArg) {
    int i;

    cxpCtxtLogPrint(pccArg, 1, "Location: '%s'", resNodeGetNameNormalized(pccArg->prnLocation));

      for (i = 0; i < cxpCtxtCliGetCount(pccArg); i++) {
	cxpCtxtLogPrint(pccArg, 1, "Arg %i: '%s'", i, cxpCtxtCliGetValue(pccArg,i));
      }

      for (i = 0; i < cxpCtxtEnvGetCount(pccArg); i++) {
	xmlChar *pucN;
	xmlChar *pucV;

	pucN = cxpCtxtEnvGetName(pccArg, i);
	pucV = cxpCtxtEnvGetValue(pccArg, i);
	cxpCtxtLogPrint(pccArg, 1, "Env: '%s' = '%s'", pucN, pucV);
	xmlFree(pucV);
	xmlFree(pucN);
      }

#if 0
    if (pccArg->in == stdin) {
      cxpCtxtLogPrint(pccArg, 1, "\t in = 'stdin'");
    }
    else {
      cxpCtxtLogPrint(pccArg, 1, "\t in = '%0x'", pccArg->in);
    }

    if (pccArg->out == stdout) {
      cxpCtxtLogPrint(pccArg, 1, "\tout = 'stdout'");
    }
    else {
      cxpCtxtLogPrint(pccArg, 1, "\tout = '%0x'", pccArg->out);
    }

    if (pccArg->log == stdout) {
      cxpCtxtLogPrint(pccArg, 1, "\tlog = 'stdout'");
    }
    else {
      cxpCtxtLogPrint(pccArg, 1, "\tlog = '%0x'", pccArg->log);
    }

    if (pccArg->err == stderr) {
      cxpCtxtLogPrint(pccArg, 1, "\terr = 'stderr'");
    }
    else {
      cxpCtxtLogPrint(pccArg, 1, "\terr = '%0x'", pccArg->err);
    }
#endif

    cxpCtxtLogPrintDoc(pccArg, 1, "\nContext DOM:", pccArg->pdocContextNode);

    //cxpCtxtLogPrintDoc(pccArg, 1, "Context DOM:", pccArg->pndContextNode);

    cxpCtxtLogPrintDoc(pccArg, 1, "Context DOM:", NULL);

    return TRUE;
  }
  return FALSE;
}
/* end of cxpCtxtLogInfo() */


/*! set the internal debug level

0 - no log messages at all, error messages only
1 - minimum log level
 */
int
cxpCtxtLogSetLevel(cxpContextPtr pccArg, int level)
{
  int iResult = -1;

  if (pccArg) {
    if (level > 0) {
      if (level != pccArg->level_set) {
	if (level > LEVEL_MAX) {
	  level = LEVEL_MAX;
	}
	cxpCtxtLogPrint(pccArg, 1, "Change log level from '%i' to '%i'", pccArg->level_set, level);
	pccArg->level_set = level;
	iResult = 0;
      }
    }
    else {
      pccArg->level_set = 0;
      iResult = 0;
    }
  }
  return iResult;
} /* end of cxpCtxtLogSetLevel() */


/*! set the internal debug level via string

0 - no log messages at all, error messages only
1 - minimum log level
*/
int
cxpCtxtLogSetLevelStr(cxpContextPtr pccArg, xmlChar *pucArg)
{
  int iResult = -1;

  if (STR_IS_NOT_EMPTY(pucArg)) {
    int i;

    if (*pucArg == (xmlChar)'\"') {
      pucArg++;
    }

    i = atoi((const char *)pucArg);
    iResult = cxpCtxtLogSetLevel(pccArg, i);
  }
  return iResult;
}
/* end of cxpCtxtLogSetLevelStr() */


/*! set the internal debug level

0 - no log messages at all, error messages only
1 - minimum log level
*/
int
cxpCtxtLogGetLevel(cxpContextPtr pccArg)
{
  int iResult = -1;

  if (pccArg) {
    cxpContextPtr pccParent;

    if (pccArg->level_set > -1) {
      iResult = pccArg->level_set;
    }
    else if ((pccParent = cxpCtxtGetParent(pccArg))) {
      iResult = cxpCtxtLogGetLevel(pccParent);
    }
    else {
    }
  }
  return iResult;
} /* end of cxpCtxtLogGetLevel() */


/*! a mapper function with UTF-8 arguments and result
 */
xmlChar *
cxpCtxtGetHostValueNamed(cxpContextPtr pccArgParent, const xmlChar *pucName)
{
  xmlChar *pucResult = NULL;

  if (xmlStrEqual(BAD_CAST"os",pucName)) {
#ifdef _MSC_VER
    pucResult = xmlStrdup(BAD_CAST "Windows");
#else
    struct utsname Uname;

    if (uname(&Uname) > -1) {
      pucResult = xmlStrdup(BAD_CAST Uname.sysname);
    }
#endif
  }

  if (pucResult) {
#ifdef DEBUG
    PrintFormatLog(4, "host[%s]='%s'", pucName, pucResult);
#endif
  }
  else {
    PrintFormatLog(2, "No valid host variable named '%s'", pucName);
  }

  return pucResult;
}
/* End of GetHostValueNamed() */


/*! \return the running time of current process in seconds

*/
int
cxpCtxtGetRunningTime(cxpContextPtr pccArg)
{
  time_t system_zeit_1;

  time(&system_zeit_1);

  return (int)(pccArg->system_zeit > 0 ? difftime(system_zeit_1, pccArg->system_zeit) : -1);
}
/* end of cxpCtxtGetRunningTime() */


/*! Resource Node List Get Next

\return pointer to next cxpContext in list
*/
cxpContextPtr
cxpCtxtGetNext(cxpContextPtr pccArg)
{
  cxpContextPtr pccResult = NULL;

  if (pccArg) {
    pccResult = pccArg->next;
  }
  return pccResult;
} /* end of cxpCtxtGetNext() */


/*! Resource Node List Get Parent

\return pointer to Parent cxpContext of pfdArg
*/
cxpContextPtr
cxpCtxtGetParent(cxpContextPtr pccArg)
{
  cxpContextPtr pccResult = NULL;

  if (pccArg) {
    pccResult = pccArg->parent;
  }
  return pccResult;
} /* end of cxpCtxtGetParent() */


/*! Resource Node List Get Parent top

\return pointer to top Parent cxpContext of pfdArg
*/
cxpContextPtr
cxpCtxtGetTop(cxpContextPtr pccArg)
{
  cxpContextPtr pccResult = NULL;

  if (pccArg) {
    cxpContextPtr pccT;

    /*! find top context */
    for (pccT = pccResult = pccArg; (pccT = pccT->parent); pccResult = pccT);
  }
  return pccResult;
} /* end of cxpCtxtGetTop() */


/*! Resource Node List Get Child

\return pointer to child cxpContext of pfdArg
*/
cxpContextPtr
cxpCtxtGetChild(cxpContextPtr pccArg)
{
  cxpContextPtr pccResult = NULL;

  if (pccArg) {
    pccResult = pccArg->children;
  }
  return pccResult;
} /* end of cxpCtxtGetChild() */


/*! Resource Node List Append

\param pccArgList -- pointer to cxpContext list
\param pccArg -- cxpContext to append to list
\return TRUE if successful, FALSE in case of errors
*/
BOOL_T
cxpCtxtUnlinkChild(cxpContextPtr pccArgList, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;

  if (pccArgList != NULL && pccArg != NULL) {
    cxpContextPtr pccT;

    if (pccArg == pccArgList->children) { /* very first element */
      pccArgList->children = pccArg->next;
      fResult = TRUE;
    }
    else {
      for (pccT = pccArgList->children; pccT; pccT = pccT->next) {
	if (pccT->next == pccArg) {
	  pccT->next = pccArg->next;
	  fResult = TRUE;
	  break;
	}
      }
    }
    pccArg->next = NULL;
  }
  return fResult;
} /* end of cxpCtxtUnlinkChild() */


/*! Resource Node List AppendChild

\param pccArgParent -- cxpContext to use as parent
\param pccArgChild -- cxpContext to use as child
\return TRUE if successful, FALSE in case of errors
*/
cxpContextPtr
cxpCtxtAddChild(cxpContextPtr pccArgParent, cxpContextPtr pccArgChild)
{
  cxpContextPtr pccResult = NULL;

  if (pccArgParent != NULL && pccArgChild != NULL) {

    assert(pccArgParent != pccArgChild);

    if (pccArgParent->children) {
      cxpContextPtr pccT;

      for (pccT = pccArgParent->children; pccT; pccT = pccT->next) {
	if (pccT->next == NULL) {
	  pccT->next = pccArgChild;
	  pccArgChild->parent = pccArgParent;
	  pccResult = pccArgChild;
	  break;
	}
      }
    }
    else {
      pccArgParent->children = pccArgChild;
      pccArgChild->parent = pccArgParent;
      pccResult = pccArgChild;
    }
  }
  return pccResult;
} /* end of cxpCtxtAddChild() */


#ifdef TESTCODE
#include "test/test_cxp_context.c"
#endif

