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

/*
 */
#include <libxml/xmlversion.h>
#include <libxml/HTMLtree.h>
#include <libxml/parser.h>

/*
 */
#include "basics.h"
#include "utils.h"
#include <res_node/res_node_io.h>
#include <cxp/cxp.h>
#include "dom.h"
#include <cxp/cxp_dir.h>
#include "plain_text.h"

#ifdef HAVE_PIE
#include <pie/pie_dtd.h>
#endif

#ifdef HAVE_PETRINET
#include <petrinet/petrinet.h>
#endif

#ifdef HAVE_JS
#include <script/script.h>
#endif

#ifdef HAVE_CGI
#include <cxp/cxp_context_cgi.h>
#endif

static BOOL_T
ApplySubstText(const xmlNodePtr node, const xmlChar *pucFrom, const xmlChar *pucTo, BOOL_T(*pSkip)(xmlNodePtr pndArg));

static BOOL_T
ApplySubstRegExp(const xmlNodePtr pndArg, const pcre2_code* preArgFrom, const xmlChar* pucTo, BOOL_T(*pSkip)(xmlNodePtr pndArg));

static BOOL_T
cxpSubstSkip(xmlNodePtr pndArg);

static cxpSubstPtr
cxpSubstNew(void);

static BOOL_T
cxpSubstPrint(cxpSubstPtr pcxpSubstArg, cxpContextPtr pccArg);


/*! apply all substitutions recursively

\param pndArg a xmlNodePtr to substitute node and its childs
\param pucFrom
\param pucTo

\return TRUE if successful
*/
BOOL_T
ApplySubstText(const xmlNodePtr pndArg, const xmlChar *pucFrom, const xmlChar *pucTo, BOOL_T (* pSkip)(xmlNodePtr pndArg))
{
  BOOL_T fResult = FALSE;

  if (pndArg) {
    if (pSkip != NULL && (*pSkip)(pndArg)) {
      /* skip this branch */
    }
    else if (pndArg->type == XML_TEXT_NODE) {
      if (pndArg->content) {
	xmlChar *pucNew;

	/*!\todo reduce multiple xmlNodeSetContent() */
	pucNew = ReplaceStr(pndArg->content, pucFrom, pucTo);
	if (pucNew) {
	  xmlNodeSetContent(pndArg, pucNew);
	  xmlFree(pucNew);
	  fResult = TRUE;
	}
      }
    }
    else if (pndArg->type == XML_ELEMENT_NODE || pndArg->type == XML_ATTRIBUTE_NODE) {
      ApplySubstText((xmlNode *)pndArg->properties, pucFrom, pucTo, pSkip);
      ApplySubstText(pndArg->children, pucFrom, pucTo,pSkip);
    }
    else {
      /* ignore */
    }
    ApplySubstText(pndArg->next, pucFrom, pucTo, pSkip);
  }
  return fResult;
} /* end of ApplySubstText() */


#ifdef HAVE_PCRE2

/*! apply all substitutions recursively

\param pndArg a xmlNodePtr to substitute node and its childs
\param pucFrom
\param pucTo

\return TRUE if successful
*/
BOOL_T
ApplySubstRegExp(const xmlNodePtr pndArg, const pcre2_code* preArgFrom, const xmlChar* pucTo, BOOL_T(*pSkip)(xmlNodePtr pndArg))
{
  BOOL_T fResult = FALSE;

  if (pndArg) {
    if (pSkip != NULL && (*pSkip)(pndArg)) {
      /* skip this branch */
    }
    else if (pndArg->type == XML_TEXT_NODE || pndArg->type == XML_PI_NODE || pndArg->type == XML_COMMENT_NODE) {
      int rc;
      size_t sInput;

      sInput = xmlStrlen(pndArg->content);
      if (sInput > 0) {
	size_t sOutput;
	size_t sOutputMax;
	xmlChar* pucOutput;

	sOutput = sOutputMax = ((sInput < 256) ? 256 : sInput) * 8;
	
	if ((pucOutput = BAD_CAST xmlMalloc(sOutputMax)) != NULL) {

	  if ((rc = pcre2_substitute(preArgFrom, (PCRE2_SPTR8)pndArg->content, sInput, 0, PCRE2_SUBSTITUTE_GLOBAL,
	    NULL, NULL, (PCRE2_SPTR8)pucTo, xmlStrlen(pucTo), pucOutput, &sOutput)) > 0) {

	    assert(sOutput < sOutputMax);
	    xmlNodeSetContent(pndArg, BAD_CAST pucOutput);
	    fResult = TRUE;
	  }
	  xmlFree(pucOutput);
	}
      }
    }
    else if (pndArg->type == XML_ELEMENT_NODE || pndArg->type == XML_ATTRIBUTE_NODE) {
      ApplySubstRegExp((xmlNode*)pndArg->properties, preArgFrom, pucTo, pSkip);
      ApplySubstRegExp(pndArg->children, preArgFrom, pucTo, pSkip);
    }
    else {
      /* ignore */
    }
    ApplySubstRegExp(pndArg->next, preArgFrom, pucTo, pSkip);
  }
  return fResult;
} /* end of ApplySubstRegExp() */

#endif


