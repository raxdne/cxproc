################################################################################
#
# either local libzip or system libzip
#

find_path(LIBZIP_INCLUDE_DIR NAMES "zip.h" HINTS ${HINT_DIR_INCLUDE} NO_DEFAULT_PATH)

IF (LIBZIP_INCLUDE_DIR)
  find_library(LIBZIP_LIBRARY_DEBUG NAMES "zip" HINTS ${HINT_DIR_LIBD} NO_DEFAULT_PATH)
  find_library(LIBZIP_LIBRARY       NAMES "zip" HINTS ${HINT_DIR_LIB}  NO_DEFAULT_PATH)
ELSE()
  # system
  find_path(LIBZIP_INCLUDE_DIR NAMES "zip.h")
  find_library(LIBZIP_LIBRARY NAMES "zip")
ENDIF ()

IF (LIBZIP_LIBRARY)
  MESSAGE(STATUS "Found libzip: ${LIBZIP_LIBRARY}")
ELSE()
  MESSAGE(ERROR " Not found: libzip")
ENDIF ()
