#
#
#
cmake_minimum_required(VERSION 3.0) # setting this is required

project(cxproc)            # this sets the project name

# https://stackoverflow.com/questions/24460486/cmake-build-type-is-not-being-used-in-cmakelists-txt
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Profile" CACHE STRING "" FORCE) 
#set(CMAKE_BUILD_TYPE "Debug")

include (CTest)

# for profiling s. https://stackoverflow.com/questions/26491948/how-to-use-gprof-with-cmake
# cmake -DCMAKE_CXX_FLAGS=-pg -DCMAKE_EXE_LINKER_FLAGS=-pg -DCMAKE_SHARED_LINKER_FLAGS=-pg ..
# gprof bin/cxproc | ../build/gprof2dot.py | dot -Tpng -o gprof.png
# or see test/main.sh and https://github.com/jrfonseca/gprof2dot
# http://gernotklingler.com/blog/gprof-valgrind-gperftools-evaluation-tools-application-level-cpu-profiling-linux/

# https://cmake.org/Wiki/CMake/Testing_With_CTest#Simple_Testing
#enable_testing()

# s. https://cmake.org/Wiki/CMake_Useful_Variables

# CMAKE_POLICY(SET CMP0026 OLD)

# SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
SET(CMAKE_VERBOSE_MAKEFILE ON)

# http://www.vtk.org/Wiki/CMake:Eclipse_UNIX_Tutorial
# http://stackoverflow.com/questions/9453851/how-to-configure-eclipse-cdt-for-cmake
#FIND_PACKAGE(EclipseCDT4)

# (shell-command "(cmake --graphviz=cxproc.dot --build x86_64-gnu-linux-debug/ && dot -T png -o cxproc.png cxproc.dot.cxproc)")

FIND_PACKAGE(PkgConfig)

FIND_PATH(CXPROC_SRC_DIR
  NAME cxproc.c
  PATHS "src")

FIND_PATH(CXPROC_SRCLIB_DIR
  NAME cgi-util
  PATHS "third-party")

SET(CXPROC_PREFIX "${PROJECT_BINARY_DIR}/.." CACHE PATH "Prefix")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/third-party" "${PROJECT_SOURCE_DIR}/third-party/option" "${CXPROC_SRC_DIR}" "${CXPROC_SRC_DIR}/option" "${CXPROC_PREFIX}/include")

# working directory for tests
SET(CXPROC_TEST_DIR "${PROJECT_SOURCE_DIR}/test")
SET(CXPROC_TEMP_DIR "${CXPROC_PREFIX}/tmp")

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
set(PROJECT_VERSION_PATCH "")

# https://vicrucann.github.io/tutorials/quick-cmake-doxygen/
# check if Doxygen is installed
find_package(Doxygen)
if (DOXYGEN_FOUND)
  option(CXPROC_DOC "Build documentation" OFF)
else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)

if (CXPROC_DOC)
  # note the option ALL which allows to build the docs together with the application
  add_custom_target( doc_doxygen ALL
    COMMAND DOXYGEN_NAME=${CMAKE_PROJECT_NAME} DOXYGEN_OUTPUT=${PROJECT_BINARY_DIR}/.. ${DOXYGEN_EXECUTABLE} ${PROJECT_SOURCE_DIR}/misc/Doxyfile
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM )
endif ()

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

