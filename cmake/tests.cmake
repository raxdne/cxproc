
# https://gitlab.kitware.com/cmake/community/-/wikis/home#ctest
include (CTest)

# working directory for tests
SET(CXPROC_TEST_DIR "${PROJECT_SOURCE_DIR}/test")
SET(CXPROC_TEMP_DIR "${CXPROC_PREFIX}/tmp")

###############################################################################
## testing ####################################################################
###############################################################################

# external tests
add_test(NAME basics-code
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/bin/cxproc-test -t basics)

add_test(NAME res_node-code
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/bin/cxproc-test -t res_node)

add_test(NAME cxp-code
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/bin/cxproc-test -t cxp)

add_test(NAME dom-code
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/bin/cxproc-test -t dom)

add_test(NAME plain-code
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/bin/cxproc-test -t plain)

IF(CXPROC_PTHREAD)
  add_test(NAME thread-code
    WORKING_DIRECTORY ${CXPROC_PREFIX}
    COMMAND ${CXPROC_PREFIX}/bin/cxproc-test -t thread)

  add_test(NAME thread-cxp
    WORKING_DIRECTORY ${CXPROC_TEST_DIR}/thread
    COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)

ENDIF (CXPROC_PTHREAD)

add_test(NAME parser-code
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/bin/cxproc-test -t parser)

add_test(NAME cgi-empty
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/www/cgi-bin/cxproc-cgi)

set_property(TEST cgi-empty
  APPEND PROPERTY ENVIRONMENT CXP_LOGFILE=cxproc-empty.log;QUERY_STRING=
)

add_test(NAME cgi-mini
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/www/cgi-bin/cxproc-cgi)

set_property(TEST cgi-mini
  APPEND PROPERTY ENVIRONMENT CXP_LOGFILE=cxproc-mini.log;QUERY_STRING=cxp=info
)

add_test(NAME cgi-maxi
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/www/cgi-bin/cxproc-cgi)

set_property(TEST cgi-maxi
  APPEND PROPERTY ENVIRONMENT CXP_LOGFILE=cxproc-maxi.log;QUERY_STRING=path=&cxp=PieUiDir
)

add_test(NAME cgi-archive
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/www/cgi-bin/cxproc-cgi)

set_property(TEST cgi-archive
  APPEND PROPERTY ENVIRONMENT CXP_LOGFILE=cxproc-archive.log;QUERY_STRING=path=Test/Archive/TestArchive.zip/Test/SubTest/SubTest/Length_1024.txt
)

add_test(NAME cgi-sql
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/www/cgi-bin/cxproc-cgi)

set_property(TEST cgi-sql
  APPEND PROPERTY ENVIRONMENT CXP_LOGFILE=cxproc-sql.log;QUERY_STRING=path=Test/Databases/TestContent.db3&cxp=PiejQSqlite
)

set_property(TEST cgi-empty cgi-mini cgi-archive cgi-sql cgi-maxi
  APPEND PROPERTY ENVIRONMENT DOCUMENT_ROOT=${CXPROC_PREFIX}/www/html/;REQUEST_METHOD=GET;CXP_LOG=3;CXP_PATH=${CXPROC_PREFIX}/www/html/pie//;CXP_ROOT=${CXPROC_PREFIX}/www/html/test/
)
  
add_test(NAME cli-help
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/bin/cxproc -?)

add_test(NAME cli-description
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/bin/cxproc -? myclean.cxp)

add_test(NAME cli-env
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/bin/cxproc -e)

add_test(NAME cli-conf-plain
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  COMMAND ${CXPROC_PREFIX}/bin/cxproc -t abc.txt pie2html.xsl)

add_test(NAME cli-search
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/bin/cxproc -s "Validate.xsl")

