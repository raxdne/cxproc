#string(REGEX REPLACE "[.]([0-9])\$" ".0\\1" upstream_version "${VERSION}")

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO raxdne/cxproc
    HEAD_REF develop
    REF 10c698556ac463924b467dccb2413fc3df59559f
    SHA512 0
)

#file(COPY "${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt" DESTINATION "${SOURCE_PATH}")
#file(COPY "${CMAKE_CURRENT_LIST_DIR}/7zip-config.cmake.in" DESTINATION "${SOURCE_PATH}")

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS        
	"-DVCPKG_TARGET_ARCHITECTURE=${VCPKG_TARGET_ARCHITECTURE}"
	"-DCMAKE_BUILD_TYPE=Release" 
	"-DCXPROC_DOC:BOOL=OFF" 
	"-DCXPROC_LEGACY:BOOL=ON" 
	"-DCXPROC_EXPERIMENTAL:BOOL=ON" 
	"-DCXPROC_MARKDOWN:BOOL=ON" 
	"-DCXPROC_ARCHIVE:BOOL=OFF" 
	"-DCXPROC_CURL:BOOL=ON" 
	"-DCXPROC_DUKTAPE:BOOL=ON" 
	"-DCXPROC_SQLITE3:BOOL=ON"
)

vcpkg_cmake_install()
vcpkg_copy_pdbs()

vcpkg_cmake_config_fixup()

#vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/DOC/License.txt")

#file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
