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
cxpCtxtEnvDup(cxpContextPtr pccArg, char *envp[]);

extern BOOL_T
cxpCtxtEncSetDefaults(cxpContextPtr pccArg);

extern BOOL_T
cxpCtxtEncSetFilesystem(cxpContextPtr pccArg, xmlChar *pucArgEnc);

extern BOOL_T
cxpCtxtEncSetEnv(cxpContextPtr pccArg, xmlChar *pucArgEnc);

extern BOOL_T
cxpCtxtEncSetCGI(cxpContextPtr pccArg, xmlChar *pucArgEnc);

extern BOOL_T
cxpCtxtEncSetPlain(cxpContextPtr pccArg, xmlChar *pucArgEnc);

extern iconv_t
cxpCtxtEncGetToFilesystem(cxpContextPtr pccArg);

extern iconv_t
cxpCtxtEncGetToArgv(cxpContextPtr pccArg);

extern iconv_t
cxpCtxtEncGetToEnv(cxpContextPtr pccArg);

extern iconv_t
cxpCtxtEncGetToCGI(cxpContextPtr pccArg);

extern iconv_t
cxpCtxtEncGetToPlain(cxpContextPtr pccArg);

extern iconv_t
cxpCtxtEncGetFromFilesystem(cxpContextPtr pccArg);

extern iconv_t
cxpCtxtEncGetFromArgv(cxpContextPtr pccArg);

extern iconv_t
cxpCtxtEncGetFromEnv(cxpContextPtr pccArg);

extern iconv_t
cxpCtxtEncGetFromCGI(cxpContextPtr pccArg);

extern iconv_t
cxpCtxtEncGetFromPlain(cxpContextPtr pccArg);

extern iconv_t
cxpCtxtEncGetFromZip(cxpContextPtr pccArg);

extern BOOL_T
cxpCtxtEncSetArgv(cxpContextPtr pccArg, xmlChar *pucArgEnc);


extern int
cxpCtxtEnvGetCount(cxpContextPtr pccArg);

extern xmlChar*
cxpCtxtEnvGetName(cxpContextPtr pccArg, index_t iIndex);

extern xmlChar*
cxpCtxtEnvGetValue(cxpContextPtr pccArg, index_t iIndex);

extern xmlChar*
cxpCtxtEnvGetValueByName(cxpContextPtr pccArg, xmlChar *pucArgName);

extern BOOL_T
cxpCtxtEnvGetBoolByName(cxpContextPtr pccArg, xmlChar *pucArgName, BOOL_T fDefault);


extern char*
cxpCtxtEncShellCommand(cxpContextPtr pccArg, xmlChar *pucArg);


extern int
cxpCtxtCliGetCount(cxpContextPtr pccArg);

extern xmlChar*
cxpCtxtCliGetName(cxpContextPtr pccArg, index_t iIndex);

extern xmlChar*
cxpCtxtCliGetValue(cxpContextPtr pccArg, index_t iIndex);

extern xmlChar*
cxpCtxtCliGetValueByName(cxpContextPtr pccArg, xmlChar *pucArgName);

extern xmlChar*
cxpCtxtCliGetFormat(cxpContextPtr pccArg, xmlChar *pucArgFormat);


extern BOOL_T
cxpCtxtEncSetCgiDefault(cxpContextPtr pccArg, xmlChar *pucArg);

extern int
cxpCtxtCgiGetCount(cxpContextPtr pccArg);

extern xmlChar*
cxpCtxtCgiGetName(cxpContextPtr pccArg, index_t iIndex);

extern xmlChar*
cxpCtxtCgiGetValue(cxpContextPtr pccArg, index_t iIndex);

extern xmlChar*
cxpCtxtCgiGetValueByName(cxpContextPtr pccArg, xmlChar *pucArgName);


extern void
cxpCtxtEncFree(cxpContextPtr pccArg);

extern BOOL_T
cxpCtxtEncError(cxpContextPtr pccArg, int int_errno, size_t nconv);


#ifdef TESTCODE
extern int
cxpCtxtEncTest(cxpContextPtr pccArg);
#endif