/*! process the childs of top element MAKE (non-recursive!)

\param pndArg a xmlNodePtr to append data
\param pccArg the filesystem context

\return TRUE if successful
*/
BOOL_T
cxpSubstIncludeNodes(xmlNodePtr pndArg,cxpContextPtr pccArg)
{
#ifdef DEBUG
  cxpCtxtLogPrint(pccArg,4,"cxpSubstIncludeNodes(pndArg=%0x,pccArg=%0x)",pndArg,pccArg);
#endif

  if (IS_VALID_NODE(pndArg) == FALSE) {
    /* ignore non-valid elements */
    return FALSE;
  }
  else if (IS_NODE_INCLUDE(pndArg)) {
    if (IS_ENODE(pndArg->children)) {
      cxpCtxtLogPrint(pccArg,1,"Ignoring include element with childs");
    }
    else {
      cxpSubstPtr pcxpSubstT;

      pcxpSubstT = cxpSubstDetect(pndArg,pccArg);
      if (pcxpSubstT) {
	xmlDocPtr pdocInclude = NULL;
	xmlNodePtr pndRootInclude;
	xmlChar *pucT;

	if (pcxpSubstT->pucName) {
	  resNodePtr prnInclude;

	  prnInclude = cxpResNodeResolveNew(pccArg, pndArg, pcxpSubstT->pucName, CXP_O_READ);
	  if (prnInclude) {
	    pdocInclude = resNodeReadDoc(prnInclude);
	    resNodeFree(prnInclude);
	  }
	}
	else if ((pucT = cxpSubstGetPtr(pcxpSubstT)) != NULL) {
	  pdocInclude = xmlParseMemory((const char*)pucT, xmlStrlen(pucT));
	}

	if (pdocInclude) {
	  pndRootInclude = xmlDocGetRootElement(pdocInclude);
	  if (pndRootInclude) {
	    /* replace pndArg with a copy of pndRootInclude */
	    xmlNodePtr pndCopy;

	    pndCopy = xmlCopyNode(pndRootInclude,1);
	    if (pndCopy) {
	      xmlNodePtr pndOld;

	      /*!\todo use cxpSubstApply(pndCopy) */
	      pndOld = xmlReplaceNode(pndArg,pndCopy);
	      if (pndOld) {
		xmlFreeNode(pndOld);
	      }
	    }
	  }
	  xmlFreeDoc(pdocInclude);
	} else {
	  xmlAddChild(pndArg,xmlNewComment(BAD_CAST" XML parser error "));
	}
	cxpSubstFree(pcxpSubstT);
      }
    }
  }
  else if (pndArg) {
    /*
     */
    xmlNodePtr pndChild;

    for (pndChild = pndArg->children; pndChild;) {
      xmlNodePtr pndChildNext;

      pndChildNext = pndChild->next;
      cxpSubstIncludeNodes(pndChild,pccArg);
      pndChild = pndChildNext;
    }
  }

  return TRUE;
}
/* end of cxpSubstIncludeNodes() */


/*!
*/
cxpSubstPtr
cxpSubstNew(void)
{
  cxpSubstPtr pcxpSubstResult = NULL;

  pcxpSubstResult = (cxpSubstPtr)xmlMalloc(sizeof(cxpSubst));
  if (pcxpSubstResult) {
    /*! */
    memset(pcxpSubstResult,0,sizeof(cxpSubst));
  }
  return pcxpSubstResult;
}
/* end of cxpSubstNew() */


