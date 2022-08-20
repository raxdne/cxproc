#
#
#
cmake_minimum_required(VERSION 3.10) # setting this is required

project(cxproc)            # this sets the project name

# https://stackoverflow.com/questions/24460486/cmake-build-type-is-not-being-used-in-cmakelists-txt
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Profile" CACHE STRING "" FORCE) 
# cmake --help-variable-list | grep Release

# for profiling s. https://stackoverflow.com/questions/26491948/how-to-use-gprof-with-cmake
# or see test/main.sh and https://github.com/jrfonseca/gprof2dot
# http://gernotklingler.com/blog/gprof-valgrind-gperftools-evaluation-tools-application-level-cpu-profiling-linux/
#set(CMAKE_BUILD_TYPE "Profile")

# s. https://cmake.org/Wiki/CMake_Useful_Variables

# CMAKE_POLICY(SET CMP0026 OLD)

SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" "${PROJECT_SOURCE_DIR}/cmake/Modules" "${PROJECT_SOURCE_DIR}/cmake/Options")

SET(CMAKE_VERBOSE_MAKEFILE ON)

# http://www.vtk.org/Wiki/CMake:Eclipse_UNIX_Tutorial
# http://stackoverflow.com/questions/9453851/how-to-configure-eclipse-cdt-for-cmake
#FIND_PACKAGE(EclipseCDT4)

# (shell-command "(cmake --graphviz=cxproc.dot --build ../x86_64-gnu-linux/ && dot -T png -o cxproc.png cxproc.dot.cxproc)")

FIND_PATH(CXPROC_SRC_DIR
  NAME cxproc.c
  PATHS "src")

FIND_PATH(CXPROC_SRCLIB_DIR
  NAME cgi-util
  PATHS "third-party")

SET(CXPROC_PREFIX "${PROJECT_BINARY_DIR}/.." CACHE PATH "Prefix")
SET(CXPROC_TEMP_DIR "${CXPROC_PREFIX}/tmp")
# working directory for tests
SET(CXPROC_TEST_DIR "${PROJECT_SOURCE_DIR}/test")

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/third-party"
  "${PROJECT_SOURCE_DIR}/third-party/option"
  "${CXPROC_SRC_DIR}"
  "${CXPROC_SRC_DIR}/option"
  "${CXPROC_PREFIX}/include"
  )

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/debug)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/release)

# https://www.mattkeeter.com/blog/2018-01-06-versioning/

execute_process(COMMAND git describe --tags --long
  OUTPUT_VARIABLE GIT_TAG
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  )
string(STRIP "${GIT_TAG}" GIT_TAG)

execute_process(COMMAND git branch --show-current
  OUTPUT_VARIABLE GIT_BRANCH
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  )
string(STRIP "${GIT_BRANCH}" GIT_BRANCH)

set(PROJECT_VERSION_MAJOR "1")
set(PROJECT_VERSION_MINOR "3")
set(PROJECT_VERSION_PATCH "4-beta")

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
  ${CXPROC_SRC_DIR}/option/pie/calendar_element.c
  ${CXPROC_SRC_DIR}/option/pie/calendar_element.h
  ${CXPROC_SRC_DIR}/plain_text.c
  ${CXPROC_SRC_DIR}/plain_text.h
  ${CXPROC_SRC_DIR}/res_node/res_mime.c
  ${CXPROC_SRC_DIR}/res_node/res_mime.h
  ${CXPROC_SRC_DIR}/res_node/res_node.c
  ${CXPROC_SRC_DIR}/res_node/res_node.h
  ${CXPROC_SRC_DIR}/res_node/res_node_io.c
  ${CXPROC_SRC_DIR}/res_node/res_node_io.h
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
  ${CXPROC_SRC_DIR}/res_node/res_node_io.c
  ${CXPROC_SRC_DIR}/res_node/res_node_io.h
  ${CXPROC_SRC_DIR}/res_node/res_node_list.c
  ${CXPROC_SRC_DIR}/res_node/res_node_list.h
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
  ${CXPROC_SRC_DIR}/res_node/res_node_io.c
  ${CXPROC_SRC_DIR}/res_node/res_node_io.h
  ${CXPROC_SRC_DIR}/res_node/res_node_list.c
  ${CXPROC_SRC_DIR}/res_node/res_node_list.h
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
  ${CXPROC_SRC_DIR}/res_node/res_path.c
  ${CXPROC_SRC_DIR}/res_node/res_path.h
  ${CXPROC_SRC_DIR}/version.h
  ${CXPROC_SRC_DIR}/pietextx.c
  )

