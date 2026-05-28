################################################################################
#
# either local libzip or system libzip
#

IF (HINT_DIR_INCLUDE)
  find_path(LIBZIP_INCLUDE_DIR NAMES "zip.h" HINTS ${HINT_DIR_INCLUDE} NO_DEFAULT_PATH)
  find_library(LIBZIP_LIBRARY_DEBUG NAMES "libzip" "zip" HINTS ${HINT_DIR_LIBD} NO_DEFAULT_PATH)
  find_library(LIBZIP_LIBRARY       NAMES "libzip" "zip" HINTS ${HINT_DIR_LIB}  NO_DEFAULT_PATH)
ENDIF ()

find_path(LIBZIP_INCLUDE_DIR NAMES "zip.h")
find_library(LIBZIP_LIBRARY  NAMES "libzip" "zip")

IF (LIBZIP_INCLUDE_DIR AND LIBZIP_LIBRARY)
  MESSAGE(STATUS "Found libzip: ${LIBZIP_LIBRARY}")
  SET(LIBZIP_FOUND)
  add_compile_definitions(HAVE_LIBZIP)
ELSE()
  MESSAGE(ERROR " Not found: libzip")
ENDIF ()