/*!\return a new cxp subst
*/
cxpSubstPtr
cxpSubstDetect(xmlNodePtr pndArgSubst, cxpContextPtr pccArg)
{
  cxpSubstPtr pcxpSubstResult = NULL;

#ifdef DEBUG
  cxpCtxtLogPrint(pccArg,3,"cxpSubstDetect(pndArgSubst=%0x,pccArg=%0x)",pndArgSubst,pccArg);
#endif

  if (pndArgSubst) {
    //assert(IS_NODE_SUBST(pndArgSubst) || IS_NODE_INCLUDE(pndArgSubst) || IS_NODE_VARIABLE(pndArgSubst));
    pcxpSubstResult = cxpSubstNew();
    if (pcxpSubstResult) {
      xmlChar *pucT;
      xmlChar *pucTT;

      pcxpSubstResult->pndSubst = pndArgSubst;

      if (domPropIsEqual(pndArgSubst,BAD_CAST"encoding",BAD_CAST"base64")) {
	pcxpSubstResult->eEncoding = base64;
      }
      else if (domGetPropFlag(pndArgSubst,BAD_CAST"escaping",FALSE)) {
	pcxpSubstResult->eEncoding = rfc1738;
      }

      if ((pucT = domGetPropValuePtr(pndArgSubst, BAD_CAST "name"))) {
	if (xmlStrlen(pucT) > 0 && ! IS_ENODE(pndArgSubst->children)) {
	  pcxpSubstResult->pucName  = xmlStrdup(pucT);
	}
      }
      else if ((pucT = domGetPropValuePtr(pndArgSubst, BAD_CAST "string"))) {
	pcxpSubstResult->pucName  = xmlStrdup(pucT);
      }
#ifdef HAVE_PCRE2
      else if ((pucT = domGetPropValuePtr(pndArgSubst, BAD_CAST "regexp"))) {
	int errornumber = 0;
	size_t erroroffset;
	
	pcxpSubstResult->preFrom = pcre2_compile(
						 (PCRE2_SPTR8)pucT, /* the pattern */
						 PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
						 PCRE2_UTF|PCRE2_CASELESS,        /* default options */
						 &errornumber,          /* for error number */
						 &erroroffset,          /* for error offset */
						 NULL);                 /* use default compile context */
	
	if (pcxpSubstResult->preFrom == NULL) {
	  PCRE2_UCHAR buffer[256];
	
	  pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
	  cxpCtxtLogPrint(pccArg,1,"PCRE2 compilation failed at offset %d: %s", (int)erroroffset, buffer);
	}

	/*!\todo check pcre2_compile() of pucTo once ??? */
      }
#endif

      if ((pucT = domGetPropValuePtr(pndArgSubst, BAD_CAST "default"))) {
	/* detect a possible default value */
	pcxpSubstResult->pucDefault = xmlStrdup(pucT);
      }
      else if ((pucT = domGetPropValuePtr(pndArgSubst, BAD_CAST "select"))) {
	pcxpSubstResult->pucDefault = xmlStrdup(pucT);
      }
      else if (pndArgSubst->children != NULL && pndArgSubst->children == pndArgSubst->last
	       && xmlNodeIsText(pndArgSubst->children) && STR_IS_NOT_EMPTY(pndArgSubst->children->content)) {
	/* detect a possible default value */
	pcxpSubstResult->pucDefault = xmlStrdup(pndArgSubst->children->content);
      }
      
      if (((pucT = domGetPropValuePtr(pndArgSubst, BAD_CAST "to")) != NULL
	   && (pucT = xmlStrdup(pucT)) != NULL)
	  ||
	  ((pcxpSubstResult->pndPlain = domGetFirstChild(pndArgSubst, NAME_PLAIN)) != NULL
	   && (pucT = cxpProcessPlainNode(pcxpSubstResult->pndPlain,pccArg)) != NULL)
	  ) {
	/* this is a substitution with a simple string or plain result of child */
	pcxpSubstResult->pucTo = pucT;
      }

      /*!\todo use text node content also */

#ifdef HAVE_JS
      if ((pcxpSubstResult->pucScriptResult = scriptProcessScriptAttribute(pndArgSubst,pccArg)) != NULL) {
	/* this is a substitution with script result */
	if (pcxpSubstResult->eEncoding == rfc1738) {
	  pucTT = pcxpSubstResult->pucScriptResult;
	  pcxpSubstResult->pucScriptResult = EncodeRFC1738(pcxpSubstResult->pucScriptResult);
	  xmlFree(pucTT);
	}
      }
#endif

      if ((pucT = domGetPropValuePtr(pndArgSubst, BAD_CAST "dir"))) {
	/*
  	       this is a substitution with current directories
	 */
	if (xmlStrcasecmp(pucT, BAD_CAST "pwd")==0) {
	  pcxpSubstResult->pucDir = resPathGetCwdStr();
	}
	else if (xmlStrcasecmp(pucT, BAD_CAST "ofFile")==0) {
	  if (pccArg) {
	    pcxpSubstResult->pucDir = xmlStrdup(cxpCtxtLocationGetStr(pccArg));
	  }
	  else {
	    cxpCtxtLogPrint(pccArg,1, "No filename registered");
	  }
	}
	else if (xmlStrcasecmp(pucT,BAD_CAST "dialog")==0) {
	  pcxpSubstResult->pucDir = GetSelectedFolderName(BAD_CAST "Dir", resNodeGetNameNormalized(cxpCtxtLocationGet(pccArg)));
	}
	else if (xmlStrlen(pucT) > 0) {
	  /*! basedir of given file detected */
	  resNodePtr prnDir;

	  prnDir = resNodeFromNodeNew(cxpCtxtLocationGet(pccArg),pucT);
	  if (prnDir) {
#ifdef HAVE_CGI
	      pcxpSubstResult->pucDir = resPathGetBasedirStr(resNodeGetNameRelative(cxpCtxtRootGet(pccArg),prnDir));
#else
	      pcxpSubstResult->pucDir = xmlStrdup(resNodeGetNameBaseDir(prnDir));
#endif
	    resNodeFree(prnDir);
	  }
	  else {
	    cxpCtxtLogPrint(pccArg,1, "No filename registered");
	  }
	}
	resPathChangeToSlashes(pcxpSubstResult->pucDir);

	if (pcxpSubstResult->pucDir != NULL && pcxpSubstResult->eEncoding == rfc1738) {
	  pucT = pcxpSubstResult->pucDir;
	  pcxpSubstResult->pucDir = EncodeRFC1738(pcxpSubstResult->pucDir);
	  xmlFree(pucT);
	}
      }

      pucT = domGetPropValuePtr(pndArgSubst, BAD_CAST "filename");
      if (STR_IS_NOT_EMPTY(pucT)) {
	/*
          this is a substitution with file name
	 */
	if (xmlStrEqual(pucT,BAD_CAST".") && pccArg != NULL && pndArgSubst->doc != NULL) {
	  pcxpSubstResult->pucFilename = resPathGetBasenameStr(BAD_CAST pndArgSubst->doc->URL);
	}
#ifdef HAVE_CGI
#else
	else if (xmlStrcasecmp(pucT,BAD_CAST "dialog")==0) {
	  pcxpSubstResult->pucFilename = GetSelectedFileName(BAD_CAST "FILE", resNodeGetNameNormalized(cxpCtxtLocationGet(pccArg)));
	}
#endif
	else {
	  pcxpSubstResult->pucFilename = resPathGetBasenameStr(pucT);
	}

	if (pcxpSubstResult->pucFilename != NULL && pcxpSubstResult->eEncoding == rfc1738) {
	  pucT = pcxpSubstResult->pucFilename;
	  pcxpSubstResult->pucFilename = EncodeRFC1738(pcxpSubstResult->pucFilename);
	  xmlFree(pucT);
	}
      }
      /*!\todo use also dirSearchFile() */

      pucT = domGetPropValuePtr(pndArgSubst, BAD_CAST "ext");
      if (STR_IS_NOT_EMPTY(pucT)) {
	/*
          this is a substitution with file extension
	 */
	if (xmlStrEqual(pucT,BAD_CAST".") && pccArg != NULL && pndArgSubst->doc != NULL) {
	  pcxpSubstResult->pucExt = resPathGetExtensionStr(BAD_CAST pndArgSubst->doc->URL);
	}
	else {
	  pcxpSubstResult->pucExt = resPathGetExtensionStr(pucT);
	}

	if (pcxpSubstResult->pucExt != NULL && pcxpSubstResult->eEncoding == rfc1738) {
	  pucT = pcxpSubstResult->pucExt;
	  pcxpSubstResult->pucExt = EncodeRFC1738(pcxpSubstResult->pucExt);
	  xmlFree(pucT);
	}
      }

      if ((pucT = domGetPropValuePtr(pndArgSubst, BAD_CAST "type")) != NULL) {
	/*
  	    this is a substitution with symbolic MIME type string
	 */
	if (xmlStrEqual(pucT,BAD_CAST".") && pccArg != NULL && pndArgSubst->doc != NULL) {
	  pucTT = resPathGetExtensionStr(BAD_CAST pndArgSubst->doc->URL);
	}
	else {
	  pucTT = resPathGetExtensionStr(pucT);
	}

	if (STR_IS_NOT_EMPTY(pucTT)) {
	  pcxpSubstResult->pucType = xmlStrdup(BAD_CAST resMimeGetTypeStr(resMimeGetTypeFromExt(pucTT)));
	}
	else { /* without extension directory is default */
	  pcxpSubstResult->pucType = xmlStrdup(BAD_CAST resMimeGetTypeStr(MIME_INODE_DIRECTORY));
	}
	xmlFree(pucTT);
      }

      /* this is a substitution with current time values */
      pcxpSubstResult->pucNow = GetNowFormatStr(domGetPropValuePtr(pndArgSubst, BAD_CAST "now"));
      /* no RFC 1738 encoding required */

      if ((pucT = domGetPropValuePtr(pndArgSubst, BAD_CAST "argv"))) {
	/* this is a substitution with program arguments */
	pcxpSubstResult->pucArgv = cxpCtxtCliGetFormat(pccArg, pucT);
      }

      if ((pucT = domGetPropValuePtr(pndArgSubst, BAD_CAST "env"))!=NULL) {
	/* this is a substitution with Environment variables */
	pcxpSubstResult->pucEnv = cxpCtxtEnvGetValueByName(pccArg,pucT);
      }

      if ((pucT = domGetPropValuePtr(pndArgSubst, BAD_CAST "host"))!=NULL) {
	/* this is a substitution with Host information variables */
	pcxpSubstResult->pucHost = cxpCtxtGetHostValueNamed(pccArg,pucT);
      }

      if ((pucT = domGetPropValuePtr(pndArgSubst, BAD_CAST "cgi"))) {
	/* there is an according named value, check access permission to value of "file" or "dir" and map the path */
	if ((pcxpSubstResult->pucCgi = cxpCtxtCgiGetValueByName(pccArg,pucT)) == NULL && (pcxpSubstResult->pucCgi = cxpCtxtCliGetValueByName(pccArg, pucT)) == NULL) {
	  cxpCtxtLogPrint(pccArg,2,"No value for '%s'",pucT);
	}
	else if (xmlStrEqual(pucT,BAD_CAST"dir") || xmlStrEqual(pucT,BAD_CAST"file") || xmlStrEqual(pucT,BAD_CAST"path")) {
	  resNodePtr prnTest;

	  if ((prnTest = resNodeRootNew(cxpCtxtRootGet(pccArg), pcxpSubstResult->pucCgi)) == NULL || resNodeIsReadable(prnTest) == FALSE) {
	    prnTest = resNodeListFindPath(cxpCtxtRootGet(pccArg), pcxpSubstResult->pucCgi, (RN_FIND_FILE | RN_FIND_IN_SUBDIR | RN_FIND_REGEXP));
	  }
	  
	  if (cxpCtxtAccessIsPermitted(pccArg,prnTest)) {
	    xmlFree(pcxpSubstResult->pucCgi);

	    if (domGetPropFlag(pndArgSubst, BAD_CAST "normalize", FALSE)) {
	      pcxpSubstResult->pucCgi = xmlStrdup(resNodeGetNameNormalized(prnTest));
	    }
	    else {
	      pcxpSubstResult->pucCgi = xmlStrdup(resNodeGetNameRelative(cxpCtxtRootGet(pccArg),prnTest)); // file access is checked, use CGI value without path
	    }
	    resPathChangeToSlashes(pcxpSubstResult->pucCgi);
	  }
	  else {
	    cxpCtxtLogPrint(pccArg,2,"No file '%s'",pcxpSubstResult->pucCgi);
	  }
	  resNodeFree(prnTest);
	}
	else {
	  //NormalizeStringNewLines((char *) pcxpSubstResult->pucCgi);
	}
      }

      if ((IS_NODE_SUBST(pndArgSubst) || IS_NODE_VARIABLE(pndArgSubst)) && pcxpSubstResult->pucDefault != NULL) {
	/*
	  detect type of value from default (XSLT: boolean, string or numeric)
	*/
	if (xmlStrEqual(pcxpSubstResult->pucDefault,BAD_CAST"true()") || xmlStrEqual(pcxpSubstResult->pucDefault,BAD_CAST"false()")) {
	  if ((pcxpSubstResult->pucTo != NULL && (xmlStrEqual(pcxpSubstResult->pucTo,BAD_CAST"true()") || xmlStrEqual(pcxpSubstResult->pucTo,BAD_CAST"false()")))
	      ||
	      ((pcxpSubstResult->pucCgi != NULL && (xmlStrEqual(pcxpSubstResult->pucCgi,BAD_CAST"true()") || xmlStrEqual(pcxpSubstResult->pucCgi,BAD_CAST"false()"))))) {
	    /* OK: value and default matching */
	  }
	  else {
	  }
	}
	else if (HasStringPairQuotes(pcxpSubstResult->pucDefault)) {
	  if (pcxpSubstResult->pucTo != NULL) {
	    if (HasStringPairQuotes(pcxpSubstResult->pucTo)) {
	      /* OK */
	    }
	    else {
	      pucT = pcxpSubstResult->pucTo;
	      pcxpSubstResult->pucTo = StringAddPairQuotesNew(pcxpSubstResult->pucTo,BAD_CAST"'");
	      xmlFree(pucT);
	    }
	  }
	  else if (pcxpSubstResult->pucEnv != NULL) {
	    if (HasStringPairQuotes(pcxpSubstResult->pucEnv)) {
	      /* OK */
	    }
	    else {
	      pucT = pcxpSubstResult->pucEnv;
	      pcxpSubstResult->pucEnv = StringAddPairQuotesNew(pcxpSubstResult->pucEnv,BAD_CAST"'");
	      xmlFree(pucT);
	    }
	  }
	  else if (pcxpSubstResult->pucCgi != NULL) {
	    if (HasStringPairQuotes(pcxpSubstResult->pucCgi)) {
	      /* OK */
	    }
	    else {
	      pucT = pcxpSubstResult->pucCgi;
	      pcxpSubstResult->pucCgi = StringAddPairQuotesNew(pcxpSubstResult->pucCgi,BAD_CAST"'");
	      xmlFree(pucT);
	    }
	  }
	}
	else {
	}
      }
      
      /*
       * decode values if necessary
       */
      if (pcxpSubstResult->eEncoding == base64) {
	if (pcxpSubstResult->pucDefault) {
	  pucT = pcxpSubstResult->pucDefault;
	  pcxpSubstResult->pucDefault = DecodeBase64(pcxpSubstResult->pucDefault);
	  xmlFree(pucT);
	}
	if (pcxpSubstResult->pucTo) {
	  pucT = pcxpSubstResult->pucTo;
	  pcxpSubstResult->pucTo = DecodeBase64(pcxpSubstResult->pucTo);
	  xmlFree(pucT);
	}
	if (pcxpSubstResult->pucCgi) {
	  pucT = pcxpSubstResult->pucCgi;
	  pcxpSubstResult->pucCgi = DecodeBase64(pcxpSubstResult->pucCgi);
	  xmlFree(pucT);
	}
	if (pcxpSubstResult->pucArgv) {
	  pucT = pcxpSubstResult->pucArgv;
	  pcxpSubstResult->pucArgv = DecodeBase64(pcxpSubstResult->pucArgv);
	  xmlFree(pucT);
	}
	if (pcxpSubstResult->pucEnv) {
	  pucT = pcxpSubstResult->pucEnv;
	  pcxpSubstResult->pucEnv = DecodeBase64(pcxpSubstResult->pucEnv);
	  xmlFree(pucT);
	}
	if (pcxpSubstResult->pucHost) {
	  pucT = pcxpSubstResult->pucHost;
	  pcxpSubstResult->pucHost = DecodeBase64(pcxpSubstResult->pucHost);
	  xmlFree(pucT);
	}
      }
      else if (pcxpSubstResult->eEncoding == rfc1738) {
	if (pcxpSubstResult->pucDefault) {
	  pucT = pcxpSubstResult->pucDefault;
	  pcxpSubstResult->pucDefault = EncodeRFC1738(pcxpSubstResult->pucDefault);
	  xmlFree(pucT);
	}
	if (pcxpSubstResult->pucTo) {
	  pucT = pcxpSubstResult->pucTo;
	  pcxpSubstResult->pucTo = EncodeRFC1738(pcxpSubstResult->pucTo);
	  xmlFree(pucT);
	}
	if (pcxpSubstResult->pucCgi) {
	  pucT = pcxpSubstResult->pucCgi;
	  pcxpSubstResult->pucCgi = EncodeRFC1738(pcxpSubstResult->pucCgi);
	  xmlFree(pucT);
	}
	if (pcxpSubstResult->pucArgv) {
	  pucT = pcxpSubstResult->pucArgv;
	  pcxpSubstResult->pucArgv = EncodeRFC1738(pcxpSubstResult->pucArgv);
	  xmlFree(pucT);
	}
	if (pcxpSubstResult->pucEnv) {
	  pucT = pcxpSubstResult->pucEnv;
	  pcxpSubstResult->pucEnv = EncodeRFC1738(pcxpSubstResult->pucEnv);
	  xmlFree(pucT);
	}
	if (pcxpSubstResult->pucHost) {
	  pucT = pcxpSubstResult->pucHost;
	  pcxpSubstResult->pucHost = EncodeRFC1738(pcxpSubstResult->pucHost);
	  xmlFree(pucT);
	}
      }

      if (pcxpSubstResult->pucCgi) {
	/* do a UTF-8 encoding after the rfc1738 or base64 decoding */
	pucT = pcxpSubstResult->pucCgi;
	pcxpSubstResult->pucCgi = plainDetectAndDecode((const unsigned char*)pcxpSubstResult->pucCgi, (int) strlen((const char*)pcxpSubstResult->pucCgi));
	xmlFree(pucT);
      }

      pcxpSubstResult->fReplaceInAttr = domGetPropFlag(pndArgSubst,BAD_CAST"attribute",FALSE);
#ifdef HAVE_CGI
      /* avoid huge log output */
#else
      cxpSubstPrint(pcxpSubstResult, pccArg);
#endif
    }
  }

  return pcxpSubstResult;
}
/* end of cxpSubstDetect() */


