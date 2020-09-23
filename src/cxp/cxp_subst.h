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

/*! different encodings of subst */
typedef enum {
  none,
  base64,
  rfc1738
} encoding_t;

typedef struct _cxpSubst cxpSubst;
typedef cxpSubst *cxpSubstPtr;
struct _cxpSubst {

  /*! is a dedicated processing context for Threads etc
  */
  xmlChar *pucStringFrom;
  xmlNodePtr pndSubst; /* only for access, not free!!! */
  xmlNodePtr pndPlain;
  xmlChar *pucArgv;
  xmlChar *pucCgi;
  xmlChar *pucDefault;
  xmlChar *pucDir;
  xmlChar *pucFile;
  xmlChar *pucExt;
  xmlChar *pucType;
  xmlChar *pucFilename;
  xmlChar *pucEnv;
  xmlChar *pucHost;
  xmlChar *pucProgram;
  xmlChar *pucName;
  xmlChar *pucNow;
  xmlChar *pucTo;
#ifdef HAVE_JS
  xmlChar *pucScriptResult;
#endif

  BOOL_T  fReplaceInAttr;

  /*! different encodings of subst */
  encoding_t eEncoding;

};

#ifdef TESTCODE
extern int
cxpSubstTest(cxpContextPtr pccArg);
#endif

extern cxpSubstPtr
cxpSubstDetect(xmlNodePtr pndArgSubst,cxpContextPtr pccArg);

extern BOOL_T
cxpSubstFree(cxpSubstPtr pcxpSubstArg);

extern BOOL_T
cxpSubstInChildNodes(xmlNodePtr pndArgTop, xmlNodePtr pndArgSubst, cxpContextPtr pccArg);

extern BOOL_T
cxpSubstReplaceNodes(xmlNodePtr pndArg, cxpContextPtr pccArg);

extern xmlChar *
cxpSubstInStringNew(xmlChar *pucArg, cxpSubstPtr pcxpSubstArg, cxpContextPtr pccArg);

extern BOOL_T
cxpSubstIncludeNodes(xmlNodePtr pndArg,cxpContextPtr pccArg);

extern xmlChar *
cxpSubstGetPtr(cxpSubstPtr pcxpSubstArg);

extern xmlChar *
_cxpSubstGetNowPtr(cxpSubstPtr pcxpSubstArg);

extern xmlChar *
_cxpSubstGetScriptResultPtr(cxpSubstPtr pcxpSubstArg);

extern xmlChar *
_cxpSubstGetPathPtr(cxpSubstPtr pcxpSubstArg);

extern xmlChar *
_cxpSubstGetDefaultPtr(cxpSubstPtr pcxpSubstArg);

extern xmlChar *
cxpSubstGetNamePtr(cxpSubstPtr pcxpSubstArg);
