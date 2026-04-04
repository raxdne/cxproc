################################################################################
#
# libiconv
#

find_path(LIBICONV_INCLUDE_DIR NAME iconv.h HINTS ${HINT_DIR_INCLUDE} NO_DEFAULT_PATH)

IF (LIBICONV_INCLUDE_DIR)
  IF (MSVC)
    find_library(LIBICONV_LIBRARY NAMES "iconv" HINTS ${HINT_DIR_LIB} REQUIRED)
    find_library(LIBCHARSET_LIBRARY NAMES "charset" HINTS ${HINT_DIR_LIB} REQUIRED)
    IF (LIBICONV_LIBRARY AND LIBCHARSET_LIBRARY)
      MESSAGE(STATUS "Found required libiconv: ${LIBICONV_LIBRARY} ${LIBCHARSET_LIBRARY}")
    ELSE ()
      # select interactively
    ENDIF ()
  ELSE ()
    # no library file required on Linux
    add_compile_definitions(ICONV_CODE_UNIT_WIDTH=8)
  ENDIF ()
ELSE ()
  FIND_PACKAGE( Iconv )  
ENDIF (LIBICONV_INCLUDE_DIR)