/*! \return TRUE if subtree of pndArg has to be skipped
 */
BOOL_T
cxpSubstSkip(xmlNodePtr pndArg)
{
  return (pndArg == NULL || pndArg->type == XML_PI_NODE || pndArg->type == XML_COMMENT_NODE || IS_NODE_META(pndArg));
} /* End of cxpSubstSkip() */


/*! Apply the pArgcxpSubst.

\param pArgcxpSubst the context to apply
\return a TRUE if successful
*/
BOOL_T
cxpSubstApply(xmlNodePtr pndArgTop, cxpSubstPtr pcxpSubstArg, cxpContextPtr pccArg)
{
  BOOL_T fResult = TRUE;

  if (pcxpSubstArg) {
#ifdef HAVE_PCRE2
    if (cxpSubstGetRegExp(pcxpSubstArg)) {
      if (cxpSubstGetPtr(pcxpSubstArg)) {
	ApplySubstRegExp(pndArgTop, cxpSubstGetRegExp(pcxpSubstArg), cxpSubstGetPtr(pcxpSubstArg), pcxpSubstArg->pPredicateSkip);
      }
      else {
      }
    }
    else {
      ApplySubstText(pndArgTop, cxpSubstGetNamePtr(pcxpSubstArg), cxpSubstGetPtr(pcxpSubstArg), pcxpSubstArg->pPredicateSkip);
    }
#else
    ApplySubstText(pndArgTop, cxpSubstGetNamePtr(pcxpSubstArg), cxpSubstGetPtr(pcxpSubstArg), pcxpSubstArg->pPredicateSkip);
#endif
  }
  else {
    fResult = FALSE;
  }

  return fResult;
} /* end of cxpSubstApply() */


