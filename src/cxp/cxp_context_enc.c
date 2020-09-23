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

#ifdef HAVE_CGI
#include <cgi-util/cgi-util.h>
#endif

/*
*/
#include <libxml/HTMLtree.h>
#include <libxml/parser.h>
#include <libxml/uri.h>
#include <libxml/xmlstring.h>

/* 
 */
#include "basics.h"
#include "utils.h"
#include <res_node/res_node.h>
#include <cxp/cxp.h>


/*! cxp Ctxt Enc Set Plain  sets the according conversion descriptor for this context

\param pccArg -- pointer to context
\param *pucArgEnc -- pointer to encoding ID
\return TRUE if  successful, FALSE in case of error
*/
BOOL_T
cxpCtxtEncSetDefaults(cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;

  fResult = cxpCtxtEncSetEnv(pccArg,BAD_CAST"UTF-8");

  fResult = cxpCtxtEncSetPlain(pccArg, BAD_CAST"UTF-8");

  fResult = cxpCtxtEncSetArgv(pccArg, BAD_CAST"UTF-8");

  /*
    create the required conversion descriptors
  */
  if (pccArg->cd.from.utf16be) iconv_close(pccArg->cd.from.utf16be);
  pccArg->cd.from.utf16be = iconv_open("UTF-8","UTF-16BE");
  if (pccArg->cd.from.utf16be == (iconv_t) -1) {
    if (errno == EINVAL) { /* Something went wrong.  */
      cxpCtxtLogPutsExit(pccArg,77,"Conversion from 'UTF-16BE' to 'UTF-8' not possible");
    }
  }

  if (pccArg->cd.to.utf16be) iconv_close(pccArg->cd.to.utf16be);
  pccArg->cd.to.utf16be = iconv_open("UTF-16BE","UTF-8");
  if (pccArg->cd.to.utf16be == (iconv_t) -1) {
     if (errno == EINVAL) { /* Something went wrong.  */
      cxpCtxtLogPutsExit(pccArg,77,"Conversion from 'UTF-8' to 'UTF-16BE' not possible");
    }
  }

  /*
    create the required conversion descriptors
  */
  if (pccArg->cd.from.utf16le) iconv_close(pccArg->cd.from.utf16le);
  pccArg->cd.from.utf16le = iconv_open("UTF-8","UTF-16LE");
  if (pccArg->cd.from.utf16le == (iconv_t) -1) {
    if (errno == EINVAL) { /* Something went wrong.  */
      cxpCtxtLogPutsExit(pccArg,77,"Conversion from 'UTF-16LE' to 'UTF-8' not possible");
    }
  }

  if (pccArg->cd.to.utf16le) iconv_close(pccArg->cd.to.utf16le);
  pccArg->cd.to.utf16le = iconv_open("UTF-16LE","UTF-8");
  if (pccArg->cd.to.utf16le == (iconv_t) -1) {
    if (errno == EINVAL) { /* Something went wrong.  */
      cxpCtxtLogPutsExit(pccArg,77,"Conversion from 'UTF-8' to 'UTF-16LE' not possible");
    }
  }

  /*
    create the required conversion descriptors
  */
  if (pccArg->cd.from.isolat1) iconv_close(pccArg->cd.from.isolat1);
  pccArg->cd.from.isolat1 = iconv_open("UTF-8","ISO-8859-1");
  if (pccArg->cd.from.isolat1 == (iconv_t) -1) {
    if (errno == EINVAL) { /* Something went wrong.  */
      cxpCtxtLogPutsExit(pccArg,77,"Conversion from 'ISO-8859-1' to 'UTF-8' not possible");
    }
  }

  if (pccArg->cd.to.isolat1) iconv_close(pccArg->cd.to.isolat1);
  pccArg->cd.to.isolat1 = iconv_open("ISO-8859-1","UTF-8");
  if (pccArg->cd.to.isolat1 == (iconv_t) -1) {
    if (errno == EINVAL) { /* Something went wrong.  */
      cxpCtxtLogPutsExit(pccArg,77,"Conversion from 'UTF-8' to 'ISO-8859-1' not possible");
    }
  }

  return fResult;
} /* end of cxpCtxtEncSetDefaults() */


/*! cxp Ctxt Enc Set Filesystem  sets the according conversion descriptor for this context

\param pccArg -- pointer to context 
\param *pucArgEnc -- pointer to encoding ID
\return TRUE if  successful, FALSE in case of error
*/
BOOL_T
cxpCtxtEncSetFilesystem(cxpContextPtr pccArg, xmlChar *pucArgEnc)
{
  BOOL_T fResult = FALSE;

  if (pccArg != NULL && STR_IS_NOT_EMPTY(pucArgEnc)) {
    fResult = TRUE;
  }

  return fResult;
} /* end of cxpCtxtEncSetFilesystem() */


/*! cxp Ctxt Enc Set Env  sets the according conversion descriptor for this context

\todo detect encoding from LANG or attributes of make element

\param pccArg -- pointer to context 
\param *pucArgEnc -- pointer to encoding ID
\return TRUE if  successful, FALSE in case of error
*/
BOOL_T
cxpCtxtEncSetEnv(cxpContextPtr pccArg, xmlChar *pucArgEnc)
{
  BOOL_T fResult = FALSE;

  if (pccArg != NULL && STR_IS_NOT_EMPTY(pucArgEnc)) {

    if (pccArg->cd.from.env) {
      iconv_close(pccArg->cd.from.env);
      pccArg->cd.from.env = NULL;
    }

    if (pccArg->cd.to.env) {
      iconv_close(pccArg->cd.to.env);
      pccArg->cd.to.env = NULL;
    }
    
    if (xmlStrcasecmp(pucArgEnc, BAD_CAST "UTF-8")) { /* avoid 1:1 conversion */
      /*
	create the required conversion descriptors
      */
      pccArg->cd.from.env = iconv_open("UTF-8", (const char *)pucArgEnc);
      if (pccArg->cd.from.env == (iconv_t)-1) {
	/* Something went wrong. */
	if (errno == EINVAL) {
	  cxpCtxtLogPrint(pccArg, 1, "Conversion from '%s' to UTF-8 not possible", pucArgEnc);
	}
      }
      else {
	cxpCtxtLogPrint(pccArg, 3, "Environment conversion '%s' to '%s'", pucArgEnc, "UTF-8");
	iconv(pccArg->cd.from.env, NULL, NULL, NULL, NULL);
	fResult = TRUE;
      }

      pccArg->cd.to.env = iconv_open((const char *)pucArgEnc, "UTF-8");
      if (pccArg->cd.to.env == (iconv_t)-1) {
	/* Something went wrong. */
	if (errno == EINVAL) {
	  cxpCtxtLogPrint(pccArg, 1, "Conversion to '%s' to UTF-8 not possible", pucArgEnc);
	}
      }
      else {
	cxpCtxtLogPrint(pccArg, 3, "Environment conversion '%s' to '%s'", pucArgEnc, "UTF-8");
	iconv(pccArg->cd.to.env, NULL, NULL, NULL, NULL);
	fResult = TRUE;
      }
    }
    else {
      cxpCtxtLogPrint(pccArg, 3, "No Environment conversion necessary");
      fResult = TRUE;
    }
  }

  return fResult;
} /* end of cxpCtxtEncSetEnv() */


