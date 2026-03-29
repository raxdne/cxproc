#
#
#
cmake_minimum_required(VERSION 3.20) # setting this is required

project(cxproc)            # this sets the project name

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Profile" CACHE STRING "" FORCE) 
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Default build type: Release" FORCE)

# for profiling s. https://stackoverflow.com/questions/26491948/how-to-use-gprof-with-cmake
# or see test/main.sh and https://github.com/jrfonseca/gprof2dot
# http://gernotklingler.com/blog/gprof-valgrind-gperftools-evaluation-tools-application-level-cpu-profiling-linux/
#set(CMAKE_BUILD_TYPE RelWithDebInfow)

# s. https://cmake.org/Wiki/CMake_Useful_Variables

# CMAKE_POLICY(SET CMP0026 OLD)

SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" "${PROJECT_SOURCE_DIR}/cmake/Modules" "${PROJECT_SOURCE_DIR}/cmake/Options")

SET(CMAKE_VERBOSE_MAKEFILE ON)

# http://www.vtk.org/Wiki/CMake:Eclipse_UNIX_Tutorial
# http://stackoverflow.com/questions/9453851/how-to-configure-eclipse-cdt-for-cmake
#FIND_PACKAGE(EclipseCDT4)

# (cmake --graphviz=cxproc.dot -B ../x86_64-gnu-linux/build && dot -T png -o cxproc.png cxproc.dot.cxproc)

# default value for Testing
SET(CXPROC_HTTPPREFIX "http://localhost:8183/" CACHE STRING "default URL for tests")

# flag for Code check (see target "cpp-check")
SET(CXPROC_CHECK OFF)

FIND_PATH(CXPROC_SRC_DIR
  NAME cxproc.c
  PATHS "src")

FIND_PATH(CXPROC_SRCLIB_DIR
  NAME cgi-util
  PATHS "third-party")

cmake_path(GET PROJECT_BINARY_DIR PARENT_PATH _CXPROC_PREFIX)
SET(CXPROC_PREFIX ${_CXPROC_PREFIX} CACHE PATH "Prefix")
SET(CXPROC_TEMP_DIR "${CXPROC_PREFIX}/tmp")
# working directory for tests
SET(CXPROC_TEST_DIR "${PROJECT_SOURCE_DIR}/test")

# helper: default path to use as HINT
SET(HINT_DIR ${CXPROC_PREFIX})
#cmake_path(NORMAL_PATH CXPROC_PREFIX OUTPUT_VARIABLE HINT_DIR)
SET(HINT_DIR_INCLUDE "${CXPROC_PREFIX}/include")
SET(HINT_DIR_LIB "${CXPROC_PREFIX}/lib")
SET(HINT_DIR_LIBD "${CXPROC_PREFIX}/debug/lib")
SET(HINT_DIR_BIN "${CXPROC_PREFIX}/bin")

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/third-party"
  "${PROJECT_SOURCE_DIR}/third-party/option"
  ${CXPROC_SRC_DIR}
  "${CXPROC_SRC_DIR}/option"
  ${HINT_DIR_INCLUDE}
  )

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/debug)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/release)

string(TIMESTAMP TODAY "%Y%m%d")

# https://www.mattkeeter.com/blog/2018-01-06-versioning/

execute_process(COMMAND git describe --tags --long
  OUTPUT_VARIABLE GIT_TAG
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  RESULT_VARIABLE STATUS
)

IF(STATUS AND NOT STATUS EQUAL "0")
  MESSAGE(ERROR " it's not a git clone, use a fixed development version")
  SET(GIT_TAG "v2.0-dummy")
  SET(GIT_BRANCH "develop")
ELSE()
  execute_process(COMMAND git branch --show-current
    OUTPUT_VARIABLE GIT_BRANCH
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  )
  string(STRIP "${GIT_BRANCH}" GIT_BRANCH)
ENDIF()

string(STRIP ${GIT_TAG} GIT_TAG)
string(REGEX REPLACE "^v" "" INFO ${GIT_TAG})
string(FIND ${INFO} "." POS_DOT)
string(FIND ${INFO} "-" POS_MINUS)
string(SUBSTRING ${INFO} 0 ${POS_DOT} GIT_TAG_MAJOR)
MATH(EXPR POS_DOT "${POS_DOT}+1")
MATH(EXPR L_MINUS "${POS_MINUS} - ${POS_DOT}")
string(SUBSTRING ${INFO} ${POS_DOT} ${L_MINUS} GIT_TAG_MINOR)

include(docs)

# Configuration checks

INCLUDE(CheckIncludeFile)
INCLUDE(CheckFunctionExists)
INCLUDE(CheckTypeSize)

