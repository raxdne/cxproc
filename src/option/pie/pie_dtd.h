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

#include <cxp/cxp_dtd.h>

/* 
   definition of element names (s. pie.dtd)
*/

#define NAME_PIE_PI_TAG BAD_CAST "regexp-tag"

#define NAME_PIE_DIR BAD_CAST "dir"

#define NAME_PIE_FILE BAD_CAST "file"

#define NAME_PIE_PIE BAD_CAST "pie"

#define NAME_PIE_BLOCK BAD_CAST "block"

#define NAME_PIE_META BAD_CAST "meta"

#define NAME_PIE_TTAG BAD_CAST "t"

#define NAME_PIE_ETAG BAD_CAST "tag"

#define NAME_PIE_HTAG BAD_CAST "htag"

#define NAME_PIE_TAGLIST NAME_PIE_TTAG

#define NAME_PIE_COPYRIGHT BAD_CAST "copyright"

#define NAME_PIE_INCLUDE BAD_CAST "include"

#define NAME_PIE_IMPORT BAD_CAST "import"

#define NAME_PIE_SECTION BAD_CAST "section"

#define NAME_PIE_ABSTRACT BAD_CAST "abstract"

#define NAME_PIE_AUTHOR BAD_CAST "author"

#define NAME_PIE_VERSION BAD_CAST "version"

#define NAME_PIE_TASK BAD_CAST "task"

#define NAME_PIE_TARGET BAD_CAST "target"

#define NAME_PIE_FIG BAD_CAST "fig"

#define NAME_PIE_IMG BAD_CAST "img"

#define NAME_PIE_HEADER BAD_CAST "h"

#define NAME_PIE_LIST BAD_CAST "list"

#define NAME_PIE_PAR BAD_CAST "p"

#define NAME_PIE_PRE BAD_CAST "pre"

#define NAME_PIE_TABLE BAD_CAST "table"

#define NAME_PIE_TR BAD_CAST "tr"

#define NAME_PIE_TH BAD_CAST "th"

#define NAME_PIE_TD BAD_CAST "td"

#define NAME_PIE_DATE BAD_CAST "date"

#define NAME_PIE_EM BAD_CAST "em"

#define NAME_PIE_STRONG BAD_CAST "strong"

#define NAME_PIE_TT BAD_CAST "tt"

#define NAME_PIE_LINK BAD_CAST "link"

#define NAME_PIE_CONTACT BAD_CAST "contact"

#define NAME_PIE_PHONE BAD_CAST "phone"

#define NAME_PIE_MAIL BAD_CAST "mail"

#define NAME_PIE_WORK BAD_CAST "work"

#define NAME_PIE_PRIVATE BAD_CAST "private"

#define NAME_PIE_MOBILE BAD_CAST "mobile"

#define NAME_PIE_URL BAD_CAST "url"

#define NAME_PIE_VCARD BAD_CAST "vcard"

#define NAME_PIE_NAME BAD_CAST "name"

#define NAME_PIE_TITLE BAD_CAST "title"

#define NAME_PIE_BIRTHDAY BAD_CAST "birthday"

#define NAME_PIE_BREAK BAD_CAST "br"

#define NAME_PIE_RULER BAD_CAST "hr"

#define NAME_PIE_FIRSTNAME BAD_CAST "firstname"


#define IS_NODE_PIE_DIR(NODE) (IS_NODE(NODE,NAME_PIE_DIR))

#define IS_NODE_PIE_FILE(NODE) (IS_NODE(NODE,NAME_PIE_FILE))

#define IS_NODE_PIE_PIE(NODE) (IS_NODE(NODE,NAME_PIE_PIE))

#define IS_NODE_PIE_BLOCK(NODE) (IS_NODE(NODE,NAME_PIE_BLOCK))

#define IS_NODE_PIE_META(NODE) (IS_NODE(NODE,NAME_PIE_META))

#define IS_NODE_PIE_TAGLIST(NODE) (IS_NODE(NODE,NAME_PIE_TAGLIST))

#define IS_NODE_PIE_TTAG(NODE) (IS_NODE(NODE,NAME_PIE_TTAG))

#define IS_NODE_PIE_ETAG(NODE) (IS_NODE(NODE,NAME_PIE_ETAG))

#define IS_NODE_PIE_HTAG(NODE) (IS_NODE(NODE,NAME_PIE_HTAG))

#define IS_NODE_PIE_COPYRIGHT(NODE) (IS_NODE(NODE,NAME_PIE_COPYRIGHT))

