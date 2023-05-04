project(patchCrc)

add_executable(patchCrc main.cpp patchCrc.cpp iterate_dir.cpp)

# for the kernel image parsing
set(INCLUDE_LIST
    ${ANDROID_TOOL_BASE}/common/c_reginst
    ${ANDROID_TOOL_BASE}/common/local_util
    ${ANDROID_TOOL_BASE}/common/bgrep_c
    ../parseAndKern
    ${ANDROID_TOOL_BASE}/linux/rwDrv/include
    ${ANDROID_TOOL_BASE}/common/kern_img/linux
    ${ANDROID_TOOL_BASE}/common/kern_img/common
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    list(APPEND INCLUDE_LIST /usr/include/json-c)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    list(APPEND INCLUDE_LIST ${ANDROID_TOOL_BASE}/common/local_util/linux_compat)
endif()

target_include_directories(patchCrc PRIVATE ${INCLUDE_LIST})


# for the android kernel static image parser
target_link_libraries(patchCrc PRIVATE
    parseAndKern kern_linux kernel_block
    # for the instruction parsing
    # c_reginst
    hdeA64 localUtil bgrep_e)
