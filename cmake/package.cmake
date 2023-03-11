
###############################################################################
## packaging ##################################################################
###############################################################################

# TODO: configure package

set(CPACK_PACKAGE_DIRECTORY "${PROJECT_SOURCE_DIR}/..")

INSTALL(TARGETS filex dir2csv dir2sqlite pietextx cxproc RUNTIME DESTINATION bin)
INSTALL(TARGETS cxproc-cgi test-cgi RUNTIME DESTINATION lib/cgi-bin)

if (MSVC)
  set(CPACK_GENERATOR ZIP)
  IF(NOT CXPROC_STATIC)
    file(GLOB DLL_DEPS ${CXPROC_PREFIX}/bin/*.dll)
    MESSAGE(STATUS "Dependencies: ${DLL_DEPS}")
    INSTALL(FILES ${DLL_DEPS} DESTINATION bin)
    INSTALL(FILES ${DLL_DEPS} DESTINATION lib/cgi-bin)
    # TODO: handle debug DLLs
  ENDIF()
else()
  set(CPACK_GENERATOR TGZ)
endif()

INSTALL(DIRECTORY xml/schema DESTINATION share/xml/cxproc)
INSTALL(DIRECTORY doc DESTINATION share/doc/cxproc)
INSTALL(DIRECTORY examples DESTINATION share/doc/cxproc)
INSTALL(DIRECTORY "${PROJECT_SOURCE_DIR}/www/html" DESTINATION share/doc/cxproc/www)
INSTALL(FILES LICENSE README.md DESTINATION share/doc/cxproc)
INSTALL(FILES "${PROJECT_SOURCE_DIR}/www/conf/httpd/cxproc-template.conf" DESTINATION share/doc/cxproc/www/conf)

set(CPACK_PACKAGE_VENDOR "Vendor name")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Some summary")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README  "${PROJECT_SOURCE_DIR}/README.md")

