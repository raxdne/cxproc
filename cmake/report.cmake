
###############################################################################
## reporting ##################################################################
###############################################################################

#IF(CXPROC_SHOW_REPORT)
  STRING(TOUPPER "${CMAKE_BUILD_TYPE}" buildtype)
  MESSAGE(STATUS "")
  MESSAGE(STATUS "git tag ..............................: ${GIT_TAG}")
  MESSAGE(STATUS "git branch ...........................: ${GIT_BRANCH}")
  MESSAGE(STATUS "")
  MESSAGE(STATUS "CXPROC configuration summary:")
  MESSAGE(STATUS "")
  MESSAGE(STATUS "  Install prefix .................... : ${CMAKE_INSTALL_PREFIX}")
  MESSAGE(STATUS "  Build type ........................ : ${CMAKE_BUILD_TYPE}")
  MESSAGE(STATUS "  C compiler ........................ : ${CMAKE_C_COMPILER}")
  MESSAGE(STATUS "  C compiler flags .................. : ${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_${buildtype}}")
  MESSAGE(STATUS "")
  MESSAGE(STATUS "  Test directory .................... : ${CXPROC_TEST_DIR}")
  MESSAGE(STATUS "")
#  MESSAGE(STATUS "  Build static binary ............... : ${CXPROC_STATIC}")
#  MESSAGE(STATUS "  Buffer size for cxproc ............ : ${CXPROC_BUFSIZE}")
  MESSAGE(STATUS "  Enable tests ...................... : ${BUILD_TESTING}")
  MESSAGE(STATUS "  Compile cxproc code for legacy  ... : ${CXPROC_LEGACY}")
  MESSAGE(STATUS "  Compile cxproc experimental code .. : ${CXPROC_EXPERIMENTAL}")
  
if (CXPROC_MARKDOWN)
  MESSAGE(STATUS "  Compile cxproc MARKDOWN code ...... : ${CXPROC_MARKDOWN}")
ENDIF (CXPROC_MARKDOWN)

  #  MESSAGE(STATUS "  Compile cxproc with profile ....... : ${CXPROC_PROFILE}")
  
if (CXPROC_DOC)
  MESSAGE(STATUS "  Build documentation........ ....... : ${CXPROC_DOC}")
ENDIF (CXPROC_DOC)
  
IF(CXPROC_PTHREAD)
  MESSAGE(STATUS "  Compile cxproc with pthread ....... : ${CXPROC_PTHREAD}")
ELSEIF (CXPROC_THREAD)
  MESSAGE(STATUS "  Compile cxproc with native threads  : ${CXPROC_THREAD}")
ENDIF (CXPROC_PTHREAD)

IF(CXPROC_PIE)
  MESSAGE(STATUS "  Compile cxproc with PIE ........... : ${CXPROC_PIE}")
ENDIF (CXPROC_PIE)

IF(CXPROC_PETRINET)
  MESSAGE(STATUS "  Compile cxproc with Petrinet ...... : ${CXPROC_PETRINET}")
ENDIF (CXPROC_PETRINET)

#  MESSAGE(STATUS "  Compile cxproc with DIR_INDEX ..... : ${CXPROC_DIR_INDEX}")

IF(CXPROC_JSON)
  MESSAGE(STATUS "  Compile cxproc with JSON  ......... : ${CXPROC_JSON}")
ENDIF (CXPROC_JSON)

  MESSAGE(STATUS "")
  
  IF (CXPROC_DUKTAPE)
    MESSAGE(STATUS "  Compile cxproc with JavaScript .... : ${CXPROC_DUKTAPE}")
  ENDIF ()
  
  IF (CXPROC_PCRE2)
    MESSAGE(STATUS "  Link cxproc with libpcre2 ......... : ${CXPROC_PCRE2}")
  ENDIF ()

  IF (CXPROC_SQLITE3)
    MESSAGE(STATUS "  Link cxproc with libsqlite ........ : ${CXPROC_SQLITE3}")
  ENDIF ()
  
  IF (CXPROC_LIBMAGICK)
    MESSAGE(STATUS "  Link cxproc with ImageMagick ...... : ${CXPROC_LIBMAGICK}")
  ENDIF ()

  IF (CXPROC_LIBEXIF)
    MESSAGE(STATUS "  Link cxproc with libexif .......... : ${CXPROC_LIBEXIF}")
  ENDIF ()

  IF (CXPROC_LIBVORBIS)
    MESSAGE(STATUS "  Link cxproc with libvorbis ........ : ${CXPROC_LIBVORBIS}")
  ENDIF ()

  IF (CXPROC_LIBID3TAG)
    MESSAGE(STATUS "  Link cxproc with libid3tag ........ : ${CXPROC_LIBID3TAG}")
  ENDIF ()

  IF (CXPROC_LIBOGG)
    MESSAGE(STATUS "  Link cxproc with libogg ........ : ${CXPROC_LIBOGG}")
  ENDIF ()

  IF (CXPROC_ARCHIVE)
    MESSAGE(STATUS "  Link cxproc with libarchive ....... : ${CXPROC_ARCHIVE}")
  ENDIF ()

  IF (CXPROC_LIBZ)
    MESSAGE(STATUS "  Link cxproc with libz ............. : ${CXPROC_LIBZ}")
  ENDIF ()

  IF (CXPROC_LZMA)
    MESSAGE(STATUS "  Link cxproc with liblzma .......... : ${CXPROC_LZMA}")
  ENDIF ()

  IF (CXPROC_LIBBZ2)
    MESSAGE(STATUS "  Link cxproc with libbz2 ........... : ${CXPROC_LIBBZ2}")
  ENDIF ()

  IF (CXPROC_CURL)
    MESSAGE(STATUS "  Link cxproc with libcurl .......... : ${CXPROC_CURL}")
  ENDIF ()

  IF (CXPROC_LIBCMARK)
    MESSAGE(STATUS "  Link cxproc with libcmark .......... : ${CXPROC_LIBCMARK}")
  ENDIF ()

  MESSAGE(STATUS "")
  
  #IF (CXPROC_LIBMICROHTTPD)
  #  MESSAGE(STATUS "  Build cxproc-httpd (libmicrohttpd). : ${CXPROC_LIBMICROHTTPD}")
  #ENDIF ()

  IF(MINGW AND NOT CXPROC_STATIC)
    MESSAGE(STATUS "  Non-standard dll names (prefix) . : ${NON_STANDARD_LIB_PREFIX}")
    MESSAGE(STATUS "  Non-standard dll names (suffix) . : ${NON_STANDARD_LIB_SUFFIX}")
  ENDIF (MINGW AND NOT CXPROC_STATIC)
  MESSAGE(STATUS "")
#ENDIF (CXPROC_SHOW_REPORT)