/*! \return a pointer to name if successful or NULL
*/
xmlChar *
cxpSubstGetNamePtr(cxpSubstPtr pcxpSubstArg)
{
  xmlChar *pucResult = NULL;

  if ((pcxpSubstArg != NULL)
    &&
    ((pucResult = pcxpSubstArg->pucName))) {
    /* OK */
  }
  return pucResult;
}
/* end of cxpSubstGetNamePtr() */


/*! \return a pointer to subst value if successful or NULL
*/
xmlChar *
cxpSubstGetPtr(cxpSubstPtr pcxpSubstArg)
{
  xmlChar *pucResult = NULL;

  if ((pcxpSubstArg != NULL)
    &&
    ((pucResult = pcxpSubstArg->pucTo) != NULL
    || (pucResult = pcxpSubstArg->pucCgi) != NULL
    || (pucResult = pcxpSubstArg->pucArgv) != NULL
    || (pucResult = pcxpSubstArg->pucNow) != NULL
    || (pucResult = pcxpSubstArg->pucDir) != NULL
    || (pucResult = pcxpSubstArg->pucFile) != NULL
    || (pucResult = pcxpSubstArg->pucExt) != NULL
    || (pucResult = pcxpSubstArg->pucType) != NULL
    || (pucResult = pcxpSubstArg->pucFilename) != NULL
    || (pucResult = pcxpSubstArg->pucEnv) != NULL
    || (pucResult = pcxpSubstArg->pucHost) != NULL
    || (pucResult = pcxpSubstArg->pucProgram) != NULL
#ifdef HAVE_JS
    || (pucResult = pcxpSubstArg->pucScriptResult) != NULL
#endif
    || (pucResult = pcxpSubstArg->pucDefault) != NULL
    )) {
    /* OK */
  }
  return pucResult;
}
/* end of cxpSubstGetPtr() */


