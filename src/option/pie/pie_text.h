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

#include <pie/pie_text_blocks.h>

#define PARENTSTRING_LENGTH_MAX 20 /* maximum string length for generated parent header string */

#define CXP_PIE_URL (BAD_CAST "http://www.tenbusch.info/pie")

extern xmlDocPtr
pieProcessPieNode(xmlNodePtr pndMakePie, cxpContextPtr pccArg);

extern xmlChar *
pieGetParentHeaderStr(xmlNodePtr pndN);

extern xmlChar *
pieGetAncestorContextStr(xmlNodePtr pndArg);

extern void
pieTextCleanup(void);

#ifdef TESTCODE
extern int
pieTextTest(cxpContextPtr pccArg);
#endif
