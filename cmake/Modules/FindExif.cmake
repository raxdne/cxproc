################################################################################
#
# either local libexif or system libexif
#

IF (HINT_DIR_INCLUDE)
  find_path(LIBEXIF_INCLUDE_DIR NAME "exif-data.h" HINTS ${HINT_DIR_INCLUDE} PATH_SUFFIXES "libexif" NO_DEFAULT_PATH)
  find_library(LIBEXIF_LIBRARY_DEBUG NAMES "exif" HINTS ${HINT_DIR_LIBD} NO_DEFAULT_PATH)
  find_library(LIBEXIF_LIBRARY       NAMES "exif" HINTS ${HINT_DIR_LIB}  NO_DEFAULT_PATH)
ENDIF ()

find_path(LIBEXIF_INCLUDE_DIR NAMES "exif-data.h" PATH_SUFFIXES "libexif")
find_library(LIBEXIF_LIBRARY NAMES "exif")

IF (LIBEXIF_INCLUDE_DIR AND LIBEXIF_LIBRARY)
  MESSAGE(STATUS "Found libexif: ${LIBEXIF_LIBRARY}")
ELSE()
  MESSAGE(ERROR " Not found: libexif")
ENDIF ()