CHECK_INCLUDE_FILE(assert.h     HAVE_ASSERT_H)
CHECK_INCLUDE_FILE(ctype.h      HAVE_CTYPE_H)
CHECK_INCLUDE_FILE(dirent.h     HAVE_DIRENT_H)
CHECK_INCLUDE_FILE(errno.h      HAVE_ERRNO_H)
CHECK_INCLUDE_FILE(inttypes.h   HAVE_INTTYPES_H)
CHECK_INCLUDE_FILE(limits.h     HAVE_LIMITS_H)
CHECK_INCLUDE_FILE(math.h       HAVE_MATH_H)
CHECK_INCLUDE_FILE(stdarg.h     HAVE_stdarg_H)
CHECK_INCLUDE_FILE(stddef.h     HAVE_stddef_H)
CHECK_INCLUDE_FILE(stdint.h     HAVE_STDINT_H)
CHECK_INCLUDE_FILE(stdio.h      HAVE_STDIO_H)
CHECK_INCLUDE_FILE(stdlib.h     HAVE_STDLIB_H)
CHECK_INCLUDE_FILE(string.h     HAVE_STRING_H)
CHECK_INCLUDE_FILE(sys/dir.h    HAVE_SYS_DIR_H)
CHECK_INCLUDE_FILE(sys/stat.h   HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILE(sys/types.h  HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILE(time.h       HAVE_TIME_H)
CHECK_INCLUDE_FILE(unistd.h     HAVE_UNISTD_H)
IF(MSVC)
  CHECK_INCLUDE_FILE(windows.h    HAVE_WINDOWS_H)
ELSE(MSVC)
ENDIF(MSVC)

###############################################################################
## target definitions #########################################################
###############################################################################

ADD_EXECUTABLE(filex
  ${CXPROC_SRC_DIR}/basics.h
  ${CXPROC_SRC_DIR}/dom.c
  ${CXPROC_SRC_DIR}/dom.h
#  ${CXPROC_SRC_DIR}/git_info.h
#  ${CXPROC_SRC_DIR}/git_revision.h
  ${CXPROC_SRC_DIR}/calendar_element.c
  ${CXPROC_SRC_DIR}/calendar_element.h
  ${CXPROC_SRC_DIR}/plain_text.c
  ${CXPROC_SRC_DIR}/plain_text.h
  ${CXPROC_SRC_DIR}/res_node/res_mime.c
  ${CXPROC_SRC_DIR}/res_node/res_mime.h
  ${CXPROC_SRC_DIR}/res_node/res_node.c
  ${CXPROC_SRC_DIR}/res_node/res_node.h
  ${CXPROC_SRC_DIR}/res_node/res_node_formats.c
  ${CXPROC_SRC_DIR}/res_node/res_node_formats.h
  ${CXPROC_SRC_DIR}/res_node/res_node_io.c
  ${CXPROC_SRC_DIR}/res_node/res_node_io.h
  ${CXPROC_SRC_DIR}/res_node/res_node_zip.c
  ${CXPROC_SRC_DIR}/res_node/res_node_zip.h
  ${CXPROC_SRC_DIR}/res_node/res_node_list.c
  ${CXPROC_SRC_DIR}/res_node/res_node_list.h
  ${CXPROC_SRC_DIR}/res_node/res_node_ops.c
  ${CXPROC_SRC_DIR}/res_node/res_node_ops.h
  ${CXPROC_SRC_DIR}/res_node/res_node_properties.c
  ${CXPROC_SRC_DIR}/res_node/res_path.c
  ${CXPROC_SRC_DIR}/res_node/res_path.h
  ${CXPROC_SRC_DIR}/utils.c
  ${CXPROC_SRC_DIR}/utils.h
  ${CXPROC_SRC_DIR}/version.h
  ${CXPROC_SRC_DIR}/filex.c
 )

ADD_EXECUTABLE(dir2csv
  ${CXPROC_SRC_DIR}/basics.h
  ${CXPROC_SRC_DIR}/dom.c
  ${CXPROC_SRC_DIR}/dom.h
  ${CXPROC_SRC_DIR}/res_node/res_mime.c
  ${CXPROC_SRC_DIR}/res_node/res_mime.h
  ${CXPROC_SRC_DIR}/res_node/res_node.c
  ${CXPROC_SRC_DIR}/res_node/res_node.h
  ${CXPROC_SRC_DIR}/res_node/res_node_formats.c
  ${CXPROC_SRC_DIR}/res_node/res_node_formats.h
  ${CXPROC_SRC_DIR}/res_node/res_node_io.c
  ${CXPROC_SRC_DIR}/res_node/res_node_io.h
  ${CXPROC_SRC_DIR}/res_node/res_node_zip.c
  ${CXPROC_SRC_DIR}/res_node/res_node_zip.h
  ${CXPROC_SRC_DIR}/res_node/res_node_list.c
  ${CXPROC_SRC_DIR}/res_node/res_node_list.h
  ${CXPROC_SRC_DIR}/res_node/res_node_ops.c
  ${CXPROC_SRC_DIR}/res_node/res_node_ops.h
  ${CXPROC_SRC_DIR}/res_node/res_path.c
  ${CXPROC_SRC_DIR}/res_node/res_path.h
  ${CXPROC_SRC_DIR}/utils.c
  ${CXPROC_SRC_DIR}/utils.h
  ${CXPROC_SRC_DIR}/version.h
  ${CXPROC_SRC_DIR}/dir2csv.c
  )

ADD_EXECUTABLE(dir2sqlite
  ${CXPROC_SRC_DIR}/basics.h
  ${CXPROC_SRC_DIR}/dom.c
  ${CXPROC_SRC_DIR}/dom.h
  ${CXPROC_SRC_DIR}/res_node/res_mime.c
  ${CXPROC_SRC_DIR}/res_node/res_mime.h
  ${CXPROC_SRC_DIR}/res_node/res_node.c
  ${CXPROC_SRC_DIR}/res_node/res_node.h
  ${CXPROC_SRC_DIR}/res_node/res_node_formats.c
  ${CXPROC_SRC_DIR}/res_node/res_node_formats.h
  ${CXPROC_SRC_DIR}/res_node/res_node_io.c
  ${CXPROC_SRC_DIR}/res_node/res_node_io.h
  ${CXPROC_SRC_DIR}/res_node/res_node_zip.c
  ${CXPROC_SRC_DIR}/res_node/res_node_zip.h
  ${CXPROC_SRC_DIR}/res_node/res_node_list.c
  ${CXPROC_SRC_DIR}/res_node/res_node_list.h
  ${CXPROC_SRC_DIR}/res_node/res_node_ops.c
  ${CXPROC_SRC_DIR}/res_node/res_node_ops.h
  ${CXPROC_SRC_DIR}/res_node/res_path.c
  ${CXPROC_SRC_DIR}/res_node/res_path.h
  ${CXPROC_SRC_DIR}/utils.c
  ${CXPROC_SRC_DIR}/utils.h
  ${CXPROC_SRC_DIR}/version.h
  ${CXPROC_SRC_DIR}/dir2sqlite.c
  )

ADD_EXECUTABLE(pietextx
  ${CXPROC_SRC_DIR}/basics.h
  ${CXPROC_SRC_DIR}/dom.c
  ${CXPROC_SRC_DIR}/dom.h
  ${CXPROC_SRC_DIR}/utils.c
  ${CXPROC_SRC_DIR}/utils.h
  ${CXPROC_SRC_DIR}/res_node/res_mime.c
  ${CXPROC_SRC_DIR}/res_node/res_mime.h
  ${CXPROC_SRC_DIR}/res_node/res_path.c
  ${CXPROC_SRC_DIR}/res_node/res_path.h
  ${CXPROC_SRC_DIR}/version.h
  ${CXPROC_SRC_DIR}/pietextx.c
  )

target_compile_definitions(pietextx PUBLIC PIE_STANDALONE)

#
add_custom_command(
  OUTPUT  ${CXPROC_SRC_DIR}/cxp/cxp_rng.h
  DEPENDS ${PROJECT_SOURCE_DIR}/xml/schema/cxp.rng ${PROJECT_SOURCE_DIR}/xml/xsl/xml2c.xsl
  COMMAND xsltproc -o ${CXPROC_SRC_DIR}/cxp/cxp_rng.h ${PROJECT_SOURCE_DIR}/xml/xsl/xml2c.xsl ${PROJECT_SOURCE_DIR}/xml/schema/cxp.rng
  )
 
SET(CXPROC_HEADERS
  ${CXPROC_SRC_DIR}/version.h
#  ${CXPROC_SRC_DIR}/git_revision.h
#  ${CXPROC_SRC_DIR}/git_info.h
  ${CXPROC_SRC_DIR}/basics.h
  ${CXPROC_SRC_DIR}/cxp/cxp_dtd.h
#  ${CXPROC_SRC_DIR}/cxp/cxp_rng.h
  ${CXPROC_SRC_DIR}/cxp/cxp_context_enc.h
  ${CXPROC_SRC_DIR}/cxp/cxp_context.h
  ${CXPROC_SRC_DIR}/cxp/cxp_context_cli.h
  ${CXPROC_SRC_DIR}/cxp/cxp_context_cache.h
  ${CXPROC_SRC_DIR}/cxp/cxp_subst.h
  ${CXPROC_SRC_DIR}/cxp/cxp_dir.h
  ${CXPROC_SRC_DIR}/calendar_element.h
  ${CXPROC_SRC_DIR}/cxp/cxp_calendar.h
  ${CXPROC_SRC_DIR}/dom.h
  ${CXPROC_SRC_DIR}/res_node/res_node.h
  ${CXPROC_SRC_DIR}/res_node/res_node_formats.h
  ${CXPROC_SRC_DIR}/res_node/res_node_list.h
  ${CXPROC_SRC_DIR}/res_node/res_node_io.h
  ${CXPROC_SRC_DIR}/res_node/res_node_zip.h
  ${CXPROC_SRC_DIR}/res_node/res_node_ops.h
  ${CXPROC_SRC_DIR}/res_node/res_path.h
  ${CXPROC_SRC_DIR}/res_node/res_mime.h
  ${CXPROC_SRC_DIR}/plain_text.h
  ${CXPROC_SRC_DIR}/utils.h
  )

SET(CXPROC_SOURCES
  ${CXPROC_SRC_DIR}/cxp/cxp_context.c
  ${CXPROC_SRC_DIR}/cxp/cxp_context_cli.c
  ${CXPROC_SRC_DIR}/cxp/cxp_context_cache.c
  ${CXPROC_SRC_DIR}/cxp/cxp_context_enc.c
  ${CXPROC_SRC_DIR}/cxp/cxp_subst.c
  ${CXPROC_SRC_DIR}/cxp/cxp.c
  ${CXPROC_SRC_DIR}/cxp/cxp_dir.c
  ${CXPROC_SRC_DIR}/calendar_element.c
  ${CXPROC_SRC_DIR}/cxp/cxp_calendar.c
  ${CXPROC_SRC_DIR}/dom.c
  ${CXPROC_SRC_DIR}/res_node/res_node.c
  ${CXPROC_SRC_DIR}/res_node/res_node_formats.c
  ${CXPROC_SRC_DIR}/res_node/res_node_list.c
  ${CXPROC_SRC_DIR}/res_node/res_node_properties.c
  ${CXPROC_SRC_DIR}/res_node/res_node_io.c
  ${CXPROC_SRC_DIR}/res_node/res_node_zip.c
  ${CXPROC_SRC_DIR}/res_node/res_node_ops.c
  ${CXPROC_SRC_DIR}/res_node/res_path.c
  ${CXPROC_SRC_DIR}/res_node/res_mime.c
  ${CXPROC_SRC_DIR}/plain_text.c
  ${CXPROC_SRC_DIR}/utils.c
)

ADD_EXECUTABLE(cxproc
 ${CXPROC_SRC_DIR}/cxproc.c
 ${CXPROC_SOURCES} ${CXPROC_HEADERS})

ADD_EXECUTABLE(cxproc-cgi
 ${CXPROC_SRC_DIR}/cxproc-cgi.c
 ${CXPROC_SRC_DIR}/cxp/cxp_context_cgi.c
 ${CXPROC_SRC_DIR}/cxp/cxp_context_cgi.h
 ${CXPROC_SRCLIB_DIR}/cgi-util/cgi-util.c
 ${CXPROC_SRCLIB_DIR}/cgi-util/cgi-util.h
 ${CXPROC_SOURCES} ${CXPROC_HEADERS}
 )

target_compile_definitions(cxproc-cgi
 PUBLIC HAVE_CGI)

IF(CXPROC_TESTS)
  ADD_EXECUTABLE(cxproc-test
    ${CXPROC_SRC_DIR}/cxproc-test.c
    ${CXPROC_SOURCES} ${CXPROC_HEADERS})

  target_compile_definitions(cxproc-test PUBLIC
    TESTCODE
    BUILDPREFIX="${CXPROC_PREFIX}/"
    TESTPREFIX="${CXPROC_TEST_DIR}/"
    TEMPPREFIX="${CXPROC_TEMP_DIR}/"
    HTTPPREFIX="${CXPROC_HTTPPREFIX}")
ENDIF ()

#IF (LIBMICROHTTPD_FOUND)
#  ADD_EXECUTABLE(cxproc-httpd
#    ${CXPROC_SRC_DIR}/cxproc-httpd.c
#    ${CXPROC_SOURCES} ${CXPROC_HEADERS})
#
#  target_compile_definitions(cxproc-httpd
#    PUBLIC HAVE_HTTPD)
#ENDIF ()

IF(MSVC)
  target_sources(filex      PUBLIC ${CXPROC_SRC_DIR}/version.rc)
  target_sources(dir2csv    PUBLIC ${CXPROC_SRC_DIR}/version.rc)
  target_sources(dir2sqlite PUBLIC ${CXPROC_SRC_DIR}/version.rc)
  target_sources(pietextx   PUBLIC ${CXPROC_SRC_DIR}/version.rc)
  target_sources(cxproc     PUBLIC ${CXPROC_SRC_DIR}/version.rc)
  target_sources(cxproc-cgi PUBLIC ${CXPROC_SRC_DIR}/version.rc)
ENDIF(MSVC)

################################################################################
#
# dummy target to compile as C++ (without linking)
#

IF(CXPROC_CHECK)
  add_library(cpp-check OBJECT ${CXPROC_SOURCES} ${CXPROC_SRC_DIR}/cxp/cxp_context_cgi.c ${CXPROC_SRC_DIR}/cxproc-cgi.c ${CXPROC_SRC_DIR}/cxproc.c)

  target_compile_definitions(cpp-check PUBLIC
    TESTCODE
    BUILDPREFIX="${CXPROC_PREFIX}/"
    TESTPREFIX="${CXPROC_TEST_DIR}/"
    TEMPPREFIX="${CXPROC_TEMP_DIR}/"
    HTTPPREFIX="${CXPROC_HTTPPREFIX}")

  IF(MSVC)
    target_compile_options(cpp-check PUBLIC /W4 /permissive- /analyze)
    # /TP /std:c++17 /Od  /WX /Wall
    #  /errorreport:prompt
    #  /showIncludes
  ELSE(MSVC)
    target_compile_options(cpp-check PUBLIC -x c++ -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable -Wno-sign-compare -fmax-errors=25 -funsigned-char -fpermissive)
  ENDIF (MSVC)
ENDIF (CXPROC_CHECK)

################################################################################
#
# pthread
#

# FIND_PACKAGE( Threads )

# IF(CMAKE_USE_PTHREADS_INIT)
#   MESSAGE(STATUS "Found optional libpthtread: ${CMAKE_THREAD_LIBS_INIT}")
#   OPTION (CXPROC_PTHREAD "Enable support for linking cxproc with libpthread." OFF)
# ELSE ()
#   OPTION (CXPROC_THREAD "Enable support native threads." OFF)
# ENDIF ()
# MARK_AS_ADVANCED(CXPROC_THREAD)

# SET(THREAD_FILES
#   ${CXPROC_SRC_DIR}/cxp/cxp_threadp.h
#   ${CXPROC_SRC_DIR}/cxp/cxp_threadp.c
#   )

# IF(CXPROC_PTHREAD)
#   target_sources(cxproc PUBLIC ${THREAD_FILES})
#   target_sources(cxproc-cgi PUBLIC ${THREAD_FILES})
#   target_sources(cxproc-test PUBLIC ${THREAD_FILES})

#   add_compile_definitions(HAVE_LIBPTHREAD)
# ELSEIF (CXPROC_THREAD)
#   target_sources(cxproc PUBLIC ${THREAD_FILES})
#   target_sources(cxproc-cgi PUBLIC ${THREAD_FILES})
#   target_sources(cxproc-test PUBLIC ${THREAD_FILES})

#   add_compile_definitions(WITH_THREAD)
# ENDIF (CXPROC_PTHREAD)

################################################################################
#
# pcre2 (impact on PIE)
#

FIND_PACKAGE( PCRE2 )

IF (PCRE2_LIBRARY)
  OPTION(CXPROC_PCRE2 "Enable support for linking cxproc with libpcre2." ON)
ENDIF ()

IF (CXPROC_PCRE2)
  target_compile_definitions(cxproc PUBLIC HAVE_PCRE2 PCRE2_CODE_UNIT_WIDTH=8)
  target_compile_definitions(cxproc-cgi PUBLIC HAVE_PCRE2 PCRE2_CODE_UNIT_WIDTH=8)
  IF(CXPROC_TESTS)
    target_compile_definitions(cxproc-test PUBLIC HAVE_PCRE2 PCRE2_CODE_UNIT_WIDTH=8)
  ENDIF ()
  IF(CXPROC_CHECK)
    target_compile_definitions(cpp-check PUBLIC HAVE_PCRE2 PCRE2_CODE_UNIT_WIDTH=8)
  ENDIF ()
  IF (${PCRE2_LIBRARY} MATCHES ".+static.*\\.lib$" OR ${PCRE2_LIBRARY} MATCHES ".+\\.a$")
    #ADD_LIBRARY(pcre2-8 STATIC IMPORTED)
    add_compile_definitions(PCRE2_STATIC)
  ENDIF ()
ELSE ()
ENDIF ()

################################################################################
#
# cmark
#

FIND_PACKAGE( LibCMark )

################################################################################
#
# libzip
#

FIND_PACKAGE( LibZip )

################################################################################
#
# libcurl
#

FIND_PACKAGE( CURL )

IF (CURL_LIBRARY_RELEASE)
  OPTION(CXPROC_CURL "Enable support for linking cxproc with libcurl." OFF)
  MARK_AS_ADVANCED(CXPROC_CURL)
ELSE ()
  SET(CXPROC_CURL OFF)
ENDIF ()

################################################################################
#
# zlib
#

FIND_PACKAGE( ZLIB )

IF (ZLIB_INCLUDE_DIR)
  # package found
ELSE(ZLIB_INCLUDE_DIR)
  find_path(ZLIB_INCLUDE_DIR NAMES "zlib.h" HINTS ${HINT_DIR_INCLUDE}) 
  find_library(ZLIB_LIBRARY_DEBUG NAMES "zlibd" HINTS ${HINT_DIR_LIBD})
  find_library(ZLIB_LIBRARY_RELEASE NAMES "zlib" HINTS ${HINT_DIR_LIB})
ENDIF (ZLIB_INCLUDE_DIR)

IF (ZLIB_LIBRARY_RELEASE)
  SET(CXPROC_LIBZ ON)
ENDIF (ZLIB_LIBRARY_RELEASE)

################################################################################
#
# liblzma
#

FIND_PACKAGE( LibLZMA )

IF (LIBLZMA_INCLUDE_DIR)
  # package found
ELSE(LIBLZMA_INCLUDE_DIR)
  find_path(LIBLZMA_INCLUDE_DIR NAMES "lzma.h" HINTS ${HINT_DIR_INCLUDE}) 
  find_library(LIBLZMA_LIBRARY_DEBUG NAMES "lzma" HINTS ${HINT_DIR_LIBD})
  find_library(LIBLZMA_LIBRARY_RELEASE NAMES "lzma" HINTS ${HINT_DIR_LIB})
ENDIF (LIBLZMA_INCLUDE_DIR)

IF (LIBLZMA_LIBRARIES)
  SET(CXPROC_LZMA ON)
ENDIF (LIBLZMA_LIBRARIES)

###############################################################################

include(tests)

IF (CXPROC_EXPERIMENTAL)
  include(archive)
ELSE (CXPROC_EXPERIMENTAL)
  UNSET(CXPROC_ARCHIVE CACHE)
ENDIF (CXPROC_EXPERIMENTAL)

include(audio)
include(database)
include(image)
include(json)
include(petrinet)
include(pie)
include(c-dt)
# TODO: include(rp)
include(script)
# TODO: include(vorbiscomment)


################################################################################
#
# libiconv
#

FIND_PATH(LIBICONV_INCLUDE_DIR NAME iconv.h HINTS ${HINT_DIR_INCLUDE})

IF (LIBICONV_INCLUDE_DIR)
  IF (MSVC)
    find_library(LIBICONV_LIBRARY NAMES "iconv" HINTS ${HINT_DIR_LIB})
    find_library(LIBCHARSET_LIBRARY NAMES "charset" HINTS ${HINT_DIR_LIB})
    IF (LIBICONV_LIBRARY AND LIBCHARSET_LIBRARY)
      SET(LIBICONV_FOUND ON)
    ELSE ()
      # select interactively
    ENDIF ()
  ELSE ()
    # no library file required on Linux
    SET(LIBICONV_FOUND ON)
  ENDIF ()
ENDIF (LIBICONV_INCLUDE_DIR)

IF (LIBICONV_FOUND)
  MESSAGE(STATUS "Found required libiconv: ${LIBICONV_LIBRARY} ${LIBCHARSET_LIBRARY}")
ELSE (LIBICONV_FOUND)
  MESSAGE(FATAL_ERROR "libiconv must be available")
ENDIF (LIBICONV_FOUND)

add_compile_definitions(ICONV_CODE_UNIT_WIDTH=8)

################################################################################
#
# libxml2
#

FIND_PACKAGE( LibXml2 )

IF (LIBXML2_INCLUDE_DIR AND LIBXML2_LIBRARY)
  # package found
ELSE()
  #
  find_path(_LIBXML2_INCLUDE_DIR NAMES "tree.h" HINTS "${HINT_DIR_INCLUDE}/libxml2/libxml") 
  cmake_path(GET _LIBXML2_INCLUDE_DIR PARENT_PATH _LIBXML2_INCLUDE_PARENT_DIR)
  #message(ERROR " LIBXML2_INCLUDE_DIR=${_LIBXML2_INCLUDE_PARENT_DIR}")
  SET(LIBXML2_INCLUDE_DIR ${_LIBXML2_INCLUDE_PARENT_DIR} CACHE PATH "" FORCE)
  find_library(LIBXML2_LIBRARY NAMES "libxml2" HINTS ${HINT_DIR_LIB})
  find_program(LIBXML2_XMLLINT_EXECUTABLE NAMES "xmllint" HINTS ${HINT_DIR_BIN})
ENDIF ()

IF (LIBXML2_INCLUDE_DIR AND LIBXML2_LIBRARY)
  INCLUDE_DIRECTORIES(${LIBXML2_INCLUDE_DIR})
  #add_compile_definitions(${LIBXML2_DEFINITIONS})
  IF ("${LIBXML2_LIBRARY}" MATCHES ".+static.*\\.lib" OR "${LIBXML2_TEST}" MATCHES ".+\\.a")
    add_compile_definitions(LIBXML_STATIC)
    IF(MSVC)
      target_link_libraries(filex ws2_32.lib)
      target_link_libraries(dir2sqlite ws2_32.lib)
      target_link_libraries(dir2csv ws2_32.lib)
      target_link_libraries(pietextx ws2_32.lib)
      target_link_libraries(cxproc ws2_32.lib)
      target_link_libraries(cxproc-cgi ws2_32.lib)
      IF(CXPROC_TESTS)
        target_link_libraries(cxproc-test ws2_32.lib)
      ENDIF ()
      #    IF (LIBMICROHTTPD_FOUND)
      #      target_link_libraries(cxproc-httpd ws2_32.lib)
      #    ENDIF ()
    ENDIF (MSVC)
  ENDIF ()
ELSE()
  MESSAGE(FATAL_ERROR " 'libxml2' is a mandatory library")
ENDIF ()

################################################################################
#
# libxslt
#

FIND_PACKAGE( LibXslt )

IF (LIBXSLT_INCLUDE_DIR)
  # package found
ELSE(LIBXSLT_INCLUDE_DIR)
  SET(LIBXSLT_INCLUDE_DIR ${HINT_DIR_INCLUDE} CACHE PATH "include files of libxslt")
  find_path(_LIBXSLT_INCLUDE_DIR NAMES "xslt.h" HINTS "${HINT_DIR_INCLUDE}/libxslt") 
  cmake_path(GET _LIBXSLT_INCLUDE_DIR PARENT_PATH LIBXSLT_INCLUDE_DIR)
  #message(ERROR " LIBXSLT_INCLUDE_DIR=${LIBXSLT_INCLUDE_DIR}")
  SET(LIBXSLT_INCLUDE_DIR ${LIBXSLT_INCLUDE_DIR} CACHE PATH "" FORCE)
  find_library(LIBXSLT_LIBRARY NAMES "libxslt" HINTS ${HINT_DIR_LIB})
  find_program(LIBXSLT_XSLTPROC_EXECUTABLE NAMES "xsltproc" HINTS ${HINT_DIR_BIN})
ENDIF (LIBXSLT_INCLUDE_DIR)

IF (LIBXSLT_EXSLT_INCLUDE_DIR)
  # package found
ELSE(LIBXSLT_EXSLT_INCLUDE_DIR)
  SET(LIBXSLT_EXSLT_INCLUDE_DIR ${HINT_DIR_INCLUDE} CACHE PATH "include files of libexslt")
  find_path(_LIBXSLT_EXSLT_INCLUDE_DIR NAMES "exslt.h" HINTS "${HINT_DIR_INCLUDE}/libexslt") 
  cmake_path(GET _LIBXSLT_EXSLT_INCLUDE_DIR PARENT_PATH LIBXSLT_EXSLT_INCLUDE_DIR)
  #message(ERROR " LIBXSLT_EXSLT_INCLUDE_DIR=${LIBXSLT_EXSLT_INCLUDE_DIR}")
  SET(LIBXSLT_EXSLT_INCLUDE_DIR ${LIBXSLT_EXSLT_INCLUDE_DIR} CACHE PATH "" FORCE)
  find_library(LIBXSLT_EXSLT_LIBRARY NAMES "libexslt" HINTS ${HINT_DIR_LIB})
  find_program(LIBXSLT_XSLTPROC_EXECUTABLE NAMES "xsltproc" HINTS ${HINT_DIR_BIN})
ENDIF (LIBXSLT_EXSLT_INCLUDE_DIR)

IF (LIBXSLT_LIBRARIES)
  INCLUDE_DIRECTORIES(${LIBXSLT_INCLUDE_DIR})
  #add_compile_definitions(${LIBXSLT_DEFINITIONS})
  IF ("${LIBXSLT_LIBRARIES}" MATCHES ".+static.*\\.lib" OR "${LIBXSLT_LIBRARIES}" MATCHES ".+\\.a")
    add_compile_definitions(LIBXSLT_STATIC)
  ENDIF ()
ENDIF ()

################################################################################
#
# configurable options
#
# Note: CMakeSetup displays these in alphabetical order, regardless of
# the order we use here.

#SET(CXPROC_BUFSIZE 4096 CACHE NUMBER "Default size for buffers.")
#OPTION(CXPROC_BUFSIZE "Default size for buffers." "4096")

SET(CXPROC_LEGACY OFF CACHE BOOL "Compile code for handling of legacy issues")
MARK_AS_ADVANCED(CXPROC_LEGACY)
IF (CXPROC_LEGACY)
  add_compile_definitions(LEGACY)
ENDIF (CXPROC_LEGACY)

SET(CXPROC_EXPERIMENTAL OFF CACHE BOOL "Compile experimental code")
MARK_AS_ADVANCED(CXPROC_EXPERIMENTAL)
IF (CXPROC_EXPERIMENTAL)
  add_compile_definitions(EXPERIMENTAL)
ENDIF (CXPROC_EXPERIMENTAL)

add_compile_definitions(CXP_VERSION_URL="https://github.com/raxdne/cxproc")
add_compile_definitions(CXP_VER_FILE_DESCRIPTION_STR="A Configurable Xml PROCessor.")
add_compile_definitions(CXP_VERSION_MAJOR=${GIT_TAG_MAJOR})
add_compile_definitions(CXP_VERSION_MINOR=${GIT_TAG_MINOR})
add_compile_definitions(CXP_VER_FILE_VERSION_STR="v${GIT_TAG_MAJOR}.${GIT_TAG_MINOR}")
add_compile_definitions(CXP_VER_FILE_DEVELOP_STR="${GIT_TAG}")
add_compile_definitions(CXP_VER_FILE_BRANCH_STR="${GIT_BRANCH}")
add_compile_definitions(CXP_VERSION_REVISION=0)
add_compile_definitions(CXP_REVISION_STR="0")
add_compile_definitions(CXP_VERSION_BUILD=0000)
add_compile_definitions(CXP_VERSION_BUILD_STR="0000")
add_compile_definitions(CXP_VER_COPYRIGHT_STR="\(C\) 2006..2025 by Alexander Tenbusch")
add_compile_definitions(CXP_TIMESTAMP_STR="${TODAY}")

IF(MSVC)
  SET(CMAKE_C_FLAGS "/TC /std:c17 /permissive- /fp:fast /MT /Gm- /MP /GS /GR- /Gd /analyze- /J /errorreport:prompt -D_REENTRANT -DDEF_MEM_LEVEL=8 -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -DWIN32_LEAN_AND_MEAN")
  set(CMAKE_C_FLAGS_DEBUG "-DDEBUG /Od /Z7 /W4")
  SET(CMAKE_C_FLAGS_RELEASE "-DNDEBUG -Ot -Oy- /GL")
  set(CMAKE_C_FLAGS_PROFILE "")
  #set(CMAKE_EXE_LINKER_FLAGS_DEBUG "")
ELSE(MSVC)
  set(CMAKE_C_FLAGS "-std=c18 -D_REENTRANT -DDEF_MEM_LEVEL=8 -D_DEFAULT_SOURCE -Werror=implicit-function-declaration -funsigned-char")
  set(CMAKE_C_FLAGS_DEBUG "-DDEBUG -g -O0 -Wall -Wpedantic -Wuninitialized -Wdiscarded-qualifiers -Wno-unused -Wno-format")
  set(CMAKE_C_FLAGS_RELEASE "-DNDEBUG -O3")
  set(CMAKE_C_FLAGS_PROFILE "-pg -DNDEBUG -O3")
  #set(CMAKE_SHARED_LINKER_FLAGS "")
ENDIF (MSVC)

################################################################################
#
# pcre2
#

IF (CXPROC_PCRE2)
  target_link_libraries(filex ${PCRE2_LIBRARY})
  target_link_libraries(dir2csv ${PCRE2_LIBRARY})
  target_link_libraries(dir2sqlite ${PCRE2_LIBRARY})
  target_link_libraries(pietextx ${PCRE2_LIBRARY})
  target_link_libraries(cxproc ${PCRE2_LIBRARY})
  target_link_libraries(cxproc-cgi ${PCRE2_LIBRARY})
  IF(CXPROC_TESTS)
    target_link_libraries(cxproc-test ${PCRE2_LIBRARY})
  ENDIF ()
  #IF (LIBMICROHTTPD_FOUND)
  #  target_link_libraries(cxproc-httpd ${PCRE2_LIBRARY})
  #ENDIF ()
ENDIF ()

################################################################################
#
# either local ZLIB or system ZLIB
#

IF (CXPROC_LIBZ)
  target_link_libraries(filex ${ZLIB_LIBRARY_RELEASE})
  target_link_libraries(dir2csv ${ZLIB_LIBRARY_RELEASE})
  target_link_libraries(dir2sqlite ${ZLIB_LIBRARY_RELEASE})
  target_link_libraries(pietextx ${ZLIB_LIBRARY_RELEASE})
  target_link_libraries(cxproc ${ZLIB_LIBRARY_RELEASE})
  target_link_libraries(cxproc-cgi  ${ZLIB_LIBRARY_RELEASE})
  IF(CXPROC_TESTS)
    target_link_libraries(cxproc-test ${ZLIB_LIBRARY_RELEASE})
  ENDIF ()
  #IF (LIBMICROHTTPD_FOUND)
  #  target_link_libraries(cxproc-httpd ${ZLIB_LIBRARY_RELEASE})
  #ENDIF ()
ENDIF (CXPROC_LIBZ)

################################################################################
#
# libbz2
#

FIND_PACKAGE( BZip2 )

IF (LIBBZ2_LIBRARY_RELEASE)
  OPTION (CXPROC_LIBBZ2 "Enable support for linking cxproc with libbz2." OFF)
ELSE(LIBBZ2_LIBRARY_RELEASE)
  SET(CXPROC_LIBBZ2 OFF)
ENDIF (LIBBZ2_LIBRARY_RELEASE)

IF (CXPROC_LIBBZ2)
  INCLUDE_DIRECTORIES(${LIBBZ2_INCLUDE_DIR})
  add_compile_definitions(HAVE_LIBBZ2)
  IF (${LIBBZ2_LIBRARY_RELEASE} MATCHES ".+static.*\\.lib$" OR ${LIBBZ2_LIBRARY_RELEASE} MATCHES ".+\\.a$")
    add_compile_definitions(LIBBZ2_STATIC)
  ENDIF ()
  target_link_libraries(cxproc ${LIBBZ2_LIBRARY_RELEASE})
  target_link_libraries(cxproc-cgi  ${LIBBZ2_LIBRARY_RELEASE})
  IF(CXPROC_TESTS)
    target_link_libraries(cxproc-test ${LIBBZ2_LIBRARY_RELEASE})
  ENDIF ()
  #IF (LIBMICROHTTPD_FOUND)
  #  target_link_libraries(cxproc-httpd ${LIBBZ2_LIBRARY_RELEASE})
  #ENDIF ()
ENDIF (CXPROC_LIBBZ2)

################################################################################
#
# liblzma
#

IF (CXPROC_LZMA)
  INCLUDE_DIRECTORIES(${LIBLZMA_INCLUDE_DIR})
  add_compile_definitions(HAVE_LIBLZMA)
  #IF (${LIBLZMA_LIBRARIES} MATCHES ".+static\\.lib$" OR ${LIBLZMA_LIBRARIES} MATCHES ".+\\.a$")
  #  add_compile_definitions(LIBLZMA_STATIC)
  #ENDIF ()
  target_link_libraries(filex ${LIBLZMA_LIBRARIES})
  target_link_libraries(dir2csv ${LIBLZMA_LIBRARIES})
  target_link_libraries(dir2sqlite ${LIBLZMA_LIBRARIES})
  target_link_libraries(pietextx ${LIBLZMA_LIBRARIES})
  target_link_libraries(cxproc ${LIBLZMA_LIBRARIES})
  target_link_libraries(cxproc-cgi  ${LIBLZMA_LIBRARIES})
  IF(CXPROC_TESTS)
    target_link_libraries(cxproc-test ${LIBLZMA_LIBRARIES})
  ENDIF ()
  #IF (LIBMICROHTTPD_FOUND)
  #  target_link_libraries(cxproc-httpd ${LIBLZMA_LIBRARIES})
  #ENDIF ()
ENDIF (CXPROC_LZMA)

################################################################################
#
# either local LIBMICROHTTPD or system LIBMICROHTTPD
#

#find_path(LIBMICROHTTPD_INCLUDE_DIR NAMES microhttpd.h
#  HINTS "${CXPROC_PREFIX}/include"
#  ) 

#IF (LIBMICROHTTPD_INCLUDE_DIR)
#  find_library(LIBMICROHTTPD_LIBRARIES NAMES libmicrohttpd.a microhttpd libmicrohttpd-dll_d
#    HINTS "${CXPROC_PREFIX}/lib"
#    )
#  IF (LIBMICROHTTPD_LIBRARIES)
#    SET(LIBMICROHTTPD_FOUND ON)
#  ELSE(LIBMICROHTTPD_LIBRARIES)
#    FIND_PACKAGE( LIBMICROHTTPD )
#  ENDIF (LIBMICROHTTPD_LIBRARIES)
#ELSE(LIBMICROHTTPD_INCLUDE_DIR)
#  FIND_PACKAGE( LIBMICROHTTPD )
#ENDIF (LIBMICROHTTPD_INCLUDE_DIR)

#IF (LIBMICROHTTPD_FOUND)
#  MESSAGE(STATUS "Found optional libmicrohttpd: ${LIBMICROHTTPD_LIBRARIES}")
#  INCLUDE_DIRECTORIES(${LIBMICROHTTPD_INCLUDE_DIR})
#  add_compile_definitions(HAVE_LIBMICROHTTPD)
#  IF (${LIBMICROHTTPD_LIBRARIES} MATCHES ".+static\\.lib$" OR ${LIBMICROHTTPD_LIBRARIES} MATCHES ".+\\.a$")
#    add_compile_definitions(LIBMICROHTTPD_STATIC)
#  ENDIF ()
#ELSE(LIBMICROHTTPD_FOUND)

#ENDIF (LIBMICROHTTPD_FOUND)

################################################################################
#
# pthread
#

IF(CXPROC_PTHREAD)
  target_link_libraries(filex ${CMAKE_THREAD_LIBS_INIT})
  target_link_libraries(cxproc ${CMAKE_THREAD_LIBS_INIT})
  target_link_libraries(cxproc-cgi ${CMAKE_THREAD_LIBS_INIT})
  IF(CXPROC_TESTS)
    target_link_libraries(cxproc-test ${CMAKE_THREAD_LIBS_INIT})
  ENDIF ()
  #IF (LIBMICROHTTPD_FOUND)
  #  target_link_libraries(cxproc-httpd ${CMAKE_THREAD_LIBS_INIT})
  #ENDIF ()
ENDIF (CXPROC_PTHREAD)

################################################################################
#
# CURL
#

IF(CXPROC_CURL)
  INCLUDE_DIRECTORIES(${CURL_INCLUDE_DIR})

  target_compile_definitions(filex PUBLIC HAVE_LIBCURL)
  target_compile_definitions(cxproc PUBLIC HAVE_LIBCURL)
  target_compile_definitions(cxproc-cgi PUBLIC HAVE_LIBCURL)
  IF(CXPROC_TESTS)
    target_compile_definitions(cxproc-test PUBLIC HAVE_LIBCURL)
  ENDIF ()
  IF (${CURL_LIBRARY_RELEASE} MATCHES ".+static\\.lib$" OR ${CURL_LIBRARY_RELEASE} MATCHES ".+\\.a$")
    add_compile_definitions(LIBCURL_STATIC)
  ENDIF ()

  target_link_libraries(filex ${CURL_LIBRARY_RELEASE})
  target_link_libraries(cxproc ${CURL_LIBRARY_RELEASE})
  target_link_libraries(cxproc-cgi  ${CURL_LIBRARY_RELEASE})
  IF(CXPROC_TESTS)
    target_link_libraries(cxproc-test ${CURL_LIBRARY_RELEASE})
  ENDIF ()
  #IF (LIBMICROHTTPD_FOUND)
  #  target_link_libraries(cxproc-httpd ${CURL_LIBRARY_RELEASE})
  #ENDIF ()

  IF(MSVC)
    # additional libraries with VC++
    target_link_libraries(filex crypt32 wldap32)
    target_link_libraries(cxproc crypt32 wldap32)
    target_link_libraries(cxproc-cgi  crypt32 wldap32)
    IF(CXPROC_TESTS)
      target_link_libraries(cxproc-test crypt32 wldap32)
    ENDIF ()
  ELSE(MSVC)
    target_link_libraries(filex ssl crypto)
    target_link_libraries(cxproc ssl crypto)
    target_link_libraries(cxproc-cgi  ssl crypto)
    IF(CXPROC_TESTS)
      target_link_libraries(cxproc-test ssl crypto)
    ENDIF ()
    #IF (LIBMICROHTTPD_FOUND)
    #  target_link_libraries(cxproc-httpd ssl crypto)
    #ENDIF ()
  ENDIF(MSVC)
ENDIF ()

################################################################################
#
# add list of mandatory libraries
#

target_link_libraries(filex ${LIBZIP_LIBRARY} ${LIBICONV_LIBRARY} ${LIBCHARSET_LIBRARY} ${LIBXML2_LIBRARY})
target_link_libraries(dir2csv ${LIBZIP_LIBRARY} ${LIBICONV_LIBRARY} ${LIBCHARSET_LIBRARY} ${LIBXML2_LIBRARY})
target_link_libraries(dir2sqlite ${LIBZIP_LIBRARY} ${LIBICONV_LIBRARY} ${LIBCHARSET_LIBRARY} ${LIBXML2_LIBRARY})
target_link_libraries(pietextx ${LIBZIP_LIBRARY} ${LIBICONV_LIBRARY} ${LIBCHARSET_LIBRARY} ${LIBXML2_LIBRARY})
target_link_libraries(cxproc ${LIBZIP_LIBRARY} ${LIBICONV_LIBRARY} ${LIBCHARSET_LIBRARY} ${LIBXML2_LIBRARY} ${LIBXSLT_LIBRARIES} ${LIBXSLT_EXSLT_LIBRARIES})
target_link_libraries(cxproc-cgi ${LIBZIP_LIBRARY} ${LIBICONV_LIBRARY} ${LIBCHARSET_LIBRARY} ${LIBXML2_LIBRARY} ${LIBXSLT_LIBRARIES} ${LIBXSLT_EXSLT_LIBRARIES})
IF(CXPROC_TESTS)
  target_link_libraries(cxproc-test ${LIBZIP_LIBRARY} ${LIBICONV_LIBRARY} ${LIBCHARSET_LIBRARY} ${LIBXML2_LIBRARY} ${LIBXSLT_LIBRARIES} ${LIBXSLT_EXSLT_LIBRARIES})
ENDIF ()

#IF (LIBMICROHTTPD_FOUND)
#  target_link_libraries(cxproc-httpd ${LIBZIP_LIBRARY} ${LIBICONV_LIBRARY} ${LIBCHARSET_LIBRARY} ${LIBXML2_LIBRARY} ${LIBXSLT_LIBRARIES} ${LIBMICROHTTPD_LIBRARIES})
#ENDIF ()

################################################################################
#
# add the command to generate the source code

################################################################################
#

################################################################################
#

IF(MSVC)
ELSE(MSVC)
  target_link_libraries(filex -ldl -lm)
  target_link_libraries(dir2csv -ldl -lm)
  target_link_libraries(dir2sqlite -ldl -lm)
  target_link_libraries(pietextx -ldl -lm)
  target_link_libraries(cxproc -ldl -lm)
  target_link_libraries(cxproc-cgi -ldl -lm)
  IF(CXPROC_TESTS)
    target_link_libraries(cxproc-test -ldl -lm)
  ENDIF ()
  #IF (LIBMICROHTTPD_FOUND)
  #  target_link_libraries(cxproc-httpd -ldl -lm)
  #ENDIF ()
  IF (CXPROC_LIBZ)
    target_link_libraries(filex -lz)
    target_link_libraries(dir2csv -lz)
    target_link_libraries(dir2sqlite -lz)
    target_link_libraries(cxproc -lz)
    target_link_libraries(cxproc-cgi -lz)
    IF(CXPROC_TESTS)
      target_link_libraries(cxproc-test -lz)
    ENDIF ()
    #IF (LIBMICROHTTPD_FOUND)
    #  target_link_libraries(cxproc-httpd -lz)
    #ENDIF ()
  ENDIF (CXPROC_LIBZ)
ENDIF(MSVC)

set_target_properties(filex
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CXPROC_PREFIX}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CXPROC_PREFIX}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CXPROC_PREFIX}/bin"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CXPROC_PREFIX}/lib"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CXPROC_PREFIX}/lib"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CXPROC_PREFIX}/bin"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CXPROC_PREFIX}/lib"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CXPROC_PREFIX}/lib"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CXPROC_PREFIX}/bin"
)

