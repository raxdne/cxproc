
################################################################################
#
# PETRINET
#
OPTION(CXPROC_PETRINET "Include Petrinet code" ON)


IF (CXPROC_PETRINET)

  SET(PETRINET_FILES
    ${CXPROC_SRC_DIR}/option/petrinet/petrinet.h
    ${CXPROC_SRC_DIR}/option/petrinet/petrinet.c
    )

  target_sources(cxproc PUBLIC ${PETRINET_FILES})

  target_sources(cxproc-cgi PUBLIC ${PETRINET_FILES})

  target_sources(cxproc-test PUBLIC ${PETRINET_FILES})

  add_definitions(-DHAVE_PETRINET)

  add_test(NAME petrinet-code
    WORKING_DIRECTORY ${CXPROC_PREFIX}
    COMMAND ${CXPROC_PREFIX}/bin/cxproc-test -t petrinet)

  add_test(NAME petrinet-cxp
    WORKING_DIRECTORY ${CXPROC_TEST_DIR}/option/petrinet
    COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)

  set_tests_properties(petrinet-cxp PROPERTIES
    ENVIRONMENT "CXP_PATH=${PROJECT_SOURCE_DIR}//")

ENDIF (CXPROC_PETRINET)

