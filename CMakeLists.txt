#
#
#
cmake_minimum_required(VERSION 3.20) # setting this is required

project(cxproc)            # this sets the project name

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Profile" CACHE STRING "" FORCE) 
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Default build type: Release")

# for profiling s. https://stackoverflow.com/questions/26491948/how-to-use-gprof-with-cmake
# or see test/main.sh and https://github.com/jrfonseca/gprof2dot
# http://gernotklingler.com/blog/gprof-valgrind-gperftools-evaluation-tools-application-level-cpu-profiling-linux/
#set(CMAKE_BUILD_TYPE RelWithDebInfow)

# s. https://cmake.org/Wiki/CMake_Useful_Variables

# /sbin/ldconfig -p

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

include(helper)

include_directories(BEFORE SYSTEM
  "${PROJECT_SOURCE_DIR}/third-party"
  "${PROJECT_SOURCE_DIR}/third-party/option"
  ${CXPROC_SRC_DIR}
  "${CXPROC_SRC_DIR}/option"
  ${HINT_DIR_INCLUDE}
)

#link_directories(BEFORE
#  ${HINT_DIR_LIB}
#  ${HINT_DIR_LIBD}
#)
  
#SET(CMAKE_INCLUDE_PATH "${CMAKE_SYSTEM_INCLUDE_PATH};/usr/include")
#MESSAGE(STATUS "Using CMAKE_INCLUDE_PATH: ${CMAKE_INCLUDE_PATH}")
#SET(CMAKE_LIBRARY_PATH "/usr/lib;/usr/lib/x86_64-linux-gnu")
#MESSAGE(STATUS "Using CMAKE_LIBRARY_PATH: ${CMAKE_LIBRARY_PATH}")

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

# FIND_PACKAGE( LocalThreads )

FIND_PACKAGE( PCRE2 )

FIND_PACKAGE( Duktape )

FIND_PACKAGE( LibBase64 )

FIND_PACKAGE( LibCMark )

FIND_PACKAGE( LocalSQLite3 )

FIND_PACKAGE( LocalCURL )

FIND_PACKAGE( LocalZLIB )

FIND_PACKAGE( LocalLibLZMA )

FIND_PACKAGE( LocalBZip2 )

FIND_PACKAGE( LibZip )

FIND_PACKAGE( LocalIconv )

FIND_PACKAGE( LocalLibXml2 )

FIND_PACKAGE( LocalLibXslt )

################################################################################
#
# pthread
#

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

###############################################################################

IF (PCRE2_INCLUDE_DIR AND PCRE2_LIBRARY)
  target_compile_definitions(cxproc PUBLIC HAVE_PCRE2 PCRE2_CODE_UNIT_WIDTH=8)
  target_compile_definitions(cxproc-cgi PUBLIC HAVE_PCRE2 PCRE2_CODE_UNIT_WIDTH=8)
  IF(CXPROC_TESTS)
    target_compile_definitions(cxproc-test PUBLIC HAVE_PCRE2 PCRE2_CODE_UNIT_WIDTH=8)
  ENDIF ()
  IF(CXPROC_CHECK)
    target_compile_definitions(cpp-check PUBLIC HAVE_PCRE2 PCRE2_CODE_UNIT_WIDTH=8)
  ENDIF ()
ENDIF ()

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
# libxml2
#

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
  SET(CMAKE_C_FLAGS "/TC /std:c17 /permissive- /fp:fast /MT /Gm- /MP /GS /GR- /Gd /analyze- /J /errorreport:prompt -D_REENTRANT -DDEF_MEM_LEVEL=8 -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -DWIN32_LEAN_AND_MEAN -DBASE64_STATIC_DEFINE")
  set(CMAKE_C_FLAGS_DEBUG "-DDEBUG /Od /Z7 /W4")
  SET(CMAKE_C_FLAGS_RELEASE "-DNDEBUG -Ot -Oy- /GL")
  set(CMAKE_C_FLAGS_PROFILE "")
  #set(CMAKE_EXE_LINKER_FLAGS_DEBUG "")
ELSE(MSVC)
  set(CMAKE_C_FLAGS "-std=c18 -D_REENTRANT -DDEF_MEM_LEVEL=8 -D_DEFAULT_SOURCE -DBASE64_STATIC_DEFINE -Werror=implicit-function-declaration -funsigned-char")
  set(CMAKE_C_FLAGS_DEBUG "-DDEBUG -g -O0 -Wall -Wpedantic -Wuninitialized -Wdiscarded-qualifiers -Wno-unused -Wno-format")
  set(CMAKE_C_FLAGS_RELEASE "-DNDEBUG -O3")
  set(CMAKE_C_FLAGS_PROFILE "-pg -DNDEBUG -O3")
  #set(CMAKE_SHARED_LINKER_FLAGS "")
ENDIF (MSVC)

################################################################################
#
# pcre2
#

IF (PCRE2_LIBRARY)
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

IF (ZLIB_LIBRARY_RELEASE)
  target_link_libraries(filex ${ZLIB_LIBRARY_RELEASE})
  target_link_libraries(dir2csv ${ZLIB_LIBRARY_RELEASE})
  target_link_libraries(dir2sqlite ${ZLIB_LIBRARY_RELEASE})
  #target_link_libraries(pietextx ${ZLIB_LIBRARY_RELEASE})
  target_link_libraries(cxproc ${ZLIB_LIBRARY_RELEASE})
  target_link_libraries(cxproc-cgi  ${ZLIB_LIBRARY_RELEASE})
  IF(CXPROC_TESTS)
    target_link_libraries(cxproc-test ${ZLIB_LIBRARY_RELEASE})
  ENDIF ()
  #IF (LIBMICROHTTPD_FOUND)
  #  target_link_libraries(cxproc-httpd ${ZLIB_LIBRARY_RELEASE})
  #ENDIF ()