SET(FILEX_HEADERS
  ${CXPROC_SRC_DIR}/version.h
#  ${CXPROC_SRC_DIR}/git_revision.h
#  ${CXPROC_SRC_DIR}/git_info.h
  ${CXPROC_SRC_DIR}/basics.h
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

SET(FILEX_SOURCES
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

###############################################################################
## target definitions #########################################################
###############################################################################

IF(MSVC)
  SET(FILEX_SOURCES ${FILEX_SOURCES} ${CXPROC_SRC_DIR}/version.rc)
  SET(CXPROC_SOURCES ${CXPROC_SOURCES} ${CXPROC_SRC_DIR}/version.rc)
ENDIF(MSVC)

################################################################################
#
# pthread
#

#FIND_PACKAGE( Threads )

#IF(CMAKE_USE_PTHREADS_INIT)
#  MESSAGE(STATUS "Found optional libpthtread: ${CMAKE_THREAD_LIBS_INIT}")
#  OPTION (CXPROC_PTHREAD "Enable support for linking cxproc with libpthread." OFF)
#ELSE ()
#  OPTION (CXPROC_THREAD "Enable support native threads." OFF)
#ENDIF ()
#MARK_AS_ADVANCED(CXPROC_THREAD)

IF(CXPROC_PTHREAD)
  SET(CXPROC_HEADERS ${CXPROC_HEADERS} ${CXPROC_SRC_DIR}/cxp/cxp_threadp.h)
  SET(CXPROC_SOURCES ${CXPROC_SOURCES} ${CXPROC_SRC_DIR}/cxp/cxp_threadp.c)
  add_definitions(-DHAVE_LIBPTHREAD)
ELSEIF (CXPROC_THREAD)
  SET(CXPROC_HEADERS ${CXPROC_HEADERS} ${CXPROC_SRC_DIR}/cxp/cxp_threadp.h)
  SET(CXPROC_SOURCES ${CXPROC_SOURCES} ${CXPROC_SRC_DIR}/cxp/cxp_threadp.c)
  add_definitions(-DWITH_THREAD)
ENDIF (CXPROC_PTHREAD)

################################################################################
#
# either local LibArchive or system LibArchive
#

find_path(LibArchive_INCLUDE_DIR NAMES archive.h
  HINTS "${CXPROC_PREFIX}/include"
  ) 

IF (LibArchive_INCLUDE_DIR)
  find_library(LibArchive_LIBRARIES NAMES libarchive archive archive_static
    HINTS "${CXPROC_PREFIX}/lib"
    )
  IF (LibArchive_LIBRARIES)
    SET(LibArchive_FOUND ON)
  ELSE(LibArchive_LIBRARIES)
    FIND_PACKAGE( LibArchive )
  ENDIF (LibArchive_LIBRARIES)
ELSE(LibArchive_INCLUDE_DIR)
  FIND_PACKAGE( LibArchive )
ENDIF (LibArchive_INCLUDE_DIR)

IF (LibArchive_FOUND)
  MESSAGE(STATUS "Found optional LibArchive: ${LibArchive_LIBRARIES}")
  OPTION(CXPROC_ARCHIVE "Enable support for linking cxproc with libarchive." OFF)
ENDIF (LibArchive_FOUND)

IF (CXPROC_ARCHIVE)
  INCLUDE_DIRECTORIES(${LibArchive_INCLUDE_DIR})
  SET(FILEX_HEADERS ${FILEX_HEADERS} ${CXPROC_SRC_DIR}/option/archive/archive.h)
  SET(FILEX_SOURCES ${FILEX_SOURCES} ${CXPROC_SRC_DIR}/option/archive/archive.c)
  SET(CXPROC_HEADERS ${CXPROC_HEADERS} ${CXPROC_SRC_DIR}/option/archive/cxp_archive.h)
  SET(CXPROC_SOURCES ${CXPROC_SOURCES} ${CXPROC_SRC_DIR}/option/archive/cxp_archive.c ${CXPROC_SRC_DIR}/option/archive/archive.c)
  add_definitions(-DHAVE_LIBARCHIVE)
  IF (${LibArchive_LIBRARIES} MATCHES ".+static\\.lib$" OR ${LibArchive_LIBRARIES} MATCHES ".+\\.a$")
    add_definitions(-DLIBARCHIVE_STATIC)
  ENDIF ()
ENDIF (CXPROC_ARCHIVE)

################################################################################
#
# SQLite3 library
#

find_path(SQLITE3_INCLUDE_DIR NAMES sqlite3.h
  HINTS "${CXPROC_PREFIX}/include"
  ) 

IF (SQLITE3_INCLUDE_DIR)
  IF (EXISTS "${SQLITE3_INCLUDE_DIR}/sqlite3.c")
    SET(SQLITE3_SOURCE_FILE "${SQLITE3_INCLUDE_DIR}/sqlite3.c")
    SET(SQLITE3_FOUND ON)
  ELSE()
    find_library(SQLITE3_LIBRARIES NAMES libsqlite3 sqlite3
      HINTS "${CXPROC_PREFIX}/lib"
      )
    IF (SQLITE3_LIBRARIES)
      SET(SQLITE3_FOUND ON)
    ELSE(SQLITE3_LIBRARIES)
      FIND_PACKAGE( SQLite3 )
    ENDIF (SQLITE3_LIBRARIES)
  ENDIF()
ELSE(SQLITE3_INCLUDE_DIR)
FIND_PACKAGE( SQLITE3 )
ENDIF (SQLITE3_INCLUDE_DIR)

IF (SQLITE3_FOUND)
  MESSAGE(STATUS "Found optional sqlite3: ${SQLITE3_SOURCE_FILE} ${SQLITE3_LIBRARIES}")
  OPTION (CXPROC_SQLITE3 "Enable support for linking cxproc with sqlite3." OFF)
ENDIF (SQLITE3_FOUND)

IF (CXPROC_SQLITE3)
  INCLUDE_DIRECTORIES(${SQLITE3_INCLUDE_DIR})
  SET(CXPROC_HEADERS ${CXPROC_HEADERS} ${CXPROC_SRC_DIR}/option/database/database.h ${CXPROC_SRC_DIR}/option/database/cxp_database.h)
  SET(CXPROC_SOURCES ${CXPROC_SOURCES} ${CXPROC_SRC_DIR}/option/database/database.c ${CXPROC_SRC_DIR}/option/database/cxp_database.c)
  SET(FILEX_HEADERS ${FILEX_HEADERS} ${CXPROC_SRC_DIR}/option/database/database.h)
  SET(FILEX_SOURCES ${FILEX_SOURCES} ${CXPROC_SRC_DIR}/option/database/database.c)
  add_definitions(-DHAVE_LIBSQLITE3)
  IF (SQLITE3_SOURCE_FILE)
    SET(CXPROC_SOURCES ${CXPROC_SOURCES} ${SQLITE3_SOURCE_FILE})
    SET(FILEX_SOURCES ${FILEX_SOURCES} ${SQLITE3_SOURCE_FILE})
    add_definitions(-DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION)
  ENDIF ()
ENDIF (CXPROC_SQLITE3)

################################################################################
#
# either local libid3tag or system libid3tag
#
# code of libid3tag is not C++ compilable and memory management should be separated
#

#TODO: OPTION(CXPROC_LIBVORBIS "Include OGG/Vorbis code" OFF)
#SET(CXPROC_LIBVORBIS OFF)

find_path(LIBID3TAG_INCLUDE_DIR NAMES id3tag.h
  HINTS "${CXPROC_PREFIX}/include"
  ) 

IF (LIBID3TAG_INCLUDE_DIR)
  find_library(LIBID3TAG_LIBRARIES NAMES libid3tag.a id3tag libid3tag.lib
    HINTS "${CXPROC_PREFIX}/lib"
    )
  IF (LIBID3TAG_LIBRARIES)
    SET(LIBID3TAG_FOUND ON)
  ELSE (LIBID3TAG_LIBRARIES)
#    FIND_PACKAGE( LIBID3TAG )
  ENDIF (LIBID3TAG_LIBRARIES)
ENDIF (LIBID3TAG_INCLUDE_DIR)

IF (LIBID3TAG_FOUND)
  MESSAGE(STATUS "Found optional LIBID3TAG: ${LIBID3TAG_LIBRARIES}")
  OPTION (CXPROC_LIBID3TAG "Enable support for linking cxproc with libid3tag." OFF)
ENDIF (LIBID3TAG_FOUND)

IF (CXPROC_LIBID3TAG)
  INCLUDE_DIRECTORIES(${LIBID3TAG_INCLUDE_DIR})
  add_definitions(-DHAVE_LIBID3TAG)
ENDIF (CXPROC_LIBID3TAG)

IF (CXPROC_LIBID3TAG OR CXPROC_LIBVORBIS)
  SET(CXPROC_HEADERS ${CXPROC_HEADERS} ${CXPROC_SRC_DIR}/option/audio/audio.h)
  SET(CXPROC_SOURCES ${CXPROC_SOURCES} ${CXPROC_SRC_DIR}/option/audio/audio.c)
  SET(FILEX_HEADERS ${FILEX_HEADERS} ${CXPROC_SRC_DIR}/option/audio/audio.h)
  SET(FILEX_SOURCES ${FILEX_SOURCES} ${CXPROC_SRC_DIR}/option/audio/audio.c)
ENDIF ()


################################################################################
#
# JSON
#
OPTION(CXPROC_JSON "Include JSON code" OFF)
MARK_AS_ADVANCED(CXPROC_JSON)

IF (CXPROC_JSON)
  add_definitions(-DHAVE_JSON)
  SET(CXPROC_HEADERS ${CXPROC_HEADERS} ${CXPROC_SRC_DIR}/option/json/json.h)
  SET(CXPROC_SOURCES ${CXPROC_SOURCES} ${CXPROC_SRC_DIR}/option/json/json.c)
ENDIF (CXPROC_JSON)

################################################################################
#
# local pcre2 (impact on PIE)
#
FIND_PATH(LIBPCRE2_INCLUDE_DIR NAME pcre2.h
  HINTS "${CXPROC_PREFIX}/include"
  )

IF (LIBPCRE2_INCLUDE_DIR)
  FIND_LIBRARY(LIBPCRE2_LIBRARIES NAMES pcre2-8 pcre2-8d libpcre2-8.a
    HINTS "${CXPROC_PREFIX}/lib"
    )
ELSE ()
  FIND_LIBRARY(LIBPCRE2_LIBRARIES NAMES pcre2)
ENDIF (LIBPCRE2_INCLUDE_DIR)

IF (LIBPCRE2_LIBRARIES)
  SET(LIBPCRE2_FOUND ON)
ENDIF ()

IF (LIBPCRE2_FOUND)
  MESSAGE(STATUS "Found optional libpcre2: ${LIBPCRE2_LIBRARIES}")
  OPTION(CXPROC_PCRE2 "Enable support for linking cxproc with libpcre2." ON)
  OPTION(CXPROC_PIE "Include PIE code" ON)
ELSE (LIBPCRE2_FOUND)
  SET(CXPROC_PCRE2 OFF)
ENDIF (LIBPCRE2_FOUND)

IF (CXPROC_PCRE2)
  add_definitions(-DHAVE_PCRE2 -DPCRE2_CODE_UNIT_WIDTH=8)
  IF (${LIBPCRE2_LIBRARIES} MATCHES ".+static.*\\.lib$" OR ${LIBPCRE2_LIBRARIES} MATCHES ".+\\.a$")
    #ADD_LIBRARY(pcre2-8 STATIC IMPORTED)
    add_definitions(-DPCRE2_STATIC)
  ENDIF ()
ELSE ()
ENDIF ()

################################################################################
#
# PIE
#

IF (CXPROC_PIE)
  add_definitions(-DHAVE_PIE)
  SET(CXPROC_HEADERS ${CXPROC_HEADERS} 
    ${CXPROC_SRC_DIR}/option/pie/pie_calendar.h
    ${CXPROC_SRC_DIR}/option/pie/pie_element.h
    ${CXPROC_SRC_DIR}/option/pie/pie_text_tags.h
    ${CXPROC_SRC_DIR}/option/pie/pie_text_blocks.h
    ${CXPROC_SRC_DIR}/option/pie/pie_text.h ${CXPROC_SRC_DIR}/option/pie/pie_dtd.h
    ${CXPROC_SRC_DIR}/option/pie/pie_timezone.h
    ${CXPROC_SRCLIB_DIR}/option/sunriset/sunriset.h
    ${CXPROC_SRC_DIR}/option/ics/ics.h
    ${CXPROC_SRC_DIR}/option/vcf/vcf.h
    )

  SET(CXPROC_SOURCES ${CXPROC_SOURCES} 
    ${CXPROC_SRC_DIR}/option/pie/pie_calendar.c
    ${CXPROC_SRC_DIR}/option/pie/pie_element.c
    ${CXPROC_SRC_DIR}/option/pie/pie_text_tags.c
    ${CXPROC_SRC_DIR}/option/pie/pie_text_blocks.c
    ${CXPROC_SRC_DIR}/option/pie/pie_text.c
    ${CXPROC_SRC_DIR}/option/pie/pie_timezone.c
    ${CXPROC_SRCLIB_DIR}/option/sunriset/sunriset.c
    ${CXPROC_SRC_DIR}/option/ics/ics.c
    ${CXPROC_SRC_DIR}/option/vcf/vcf.c
    )

  SET(FILEX_SOURCES ${FILEX_SOURCES} 
    ${CXPROC_SRC_DIR}/option/pie/pie_element.h
    ${CXPROC_SRC_DIR}/option/pie/pie_text_tags.h
    ${CXPROC_SRC_DIR}/option/pie/pie_text_blocks.h
    ${CXPROC_SRC_DIR}/option/pie/pie_dtd.h
    ${CXPROC_SRC_DIR}/option/pie/pie_timezone.h
    ${CXPROC_SRC_DIR}/option/ics/ics.h
    ${CXPROC_SRC_DIR}/option/vcf/vcf.h
    )
  
  SET(FILEX_SOURCES ${FILEX_SOURCES}
    ${CXPROC_SRC_DIR}/option/pie/pie_element.c
    ${CXPROC_SRC_DIR}/option/pie/pie_text_tags.c
    ${CXPROC_SRC_DIR}/option/pie/pie_text_blocks.c
    ${CXPROC_SRC_DIR}/option/pie/pie_timezone.c
    ${CXPROC_SRC_DIR}/option/ics/ics.c
    ${CXPROC_SRC_DIR}/option/vcf/vcf.c
    )

  OPTION(CXPROC_MARKDOWN "Compile MARKDOWN code" ON)
ELSE ()
  SET(CXPROC_MARKDOWN OFF)
ENDIF (CXPROC_PIE)

IF (CXPROC_MARKDOWN)
  add_definitions(-DWITH_MARKDOWN)
ENDIF (CXPROC_MARKDOWN)

################################################################################
#
# PETRINET
#
OPTION(CXPROC_PETRINET "Include Petrinet code" ON)

IF (CXPROC_PETRINET)
  add_definitions(-DHAVE_PETRINET)
  SET(CXPROC_SOURCES ${CXPROC_SOURCES} 
  ${CXPROC_SRC_DIR}/option/petrinet/petrinet.c ${CXPROC_SRC_DIR}/option/petrinet/petrinet.h
)
ENDIF (CXPROC_PETRINET)

################################################################################
#
# either local ZLIB or system ZLIB
#

find_path(ZLIB_INCLUDE_DIR NAMES zlib.h
  HINTS "${CXPROC_PREFIX}/include"
  ) 

IF (ZLIB_INCLUDE_DIR)
  find_library(ZLIB_LIBRARIES NAMES libz zlib zlibd zlibStatic
    HINTS "${CXPROC_PREFIX}/lib"
    )
  IF (ZLIB_LIBRARIES)
    SET(ZLIB_FOUND ON)
  ELSE(ZLIB_LIBRARIES)
    FIND_PACKAGE( ZLIB )
  ENDIF (ZLIB_LIBRARIES)
ELSE(ZLIB_INCLUDE_DIR)
  FIND_PACKAGE( ZLIB )
ENDIF (ZLIB_INCLUDE_DIR)

IF (ZLIB_FOUND)
  MESSAGE(STATUS "Found zlib: ${ZLIB_LIBRARIES}")
  SET(CXPROC_LIBZ ON)
ELSE (ZLIB_FOUND)
  MESSAGE(FATAL_ERROR "zlib must be available (libxml2)")
ENDIF (ZLIB_FOUND)

IF (CXPROC_LIBZ)
  INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR} ${CXPROC_SRCLIB_DIR}/minizip)
  add_definitions(-DHAVE_ZLIB)
  SET(FILEX_SOURCES ${FILEX_SOURCES}
    ${CXPROC_SRCLIB_DIR}/minizip/ioapi.c
    ${CXPROC_SRCLIB_DIR}/minizip/zip.c
    ${CXPROC_SRCLIB_DIR}/minizip/unzip.c
    ${CXPROC_SRCLIB_DIR}/xmlzipio/xmlzipio.c)
  SET(FILEX_HEADERS ${FILEX_HEADERS}
    ${CXPROC_SRCLIB_DIR}/minizip/ioapi.h
    ${CXPROC_SRCLIB_DIR}/minizip/zip.h
    ${CXPROC_SRCLIB_DIR}/minizip/unzip.h
    ${CXPROC_SRCLIB_DIR}/xmlzipio/xmlzipio.h)
  SET(CXPROC_SOURCES ${CXPROC_SOURCES}
    ${CXPROC_SRCLIB_DIR}/minizip/ioapi.c
    ${CXPROC_SRCLIB_DIR}/minizip/zip.c
    ${CXPROC_SRCLIB_DIR}/minizip/unzip.c
    ${CXPROC_SRCLIB_DIR}/xmlzipio/xmlzipio.c)
  SET(CXPROC_HEADERS ${CXPROC_HEADERS}
    ${CXPROC_SRCLIB_DIR}/minizip/ioapi.h
    ${CXPROC_SRCLIB_DIR}/minizip/zip.h
    ${CXPROC_SRCLIB_DIR}/minizip/unzip.h
    ${CXPROC_SRCLIB_DIR}/xmlzipio/xmlzipio.h)
  IF (${ZLIB_LIBRARIES} MATCHES ".+static.*\\.lib$" OR ${ZLIB_LIBRARIES} MATCHES ".+\\.a$")
    add_definitions(-DZLIB_STATIC)
  ENDIF ()
