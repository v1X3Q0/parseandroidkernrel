cmake_minimum_required(VERSION 3.15.0)
project(parseandroidkernel VERSION 0.1.0)

if(NOT DEFINED ANDROID_TOOL_BASE)
    message(WARNING "need ANDROID_TOOL_BASE")
    set(ANDROID_TOOL_BASE ${CMAKE_CURRENT_SOURCE_DIR})

    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        set(MY_HOST_COMPILER linux)
    elseif (${CMAKE_SYSTEM_NAME} MATCHES "Android")
        set(MY_HOST_COMPILER linux)
    endif()

    if (NOT DEFINED ARCH)
        set(ARCH arm64)
    endif()

    set(NOLIVE 1)
    if (NOT DEFINED SV_GPL)
        set(SV_GPL 1)
    endif()
    set(CMAKE_CXX_STANDARD 17)
    
    # libraries
    add_subdirectory(${ANDROID_TOOL_BASE}/common/hde_armv8)
    add_subdirectory(${ANDROID_TOOL_BASE}/common/hde_x86)
    add_subdirectory(${ANDROID_TOOL_BASE}/common/c_reginst)
    add_subdirectory(${ANDROID_TOOL_BASE}/common/local_util)

    # kernel bonus adds
    # kernel and linux parsers
    add_subdirectory(${ANDROID_TOOL_BASE}/common/bgrep_c)
    add_subdirectory(${ANDROID_TOOL_BASE}/linux/helpers/scripts/ksym_gpl)
    add_subdirectory(${ANDROID_TOOL_BASE}/common/kern_img/common)
    if (${CMAKE_SYSTEM_NAME} MATCHES "Android")
        add_subdirectory(${ANDROID_TOOL_BASE}/common/kern_img)
    endif()
endif()

if (NOT DEFINED ANDROID_DEVICE)
    set(ANDROID_DEVICE PIXEL3A)
    message(WARNING "No ANDROID_DEVICE specified, defaulting to value ${ANDROID_DEVICE}")
endif()
if (NOT DEFINED ANDROID_OS)
    set(ANDROID_OS 11)
    message(WARNING "No ANDROID_OS specified, defaulting to value ${ANDROID_OS}")
endif()

add_subdirectory(parseAndKern)
# executables
add_subdirectory(lz42lz4)
add_subdirectory(uncrc)
add_subdirectory(count_ksymtabstr)

add_subdirectory(patchCrc)
add_dependencies(patchCrc ksym_gpl)
