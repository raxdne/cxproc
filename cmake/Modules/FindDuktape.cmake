
################################################################################
#
# local duktape
#

FIND_PATH(DUKTAPE_INCLUDE_DIR NAME duktape.h HINTS "${CXPROC_PREFIX}/include")

IF (DUKTAPE_INCLUDE_DIR)
  IF (EXISTS "${DUKTAPE_INCLUDE_DIR}/duktape.c")
    SET(DUKTAPE_SOURCE_FILE "${DUKTAPE_INCLUDE_DIR}/duktape.c")
    MESSAGE(STATUS "Found duktape: ${DUKTAPE_SOURCE_FILE}")
  ELSE()
    FIND_LIBRARY(DUKTAPE_LIBRARY NAMES duktape)
    IF (DUKTAPE_LIBRARY)
      MESSAGE(STATUS "Found duktape: ${DUKTAPE_LIBRARY}")
    ENDIF()
  ENDIF()
ENDIF (DUKTAPE_INCLUDE_DIR)
