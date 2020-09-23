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
#include <libxml/uri.h>

#ifdef HAVE_PCRE2
#include <pcre2.h>
#endif

#include "basics.h"
#include "utils.h"
#include <res_node/res_node_io.h>
#include <cxp/cxp.h>
#include "dom.h"
#include <cxp/cxp_dir.h>
#include <pie/pie_text.h>
#include <script/script.h>


duk_context *pDukContext = NULL;

static void
scriptCleanup(void);

static void
getXmlBody(xmlChar *pucSource, int *piA, int *piB);

static void
scriptErrorMsg(void *udata, const char *msg);


/* 
   http://wiki.duktape.org/HowtoFatalErrors.html
 */
void
scriptErrorMsg(void *udata, const char *msg)
{
  (void) udata;  /* ignored in this case, silence warning */

  /* Note that 'msg' may be NULL. */
  fprintf(stderr, "*** FATAL ERROR: %s\n", (msg ? msg : "no message"));
  fflush(stderr);
  abort();
}
/* end of scriptErrorMsg() */


/*! substitutes

\param ppucScript pointer to array of string pointers
\param pndArg

\return
 */
BOOL_T
scriptChangeParam(xmlChar **ppucScript, xmlNodePtr pndArg, cxpContextPtr pccArg)
{
  BOOL_T fResult = TRUE;
  xmlChar *pucScript;

  assert(ppucScript);
  pucScript = *ppucScript;
  assert(pucScript);

  if (xmlStrlen(pucScript)) {
    char **ppchParam;

    ppchParam = (char **)cxpXslParamProcess(pndArg, NULL);
    if (ppchParam) {
      index_t i;
      for (i = 0; ppchParam[i]; i += 2) {	/* all name/value pairs */
	/*!\todo: extract code to cxpChangeJsParam() s. cxpChangeXslParam */
	size_t erroroffset;
	int errornumber;
	int rc;
	pcre2_match_data *match_data_var;
	pcre2_match_data *match_data_type;
	pcre2_code *re_var = NULL;
	pcre2_code *re_type;
	xmlChar mpucValue[BUFFER_LENGTH];
	xmlChar mpucVariable[BUFFER_LENGTH];
	xmlChar mpucRegExp[BUFFER_LENGTH];
	xmlChar *pucContentNew = NULL;

	mpucValue[0] = '\0';
	/*
	   detect type of variable
	   */
	re_type = pcre2_compile(
	  (PCRE2_SPTR8)"^[ \\t]*[0-9]+(\\.[0-9]*)*[ \\t]*$", PCRE2_ZERO_TERMINATED,
	  0, &errornumber, &erroroffset, NULL);

	if (re_type == NULL) {
	  /* regexp error handling */
	  cxpCtxtLogPrint(pccArg,1, "Script variable regexp '%s' error: '%i'", "", errornumber);
	  fResult = FALSE;
	}

	match_data_type = pcre2_match_data_create_from_pattern(re_type, NULL);
	rc = pcre2_match(
	  re_type,        /* result of pcre2_compile() */
	  (PCRE2_SPTR8)ppchParam[i + 1],  /* the subject string */
	  PCRE2_ZERO_TERMINATED,             /* the length of the subject string */
	  0,              /* start at offset 0 in the subject */
	  0,              /* default options */
	  match_data_type,        /* vector of integers for substring information */
	  NULL);            /* number of elements (NOT size in bytes) */

	if (rc > 0) {
	  PCRE2_SIZE *ovector;

	  ovector = pcre2_get_ovector_pointer(match_data_type);
	  if (ovector[1] - ovector[0] > 0) {
	    xmlStrPrintf(mpucValue, BUFFER_LENGTH, "%s", ppchParam[i + 1]);
	    cxpCtxtLogPrint(pccArg,3, "value is a number '%s'", mpucValue);
	  }
	  else {
	    int iA;
	    int iB;
	    int j;
	    int k;
	    getXmlBody(BAD_CAST ppchParam[i + 1], &iA, &iB);
	    if (iA > 0 && iB > iA) {
	      for (k = 0, j = 0; j <= (iB - iA) && j < BUFFER_LENGTH; j++) {
		switch (ppchParam[i + 1][iA + j]) {
		case '\n':
		case '\r':
		  break;
		case '\t':
		  mpucValue[k] = ' ';
		  k++;
		  break;
		default:
		  mpucValue[k] = ppchParam[i + 1][iA + j];
		  k++;
		}
	      }
	      mpucValue[k] = '\0';
	      cxpCtxtLogPrint(pccArg,3, "value is XML '%s'", mpucValue);
	    }

	    if (mpucValue[0] == '\0') {
	      /*
		 neither Number nor XML found, its a String now
		 */
	      char *pchSource;

	      mpucValue[0] = '\"';
	      for (k = 0, j = 1, pchSource = ppchParam[i + 1];
		pchSource[k] && k < BUFFER_LENGTH && j < BUFFER_LENGTH;
		k++) {
		switch (pchSource[k]) {
		case '\n':
		  mpucValue[j] = '\\';
		  j++;
		  mpucValue[j] = 'n';
		  j++;
		  break;
		case '\r':
		  break;
		default:
		  mpucValue[j] = pchSource[k];
		  j++;
		}
	      }
	      mpucValue[j] = '\"';
	      mpucValue[j + 1] = '\0';
	      cxpCtxtLogPrint(pccArg,3, "value is a String '%s'", mpucValue);
	    }
	  }

	  xmlStrPrintf(mpucRegExp, BUFFER_LENGTH, "var +%s *=[ \\t]*([^\\\";]+|[ \\t\\n]*<.+>[ \\t\\n]*|\\\"[^\\\"]+\\\")[ \\t]*;", ppchParam[i]);

	  cxpCtxtLogPrint(pccArg,3, "Initialize Variable regexp '%s'", mpucRegExp);

	  re_var = pcre2_compile(
	    (PCRE2_SPTR8)mpucRegExp, PCRE2_ZERO_TERMINATED,
	    0, &errornumber, &erroroffset, NULL);

	  if (re_var == NULL) {
	    /* regexp error handling */
	    cxpCtxtLogPrint(pccArg,1, "Script variable regexp '%s' error: '%i'", &mpucRegExp[erroroffset], errornumber);
	    fResult = FALSE;
	  }

	  match_data_var = pcre2_match_data_create_from_pattern(re_var, NULL);
	  rc = pcre2_match(
	    re_var,        /* result of pcre2_compile() */
	    (PCRE2_SPTR8)pucScript,  /* the subject string */
	    PCRE2_ZERO_TERMINATED,             /* the length of the subject string */
	    0,              /* start at offset 0 in the subject */
	    0,              /* default options */
	    match_data_var,        /* vector of integers for substring information */
	    NULL);            /* number of elements (NOT size in bytes) */

	  if (rc > 0) {
	    ovector = pcre2_get_ovector_pointer(match_data_var);
	    if (ovector[1] - ovector[0] > 3 && ovector[3] - ovector[2] > 0) {
	      xmlChar *pucRelease;

	      pucRelease = xmlStrndup(pucScript + ovector[2], ovector[3] - ovector[2]);
	      cxpCtxtLogPrint(pccArg,3, "var '%s' (%i..%i) in '%s'", pucRelease, ovector[0], ovector[1], pucScript);
	      xmlFree(pucRelease);

	      xmlStrPrintf(mpucVariable, BUFFER_LENGTH, "var %s = %s; // cxproc changed\n", ppchParam[i], mpucValue);
	      pucContentNew = xmlStrndup(pucScript, ovector[0]);
	      pucRelease = pucContentNew;
	      pucContentNew = xmlStrncatNew(pucContentNew, mpucVariable, -1);
	      xmlFree(pucRelease);
	      pucRelease = pucContentNew;
	      pucContentNew = xmlStrncatNew(pucContentNew, pucScript + ovector[1], -1);
	      xmlFree(pucRelease);
	    }
	  }
	  else {
	    xmlChar *pucRelease;
	    xmlStrPrintf(mpucVariable, BUFFER_LENGTH, "var %s = %s; // cxproc inserted\n", ppchParam[i], mpucValue);

	    cxpCtxtLogPrint(pccArg,2, "Insert Variable '%s'", mpucVariable);
	    pucContentNew = pucRelease = xmlStrdup(mpucVariable);
	    pucContentNew = xmlStrncatNew(pucContentNew, pucScript, -1);
	    xmlFree(pucRelease);
	  }
	  /* cxpCtxtLogPrint(pccArg,4,"New script var '%s'",pucContentNew); */
	  pcre2_match_data_free(match_data_var);   /* Release memory used for the match */
	  pcre2_code_free(re_var);
	  xmlFree(pucScript);
	  pucScript = pucContentNew;
	  *ppucScript = pucContentNew;
	}
	pcre2_match_data_free(match_data_type);   /* Release memory used for the match */
	pcre2_code_free(re_type);
	cxpXslParamFree(ppchParam);
      }
    }
  }
  return fResult;
}
/* end of scriptChangeParam() */


