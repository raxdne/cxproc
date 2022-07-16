
###############################################################################
## packaging ##################################################################
###############################################################################

# TODO: configure package

set(CPACK_BINARY_NSIS "OFF")
set(CPACK_BINARY_NUGET "OFF")
set(CPACK_BINARY_WIX "OFF")
set(CPACK_BINARY_ZIP "ON")

INSTALL(TARGETS filex dir2csv dir2sqlite pietextx cxproc RUNTIME DESTINATION bin)
INSTALL(TARGETS cxproc-cgi test-cgi RUNTIME DESTINATION www/cgi-bin)
IF(MSVC AND CXPROC_STATIC)
  # no DLLs required
ELSE()
  file(GLOB DLL_DEPS ${CXPROC_PREFIX}/bin/*.dll)
  MESSAGE(STATUS "Dependencies: ${DLL_DEPS}")
  INSTALL(FILES ${DLL_DEPS} DESTINATION bin)
  INSTALL(FILES ${DLL_DEPS} DESTINATION www/cgi-bin)
  # TODO: handle debug DLLs
ENDIF()

INSTALL(DIRECTORY xml/schema DESTINATION .)
INSTALL(DIRECTORY doc DESTINATION .)
INSTALL(DIRECTORY examples DESTINATION .)
INSTALL(DIRECTORY "${PROJECT_SOURCE_DIR}/www/html" DESTINATION www)
INSTALL(FILES LICENSE README.md DESTINATION .)
INSTALL(FILES "${PROJECT_SOURCE_DIR}/www/conf/httpd/cxproc-template.conf" DESTINATION www/conf)

set(CPACK_PACKAGE_VENDOR "Vendor name")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Some summary")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README  "${PROJECT_SOURCE_DIR}/README.md")

set(CPACK_SOURCE_GENERATOR "ZIP")

set(CPACK_SOURCE_IGNORE_FILES
    /.git
    /dist
    /.*build.*
    /\\\\.DS_Store
)

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "raxdne")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libxml2" "libxslt" "libpcre2")
