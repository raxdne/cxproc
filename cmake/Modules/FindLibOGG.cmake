################################################################################
#
# libogg
#

find_path(LIBOGG_INCLUDE_DIR NAMES ogg/ogg.h) 

IF (LIBOGG_INCLUDE_DIR)
  FIND_LIBRARY(LIBOGG_LIBRARY NAMES libogg.so libogg.a ogg libogg.lib)
  IF (LIBOGG_LIBRARY)
    MESSAGE(STATUS "Found libogg: ${LIBOGG_LIBRARY}")
  ELSE ()
    MESSAGE(STATUS "Not found: libogg")
  ENDIF ()
ELSE ()
  MESSAGE(STATUS "Not found: headers of libogg")
ENDIF (LIBOGG_INCLUDE_DIR)

