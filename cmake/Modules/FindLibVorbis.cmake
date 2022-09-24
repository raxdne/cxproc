################################################################################
#
# libvorbis
#

find_path(LIBVORBIS_INCLUDE_DIR NAMES vorbis/codec.h HINTS ${HINT_DIR_INCLUDE}) 

IF (LIBVORBIS_INCLUDE_DIR)
  FIND_LIBRARY(LIBVORBIS_LIBRARY NAMES libvorbis.so libvorbis.a vorbis libvorbis.lib HINTS ${HINT_DIR_LIB})
  IF (LIBVORBIS_LIBRARY)
    MESSAGE(STATUS "Found libvorbis: ${LIBVORBIS_LIBRARY}")
  ELSE ()
    MESSAGE(STATUS "Not found: libvorbis")
  ENDIF ()
ELSE ()
  MESSAGE(STATUS "Not found: headers of libvorbis")
ENDIF (LIBVORBIS_INCLUDE_DIR)