/*! cxp Ctxt Enc Set CGI  sets the according conversion descriptor for this context

\param pccArg -- pointer to context 
\param *pucArgEnc -- pointer to encoding ID
\return TRUE if  successful, FALSE in case of error
*/
BOOL_T
cxpCtxtEncSetCGI(cxpContextPtr pccArg, xmlChar *pucArgEnc)
{
  BOOL_T fResult = FALSE;

  if (pccArg != NULL && STR_IS_NOT_EMPTY(pucArgEnc)) {

    if (pccArg->cd.from.cgi) {
      iconv_close(pccArg->cd.from.cgi);
      pccArg->cd.from.cgi = NULL;
    }

    if (pccArg->cd.to.cgi) {
      iconv_close(pccArg->cd.to.cgi);
      pccArg->cd.to.cgi = NULL;
    }
    
    if (xmlStrcasecmp(pucArgEnc, BAD_CAST "UTF-8")) { /* avoid 1:1 conversion */
      /*
	create the required conversion descriptors
      */
      pccArg->cd.from.cgi = iconv_open("UTF-8", (const char *)pucArgEnc);
      if (pccArg->cd.from.cgi == (iconv_t)-1) {
	if (errno == EINVAL) { /* Something went wrong.  */
	  cxpCtxtLogPrint(pccArg, 1, "Conversion from '%s' to '%s' not possible", pucArgEnc, "UTF-8");
	}
      }
      else {
	cxpCtxtLogPrint(pccArg, 3, "cgi text conversion '%s' to '%s'", pucArgEnc, "UTF-8");
	iconv(pccArg->cd.from.cgi, NULL, NULL, NULL, NULL);
      }

      pccArg->cd.to.cgi = iconv_open((const char *)pucArgEnc, "UTF-8");
      if (pccArg->cd.to.cgi == (iconv_t)-1) {
	if (errno == EINVAL) { /* Something went wrong.  */
	  cxpCtxtLogPrint(pccArg, 1, "Conversion to '%s' from '%s' not possible", pucArgEnc, "UTF-8");
	}
      }
      else {
	cxpCtxtLogPrint(pccArg, 3, "cgi text conversion '%s' to '%s'", "UTF-8", pucArgEnc);
	iconv(pccArg->cd.to.cgi, NULL, NULL, NULL, NULL);
      }
    }
    else {
      cxpCtxtLogPrint(pccArg, 3, "No cgi text conversion necessary");
    }
    fResult = TRUE;
  }

  return fResult;
} /* end of cxpCtxtEncSetCGI() */


/*! cxp Ctxt Enc Set Plain  sets the according conversion descriptor for this context

\param pccArg -- pointer to context
\param *pucArgEnc -- pointer to encoding ID
\return TRUE if  successful, FALSE in case of error
*/
BOOL_T
cxpCtxtEncSetPlain(cxpContextPtr pccArg, xmlChar *pucArgEnc)
{
  BOOL_T fResult = FALSE;

  if (pccArg != NULL && STR_IS_NOT_EMPTY(pucArgEnc)) {

    if (pccArg->cd.from.plain) {
      iconv_close(pccArg->cd.from.plain);
      pccArg->cd.from.plain = NULL;
    }

    if (pccArg->cd.to.plain) {
      iconv_close(pccArg->cd.to.plain);
      pccArg->cd.to.plain = NULL;
    }
    
    if (xmlStrcasecmp(pucArgEnc,BAD_CAST "UTF-8")) { /* avoid 1:1 conversion */
      /*
	create the required conversion descriptors
      */
      pccArg->cd.from.plain = iconv_open("UTF-8",(const char *)pucArgEnc);
      if (pccArg->cd.from.plain == (iconv_t) -1) {
	if (errno == EINVAL) { /* Something went wrong.  */
	  cxpCtxtLogPrint(pccArg, 1, "Conversion from '%s' to '%s' not possible", pucArgEnc, "UTF-8");
	}
	else {
	  fResult = TRUE;
	}
      }
      else {
	cxpCtxtLogPrint(pccArg,3,"Plain text conversion '%s' to '%s'", pucArgEnc, "UTF-8");
	iconv(pccArg->cd.from.plain, NULL, NULL, NULL, NULL);
	fResult = TRUE;
      }

      pccArg->cd.to.plain = iconv_open((const char *)pucArgEnc,"UTF-8");
      if (pccArg->cd.to.plain == (iconv_t) -1) {
	if (errno == EINVAL) { /* Something went wrong.  */
	  cxpCtxtLogPrint(pccArg,1,"Conversion to '%s' from '%s' not possible", pucArgEnc, "UTF-8");
	}
	else {
	  fResult = TRUE;
	}
      }
      else {
	cxpCtxtLogPrint(pccArg,3,"Plain text conversion '%s' to '%s'", "UTF-8", pucArgEnc);
	iconv(pccArg->cd.to.plain, NULL, NULL, NULL, NULL);
	fResult = TRUE;
      }
    }
    else {
      cxpCtxtLogPrint(pccArg,3,"No Plain text conversion necessary");
      fResult = TRUE;
    }
  }
  return fResult;
} /* end of cxpCtxtEncSetPlain() */


