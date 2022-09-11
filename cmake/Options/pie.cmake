#
#
#

FIND_PACKAGE( LibCMark )

# TODO: cmake_dependent_option(USE_FOO "Use Foo" ON "USE_BAR;NOT USE_ZOT" OFF)

IF (CXPROC_PCRE2)
  OPTION(CXPROC_PIE "Include PIE code" ON)
  OPTION(CXPROC_MARKDOWN "Compile MARKDOWN code" ON)
ELSE ()
  SET(CXPROC_PIE OFF)
  SET(CXPROC_MARKDOWN OFF)
ENDIF ()


################################################################################
#
# PIE
#

IF (CXPROC_PIE)

  target_sources(filex PUBLIC 
    ${CXPROC_SRC_DIR}/option/ics/ics.c
    ${CXPROC_SRC_DIR}/option/ics/ics.h
    ${CXPROC_SRC_DIR}/option/pie/pie_dtd.h
    ${CXPROC_SRC_DIR}/option/pie/pie_element.c
    ${CXPROC_SRC_DIR}/option/pie/pie_element.h
    ${CXPROC_SRC_DIR}/option/pie/pie_text_blocks.c
    ${CXPROC_SRC_DIR}/option/pie/pie_text_blocks.h
    ${CXPROC_SRC_DIR}/option/pie/pie_text_tags.c
    ${CXPROC_SRC_DIR}/option/pie/pie_text_tags.h
    ${CXPROC_SRC_DIR}/option/vcf/vcf.c
    ${CXPROC_SRC_DIR}/option/vcf/vcf.h
    )

  target_sources(pietextx PUBLIC 
    ${CXPROC_SRC_DIR}/option/pie/pie_dtd.h
    ${CXPROC_SRC_DIR}/option/pie/pie_element.c
    ${CXPROC_SRC_DIR}/option/pie/pie_element.h
    ${CXPROC_SRC_DIR}/option/pie/pie_text_blocks.c
    ${CXPROC_SRC_DIR}/option/pie/pie_text_blocks.h
    ${CXPROC_SRC_DIR}/option/pie/pie_text_tags.c
    ${CXPROC_SRC_DIR}/option/pie/pie_text_tags.h
    )

  SET(PIE_FILES
    ${CXPROC_SRC_DIR}/option/ics/ics.c
    ${CXPROC_SRC_DIR}/option/ics/ics.h
    ${CXPROC_SRC_DIR}/option/pie/pie_element.c
    ${CXPROC_SRC_DIR}/option/pie/pie_element.h
    ${CXPROC_SRC_DIR}/option/pie/pie_text_blocks.c
    ${CXPROC_SRC_DIR}/option/pie/pie_text_blocks.h
    ${CXPROC_SRC_DIR}/option/pie/pie_text.c
    ${CXPROC_SRC_DIR}/option/pie/pie_text.h ${CXPROC_SRC_DIR}/option/pie/pie_dtd.h
    ${CXPROC_SRC_DIR}/option/pie/pie_text_tags.c
    ${CXPROC_SRC_DIR}/option/pie/pie_text_tags.h
    ${CXPROC_SRC_DIR}/option/vcf/vcf.c
    ${CXPROC_SRC_DIR}/option/vcf/vcf.h
    )

  IF (CXPROC_LEGACY)
    SET(PIE_FILES
      ${PIE_FILES}
      ${CXPROC_SRCLIB_DIR}/option/sunriset/sunriset.c
      ${CXPROC_SRCLIB_DIR}/option/sunriset/sunriset.h
      )
  ENDIF ()

  IF (CXPROC_MARKDOWN)
    SET(PIE_FILES
      ${PIE_FILES}
      #${LIBCMARK_INCLUDE_DIR}/buffer.c
      ${CXPROC_SRC_DIR}/option/pie/pie_cmark.c
      ${CXPROC_SRC_DIR}/option/pie/pie_cmark.h
      )
  ENDIF ()

  target_sources(cxproc PUBLIC ${PIE_FILES})

  target_sources(cxproc-cgi PUBLIC ${PIE_FILES})

  target_sources(cxproc-test PUBLIC ${PIE_FILES})

  target_compile_definitions(filex       PUBLIC HAVE_PIE)
  target_compile_definitions(pietextx    PUBLIC HAVE_PIE)
  target_compile_definitions(cxproc      PUBLIC HAVE_PIE)
  target_compile_definitions(cxproc-cgi  PUBLIC HAVE_PIE)
  target_compile_definitions(cxproc-test PUBLIC HAVE_PIE)