set_target_properties(dir2csv
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CXPROC_PREFIX}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CXPROC_PREFIX}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CXPROC_PREFIX}/bin"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CXPROC_PREFIX}/lib"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CXPROC_PREFIX}/lib"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CXPROC_PREFIX}/bin"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CXPROC_PREFIX}/lib"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CXPROC_PREFIX}/lib"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CXPROC_PREFIX}/bin"
)

set_target_properties(dir2sqlite
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CXPROC_PREFIX}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CXPROC_PREFIX}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CXPROC_PREFIX}/bin"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CXPROC_PREFIX}/lib"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CXPROC_PREFIX}/lib"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CXPROC_PREFIX}/bin"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CXPROC_PREFIX}/lib"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CXPROC_PREFIX}/lib"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CXPROC_PREFIX}/bin"
)

set_target_properties(pietextx
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CXPROC_PREFIX}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CXPROC_PREFIX}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CXPROC_PREFIX}/bin"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CXPROC_PREFIX}/lib"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CXPROC_PREFIX}/lib"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CXPROC_PREFIX}/bin"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CXPROC_PREFIX}/lib"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CXPROC_PREFIX}/lib"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CXPROC_PREFIX}/bin"
)

set_target_properties(cxproc
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CXPROC_PREFIX}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CXPROC_PREFIX}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CXPROC_PREFIX}/bin"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CXPROC_PREFIX}/lib"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CXPROC_PREFIX}/lib"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CXPROC_PREFIX}/bin"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CXPROC_PREFIX}/lib"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CXPROC_PREFIX}/lib"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CXPROC_PREFIX}/bin"
)