ENDIF ()

################################################################################
#
# either local LIBZIP or system LIBZIP
#

IF (LIBZIP_LIBRARY)
  target_link_libraries(filex ${LIBZIP_LIBRARY})
  target_link_libraries(dir2csv ${LIBZIP_LIBRARY})
  target_link_libraries(dir2sqlite ${LIBZIP_LIBRARY})
  #target_link_libraries(pietextx ${LIBZIP_LIBRARY})
  target_link_libraries(cxproc ${LIBZIP_LIBRARY})
  target_link_libraries(cxproc-cgi  ${LIBZIP_LIBRARY})
  IF(CXPROC_TESTS)
    target_link_libraries(cxproc-test ${LIBZIP_LIBRARY})
  ENDIF ()
  #IF (LIBMICROHTTPD_FOUND)
  #  target_link_libraries(cxproc-httpd ${LIBZIP_LIBRARY})
  #ENDIF ()
ENDIF ()

################################################################################
#
# libbz2
#

IF (BZIP2_LIBRARIES)
  target_link_libraries(filex ${BZIP2_LIBRARIES})
  target_link_libraries(dir2csv ${BZIP2_LIBRARIES})
  target_link_libraries(dir2sqlite ${BZIP2_LIBRARIES})
  target_link_libraries(pietextx ${BZIP2_LIBRARIES})
  target_link_libraries(cxproc ${BZIP2_LIBRARIES})
  target_link_libraries(cxproc-cgi  ${BZIP2_LIBRARIES})
  IF(CXPROC_TESTS)
    target_link_libraries(cxproc-test ${BZIP2_LIBRARIES})
  ENDIF ()
  #IF (LIBMICROHTTPD_FOUND)
  #  target_link_libraries(cxproc-httpd ${BZIP2_LIBRARIES})
  #ENDIF ()
ENDIF ()

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

IF(CURL_FOUND)
  OPTION (CXPROC_CURL "Enable support for compiling cxproc with curl." OFF)
ENDIF ()

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

ENDIF ()

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


################################################################################
#
# add list of mandatory libraries
#

target_link_libraries(filex ${LIBICONV_LIBRARY} ${LIBCHARSET_LIBRARY} ${LIBXML2_LIBRARY} ${LIBBASE64_LIBRARY})
target_link_libraries(dir2csv ${LIBICONV_LIBRARY} ${LIBCHARSET_LIBRARY} ${LIBXML2_LIBRARY} ${LIBBASE64_LIBRARY})
target_link_libraries(dir2sqlite ${LIBICONV_LIBRARY} ${LIBCHARSET_LIBRARY} ${LIBXML2_LIBRARY} ${LIBBASE64_LIBRARY})
target_link_libraries(pietextx ${LIBICONV_LIBRARY} ${LIBCHARSET_LIBRARY} ${LIBXML2_LIBRARY} ${LIBBASE64_LIBRARY})
target_link_libraries(cxproc ${LIBICONV_LIBRARY} ${LIBCHARSET_LIBRARY} ${LIBXML2_LIBRARY} ${LIBXSLT_LIBRARIES} ${LIBXSLT_EXSLT_LIBRARIES} ${LIBBASE64_LIBRARY})
target_link_libraries(cxproc-cgi ${LIBICONV_LIBRARY} ${LIBCHARSET_LIBRARY} ${LIBXML2_LIBRARY} ${LIBXSLT_LIBRARIES} ${LIBXSLT_EXSLT_LIBRARIES} ${LIBBASE64_LIBRARY})
IF(CXPROC_TESTS)
  target_link_libraries(cxproc-test ${LIBICONV_LIBRARY} ${LIBCHARSET_LIBRARY} ${LIBXML2_LIBRARY} ${LIBXSLT_LIBRARIES} ${LIBXSLT_EXSLT_LIBRARIES} ${LIBBASE64_LIBRARY})
ENDIF ()

#IF (LIBMICROHTTPD_FOUND)
#  target_link_libraries(cxproc-httpd ${LIBICONV_LIBRARY} ${LIBCHARSET_LIBRARY} ${LIBXML2_LIBRARY} ${LIBXSLT_LIBRARIES} ${LIBMICROHTTPD_LIBRARIES})
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
  target_link_libraries(filex dl m)
  target_link_libraries(dir2csv dl m)
  target_link_libraries(dir2sqlite dl m)
  target_link_libraries(pietextx dl m)
  target_link_libraries(cxproc dl m)
  target_link_libraries(cxproc-cgi dl m)
  IF(CXPROC_TESTS)
    target_link_libraries(cxproc-test dl m)
  ENDIF ()
  #IF (LIBMICROHTTPD_FOUND)
  #  target_link_libraries(cxproc-httpd dl m)
  #ENDIF ()
  IF (LIBZIP_LIBRARY)
    target_link_libraries(filex z)
    target_link_libraries(dir2csv z)
    target_link_libraries(dir2sqlite z)
    target_link_libraries(cxproc z)
    target_link_libraries(cxproc-cgi z)
    IF(CXPROC_TESTS)
      target_link_libraries(cxproc-test z)
    ENDIF ()
    #IF (LIBMICROHTTPD_FOUND)
    #  target_link_libraries(cxproc-httpd z)
    #ENDIF ()
  ENDIF ()
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

