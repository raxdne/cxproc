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

typedef struct _cxpThreadPool cxpThreadPool;
typedef cxpThreadPool *cxpThreadPoolPtr;
struct _cxpThreadPool {
  /*!
  */
  int ciThreads;
#ifdef HAVE_LIBPTHREAD
  pthread_t *mThreadPool;
#elif WITH_THREAD
  HANDLE *mThreadPool;
#endif
};

extern cxpThreadPoolPtr
cxpThreadPoolNew(void);

extern BOOL_T
cxpThreadPoolAppend(cxpThreadPoolPtr pmThreadPool, cxpContextPtr pccArg);

extern BOOL_T
cxpThreadPoolJoin(cxpThreadPoolPtr pmThreadPool);

#ifdef TESTCODE
extern int
cxpThreadPoolTest(cxpContextPtr pccArg);
#endif
