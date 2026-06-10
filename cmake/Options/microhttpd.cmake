
################################################################################
#
# LIBMICROHTTPD
#

IF (LIBMICROHTTPD_LIBRARY)
  OPTION (CXPROC_HTTPD "Enable support for linking cxproc with LIBMICROHTTPD." OFF)
  MARK_AS_ADVANCED(CXPROC_HTTPD)
  INCLUDE_DIRECTORIES(${LIBMICROHTTPD_INCLUDE_DIR})
ENDIF (LIBMICROHTTPD_LIBRARY)