target_compile_definitions(pietextx PUBLIC PIE_STANDALONE)

SET(CXPROC_HEADERS
  ${CXPROC_SRC_DIR}/version.h
#  ${CXPROC_SRC_DIR}/git_revision.h
#  ${CXPROC_SRC_DIR}/git_info.h
  ${CXPROC_SRC_DIR}/basics.h
  ${CXPROC_SRC_DIR}/cxp/cxp_dtd.h
  ${CXPROC_SRC_DIR}/cxp/cxp_context_enc.h
  ${CXPROC_SRC_DIR}/cxp/cxp_context.h
  ${CXPROC_SRC_DIR}/cxp/cxp_context_cli.h
  ${CXPROC_SRC_DIR}/cxp/cxp_context_cache.h
  ${CXPROC_SRC_DIR}/cxp/cxp_subst.h
  ${CXPROC_SRC_DIR}/cxp/cxp_dir.h
  ${CXPROC_SRC_DIR}/dom.h
  ${CXPROC_SRC_DIR}/res_node/res_node.h
  ${CXPROC_SRC_DIR}/res_node/res_node_list.h
  ${CXPROC_SRC_DIR}/res_node/res_node_io.h
  ${CXPROC_SRC_DIR}/res_node/res_node_ops.h
  ${CXPROC_SRC_DIR}/res_node/res_path.h
  ${CXPROC_SRC_DIR}/res_node/res_mime.h
  ${CXPROC_SRC_DIR}/plain_text.h
  ${CXPROC_SRC_DIR}/option/pie/calendar_element.h
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
  ${CXPROC_SRC_DIR}/dom.c
  ${CXPROC_SRC_DIR}/res_node/res_node.c
  ${CXPROC_SRC_DIR}/res_node/res_node_list.c
  ${CXPROC_SRC_DIR}/res_node/res_node_properties.c
  ${CXPROC_SRC_DIR}/res_node/res_node_io.c
  ${CXPROC_SRC_DIR}/res_node/res_node_ops.c
  ${CXPROC_SRC_DIR}/res_node/res_path.c
  ${CXPROC_SRC_DIR}/res_node/res_mime.c
  ${CXPROC_SRC_DIR}/plain_text.c
  ${CXPROC_SRC_DIR}/option/pie/calendar_element.c
  ${CXPROC_SRC_DIR}/utils.c
)

ADD_EXECUTABLE(cxproc
 ${CXPROC_SRC_DIR}/cxproc.c
 ${CXPROC_SOURCES} ${CXPROC_HEADERS})

ADD_EXECUTABLE(cxproc-test
 ${CXPROC_SRC_DIR}/cxproc-test.c
 ${CXPROC_SOURCES} ${CXPROC_HEADERS})

target_compile_definitions(cxproc-test PUBLIC
  TESTCODE
  BUILDPREFIX="${CXPROC_PREFIX}/"
  TESTPREFIX="${CXPROC_TEST_DIR}/"
  TEMPPREFIX="${CXPROC_TEMP_DIR}/"
  HTTPPREFIX="${CXPROC_HTTPPREFIX}")

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

