
# https://vicrucann.github.io/tutorials/quick-cmake-doxygen/

# check if Doxygen is installed
FIND_PACKAGE(Doxygen)

if (DOXYGEN_FOUND)
  add_custom_target( doc_doxygen
    COMMAND DOXYGEN_NAME=${CMAKE_PROJECT_NAME} DOXYGEN_OUTPUT=${PROJECT_BINARY_DIR}/.. ${DOXYGEN_EXECUTABLE} ${PROJECT_SOURCE_DIR}/misc/Doxyfile
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM )
else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)