#ifdef HAVE_PCRE2

/*! \return a pointer to name if successful or NULL
*/
pcre2_code *
cxpSubstGetRegExp(cxpSubstPtr pcxpSubstArg)
{
  if (pcxpSubstArg) {
    return pcxpSubstArg->preFrom;
  }
  return NULL;
} /* end of cxpSubstGetRegExp() */

#endif


/*! prints all data of pcxpSubstArg.

\return a TRUE if successful
*/
BOOL_T
cxpSubstPrint(cxpSubstPtr pcxpSubstArg, cxpContextPtr pccArg)
{
  if (pcxpSubstArg) {
    xmlChar *pucT = NULL;

    if (pcxpSubstArg->pucName) {
      cxpCtxtLogPrint(pccArg, 1, "String Substitution '%s':", pcxpSubstArg->pucName);
    }
#if HAVE_PCRE2
    else if (pcxpSubstArg->preFrom) {
      cxpCtxtLogPrint(pccArg, 1, "Regexp Substitution");
    }
#endif
    else {
      cxpCtxtLogPrint(pccArg, 1, "No valid Substitution");
    }

    if (pcxpSubstArg->pucTo) {
      cxpCtxtLogPrint(pccArg,1,"\tpucTo       = '%s'",pcxpSubstArg->pucTo);
    }

    if (pcxpSubstArg->pucHost) {
      cxpCtxtLogPrint(pccArg,1,"\tpucHost      = '%s'",pcxpSubstArg->pucHost);
    }

    if (pcxpSubstArg->pucProgram) {
      cxpCtxtLogPrint(pccArg,1,"\tpucProgram      = '%s'",pcxpSubstArg->pucProgram);
    }

    if (pcxpSubstArg->pucEnv) {
      cxpCtxtLogPrint(pccArg,1,"\tpucEnv      = '%s'",pcxpSubstArg->pucEnv);
    }

    if (pcxpSubstArg->pucDir) {
      cxpCtxtLogPrint(pccArg,1,"\tpucDir      = '%s'",pcxpSubstArg->pucDir);
    }

    if (pcxpSubstArg->pucFile) {
      cxpCtxtLogPrint(pccArg,1,"\tpucFile     = '%s'",pcxpSubstArg->pucFile);
    }

    if (pcxpSubstArg->pucExt) {
      cxpCtxtLogPrint(pccArg, 1, "\tpucExt     = '%s'", pcxpSubstArg->pucExt);
    }

    if (pcxpSubstArg->pucFilename) {
      cxpCtxtLogPrint(pccArg,1,"\tpucFilename = '%s'",pcxpSubstArg->pucFilename);
    }

    if (pcxpSubstArg->pucType) {
      cxpCtxtLogPrint(pccArg,1,"\tpucType     = '%s'",pcxpSubstArg->pucType);
    }

    if (pcxpSubstArg->pucCgi) {
      pucT = xmlStrndup(pcxpSubstArg->pucCgi,128);
      cxpCtxtLogPrint(pccArg,1,"\tpucCgi      = '%s'",pucT);
      xmlFree(pucT);
    }

    if (pcxpSubstArg->pucArgv) {
      cxpCtxtLogPrint(pccArg,1,"\tpucArgv     = '%s'",pcxpSubstArg->pucArgv);
    }

    if (pcxpSubstArg->pucNow) {
      cxpCtxtLogPrint(pccArg,1,"\tpucNow      = '%s'",pcxpSubstArg->pucNow);
    }

#ifdef HAVE_JS
    if (pcxpSubstArg->pucScriptResult) {
      pucT = xmlStrndup(pcxpSubstArg->pucScriptResult,128);
      cxpCtxtLogPrint(pccArg,1,"\tpucScriptResult  = '%s'",pucT);
      xmlFree(pucT);
    }
#endif

    if (pcxpSubstArg->pucDefault) {
      cxpCtxtLogPrint(pccArg,1,"\tpucDefault  = '%s'",pcxpSubstArg->pucDefault);
    }
    
    return TRUE;
  }
  return FALSE;
}
/* end of cxpSubstPrint() */


