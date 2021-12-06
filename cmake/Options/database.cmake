
################################################################################
#
# SQLite3 library
#

FIND_PACKAGE( SQLite3 )

IF (SQLite3_INCLUDE_DIR)
  IF (EXISTS "${SQLite3_INCLUDE_DIR}/sqlite3.c")
    SET(SQLite3_SOURCE_FILE "${SQLite3_INCLUDE_DIR}/sqlite3.c")
    UNSET(SQLite3_LIBRARY)
  ENDIF()
ENDIF (SQLite3_INCLUDE_DIR)

IF (SQLite3_SOURCE_FILE)
  OPTION (CXPROC_SQLITE3 "Enable support for compiling cxproc with sqlite3." OFF)
ELSEIF (SQLite3_LIBRARY)
  OPTION (CXPROC_SQLITE3 "Enable support for linking cxproc with sqlite3." OFF)
ENDIF ()

IF (CXPROC_SQLITE3)
  INCLUDE_DIRECTORIES(${SQLite3_INCLUDE_DIR})

  target_sources(filex PUBLIC
    ${CXPROC_SRC_DIR}/option/database/database.c
    ${CXPROC_SRC_DIR}/option/database/database.h
    )

  SET(SQLITE3_FILES
    ${CXPROC_SRC_DIR}/option/database/cxp_database.c
    ${CXPROC_SRC_DIR}/option/database/cxp_database.h
    ${CXPROC_SRC_DIR}/option/database/database.c
    ${CXPROC_SRC_DIR}/option/database/database.h
    )
  
  target_sources(cxproc PUBLIC ${PIE_FILES})

  target_sources(cxproc-cgi PUBLIC ${PIE_FILES})

  target_sources(cxproc-test PUBLIC ${PIE_FILES})

  IF (SQLite3_SOURCE_FILE)
    target_sources(cxproc PUBLIC ${SQLite3_SOURCE_FILE})

    target_sources(cxproc-cgi PUBLIC ${SQLite3_SOURCE_FILE})

    target_sources(cxproc-test PUBLIC ${SQLite3_SOURCE_FILE})

    target_sources(filex PUBLIC ${SQLite3_SOURCE_FILE})
  ENDIF ()

  target_compile_definitions(filex       PUBLIC HAVE_LIBSQLITE3)
  target_compile_definitions(cxproc      PUBLIC HAVE_LIBSQLITE3)
  target_compile_definitions(cxproc-cgi  PUBLIC HAVE_LIBSQLITE3)
  target_compile_definitions(cxproc-test PUBLIC HAVE_LIBSQLITE3)

  IF (SQLite3_SOURCE_FILE)
    add_definitions(-DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION)
  ELSE (SQLite3_SOURCE_FILE)
    target_link_libraries(filex ${SQLite3_LIBRARY})
    target_link_libraries(cxproc ${SQLite3_LIBRARY})
    target_link_libraries(cxproc-test ${SQLite3_LIBRARY})
    target_link_libraries(cxproc-cgi  ${SQLite3_LIBRARY})
    #IF (LIBMICROHTTPD_FOUND)
    #  target_link_libraries(cxproc-httpd ${SQLite3_LIBRARY})
    #ENDIF ()
  ENDIF ()

IF(CXPROC_TESTS)
  
  add_test(NAME sql-code  
    COMMAND ${CXPROC_PREFIX}/bin/cxproc-test -t sql)

  add_test(NAME sql-cxp
    WORKING_DIRECTORY ${CXPROC_TEST_DIR}/option/sql
    COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)

  set_tests_properties(sql-code sql-cxp
    PROPERTIES ENVIRONMENT "CXP_PATH=${PROJECT_SOURCE_DIR}//"
  )

  add_test(NAME filex-sql
    WORKING_DIRECTORY ${CXPROC_PREFIX}
    COMMAND ${CXPROC_PREFIX}/bin/filex ${CXPROC_TEST_DIR}/option/sql/test.db3)
ENDIF(CXPROC_TESTS)
  
ENDIF()