/*! cxp Ctxt Enc Get To Filesystem  gets the according conversion descriptor for this context

\param pccArg -- pointer to context 
\return conversion descriptor if successful or -1 in case of error
*/
iconv_t
cxpCtxtEncGetToFilesystem(cxpContextPtr pccArg)
{
  iconv_t cdResult = (iconv_t) -1;

  if (pccArg != NULL) {
    cxpContextPtr pccParent;

    if ((cdResult = pccArg->cd.to.fs)) {
    }
    else if ((pccParent = cxpCtxtGetParent(pccArg)) && (cdResult = cxpCtxtEncGetToFilesystem(pccParent))) {
    }
    else {
    }
  }
  return cdResult;
} /* end of cxpCtxtEncGetToFilesystem() */


/*! cxp Ctxt Enc Get To Argv  gets the according conversion descriptor for this context

\param pccArg -- pointer to context 
\return conversion descriptor if successful or -1 in case of error
*/
iconv_t
cxpCtxtEncGetToArgv(cxpContextPtr pccArg)
{
  iconv_t cdResult = (iconv_t) -1;

  if (pccArg != NULL) {
    cxpContextPtr pccParent;

    if ((cdResult = pccArg->cd.to.argv)) {
    }
    else if ((pccParent = cxpCtxtGetParent(pccArg)) && (cdResult = cxpCtxtEncGetToArgv(pccParent))) {
    }
    else {
    }
  }
  return cdResult;
} /* end of cxpCtxtEncGetToArgv() */


/*! cxp Ctxt Enc Get To Env  gets the according conversion descriptor for this context

\param pccArg -- pointer to context 
\return conversion descriptor if successful or -1 in case of error
*/
iconv_t
cxpCtxtEncGetToEnv(cxpContextPtr pccArg)
{
  iconv_t cdResult = (iconv_t) -1;

  if (pccArg != NULL) {
    cxpContextPtr pccParent;

    if ((cdResult = pccArg->cd.to.env)) {
    }
    else if ((pccParent = cxpCtxtGetParent(pccArg)) && (cdResult = cxpCtxtEncGetToEnv(pccParent))) {
    }
    else {
    }
  }
  return cdResult;
} /* end of cxpCtxtEncGetToEnv() */


/*! cxp Ctxt Enc Get To CGI  gets the according conversion descriptor for this context

\param pccArg -- pointer to context 
\return conversion descriptor if successful or -1 in case of error
*/
iconv_t
cxpCtxtEncGetToCGI(cxpContextPtr pccArg)
{
  iconv_t cdResult = (iconv_t) -1;

  if (pccArg != NULL) {
    cxpContextPtr pccParent;

    if ((cdResult = pccArg->cd.to.cgi)) {
    }
    else if ((pccParent = cxpCtxtGetParent(pccArg)) && (cdResult = cxpCtxtEncGetToCGI(pccParent))) {
    }
    else {
    }
  }
  return cdResult;
} /* end of cxpCtxtEncGetToCGI() */


/*! cxp Ctxt Enc Get To Plain  gets the according conversion descriptor for this context

\param pccArg -- pointer to context 
\return conversion descriptor if successful or -1 in case of error
*/
iconv_t
cxpCtxtEncGetToPlain(cxpContextPtr pccArg)
{
  iconv_t cdResult = (iconv_t) -1;

  if (pccArg != NULL) {
    cxpContextPtr pccParent;

    if ((cdResult = pccArg->cd.to.plain)) {
    }
    else if ((pccParent = cxpCtxtGetParent(pccArg)) && (cdResult = cxpCtxtEncGetToPlain(pccParent))) {
    }
    else {
    }
  }
  return cdResult;
} /* end of cxpCtxtEncGetToPlain() */


/*! cxp Ctxt Enc Get From Filesystem  gets the according conversion descriptor for this context

\param pccArg -- pointer to context 
\return conversion descriptor if successful or -1 in case of error
*/
iconv_t
cxpCtxtEncGetFromFilesystem(cxpContextPtr pccArg)
{
  iconv_t cdResult = (iconv_t) -1;

  if (pccArg != NULL) {
    cxpContextPtr pccParent;

    if ((cdResult = pccArg->cd.from.fs)) {
    }
    else if ((pccParent = cxpCtxtGetParent(pccArg)) && (cdResult = cxpCtxtEncGetFromFilesystem(pccParent))) {
    }
    else {
    }
  }
  return cdResult;
} /* end of cxpCtxtEncGetFromFilesystem() */


/*! cxp Ctxt Enc Get From Argv  gets the according conversion descriptor for this context

\param pccArg -- pointer to context 
\return conversion descriptor if successful or -1 in case of error
*/
iconv_t
cxpCtxtEncGetFromArgv(cxpContextPtr pccArg)
{
  iconv_t cdResult = (iconv_t) -1;

  if (pccArg != NULL) {
    cxpContextPtr pccParent;

    if ((cdResult = pccArg->cd.from.argv)) {
    }
    else if ((pccParent = cxpCtxtGetParent(pccArg)) && (cdResult = cxpCtxtEncGetFromArgv(pccParent))) {
    }
    else {
    }
  }
  return cdResult;
} /* end of cxpCtxtEncGetFromArgv() */


/*! cxp Ctxt Enc Get From Env  gets the according conversion descriptor for this context

\param pccArg -- pointer to context 
\return conversion descriptor if successful or -1 in case of error
*/
iconv_t
cxpCtxtEncGetFromEnv(cxpContextPtr pccArg)
{
  iconv_t cdResult = (iconv_t) -1;

  if (pccArg != NULL) {
    cxpContextPtr pccParent;

    if ((cdResult = pccArg->cd.from.env)) {
    }
    else if ((pccParent = cxpCtxtGetParent(pccArg)) && (cdResult = cxpCtxtEncGetFromEnv(pccParent))) {
    }
    else {
    }
  }
  return cdResult;
} /* end of cxpCtxtEncGetFromEnv() */


/*! cxp Ctxt Enc Get From CGI  gets the according conversion descriptor for this context

\param pccArg -- pointer to context 
\return conversion descriptor if successful or -1 in case of error
*/
iconv_t
cxpCtxtEncGetFromCGI(cxpContextPtr pccArg)
{
  iconv_t cdResult = (iconv_t) -1;

  if (pccArg != NULL) {
    cxpContextPtr pccParent;

    if ((cdResult = pccArg->cd.from.cgi)) {
    }
    else if ((pccParent = cxpCtxtGetParent(pccArg)) && (cdResult = cxpCtxtEncGetFromCGI(pccParent))) {
    }
    else {
    }
  }

  return cdResult;
} /* end of cxpCtxtEncGetFromCGI() */