/*! Frees pArgcxpSubst.

\param pArgcxpSubst the context to free
\return a TRUE if successful
*/
BOOL_T
cxpSubstFree(cxpSubstPtr pcxpSubstArg)
{
  if (pcxpSubstArg) {
    xmlFree(pcxpSubstArg->pucHost);
    xmlFree(pcxpSubstArg->pucProgram);
    xmlFree(pcxpSubstArg->pucEnv);
    xmlFree(pcxpSubstArg->pucDir);
    xmlFree(pcxpSubstArg->pucFile);
    xmlFree(pcxpSubstArg->pucExt);
    xmlFree(pcxpSubstArg->pucFilename);
    xmlFree(pcxpSubstArg->pucType);
    xmlFree(pcxpSubstArg->pucTo);
    xmlFree(pcxpSubstArg->pucCgi);
    xmlFree(pcxpSubstArg->pucDefault);
    xmlFree(pcxpSubstArg->pucArgv);
    xmlFree(pcxpSubstArg->pucNow);
    xmlFree(pcxpSubstArg->pucName);
#ifdef HAVE_JS
    xmlFree(pcxpSubstArg->pucScriptResult);
#endif
#ifdef HAVE_PCRE2
    pcre2_code_free(pcxpSubstArg->preFrom);
#endif

    //resNodeFree(pccArg->prnLocation);

    xmlFree(pcxpSubstArg);
    return TRUE;
  }
  return FALSE;
}
/* end of cxpSubstFree() */