/*! tries to find an XML tagged body at pucSource and sets *piA to
    index of first element '<' and *piB to last '>' of last element.

\param pucSource
\param piA
\param piB

 */
void
getXmlBody(xmlChar *pucSource, int *piA, int *piB)
{
  int i;
  int iDepth;

  for (*piA=-1, *piB=-1, i=0, iDepth=0; pucSource[i]!='\0'; i++) {

    if (pucSource[i]=='<') {
      /* this is a tag */
      if (pucSource[i+1]=='/') {
	/* this is a closing tag */
	for (i++; pucSource[i]!='\0' && pucSource[i]!='>'; i++) {}
	*piB = i;
	iDepth--;
      }
      else if (isalpha(pucSource[i+1])) {
	/* this is a opening tag */
	if (*piA==-1) {
	  /* first element */
	  *piA = i;
	}
	for ( i+=2; pucSource[i]!='\0'; i++) {
	  /* search for closing '>' */
	  if (pucSource[i]=='>') {
	    if (i>0) {
	      if (pucSource[i-1]=='/') {
		*piB = i;
	      }
	      else {
		iDepth++;
	      }
	    }
	    break;
	  }
	}
      }
      else {
	/* processing instruction or comment */
	for ( ; pucSource[i]!='\0' && pucSource[i]!='>'; i++) {
	  /* search for closing '>' */
	}
      }
    }
  }
  if (*piA>-1 && *piA < *piB && iDepth==0) {
    /* OK */
  }
  else {
    *piB = -1;
  }
}
/* end of getXmlBody() */


