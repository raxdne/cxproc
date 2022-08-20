################################################################################
#
# either local libexif or system libexif
#

find_path(LIBEXIF_INCLUDE_DIR NAMES libexif/exif-data.h) 

IF (LIBEXIF_INCLUDE_DIR)
  FIND_LIBRARY(LIBEXIF_LIBRARY NAMES libexif.so libexif.a libexif)
  IF (LIBEXIF_LIBRARY)
    MESSAGE(STATUS "Found libexif: ${LIBEXIF_LIBRARY}")
  ELSE ()
    MESSAGE(STATUS "Not found: libexif")
  ENDIF ()
ELSE ()
  MESSAGE(STATUS "Not found: headers of libexif")
ENDIF (LIBEXIF_INCLUDE_DIR)

