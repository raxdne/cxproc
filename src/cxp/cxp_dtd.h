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

/* 
   access to element names
*/

/* 
   definition of element names (s. xml/schema/cxp.rng)
*/

#define NAME_MAKE BAD_CAST "make"

#define NAME_DESCRIPTION BAD_CAST "description"

#define NAME_FROM BAD_CAST "from"

#define NAME_EACH BAD_CAST "each"

#define NAME_XML BAD_CAST "xml"

#define NAME_XSL BAD_CAST "xsl"

#define NAME_XPATH BAD_CAST "xpath"

#define NAME_SQL BAD_CAST "sql"

#define NAME_ARCHIVE BAD_CAST "archive"

#define NAME_ZIP BAD_CAST "zip"

#define NAME_TAR BAD_CAST "tar"

#define NAME_ISO BAD_CAST "iso"

#define NAME_BASE64 BAD_CAST "base64"

#define NAME_VARIABLE BAD_CAST "variable"

#define NAME_PIE BAD_CAST "pie"

#define NAME_PLAIN BAD_CAST "plain"

#define NAME_SCRIPT BAD_CAST "script"

#define NAME_JSON BAD_CAST "json"

#define NAME_IMAGE BAD_CAST "image"

#define NAME_HTML BAD_CAST "html"

#define NAME_XHTML BAD_CAST "xhtml"

#define NAME_SYSTEM BAD_CAST "system"

#define NAME_SUBST BAD_CAST "subst"

#define NAME_INCLUDE BAD_CAST "include"

#define NAME_FILECOPY BAD_CAST "copy"

#define NAME_INFO BAD_CAST "info"

#define NAME_ENV BAD_CAST "env"

#define NAME_CALENDAR BAD_CAST "calendar"

#define NAME_COL BAD_CAST "col"

#define NAME_DB BAD_CAST "db"

#define NAME_QUERY BAD_CAST "query"

#define NAME_GREP BAD_CAST "grep"

#define NAME_MATCH BAD_CAST "match"


#define NAME_PIE_CALENDAR BAD_CAST "calendar"

#define NAME_PIE_YEAR BAD_CAST "year"

#define NAME_PIE_WEEK BAD_CAST "week"

#define NAME_PIE_MONTH BAD_CAST "month"

#define NAME_PIE_DAY BAD_CAST "day"

#define NAME_PIE_HOUR BAD_CAST "hour"

#define NAME_PIE_COL BAD_CAST "col"


#define NAME_PATHNET BAD_CAST "pathnet"

#define NAME_PATHTABLE BAD_CAST "pathtable"


#define IS_VALID_NODE(NODE) (NODE != NULL && NODE->type == XML_ELEMENT_NODE && domGetPropFlag(NODE,BAD_CAST "valid",TRUE))

#define IS_ROOT(NODE) (NODE != NULL && NODE->type == XML_ELEMENT_NODE && NODE->doc != NULL && NODE == NODE->doc->children)

#define IS_NODE_PI(NODE) (NODE != NULL && NODE->type == XML_PI_NODE)


/* 
   comparison of element names
*/

#define IS_NODE(NODE,NAME) (NODE != NULL && NODE->type == XML_ELEMENT_NODE && NODE->name != NULL && (NAME==NULL || xmlStrEqual(NODE->name,BAD_CAST NAME)))

#define IS_TEXT(NODE) (NODE != NULL && NODE->type == XML_TEXT_NODE && NODE->content != NULL)

#define IS_ENODE(NODE) (NODE != NULL && NODE->type == XML_ELEMENT_NODE && NODE->name != NULL)

#define IS_COMMENT(NODE) (NODE != NULL && NODE->type == XML_COMMENT_NODE && NODE->content != NULL)

#define IS_NODE_MAKE(NODE) (IS_NODE(NODE,NAME_MAKE))

#define IS_NODE_DESCRIPTION(NODE) (IS_NODE(NODE,NAME_DESCRIPTION))

#define IS_NODE_FROM(NODE) (IS_NODE(NODE,NAME_FROM))

#define IS_NODE_EACH(NODE) (IS_NODE(NODE,NAME_EACH))

#define IS_NODE_XML(NODE) (IS_NODE(NODE,NAME_XML) || IS_NODE(NODE,NAME_XHTML))