add_test(NAME cli-conf-dir
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  COMMAND ${CXPROC_PREFIX}/bin/cxproc -t doc//)

add_test(NAME dir-code
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/bin/cxproc-test -t dir)

add_test(NAME dir-cxp
  WORKING_DIRECTORY ${CXPROC_TEST_DIR}/dir
  COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)

add_test(NAME each-cxp
  WORKING_DIRECTORY ${CXPROC_TEST_DIR}/each
  COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)

add_test(NAME eval-cxp
  WORKING_DIRECTORY ${CXPROC_TEST_DIR}/eval
  COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)

add_test(NAME plain-cxp
  WORKING_DIRECTORY ${CXPROC_TEST_DIR}/plain
  COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)

add_test(NAME system-cxp
  WORKING_DIRECTORY ${CXPROC_TEST_DIR}/system
  COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)

add_test(NAME worst-cxp
  WORKING_DIRECTORY ${CXPROC_TEST_DIR}/worst
  COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)

add_test(NAME xhtml-cxp
  WORKING_DIRECTORY ${CXPROC_TEST_DIR}/xhtml
  COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)

add_test(NAME xml-cxp
  WORKING_DIRECTORY ${CXPROC_TEST_DIR}/xml
  COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)

add_test(NAME xsl-cxp
  WORKING_DIRECTORY ${CXPROC_TEST_DIR}/xsl
  COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)

set_property(TEST cli-help cli-description cli-env cli-conf-plain cli-search cli-conf-dir dir-code dir-cxp each-cxp eval-cxp plain-cxp system-cxp worst-cxp xhtml-cxp xml-cxp xsl-cxp
  APPEND PROPERTY ENVIRONMENT CXP_PATH=${PROJECT_SOURCE_DIR}//
)

IF(MSVC)
  # no memcheck available yet
ELSE(MSVC)
  # http://stackoverflow.com/questions/40325957/how-do-i-add-valgrind-tests-to-my-cmake-test-target
  add_custom_target(test_memcheck
    COMMAND ${CMAKE_CTEST_COMMAND} --force-new-ctest-process --test-action memcheck
    )
ENDIF(MSVC)

# set envionment variables for testing
#set_tests_properties(basics-code res_node-code cxp-code dom-code plain-code parser-code cli-help cli-description cli-env cli-conf-plain cli-search cli-conf-file cli-conf-dir cli-dir-xsl dir-code dir-cxp each-cxp eval-cxp plain-cxp system-cxp worst-cxp xhtml-cxp xml-cxp xsl-cxp PROPERTIES
#  ENVIRONMENT "CXP_PATH=${PROJECT_SOURCE_DIR}//"
#  )

add_test(NAME dir2sqlite-args
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/bin/dir2sqlite ${CXPROC_TEST_DIR}/xml/)

add_test(NAME filex-empty
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/bin/filex)

set_tests_properties(filex-empty PROPERTIES WILL_FAIL TRUE)

add_test(NAME filex-dir
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/bin/filex ${CXPROC_TEST_DIR})

add_test(NAME filex-dir-recursive
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/bin/filex ${CXPROC_TEST_DIR}// ${CXPROC_TEMP_DIR}//)

add_test(NAME filex-archive
  WORKING_DIRECTORY ${CXPROC_PREFIX}
  COMMAND ${CXPROC_PREFIX}/bin/filex ${CXPROC_TEST_DIR}/xml/test-xml-zip.odt)

#filex-empty filex-dir filex-dir-recursive filex-archive filex-archive-intern filex-archive-intern-intern filex-url-http filex-sql filex-url-http-sql 

# ctest -N | perl -ne 'if (/^ +Test/) {chomp(); $_ =~ s/^[^\:]+\: +/ /; print;}'

IF (CXPROC_CURL)
  add_test(NAME filex-url-http
    WORKING_DIRECTORY ${CXPROC_PREFIX}
    COMMAND ${CXPROC_PREFIX}/bin/filex http://127.0.0.1/)

  IF (CXPROC_SQLITE3)
    add_test(NAME filex-url-http-sql
      WORKING_DIRECTORY ${CXPROC_PREFIX}
      COMMAND ${CXPROC_PREFIX}/bin/filex http://127.0.0.1/TestContent.db3)
  ENDIF()
ENDIF()


IF (CXPROC_PIE)
  add_test(NAME pie-code
    WORKING_DIRECTORY ${CXPROC_PREFIX}
    COMMAND ${CXPROC_PREFIX}/bin/cxproc-test -t pie)

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

  add_test(NAME calendar-cxp
    WORKING_DIRECTORY ${CXPROC_TEST_DIR}/option/pie/calendar
    COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)

  add_test(NAME cli-conf-year
    WORKING_DIRECTORY ${CXPROC_PREFIX}
    COMMAND ${CXPROC_PREFIX}/bin/cxproc -t 2012)

  add_test(NAME cli-year
    WORKING_DIRECTORY ${CXPROC_PREFIX}
    COMMAND ${CXPROC_PREFIX}/bin/cxproc 2012)

  set_property(TEST pie-code pie-cxp-import-cxp calendar-cxp cli-conf-year cli-year
    APPEND PROPERTY ENVIRONMENT CXP_PATH=${PROJECT_SOURCE_DIR}//
    )
  
  add_test(NAME ics-code
    WORKING_DIRECTORY ${CXPROC_PREFIX}
    COMMAND ${CXPROC_PREFIX}/bin/cxproc-test -t ics)

  add_test(NAME vcf-code
    WORKING_DIRECTORY ${CXPROC_PREFIX}
    COMMAND ${CXPROC_PREFIX}/bin/cxproc-test -t vcf)

ENDIF (CXPROC_PIE)