/*! apply the substitution of 'pndArgSubst' on 'pndArgTop' or all child substitutions of 'pndArgTop' if 'pndArgSubst' is NULL

\param pndArgTop a xmlNodePtr to apply substitutions
\param pndArgSubst a xmlNodePtr to a subst node to apply
\param pccArg the context

\return TRUE if successful
 */
BOOL_T
cxpSubstInChildNodes(xmlNodePtr pndArgTop, xmlNodePtr pndArgSubst, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;

  if (IS_VALID_NODE(pndArgTop) == FALSE) {
    /* ignore non-valid elements */
  }
  else if (pndArgSubst == NULL) {
    /* subst nodes not yet detected */
    xmlNodePtr pndT;
    xmlNodePtr pndChild;
    xmlNodePtr pndNextChild;

    for (pndChild=pndArgTop->children; pndChild; pndChild=pndNextChild) {
      pndNextChild = pndChild->next;

      if (IS_NODE_SUBST(pndChild)) {
	cxpCtxtLogPrint(pccArg, 2, "New substitution found ''");
	fResult |= cxpSubstInChildNodes(pndArgTop, pndChild, pccArg);
      }
#ifdef HAVE_PETRINET
      else if (IS_NODE_PKG2_STATE(pndChild)
	|| IS_NODE_PKG2_TRANSITION(pndChild)
	|| IS_NODE_PKG2_REQUIREMENT(pndChild)
	) {
	fResult |= cxpSubstInChildNodes(pndChild, NULL, pccArg);
      }
#endif
    }
  }
  else if (IS_NODE_SUBST(pndArgSubst) && IS_VALID_NODE(pndArgSubst)) {
    cxpSubstPtr pcxpSubstT;

    if ((pcxpSubstT = cxpSubstDetect(pndArgSubst, pccArg))) {
      fResult = TRUE;
      pcxpSubstT->pPredicateSkip = cxpSubstSkip;

      if (
#ifdef HAVE_PCRE2
	cxpSubstGetRegExp(pcxpSubstT) != NULL ||
#endif
	cxpSubstGetNamePtr(pcxpSubstT) != NULL
	) {

	if (pndArgTop->parent != NULL && pndArgTop->parent->type == XML_DOCUMENT_NODE) {
	  if (IS_VALID_NODE(pndArgTop)) {
	    cxpSubstApply(pndArgTop, pcxpSubstT, pccArg);
	  }
	}
	else {
	  if (IS_VALID_NODE(pndArgSubst->next)) {
	    cxpSubstApply(pndArgSubst->next, pcxpSubstT, pccArg);
	  }
	}
	xmlUnlinkNode(pndArgSubst);
	xmlFreeNode(pndArgSubst);
      }
      cxpSubstFree(pcxpSubstT);
    }
  }
  return fResult;
} /* end of cxpSubstInChildNodes() */


/*! replace all found 'subst' children nodes of 'pndArg' by its result string

\param pndArgTop a xmlNodePtr to apply substitutions
\param pccArg the context

\return TRUE if successful
*/
BOOL_T
cxpSubstReplaceNodes(xmlNodePtr pndArg, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;

  if (IS_VALID_NODE(pndArg) == FALSE) {
    /* ignore non-valid elements */
  }
  else if (IS_NODE_SUBST(pndArg) && pndArg->children == NULL) {
    cxpSubstPtr pcxpSubstT;

    pcxpSubstT = cxpSubstDetect(pndArg, pccArg);
    if (pcxpSubstT) {
      xmlChar *pucT;
      
      if ((pucT = cxpSubstGetPtr(pcxpSubstT)) != NULL) { /* replace this subst node by result */
	xmlNodePtr pndReplace;

	if ((pndReplace = xmlNewText(pucT))) {
	  xmlReplaceNode(pndArg,pndReplace);
	  xmlFreeNode(pndArg);
	  fResult = TRUE;
	}
      }
      cxpSubstFree(pcxpSubstT);
    }
  }
  else {
    xmlNodePtr pndChild;
    xmlNodePtr pndNextChild;

    for (pndChild=pndArg->children; pndChild; pndChild=pndNextChild) {
      pndNextChild = pndChild->next;
      fResult |= cxpSubstReplaceNodes(pndChild, pccArg);
    }
  }
  return fResult;
} /* end of cxpSubstReplaceNodes() */


/*! Apply the pArgcxpSubst.

\param pArgcxpSubst the context to apply
\return a TRUE if successful
*/
xmlChar *
cxpSubstInStringNew(xmlChar *pucArg, cxpSubstPtr pcxpSubstArg, cxpContextPtr pccArg)
{
  xmlChar *pucResult = NULL;

  if (pcxpSubstArg) {
    pucResult = ReplaceStr(pucArg, cxpSubstGetNamePtr(pcxpSubstArg), cxpSubstGetPtr(pcxpSubstArg));
  }
  return pucResult;
} /* end of cxpSubstInStringNew() */


#ifdef TESTCODE
#include "test/test_cxp_subst.c"
#endif

