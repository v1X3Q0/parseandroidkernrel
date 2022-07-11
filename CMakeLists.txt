cmake_minimum_required(VERSION 3.0.0)
project(parseandroidkernel VERSION 0.1.0)

if(NOT DEFINED ANDROID_TOOL_BASE)
    message(FATAL_ERROR "need ANDROID_TOOL_BASE")
endif()

# includes
# include_directories(${ANDROID_TOOL_BASE}/common/hde_armv8 ${ANDROID_TOOL_BASE}/common/local_util ${ANDROID_TOOL_BASE}/common/c_reginst)
# include_directories(${ANDROID_TOOL_BASE}/common/bgrep_c)
# # this include is just forsome of the kernel stuff... not really necessary for x86 host tooling but have it anyway for includes sake
# include_directories(${ANDROID_TOOL_BASE}/common/krw_util ${ANDROID_TOOL_BASE}/linux/include ${ANDROID_TOOL_BASE}/linux/targkrw)
# # finally, includes for the kern_img stuff
# include_directories(${ANDROID_TOOL_BASE}/common/kernel_block ${ANDROID_TOOL_BASE}/linux/kern_img)
# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/parseAndKern)
# # python generator for gpl map
add_subdirectory(${ANDROID_TOOL_BASE}/linux/helpers/scripts/ksym_gpl ${CMAKE_BINARY_DIR}/ksym_gpl)
# include_directories(${CMAKE_BINARY_DIR}/ksym_gpl)

# libraries
add_subdirectory(${ANDROID_TOOL_BASE}/common/hde_armv8 ${CMAKE_BINARY_DIR}/hde_armv8)
add_subdirectory(${ANDROID_TOOL_BASE}/common/c_reginst ${CMAKE_BINARY_DIR}/c_reginst)
add_subdirectory(${ANDROID_TOOL_BASE}/common/local_util ${CMAKE_BINARY_DIR}/local_util)
add_subdirectory(parseAndKern)
# kernel bonus adds
# add_subdirectory(${ANDROID_TOOL_BASE}/common/krw_util ${CMAKE_BINARY_DIR}/krw_util)
# add_subdirectory(${ANDROID_TOOL_BASE}/linux/targkrw ${CMAKE_BINARY_DIR}/targkrw)
# add_subdirectory(${ANDROID_TOOL_BASE}/linux/drv_krw ${CMAKE_BINARY_DIR}/drv_krw)
# kernel and linux parsers
add_subdirectory(${ANDROID_TOOL_BASE}/common/kern_img ${CMAKE_BINARY_DIR}/kern_img)
add_subdirectory(${ANDROID_TOOL_BASE}/common/bgrep_c ${CMAKE_BINARY_DIR}/bgrep_c)

# executables
add_subdirectory(lz42lz4)
add_subdirectory(ki_to_vmlinux)
add_subdirectory(patchCrc)
add_dependencies(patchCrc ksym_gpl)
add_subdirectory(uncrc)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
