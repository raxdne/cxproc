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

/*! common basics of all modules
 */

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <sys/stat.h>
#include <assert.h>
#include <limits.h>

#include <errno.h>

#include <iconv.h>

#ifndef HAVE_PCRE2
typedef int  pcre2_code; 	/*! for configuration reasons only */
#endif

#define CXP_NAMESPACE_URL      "http://www.tenbusch.info/cxproc"

/*! numeric index type */
typedef int index_t;

/*! value for error */
#define ERROR_INDEX -1

#define NAME_FILE_INDEX ".index.pie"

#define NAME_TMP_INDEX "#index.pie#"

#define NAME_CONFIG_INDEX ".index.cxp"

/*! size of static buffers */
#define BUFFER_LENGTH ((size_t)(4 * 1024))

#ifndef BOOL_T
  #ifdef __cplusplus
    /*! use native C++ types */
    #include <stdbool.h>
    #define BOOL_T bool
  #else
    /*! use int as boolean type */
    #define BOOL_T int
  #endif
#endif

#ifndef FALSE
  #ifdef __cplusplus
    /*! use native C++ types */
    #define FALSE  false
    #define TRUE   true
  #else
    /*! use int as boolean type in C */
    #define FALSE  (0)
    #define TRUE   (!FALSE)
  #endif
#endif

#ifdef TESTCODE
  #define RUNTEST (TRUE) // (i == n_ok)
  #ifdef HAVE_LIBCURL
    #define TESTHTTP 1
  #else
    #define TESTHTTP 0
  #endif
#else
  #define RUNTEST (FALSE)
#endif

#define SKIPTEST (FALSE)


/* global macros */

#define SIZE_KILO (1024)

#define SIZE_MEGA (SIZE_KILO * SIZE_KILO)

#define SIZE_GIGA (SIZE_KILO * SIZE_MEGA)

#include "version.h"

#ifdef _MSC_VER
#include <windows.h>
#include <WinBase.h>
#include <WinDef.h>
#include <aclapi.h>
#include <share.h>
#include <fcntl.h>

#if defined(__cplusplus)
#include <tchar.h>
#include <strsafe.h>
#include <shobjidl.h>
#include <shlguid.h>
#include <atlbase.h> 
#endif

#include <objbase.h>
#include <shlobj.h>
#include <strsafe.h>
#include <ole2.h>

#include <direct.h>

#include <commdlg.h>

#define MAXFILENAME (256)

#pragma warning (disable : 4706)
#pragma warning (disable : 4100)
#pragma warning (disable : 4101)
#pragma warning (disable : 4127)

#else
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/utsname.h>
#define MAX_PATH BUFFER_LENGTH
#endif

#ifndef TIME_H_INCLUDED
#include <time.h>
#endif

/* https://www.cs.tut.fi/~jkorpela/round.html */
#define RoundToInt(x) ((x)>=0?(int)((x)+0.5):(int)((x)-0.5))

#define STR_IS_EMPTY(S) (S == NULL || *S == 0)

#define STR_IS_NOT_EMPTY(S) (STR_IS_EMPTY(S) == FALSE)


/* https://unicode.org/charts/
*/

#define STR_UTF8_LEFT_SINGLE_QUOTATION_MARK "\xE2\x80\x98"

#define STR_UTF8_RIGHT_SINGLE_QUOTATION_MARK "\xE2\x80\x99"

#define STR_UTF8_SINGLE_LOW_9_QUOTATION_MARK "\xE2\x80\x9A"

#define STR_UTF8_SINGLE_HIGH_REVERSED_9_QUOTATION_MARK "\xE2\x80\x9B"

#define STR_UTF8_LEFT_DOUBLE_QUOTATION_MARK "\xE2\x80\x9C"

#define STR_UTF8_RIGHT_DOUBLE_QUOTATION_MARK "\xE2\x80\x9D"

#define STR_UTF8_DOUBLE_LOW_9_QUOTATION_MARK "\xE2\x80\x9E"

#define STR_UTF8_DOUBLE_HIGH_REVERSED_9_QUOTATION_MARK "\xE2\x80\x9F"

#define STR_UTF8_SINGLE_LEFT_POINTING_ANGLE_QUOTATIONMARK "\xE2\x80\xB9"

#define STR_UTF8_SINGLE_RIGHT_POINTING_ANGLE_QUOTATIONMARK "\xE2\x80\xBA"

#define STR_UTF8_LEFT_POINTING_DOUBLE_ANGLE_QUOTATIONMARK "\xC2\xAB"

#define STR_UTF8_RIGHT_POINTING_DOUBLE_ANGLE_QUOTATIONMARK "\xC2\xBB"

#define STR_UTF8_LEFTWARDS_ARROW "\xE2\x86\x90"

#define STR_UTF8_RIGHTWARDS_ARROW "\xE2\x86\x92"

#define STR_UTF8_LEFT_RIGHT_ARROW "\xE2\x86\x94"

#define STR_UTF8_LEFTWARDS_SINGLE_ARROW "\xE2\x87\x90"

#define STR_UTF8_RIGHTWARDS_SINGLE_ARROW "\xE2\x87\x92"

#define STR_UTF8_LEFT_RIGHT_SINGLE_ARROW "\xE2\x87\x94"

#define STR_UTF8_EN_DASH "\xE2\x80\x93"

#define STR_UTF8_EM_DASH "\xE2\x80\x94"

#define STR_UTF8_HEAVY_CHECK_MARK "\xE2\x9C\x94"

#define STR_UTF8_HEAVY_BALLOT_X "\xE2\x9C\x98"

#define STR_PIE_OK STR_UTF8_HEAVY_CHECK_MARK

#define STR_PIE_CANCEL STR_UTF8_HEAVY_BALLOT_X

#define STR_UTF8_MINUS "\xE2\x88\x92"

