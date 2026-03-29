################################################################################
#
# either local libzip or system libzip
#

find_path(LIBZIP_INCLUDE_DIR NAMES "zip.h" HINTS ${HINT_DIR_INCLUDE})

IF (LIBZIP_INCLUDE_DIR)
  FIND_LIBRARY(LIBZIP_LIBRARY NAMES "zip" "libzip" HINTS ${HINT_DIR_LIB})
  IF (LIBZIP_LIBRARY)
    MESSAGE(STATUS " Found libzip: ${LIBZIP_LIBRARY}")
  ELSE ()
    MESSAGE(ERROR " libzip must be available")
  ENDIF ()
ELSE ()
  MESSAGE(ERROR " Not found: headers of libzip")
ENDIF (LIBZIP_INCLUDE_DIR)