ENDIF (CXPROC_LIBZ)

################################################################################
#
# either local liblzma or system liblzma
#

find_path(LIBLZMA_INCLUDE_DIR NAMES lzma.h
  HINTS "${CXPROC_PREFIX}/include"
  ) 

IF (LIBLZMA_INCLUDE_DIR)
  find_library(LIBLZMA_LIBRARIES NAMES liblzma lzma lzmad lzma_static
    HINTS "${CXPROC_PREFIX}/lib"
    )
  IF (LIBLZMA_LIBRARIES)
    SET(LIBLZMA_FOUND ON)
  ELSE(LIBLZMA_LIBRARIES)
    FIND_PACKAGE( LibLZMA )
  ENDIF (LIBLZMA_LIBRARIES)
ELSE(LIBLZMA_INCLUDE_DIR)
  FIND_PACKAGE( LibLZMA )
ENDIF (LIBLZMA_INCLUDE_DIR)

IF (LIBLZMA_FOUND)
  MESSAGE(STATUS "Found liblzma: ${LIBLZMA_LIBRARIES}")
  SET(CXPROC_LZMA ON)
ELSE (LIBLZMA_FOUND)
  MESSAGE(FATAL_ERROR "liblzma must be available (libxml2)")
ENDIF (LIBLZMA_FOUND)

################################################################################
#
# RP
#
#OPTION(CXPROC_RP "Include RP code" OFF)

IF (CXPROC_RP)
  add_definitions(-DHAVE_RP)
  SET(CXPROC_HEADERS ${CXPROC_HEADERS} ${CXPROC_SRC_DIR}/rp.h)
  SET(CXPROC_SOURCES ${CXPROC_SOURCES} ${CXPROC_SRC_DIR}/rp.c)
ENDIF (CXPROC_RP)


################################################################################
#
# local duktape
#
FIND_PATH(DUKTAPE_INCLUDE_DIR NAME duktape.h
  HINTS "${CXPROC_PREFIX}/include"
  )

IF (DUKTAPE_INCLUDE_DIR)
  IF (EXISTS "${DUKTAPE_INCLUDE_DIR}/duktape.c")
    SET(DUKTAPE_SOURCE_FILE "${DUKTAPE_INCLUDE_DIR}/duktape.c")
    SET(DUKTAPE_FOUND ON)
  ELSE()
    FIND_LIBRARY(DUKTAPE_LIBRARIES NAMES duktape
      HINTS "${CXPROC_PREFIX}/lib"
      )
    IF (DUKTAPE_LIBRARIES)
      SET(DUKTAPE_FOUND ON)
    ENDIF (DUKTAPE_LIBRARIES)
  ENDIF()
ENDIF (DUKTAPE_INCLUDE_DIR)

IF (DUKTAPE_FOUND)
  MESSAGE(STATUS "Found optional duktape: ${DUKTAPE_LIBRARIES}")
  OPTION (CXPROC_DUKTAPE "Enable support for linking cxproc with duktape." OFF)
