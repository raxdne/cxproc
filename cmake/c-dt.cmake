#
#
#

SET (CDT_SOURCE_DIR "${PROJECT_SOURCE_DIR}/third-party/c-dt")

set (CDT_SOURCE_FILES
  ${CDT_SOURCE_DIR}/dt.h
  ${CDT_SOURCE_DIR}/dt_accessor.h
  ${CDT_SOURCE_DIR}/dt_accessor.c
  ${CDT_SOURCE_DIR}/dt_arithmetic.h
  ${CDT_SOURCE_DIR}/dt_arithmetic.c
  ${CDT_SOURCE_DIR}/dt_char.h
  ${CDT_SOURCE_DIR}/dt_char.c
  ${CDT_SOURCE_DIR}/dt_core.h
  ${CDT_SOURCE_DIR}/dt_core.c
  ${CDT_SOURCE_DIR}/dt_dow.h
  ${CDT_SOURCE_DIR}/dt_dow.c
  ${CDT_SOURCE_DIR}/dt_easter.h
  ${CDT_SOURCE_DIR}/dt_easter.c
  ${CDT_SOURCE_DIR}/dt_length.h
  ${CDT_SOURCE_DIR}/dt_length.c
  ${CDT_SOURCE_DIR}/dt_navigate.h
  ${CDT_SOURCE_DIR}/dt_navigate.c
  ${CDT_SOURCE_DIR}/dt_parse_iso.h
  ${CDT_SOURCE_DIR}/dt_parse_iso.c
  ${CDT_SOURCE_DIR}/dt_search.h
  ${CDT_SOURCE_DIR}/dt_search.c
  ${CDT_SOURCE_DIR}/dt_tm.h
  ${CDT_SOURCE_DIR}/dt_tm.c
  ${CDT_SOURCE_DIR}/dt_util.h
  ${CDT_SOURCE_DIR}/dt_util.c
  ${CDT_SOURCE_DIR}/dt_valid.h
  ${CDT_SOURCE_DIR}/dt_valid.c
  ${CDT_SOURCE_DIR}/dt_weekday.h
  ${CDT_SOURCE_DIR}/dt_weekday.c
  ${CDT_SOURCE_DIR}/dt_workday.h
  ${CDT_SOURCE_DIR}/dt_workday.c
  ${CDT_SOURCE_DIR}/dt_zone.h
  ${CDT_SOURCE_DIR}/dt_zone.c
  )

#INCLUDE_DIRECTORIES(${CDT_SOURCE_DIR})

target_sources(pietextx   PUBLIC ${CDT_SOURCE_FILES})

target_sources(dir2sqlite PUBLIC ${CDT_SOURCE_FILES})

target_sources(dir2csv    PUBLIC ${CDT_SOURCE_FILES})

target_sources(filex      PUBLIC ${CDT_SOURCE_FILES})

target_sources(cxproc     PUBLIC ${CDT_SOURCE_FILES})

target_sources(cxproc-cgi PUBLIC ${CDT_SOURCE_FILES})

target_sources(cxproc-test PUBLIC ${CDT_SOURCE_FILES})


# add_library(cdt STATIC ${CDT_SOURCE_FILES})

# SET (CDT_LIBRARY "${CXPROC_PREFIX}/libcdt.a")

# target_link_libraries(pietextx   ${CDT_LIBRARY})

# target_link_libraries(dir2sqlite ${CDT_LIBRARY})

# target_link_libraries(dir2csv    ${CDT_LIBRARY})

# target_link_libraries(filex      ${CDT_LIBRARY})

# target_link_libraries(cxproc     ${CDT_LIBRARY})

# target_link_libraries(cxproc-cgi ${CDT_LIBRARY})

# target_link_libraries(cxproc-test ${CDT_LIBRARY})

#add_definitions(-DDT_PARSE_ISO_STRICT)

