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

#define WITH_AJAX 1

#include "dom.h"

#include "cxp_dtd.h"
#include "cxp_context.h"
#include "cxp_context_cache.h"
#include "cxp_context_enc.h"
#include "cxp_subst.h"

#define CXP_O_NONE   (0)

#define CXP_O_READ   (1)

#define CXP_O_SEARCH (3)

#define CXP_O_WRITE  (4)

#define CXP_O_FILE   (8)

#define CXP_O_DIR    (16)


extern BOOL_T
cxpProcessTransformations(const xmlDocPtr pdocArgXml, const xmlNodePtr pndArgParent, xmlDocPtr *ppdocArgResult, xmlChar **ppucArgResult, cxpContextPtr pccArg);

extern xmlChar **
cxpXslParamProcess(xmlNodePtr pndArg, cxpContextPtr pccArg);

extern void
cxpXslParamFree(char ** ppchArg);

extern xmlDocPtr
cxpXslRetrieve(const xmlNodePtr pndArgXsl, cxpContextPtr pccArg);

extern void
cxpProcessMakeNode(xmlNodePtr pndArg, cxpContextPtr pccArg);

extern xmlDocPtr
cxpProcessXmlNode(xmlNodePtr pndArg, cxpContextPtr pccArg);

extern BOOL_T
cxpProcessCopyNode(xmlNodePtr pndArgCopy, cxpContextPtr pccArg);

extern xmlNodePtr
cxpInfoProgram(xmlNodePtr pndArg, cxpContextPtr pccArg);

extern xmlDocPtr
cxpProcessInfoNode(xmlNodePtr pndInfo, cxpContextPtr pccArg);

extern resNodePtr
cxpResNodeResolveNew(cxpContextPtr pccArg, xmlNodePtr pndArg, xmlChar *pucArg, int iArgOptions);

extern resNodePtr
cxpAttributeLocatorResNodeNew(cxpContextPtr pccArg, xmlNodePtr pndArg, xmlChar *pucArg);

extern xmlChar *
cxpProcessPlainNode(xmlNodePtr pndArg, cxpContextPtr pccArg);

#ifdef TESTCODE
extern int
cxpTest(cxpContextPtr pccArg);
#endif