/*! cxp Ctxt Enc Get From Plain  gets the according conversion descriptor for this context

\param pccArg -- pointer to context 
\return conversion descriptor if successful or -1 in case of error
*/
iconv_t
cxpCtxtEncGetFromPlain(cxpContextPtr pccArg)
{
  iconv_t cdResult = (iconv_t) -1;

  if (pccArg != NULL) {
    cxpContextPtr pccParent;

    if ((cdResult = pccArg->cd.from.plain)) {
    }
    else if ((pccParent = cxpCtxtGetParent(pccArg)) && (cdResult = cxpCtxtEncGetFromPlain(pccParent))) {
    }
    else {
    }
  }
  return cdResult;
} /* end of cxpCtxtEncGetFromPlain() */


/*! count the name/value pairs in environment array

\param pccArg -- pointer to context
\return count of existing environment variables
*/
int
cxpCtxtEnvGetCount(cxpContextPtr pccArg)
{
  int iResult = 0;

  if (pccArg) {
    cxpContextPtr pccT;

    for (pccT = pccArg; pccT; pccT = cxpCtxtGetParent(pccT)) { /* find top context */
      pccArg = pccT;
    }

    if (pccArg->iCountEnv < 1 && pccArg->ppcEnv != NULL) {
      for (pccArg->iCountEnv = 0; pccArg->ppcEnv[pccArg->iCountEnv] != NULL; pccArg->iCountEnv++);
    }
    iResult = pccArg->iCountEnv;
  }
  return iResult;
} /* end of cxpCtxtEnvGetCount() */


/*!
\param pccArg -- pointer to context
\param iIndex -- index
\return pointer to name of environment variable with index 'iIndex' or NULL else
*/
xmlChar*
cxpCtxtEnvGetName(cxpContextPtr pccArg, index_t iIndex)
{
  xmlChar* pucResult = NULL;

  if (pccArg) {
    char *pchName;
    char *pchSep;
    cxpContextPtr pccT;

    for (pccT = pccArg; pccT; pccT = cxpCtxtGetParent(pccT)) { /* find top context */
      pccArg = pccT;
    }

    if (iIndex < 0 || iIndex >= cxpCtxtEnvGetCount(pccArg)) {
#ifdef DEBUG
      cxpCtxtLogPrint(pccArg, 4, "No valid index '%i' for environment variables ('0 ... %i')", iIndex, cxpCtxtEnvGetCount(pccArg) - 1);
#endif
      return NULL;
    }

    if ((pchName = pccArg->ppcEnv[iIndex]) != NULL && (pchSep = strchr(pchName, '=')) != NULL) {
      /*  */
      if (cxpCtxtEncGetFromEnv(pccArg)) {
	/* convert name of environment variable */
	char buffer_in[BUFFER_LENGTH];
	char buffer_out[BUFFER_LENGTH];
	char *pchOut;
	char *pchIn;
	size_t int_in;
	size_t int_out;
	size_t nconv;

	strncpy(buffer_in, pchName, BUFFER_LENGTH - 1);
	int_in = pchSep - pchName;
	buffer_in[int_in] = '\0';
	int_in++;
	pchIn = buffer_in;
	int_out = BUFFER_LENGTH - 1;
	pchOut = buffer_out;

	nconv = iconv(cxpCtxtEncGetFromEnv(pccArg), (char **)&pchIn, &int_in, &pchOut, &int_out);
	cxpCtxtEncError(pccArg,errno, nconv);

	pucResult = xmlStrdup(BAD_CAST buffer_out);
      }
      else {
	/* no conversion necessary */
	pucResult = xmlStrndup(BAD_CAST pchName, pchSep - pchName);
      }
    }

    if (pucResult) {
#ifdef DEBUG
      cxpCtxtLogPrint(pccArg, 4, "Name of envp[%i] = '%s'", iIndex, pucResult);
#endif
    }
    else {
      cxpCtxtLogPrint(pccArg, 3, "No valid environment variable at '%i'", iIndex);
    }
  }
  return pucResult;
} /* end of cxpCtxtEnvGetName() */


/*! cxp Ctxt Env Get Value

\param pccArg -- pointer to context
\param iIndex -- index
\return pointer to string value of environment variable with index 'iIndex' or NULL in case of error
*/
xmlChar*
cxpCtxtEnvGetValue(cxpContextPtr pccArg, index_t iIndex)
{
  xmlChar* pucResult = NULL;

  if (pccArg) {
    char *pchValue;
    char *pchSep;
    cxpContextPtr pccT;

    for (pccT = pccArg; pccT; pccT = cxpCtxtGetParent(pccT)) { /* find top context */
      pccArg = pccT;
    }

    if (iIndex < 0 || iIndex >= cxpCtxtEnvGetCount(pccArg)) {
#ifdef DEBUG
      cxpCtxtLogPrint(pccArg, 4, "No valid index '%i' for environment variables ('0 ... %i')", iIndex, cxpCtxtEnvGetCount(pccArg) - 1);
#endif
      return NULL;
    }

    if ((pchValue = pccArg->ppcEnv[iIndex]) != NULL && (pchSep = strchr(pchValue, '=')) != NULL) {
      /*  */
      if (cxpCtxtEncGetFromEnv(pccArg)) {
	/* convert value of environment variable */
	char buffer_in[BUFFER_LENGTH];
	char buffer_out[BUFFER_LENGTH];
	char *pchOut;
	char *pchIn;
	size_t int_in;
	size_t int_out;
	size_t nconv;

	int_out = BUFFER_LENGTH - 1;
	int_in = strlen(pchSep + 1) + 1;
	strncpy(buffer_in, pchSep + 1, int_out);
	buffer_in[int_in] = '\0';
	pchIn = buffer_in;
	pchOut = buffer_out;

	nconv = iconv(cxpCtxtEncGetFromEnv(pccArg), (char **)&pchIn, &int_in, &pchOut, &int_out);
	cxpCtxtEncError(pccArg,errno, nconv);

	pucResult = xmlStrdup(BAD_CAST buffer_out);
      }
      else {
	/* no conversion necessary */
	pucResult = xmlStrdup(BAD_CAST pchSep + 1);
      }
    }

    if (pucResult) {
#ifdef DEBUG
      cxpCtxtLogPrint(pccArg, 4, "Value of envp[%i] = '%s'", iIndex, pucResult);
#endif
    }
    else {
      cxpCtxtLogPrint(pccArg, 3, "No valid environment variable at '%i'", iIndex);
    }
  }

  return pucResult;
} /* end of cxpCtxtEnvGetValue() */