ENDIF (DUKTAPE_FOUND)

IF (CXPROC_DUKTAPE)
  add_definitions(-DHAVE_JS)
  SET(CXPROC_HEADERS ${CXPROC_HEADERS} ${CXPROC_SRC_DIR}/option/script/script.h)
  SET(CXPROC_SOURCES ${CXPROC_SOURCES} ${CXPROC_SRC_DIR}/option/script/script.c)
  IF (DUKTAPE_SOURCE_FILE)
    SET(CXPROC_HEADERS ${CXPROC_HEADERS} ${DUKTAPE_INCLUDE_DIR}/duktape.h)
    SET(CXPROC_SOURCES ${CXPROC_SOURCES} ${DUKTAPE_SOURCE_FILE})
  ENDIF (DUKTAPE_SOURCE_FILE)
  INCLUDE_DIRECTORIES(${DUKTAPE_INCLUDE_DIR})
ENDIF (CXPROC_DUKTAPE)

################################################################################
#
# either local libexif or system libexif
#

find_path(LIBEXIF_INCLUDE_DIR NAMES libexif/exif-data.h
  HINTS "${CXPROC_PREFIX}/include"
  ) 

IF (LIBEXIF_INCLUDE_DIR)
  find_library(LIBEXIF_LIBRARY NAMES libexif.a libexif
    HINTS "${CXPROC_PREFIX}/lib"
    )
  IF (LIBEXIF_LIBRARY)
    SET(LIBEXIF_FOUND ON)
  ELSE ()
    # FIND_PACKAGE( LIBEXIF )
    SET(LIBEXIF_LIBRARY "")
  ENDIF ()
ELSE (LIBEXIF_INCLUDE_DIR)
  # FIND_PACKAGE( LIBEXIF )
ENDIF (LIBEXIF_INCLUDE_DIR)

IF (LIBEXIF_FOUND)
  MESSAGE(STATUS "Found optional LIBEXIF: ${LIBEXIF_LIBRARY}")
  OPTION (CXPROC_LIBEXIF "Enable support for linking cxproc with libexif." OFF)
ENDIF (LIBEXIF_FOUND)

IF (CXPROC_LIBEXIF)
  INCLUDE_DIRECTORIES(${LIBEXIF_INCLUDE_DIR})
  add_definitions(-DHAVE_LIBEXIF)
ENDIF (CXPROC_LIBEXIF)

IF (CXPROC_LIBEXIF)
  SET(FILEX_HEADERS ${FILEX_HEADERS} ${CXPROC_SRC_DIR}/option/image/image_exif.h)
  SET(FILEX_SOURCES ${FILEX_SOURCES} ${CXPROC_SRC_DIR}/option/image/image_exif.c)
  SET(CXPROC_HEADERS ${CXPROC_HEADERS} ${CXPROC_SRC_DIR}/option/image/image_exif.h)
  SET(CXPROC_SOURCES ${CXPROC_SOURCES} ${CXPROC_SRC_DIR}/option/image/image_exif.c)
ENDIF ()

################################################################################
#
# libmagick
#

#OPTION(CXPROC_LIBMAGICK "Include ImageMagick code" OFF)
SET(CXPROC_LIBMAGICK OFF)

IF (CXPROC_LIBMAGICK)
  INCLUDE_DIRECTORIES(${LIBMAGICK_INCLUDE_DIR})
  add_definitions(-DHAVE_LIBMAGICK)
ENDIF (CXPROC_LIBMAGICK)

IF (CXPROC_LIBMAGICK)
  SET(CXPROC_HEADERS ${CXPROC_HEADERS} ${CXPROC_SRC_DIR}/option/image/image.h)
  SET(CXPROC_SOURCES ${CXPROC_SOURCES} ${CXPROC_SRC_DIR}/option/image/image.c)
ENDIF ()

################################################################################
# The data is just added to the executable, because in some IDEs (QtCreator) 
# files are invisible when they are not explicitly part of the project.
ADD_EXECUTABLE(filex
 ${CXPROC_SRC_DIR}/filex.c
 ${FILEX_SOURCES} ${FILEX_HEADERS})

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
  HTTPPREFIX="http://localhost:8189/")

ADD_EXECUTABLE(cxproc-cgi
 ${CXPROC_SRC_DIR}/cxproc-cgi.c
 ${CXPROC_SRC_DIR}/cxp/cxp_context_cgi.c ${CXPROC_SRC_DIR}/cxp/cxp_context_cgi.h
 ${CXPROC_SRCLIB_DIR}/cgi-util/cgi-util.c ${CXPROC_SRCLIB_DIR}/cgi-util/cgi-util.h
 ${CXPROC_SOURCES} ${CXPROC_HEADERS})

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

# This copies all resource files in the build directory.
# We need this, because we want to work with paths relative to the executable.
#file(COPY ${data} DESTINATION resources)

###############################################################################
## dependencies ###############################################################
###############################################################################

################################################################################
#
# local libiconv
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
# either local libxml2 or system libxml2
#

find_path(LIBXML2_INCLUDE_DIR NAMES libxml/xpath.h
  HINTS "${CXPROC_PREFIX}/include/libxml2"
  HINTS "${CXPROC_PREFIX}/include"
  ) 

IF (LIBXML2_INCLUDE_DIR)
  find_library(LIBXML2_LIBRARIES NAMES xml2 libxml2 libxml2.a libxml2_a
    HINTS "${CXPROC_PREFIX}/lib"
    )
  IF (LIBXML2_LIBRARIES)
    SET(LIBXML2_FOUND ON)
  ELSE ()
    FIND_PACKAGE( LibXml2 )
  ENDIF ()
ELSE (LIBXML2_INCLUDE_DIR)
  FIND_PACKAGE( LibXml2 )
ENDIF (LIBXML2_INCLUDE_DIR)

IF (LIBXML2_FOUND)
  MESSAGE(STATUS "Found required libxml2: ${LIBXML2_LIBRARIES}")
ELSE (LIBXML2_FOUND)
  MESSAGE(FATAL_ERROR "libxml2 must be available")
ENDIF (LIBXML2_FOUND)

INCLUDE_DIRECTORIES(${LIBXML2_INCLUDE_DIR})
#add_definitions(-D${LIBXML2_DEFINITIONS})
IF (${LIBXML2_LIBRARIES} MATCHES ".+static.*\\.lib$" OR ${LIBXML2_LIBRARIES} MATCHES ".+\\.a$")
  add_definitions(-DLIBXML_STATIC)
  IF(MSVC)
    target_link_libraries(filex ws2_32.lib)
    target_link_libraries(cxproc ws2_32.lib)
    target_link_libraries(cxproc-test ws2_32.lib)
    target_link_libraries(cxproc-cgi ws2_32.lib)
#    IF (LIBMICROHTTPD_FOUND)
#      target_link_libraries(cxproc-httpd ws2_32.lib)
#    ENDIF ()
  ENDIF (MSVC)
ENDIF ()

################################################################################
#
# either local libxslt or system libxslt
#

find_path(LIBXSLT_INCLUDE_DIR NAMES libxslt/xslt.h
  HINTS "${CXPROC_PREFIX}/include"
  ) 

IF (LIBXSLT_INCLUDE_DIR)
  find_library(LIBXSLT_LIBRARIES NAMES xslt libxslt libxslt.a libxslt_a
    HINTS "${CXPROC_PREFIX}/lib"
    )
  IF (LIBXSLT_LIBRARIES)
    SET(LIBXSLT_FOUND ON)
  ELSE ()
    FIND_PACKAGE( LibXslt )
  ENDIF ()
ELSE (LIBXSLT_INCLUDE_DIR)
  FIND_PACKAGE( LibXslt )
ENDIF (LIBXSLT_INCLUDE_DIR)

IF (LIBXSLT_FOUND)
  MESSAGE(STATUS "Found required libxslt: ${LIBXSLT_LIBRARIES}")
ELSE (LIBXSLT_FOUND)
  MESSAGE(FATAL_ERROR "libxslt must be available")
ENDIF (LIBXSLT_FOUND)

INCLUDE_DIRECTORIES(${LIBXSLT_INCLUDE_DIR})
#add_definitions(-D${LIBXSLT_DEFINITIONS})
IF (${LIBXSLT_LIBRARIES} MATCHES ".+static.*\\.lib$" OR ${LIBXSLT_LIBRARIES} MATCHES ".+\\.a$")
  add_definitions(-DLIBXSLT_STATIC)
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

