project(vxworksraw)

set(INCLUDE_LIBS
    ${ANDROID_TOOL_BASE}/common/kern_img/common
    ${ANDROID_TOOL_BASE}/common/kern_img/linux
    ${ANDROID_TOOL_BASE}/common/local_util
    ${ANDROID_TOOL_BASE}/common/bgrep_c
    ${ANDROID_TOOL_BASE}/linux/helpers/tools/parseAndKern
    ${ANDROID_TOOL_BASE}/linux/rwDrv/include
)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    list(APPEND INCLUDE_LIBS ${ANDROID_TOOL_BASE}/common/local_util/linux_compat)
endif()

add_library(${PROJECT_NAME} SHARED vxworks_parse.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_LIBS})

target_link_libraries(${PROJECT_NAME} PRIVATE parseAndKern kernel_block)