set_target_properties(cxproc-cgi
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CXPROC_PREFIX}/www/"
  LIBRARY_OUTPUT_DIRECTORY "${CXPROC_PREFIX}/www/"
  RUNTIME_OUTPUT_DIRECTORY "${CXPROC_PREFIX}/www/cgi-bin"
  ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CXPROC_PREFIX}/www/"
  LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CXPROC_PREFIX}/www/"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CXPROC_PREFIX}/www/cgi-bin"
  ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CXPROC_PREFIX}/www/"
  LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CXPROC_PREFIX}/www/"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CXPROC_PREFIX}/www/cgi-bin"
)

IF(CXPROC_TESTS)
  set_target_properties(cxproc-test
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CXPROC_PREFIX}/"
    LIBRARY_OUTPUT_DIRECTORY "${CXPROC_PREFIX}/"
    RUNTIME_OUTPUT_DIRECTORY "${CXPROC_PREFIX}/bin"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CXPROC_PREFIX}/"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CXPROC_PREFIX}/"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CXPROC_PREFIX}/bin"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CXPROC_PREFIX}/"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CXPROC_PREFIX}/"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CXPROC_PREFIX}/bin"
  )
ENDIF ()

#IF (LIBMICROHTTPD_FOUND)
#  set_target_properties(cxproc-httpd
#    PROPERTIES
#    ARCHIVE_OUTPUT_DIRECTORY "${CXPROC_PREFIX}/www/"
#    LIBRARY_OUTPUT_DIRECTORY "${CXPROC_PREFIX}/www/"
#    RUNTIME_OUTPUT_DIRECTORY "${CXPROC_PREFIX}/www/http-bin"
#    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CXPROC_PREFIX}/www/"
#    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CXPROC_PREFIX}/www/"
#    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CXPROC_PREFIX}/www/http-bin"
#    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CXPROC_PREFIX}/www/"
#    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CXPROC_PREFIX}/www/"
#    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CXPROC_PREFIX}/www/http-bin"
#    )
#ENDIF ()

###############################################################################

include(package)

include(CPack)

#OPTION(CXPROC_SHOW_REPORT     "Show the final configuration report" ON)
include(report)

