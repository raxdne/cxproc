
################################################################################
#
# LIBMICROHTTPD
#

IF (LIBMICROHTTPD_LIBRARY)
  OPTION (CXPROC_HTTPD "Enable support for linking cxproc with LIBMICROHTTPD." OFF)
  MARK_AS_ADVANCED(CXPROC_HTTPD)
ENDIF (LIBMICROHTTPD_LIBRARY)

IF (CXPROC_HTTPD)

  ADD_EXECUTABLE(cxproc-httpd
    ${CXPROC_SRC_DIR}/cxproc-httpd.c
    #${CXPROC_SOURCES} ${CXPROC_HEADERS}
  )

  INCLUDE_DIRECTORIES(${LIBMICROHTTPD_INCLUDE_DIR})
  target_compile_definitions(cxproc-httpd PUBLIC HAVE_HTTPD)
  target_link_libraries(cxproc-httpd ${LIBMICROHTTPD_LIBRARY} gnutls)

  IF(BUILD_TESTING)
  ENDIF(BUILD_TESTING)
  
ENDIF ()