SET(CXPROC_AJAX ON CACHE BOOL "Compile AJAX code")

IF (CXPROC_AJAX)
  add_definitions(-DWITH_AJAX)
ENDIF (CXPROC_AJAX)

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

# assign c++ options to c
IF (CXPROC_CXX)
  IF(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /TP")
  ELSE(MSVC)
    set(CMAKE_C_COMPILER ${CMAKE_CXX_COMPILER})
    #set(CMAKE_C_FLAGS ${CMAKE_CXX_FLAGS})
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fpermissive")
    #set(CMAKE_C_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
    #set(CMAKE_C_FLAGS_PROFILE ${CMAKE_CXX_FLAGS_PROFILE})
  ENDIF (MSVC)
ELSE(CXPROC_CXX)
  IF(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /TC")
  ELSE(MSVC)
    set(CMAKE_C_FLAGS "-std=c11 ${CMAKE_C_FLAGS}")
    #set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
    #set(CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
    #set(CMAKE_C_FLAGS_PROFILE ${CMAKE_C_FLAGS_PROFILE})
  ENDIF (MSVC)
ENDIF (CXPROC_CXX)

################################################################################
#
# local pcre2
#

IF (CXPROC_PCRE2)
  target_link_libraries(filex ${LIBPCRE2_LIBRARIES})
  target_link_libraries(cxproc ${LIBPCRE2_LIBRARIES})
  target_link_libraries(cxproc-test ${LIBPCRE2_LIBRARIES})
  target_link_libraries(cxproc-cgi ${LIBPCRE2_LIBRARIES})
  #IF (LIBMICROHTTPD_FOUND)
  #  target_link_libraries(cxproc-httpd ${LIBPCRE2_LIBRARIES})
  #ENDIF ()
ENDIF ()

################################################################################
#
# local duktape
#

IF (CXPROC_DUKTAPE)
  IF (DUKTAPE_SOURCE_FILE)
  ELSE (DUKTAPE_SOURCE_FILE)
    target_link_libraries(cxproc ${DUKTAPE_LIBRARIES})
    target_link_libraries(cxproc-test ${DUKTAPE_LIBRARIES})
    target_link_libraries(cxproc-cgi ${DUKTAPE_LIBRARIES})
    #IF (LIBMICROHTTPD_FOUND)
    #  target_link_libraries(cxproc-httpd ${DUKTAPE_LIBRARIES})
    #ENDIF ()
  ENDIF ()
ENDIF (CXPROC_DUKTAPE)

################################################################################
#
# SQLite3 library
#

IF (CXPROC_SQLITE3)
  IF (SQLITE3_SOURCE_FILE)
  ELSE (SQLITE3_SOURCE_FILE)
    target_link_libraries(filex ${SQLITE3_LIBRARIES})
    target_link_libraries(cxproc ${SQLITE3_LIBRARIES})
    target_link_libraries(cxproc-test ${SQLITE3_LIBRARIES})
    target_link_libraries(cxproc-cgi  ${SQLITE3_LIBRARIES})
    #IF (LIBMICROHTTPD_FOUND)
    #  target_link_libraries(cxproc-httpd ${SQLITE3_LIBRARIES})
    #ENDIF ()
  ENDIF ()
ENDIF (CXPROC_SQLITE3)

################################################################################
#
# either local ZLIB or system ZLIB
#

IF (CXPROC_LIBZ)
  target_link_libraries(filex ${ZLIB_LIBRARIES})
  target_link_libraries(cxproc ${ZLIB_LIBRARIES})
  target_link_libraries(cxproc-test ${ZLIB_LIBRARIES})
  target_link_libraries(cxproc-cgi  ${ZLIB_LIBRARIES})
  #IF (LIBMICROHTTPD_FOUND)
  #  target_link_libraries(cxproc-httpd ${ZLIB_LIBRARIES})
  #ENDIF ()
ENDIF (CXPROC_LIBZ)

################################################################################
#
# either local LIBBZ2 or system LIBBZ2
#

find_path(LIBBZ2_INCLUDE_DIR NAMES bzlib.h
  HINTS "${CXPROC_PREFIX}/include"
  ) 

IF (LIBBZ2_INCLUDE_DIR)
  find_library(LIBBZ2_LIBRARIES NAMES libbz2 bz2 bz2d bzip2 libbz2 libbz2Static
    HINTS "${CXPROC_PREFIX}/lib"
    )
  IF (LIBBZ2_LIBRARIES)
    SET(LIBBZ2_FOUND ON)
  ELSE(LIBBZ2_LIBRARIES)
    FIND_PACKAGE( LIBBZ2 )
  ENDIF (LIBBZ2_LIBRARIES)
ELSE(LIBBZ2_INCLUDE_DIR)
  FIND_PACKAGE( BZip2 )
ENDIF (LIBBZ2_INCLUDE_DIR)

IF (LIBBZ2_FOUND)
  MESSAGE(STATUS "Found optional LIBBZ2: ${LIBBZ2_LIBRARIES}")
  OPTION (CXPROC_LIBBZ2 "Enable support for linking cxproc with libbz2." OFF)
ELSE(LIBBZ2_FOUND)
  SET(CXPROC_LIBBZ2 OFF)
ENDIF (LIBBZ2_FOUND)

IF (CXPROC_LIBBZ2)
  INCLUDE_DIRECTORIES(${LIBBZ2_INCLUDE_DIR})
  add_definitions(-DHAVE_LIBBZ2)
  IF (${LIBBZ2_LIBRARIES} MATCHES ".+static.*\\.lib$" OR ${LIBBZ2_LIBRARIES} MATCHES ".+\\.a$")
    add_definitions(-DLIBBZ2_STATIC)
  ENDIF ()
  target_link_libraries(cxproc ${LIBBZ2_LIBRARIES})
  target_link_libraries(cxproc-test ${LIBBZ2_LIBRARIES})
  target_link_libraries(cxproc-cgi  ${LIBBZ2_LIBRARIES})
  #IF (LIBMICROHTTPD_FOUND)
  #  target_link_libraries(cxproc-httpd ${LIBBZ2_LIBRARIES})
  #ENDIF ()
ENDIF (CXPROC_LIBBZ2)

################################################################################
#
# either local liblzma or system liblzma
#

IF (CXPROC_LZMA)
  INCLUDE_DIRECTORIES(${LIBLZMA_INCLUDE_DIR})
  add_definitions(-DHAVE_LIBLZMA)
  IF (${LIBLZMA_LIBRARIES} MATCHES ".+static\\.lib$" OR ${LIBLZMA_LIBRARIES} MATCHES ".+\\.a$")
    add_definitions(-DLIBLZMA_STATIC)
  ENDIF ()
  target_link_libraries(filex ${LIBLZMA_LIBRARIES})
  target_link_libraries(cxproc ${LIBLZMA_LIBRARIES})
  target_link_libraries(cxproc-test ${LIBLZMA_LIBRARIES})
  target_link_libraries(cxproc-cgi  ${LIBLZMA_LIBRARIES})
  #IF (LIBMICROHTTPD_FOUND)
  #  target_link_libraries(cxproc-httpd ${LIBLZMA_LIBRARIES})
  #ENDIF ()
ENDIF (CXPROC_LZMA)
IF (CXPROC_ARCHIVE)
  target_link_libraries(filex ${LibArchive_LIBRARIES})
  target_link_libraries(cxproc ${LibArchive_LIBRARIES})
  target_link_libraries(cxproc-test ${LibArchive_LIBRARIES})
  target_link_libraries(cxproc-cgi ${LibArchive_LIBRARIES})
  #IF (LIBMICROHTTPD_FOUND)
  #  target_link_libraries(cxproc-httpd ${LibArchive_LIBRARIES})
  #ENDIF ()
ENDIF (CXPROC_ARCHIVE)

################################################################################
#
# either local libcurl or system libcurl
#

find_path(LIBCURL_INCLUDE_DIR NAMES curl.h
  HINTS "${CXPROC_PREFIX}/include/curl"
  ) 

IF (LIBCURL_INCLUDE_DIR)
  find_library(LIBCURL_LIBRARIES NAMES libcurl libcurl-d_imp
    HINTS "${CXPROC_PREFIX}/lib"
    )
  IF (LIBCURL_LIBRARIES)
    SET(LIBCURL_FOUND ON)
  ELSE(LIBCURL_LIBRARIES)
    FIND_PACKAGE( LIBCURL )
  ENDIF (LIBCURL_LIBRARIES)
ENDIF (LIBCURL_INCLUDE_DIR)

IF (LIBCURL_FOUND)
  MESSAGE(STATUS "Found optional libcurl: ${LIBCURL_LIBRARIES}")
  OPTION(CXPROC_CURL "Enable support for linking cxproc with libcurl." OFF)
ELSE (LIBCURL_FOUND)
  SET(CXPROC_CURL OFF)
ENDIF (LIBCURL_FOUND)

IF (CXPROC_CURL)
  INCLUDE_DIRECTORIES(${LIBCURL_INCLUDE_DIR})
  add_definitions(-DHAVE_LIBCURL)
  IF (${LIBCURL_LIBRARIES} MATCHES ".+static\\.lib$" OR ${LIBCURL_LIBRARIES} MATCHES ".+\\.a$")
    add_definitions(-DLIBCURL_STATIC)
  ENDIF ()
  target_link_libraries(filex ${LIBCURL_LIBRARIES})
  target_link_libraries(cxproc ${LIBCURL_LIBRARIES})
  target_link_libraries(cxproc-test ${LIBCURL_LIBRARIES})
  target_link_libraries(cxproc-cgi  ${LIBCURL_LIBRARIES})
  #IF (LIBMICROHTTPD_FOUND)
  #  target_link_libraries(cxproc-httpd ${LIBCURL_LIBRARIES})
  #ENDIF ()
  IF(MSVC)
    # no additional libraries with VC++
  ELSE(MSVC)
    target_link_libraries(filex ssl crypto)
    target_link_libraries(cxproc ssl crypto)
    target_link_libraries(cxproc-test ssl crypto)
    target_link_libraries(cxproc-cgi  ssl crypto)
    #IF (LIBMICROHTTPD_FOUND)
    #  target_link_libraries(cxproc-httpd ssl crypto)
    #ENDIF ()
  ENDIF(MSVC)
ENDIF (CXPROC_CURL)

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
################################################################################
#
# either local libid3tag or system libid3tag
#

IF (CXPROC_LIBID3TAG)
  target_link_libraries(filex ${LIBID3TAG_LIBRARIES})
  target_link_libraries(cxproc ${LIBID3TAG_LIBRARIES})
  target_link_libraries(cxproc-test ${LIBID3TAG_LIBRARIES})
  target_link_libraries(cxproc-cgi  ${LIBID3TAG_LIBRARIES})
  #IF (LIBMICROHTTPD_FOUND)
  #  target_link_libraries(cxproc-httpd ${LIBID3TAG_LIBRARIES})
  #ENDIF ()
ENDIF (CXPROC_LIBID3TAG)

################################################################################
#
# either local libexif or system libexif
#

IF (CXPROC_LIBEXIF)
  target_link_libraries(filex ${LIBEXIF_LIBRARY})
  target_link_libraries(cxproc ${LIBEXIF_LIBRARY})
  target_link_libraries(cxproc-test ${LIBEXIF_LIBRARY})
  target_link_libraries(cxproc-cgi  ${LIBEXIF_LIBRARY})
  #IF (LIBMICROHTTPD_FOUND)
  #  target_link_libraries(cxproc-httpd ${LIBEXIF_LIBRARY})
  #ENDIF ()
ENDIF (CXPROC_LIBEXIF)

################################################################################
#
# add list of mandatory libraries
#

target_link_libraries(filex ${LIBICONV_LIBRARY} ${LIBCHARSET_LIBRARY} ${LIBXML2_LIBRARIES})
target_link_libraries(cxproc ${LIBICONV_LIBRARY} ${LIBCHARSET_LIBRARY} ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES})
target_link_libraries(cxproc-test ${LIBICONV_LIBRARY} ${LIBCHARSET_LIBRARY} ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES})
target_link_libraries(cxproc-cgi ${LIBICONV_LIBRARY} ${LIBCHARSET_LIBRARY} ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES})

