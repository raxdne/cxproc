################################################################################
#
# libcurl
#

IF (HINT_DIR_INCLUDE)
  find_path(CURL_INCLUDE_DIRS NAMES "curl.h" HINTS ${HINT_DIR_INCLUDE} PATH_SUFFIXES "curl" NO_DEFAULT_PATH) 
  find_library(CURL_LIBRARIES "curl" "libcurl" HINTS ${HINT_DIR_LIB} NO_DEFAULT_PATH)
  #find_library(CURL_LIBRARIES "curl-d" HINTS ${HINT_DIR_LIBD} NO_DEFAULT_PATH)
  #MESSAGE(FATAL_ERROR " ${CURL_INCLUDE_DIRS} ${CURL_LIBRARIES} ")
ENDIF ()

IF (CURL_INCLUDE_DIRS AND CURL_LIBRARIES)
  # CURL_FOUND
  MESSAGE(STATUS "Found libcurl: ${CURL_LIBRARIES}")
ELSE()
  FIND_PACKAGE( CURL )
ENDIF ()

IF (CURL_INCLUDE_DIRS AND CURL_LIBRARIES)
  INCLUDE_DIRECTORIES(${CURL_INCLUDE_DIRS})
  add_compile_definitions(HAVE_CURL)
  # IF (${CURL_LIBRARY_RELEASE} MATCHES ".+static.*\\.lib$" OR ${CURL_LIBRARY_RELEASE} MATCHES ".+\\.a$")
  #   add_compile_definitions(CURL_STATIC)
  # ENDIF ()
ENDIF ()
