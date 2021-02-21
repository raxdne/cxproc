/*
  cxproc - Configurable Xml PROCessor

  Copyright (C) 1999..2020 by Alexander Tenbusch

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

#define PKG2_NAMESPACE_URL      "http://pkg2.tenbusch.info/"

#define NAME_PKG2_TRANSITION BAD_CAST "transition"

#define NAME_PKG2_STATE BAD_CAST "stelle"

#define NAME_PKG2_EDGE BAD_CAST "relation"

#define NAME_PKG2_REQUIREMENT BAD_CAST "requirement"

#define NAME_PKG2_PATHTABLE BAD_CAST "pathtable"

#define NAME_PKG2_PATHNET BAD_CAST "pathnet"

#define IS_NODE_PKG2_TRANSITION(NODE) (IS_NODE(NODE,NAME_PKG2_TRANSITION))

#define IS_NODE_PKG2_STATE(NODE) (IS_NODE(NODE,NAME_PKG2_STATE))

#define IS_NODE_PKG2_EDGE(NODE) (IS_NODE(NODE,NAME_PKG2_EDGE))

#define IS_NODE_PKG2_REQUIREMENT(NODE) (IS_NODE(NODE,NAME_PKG2_REQUIREMENT))

#define IS_NODE_PKG2_PATHTABLE(NODE) (IS_NODE(NODE,NAME_PKG2_PATHTABLE))

#define IS_NODE_PKG2_PATHNET(NODE) (IS_NODE(NODE,NAME_PKG2_PATHNET))

/****************************************************************************

   Declaration of public functions (prototypes)
*/

extern xmlNsPtr
pkgGetNs(void);

extern xmlDocPtr
pnetProcessNode(xmlNodePtr pndArg, cxpContextPtr pccArg);

#ifdef TESTCODE
extern int
petrinetTest(cxpContextPtr pccArg);
#endif