/*! cxp Ctxt Env Get ValueByName

\param pccArg -- pointer to context
\param *pucArgName -- name of environment variable
\return pointer to string value of environment variable with name 'pucArgName' or NULL in case of error
*/
xmlChar*
cxpCtxtEnvGetValueByName(cxpContextPtr pccArg, xmlChar *pucArgName)
{
  index_t i;
  xmlChar* pucResult = NULL;
  xmlChar *pucArgv;

  for (i = 0; (pucArgv = cxpCtxtEnvGetName(pccArg, i)); i++) {
    if (xmlStrEqual(pucArgv, pucArgName)) {
      pucResult = cxpCtxtEnvGetValue(pccArg, i);
      xmlFree(pucArgv);
      break;
    }
    xmlFree(pucArgv);
  }

  if (pucResult) {
#ifdef DEBUG
    cxpCtxtLogPrint(pccArg, 4, "env[%s]='%s'", pucArgName, pucResult);
#endif
  }
  else {
    cxpCtxtLogPrint(pccArg, 3, "No valid environment variable named '%s'", pucArgName);
  }

  return pucResult;
} /* end of cxpCtxtEnvGetValueByName() */


/*! cxp Ctxt Env Get BoolByName

\param pccArg -- pointer to context
\param *pucArgName -- name of environment variable
\param fDefault default return value if attribute not found
\return value of environment variable with name 'pucArgName' as TRUE/FALSE
*/
BOOL_T
cxpCtxtEnvGetBoolByName(cxpContextPtr pccArg, xmlChar *pucArgName, BOOL_T fDefault)
{
  BOOL_T fResult = fDefault;
  xmlChar *pucEnvValue;

  pucEnvValue = cxpCtxtEnvGetValueByName(pccArg, pucArgName);
  if (pucEnvValue != NULL && xmlStrcasecmp(pucEnvValue, BAD_CAST "yes") == 0) {
    fResult = TRUE;
  }
  return fResult;
} /* end of cxpCtxtEnvGetBoolByName() */


/*! cxp Ctxt Env Get BoolByName

\param pccArg -- pointer to context
\return value of environment variable with name 'pucArgName' as TRUE/FALSE
*/
BOOL_T
cxpCtxtEnvDup(cxpContextPtr pccArg, char *envp[])
{
  BOOL_T fResult = TRUE;

  if (pccArg != NULL) {
    if (envp != NULL) {
      int i;

      /*! duplicate environment values */
      for (i = 0; envp[i]; i++) {}

      if (i > 0) {
	pccArg->iCountEnv = i;

	pccArg->ppcEnv = (char**)xmlMalloc(sizeof(char*) * (i + 1));
	if (pccArg->ppcEnv) {
	  pccArg->ppcEnv[i] = NULL; /* terminator */
	  for (i--; i > -1; i--) {
	    pccArg->ppcEnv[i] = (char*)xmlStrdup(BAD_CAST envp[i]);
	  }
	}
      }
    }
    else {
      pccArg->ppcEnv = NULL;
    }
  }
  else {
    fResult = FALSE;
  }
  return fResult;
} /* end of cxpCtxtEnvDup() */


/*! creates and sets the according conversion descriptor for this context

\param pccArg -- pointer to context
\param *pucArgEnc -- pointer to encoding ID
\return TRUE if  successful, FALSE in case of error
*/
BOOL_T
cxpCtxtEncSetArgv(cxpContextPtr pccArg, xmlChar *pucArgEnc)
{
  BOOL_T fResult = FALSE;

  if (pccArg == NULL) {
  }
  else if (pucArgEnc == NULL || xmlStrlen(pucArgEnc) < 1) {
  }
  else if (xmlStrcasecmp(pucArgEnc, BAD_CAST "UTF-8")) { /* avoid 1:1 conversion */
    /*
    create the required conversion descriptors
    */
    if (pccArg->cd.from.argv) iconv_close(pccArg->cd.from.argv);
    pccArg->cd.from.argv = iconv_open("UTF-8", (const char *)pucArgEnc);
    if (pccArg->cd.from.argv == (iconv_t)-1) {
      /* Something went wrong. */
      if (errno == EINVAL) {
	cxpCtxtLogPrint(pccArg, 1, "Conversion from '%s' to UTF-8 not possible", pucArgEnc);
      }
    }
    else {
      cxpCtxtLogPrint(pccArg, 3, "Argument conversion '%s' to '%s'", pucArgEnc, "UTF-8");
      iconv(pccArg->cd.from.argv, NULL, NULL, NULL, NULL);
      fResult = TRUE;
    }

    if (pccArg->cd.to.argv) iconv_close(pccArg->cd.to.argv);
    pccArg->cd.to.argv = iconv_open((const char *)pucArgEnc, "UTF-8");
    if (pccArg->cd.to.argv == (iconv_t)-1) {
      /* Something went wrong. */
      if (errno == EINVAL) {
	cxpCtxtLogPrint(pccArg, 1, "Conversion to '%s' to UTF-8 not possible", pucArgEnc);
      }
    }
    else {
      cxpCtxtLogPrint(pccArg, 3, "Argument conversion '%s' to '%s'", pucArgEnc, "UTF-8");
      iconv(pccArg->cd.to.argv, NULL, NULL, NULL, NULL);
      fResult = TRUE;
    }
  }
  else {
    cxpCtxtLogPrint(pccArg, 3, "No Argument conversion necessary");
    pccArg->cd.from.argv = NULL;
    pccArg->cd.to.argv = NULL;
    fResult = TRUE;
  }
  return fResult;
} /* end of cxpCtxtEncSetArgv() */