ADD_EXECUTABLE(test-cgi
 ${CXPROC_SRC_DIR}/test/test-cgi.c)

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
  target_sources(dir2csv PUBLIC ${CXPROC_SRC_DIR}/version.rc)
  target_sources(dir2sqlite PUBLIC ${CXPROC_SRC_DIR}/version.rc)
  target_sources(pietextx   PUBLIC ${CXPROC_SRC_DIR}/version.rc)
  target_sources(cxproc     PUBLIC ${CXPROC_SRC_DIR}/version.rc)
  target_sources(cxproc-cgi PUBLIC ${CXPROC_SRC_DIR}/version.rc)
ENDIF(MSVC)

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

#   add_definitions(-DHAVE_LIBPTHREAD)
# ELSEIF (CXPROC_THREAD)
#   target_sources(cxproc PUBLIC ${THREAD_FILES})
#   target_sources(cxproc-cgi PUBLIC ${THREAD_FILES})
#   target_sources(cxproc-test PUBLIC ${THREAD_FILES})

#   add_definitions(-DWITH_THREAD)
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
  add_definitions(-DHAVE_PCRE2 -DPCRE2_CODE_UNIT_WIDTH=8)
  IF (${PCRE2_LIBRARY} MATCHES ".+static.*\\.lib$" OR ${PCRE2_LIBRARY} MATCHES ".+\\.a$")
    #ADD_LIBRARY(pcre2-8 STATIC IMPORTED)
    add_definitions(-DPCRE2_STATIC)
  ENDIF ()
ELSE ()
ENDIF ()

################################################################################
#
# libcurl
#

FIND_PACKAGE( CURL )

IF (CURL_LIBRARY_RELEASE)
  OPTION(CXPROC_CURL "Enable support for linking cxproc with libcurl." OFF)
ELSE ()
  SET(CXPROC_CURL OFF)
ENDIF ()

IF (CXPROC_CURL)
  SET(CXPROC_HTTPPREFIX CACHE STRING "http://localhost:8183/")
ENDIF ()

################################################################################
#
# zlib
#

FIND_PACKAGE( ZLIB )

IF (ZLIB_LIBRARY_RELEASE)
  SET(CXPROC_LIBZ ON)
ENDIF (ZLIB_LIBRARY_RELEASE)

IF (CXPROC_LIBZ)
  SET(ZIP_FILES
    ${CXPROC_SRCLIB_DIR}/minizip/ioapi.c
    ${CXPROC_SRCLIB_DIR}/minizip/ioapi.h
    ${CXPROC_SRCLIB_DIR}/minizip/unzip.c
    ${CXPROC_SRCLIB_DIR}/minizip/unzip.h
    ${CXPROC_SRCLIB_DIR}/minizip/zip.c
    ${CXPROC_SRCLIB_DIR}/minizip/zip.h
    ${CXPROC_SRCLIB_DIR}/xmlzipio/xmlzipio.c
    ${CXPROC_SRCLIB_DIR}/xmlzipio/xmlzipio.h
    )

  target_sources(filex PUBLIC ${ZIP_FILES})

  target_sources(cxproc PUBLIC ${ZIP_FILES})

  target_sources(cxproc-cgi PUBLIC ${ZIP_FILES})

  target_sources(cxproc-test PUBLIC ${ZIP_FILES})

  INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR} ${CXPROC_SRCLIB_DIR}/minizip)
  
  add_definitions(-DHAVE_ZLIB)
  IF (${ZLIB_LIBRARY_RELEASE} MATCHES ".+static.*\\.lib$" OR ${ZLIB_LIBRARY_RELEASE} MATCHES ".+\\.a$")
    add_definitions(-DZLIB_STATIC)
  ENDIF ()
ENDIF (CXPROC_LIBZ)

################################################################################
#
# liblzma
#

FIND_PACKAGE( LibLZMA )

IF (LIBLZMA_LIBRARIES)
  SET(CXPROC_LZMA ON)
ENDIF (LIBLZMA_LIBRARIES)

