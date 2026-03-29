
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
    ${CXPROC_SRC_DIR}/option/script/cxp_script.h
    ${CXPROC_SRC_DIR}/option/script/cxp_script.c
    )

  IF (DUKTAPE_SOURCE_FILE)
    SET(DUKTAPE_FILES ${DUKTAPE_FILES}
      ${DUKTAPE_INCLUDE_DIR}/duktape.h
      ${DUKTAPE_SOURCE_FILE}
      )
    # no library to be linked
  ELSE (DUKTAPE_SOURCE_FILE)
    target_link_libraries(cxproc ${DUKTAPE_LIBRARY})
    target_link_libraries(cxproc-cgi ${DUKTAPE_LIBRARY})
    IF(CXPROC_TESTS)
      target_link_libraries(cxproc-test ${DUKTAPE_LIBRARY})
    ENDIF ()
    #IF (LIBMICROHTTPD_FOUND)
    #  target_link_libraries(cxproc-httpd ${DUKTAPE_LIBRARY})
    #ENDIF ()
  ENDIF (DUKTAPE_SOURCE_FILE)

  target_sources(cxproc PUBLIC ${DUKTAPE_FILES})

  target_sources(cxproc-cgi PUBLIC ${DUKTAPE_FILES})

  IF(CXPROC_TESTS)
    target_sources(cxproc-test PUBLIC ${DUKTAPE_FILES})
  ENDIF ()

  INCLUDE_DIRECTORIES(${DUKTAPE_INCLUDE_DIR})

  target_compile_definitions(cxproc      PUBLIC HAVE_JS)
  target_compile_definitions(cxproc-cgi  PUBLIC HAVE_JS)
  IF(CXPROC_TESTS)
    target_compile_definitions(cxproc-test PUBLIC HAVE_JS)
  ENDIF ()

  IF(BUILD_TESTING)
    add_test(NAME script-cxp
      WORKING_DIRECTORY ${CXPROC_TEST_DIR}/option/script
      COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)
  ENDIF(BUILD_TESTING)
  
ENDIF ()
