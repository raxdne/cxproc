
################################################################################
#
# SQLite3 library
#

IF(SQLite3_FOUND)
  OPTION (CXPROC_SQLITE3 "Enable support for compiling cxproc with sqlite3." OFF)
ENDIF ()

IF (CXPROC_SQLITE3)
  INCLUDE_DIRECTORIES(${SQLite3_INCLUDE_DIR})

  target_sources(filex PUBLIC
    ${CXPROC_SRC_DIR}/option/database/database.c
    ${CXPROC_SRC_DIR}/option/database/database.h
    )
  target_compile_definitions(filex       PUBLIC HAVE_LIBSQLITE3)

  SET(SQLITE3_FILES
    ${CXPROC_SRC_DIR}/option/database/cxp_database.c
    ${CXPROC_SRC_DIR}/option/database/cxp_database.h
    ${CXPROC_SRC_DIR}/option/database/database.c
    ${CXPROC_SRC_DIR}/option/database/database.h
    )
  
  target_sources(cxproc PUBLIC ${SQLITE3_FILES})
  target_compile_definitions(cxproc      PUBLIC HAVE_LIBSQLITE3)

  target_sources(cxproc-cgi PUBLIC ${SQLITE3_FILES})
  target_compile_definitions(cxproc-cgi  PUBLIC HAVE_LIBSQLITE3)

  IF(CXPROC_TESTS)
    target_sources(cxproc-test PUBLIC ${SQLITE3_FILES})
    target_compile_definitions(cxproc-test PUBLIC HAVE_LIBSQLITE3)
  ENDIF ()

  IF (SQLite3_SOURCE_FILE)
    target_sources(cxproc PUBLIC ${SQLite3_SOURCE_FILE})

    target_sources(cxproc-cgi PUBLIC ${SQLite3_SOURCE_FILE})

    IF(CXPROC_TESTS)
      target_sources(cxproc-test PUBLIC ${SQLite3_SOURCE_FILE})
    ENDIF ()

    target_sources(filex PUBLIC ${SQLite3_SOURCE_FILE})
  ENDIF ()

  IF (SQLite3_SOURCE_FILE)
    add_compile_definitions(SQLITE_THREADSAFE=0 SQLITE_OMIT_LOAD_EXTENSION)
  ELSE (SQLite3_SOURCE_FILE)
    target_link_libraries(filex ${SQLite3_LIBRARY})
    target_link_libraries(cxproc ${SQLite3_LIBRARY})
    target_link_libraries(cxproc-cgi  ${SQLite3_LIBRARY})
    IF(CXPROC_TESTS)
      target_link_libraries(cxproc-test ${SQLite3_LIBRARY})
    ENDIF ()
    #IF (LIBMICROHTTPD_FOUND)
    #  target_link_libraries(cxproc-httpd ${SQLite3_LIBRARY})
    #ENDIF ()
  ENDIF ()

IF(BUILD_TESTING)
  
  IF(CXPROC_TESTS)
    add_test(NAME sql-code  
      COMMAND ${CXPROC_PREFIX}/bin/cxproc-test -t sql)

    set_tests_properties(sql-code
      PROPERTIES ENVIRONMENT "CXP_PATH=${PROJECT_SOURCE_DIR}//"
    )

    ENDIF ()

  add_test(NAME sql-cxp
    WORKING_DIRECTORY ${CXPROC_TEST_DIR}/option/sql
    COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)

  set_tests_properties(sql-cxp
    PROPERTIES ENVIRONMENT "CXP_PATH=${PROJECT_SOURCE_DIR}//"
  )

  add_test(NAME filex-sql
    WORKING_DIRECTORY ${CXPROC_PREFIX}
    COMMAND ${CXPROC_PREFIX}/bin/filex ${CXPROC_TEST_DIR}/option/sql/test.db3)
ENDIF(BUILD_TESTING)
  
ENDIF()

