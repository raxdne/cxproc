#
#
#

SET (CDT_SOURCE_DIR "${PROJECT_SOURCE_DIR}/../c-dt/")

set (CDT_SOURCE_FILES
        ${CDT_SOURCE_DIR}/dt_accessor.c
        ${CDT_SOURCE_DIR}/dt_arithmetic.c
        ${CDT_SOURCE_DIR}/dt_char.c
        ${CDT_SOURCE_DIR}/dt_core.c
        ${CDT_SOURCE_DIR}/dt_dow.c
        ${CDT_SOURCE_DIR}/dt_easter.c
        ${CDT_SOURCE_DIR}/dt_length.c
        ${CDT_SOURCE_DIR}/dt_navigate.c
        ${CDT_SOURCE_DIR}/dt_parse_iso.c
        ${CDT_SOURCE_DIR}/dt_search.c
        ${CDT_SOURCE_DIR}/dt_tm.c
        ${CDT_SOURCE_DIR}/dt_util.c
        ${CDT_SOURCE_DIR}/dt_valid.c
        ${CDT_SOURCE_DIR}/dt_weekday.c
        ${CDT_SOURCE_DIR}/dt_workday.c
        ${CDT_SOURCE_DIR}/dt_zone.c
)

INCLUDE_DIRECTORIES(${CDT_SOURCE_DIR})

target_sources(cxproc PUBLIC ${CDT_SOURCE_FILES})

target_sources(cxproc-cgi PUBLIC ${CDT_SOURCE_FILES})

target_sources(cxproc-test PUBLIC ${CDT_SOURCE_FILES})

