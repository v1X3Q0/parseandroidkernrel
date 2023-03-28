cmake_minimum_required(VERSION 3.0.0)
project(parseandroidkernel VERSION 0.1.0)

if(NOT DEFINED ANDROID_TOOL_BASE)
    message(FATAL_ERROR "need ANDROID_TOOL_BASE")
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(MY_HOST_COMPILER linux)
endif()

if (NOT DEFINED ARCH)
    set(ARCH arm64)
endif()

if (NOT DEFINED ANDROID_DEVICE)
    set(ANDROID_DEVICE PIXEL3A)
    message(WARNING "No ANDROID_DEVICE specified, defaulting to value ${ANDROID_DEVICE}")
endif()
if (NOT DEFINED ANDROID_OS)
    set(ANDROID_OS 11)
    message(WARNING "No ANDROID_OS specified, defaulting to value ${ANDROID_OS}")
endif()

set(NOLIVE 1)

# ===========================================
# ============ toolchain arch ===============
# ===========================================
if (ARCH MATCHES "arm64" OR ARCH MATCHES "arm64e" OR ARCH MATCHES "aarch64")
    add_definitions(-DSUPPORT_AARCH64=1)
# I believe windows does AMD64 and linux does x86_64
elseif(ARCH MATCHES "AMD64" OR ARCH MATCHES "i386" OR ARCH MATCHES "x86_64")
    add_definitions(-DSUPPORT_X86_64=1)
endif()

add_subdirectory(${ANDROID_TOOL_BASE}/linux/helpers/scripts/ksym_gpl ${CMAKE_BINARY_DIR}/ksym_gpl)

# includes
# # python generator for gpl map
if (NOT DEFINED POSTEC_PLURAL)
    message("am top project, gonna add deps")

    # libraries
    add_subdirectory(${ANDROID_TOOL_BASE}/common/hde_armv8 ${CMAKE_BINARY_DIR}/hde_armv8)
    add_subdirectory(${ANDROID_TOOL_BASE}/common/c_reginst ${CMAKE_BINARY_DIR}/c_reginst)
    add_subdirectory(${ANDROID_TOOL_BASE}/common/local_util ${CMAKE_BINARY_DIR}/local_util)

    # kernel bonus adds
    # kernel and linux parsers
    add_subdirectory(${ANDROID_TOOL_BASE}/common/kern_img ${CMAKE_BINARY_DIR}/kern_img)
    add_subdirectory(${ANDROID_TOOL_BASE}/common/bgrep_c ${CMAKE_BINARY_DIR}/bgrep_c)
endif()

add_subdirectory(parseAndKern)
# executables
add_subdirectory(lz42lz4)
add_subdirectory(ki_to_vmlinux)
add_subdirectory(patchCrc)
add_dependencies(patchCrc ksym_gpl)
add_subdirectory(uncrc)
add_subdirectory(count_ksymtabstr)
