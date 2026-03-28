#string(REGEX REPLACE "[.]([0-9])\$" ".0\\1" upstream_version "${VERSION}")

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO raxdne/cxproc
    HEAD_REF develop
    REF ac02a699d55b7e36ab7e259c1720cc6fb91e7f69
    SHA512 026418d2215294c90dfbabb170ef6c95177666954a2f3f6ed6cb786d09f9608cd3b9635506e4a88988c609baae95736c1b7b8c0d9cb0ac5f56c134750d4fa1de
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
