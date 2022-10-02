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



/*
*/
#include <libxml/xmlversion.h>
#include <libxml/HTMLtree.h>
#include <libxml/parser.h>
#include <libxml/uri.h>

#ifdef LIBXML_THREAD_ENABLED
#include <libxml/globals.h>
#include <libxml/threads.h>
#ifdef HAVE_LIBPTHREAD
#include <pthread.h>
#elif WITH_THREAD
#endif
#endif

/*
*/
#include "basics.h"
#include "utils.h"
#include <cxp/cxp.h>
#include <cxp/cxp_threadp.h>


#ifdef HAVE_LIBPTHREAD
static void *cxpProcessThread(void *lpArg);
#elif defined(WITH_THREAD)
static DWORD WINAPI cxpProcessThread(LPVOID lpArg);
#else
static void cxpProcessThread(void *lpArg);
#endif

/*! cxp ThreadPool New

\return pointer to context
*/
cxpThreadPoolPtr
cxpThreadPoolNew(void)
{
  cxpThreadPoolPtr pmThreadPool = NULL;

#ifdef DEBUG
  //cxpCtxtLogPrint(pccArg,2, "cxpThreadPoolNew()");
#endif

#if defined(HAVE_LIBPTHREAD) || WITH_THREAD
  pmThreadPool = (cxpThreadPoolPtr)xmlMemMalloc(sizeof(cxpThreadPool));
  if (pmThreadPool) {
    memset(pmThreadPool, 0, sizeof(cxpThreadPool));
  }
  else {
    PrintFormatLog(TRUE, "No memory");
  }
#endif

  return pmThreadPool;
}
/* end of cxpThreadPoolNew() */

#if 0
/*! 
\param pmThreadPool -- pointer to context
\return TRUE if , FALSE in case of
*/
DWORD WINAPI
cxpThreadPoolDummy(cxpThreadPoolPtr pmThreadPool)
{
  BOOL_T fResult = FALSE;
  int i;

  //cxpCtxtLogPrint(pccArg,1, "cxpThreadPoolDummy(cxpContextPtr pccArg)");

  return 0;
}
/* end of cxpThreadPoolDummy() */
#endif


/*! cxp ThreadPool Append

\param pmThreadPool -- pointer to context
\param pccArg -- pointer to context
\return TRUE if , FALSE in case of
*/
BOOL_T
cxpThreadPoolAppend(cxpThreadPoolPtr pmThreadPool, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;

#ifdef DEBUG
  cxpCtxtLogPrint(pccArg,2, "cxpThreadPoolAppend(cxpThreadPoolPtr pmThreadPool, cxpContextPtr pccArg)");
#endif

#if defined(HAVE_LIBPTHREAD) || WITH_THREAD
  /* create a new thread */
  if (pmThreadPool) {
    /*!\todo check if exceeds */
    //    assert(iThread > -1 && iThread < ciThreads);

    if (pmThreadPool->ciThreads < 1) {
#ifdef HAVE_LIBPTHREAD
      pmThreadPool->mThreadPool = (pthread_t *)xmlMemMalloc(sizeof(pthread_t));
      if (pmThreadPool->mThreadPool) {
	memset(pmThreadPool,0,sizeof(pthread_t));
      }
      else {
	cxpCtxtLogPutsExit(pccArg,55,"No memory");
      }
#elif WITH_THREAD
      pmThreadPool->mThreadPool = (HANDLE *)xmlMemMalloc(sizeof(HANDLE));
      if (pmThreadPool->mThreadPool) {
	memset(pmThreadPool->mThreadPool, 0, sizeof(HANDLE));
      }
      else {
	cxpCtxtLogPutsExit(pccArg,55,"No memory");
      }
#endif
      pmThreadPool->ciThreads = 1;
    }
    else {
      /*! resize pool array */
      void *pRelease;

#ifdef HAVE_LIBPTHREAD
      pRelease = pmThreadPool->mThreadPool;
      pmThreadPool->mThreadPool = (pthread_t *)xmlMemMalloc((pmThreadPool->ciThreads + 1) * sizeof(pthread_t));
      if (pmThreadPool->mThreadPool) {
	memset(pmThreadPool->mThreadPool,0,(pmThreadPool->ciThreads + 1) * sizeof(pthread_t));
	memcpy(pmThreadPool->mThreadPool, pRelease, pmThreadPool->ciThreads * sizeof(pthread_t));
	pmThreadPool->ciThreads++;
      }
      else {
	cxpCtxtLogPutsExit(pccArg,55,"No memory");
      }
#elif WITH_THREAD
      //pmThreadPool->mThreadPool = (HANDLE *)xmlMemRealloc((void *)pmThreadPool->mThreadPool, (pmThreadPool->ciThreads + 1) * sizeof(HANDLE));
      pRelease = pmThreadPool->mThreadPool;
      pmThreadPool->mThreadPool = (HANDLE *)xmlMemMalloc((pmThreadPool->ciThreads + 1) * sizeof(HANDLE));
      if (pmThreadPool->mThreadPool) {
	memset(pmThreadPool->mThreadPool,0,(pmThreadPool->ciThreads + 1) * sizeof(HANDLE));
	memcpy_s(pmThreadPool->mThreadPool,pmThreadPool->ciThreads * sizeof(HANDLE),pRelease,pmThreadPool->ciThreads * sizeof(HANDLE));
	pmThreadPool->ciThreads++;
      }
      else {
	cxpCtxtLogPutsExit(pccArg,55,"No memory");
      }
#endif
      xmlMemFree(pRelease);
      //xmlFree(pRelease);
    }

    cxpCtxtLogPrint(pccArg,2, "Create thread %i in pool '0x%0x'", pmThreadPool->ciThreads - 1, pmThreadPool);

    if (pmThreadPool->mThreadPool) {
#ifdef HAVE_LIBPTHREAD
      fResult = (pthread_create(&(pmThreadPool->mThreadPool[pmThreadPool->ciThreads - 1]), NULL, cxpProcessThread, (void *)pccArg) == 0);
#elif WITH_THREAD
      pmThreadPool->mThreadPool[pmThreadPool->ciThreads - 1] = CreateThread(NULL,0,cxpProcessThread,(LPVOID)pccArg,0,0);
      fResult = (pmThreadPool->mThreadPool[pmThreadPool->ciThreads - 1] != NULL);
#endif
    }
  }
  else {
    //cxpProcessMakeNode(pndChild, pccHere);
  }
#else
  /* simple recursive call if no threads are available */
  //cxpProcessMakeNode(pndChild, pccHere);
#endif

  return fResult;
}
/* end of cxpThreadPoolAppend() */


