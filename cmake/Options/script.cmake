
################################################################################
#
# duktape
#

FIND_PACKAGE( Duktape )

IF (DUKTAPE_SOURCE_FILE)
  OPTION (CXPROC_DUKTAPE "Enable support for compiling cxproc with duktape." OFF)
ELSEIF (DUKTAPE_LIBRARY)
  OPTION (CXPROC_DUKTAPE "Enable support for linking cxproc with duktape." OFF)
ENDIF ()


IF (CXPROC_DUKTAPE)
  
  SET(DUKTAPE_FILES
    ${CXPROC_SRC_DIR}/option/script/script.h
    ${CXPROC_SRC_DIR}/option/script/script.c
    )

  IF (DUKTAPE_SOURCE_FILE)
    SET(DUKTAPE_FILES ${DUKTAPE_FILES}
      ${DUKTAPE_INCLUDE_DIR}/duktape.h
      ${DUKTAPE_SOURCE_FILE}
      )
  ENDIF (DUKTAPE_SOURCE_FILE)

  target_sources(cxproc PUBLIC ${DUKTAPE_FILES})

  target_sources(cxproc-cgi PUBLIC ${DUKTAPE_FILES})

  target_sources(cxproc-test PUBLIC ${DUKTAPE_FILES})

  INCLUDE_DIRECTORIES(${DUKTAPE_INCLUDE_DIR})

  target_compile_definitions(cxproc      PUBLIC HAVE_JS)
  target_compile_definitions(cxproc-cgi  PUBLIC HAVE_JS)
  target_compile_definitions(cxproc-test PUBLIC HAVE_JS)
  IF (DUKTAPE_SOURCE_FILE)
  ELSE (DUKTAPE_SOURCE_FILE)
    target_link_libraries(cxproc ${DUKTAPE_LIBRARY})
    target_link_libraries(cxproc-test ${DUKTAPE_LIBRARY})
    target_link_libraries(cxproc-cgi ${DUKTAPE_LIBRARY})
    #IF (LIBMICROHTTPD_FOUND)
    #  target_link_libraries(cxproc-httpd ${DUKTAPE_LIBRARY})
    #ENDIF ()
  ENDIF ()

IF(CXPROC_TESTS)
  
  add_test(NAME script-cxp
    WORKING_DIRECTORY ${CXPROC_TEST_DIR}/option/script
    COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)

ENDIF(CXPROC_TESTS)
  
ENDIF ()