/*!
\param pccArg -- pointer to context
\param iIndex -- index
\return pointer to name of environment variable with index 'iIndex' or NULL else
*/
char*
cxpCtxtEncShellCommand(cxpContextPtr pccArg, xmlChar *pucArg)
{
  char* pcResult = NULL;

  if (pccArg) {
    xmlChar mpucCall[BUFFER_LENGTH];
    char mpchIn[BUFFER_LENGTH];
    char mpchOut[BUFFER_LENGTH];
    char *pchOut;
    char *pchIn;
    size_t int_in;
    size_t int_out;
    size_t nconv;

    if (pccArg->cd.to.env) {
      /* convert */

      int_out = BUFFER_LENGTH - 1;
      int_in = xmlStrlen(pucArg) + 1;
      strncpy(mpchIn, (char *)pucArg, int_out);
      mpchIn[int_in] = '\0';
      pchIn = mpchIn;
      pchOut = mpchOut;

      nconv = iconv(pccArg->cd.to.env, (char **)&pchIn, &int_in, &pchOut, &int_out);
      cxpCtxtEncError(pccArg, errno, nconv);
      pcResult = (char *) xmlStrdup(BAD_CAST pchOut);
    }
    else {
      /* no conversion necessary */
      pcResult = (char *) xmlStrdup(pucArg);
    }
  }
  return pcResult;
} /* end of cxpCtxtEncShellCommand() */


/*!

\param pccArg -- pointer to context
\return count of existing program arguments
*/
int
cxpCtxtCliGetCount(cxpContextPtr pccArg)
{
  int iResult = -1;

  if (pccArg) {
    cxpContextPtr pccT;

    for (pccT = pccArg; pccT; pccT = cxpCtxtGetParent(pccT)) {
      iResult = pccT->iCountArgv;
    }
  }
  return iResult;
} /* end of cxpCtxtCliGetCount() */


/*! cxp Ctxt Cli Get Name

\param pccArg -- pointer to context
\param iIndex -- index
\return pointer to name of program argument with index 'iIndex' or NULL else
*/
xmlChar*
cxpCtxtCliGetName(cxpContextPtr pccArg, index_t iIndex)
{
  xmlChar* pucResult = NULL;
  xmlChar *pucValue;
  xmlChar *pucSep;
  cxpContextPtr pccT;

  for (pccT = pccArg; pccT; pccT = cxpCtxtGetParent(pccT)) {
    pccArg = pccT;
  }

  if (iIndex < 0 || iIndex >= cxpCtxtCliGetCount(pccArg)) {
    return NULL;
  }

  if ((pucValue = cxpCtxtCliGetValue(pccArg, iIndex))) {
    if ((pucSep = BAD_CAST xmlStrchr((const xmlChar *)pucValue, (xmlChar)'='))) {
      /*  */
      pucResult = xmlStrndup(pucValue, pucSep - pucValue);
    }
    else {
      pucResult = NULL;
    }
    xmlFree(pucValue);
  }

  if (pucResult) {
#ifdef DEBUG
    cxpCtxtLogPrint(pccArg, 4, "Name of argv[%i] = '%s'", iIndex, pucResult);
#endif
  }
  else {
    cxpCtxtLogPrint(pccArg, 3, "No valid named arg for '%i'", iIndex);
  }

  return pucResult;
} /* end of cxpCtxtCliGetName() */


/*! cxp Ctxt Cli Get Value

\param pccArg -- pointer to context
\param iIndex -- index
\return pointer to string value of program argument with index 'iIndex' or NULL in case of error
*/
xmlChar*
cxpCtxtCliGetValue(cxpContextPtr pccArg, index_t iIndex)
{
  xmlChar *pucResult = NULL;

  if (pccArg) {
    char buffer_in[BUFFER_LENGTH];
    char buffer_out[BUFFER_LENGTH];
    char *pchOut;
    char *pchIn;
    char *pchValue;
    size_t int_in;
    size_t int_out;
    size_t nconv;
    cxpContextPtr pccT;

    for (pccT = pccArg; pccT; pccT = cxpCtxtGetParent(pccT)) {
      pccArg = pccT;
    }

    if (pccArg->ppcArgv == NULL) {
#ifdef DEBUG
      cxpCtxtLogPrint(pccArg, 2, "No arguments for executable defined");
#endif
    }
    else if (iIndex < 0 || iIndex >= cxpCtxtCliGetCount(pccArg)) {
#ifdef DEBUG
      cxpCtxtLogPrint(pccArg, 4, "Index '%i' too large (max. '%i')", iIndex, cxpCtxtCliGetCount(pccArg) - 1);
#endif
    }
    else if ((pchValue = pccArg->ppcArgv[iIndex]) != NULL) {
      if (cxpCtxtEncGetFromArgv(pccArg)) {
	int_out = BUFFER_LENGTH - 1;
	int_in = strlen(pchValue) + 1;
	strncpy(buffer_in, pchValue, int_out);
	buffer_in[int_in] = '\0';
	pchIn = buffer_in;
	pchOut = buffer_out;

	nconv = iconv(cxpCtxtEncGetFromArgv(pccArg), (char **)&pchIn, &int_in, &pchOut, &int_out);
	cxpCtxtEncError(pccArg, errno, nconv);
	pucResult = xmlStrdup(BAD_CAST buffer_out);
      }
      else {
	/* no conversion necessary */
	pucResult = xmlStrdup(BAD_CAST pchValue);
      }
    }

    if (pucResult) {
      resPathRemoveQuotes(pucResult);
#ifdef DEBUG
      cxpCtxtLogPrint(pccArg, 4, "Value of argv[%i] = '%s'", iIndex, pucResult);
#endif
    }
    else {
      cxpCtxtLogPrint(pccArg, 3, "No valid argument at '%i'", iIndex);
    }
    /*!\todo additional conversion for \& \' etc. */
  }
  return pucResult;
} /* end of cxpCtxtCliGetValue() */


