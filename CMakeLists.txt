cmake_minimum_required(VERSION 3.0.0)
project(parseandroidkernel VERSION 0.1.0)

if(NOT DEFINED ANDROID_TOOL_BASE)
    message(FATAL_ERROR "need ANDROID_TOOL_BASE")
endif()

add_definitions(-DDYN_DISASM)

# includes
include_directories(${ANDROID_TOOL_BASE}/common/hde_armv8 ${ANDROID_TOOL_BASE}/common/local_util ${ANDROID_TOOL_BASE}/common/c_reginst)
include_directories(${ANDROID_TOOL_BASE}/linux/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/parseAndKern)

add_subdirectory(${ANDROID_TOOL_BASE}/common/hde_armv8 ${CMAKE_BINARY_DIR}/hde_armv8)
add_subdirectory(${ANDROID_TOOL_BASE}/common/c_reginst ${CMAKE_BINARY_DIR}/c_reginst)
add_subdirectory(${ANDROID_TOOL_BASE}/common/local_util ${CMAKE_BINARY_DIR}/local_util)

# libraries
add_subdirectory(parseAndKern)

# executables
add_subdirectory(ki_to_vmlinux)
add_subdirectory(patchCrc)
add_subdirectory(uncrc)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
