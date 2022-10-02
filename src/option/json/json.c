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


#include <libxml/tree.h>

#include "basics.h"
#include "utils.h"
#include <res_node/res_node_io.h>
#include "dom.h"
#include "plain_text.h"

#include "jsmn/jsmn.h"
#include <json/json.h>

extern int
jsonTransform(xmlNodePtr pndArgJson, const char *js, jsmntok_t *t, size_t count);


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
    int c, i, j;
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
	  for (h = 0, c = 0; c < ciObjectChilds; c++) {
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
	  for (h = 0, c = 0; c < ciArrayChilds; c++) {
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
    int c, i, j;
    int ciChilds;

    j = 0;
    pndArray = xmlNewChild(pndArgJson,NULL,BAD_CAST "anonymous",NULL);
    xmlSetProp(pndArray,BAD_CAST "type",BAD_CAST "array");
    ciChilds = t->size;
    PrintFormatLog(2,"JSON new array '%s' with %i childs", BAD_CAST "anonymous", ciChilds);
    for (c = 0; c < ciChilds; c++) {
      pndNew = xmlNewChild(pndArray,NULL,BAD_CAST "entity",NULL);
      j += jsonTransform(pndNew, js, t+1+j, count-j);
    }
    return j+1;
  }
  return 0;
}
/* end of jsonTransform() */


/*! \return a DOM for parsed buffer
*/
BOOL_T
jsonParseBuffer(xmlNodePtr pndParent, xmlChar *pucArg)
{
  BOOL_T fResult = FALSE;
  int iLength;
  
  if (pucArg != NULL && (iLength = xmlStrlen(pucArg)) > 0) {
    jsmn_parser p;
    jsmntok_t *tok;
    size_t tokcount = iLength;

    /* Prepare parser */
    jsmn_init(&p);

    tok = (jsmntok_t *) xmlMalloc(sizeof(*tok) * tokcount);
    if (tok) {
      int r;

      r = jsmn_parse(&p, (const char *)pucArg, iLength, tok, tokcount);
      if (r < 0) {
	if (r == JSMN_ERROR_NOMEM) {
	  xmlFree(tok);
	  return FALSE;
	}
      }
      else {
	jsonTransform(pndParent, (const char *)pucArg, tok, p.toknext);
      }
      xmlFree(tok);
    }
  }
  return fResult;
} /* end of jsonParseBuffer() */


/*! Append detailed information about JSON content
*/
xmlNodePtr
jsonParseFile(xmlNodePtr pndArgFile, resNodePtr prnArg)
{
  xmlNodePtr pndResult = NULL;
  xmlDocPtr pdocT;
  xmlChar *pucContent = NULL;
  
  /*! Read an JSON file.
  */
  if (resNodeIsFile(prnArg)) {
    PrintFormatLog(3,"Read JSON info from file '%s'",resNodeGetNameNormalized(prnArg));
  }
  else {
    PrintFormatLog(3,"Permission at JSON file '%s' denied",resNodeGetNameNormalized(prnArg));
    return pndResult;
  }

#if 0
  assert(resNodeIsOpen(prnArg) == FALSE);

  if (resNodeOpen(prnArg,"r") == FALSE) {
    xmlSetProp(pndParent,BAD_CAST "error",BAD_CAST "open");
    return pndResult;
  }
  
  if (pndParent) {
    pndResult = xmlNewChild(pndParent,NULL,NAME_JSON,NULL);
  }
  else {
    pndResult = xmlNewNode(NULL,NAME_JSON);
  }
#endif

#if 1
  PrintFormatLog(2,"Reading JSON file '%s'", resNodeGetNameNormalized(prnArg));

  pucContent = plainGetContextTextEat(prnArg,1024);
  pndResult = xmlNewChild(pndArgFile,NULL,BAD_CAST"json",NULL);
  if (pndResult) {
    jsonParseBuffer(pndResult, pucContent);
  }
  xmlFree(pucContent);
#else
  pdocT = jsonProcessJsonNode(pndResult, pccT);
  if (pdocT) {
    pndResult = xmlDocGetRootElement(pdocT);
    if (pndResult) {
      xmlUnlinkNode(pndResult);
      xmlAddChild(pndParent,pndResult);
    }
    xmlFreeDoc(pdocT);
  }
#endif
  
  return pndResult;
} /* end of jsonParseFile() */


#ifdef TESTCODE
#include "test/test_json.c"
#endif

