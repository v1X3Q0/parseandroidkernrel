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
    ${CMAKE_BINARY_DIR}/ksym_gpl)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    list(APPEND INCLUDE_LIST /usr/include/json-c)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    list(APPEND INCLUDE_LIST /usr/local/include/json-c ${ANDROID_TOOL_BASE}/common/local_util/linux_compat)
endif()

add_library(${PROJECT_NAME} STATIC plk_core.cpp plk_kcrc_track.cpp kern_static.cpp)

add_dependencies(${PROJECT_NAME} ksym_gpl)

if (DEFINED SV_GPL)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        SV_GPL="${SV_GPL}")
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_LIST})

add_subdirectory(vxworks)