#define IS_NODE_XSL(NODE) (IS_NODE(NODE,NAME_XSL) || IS_NODE(NODE,BAD_CAST"stylesheet"))

#define IS_NODE_VARIABLE(NODE) (IS_NODE(NODE,NAME_VARIABLE))

#define IS_NODE_XPATH(NODE) (IS_NODE(NODE,NAME_XPATH))

#define IS_NODE_PIE(NODE) (IS_NODE(NODE,NAME_PIE))

#define IS_NODE_PLAIN(NODE) (IS_NODE(NODE,NAME_PLAIN))

#define IS_NODE_SCRIPT(NODE) (IS_NODE(NODE,NAME_SCRIPT))

#define IS_NODE_JSON(NODE) (IS_NODE(NODE,NAME_JSON))

#define IS_NODE_IMAGE(NODE) (IS_NODE(NODE,NAME_IMAGE))

#define IS_NODE_HTML(NODE) (IS_NODE(NODE,NAME_HTML))

#define IS_NODE_XHTML(NODE) (IS_NODE(NODE,NAME_XHTML))

#define IS_NODE_ARCHIVE(NODE) (IS_NODE(NODE,NAME_ARCHIVE))

#define IS_NODE_ZIP(NODE) (IS_NODE(NODE,NAME_ZIP))

#define IS_NODE_TAR(NODE) (IS_NODE(NODE,NAME_TAR))

#define IS_NODE_ISO(NODE) (IS_NODE(NODE,NAME_ISO))

#define IS_NODE_SYSTEM(NODE) (IS_NODE(NODE,NAME_SYSTEM))

#define IS_NODE_SUBST(NODE) (IS_NODE(NODE,NAME_SUBST))

#define IS_NODE_INCLUDE(NODE) (IS_NODE(NODE,NAME_INCLUDE))

#define IS_NODE_INFO(NODE) (IS_NODE(NODE,NAME_INFO))


#define IS_NODE_CALENDAR(NODE) (IS_NODE(NODE,NAME_CALENDAR))

#define IS_NODE_COL(NODE) (IS_NODE(NODE,NAME_COL))


#define IS_NODE_DB(NODE) (IS_NODE(NODE,NAME_DB))

#define IS_NODE_QUERY(NODE) (IS_NODE(NODE,NAME_QUERY))

#define IS_NODE_GREP(NODE) (IS_NODE(NODE,NAME_GREP))

#define IS_NODE_MATCH(NODE) (IS_NODE(NODE,NAME_MATCH))

#define IS_NODE_FILECOPY(NODE) (IS_NODE(NODE,NAME_FILECOPY))

#define IS_NODE_SYMLINK(NODE) (IS_NODE(NODE,NAME_SYMLINK))


#define IS_NODE_PIE_CALENDAR(NODE) (IS_NODE(NODE,NAME_PIE_CALENDAR))

#define IS_NODE_PIE_YEAR(NODE) (IS_NODE(NODE,NAME_PIE_YEAR))

#define IS_NODE_PIE_WEEK(NODE) (IS_NODE(NODE,NAME_PIE_WEEK))

#define IS_NODE_PIE_MONTH(NODE) (IS_NODE(NODE,NAME_PIE_MONTH))

#define IS_NODE_PIE_DAY(NODE) (IS_NODE(NODE,NAME_PIE_DAY))

#define IS_NODE_PIE_COL(NODE) (IS_NODE(NODE,NAME_PIE_COL))


#define IS_NODE_PATHNET(NODE) (IS_NODE(NODE,NAME_PATHNET))

#define IS_NODE_PATHTABLE(NODE) (IS_NODE(NODE,NAME_PATHTABLE))

/* Validation */

#define IS_NODE_MAKE_CHILD(NODE) (IS_NODE_MAKE(NODE) || IS_NODE_DESCRIPTION(NODE) || IS_NODE_SUBST(NODE) || IS_COMMENT(NODE) || IS_NODE_XML(NODE) || IS_NODE_XHTML(NODE) || IS_NODE_PLAIN(NODE) || IS_NODE_INCLUDE(NODE) || IS_NODE_FILECOPY(NODE) || IS_NODE_SYSTEM(NODE) || IS_NODE_EACH(NODE) || IS_NODE_DB(NODE) || IS_NODE_ZIP(NODE) || IS_NODE_IMAGE(NODE))
