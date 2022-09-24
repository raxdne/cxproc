################################################################################
#
# either local libcmark or system libcmark
#

find_path(LIBCMARK_INCLUDE_DIR NAMES cmark.h HINTS ${HINT_DIR_INCLUDE} "${HINT_DIR_INCLUDE}/cmark/") 

IF (LIBCMARK_INCLUDE_DIR)
  FIND_LIBRARY(LIBCMARK_LIBRARY NAMES libcmark.so libcmark.a libcmark cmark.lib HINTS ${HINT_DIR_LIB})
  IF (LIBCMARK_LIBRARY)
    get_filename_component(LIBCMARK_LIB_DIR ${LIBCMARK_LIBRARY} DIRECTORY)
    INCLUDE_DIRECTORIES(${LIBCMARK_INCLUDE_DIR} ${LIBCMARK_LIB_DIR})
    MESSAGE(STATUS "Found libcmark: ${LIBCMARK_LIBRARY}")
  ELSE ()
    MESSAGE(STATUS "Not found: libcmark")
  ENDIF ()
ELSE ()
  MESSAGE(STATUS "Not found: headers of libcmark")
ENDIF (LIBCMARK_INCLUDE_DIR)

