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

#include <cxp/cxp_dtd.h>

#define IS_PIE_OK(P) (xmlStrstr(P, BAD_CAST STR_UTF8_HEAVY_CHECK_MARK) != NULL || xmlStrstr(P, "&#x2714;") != NULL)

#define IS_PIE_CANCEL(P) (xmlStrstr(P, BAD_CAST STR_UTF8_HEAVY_BALLOT_X) != NULL || xmlStrstr(P, "&#x2718;") != NULL)

#define STR_PIE_IMPACT_HIGH (BAD_CAST"+++")

#define STR_PIE_IMPACT_MEDIUM (BAD_CAST"++")

#define STR_PIE_HIDDEN (BAD_CAST";")

/* 
   definition of element names (s. pie.dtd)
*/

#define NAME_PIE_PI_TAG "regexp-tag"

#define NAME_PIE_DIR "dir"

#define NAME_PIE_FILE "file"

#define NAME_PIE_PIE "pie"

#define NAME_PIE_BLOCK "block"

#define NAME_PIE_BLOCKQUOTE "blockquote"

#define NAME_PIE_HTML "html"

#define NAME_PIE_SPAN "span"

#define NAME_PIE_META "meta"

#define NAME_PIE_TAGLIST NAME_PIE_TTAG

#define NAME_PIE_TTAG "t"

#define NAME_PIE_ETAG "tag"

#define NAME_PIE_HTAG "htag"

#define NAME_PIE_INCLUDE "include"

#define NAME_PIE_IMPORT "import"

#define NAME_PIE_SECTION "section"

#define NAME_PIE_TASK "task"

#define NAME_PIE_TARGET "target"

#define NAME_PIE_FIG "fig"

#define NAME_PIE_IMG "img"

#define NAME_PIE_HEADER "h"

#define NAME_PIE_LIST "list"

#define NAME_PIE_PAR "p"

#define NAME_PIE_PRE "pre"

#define NAME_PIE_CSV "csv"

#define NAME_PIE_TABLE "table"

#define NAME_PIE_TR "tr"

#define NAME_PIE_TH "th"

#define NAME_PIE_TD "td"

#define NAME_PIE_DATE "date"

#define NAME_PIE_EM "em"

#define NAME_PIE_STRONG "strong"

#define NAME_PIE_TT "code"

#define NAME_PIE_LINK "link"

#define NAME_PIE_BREAK "br"

#define NAME_PIE_RULER "hr"

#define NAME_PIE_SCRIPT "script"

#define NAME_PIE_SKIP "skip"


#define IS_NODE_PIE_DIR(N) (IS_NODE(N,NAME_PIE_DIR))

#define IS_NODE_PIE_FILE(N) (IS_NODE(N,NAME_PIE_FILE))

#define IS_NODE_PIE_PIE(N) (IS_NODE(N,NAME_PIE_PIE))

#define IS_NODE_PIE_BLOCK(N) (IS_NODE(N,NAME_PIE_BLOCK))

#define IS_NODE_PIE_BLOCKQUOTE(N) (IS_NODE(N,NAME_PIE_BLOCKQUOTE))

#define IS_NODE_PIE_HTML(N) (IS_NODE(N,NAME_PIE_HTML))

#define IS_NODE_PIE_SPAN(N) (IS_NODE(N,NAME_PIE_SPAN))

#define IS_NODE_PIE_META(N) (IS_NODE(N,NAME_PIE_META))

#define IS_NODE_PIE_TAGLIST(N) (IS_NODE(N,NAME_PIE_TAGLIST))

#define IS_NODE_PIE_TTAG(N) (IS_NODE(N,NAME_PIE_TTAG))

#define IS_NODE_PIE_ETAG(N) (IS_NODE(N,NAME_PIE_ETAG))

#define IS_NODE_PIE_HTAG(N) (IS_NODE(N,NAME_PIE_HTAG))

/* all sort of tags */
#define IS_NODE_PIE_TAGS(N) (IS_NODE(N,NAME_PIE_HTAG) || IS_NODE(N,NAME_PIE_ETAG) || IS_NODE(N,NAME_PIE_TTAG))

/* all nodes to be ignored while tagging */
#define IS_NODE_PIE_IGNORE_TAGS(N) (N == NULL || IS_NODE(N,NAME_PIE_META) || IS_NODE(N,NAME_PIE_PRE) || IS_NODE(N,NAME_PIE_TT) || IS_NODE(N,NAME_PIE_DATE))

#define IS_NODE_PIE_INCLUDE(N) (IS_NODE(N,NAME_PIE_INCLUDE))

#define IS_NODE_PIE_IMPORT(N) (IS_NODE(N,NAME_PIE_IMPORT))

#define IS_NODE_PIE_SECTION(N) (IS_NODE(N,NAME_PIE_SECTION))

#define IS_NODE_PIE_TASK(N) (IS_NODE(N,NAME_PIE_TASK))

#define IS_NODE_PIE_TARGET(N) (IS_NODE(N,NAME_PIE_TARGET))

#define IS_NODE_PIE_FIG(N) (IS_NODE(N,NAME_PIE_FIG))

#define IS_NODE_PIE_IMG(N) (IS_NODE(N,NAME_PIE_IMG))

#define IS_NODE_PIE_HEADER(N) (IS_NODE(N,NAME_PIE_HEADER))

#define IS_NODE_PIE_LIST(N) (IS_NODE(N,NAME_PIE_LIST))

#define IS_NODE_PIE_PAR(N) (IS_NODE(N,NAME_PIE_PAR))

#define IS_NODE_PIE_PRE(N) (IS_NODE(N,NAME_PIE_PRE))

#define IS_NODE_PIE_CSV(N) (IS_NODE(N,NAME_PIE_CSV))

#define IS_NODE_PIE_TABLE(N) (IS_NODE(N,NAME_PIE_TABLE))

#define IS_NODE_PIE_TR(N) (IS_NODE(N,NAME_PIE_TR))

#define IS_NODE_PIE_TH(N) (IS_NODE(N,NAME_PIE_TH))

#define IS_NODE_PIE_TD(N) (IS_NODE(N,NAME_PIE_TD))

#define IS_NODE_PIE_DATE(N) (IS_NODE(N,NAME_PIE_DATE))

#define IS_NODE_PIE_EM(N) (IS_NODE(N,NAME_PIE_EM))

#define IS_NODE_PIE_STRONG(N) (IS_NODE(N,NAME_PIE_STRONG))

#define IS_NODE_PIE_TT(N) (IS_NODE(N,NAME_PIE_TT))

#define IS_NODE_PIE_LINK(N) (IS_NODE(N,NAME_PIE_LINK))

#define IS_NODE_PIE_BR(N) (IS_NODE(N,NAME_PIE_BREAK))

#define IS_NODE_PIE_RULER(N) (IS_NODE(N,NAME_PIE_RULER))

#define IS_NODE_PIE_SCRIPT(N) (IS_NODE(N,NAME_PIE_SCRIPT))