/*! cxp Ctxt Cli Get ValueByName

\param pccArg -- pointer to context
\param *pucArgName -- name of program argument
\return pointer to string value of program argument with name 'pucArgName' or NULL in case of error
*/
xmlChar*
cxpCtxtCliGetValueByName(cxpContextPtr pccArg, xmlChar *pucArgName)
{
  xmlChar *pucResult = NULL;

  if (pccArg != NULL && STR_IS_NOT_EMPTY(pucArgName)) {
    index_t i;
    index_t j;
    cxpContextPtr pccT;

    for (pccT = pccArg; pccT; pccT = cxpCtxtGetParent(pccT)) {
      pccArg = pccT;
    }

    for (i = 0, j = cxpCtxtCliGetCount(pccArg); i < j && pucResult == NULL; i++) {
      xmlChar *pucName;
      pucName = cxpCtxtCliGetName(pccArg, i);
      if (pucName != NULL && xmlStrEqual(pucName, pucArgName)) {
	xmlChar *pucArgvValue;
	pucArgvValue = cxpCtxtCliGetValue(pccArg, i);
	if (pucArgvValue) {
	  xmlChar *pucT;
	  pucT = BAD_CAST xmlStrchr((const xmlChar *)pucArgvValue, (xmlChar)'=');
	  if (pucT) {
	    pucResult = xmlStrdup(pucT + 1);
	  }
	}
	xmlFree(pucArgvValue);
      }
      xmlFree(pucName);
    }

    if (pucResult) {
#ifdef DEBUG
      cxpCtxtLogPrint(pccArg, 4, "arg[%s]='%s'", pucArgName, pucResult);
#endif
    }
    else {
      cxpCtxtLogPrint(pccArg, 3, "No valid named arg for '%s'", pucArgName);
    }
  }
  return pucResult;
} /* end of cxpCtxtCliGetValueByName() */


/*!

\param pccArg -- pointer to context
\param *pucArgFormat -- pointer to format string
\return pointer to string value of 'pucArgFormat' substituted by program argument values or NULL in case of error
*/
xmlChar*
cxpCtxtCliGetFormat(cxpContextPtr pccArg, xmlChar *pucArgFormat)
{
  xmlChar *pucResult = NULL;

  if (pccArg != NULL && STR_IS_NOT_EMPTY(pucArgFormat)) {
    if ((isdigit(pucArgFormat[0]) && pucArgFormat[1] == (xmlChar)'\0')
      || (isdigit(pucArgFormat[0]) && isdigit(pucArgFormat[1]) && pucArgFormat[2] == (xmlChar)'\0')) {
      /* a number */
      pucResult = cxpCtxtCliGetValue(pccArg, atoi((char *)pucArgFormat));
    }
    else if (xmlStrchr(pucArgFormat, (xmlChar)'%')) {
      /* a format string */
      int i;
      xmlChar *pucT;
      xmlChar buffer[BUFFER_LENGTH];

      pucResult = xmlStrdup(pucArgFormat);
      for (i = 25; i > -1; i--) {
	xmlChar *pucTT;

	xmlStrPrintf(buffer, BUFFER_LENGTH - 1, "%%%i", i);
	pucTT = cxpCtxtCliGetValue(pccArg, i);
	pucT = ReplaceStr(pucResult, buffer, (pucTT ? pucTT : BAD_CAST""));

	if (pucT != NULL && pucT != pucResult) {
	  xmlFree(pucResult);
	  pucResult = pucT;
	}
	xmlFree(pucTT);
      }
    }
    else {
      /* a named argument */
    }
  }

  if (pucResult) {
#ifdef DEBUG
    cxpCtxtLogPrint(pccArg, 4, "Substituted value = '%s'", pucResult);
#endif
  }
  else {
    cxpCtxtLogPrint(pccArg, 2, "No valid format string");
  }

  return pucResult;
} /* end of cxpCtxtCliGetFormat() */


/*! cxp Ctxt Cgi Get Count

\param pccArg -- pointer to context
\return count of existing CGI parameters
*/
int
cxpCtxtCgiGetCount(cxpContextPtr pccArg)
{
#ifdef HAVE_CGI
  return cgi_num_entries;
#else
  return 0;
#endif
} /* end of cxpCtxtCgiGetCount() */


/*! cxp Ctxt Cgi Get Name

\param pccArg -- pointer to context
\param iIndex -- index
\return pointer to name of CGI parameter with index 'iIndex' or NULL else
*/
xmlChar*
cxpCtxtCgiGetName(cxpContextPtr pccArg, index_t iIndex)
{
  xmlChar* pucResult = NULL;

#ifdef HAVE_CGI
  if (iIndex < cgi_num_entries && cgi_entries[iIndex].name != NULL && strlen(cgi_entries[iIndex].name) > 0) {
    if (pccArg->cd.from.cgi) {
      /* convert name of CGI variable */
      char buffer_in[BUFFER_LENGTH];
      char buffer_out[BUFFER_LENGTH];
      char *pchOut;
      char *pchIn;
      size_t int_in;
      size_t int_out;
      size_t nconv;

      int_out = BUFFER_LENGTH - 1;
      int_in = strlen(cgi_entries[iIndex].name) + 1;
      strncpy(buffer_in, cgi_entries[iIndex].name, int_out);
      buffer_in[int_in] = '\0';
      pchIn = buffer_in;
      pchOut = buffer_out;

      nconv = iconv(pccArg->cd.from.cgi, (char **)&pchIn, &int_in, &pchOut, &int_out);
      cxpCtxtEncError(pccArg, errno, nconv);

      pucResult = xmlStrdup(BAD_CAST buffer_out);
    }
    else {
      /* no conversion necessary */
      pucResult = xmlStrdup(BAD_CAST cgi_entries[iIndex].name);
    }

    if (pucResult) {
#ifdef DEBUG
      cxpCtxtLogPrint(pccArg, 4, "Name of cgi[%i] = '%s'", iIndex, pucResult);
#endif
    }
    else {
      cxpCtxtLogPrint(pccArg, 3, "No valid CGI variable at '%i'", iIndex);
    }
  }
#else
#endif

  return pucResult;
} /* end of cxpCtxtCgiGetName() */