/*! process the code of script attribute

\deprecated only in cxp:subst required?

\param pndArg
\param pccArg

\return pointer to script result string or NULL in case of error
 */
xmlChar *
_scriptProcessScriptAttribute(xmlNodePtr pndArg, cxpContextPtr pccArg)
{
  xmlChar *pucResult = NULL;
  xmlChar *pucAttrScript;

  pucAttrScript = domGetAttributePtr(pndArg,BAD_CAST "script");
  if (pucAttrScript) {
    cxpCtxtLogPrint(pccArg,4, "Run Script code '%s'",pucAttrScript);
    duk_push_global_object(pDukContext);
    duk_push_string(pDukContext,(const char*)pucAttrScript);
    if (duk_peval(pDukContext) != 0) {
      cxpCtxtLogPrint(pccArg,1,"Script error: %s", duk_safe_to_string(pDukContext, -1));
    }
    pucResult = xmlStrdup(BAD_CAST duk_to_string(pDukContext, -1));
    duk_pop(pDukContext);  /* pop result/error */
  }
  return pucResult;
}
/* end of _scriptProcessScriptAttribute() */


/*! process the code of script node

\param pndArg
\param pccArg

\return
 */
xmlChar *
scriptProcessScriptNode(xmlNodePtr pndArg, cxpContextPtr pccArg)
{
  xmlChar *pucResult = NULL;
  char *pchFilenameFound = NULL;
  xmlChar *pucAttrFile;
  xmlChar *pucContent;
  xmlChar *pucScript = NULL;
  xmlNodePtr pndChildPlain;
  cxpContextPtr pccHere;
  xmlChar *pucAttrNameCacheAs;

#ifdef DEBUG
  cxpCtxtLogPrint(pccArg,1,"scriptProcessScriptNode(pndArg=%0x,pccArg=%0x)",pndArg,pccArg);
#endif

  if (IS_VALID_NODE(pndArg) == FALSE) {
    /* ignore NULL and invalid elements */
  }
  else if (IS_NODE_SCRIPT(pndArg)
    || (xmlStrEqual(domGetAttributePtr(pndArg, BAD_CAST "type"), BAD_CAST"script") && (IS_NODE_PIE_IMPORT(pndArg) || IS_NODE_PIE_BLOCK(pndArg)))) {
    BOOL_T fCache = FALSE;
    //BOOL_T fSearch;

    pucAttrFile        = domGetAttributePtr(pndArg,BAD_CAST "name");
    pucAttrNameCacheAs = domGetAttributePtr(pndArg,BAD_CAST "cacheas");
    fCache             = domGetAttributeFlag(pndArg,BAD_CAST "cache",  FALSE);
    //fSearch            = domGetAttributeFlag(pndArg,BAD_CAST "search",FALSE);

    pccHere = cxpCtxtFromAttr(pccArg,pndArg);

    if (pucAttrFile != NULL && xmlStrlen(pucAttrFile) > 0) {
      /*
      element without children, read the attribute named file
       */
      resNodePtr prnFile = NULL;

      if ((pucScript = cxpCtxtCacheGetBuffer(pccArg, pucAttrFile)) != NULL) {
	/* there is a cached Buffer */
	pucScript = xmlStrdup(pucScript);
      }
      else if ((prnFile = cxpResNodeResolveNew(pccArg, pndArg, NULL, CXP_O_READ)) != NULL) {
        if (resNodeReadStatus(prnFile)) { /* this is a Javascript file (to read) */
          cxpCtxtLogPrint(pccArg,2,"Reading Javascript file '%s'", resNodeGetNameNormalized(prnFile));

          pucScript = plainGetContextTextEat(prnFile,1024);

          if (xmlStrlen(pucAttrNameCacheAs) > 0) {
	    cxpCtxtCacheAppendBuffer(pccArg, pucScript, pucAttrNameCacheAs);
	  }
	  else if (fCache && xmlStrlen(resNodeGetNameNormalized(prnFile)) > 0) {
	    cxpCtxtCacheAppendBuffer(pccArg, pucScript, resNodeGetNameNormalized(prnFile));
	  }
        }
        else {
          cxpCtxtLogPrint(pccArg,1,"Javascript source not readable '%s'", pucAttrFile);
        }
        resNodeFree(prnFile);
      }
    }
    else if ((pndChildPlain = domGetFirstChild(pndArg,NAME_PLAIN))) {
      cxpCtxtLogPrint(pccArg,2, "Process Script code from plain");
      pucScript = cxpProcessPlainNode(pndChildPlain,NULL);
    }
    else if ((pucContent = domNodeGetContentPtr(pndArg)) != NULL && xmlStrlen(pucContent) > 0) {
      cxpCtxtLogPrint(pccArg,2, "Process Script code from text node");
      pucScript = xmlStrdup(pucContent);
    }

    if (STR_IS_NOT_EMPTY(pucScript) && domGetAttributeFlag(pndArg, BAD_CAST "eval", TRUE)) {
      xmlChar *pucScriptDecoded;

      pucScriptDecoded = xmlStrdup(pucScript); /*!\bug decode XML entities to UTF-8 */

      cxpCtxtLogPrint(pccArg, 4, "Run Script code '%s'", pucScriptDecoded);
      duk_push_global_object(pDukContext);
      duk_push_string(pDukContext, (const char*)pucScriptDecoded);
      if (duk_peval(pDukContext) != 0) {
	cxpCtxtLogPrint(pccArg, 1, "Script error: %s", duk_safe_to_string(pDukContext, -1));
	pucResult = xmlStrdup(BAD_CAST duk_safe_to_string(pDukContext, -1));
      }
      else if (duk_check_type(pDukContext, -1, DUK_TYPE_NUMBER) || duk_check_type(pDukContext, -1, DUK_TYPE_STRING)) {
	pucResult = xmlStrdup(BAD_CAST duk_to_string(pDukContext, -1));
      }
      else {
	cxpCtxtLogPrint(pccArg, 1, "Script: undefined result");
	pucResult = xmlStrdup(BAD_CAST "");
      }
      duk_pop(pDukContext);  /* pop result/error */
      xmlFree(pucScriptDecoded);
    }
    /*!\todo keep script code after eval as display attribute or title */

    xmlFree(pucScript);
    xmlFree(pchFilenameFound);
    if (pccHere != pccArg) {
      cxpCtxtIncrExitCode(pccArg, cxpCtxtGetExitCode(pccHere));
      //cxpCtxtFree(pccHere);
    }
  }
  else {
    cxpCtxtLogPrint(pccArg,1, "This is not a Script node '%s'",pndArg->name);
  }
  return pucResult;
}
/* end of scriptProcessScriptNode() */


