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


#include <libxml/tree.h>

#include "jsmn/jsmn.h"

#include "basics.h"
#include "utils.h"
#include <res_node/res_node.h>
#include <cxp/cxp.h>
#include <cxp/cxp_dir.h>
#include "dom.h"
#include <pie/pie_text.h>
#include <pie/pie_calendar.h>
#include "plain_text.h"
#include <json/json.h>


static int
jsonTransform(xmlNodePtr pndArgJson, const char *js, jsmntok_t *t, size_t count);


/*! process the JSON child instructions of pndMakePie
 */
xmlDocPtr
jsonProcessJsonNode(xmlNodePtr pndArgJson, cxpContextPtr pccArg)
{
  xmlDocPtr pdocResult = NULL;
  resNodePtr prnFile = NULL; /* filesystem context */
  xmlNodePtr pndMeta;
  //xmlNodePtr pndError;
  xmlNodePtr pndRoot;
  xmlNodePtr pndJson = NULL;
  xmlChar *pucAttrFile;
  xmlChar mpucT[BUFFER_LENGTH];
  xmlChar *pucContent = NULL;
  int iLength;

#ifdef DEBUG
  PrintFormatLog(1,"jsonProcessJsonNode(pndArgJson=%0x,pccArg=%0x)",pndArgJson,pccArg);
#endif

  assert(pndArgJson);

  pdocResult = xmlNewDoc(BAD_CAST "1.0");
  pdocResult->encoding = xmlStrdup(BAD_CAST "UTF-8");

  pndRoot = xmlNewNode(NULL,NAME_JSON);
  xmlDocSetRootElement(pdocResult,pndRoot);
  pndMeta = xmlNewChild(pndRoot,NULL,NAME_META,NULL);

  xmlAddChild(pndMeta,xmlCopyNode(pndArgJson,1));
  /* Get the current time. */
  domSetPropEat(pndMeta, BAD_CAST "ctime", GetNowFormatStr(BAD_CAST "%s"));
  domSetPropEat(pndMeta, BAD_CAST "ctime2", GetDateIsoString(0));

  if (IS_VALID_NODE(pndArgJson) == FALSE) {
    /* ignore NULL and invalid elements */
  }
  else if (IS_NODE_JSON(pndArgJson) == FALSE && IS_NODE_FILE(pndArgJson) == FALSE) {
  }
  else if (pndArgJson->children) {
    if (pndArgJson->children->content) {
      PrintFormatLog(2,"Reading JSON from node");
      pucContent = xmlStrdup(pndArgJson->children->content);
    }
    else {
      xmlNewChild(pndMeta,NULL,NAME_ERROR,BAD_CAST "Empty JSON node");
    }
  }
  else if ((prnFile = cxpResNodeResolveNew(pccArg,pndArgJson,NULL,CXP_O_READ)) != NULL) {
    if (resNodeGetMimeType(prnFile) == MIME_APPLICATION_JSON) { /* this is a JSON file (to read) */
      PrintFormatLog(2,"Reading JSON file '%s'", resNodeGetNameNormalized(prnFile));

      pucContent = plainGetContextTextEat(prnFile,1024);
    }
    else {
      PrintFormatLog(1,"JSON source not readable '%s'", pucAttrFile);
    }
    resNodeFree(prnFile);
  }
  else {
    /* no json make instructions */
    pndJson = xmlNewDocNode(pdocResult, NULL, NAME_JSON, NULL);
    xmlDocSetRootElement(pdocResult, pndJson);
    xmlSetProp(pndJson, BAD_CAST "class", BAD_CAST "empty");
  }

  if (pucContent != NULL && (iLength = xmlStrlen(pucContent)) > 0) {
    jsmn_parser p;
    jsmntok_t *tok;
    size_t tokcount = iLength;
    int r;

    /* Prepare parser */
    jsmn_init(&p);

    tok = (jsmntok_t *) xmlMalloc(sizeof(*tok) * tokcount);

    r = jsmn_parse(&p, (const char *)pucContent, iLength, tok, tokcount);
    if (r < 0) {
      if (r == JSMN_ERROR_NOMEM) {
	xmlFree(tok);
	return NULL;
      }
    } else {
      jsonTransform(pndRoot, (const char *)pucContent, tok, p.toknext);
    }
    xmlFree(tok);
  }
  xmlFree(pucContent);

  return pdocResult;
}
/* end of jsonProcessJsonNode() */


