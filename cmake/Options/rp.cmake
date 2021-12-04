
################################################################################
#
# RP
#
#OPTION(CXPROC_RP "Include RP code" OFF)

IF (CXPROC_RP)
  SET(CXPROC_HEADERS ${CXPROC_HEADERS} ${CXPROC_SRC_DIR}/rp.h)
  SET(CXPROC_SOURCES ${CXPROC_SOURCES} ${CXPROC_SRC_DIR}/rp.c)
ENDIF (CXPROC_RP)

IF (CXPROC_RP)
  add_test(NAME rp-cxp
    WORKING_DIRECTORY ${CXPROC_TEST_DIR}/option/rp
    COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)
ENDIF ()

IF (CXPROC_RP)
  target_compile_definitions(filex       PUBLIC HAVE_RP)
  target_compile_definitions(cxproc      PUBLIC HAVE_RP)
  target_compile_definitions(cxproc-cgi  PUBLIC HAVE_RP)
  target_compile_definitions(cxproc-test PUBLIC HAVE_RP)
ENDIF (CXPROC_RP)