###############################################################################

include(tests)

include(archive)
include(audio)
include(database)
include(image)
include(json)
include(petrinet)
include(pie)
# TODO: include(rp)
include(script)
# TODO: include(vorbiscomment)


################################################################################
#
# libiconv
#

FIND_PATH(LIBICONV_INCLUDE_DIR NAME iconv.h
  HINTS "${CXPROC_PREFIX}/include"
  )

IF (LIBICONV_INCLUDE_DIR)
  IF (MSVC)
    FIND_LIBRARY(LIBICONV_LIBRARY NAMES libiconv iconv
      HINTS "${CXPROC_PREFIX}/lib"
      )
    FIND_LIBRARY(LIBCHARSET_LIBRARY NAMES libcharset charset
      HINTS "${CXPROC_PREFIX}/lib"
      )
    IF (LIBICONV_LIBRARY AND LIBCHARSET_LIBRARY)
      SET(LIBICONV_FOUND ON)
    ELSE ()
      # select interactively
    ENDIF ()
  ELSE ()
    # no library file on Linux
    SET(LIBICONV_FOUND ON)
  ENDIF ()
ENDIF (LIBICONV_INCLUDE_DIR)

IF (LIBICONV_FOUND)
  MESSAGE(STATUS "Found required libiconv: ${LIBICONV_LIBRARY} ${LIBCHARSET_LIBRARY}")
ELSE (LIBICONV_FOUND)
  MESSAGE(FATAL_ERROR "libiconv must be available")
ENDIF (LIBICONV_FOUND)

add_definitions(-DICONV_CODE_UNIT_WIDTH=8)

################################################################################
#
# libxml2
#

FIND_PACKAGE( LibXml2 )

IF (LIBXML2_LIBRARY)
  INCLUDE_DIRECTORIES(${LIBXML2_INCLUDE_DIR})
  #add_definitions(-D${LIBXML2_DEFINITIONS})
  IF (${LIBXML2_LIBRARY} MATCHES ".+static.*\\.lib$" OR ${LIBXML2_LIBRARY} MATCHES ".+\\.a$")
    add_definitions(-DLIBXML_STATIC)
    IF(MSVC)
      target_link_libraries(filex ws2_32.lib)
      target_link_libraries(dir2sqlite ws2_32.lib)
      target_link_libraries(dir2csv ws2_32.lib)
      target_link_libraries(pietextx ws2_32.lib)
      target_link_libraries(cxproc ws2_32.lib)
      target_link_libraries(cxproc-test ws2_32.lib)
      target_link_libraries(cxproc-cgi ws2_32.lib)
      #    IF (LIBMICROHTTPD_FOUND)
      #      target_link_libraries(cxproc-httpd ws2_32.lib)
      #    ENDIF ()
    ENDIF (MSVC)
  ENDIF ()
ENDIF ()

################################################################################
#
# libxslt
#

FIND_PACKAGE( LibXslt )

IF (LIBXSLT_LIBRARIES)
  INCLUDE_DIRECTORIES(${LIBXSLT_INCLUDE_DIR})
  #add_definitions(-D${LIBXSLT_DEFINITIONS})
  IF (${LIBXSLT_LIBRARIES} MATCHES ".+static.*\\.lib$" OR ${LIBXSLT_LIBRARIES} MATCHES ".+\\.a$")
    add_definitions(-DLIBXSLT_STATIC)
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

SET(CXPROC_LEGACY ON CACHE BOOL "Compile code for handling of legacy issues")
MARK_AS_ADVANCED(CXPROC_LEGACY)
IF (CXPROC_LEGACY)
  add_definitions(-DLEGACY)
ENDIF (CXPROC_LEGACY)

SET(CXPROC_EXPERIMENTAL OFF CACHE BOOL "Compile experimental code")
MARK_AS_ADVANCED(CXPROC_EXPERIMENTAL)
IF (CXPROC_EXPERIMENTAL)
  add_definitions(-DEXPERIMENTAL)
