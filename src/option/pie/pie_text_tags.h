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
RecognizeHashtags(xmlNodePtr pndArg, pcre2_code* preArg);

extern BOOL_T
InheritHashtags(xmlDocPtr pdocArg);

extern xmlNodePtr
ProcessTags(xmlDocPtr pdocPie, xmlChar *pucAttrTags);

extern BOOL_T
CleanListTag(xmlNodePtr pndArg, BOOL_T fArgMerge);

extern xmlNodePtr
RecognizeGlobalTags(xmlNodePtr pndTags, xmlNodePtr pndArg);

extern void
pieTextTagsCleanup(void);

#ifdef TESTCODE
extern int
pieTextTagsTest(void);
#endif
