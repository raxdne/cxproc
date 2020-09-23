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
arcIconvInit(void);

extern BOOL_T
arcAppendEntries(resNodePtr prnArgArchive, const pcre2_code *re_match, BOOL_T fArgContent);

extern BOOL_T
arcGetFileNameDecoded(char *pcArg, xmlChar **ppucResult);

extern BOOL_T
arcGetFileNameEncoded(xmlChar *pucArg, char **ppcResult);

#include <archive.h>
#include <archive_entry.h>

typedef struct archive *arcPtr;

typedef struct archive_entry *arcEntryPtr;

#define CXP_COMRESSION_DEFAULT 3

extern BOOL_T
arcMapReadFormat(resNodePtr prnArg);