ENDIF (CXPROC_EXPERIMENTAL)

add_definitions(-DCXP_VER_FILE_VERSION_STR="${GIT_TAG}" -DCXP_VER_FILE_BRANCH_STR="${GIT_BRANCH}")

IF(MSVC)
  SET(CMAKE_C_FLAGS "/MT /Gm- /MP /GS /GR- /Gd /analyze- /errorreport:prompt -D_REENTRANT -DDEF_MEM_LEVEL=8 -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -DWIN32_LEAN_AND_MEAN")
  set(CMAKE_C_FLAGS_DEBUG "-DDEBUG /Od /Z7 /W4")
  SET(CMAKE_C_FLAGS_RELEASE "-DNDEBUG -Ot -Oy-")
  set(CMAKE_C_FLAGS_PROFILE "")
  #set(CMAKE_EXE_LINKER_FLAGS_DEBUG "")
ELSE(MSVC)
  set(CMAKE_C_FLAGS "-D_REENTRANT -DDEF_MEM_LEVEL=8 -D_DEFAULT_SOURCE")
  set(CMAKE_C_FLAGS_DEBUG "-DDEBUG -g -O0 -unused -Wall -Wpedantic -Wuninitialized")
  set(CMAKE_C_FLAGS_RELEASE "-DNDEBUG -O3")
  set(CMAKE_C_FLAGS_PROFILE "-pg -DNDEBUG -O3")
  #set(CMAKE_SHARED_LINKER_FLAGS "")
ENDIF (MSVC)

# for strict compiling only!
OPTION (CXPROC_CXX "Compile as C++ code" OFF)
MARK_AS_ADVANCED(CXPROC_CXX)

