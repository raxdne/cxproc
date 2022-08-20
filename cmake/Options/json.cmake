
################################################################################
#
# JSON
#
find_path(JSMN_INCLUDE_DIR NAMES jsmn.h
  HINTS "${PROJECT_SOURCE_DIR}/third-party/option/jsmn"
  ) 

IF (JSMN_INCLUDE_DIR)
  OPTION(CXPROC_JSON "Include JSON code" OFF)
  MARK_AS_ADVANCED(CXPROC_JSON)

  IF (CXPROC_JSON)
    target_sources(filex PUBLIC 
      ${CXPROC_SRC_DIR}/option/json/json.c
      ${CXPROC_SRC_DIR}/option/json/json.h
      )
    target_compile_definitions(filex       PUBLIC HAVE_JSON)
    
    SET(JSON_FILES
      ${CXPROC_SRC_DIR}/option/json/cxp_json.c
      ${CXPROC_SRC_DIR}/option/json/cxp_json.h
      ${CXPROC_SRC_DIR}/option/json/json.c
      ${CXPROC_SRC_DIR}/option/json/json.h
      )
    
    target_sources(cxproc PUBLIC ${JSON_FILES})
    target_compile_definitions(cxproc      PUBLIC HAVE_JSON)

    target_sources(cxproc-cgi PUBLIC ${JSON_FILES})
    target_compile_definitions(cxproc-cgi  PUBLIC HAVE_JSON)

    target_sources(cxproc-test PUBLIC ${JSON_FILES})
    target_compile_definitions(cxproc-test PUBLIC HAVE_JSON)

  ENDIF (CXPROC_JSON)

IF(BUILD_TESTING)
  
  add_test(NAME json-cxp
    WORKING_DIRECTORY ${CXPROC_TEST_DIR}/option/json
    COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)
ENDIF(BUILD_TESTING)
  
ENDIF ()


