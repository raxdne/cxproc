################################################################################
#
# libarchive
#

FIND_PACKAGE( LibArchive )

IF (LibArchive_LIBRARIES)
  OPTION(CXPROC_ARCHIVE "Enable support for linking cxproc with libarchive." OFF)
ENDIF (LibArchive_LIBRARIES)


IF (CXPROC_ARCHIVE)
  INCLUDE_DIRECTORIES(${LibArchive_INCLUDE_DIR})

  SET(ARCHIVE_FILES
    ${CXPROC_SRC_DIR}/option/archive/archive.h
    ${CXPROC_SRC_DIR}/option/archive/archive.c
    )

  SET(CXP_ARCHIVE_FILES
    ${CXPROC_SRC_DIR}/option/archive/cxp_archive.h
    ${CXPROC_SRC_DIR}/option/archive/cxp_archive.c
    )

  target_sources(dir2csv PUBLIC ${ARCHIVE_FILES})

  target_sources(dir2sqlite PUBLIC ${ARCHIVE_FILES})

  target_sources(filex PUBLIC ${ARCHIVE_FILES})

  target_sources(cxproc PUBLIC ${ARCHIVE_FILES} ${CXP_ARCHIVE_FILES})

  target_sources(cxproc-cgi PUBLIC ${ARCHIVE_FILES} ${CXP_ARCHIVE_FILES})

  target_sources(cxproc-test PUBLIC ${ARCHIVE_FILES} ${CXP_ARCHIVE_FILES})

  add_definitions(-DHAVE_LIBARCHIVE)

  IF (${LibArchive_LIBRARIES} MATCHES ".+static\\.lib$" OR ${LibArchive_LIBRARIES} MATCHES ".+\\.a$")
    add_definitions(-DLIBARCHIVE_STATIC)
  ENDIF ()

  IF(MSVC)
    # additional libraries with VC++
    target_link_libraries(filex ${LibArchive_LIBRARIES} ${LIBBZ2_LIBRARY_RELEASE} ${CRYPTO_LIBRARY} ${LZ4_LIBRARY} ${ZSTD_LIBRARY} ${LZO2_LIBRARY})
    target_link_libraries(dir2csv ${LibArchive_LIBRARIES} ${LIBBZ2_LIBRARY_RELEASE} ${CRYPTO_LIBRARY} ${LZ4_LIBRARY} ${ZSTD_LIBRARY} ${LZO2_LIBRARY})
    target_link_libraries(dir2sqlite ${LibArchive_LIBRARIES} ${LIBBZ2_LIBRARY_RELEASE} ${CRYPTO_LIBRARY} ${LZ4_LIBRARY} ${ZSTD_LIBRARY} ${LZO2_LIBRARY})
    target_link_libraries(cxproc ${LibArchive_LIBRARIES} ${LIBBZ2_LIBRARY_RELEASE} ${CRYPTO_LIBRARY} ${LZ4_LIBRARY} ${ZSTD_LIBRARY} ${LZO2_LIBRARY})
    target_link_libraries(cxproc-test ${LibArchive_LIBRARIES} ${LIBBZ2_LIBRARY_RELEASE} ${CRYPTO_LIBRARY} ${LZ4_LIBRARY} ${ZSTD_LIBRARY} ${LZO2_LIBRARY})
    target_link_libraries(cxproc-cgi ${LibArchive_LIBRARIES} ${LIBBZ2_LIBRARY_RELEASE} ${CRYPTO_LIBRARY} ${LZ4_LIBRARY} ${ZSTD_LIBRARY} ${LZO2_LIBRARY})
  ELSE(MSVC)
    target_link_libraries(filex ${LibArchive_LIBRARIES})
    target_link_libraries(dir2csv ${LibArchive_LIBRARIES})
    target_link_libraries(dir2sqlite ${LibArchive_LIBRARIES})
    target_link_libraries(cxproc ${LibArchive_LIBRARIES})
    target_link_libraries(cxproc-test ${LibArchive_LIBRARIES})
    target_link_libraries(cxproc-cgi ${LibArchive_LIBRARIES})
  ENDIF(MSVC)
  
  #IF (LIBMICROHTTPD_FOUND)
  #  target_link_libraries(cxproc-httpd ${LibArchive_LIBRARIES})
  #ENDIF ()

IF(BUILD_TESTING)
  
  add_test(NAME archive-code
    WORKING_DIRECTORY ${CXPROC_PREFIX}
    COMMAND ${CXPROC_PREFIX}/bin/cxproc-test -t archive)

  add_test(NAME archive-cxp
    WORKING_DIRECTORY ${CXPROC_TEST_DIR}/option/archive
    COMMAND ${CXPROC_PREFIX}/bin/cxproc config.cxp)

  set_tests_properties(archive-code archive-cxp PROPERTIES
    ENVIRONMENT "CXP_PATH=${PROJECT_SOURCE_DIR}//"
    )

  add_test(NAME filex-archive-intern
    WORKING_DIRECTORY ${CXPROC_PREFIX}
    COMMAND ${CXPROC_PREFIX}/bin/filex ${CXPROC_TEST_DIR}/xml/test-xml-zip.odt/content.xml)

  add_test(NAME filex-archive-intern-intern
    WORKING_DIRECTORY ${CXPROC_PREFIX}
    COMMAND ${CXPROC_PREFIX}/bin/filex ${CXPROC_TEST_DIR}/option/archive/test-zip-odt.zip/test-xml-zip.odt/content.xml)
ENDIF(BUILD_TESTING)
  

ENDIF (CXPROC_ARCHIVE)