/*! cxp ThreadPool Join

\param pmThreadPool -- pointer to context
\return TRUE if , FALSE in case of
*/
BOOL_T
cxpThreadPoolJoin(cxpThreadPoolPtr pmThreadPool)
{
  BOOL_T fResult = FALSE;

  if (pmThreadPool) {
    int i;

#ifdef DEBUG
    PrintFormatLog(3,"cxpThreadPoolJoin(cxpThreadPoolPtr pmThreadPool)");
#endif

    /*! wait for all threads */
#if WITH_THREAD
    if (pmThreadPool->ciThreads > 0
	&& WaitForMultipleObjects(pmThreadPool->ciThreads, pmThreadPool->mThreadPool, TRUE, INFINITE) == WAIT_FAILED) {
      PrintFormatLog(1,"WaitForMultipleObjects failed");
    }
#endif

    for (i = 0; i < pmThreadPool->ciThreads; i++) {
      if (pmThreadPool->mThreadPool[i]) {
#ifdef HAVE_LIBPTHREAD
	int iThreadReturn;
	
	iThreadReturn = pthread_join(pmThreadPool->mThreadPool[i], NULL);
	if (iThreadReturn != 0) {
	  PrintFormatLog(1,"pthread_join()");
	}
#elif WITH_THREAD
	DWORD iThreadReturn;

	if (GetExitCodeThread(pmThreadPool->mThreadPool[i], &iThreadReturn) == FALSE) {
	  PrintFormatLog(1,"GetExitCodeThread()");
	}
	CloseHandle(pmThreadPool->mThreadPool[i]);
#endif
      }
    }

    /*! free pool array */
    xmlMemFree(pmThreadPool->mThreadPool);
    xmlMemFree(pmThreadPool);
    fResult = TRUE;
  }

  return fResult;
}
/* end of cxpThreadPoolJoin() */


/*! process this context and release it
*/
#ifdef HAVE_LIBPTHREAD
static void *cxpProcessThread(void *lpArg)
#elif defined(WITH_THREAD)
static DWORD WINAPI cxpProcessThread(LPVOID lpArg)
#else
void cxpProcessThread(void *lpArg)
#endif
{
  if (lpArg) {
    cxpContextPtr pccThread = (cxpContextPtr)lpArg;
    xmlNodePtr pndT = NULL;

#ifdef DEBUG
    //  cxpCtxtLogPrint(pccArg,1,"cxpProcessThread()");
#endif

    if ((pndT = cxpCtxtProcessGetNode(pccThread))) {
      cxpProcessMakeNode(pndT,pccThread);
      //cxpCtxtFree(pccThread); /* release context after processing */
    }
  }

#ifdef HAVE_LIBPTHREAD
    return NULL;
#elif defined(WITH_THREAD)
    return 0;
#else
    return NULL;
#endif
} /* end of cxpProcessThread() */


#ifdef TESTCODE
#include "test/test_cxp_threadp.c"
#endif