IF (CXPROC_CXX)
  # compile as C++
  IF(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /TP")
  ELSE(MSVC)
    set(CMAKE_C_COMPILER ${CMAKE_CXX_COMPILER})
    set(CMAKE_C_FLAGS "-std=c++17 ${CMAKE_C_FLAGS} -fpermissive")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
    #set(CMAKE_C_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
    #set(CMAKE_C_FLAGS_PROFILE ${CMAKE_CXX_FLAGS_PROFILE})
  ENDIF (MSVC)
ELSE(CXPROC_CXX)
  # compile as C
  IF(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /TC")
  ELSE(MSVC)
    set(CMAKE_C_FLAGS "-std=c18 ${CMAKE_C_FLAGS}")
    #set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
    #set(CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
    #set(CMAKE_C_FLAGS_PROFILE ${CMAKE_C_FLAGS_PROFILE})
  ENDIF (MSVC)
ENDIF (CXPROC_CXX)

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
  target_link_libraries(cxproc-test ${PCRE2_LIBRARY})
  target_link_libraries(cxproc-cgi ${PCRE2_LIBRARY})
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
  target_link_libraries(cxproc-test ${ZLIB_LIBRARY_RELEASE})
  target_link_libraries(cxproc-cgi  ${ZLIB_LIBRARY_RELEASE})
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
  add_definitions(-DHAVE_LIBBZ2)
  IF (${LIBBZ2_LIBRARY_RELEASE} MATCHES ".+static.*\\.lib$" OR ${LIBBZ2_LIBRARY_RELEASE} MATCHES ".+\\.a$")
    add_definitions(-DLIBBZ2_STATIC)
  ENDIF ()
  target_link_libraries(cxproc ${LIBBZ2_LIBRARY_RELEASE})
  target_link_libraries(cxproc-test ${LIBBZ2_LIBRARY_RELEASE})
  target_link_libraries(cxproc-cgi  ${LIBBZ2_LIBRARY_RELEASE})
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
  add_definitions(-DHAVE_LIBLZMA)
  #IF (${LIBLZMA_LIBRARIES} MATCHES ".+static\\.lib$" OR ${LIBLZMA_LIBRARIES} MATCHES ".+\\.a$")
  #  add_definitions(-DLIBLZMA_STATIC)
  #ENDIF ()
  target_link_libraries(filex ${LIBLZMA_LIBRARIES})
  target_link_libraries(dir2csv ${LIBLZMA_LIBRARIES})
  target_link_libraries(dir2sqlite ${LIBLZMA_LIBRARIES})
  target_link_libraries(pietextx ${LIBLZMA_LIBRARIES})
  target_link_libraries(cxproc ${LIBLZMA_LIBRARIES})
  target_link_libraries(cxproc-test ${LIBLZMA_LIBRARIES})
  target_link_libraries(cxproc-cgi  ${LIBLZMA_LIBRARIES})
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
#  add_definitions(-DHAVE_LIBMICROHTTPD)
#  IF (${LIBMICROHTTPD_LIBRARIES} MATCHES ".+static\\.lib$" OR ${LIBMICROHTTPD_LIBRARIES} MATCHES ".+\\.a$")
#    add_definitions(-DLIBMICROHTTPD_STATIC)
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
  target_link_libraries(cxproc-test ${CMAKE_THREAD_LIBS_INIT})
  target_link_libraries(cxproc-cgi ${CMAKE_THREAD_LIBS_INIT})
  #IF (LIBMICROHTTPD_FOUND)
  #  target_link_libraries(cxproc-httpd ${CMAKE_THREAD_LIBS_INIT})
  #ENDIF ()
ENDIF (CXPROC_PTHREAD)

################################################################################
#
# CURL
#

INCLUDE_DIRECTORIES(${CURL_INCLUDE_DIR})

add_definitions(-DHAVE_LIBCURL)
IF (${CURL_LIBRARY_RELEASE} MATCHES ".+static\\.lib$" OR ${CURL_LIBRARY_RELEASE} MATCHES ".+\\.a$")
  add_definitions(-DLIBCURL_STATIC)
ENDIF ()

target_link_libraries(filex ${CURL_LIBRARY_RELEASE})
target_link_libraries(dir2csv ${CURL_LIBRARY_RELEASE})
target_link_libraries(dir2sqlite ${CURL_LIBRARY_RELEASE})
target_link_libraries(cxproc ${CURL_LIBRARY_RELEASE})
target_link_libraries(cxproc-test ${CURL_LIBRARY_RELEASE})
target_link_libraries(cxproc-cgi  ${CURL_LIBRARY_RELEASE})
#IF (LIBMICROHTTPD_FOUND)
#  target_link_libraries(cxproc-httpd ${CURL_LIBRARY_RELEASE})
#ENDIF ()

IF(MSVC)
  # additional libraries with VC++
  target_link_libraries(filex crypt32 wldap32)
  target_link_libraries(dir2csv crypt32 wldap32)
  target_link_libraries(dir2sqlite crypt32 wldap32)
  target_link_libraries(cxproc crypt32 wldap32)
  target_link_libraries(cxproc-test crypt32 wldap32)
  target_link_libraries(cxproc-cgi  crypt32 wldap32)
ELSE(MSVC)
  target_link_libraries(filex ssl crypto)
  target_link_libraries(dir2csv ssl crypto)
  target_link_libraries(dir2sqlite ssl crypto)
  target_link_libraries(cxproc ssl crypto)
  target_link_libraries(cxproc-test ssl crypto)
  target_link_libraries(cxproc-cgi  ssl crypto)
  #IF (LIBMICROHTTPD_FOUND)
  #  target_link_libraries(cxproc-httpd ssl crypto)
  #ENDIF ()
ENDIF(MSVC)

################################################################################
#
# add list of mandatory libraries
#

target_link_libraries(filex ${LIBICONV_LIBRARY} ${LIBCHARSET_LIBRARY} ${LIBXML2_LIBRARY})
target_link_libraries(dir2csv ${LIBICONV_LIBRARY} ${LIBCHARSET_LIBRARY} ${LIBXML2_LIBRARY})
target_link_libraries(dir2sqlite ${LIBICONV_LIBRARY} ${LIBCHARSET_LIBRARY} ${LIBXML2_LIBRARY})
target_link_libraries(pietextx ${LIBICONV_LIBRARY} ${LIBCHARSET_LIBRARY} ${LIBXML2_LIBRARY})
target_link_libraries(cxproc ${LIBICONV_LIBRARY} ${LIBCHARSET_LIBRARY} ${LIBXML2_LIBRARY} ${LIBXSLT_LIBRARIES})
target_link_libraries(cxproc-test ${LIBICONV_LIBRARY} ${LIBCHARSET_LIBRARY} ${LIBXML2_LIBRARY} ${LIBXSLT_LIBRARIES})
target_link_libraries(cxproc-cgi ${LIBICONV_LIBRARY} ${LIBCHARSET_LIBRARY} ${LIBXML2_LIBRARY} ${LIBXSLT_LIBRARIES})

#IF (LIBMICROHTTPD_FOUND)
#  target_link_libraries(cxproc-httpd ${LIBICONV_LIBRARY} ${LIBCHARSET_LIBRARY} ${LIBXML2_LIBRARY} ${LIBXSLT_LIBRARIES} ${LIBMICROHTTPD_LIBRARIES})
#ENDIF ()

################################################################################
#
# add the command to generate the source code

# TODO: process "git log -n 1" or "git branch --contains"
IF(MSVC)
  #ADD_CUSTOM_COMMAND (
  #  OUTPUT ${CXPROC_SRC_DIR}/svn_revision.h ${CXPROC_SRC_DIR}/svn_info.h
  #  COMMAND ${PROJECT_SOURCE_DIR}/build/svn_revision.bat
  #  )
ELSE(MSVC)
  # ADD_CUSTOM_COMMAND (
  #   OUTPUT ${PROJECT_SOURCE_DIR}/build/svn_info.log
  #   COMMAND svn info ${PROJECT_SOURCE_DIR} > ${PROJECT_SOURCE_DIR}/build/svn_info.log
  #   )
  
  # ADD_CUSTOM_COMMAND (
  #   OUTPUT ${CXPROC_SRC_DIR}/svn_revision.h
  #   COMMAND awk -f ${PROJECT_SOURCE_DIR}/build/svn_revision.awk ${PROJECT_SOURCE_DIR}/build/svn_info.log > ${CXPROC_SRC_DIR}/svn_revision.h
  #   DEPENDS ${PROJECT_SOURCE_DIR}/build/svn_info.log
  #   )
  
  # ADD_CUSTOM_COMMAND (
  #   OUTPUT ${CXPROC_SRC_DIR}/svn_info.h
  #   COMMAND awk -f ${PROJECT_SOURCE_DIR}/build/svn_info.awk ${PROJECT_SOURCE_DIR}/build/svn_info.log > ${CXPROC_SRC_DIR}/svn_info.h
  #   DEPENDS ${PROJECT_SOURCE_DIR}/build/svn_info.log
  #   )
ENDIF(MSVC)
 
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
  target_link_libraries(cxproc-test -ldl -lm)
  target_link_libraries(cxproc-cgi -ldl -lm)
  #IF (LIBMICROHTTPD_FOUND)
  #  target_link_libraries(cxproc-httpd -ldl -lm)
  #ENDIF ()
  IF (CXPROC_LIBZ)
    target_link_libraries(filex -lz)
    target_link_libraries(dir2csv -lz)
    target_link_libraries(dir2sqlite -lz)
    target_link_libraries(cxproc -lz)
    target_link_libraries(cxproc-test -lz)
    target_link_libraries(cxproc-cgi -lz)
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

set_target_properties(test-cgi
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

