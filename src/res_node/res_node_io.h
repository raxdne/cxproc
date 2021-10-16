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

extern BOOL_T
resNodeOpen(resNodePtr prnArg, const char *pchMode);

extern BOOL_T
resNodeIsOpen(resNodePtr prnArg);

extern BOOL_T
resNodeClose(resNodePtr prnArg);

extern BOOL_T
resNodePutContent(resNodePtr prnArg);

extern BOOL_T
resNodeSaveContent(resNodePtr prnArg);

extern void *
resNodeGetContent(resNodePtr prnArg, int iArgMax);

extern xmlChar *
resNodeGetContentBase64Eat(resNodePtr prnArg, int iArgMax);

extern void *
resNodeGetHandleIO(resNodePtr prnArg);

extern void *
resNodeSetContentPtr(resNodePtr prnArg, void *pArg, size_t iSize);

extern void *
resNodeEatContentPtr(resNodePtr prnArg);

extern void *
resNodeGetContentPtr(resNodePtr prnArg);

extern xmlDocPtr
resNodeSetContentDocEat(resNodePtr prnArg, xmlDocPtr pdocArg);

extern xmlDocPtr
resNodeEatContentDoc(resNodePtr prnArg);

extern xmlDocPtr
resNodeGetContentDoc(resNodePtr prnArg);

extern BOOL_T
resNodeSwapContent(resNodePtr prnArgFrom, resNodePtr prnArgTo);

extern xmlDocPtr
resNodeReadDoc(resNodePtr prnArg);

extern int
resNodeGetUsageCount(resNodePtr prnArg);

extern void
resNodeChangeDomURL(xmlDocPtr pdocArg, resNodePtr pccArg);

#ifdef TESTCODE
extern int
resNodeTestInOut(void);
#endif