/*! 
 *
 * s. https://tools.ietf.org/html/rfc4627
 * s. https://en.wikipedia.org/wiki/JSON
 *
 */
int
jsonTransform(xmlNodePtr pndArgJson, const char *js, jsmntok_t *t, size_t count)
{

  if (count == 0) {
    return 0;
  }

  if (t->type == JSMN_PRIMITIVE) {
    xmlChar *pucT;
    xmlNodePtr pndNew;

    pucT = xmlStrndup(BAD_CAST (js + t->start), (t->end - t->start));
    if (pucT) {
      pndNew = xmlNewText(pucT);
      xmlAddChild(pndArgJson,pndNew);
      if (*pucT == 't' || *pucT == 'f') {
	xmlSetProp(pndArgJson,BAD_CAST "type",BAD_CAST "boolean");
	PrintFormatLog(2,"JSON new boolean '%s'", pucT);
      }
      if (isdigit(*pucT) || *pucT == '-') {
	xmlSetProp(pndArgJson,BAD_CAST "type",BAD_CAST "number");
	PrintFormatLog(2,"JSON new number '%s'", pucT);
      }
      xmlFree(pucT);
    }
    return 1;
  }
  else if (t->type == JSMN_STRING) {
    xmlChar *pucT;
    xmlNodePtr pndNew;

    pucT = xmlStrndup(BAD_CAST (js + t->start), (t->end - t->start));
    if (pucT) {
      pndNew = xmlNewText(pucT);
      xmlAddChild(pndArgJson,pndNew);
      xmlSetProp(pndArgJson,BAD_CAST "type",BAD_CAST "string");
      PrintFormatLog(2,"JSON new string '%s'", pucT);
      xmlFree(pucT);
    }
    return 1;
  }
  else if (t->type == JSMN_OBJECT) {
    xmlNodePtr pndObject;
    xmlNodePtr pndProperty;
    xmlChar *pucT;
    int i, j;
    int ciChilds;

    j = 0;
    pndObject = xmlNewChild(pndArgJson,NULL,BAD_CAST "anonymous",NULL);
    xmlSetProp(pndObject,BAD_CAST "type",BAD_CAST "object");
    ciChilds = t->size;
    PrintFormatLog(2,"JSON new object '%s' with %i properties", BAD_CAST "anonymous", ciChilds);

    for (i = 0; i < ciChilds; i++) {
      pucT = xmlStrndup(BAD_CAST (js + (t+1+j)->start), ((t+1+j)->end - (t+1+j)->start));
      if (pucT) {
	pndProperty = xmlNewChild(pndObject,NULL,pucT,NULL);
	j++;

	if ((t+1+j)->type == JSMN_OBJECT) {
	  int ciObjectChilds = (t+1+j)->size;
	  int h;

	  xmlSetProp(pndProperty,BAD_CAST "type",BAD_CAST "object");
	  PrintFormatLog(2,"JSON new property object '%s' with %i childs", pucT, ciObjectChilds);
	  for (h = 0; h < ciObjectChilds; ) {
	    xmlChar *pucTT;

	    pucTT = xmlStrndup(BAD_CAST (js + ((t+1+j)+1+h)->start), (((t+1+j)+1+h)->end - ((t+1+j)+1+h)->start));
	    if (pucTT) {
	      xmlNodePtr pndPropertyProperty;
	      pndPropertyProperty = xmlNewChild(pndProperty,NULL,pucTT,NULL);
	      xmlFree(pucTT);
	      j++;

	      h += jsonTransform(pndPropertyProperty, js, (t+1+j)+1+h, (count-j-h));
	    }
	  }
	  j += h + 1;
	}
	else if ((t+1+j)->type == JSMN_ARRAY) {
	  int h;
	  int ciArrayChilds = (t+1+j)->size;

	  xmlSetProp(pndProperty,BAD_CAST "type",BAD_CAST "array");
	  PrintFormatLog(2,"JSON new property array '%s' with %i childs", pucT, ciArrayChilds);
	  for (h = 0; h < ciArrayChilds; ) {
	    xmlNodePtr pndEntity;

	    pndEntity = xmlNewChild(pndProperty,NULL,BAD_CAST "entity",NULL);
	    h += jsonTransform(pndEntity, js, (t+1+j)+1+h, (count-j-h));
	  }
	  j += h + 1;
	}
	else {
	  j += jsonTransform(pndProperty, js, t+1+j, count-j);
	}
	xmlFree(pucT);
      }
    }
    return j+1;
  }
  else if (t->type == JSMN_ARRAY) {
    xmlNodePtr pndArray;
    xmlNodePtr pndNew;
    int i, j;
    int ciChilds;

    j = 0;
    pndArray = xmlNewChild(pndArgJson,NULL,BAD_CAST "anonymous",NULL);
    xmlSetProp(pndArray,BAD_CAST "type",BAD_CAST "array");
    ciChilds = t->size;
    PrintFormatLog(2,"JSON new array '%s' with %i childs", BAD_CAST "anonymous", ciChilds);
    for (i = 0; i < ciChilds; i++) {
      pndNew = xmlNewChild(pndArray,NULL,BAD_CAST "entity",NULL);
      j += jsonTransform(pndNew, js, t+1+j, count-j);
    }
    return j+1;
  }
  return 0;
}
/* end of jsonTransform() */