#IF (LIBMICROHTTPD_FOUND)
#  target_link_libraries(cxproc-httpd ${LIBICONV_LIBRARY} ${LIBCHARSET_LIBRARY} ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES} ${LIBMICROHTTPD_LIBRARIES})
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
#OPTION(CXPROC_SHOW_REPORT     "Show the final configuration report" ON)

################################################################################
#

IF(MSVC)
ELSE(MSVC)
  target_link_libraries(filex -ldl -lm)
  target_link_libraries(cxproc -ldl -lm)
  target_link_libraries(cxproc-test -ldl -lm)
  target_link_libraries(cxproc-cgi -ldl -lm)
  #IF (LIBMICROHTTPD_FOUND)
  #  target_link_libraries(cxproc-httpd -ldl -lm)
  #ENDIF ()
  IF (CXPROC_LIBZ)
    target_link_libraries(filex -lz)
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
## testing ####################################################################
###############################################################################

# external tests
add_test(NAME basics-code
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/bin/cxproc-test -t basics)

add_test(NAME res_node-code
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/bin/cxproc-test -t res_node)

add_test(NAME cxp-code
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/bin/cxproc-test -t cxp)

add_test(NAME dom-code
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/bin/cxproc-test -t dom)

add_test(NAME plain-code
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/bin/cxproc-test -t plain)

IF(CXPROC_PTHREAD)
  add_test(NAME thread-code
    WORKING_DIRECTORY ${CXPROC_PREFIX}
    COMMAND ${CXPROC_PREFIX}/bin/cxproc-test -t thread)

  add_test(NAME thread-cxp
    WORKING_DIRECTORY ${CXPROC_TEST_DIR}/thread
    COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)

ENDIF (CXPROC_PTHREAD)

add_test(NAME parser-code
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/bin/cxproc-test -t parser)

add_test(NAME cgi-empty
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/www/cgi-bin/cxproc-cgi)

set_property(TEST cgi-empty
  APPEND PROPERTY ENVIRONMENT CXP_LOGFILE=cxproc-empty.log;QUERY_STRING=
)

add_test(NAME cgi-mini
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/www/cgi-bin/cxproc-cgi)

set_property(TEST cgi-mini
  APPEND PROPERTY ENVIRONMENT CXP_LOGFILE=cxproc-mini.log;QUERY_STRING=cxp=info
)

add_test(NAME cgi-maxi
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/www/cgi-bin/cxproc-cgi)

set_property(TEST cgi-maxi
  APPEND PROPERTY ENVIRONMENT CXP_LOGFILE=cxproc-maxi.log;QUERY_STRING=path=&cxp=PieUiDir
)

add_test(NAME cgi-archive
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/www/cgi-bin/cxproc-cgi)

set_property(TEST cgi-archive
  APPEND PROPERTY ENVIRONMENT CXP_LOGFILE=cxproc-archive.log;QUERY_STRING=path=Test/Archive/TestArchive.zip/Test/SubTest/SubTest/Length_1024.txt
)

add_test(NAME cgi-sql
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/www/cgi-bin/cxproc-cgi)

set_property(TEST cgi-sql
  APPEND PROPERTY ENVIRONMENT CXP_LOGFILE=cxproc-sql.log;QUERY_STRING=path=Test/Databases/TestContent.db3&cxp=PiejQSqlite
)

set_property(TEST cgi-empty cgi-mini cgi-archive cgi-sql cgi-maxi
  APPEND PROPERTY ENVIRONMENT DOCUMENT_ROOT=${CXPROC_PREFIX}/www/html/;REQUEST_METHOD=GET;CXP_LOG=3;CXP_PATH=${CXPROC_PREFIX}/www/html/pie//;CXP_ROOT=${CXPROC_PREFIX}/www/html/test/
)
  
add_test(NAME cli-help
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/bin/cxproc -?)

add_test(NAME cli-description
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/bin/cxproc -? myclean.cxp)

add_test(NAME cli-env
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/bin/cxproc -e)

add_test(NAME cli-conf-plain
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  COMMAND ${CXPROC_PREFIX}/bin/cxproc -t abc.txt pie2html.xsl)

add_test(NAME cli-search
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/bin/cxproc -s "Validate.xsl")

