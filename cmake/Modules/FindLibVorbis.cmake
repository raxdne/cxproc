################################################################################
#
# libvorbis
#

find_path(LIBVORBIS_INCLUDE_DIR NAMES vorbis/codec.h) 

IF (LIBVORBIS_INCLUDE_DIR)
  FIND_LIBRARY(LIBVORBIS_LIBRARY NAMES libvorbis.so libvorbis.a vorbis libvorbis.lib)
  IF (LIBVORBIS_LIBRARY)
    MESSAGE(STATUS "Found libvorbis: ${LIBVORBIS_LIBRARY}")
  ELSE ()
    MESSAGE(STATUS "Not found: libvorbis")
  ENDIF ()
ELSE ()
  MESSAGE(STATUS "Not found: headers of libvorbis")
ENDIF (LIBVORBIS_INCLUDE_DIR)

