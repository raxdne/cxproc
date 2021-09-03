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

#include "basics.h"
#include "utils.h"
#include <res_node/res_node.h>
#include <cxp/cxp.h>
#include <cxp/cxp_dir.h>
#include "dom.h"
#include <pie/pie_text.h>
#include <pie/pie_calendar.h>
#include "plain_text.h"
#include <json/cxp_json.h>


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
      jsonParseBuffer(pndRoot, pndArgJson->children->content);
    }
    else {
      xmlNewChild(pndMeta,NULL,NAME_ERROR,BAD_CAST "Empty JSON node");
    }
  }
  else if ((prnFile = cxpResNodeResolveNew(pccArg,pndArgJson,NULL,CXP_O_READ)) != NULL) {
    if (resNodeGetMimeType(prnFile) == MIME_APPLICATION_JSON) { /* this is a JSON file (to read) */
      jsonParseFile(pndRoot,prnFile);
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

#if 1
#else
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
#endif

  return pdocResult;
}
/* end of jsonProcessJsonNode() */


#ifdef TESTCODE
#include "test/test_cxp_json.c"
#endif

