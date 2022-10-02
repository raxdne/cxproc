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

#ifdef HAVE_PCRE2
#include <pcre2.h>
#endif

#if HAVE_IMAGE
#include <image/image.h>
#endif

#ifdef HAVE_LIBID3TAG
#include <audio/audio.h>
#endif

/*! directory parsing options */
typedef enum {
  FS_PARSE_DEFAULT,
  FS_PARSE_HIDDEN,
  FS_PARSE_CVS
} dirProcessOption;


extern xmlDocPtr
dirProcessDirNode(xmlNodePtr pndMakeDir, resNodePtr prnArgContext, cxpContextPtr pccArg);

extern resNodePtr
dirNodeToResNodeList(xmlNodePtr pndArg);

extern int
dirMapInfoVerbosity(xmlNodePtr pndArgFile, cxpContextPtr pccArg);

extern void
dirCleanup(void);

#ifdef TESTCODE
extern int
dirTest(cxpContextPtr pccArg);
#endif
