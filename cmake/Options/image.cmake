
################################################################################
#
# libexif
#

FIND_PACKAGE( Exif )

IF (LIBEXIF_LIBRARY)
  OPTION (CXPROC_LIBEXIF "Enable support for linking cxproc with libexif." OFF)
ENDIF (LIBEXIF_LIBRARY)


IF (CXPROC_LIBEXIF)

  SET(EXIF_FILES
    ${CXPROC_SRC_DIR}/option/image/image_exif.h
    ${CXPROC_SRC_DIR}/option/image/image_exif.c
    )

  target_sources(filex PUBLIC ${EXIF_FILES})

  target_sources(cxproc PUBLIC ${EXIF_FILES})

  target_sources(cxproc-cgi PUBLIC ${EXIF_FILES})

  target_sources(cxproc-test PUBLIC ${EXIF_FILES})

  INCLUDE_DIRECTORIES(${LIBEXIF_INCLUDE_DIR})

  target_compile_definitions(filex       PUBLIC HAVE_LIBEXIF)
  target_compile_definitions(cxproc      PUBLIC HAVE_LIBEXIF)
  target_compile_definitions(cxproc-cgi  PUBLIC HAVE_LIBEXIF)
  target_compile_definitions(cxproc-test PUBLIC HAVE_LIBEXIF)

  target_link_libraries(filex ${LIBEXIF_LIBRARY})
  target_link_libraries(cxproc ${LIBEXIF_LIBRARY})
  target_link_libraries(cxproc-test ${LIBEXIF_LIBRARY})
  target_link_libraries(cxproc-cgi  ${LIBEXIF_LIBRARY})
  #IF (LIBMICROHTTPD_FOUND)
  #  target_link_libraries(cxproc-httpd ${LIBEXIF_LIBRARY})
  #ENDIF ()

  IF(CXPROC_TESTS)
  
add_test(NAME image-cxp
    WORKING_DIRECTORY ${CXPROC_TEST_DIR}/option/image
    COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)
ENDIF(CXPROC_TESTS)
  
ENDIF ()

################################################################################
#
# libmagick
#

#FIND_PACKAGE( ImageMagick )

#OPTION(CXPROC_LIBMAGICK "Include ImageMagick code" OFF)
SET(CXPROC_LIBMAGICK OFF)

IF (CXPROC_LIBMAGICK)
  SET(IMAGE_FILES
    ${CXPROC_SRC_DIR}/option/image/image.h
    ${CXPROC_SRC_DIR}/option/image/image.c
    )

  target_sources(filex PUBLIC ${IMAGE_FILES})

  target_sources(cxproc PUBLIC ${IMAGE_FILES})

  target_sources(cxproc-cgi PUBLIC ${IMAGE_FILES})

  target_sources(cxproc-test PUBLIC ${IMAGE_FILES})

  INCLUDE_DIRECTORIES(${LIBMAGICK_INCLUDE_DIR})
  
  target_compile_definitions(filex       PUBLIC HAVE_MAGICK)
  target_compile_definitions(cxproc      PUBLIC HAVE_MAGICK)
  target_compile_definitions(cxproc-cgi  PUBLIC HAVE_MAGICK)
  target_compile_definitions(cxproc-test PUBLIC HAVE_MAGICK)
ENDIF (CXPROC_LIBMAGICK)