#define IS_NODE_PIE_INCLUDE(NODE) (IS_NODE(NODE,NAME_PIE_INCLUDE))

#define IS_NODE_PIE_IMPORT(NODE) (IS_NODE(NODE,NAME_PIE_IMPORT))

#define IS_NODE_PIE_SECTION(NODE) (IS_NODE(NODE,NAME_PIE_SECTION))

#define IS_NODE_PIE_ABSTRACT(NODE) (IS_NODE(NODE,NAME_PIE_ABSTRACT))

#define IS_NODE_PIE_AUTHOR(NODE) (IS_NODE(NODE,NAME_PIE_AUTHOR))

#define IS_NODE_PIE_VERSION(NODE) (IS_NODE(NODE,NAME_PIE_VERSION))

#define IS_NODE_PIE_TASK(NODE) (IS_NODE(NODE,NAME_PIE_TASK))

#define IS_NODE_PIE_TARGET(NODE) (IS_NODE(NODE,NAME_PIE_TARGET))

#define IS_NODE_PIE_FIG(NODE) (IS_NODE(NODE,NAME_PIE_FIG))

#define IS_NODE_PIE_IMG(NODE) (IS_NODE(NODE,NAME_PIE_IMG))

#define IS_NODE_PIE_HEADER(NODE) (IS_NODE(NODE,NAME_PIE_HEADER))

#define IS_NODE_PIE_LIST(NODE) (IS_NODE(NODE,NAME_PIE_LIST))

#define IS_NODE_PIE_PAR(NODE) (IS_NODE(NODE,NAME_PIE_PAR))

#define IS_NODE_PIE_PRE(NODE) (IS_NODE(NODE,NAME_PIE_PRE))

#define IS_NODE_PIE_TABLE(NODE) (IS_NODE(NODE,NAME_PIE_TABLE))

#define IS_NODE_PIE_TR(NODE) (IS_NODE(NODE,NAME_PIE_TR))

#define IS_NODE_PIE_TH(NODE) (IS_NODE(NODE,NAME_PIE_TH))

#define IS_NODE_PIE_TD(NODE) (IS_NODE(NODE,NAME_PIE_TD))

#define IS_NODE_PIE_LINK(NODE) (IS_NODE(NODE,NAME_PIE_LINK))

#define IS_NODE_PIE_DATE(NODE) (IS_NODE(NODE,NAME_PIE_DATE))

#define IS_NODE_PIE_EM(NODE) (IS_NODE(NODE,NAME_PIE_EM))

#define IS_NODE_PIE_STRONG(NODE) (IS_NODE(NODE,NAME_PIE_STRONG))

#define IS_NODE_PIE_TT(NODE) (IS_NODE(NODE,NAME_PIE_TT))

#define IS_NODE_PIE_CONTACT(NODE) (IS_NODE(NODE,NAME_PIE_CONTACT))

#define IS_NODE_PIE_PHONE(NODE) (IS_NODE(NODE,NAME_PIE_PHONE))

#define IS_NODE_PIE_MAIL(NODE) (IS_NODE(NODE,NAME_PIE_MAIL))

#define IS_NODE_PIE_WORK(NODE) (IS_NODE(NODE,NAME_PIE_WORK))

#define IS_NODE_PIE_PRIVATE(NODE) (IS_NODE(NODE,NAME_PIE_PRIVATE))

#define IS_NODE_PIE_MOBILE(NODE) (IS_NODE(NODE,NAME_PIE_MOBILE))

#define IS_NODE_PIE_URL(NODE) (IS_NODE(NODE,NAME_PIE_URL))

#define IS_NODE_PIE_VCARD(NODE) (IS_NODE(NODE,NAME_PIE_VCARD))

#define IS_NODE_PIE_NAME(NODE) (IS_NODE(NODE,NAME_PIE_NAME))

#define IS_NODE_PIE_TITLE(NODE) (IS_NODE(NODE,NAME_PIE_TITLE))

#define IS_NODE_PIE_BIRTHDAY(NODE) (IS_NODE(NODE,NAME_PIE_BIRTHDAY))

#define IS_NODE_PIE_BR(NODE) (IS_NODE(NODE,NAME_PIE_BREAK))

#define IS_NODE_PIE_RULER(NODE) (IS_NODE(NODE,NAME_PIE_RULER))

#define IS_NODE_PIE_FIRSTNAME(NODE) (IS_NODE(NODE,NAME_PIE_FIRSTNAME))