IF (BUILD_TESTING)
  add_test(NAME pie-code
    WORKING_DIRECTORY ${CXPROC_PREFIX}
    COMMAND ${CXPROC_PREFIX}/bin/cxproc-test -t pie)

  add_test(NAME pietextx-cli
    WORKING_DIRECTORY ${CXPROC_PREFIX}
    COMMAND ${CXPROC_PREFIX}/bin/pietextx ${CXPROC_TEST_DIR}/option/pie/text/test-pie-9.txt)

  add_test(NAME pie-cxp-import-circular
    WORKING_DIRECTORY ${CXPROC_TEST_DIR}/option/pie/text/circular
    COMMAND ${CXPROC_PREFIX}/bin/cxproc config-circular.cxp)

  add_test(NAME pie-cxp-import-csv
    WORKING_DIRECTORY ${CXPROC_TEST_DIR}/option/pie/text/csv
    COMMAND ${CXPROC_PREFIX}/bin/cxproc config-pie-import-csv.cxp)

  add_test(NAME pie-cxp-import-cxp
    WORKING_DIRECTORY ${CXPROC_TEST_DIR}/option/pie/text/cxp
    COMMAND ${CXPROC_PREFIX}/bin/cxproc config-pie-import-cxp.cxp)

  add_test(NAME pie-cxp-import-line
    WORKING_DIRECTORY ${CXPROC_TEST_DIR}/option/pie/text/line
    COMMAND ${CXPROC_PREFIX}/bin/cxproc config-pie-import-line.cxp)

  add_test(NAME pie-cxp-import-log
    WORKING_DIRECTORY ${CXPROC_TEST_DIR}/option/pie/text/log
    COMMAND ${CXPROC_PREFIX}/bin/cxproc config-pie-import-log.cxp)

  add_test(NAME pie-cxp-import-plain
    WORKING_DIRECTORY ${CXPROC_TEST_DIR}/option/pie/text/plain
    COMMAND ${CXPROC_PREFIX}/bin/cxproc config-pie-import-plain.cxp)

  add_test(NAME pie-cxp-import-script
    WORKING_DIRECTORY ${CXPROC_TEST_DIR}/option/pie/text/script
    COMMAND ${CXPROC_PREFIX}/bin/cxproc config-pie-import-script.cxp)

  add_test(NAME pie-cxp-import-xml
    WORKING_DIRECTORY ${CXPROC_TEST_DIR}/option/pie/text/xml
    COMMAND ${CXPROC_PREFIX}/bin/cxproc config-pie-import-xml.cxp)

  set_property(TEST pie-code pie-cxp-import-cxp
    APPEND PROPERTY ENVIRONMENT CXP_PATH=${PROJECT_SOURCE_DIR}//
    )
  
  add_test(NAME ics-code
    WORKING_DIRECTORY ${CXPROC_PREFIX}
    COMMAND ${CXPROC_PREFIX}/bin/cxproc-test -t ics)

  add_test(NAME vcf-code
    WORKING_DIRECTORY ${CXPROC_PREFIX}
    COMMAND ${CXPROC_PREFIX}/bin/cxproc-test -t vcf)

ENDIF (BUILD_TESTING)

ENDIF (CXPROC_PIE)

################################################################################
#
# 
#

IF (CXPROC_PCRE2)
  target_link_libraries(pietextx ${PCRE2_LIBRARY})
ENDIF ()

IF (CXPROC_LIBZ)
  target_link_libraries(pietextx ${ZLIB_LIBRARY_RELEASE})
ENDIF (CXPROC_LIBZ)

IF (CXPROC_LZMA)
  target_link_libraries(pietextx ${LIBLZMA_LIBRARIES})
ENDIF (CXPROC_LZMA)

target_link_libraries(pietextx ${LIBICONV_LIBRARY} ${LIBCHARSET_LIBRARY} ${LIBXML2_LIBRARY})

IF (CXPROC_MARKDOWN)
  add_definitions(-DWITH_MARKDOWN)
  target_link_libraries(filex ${LIBCMARK_LIBRARY})
  target_link_libraries(dir2csv ${LIBCMARK_LIBRARY})
  target_link_libraries(dir2sqlite ${LIBCMARK_LIBRARY})
  target_link_libraries(cxproc ${LIBCMARK_LIBRARY})
  target_link_libraries(cxproc-test ${LIBCMARK_LIBRARY})
  target_link_libraries(cxproc-cgi ${LIBCMARK_LIBRARY})
ENDIF ()


IF(MSVC)
ELSE(MSVC)
  target_link_libraries(pietextx -ldl -lm)
ENDIF(MSVC)

