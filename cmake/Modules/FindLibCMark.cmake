################################################################################
#
# either local libcmark or system libcmark
#

# cmark source dir, required for internal header files
FIND_PATH(LIBCMARK_SRC_DIR NAMES node.h HINTS "${PROJECT_SOURCE_DIR}/../cmark/src")
FIND_PATH(LIBCMARK_BUILD_DIR NAMES config.h HINTS "${LIBCMARK_SRC_DIR}/../build/src")

FIND_PATH(LIBCMARK_INCLUDE_DIR NAMES cmark.h HINTS ${HINT_DIR_INCLUDE})

IF (LIBCMARK_INCLUDE_DIR)
  IF(MSVC)
    FIND_LIBRARY(LIBCMARK_LIBRARY NAMES libcmark cmark.lib HINTS ${HINT_DIR_LIB})
  ELSE ()
    FIND_LIBRARY(LIBCMARK_LIBRARY NAMES libcmark.a HINTS ${HINT_DIR_LIB})
  ENDIF(MSVC)
  IF (LIBCMARK_LIBRARY)
    get_filename_component(LIBCMARK_LIB_DIR ${LIBCMARK_LIBRARY} DIRECTORY)
    INCLUDE_DIRECTORIES(${LIBCMARK_INCLUDE_DIR} ${LIBCMARK_SRC_DIR})
    MESSAGE(STATUS "Found libcmark: ${LIBCMARK_LIBRARY}")
  ELSE ()
    MESSAGE(STATUS "Not found: libcmark")
  ENDIF ()
ELSE ()
  MESSAGE(STATUS "Not found: headers of libcmark")
ENDIF (LIBCMARK_INCLUDE_DIR)

