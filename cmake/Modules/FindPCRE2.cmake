################################################################################
#
# local pcre2 (impact on PIE)
#

IF (HINT_DIR_INCLUDE)
  find_path(PCRE2_INCLUDE_DIR NAMES "pcre2.h" HINTS ${HINT_DIR_INCLUDE} NO_CMAKE_FIND_ROOT_PATH)
  find_library(PCRE2_LIBRARY NAMES "pcre2-8" HINTS ${HINT_DIR_LIB} NO_CMAKE_FIND_ROOT_PATH)
ENDIF ()

find_path(PCRE2_INCLUDE_DIR NAMES "pcre2.h")
find_library(PCRE2_LIBRARY NAMES "pcre2-8")

IF (PCRE2_INCLUDE_DIR AND PCRE2_LIBRARY)
  MESSAGE(STATUS "Found libpcre2: ${PCRE2_LIBRARY}")
  IF (${PCRE2_LIBRARY} MATCHES ".+static.*\\.lib$" OR ${PCRE2_LIBRARY} MATCHES ".+\\.a$")
    #ADD_LIBRARY(pcre2-8 STATIC IMPORTED)
    add_compile_definitions(PCRE2_STATIC)
  ENDIF ()
ENDIF ()
