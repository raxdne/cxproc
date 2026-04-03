
# helper: default path to use as HINT

cmake_path(GET PROJECT_BINARY_DIR PARENT_PATH HINT_DIR)
MESSAGE(STATUS "Using HINT_DIR: ${HINT_DIR}")

SET(HINT_DIR_INCLUDE "${HINT_DIR}/include")
SET(HINT_DIR_LIB "${HINT_DIR}/lib")
SET(HINT_DIR_LIBD "${HINT_DIR}/debug/lib")


