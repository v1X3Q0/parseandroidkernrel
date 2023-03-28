project(parseAndKern)

set(INCLUDE_LIST
    ${ANDROID_TOOL_BASE}/common/local_util
    ${ANDROID_TOOL_BASE}/common/hdeA64
    ${ANDROID_TOOL_BASE}/common/c_reginst
    ${ANDROID_TOOL_BASE}/common/bgrep_c
    ${ANDROID_TOOL_BASE}/common/kern_img/linux
    ${ANDROID_TOOL_BASE}/common/kern_img/common
    ${ANDROID_TOOL_BASE}/linux/rwDrv/include
    ${ANDROID_TOOL_BASE}/common/krw_util/krw_util
    /usr/include/json-c
    ${CMAKE_BINARY_DIR}/ksym_gpl)

add_library(${PROJECT_NAME} STATIC plk_core.cpp plk_kcrc_track.cpp)

add_dependencies(${PROJECT_NAME} ksym_gpl)

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_LIST})
