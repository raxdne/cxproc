################################################################################
#
# local pcre2 (impact on PIE)
#

FIND_PATH(PCRE2_INCLUDE_DIR NAMES pcre2.h HINTS ${HINT_DIR_INCLUDE})

IF (PCRE2_INCLUDE_DIR)
  FIND_LIBRARY(PCRE2_LIBRARY NAMES pcre2-8 pcre2-8d libpcre2-8.a HINTS ${HINT_DIR_LIB})
  MESSAGE(STATUS "Found libpcre2: ${PCRE2_LIBRARY}")
ENDIF ()

