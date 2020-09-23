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

extern BOOL_T
cxpCtxtCacheEnable(cxpContextPtr pccArg, BOOL_T fArg);

extern BOOL_T
cxpCtxtCacheAppendResNodeEat(cxpContextPtr pccArg, resNodePtr prnArg);

extern BOOL_T
cxpCtxtCacheAppendBuffer(cxpContextPtr pccArg, xmlChar *pucArg, xmlChar *pucArgName);

extern BOOL_T
cxpCtxtCacheAppendDoc(cxpContextPtr pccArg, xmlDocPtr pdocArg, xmlChar *pucArgName);

extern xmlChar*
cxpCtxtCacheGetBuffer(cxpContextPtr pccArg, xmlChar *pucArgName);

extern xmlDocPtr
cxpCtxtCacheGetDoc(cxpContextPtr pccArg, xmlChar *pucArgName);

extern resNodePtr
cxpCtxtCacheGetResNode(cxpContextPtr pccArg, xmlChar *pucArgName);

extern xmlChar*
cxpCtxtCacheUnlinkBuffer(cxpContextPtr pccArg, xmlChar *pucArgName);

extern xmlDocPtr
cxpCtxtCacheUnlinkDoc(cxpContextPtr pccArg, xmlChar *pucArgName);

extern void
cxpCtxtCachePrint(cxpContextPtr pccArg);

#ifdef TESTCODE
extern int
cxpCtxtCacheTest(cxpContextPtr pccArg);
#endif
