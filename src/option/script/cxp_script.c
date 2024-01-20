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

/*!\todo new classes
        Cxproc
            Cxproc.new();
            Cxproc.parseXML("abc.xml");
            Cxproc.transform("def.xsl").variable("str_test","AAAA");
            var strXML = Cxproc.serialize();
            Cxproc.run()
            map method calls to an internal cxp:make
        File
            var objInput = File.new("test.txt").search("yes");
            print objInput.getContent();
        Locator
            getNameNormalized()
            getNameNative()
            Mime
        Env
            print Env.getValue('TMP');
            Env.setValue('TMP','c:\tmp');
        Cgi
            print Cgi.getValue('path');
        Date
        XML
        Plain
        Xhtml
        Pathtable
        Database
        Image

example
    Interpret script source
    iterations
    print Env.getValue('TMP');
    Env.setValue('TMP','c:\tmp');

Transform XML source
    var strCgiPath = Cgi.getValue('path');

    if (strCgiPath == undef) {
      //
    } else {
      var cxpT = new Cxproc();

      cxpT.parseXML(strCgiPath);
      cxpT.transform("def.xsl").variable("str_test","AAAA");

      print cxpT.serialize();
    }

output of plain text file
    var objInput = File.new("test.txt").search("yes");
    print objInput.getContent();

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


static BOOL_T
scriptInit(cxpContextPtr pccArg);

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


/*! process the code of script text

\param pndArg
\param pccArg

\return pointer to script result string or NULL in case of error
 */
xmlChar *
cxpScriptProcessText(xmlChar *pucArg, cxpContextPtr pccArg)
{
  xmlChar *pucResult = NULL;

  if (STR_IS_NOT_EMPTY(pucArg) && scriptInit(pccArg)) {
    xmlChar *pucScriptDecoded;

    pucScriptDecoded = xmlStrdup(pucArg); /*!\bug decode XML entities to UTF-8 StringDecodeXmlDefaultEntitiesNew(pucScript) */
    cxpCtxtLogPrint(pccArg, 4, "Run Script code '%s'", pucScriptDecoded);
    duk_push_global_object(pccArg->pDukContext);
    duk_push_string(pccArg->pDukContext, (const char *)pucScriptDecoded);
    if (duk_peval(pccArg->pDukContext) != 0) {
      cxpCtxtLogPrint(pccArg, 1, "Script error: %s", duk_safe_to_string(pccArg->pDukContext, -1));
      pucResult = xmlStrdup(BAD_CAST duk_safe_to_string(pccArg->pDukContext, -1));
    }
    else if (duk_check_type(pccArg->pDukContext, -1, DUK_TYPE_NUMBER) || duk_check_type(pccArg->pDukContext, -1, DUK_TYPE_STRING)) {
      pucResult = xmlStrdup(BAD_CAST duk_to_string(pccArg->pDukContext, -1));
    }
    else {
      cxpCtxtLogPrint(pccArg, 1, "Script: undefined result");
      pucResult = xmlStrdup(BAD_CAST "");
    }
    duk_pop(pccArg->pDukContext); /* pop result/error */
    xmlFree(pucScriptDecoded);
  }
  else {
    cxpCtxtLogPrint(pccArg, 1, "Empty Script code");
  }
  return pucResult;
} /* end of cxpScriptProcessText() */


/*! process the code of script node

\param pndArg
\param pccArg

\return
 */
xmlChar *
cxpScriptProcessNode(xmlNodePtr pndArg, cxpContextPtr pccArg)
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
  cxpCtxtLogPrint(pccArg,1,"cxpScriptProcessNode(pndArg=%0x,pccArg=%0x)",pndArg,pccArg);
#endif

  if (IS_VALID_NODE(pndArg) == FALSE) {
    /* ignore NULL and invalid elements */
  }
  else if (IS_NODE_SCRIPT(pndArg)
#ifdef HAVE_PIE
    || (xmlStrEqual(domGetPropValuePtr(pndArg, BAD_CAST "type"), BAD_CAST "script") && (IS_NODE_PIE_IMPORT(pndArg) || IS_NODE_PIE_BLOCK(pndArg)))
#endif
    ) {
    BOOL_T fCache = FALSE;
    //BOOL_T fSearch;

    pucAttrFile        = domGetPropValuePtr(pndArg,BAD_CAST "name");
    pucAttrNameCacheAs = domGetPropValuePtr(pndArg,BAD_CAST "cacheas");
    fCache             = domGetPropFlag(pndArg,BAD_CAST "cache",  FALSE);
    //fSearch            = domGetPropFlag(pndArg,BAD_CAST "search",FALSE);

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

    pucResult = cxpScriptProcessText(pucScript, pccArg);

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
/* end of cxpScriptProcessNode() */


/*!

\param pndParent pointer to node to add info attributes

\return pointer to parent node
 */
xmlNodePtr
cxpScriptInfo(xmlNodePtr pndParent, cxpContextPtr pccArg) 
{
  xmlChar mpucOut[BUFFER_LENGTH];

  xmlSetProp(pndParent,BAD_CAST "name",BAD_CAST"Duktape/JS");
  xmlSetProp(pndParent,BAD_CAST "ns",BAD_CAST"http://www.duktape.org/");
  xmlSetProp(pndParent,BAD_CAST "select", BAD_CAST "yes");
  xmlStrPrintf(mpucOut,BUFFER_LENGTH, "%li.%li.%li", (DUK_VERSION / 10000), (DUK_VERSION % 10000)/100, (DUK_VERSION % 10000) % 100);
  xmlSetProp(pndParent,BAD_CAST "version",mpucOut);

  return pndParent;
}
/* end of cxpScriptInfo() */


/*! initialize this script module

\return TRUE if no script support at all or init successful
 */
BOOL_T
scriptInit(cxpContextPtr pccArg)
{
  BOOL_T fResult = TRUE;

  if (pccArg->pDukContext == NULL) {
    pccArg->pDukContext = duk_create_heap(NULL, NULL, NULL, NULL, scriptErrorMsg);
    if (pccArg->pDukContext) {
      cxpCtxtLogPrint(pccArg,1,"New Duktape heap.");
    }
    else {
      cxpCtxtLogPrint(pccArg,1,"Failed to create a Duktape heap.");
      fResult = FALSE;
    }
  }
  return fResult;
}
/* end of scriptInit() */


#ifdef TESTCODE
#include "test/test_cxp_script.c"
#endif