/*! Append detailed information about JSON content
*/
xmlNodePtr
jsonParseFile(xmlNodePtr pndParent, resNodePtr prnArg)
{
  xmlNodePtr pndResult = NULL;
  xmlDocPtr pdocT;
  cxpContextPtr pccT;
  
  /*! Read an JSON file.
  */
  if (resNodeIsFile(prnArg)) {
    PrintFormatLog(3,"Read JSON info from file '%s'",resNodeGetNameNormalized(prnArg));
  }
  else {
    PrintFormatLog(3,"Permission at JSON file '%s' denied",resNodeGetNameNormalized(prnArg));
    return pndResult;
  }

  assert(resNodeIsOpen(prnArg) == FALSE);

  pccT = cxpCtxtCliNew(-1,NULL,NULL);
  cxpCtxtLocationSet(pccT,prnArg);

  if (resNodeOpen(cxpCtxtLocationGet(pccT),"r") == FALSE) {
    xmlSetProp(pndParent,BAD_CAST "error",BAD_CAST "open");
    return pndResult;
  }
  
  if (pndParent) {
    pndResult = xmlNewChild(pndParent,NULL,NAME_JSON,NULL);
  }
  else {
    pndResult = xmlNewNode(NULL,NAME_JSON);
  }

  pdocT = jsonProcessJsonNode(pndResult, pccT);
  if (pdocT) {
    pndResult = xmlDocGetRootElement(pdocT);
    if (pndResult) {
      xmlUnlinkNode(pndResult);
      xmlAddChild(pndParent,pndResult);
    }
    xmlFreeDoc(pdocT);
  }

//  cxpCtxtIncrExitCode(pccTest,cxpCtxtGetExitCode(pccT));
  cxpCtxtFree(pccT);

  return pndResult;
}
/* end of jsonParseFile() */


#ifdef TESTCODE
#include "test/test_json.c"
#endif

