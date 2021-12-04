################################################################################
#
# libid3tag
#

find_path(LIBID3TAG_INCLUDE_DIR NAMES id3tag.h) 

IF (LIBID3TAG_INCLUDE_DIR)
  FIND_LIBRARY(LIBID3TAG_LIBRARY NAMES libid3tag.so libid3tag.a id3tag libid3tag.lib)
  IF (LIBID3TAG_LIBRARY)
    MESSAGE(STATUS "Found libid3tag: ${LIBID3TAG_LIBRARY}")
  ELSE ()
    MESSAGE(STATUS "Not found: libid3tag")
  ENDIF ()
ELSE ()
  MESSAGE(STATUS "Not found: headers of libid3tag")
ENDIF (LIBID3TAG_INCLUDE_DIR)