/*!

\param pndParent pointer to node to add info attributes

\return pointer to parent node
 */
xmlNodePtr
scriptInfo(xmlNodePtr pndParent) 
{
  xmlChar mpucOut[BUFFER_LENGTH];

  xmlSetProp(pndParent,BAD_CAST "name",BAD_CAST"Duktape/JS");
  xmlSetProp(pndParent,BAD_CAST "ns",BAD_CAST"http://www.duktape.org/");
  xmlSetProp(pndParent,BAD_CAST "select", BAD_CAST "yes");
  xmlStrPrintf(mpucOut,BUFFER_LENGTH, "%li.%li.%li", (DUK_VERSION / 10000), (DUK_VERSION % 10000)/100, (DUK_VERSION % 10000) % 100);
  xmlSetProp(pndParent,BAD_CAST "version",mpucOut);

  return pndParent;
}
/* end of scriptInfo() */


/*! initialize this script module

\return TRUE if no script support at all or init successful
 */
BOOL_T
scriptInit(cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;

  if (pDukContext == NULL) {
    pDukContext = duk_create_heap(NULL, NULL, NULL, NULL, scriptErrorMsg);
    if (pDukContext) {
      fResult = TRUE;
    }
    else {
      cxpCtxtLogPrint(pccArg,1,"Failed to create a Duktape heap.");
    }

    /* register for exit() */
    if (atexit(scriptCleanup) != 0) {
      exit(EXIT_FAILURE);
    }
  }
  return fResult;
}
/* end of scriptInit() */


/*! cleanup this script module
*/
void
scriptCleanup(void)
{
  if (pDukContext) {
    duk_destroy_heap(pDukContext);
  }
}
/* end of scriptCleanup() */


#ifdef TESTCODE
#include "test/test_script.c"
#endif
