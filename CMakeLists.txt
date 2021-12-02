cmake_minimum_required(VERSION 3.0.0)
project(parseandroidkernel VERSION 0.1.0)

if(NOT DEFINED ANDROID_TOOL_BASE)
    message(FATAL_ERROR "need ANDROID_TOOL_BASE")
endif()

add_definitions(-DDYN_DISASM)
include_directories(${ANDROID_TOOL_BASE}/common/hde_armv8 ${ANDROID_TOOL_BASE}/common/local_util ${ANDROID_TOOL_BASE}/common/c_reginst)
add_subdirectory(${ANDROID_TOOL_BASE}/common/hde_armv8 ${CMAKE_BINARY_DIR}/hde_armv8)
add_subdirectory(${ANDROID_TOOL_BASE}/common/c_reginst ${CMAKE_BINARY_DIR}/c_reginst)
add_subdirectory(${ANDROID_TOOL_BASE}/common/local_util ${CMAKE_BINARY_DIR}/local_util)

add_subdirectory(parseAndKern)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
