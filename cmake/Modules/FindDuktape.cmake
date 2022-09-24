
################################################################################
#
# local duktape
#

FIND_PATH(DUKTAPE_INCLUDE_DIR NAME duktape.h HINTS ${HINT_DIR_INCLUDE})

IF (DUKTAPE_INCLUDE_DIR)
  IF (EXISTS "${DUKTAPE_INCLUDE_DIR}/duktape.c")
    SET(DUKTAPE_SOURCE_FILE "${DUKTAPE_INCLUDE_DIR}/duktape.c")
    MESSAGE(STATUS "Found duktape: ${DUKTAPE_SOURCE_FILE}")
  ELSE()
    FIND_LIBRARY(DUKTAPE_LIBRARY NAMES duktape HINTS ${HINT_DIR_LIB})
    IF (DUKTAPE_LIBRARY)
      MESSAGE(STATUS "Found duktape: ${DUKTAPE_LIBRARY}")
    ENDIF()
  ENDIF()
ENDIF (DUKTAPE_INCLUDE_DIR)