add_test(NAME cli-conf-dir
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  COMMAND ${CXPROC_PREFIX}/bin/cxproc -t doc//)

add_test(NAME dir-code
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/bin/cxproc-test -t dir)

add_test(NAME dir-cxp
  WORKING_DIRECTORY ${CXPROC_TEST_DIR}/dir
  COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)

add_test(NAME each-cxp
  WORKING_DIRECTORY ${CXPROC_TEST_DIR}/each
  COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)

add_test(NAME eval-cxp
  WORKING_DIRECTORY ${CXPROC_TEST_DIR}/eval
  COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)

add_test(NAME plain-cxp
  WORKING_DIRECTORY ${CXPROC_TEST_DIR}/plain
  COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)

add_test(NAME system-cxp
  WORKING_DIRECTORY ${CXPROC_TEST_DIR}/system
  COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)

add_test(NAME worst-cxp
  WORKING_DIRECTORY ${CXPROC_TEST_DIR}/worst
  COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)

add_test(NAME xhtml-cxp
  WORKING_DIRECTORY ${CXPROC_TEST_DIR}/xhtml
  COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)

add_test(NAME xml-cxp
  WORKING_DIRECTORY ${CXPROC_TEST_DIR}/xml
  COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)

add_test(NAME xsl-cxp
  WORKING_DIRECTORY ${CXPROC_TEST_DIR}/xsl
  COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)

set_property(TEST cli-help cli-description cli-env cli-conf-plain cli-search cli-conf-dir dir-code dir-cxp each-cxp eval-cxp plain-cxp system-cxp worst-cxp xhtml-cxp xml-cxp xsl-cxp
  APPEND PROPERTY ENVIRONMENT CXP_PATH=${PROJECT_SOURCE_DIR}//
)

IF (CXPROC_PETRINET)
  add_test(NAME petrinet-code
    WORKING_DIRECTORY ${CXPROC_PREFIX}
    COMMAND ${CXPROC_PREFIX}/bin/cxproc-test -t petrinet)

  add_test(NAME petrinet-cxp
    WORKING_DIRECTORY ${CXPROC_TEST_DIR}/option/petrinet
    COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)

  set_tests_properties(petrinet-cxp PROPERTIES
    ENVIRONMENT "CXP_PATH=${PROJECT_SOURCE_DIR}//")
ENDIF (CXPROC_PETRINET)

IF (CXPROC_PIE)
  add_test(NAME pie-code
    WORKING_DIRECTORY ${CXPROC_PREFIX}
    COMMAND ${CXPROC_PREFIX}/bin/cxproc-test -t pie)

  add_test(NAME pie-cxp-import-circular
    WORKING_DIRECTORY ${CXPROC_TEST_DIR}/option/pie/text/circular
    COMMAND ${CXPROC_PREFIX}/bin/cxproc config-circular.cxp)

  add_test(NAME pie-cxp-import-csv
    WORKING_DIRECTORY ${CXPROC_TEST_DIR}/option/pie/text/csv
    COMMAND ${CXPROC_PREFIX}/bin/cxproc config-pie-import-csv.cxp)

  add_test(NAME pie-cxp-import-cxp
    WORKING_DIRECTORY ${CXPROC_TEST_DIR}/option/pie/text/cxp
    COMMAND ${CXPROC_PREFIX}/bin/cxproc config-pie-import-cxp.cxp)

  add_test(NAME pie-cxp-import-line
    WORKING_DIRECTORY ${CXPROC_TEST_DIR}/option/pie/text/line
    COMMAND ${CXPROC_PREFIX}/bin/cxproc config-pie-import-line.cxp)

  add_test(NAME pie-cxp-import-log
    WORKING_DIRECTORY ${CXPROC_TEST_DIR}/option/pie/text/log
    COMMAND ${CXPROC_PREFIX}/bin/cxproc config-pie-import-log.cxp)

  add_test(NAME pie-cxp-import-plain
    WORKING_DIRECTORY ${CXPROC_TEST_DIR}/option/pie/text/plain
    COMMAND ${CXPROC_PREFIX}/bin/cxproc config-pie-import-plain.cxp)

  add_test(NAME pie-cxp-import-script
    WORKING_DIRECTORY ${CXPROC_TEST_DIR}/option/pie/text/script
    COMMAND ${CXPROC_PREFIX}/bin/cxproc config-pie-import-script.cxp)

  add_test(NAME pie-cxp-import-xml
    WORKING_DIRECTORY ${CXPROC_TEST_DIR}/option/pie/text/xml
    COMMAND ${CXPROC_PREFIX}/bin/cxproc config-pie-import-xml.cxp)

  add_test(NAME calendar
    WORKING_DIRECTORY ${CXPROC_TEST_DIR}/option/pie/calendar
    COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)

  add_test(NAME cli-conf-year
    WORKING_DIRECTORY ${CXPROC_PREFIX}
    COMMAND ${CXPROC_PREFIX}/bin/cxproc -t 2012)

  add_test(NAME cli-year
    WORKING_DIRECTORY ${CXPROC_PREFIX}
    COMMAND ${CXPROC_PREFIX}/bin/cxproc 2012)

  set_property(TEST pie-code pie-cxp-import-cxp calendar cli-conf-year cli-year
    APPEND PROPERTY ENVIRONMENT CXP_PATH=${PROJECT_SOURCE_DIR}//
    )
  
  add_test(NAME ics-code
    WORKING_DIRECTORY ${CXPROC_PREFIX}
    COMMAND ${CXPROC_PREFIX}/bin/cxproc-test -t ics)

  add_test(NAME vcf-code
    WORKING_DIRECTORY ${CXPROC_PREFIX}
    COMMAND ${CXPROC_PREFIX}/bin/cxproc-test -t vcf)

ENDIF (CXPROC_PIE)

IF (CXPROC_ARCHIVE)
add_test(NAME archive-code
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/bin/cxproc-test -t archive)

add_test(NAME archive-cxp
  WORKING_DIRECTORY ${CXPROC_TEST_DIR}/option/archive
  COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)

set_tests_properties(archive-code archive-cxp PROPERTIES
  ENVIRONMENT "CXP_PATH=${PROJECT_SOURCE_DIR}//"
  )
ENDIF (CXPROC_ARCHIVE)

IF (CXPROC_LIBEXIF)
  add_test(NAME image-cxp
    WORKING_DIRECTORY ${CXPROC_TEST_DIR}/option/image
    COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)
ENDIF ()

IF (CXPROC_DUKTAPE)
  add_test(NAME script-cxp
    WORKING_DIRECTORY ${CXPROC_TEST_DIR}/option/script
    COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)
ENDIF ()

IF (CXPROC_JSON)
  add_test(NAME json-cxp
    WORKING_DIRECTORY ${CXPROC_TEST_DIR}/option/json
    COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)
ENDIF ()

IF (CXPROC_RP)
  add_test(NAME rp-cxp
    WORKING_DIRECTORY ${CXPROC_TEST_DIR}/option/rp
    COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)
ENDIF ()

IF (CXPROC_SQLITE3)
  add_test(NAME sql-code  
    COMMAND ${CXPROC_PREFIX}/bin/cxproc-test -t sql)

  add_test(NAME sql-cxp
    WORKING_DIRECTORY ${CXPROC_TEST_DIR}/option/sql
    COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)

  set_tests_properties(sql-code sql-cxp
    PROPERTIES ENVIRONMENT "CXP_PATH=${PROJECT_SOURCE_DIR}//"
  )
ENDIF ()

IF(MSVC)
  # no memcheck available yet
ELSE(MSVC)
  # http://stackoverflow.com/questions/40325957/how-do-i-add-valgrind-tests-to-my-cmake-test-target
  add_custom_target(test_memcheck
    COMMAND ${CMAKE_CTEST_COMMAND} --force-new-ctest-process --test-action memcheck
    )
ENDIF(MSVC)

# set envionment variables for testing
#set_tests_properties(basics-code res_node-code cxp-code dom-code plain-code parser-code cli-help cli-description cli-env cli-conf-plain cli-search cli-conf-file cli-conf-dir cli-dir-xsl dir-code dir-cxp each-cxp eval-cxp plain-cxp system-cxp worst-cxp xhtml-cxp xml-cxp xsl-cxp PROPERTIES
#  ENVIRONMENT "CXP_PATH=${PROJECT_SOURCE_DIR}//"
#  )

add_test(NAME filex-empty
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/bin/filex)

add_test(NAME filex-dir
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/bin/filex ${CXPROC_TEST_DIR})

add_test(NAME filex-dir-recursive
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/bin/filex ${CXPROC_TEST_DIR}// ${CXPROC_TEMP_DIR}//)

add_test(NAME filex-archive
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/bin/filex ${CXPROC_TEST_DIR}/xml/test-xml-zip.odt)

IF (CXPROC_ARCHIVE)
  add_test(NAME filex-archive-intern
    WORKING_DIRECTORY ${CXPROC_PREFIX}
    COMMAND ${CXPROC_PREFIX}/bin/filex ${CXPROC_TEST_DIR}/xml/test-xml-zip.odt/content.xml)

  add_test(NAME filex-archive-intern-intern
    WORKING_DIRECTORY ${CXPROC_PREFIX}
    COMMAND ${CXPROC_PREFIX}/bin/filex ${CXPROC_TEST_DIR}/option/archive/test-zip-odt.zip/test-xml-zip.odt/content.xml)
ENDIF(CXPROC_ARCHIVE)

IF (CXPROC_CURL)
  add_test(NAME filex-url-http
    WORKING_DIRECTORY ${CXPROC_PREFIX}
    COMMAND ${CXPROC_PREFIX}/bin/filex http://127.0.0.1/)

  IF (CXPROC_SQLITE3)
    add_test(NAME filex-url-http-sql
      WORKING_DIRECTORY ${CXPROC_PREFIX}
      COMMAND ${CXPROC_PREFIX}/bin/filex http://127.0.0.1/TestContent.db3)
  ENDIF()
ENDIF()

IF (CXPROC_SQLITE3)
  add_test(NAME filex-sql
    WORKING_DIRECTORY ${CXPROC_PREFIX}
    COMMAND ${CXPROC_PREFIX}/bin/filex ${CXPROC_TEST_DIR}/option/sql/test.db3)
ENDIF()

#filex-empty filex-dir filex-dir-recursive filex-archive filex-archive-intern filex-archive-intern-intern filex-url-http filex-sql filex-url-http-sql 

# ctest -N | perl -ne 'if (/^ +Test/) {chomp(); $_ =~ s/^[^\:]+\: +/ /; print;}'

###############################################################################
## cleaning ####################################################################
###############################################################################

###############################################################################
## packaging ##################################################################
###############################################################################

# TODO: configure package

set(CPACK_BINARY_NSIS "OFF")
set(CPACK_BINARY_NUGET "OFF")
set(CPACK_BINARY_WIX "OFF")
set(CPACK_BINARY_ZIP "ON")

INSTALL(TARGETS filex cxproc RUNTIME DESTINATION bin)
INSTALL(TARGETS cxproc-cgi test-cgi RUNTIME DESTINATION www/cgi-bin)
INSTALL(DIRECTORY xml/schema DESTINATION .)
INSTALL(DIRECTORY doc DESTINATION .)
INSTALL(DIRECTORY examples DESTINATION .)
INSTALL(DIRECTORY "${PROJECT_SOURCE_DIR}/www/html" DESTINATION www)
INSTALL(FILES LICENSE README.md DESTINATION .)
INSTALL(FILES "${PROJECT_SOURCE_DIR}/www/conf/httpd/cxproc-template.conf" DESTINATION www/conf)

set(CPACK_PACKAGE_VENDOR "Vendor name")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Some summary")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README  "${PROJECT_SOURCE_DIR}/README.md")

set(CPACK_SOURCE_GENERATOR "ZIP")

set(CPACK_SOURCE_IGNORE_FILES
    /.git
    /dist
    /.*build.*
    /\\\\.DS_Store
)

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "raxdne")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libxml2" "libxslt")

include(CPack)

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
  MESSAGE(STATUS "  Compile cxproc code for legacy  ... : ${CXPROC_LEGACY}")
  MESSAGE(STATUS "  Compile cxproc experimental code .. : ${CXPROC_EXPERIMENTAL}")
  MESSAGE(STATUS "  Compile cxproc MARKDOWN code ...... : ${CXPROC_MARKDOWN}")
  MESSAGE(STATUS "  Compile cxproc AJAX code .......... : ${CXPROC_AJAX}")
#  MESSAGE(STATUS "  Compile cxproc with profile ....... : ${CXPROC_PROFILE}")
  
if (CXPROC_DOC)
  MESSAGE(STATUS "  Build documentation........ ....... : ${CXPROC_DOC}")
ENDIF (CXPROC_DOC)
  
IF(CXPROC_PTHREAD)
  MESSAGE(STATUS "  Link cxproc with pthread .......... : ${CXPROC_PTHREAD}")
ELSEIF (CXPROC_THREAD)
  MESSAGE(STATUS "  Compile cxproc with native threads  : ${CXPROC_THREAD}")
ENDIF (CXPROC_PTHREAD)

  MESSAGE(STATUS "  Compile cxproc with PIE ........... : ${CXPROC_PIE}")
#  MESSAGE(STATUS "  Compile cxproc with DIR_INDEX ..... : ${CXPROC_DIR_INDEX}")
  MESSAGE(STATUS "  Compile cxproc with JSON  ......... : ${CXPROC_JSON}")
  MESSAGE(STATUS "")
  
  IF (DUKTAPE_FOUND)
    MESSAGE(STATUS "  Compile cxproc with JavaScript .... : ${CXPROC_DUKTAPE}")
  ENDIF ()
  
  IF (PCRE2_FOUND)
    MESSAGE(STATUS "  Link cxproc with libpcre2 ......... : ${CXPROC_PCRE2}")
  ENDIF ()

  IF (SQLITE3_FOUND)
    MESSAGE(STATUS "  Link cxproc with libsqlite ........ : ${CXPROC_SQLITE3}")
  ENDIF ()
  
  IF (LIBMAGICK_FOUND)
    MESSAGE(STATUS "  Link cxproc with ImageMagick ...... : ${CXPROC_LIBMAGICK}")
  ENDIF ()

  IF (LIBEXIF_FOUND)
    MESSAGE(STATUS "  Link cxproc with libexif .......... : ${CXPROC_LIBEXIF}")
  ENDIF ()

  IF (LIBVORBIS_FOUND)
    MESSAGE(STATUS "  Link cxproc with libvorbis ........ : ${CXPROC_LIBVORBIS}")
  ENDIF ()

  IF (LIBID3TAG_FOUND)
    MESSAGE(STATUS "  Link cxproc with libid3tag ........ : ${CXPROC_LIBID3TAG}")
  ENDIF ()

  IF (LibArchive_FOUND)
    MESSAGE(STATUS "  Link cxproc with libarchive ....... : ${CXPROC_ARCHIVE}")
  ENDIF ()

  IF (LIBZ_FOUND)
    MESSAGE(STATUS "  Link cxproc with libz ............. : ${CXPROC_LIBZ}")
  ENDIF ()

  IF (LZMA_FOUND)
    MESSAGE(STATUS "  Link cxproc with liblzma .......... : ${CXPROC_LZMA}")
  ENDIF ()

  IF (LIBBZ2_FOUND)
    MESSAGE(STATUS "  Link cxproc with libbz2 ........... : ${CXPROC_LIBBZ2}")
  ENDIF ()

  IF (CURL_FOUND)
    MESSAGE(STATUS "  Link cxproc with libcurl .......... : ${CXPROC_CURL}")
  ENDIF ()

  MESSAGE(STATUS "")
  
  #IF (LIBMICROHTTPD_FOUND)
  #  MESSAGE(STATUS "  Build cxproc-httpd (libmicrohttpd). : ${LIBMICROHTTPD_FOUND}")
  #ENDIF ()

  IF(MINGW AND NOT CXPROC_STATIC)
    MESSAGE(STATUS "  Non-standard dll names (prefix) . : ${NON_STANDARD_LIB_PREFIX}")
    MESSAGE(STATUS "  Non-standard dll names (suffix) . : ${NON_STANDARD_LIB_SUFFIX}")
  ENDIF (MINGW AND NOT CXPROC_STATIC)
  MESSAGE(STATUS "")
#ENDIF (CXPROC_SHOW_REPORT)

