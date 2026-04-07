
################################################################################
#
# local duktape
#

IF (HINT_DIR_INCLUDE)
  find_path(DUKTAPE_INCLUDE_DIR NAME "duktape.h" HINTS ${HINT_DIR_INCLUDE} NO_DEFAULT_PATH)
  find_library(DUKTAPE_LIBRARY_DEBUG NAMES "duktape" HINTS ${HINT_DIR_LIBD} NO_DEFAULT_PATH)
  find_library(DUKTAPE_LIBRARY       NAMES "duktape" HINTS ${HINT_DIR_LIB}  NO_DEFAULT_PATH)
ENDIF ()

find_path(DUKTAPE_INCLUDE_DIR NAMES "duktape.h")
find_library(DUKTAPE_LIBRARY NAMES "duktape")

IF (DUKTAPE_INCLUDE_DIR AND DUKTAPE_LIBRARY)
  SET(DUKTAPE_FOUND TRUE)
  MESSAGE(STATUS "Found libduktape: ${DUKTAPE_LIBRARY}")
ELSE()
  SET(DUKTAPE_FOUND FALSE)
  MESSAGE(ERROR " Not found: libduktape")
ENDIF ()

