
################################################################################
#
# libid3tag
#
# code of libid3tag is not C++ compilable and memory management should be separated
#

FIND_PACKAGE( Id3Tag )

IF (LIBID3TAG_LIBRARY)
  OPTION (CXPROC_LIBID3TAG "Enable support for linking cxproc with libid3tag." OFF)
ENDIF (LIBID3TAG_LIBRARY)

################################################################################
#
# libogg
#

FIND_PACKAGE( LibOGG )

IF (LIBOGG_LIBRARY)
  OPTION (CXPROC_LIBOGG "Enable support for linking cxproc with libogg." OFF)
ENDIF (LIBOGG_LIBRARY)

################################################################################
#
# libvorbis
#

FIND_PACKAGE( LibVorbis )

IF (LIBVORBIS_LIBRARY)
  OPTION (CXPROC_LIBVORBIS "Enable support for linking cxproc with libvorbis." OFF)
ENDIF (LIBVORBIS_LIBRARY)


################################################################################
#
# libvorbis
#

IF (CXPROC_LIBID3TAG)
  INCLUDE_DIRECTORIES(${LIBID3TAG_INCLUDE_DIR})
ENDIF (CXPROC_LIBID3TAG)

IF (CXPROC_LIBOGG)
  INCLUDE_DIRECTORIES(${LIBOGG_INCLUDE_DIR})
ENDIF (CXPROC_LIBOGG)

IF (CXPROC_LIBVORBIS)
  INCLUDE_DIRECTORIES(${LIBVORBIS_INCLUDE_DIR})
ENDIF (CXPROC_LIBVORBIS)

IF (CXPROC_LIBVORBIS OR CXPROC_LIBOGG OR CXPROC_LIBVORBIS)
  target_sources(filex PUBLIC ${CXPROC_SRC_DIR}/option/audio/audio.h ${CXPROC_SRC_DIR}/option/audio/audio.c)

  target_sources(cxproc PUBLIC ${CXPROC_SRC_DIR}/option/audio/audio.h ${CXPROC_SRC_DIR}/option/audio/audio.c)

  target_sources(cxproc-cgi PUBLIC ${CXPROC_SRC_DIR}/option/audio/audio.h ${CXPROC_SRC_DIR}/option/audio/audio.c)

  target_sources(cxproc-test PUBLIC ${CXPROC_SRC_DIR}/option/audio/audio.h ${CXPROC_SRC_DIR}/option/audio/audio.c)

ENDIF ()

################################################################################
#
#  libid3tag
#

IF (CXPROC_LIBID3TAG)
  target_compile_definitions(filex       PUBLIC HAVE_LIBID3TAG)
  target_compile_definitions(cxproc      PUBLIC HAVE_LIBID3TAG)
  target_compile_definitions(cxproc-cgi  PUBLIC HAVE_LIBID3TAG)
  target_compile_definitions(cxproc-test PUBLIC HAVE_LIBID3TAG)

  target_link_libraries(filex ${LIBID3TAG_LIBRARY})
  target_link_libraries(cxproc ${LIBID3TAG_LIBRARY})
  target_link_libraries(cxproc-test ${LIBID3TAG_LIBRARY})
  target_link_libraries(cxproc-cgi  ${LIBID3TAG_LIBRARY})
  #IF (LIBMICROHTTPD_FOUND)
  #  target_link_libraries(cxproc-httpd ${LIBID3TAG_LIBRARY})
  #ENDIF ()
ENDIF (CXPROC_LIBID3TAG)

################################################################################
#
#  libogg
#

IF (CXPROC_LIBOGG)
  target_compile_definitions(filex       PUBLIC HAVE_LIBOGG)
  target_compile_definitions(cxproc      PUBLIC HAVE_LIBOGG)
  target_compile_definitions(cxproc-cgi  PUBLIC HAVE_LIBOGG)
  target_compile_definitions(cxproc-test PUBLIC HAVE_LIBOGG)

  target_link_libraries(filex ${LIBOGG_LIBRARY})
  target_link_libraries(cxproc ${LIBOGG_LIBRARY})
  target_link_libraries(cxproc-test ${LIBOGG_LIBRARY})
  target_link_libraries(cxproc-cgi  ${LIBOGG_LIBRARY})
  #IF (LIBMICROHTTPD_FOUND)
  #  target_link_libraries(cxproc-httpd ${LIBOGG_LIBRARY})
  #ENDIF ()
ENDIF (CXPROC_LIBOGG)

################################################################################
#
#  libvorbis
#

IF (CXPROC_LIBVORBIS)
  target_compile_definitions(filex       PUBLIC HAVE_LIBVORBIS)
  target_compile_definitions(cxproc      PUBLIC HAVE_LIBVORBIS)
  target_compile_definitions(cxproc-cgi  PUBLIC HAVE_LIBVORBIS)
  target_compile_definitions(cxproc-test PUBLIC HAVE_LIBVORBIS)

  target_link_libraries(filex ${LIBVORBIS_LIBRARY})
  target_link_libraries(cxproc ${LIBVORBIS_LIBRARY})
  target_link_libraries(cxproc-test ${LIBVORBIS_LIBRARY})
  target_link_libraries(cxproc-cgi  ${LIBVORBIS_LIBRARY})
  #IF (LIBMICROHTTPD_FOUND)
  #  target_link_libraries(cxproc-httpd ${LIBVORBIS_LIBRARY})
  #ENDIF ()
ENDIF (CXPROC_LIBVORBIS)