/*! cxp Ctxt Cgi Get Value

\param pccArg -- pointer to context
\param iIndex -- index
\return pointer to string value of CGI parameter with index 'iIndex' or NULL in case of error
*/
xmlChar*
cxpCtxtCgiGetValue(cxpContextPtr pccArg, index_t iIndex)
{
  xmlChar* pucResult = NULL;

#ifdef HAVE_CGI
  size_t iLengthIn;

  /* dont use 'cgi_entries[iIndex].content_length' */

  if (iIndex < cgi_num_entries && cgi_entries[iIndex].val != NULL) {

    iLengthIn = strlen(cgi_entries[iIndex].val) + 1; /* to convert null terminator too */
    if (iLengthIn > 1) {

      if (pccArg->cd.from.cgi) {
	/* convert val of CGI variable */
	char *pchOutput;
	char *pchOut;
	size_t iLengthOut;
	size_t nconv;

	iLengthOut = iLengthIn * 2 * sizeof(char) + 1;
	pchOutput = (char *)xmlMemMalloc(iLengthOut);
	pchOut = pchOutput;

	nconv = iconv(pccArg->cd.from.cgi, (char **)&(cgi_entries[iIndex].val), &iLengthIn, &pchOut, &iLengthOut);
	cxpCtxtEncError(pccArg, errno, nconv);

	pucResult = BAD_CAST pchOutput;
      }
      else {
	/* no conversion necessary */
	pucResult = xmlStrdup(BAD_CAST cgi_entries[iIndex].val);
      }
    }
    else {
      /* empty value */
      pucResult = xmlStrdup(BAD_CAST"");
    }

    if (pucResult) {
#ifdef DEBUG
      cxpCtxtLogPrint(pccArg, 4, "Value of cgi[%i] = '%s'", iIndex, pucResult);
#endif
    }
    else {
      cxpCtxtLogPrint(pccArg, 3, "No valid named cgi for '%i'", iIndex);
    }
  }
#else
#endif

  return pucResult;
} /* end of cxpCtxtCgiGetValue() */


/*! cxp Ctxt Cgi Get ValueByName

\param pccArg -- pointer to context
\param *pucArgName -- name of CGI variable
\return pointer to string value of CGI parameter with name 'pucArgName' or NULL in case of error
*/
xmlChar*
cxpCtxtCgiGetValueByName(cxpContextPtr pccArg, xmlChar *pucArgName)
{
  xmlChar* pucResult = NULL;
  index_t i;
  xmlChar* pucT;

  for (i = 0; (pucT = cxpCtxtCgiGetName(pccArg, i)); i++) {
    if (xmlStrEqual(pucT, pucArgName)) {
      pucResult = cxpCtxtCgiGetValue(pccArg, i);
      xmlFree(pucT);
      break;
    }
    xmlFree(pucT);
  }

  if (pucResult) {
#ifdef DEBUG
    cxpCtxtLogPrint(pccArg, 4, "cgi[%s]='%s'", pucArgName, pucResult);
#endif
  }
  else {
    cxpCtxtLogPrint(pccArg, 2, "No valid named cgi for '%s'", pucArgName);
  }
  return pucResult;
} /* end of cxpCtxtCgiGetValueByName() */


/* cleanup for libiconv 
 */
void
cxpCtxtEncFree(cxpContextPtr pccArg)
{
  if (pccArg) {
    if (pccArg->cd.from.env != NULL) {
      iconv_close(pccArg->cd.from.env);
      pccArg->cd.from.env = NULL;
    }

    if (pccArg->cd.to.env != NULL) {
      iconv_close(pccArg->cd.to.env);
      pccArg->cd.to.env = NULL;
    }

    if (pccArg->cd.from.plain != NULL) {
      iconv_close(pccArg->cd.from.plain);
      pccArg->cd.from.plain = NULL;
    }

    if (pccArg->cd.to.plain != NULL) {
      iconv_close(pccArg->cd.to.plain);
      pccArg->cd.to.plain = NULL;
    }

    if (pccArg->cd.from.utf16be != NULL) {
      iconv_close(pccArg->cd.from.utf16be);
      pccArg->cd.from.utf16be = NULL;
    }

    if (pccArg->cd.to.utf16be != NULL) {
      iconv_close(pccArg->cd.to.utf16be);
      pccArg->cd.to.utf16be = NULL;
    }

    if (pccArg->cd.from.utf16le != NULL) {
      iconv_close(pccArg->cd.from.utf16le);
      pccArg->cd.from.utf16le = NULL;
    }

    if (pccArg->cd.to.utf16le != NULL) {
      iconv_close(pccArg->cd.to.utf16le);
      pccArg->cd.to.utf16le = NULL;
    }

    if (pccArg->cd.from.isolat1 != NULL) {
      iconv_close(pccArg->cd.from.isolat1);
      pccArg->cd.from.isolat1 = NULL;
    }

    if (pccArg->cd.to.isolat1 != NULL) {
      iconv_close(pccArg->cd.to.isolat1);
      pccArg->cd.to.isolat1 = NULL;
    }

    if (pccArg->cd.from.fs != NULL) {
      iconv_close(pccArg->cd.from.fs);
      pccArg->cd.from.fs = NULL;
    }

    if (pccArg->cd.to.fs != NULL) {
      iconv_close(pccArg->cd.to.fs);
      pccArg->cd.to.fs = NULL;
    }
  }
}


BOOL_T
cxpCtxtEncError(cxpContextPtr pccArg, int int_errno, size_t nconv)
{
  /*!\todo make cxpCtxtEncError(int_errno); */
  if (int_errno==EILSEQ) {
    cxpCtxtLogPrint(pccArg,1, "invalid byte sequence in the input");
    return FALSE;
  }
  else if (int_errno==E2BIG) {
    cxpCtxtLogPrint(pccArg,1, "it ran out of space in the output buffer.");
    return FALSE;
  }
  else if (int_errno==EINVAL) {
    cxpCtxtLogPrint(pccArg,1, "an incomplete byte sequence at the end of the input buffer.");
    return FALSE;
  }
  else if (int_errno==EBADF) {
    cxpCtxtLogPrint(pccArg,1, "The cd argument is invalid.");
    return FALSE;
  }

#if 0
  if (nconv == (size_t) -1) {
    /* Not everything went right.  It might only be
       an unfinished byte sequence at the end of the
       buffer.  Or it is a real problem.  */
    if (int_errno == EINVAL)
      /* This is harmless.  Simply move the unused
	 bytes to the beginning of the buffer so that
	 they can be used in the next round.  */
      memmove (inbuf, inptr, insize);
    else {
      /* It is a real problem.  Maybe we ran out of
	 space in the output buffer or we have invalid
	 input.  In any case back the file pointer to
	 the position of the last processed byte.  */
      lseek (fd, -insize, SEEK_CUR);
      result = -1;
      break;
    }
  }
#endif
  return FALSE;
}



#ifdef TESTCODE
#include "test/test_cxp_context_enc.c"
#endif

