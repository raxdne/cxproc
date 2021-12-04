
# https://vicrucann.github.io/tutorials/quick-cmake-doxygen/

# check if Doxygen is installed
FIND_PACKAGE(Doxygen)

if (DOXYGEN_FOUND)
  option(CXPROC_DOC "Build documentation" OFF)
else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)

if (CXPROC_DOC)
  # note the option ALL which allows to build the docs together with the application
  add_custom_target( doc_doxygen ALL
    COMMAND DOXYGEN_NAME=${CMAKE_PROJECT_NAME} DOXYGEN_OUTPUT=${PROJECT_BINARY_DIR}/.. ${DOXYGEN_EXECUTABLE} ${PROJECT_SOURCE_DIR}/misc/Doxyfile
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM )
endif ()

