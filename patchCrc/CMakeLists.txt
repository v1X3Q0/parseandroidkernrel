project(patchCrc)

add_executable(patchCrc main.cpp patchCrc.cpp iterate_dir.cpp)

# for the kernel image parsing
# target_link_libraries(patchCrc PRIVATE kernel_block kern_img)
target_include_directories(patchCrc PRIVATE
    ${ANDROID_TOOL_BASE}/common/c_reginst
    ${ANDROID_TOOL_BASE}/common/local_util
    ${ANDROID_TOOL_BASE}/common/bgrep_c
    ../parseAndKern
    ${ANDROID_TOOL_BASE}/linux/rwDrv/include
    ${ANDROID_TOOL_BASE}/common/kern_img/linux
    ${ANDROID_TOOL_BASE}/common/kern_img/common
    /usr/include/json-c)

# for the android kernel static image parser
target_link_libraries(patchCrc PRIVATE
    parseAndKern kern_linux kernel_block
    # for the instruction parsing
    c_reginst hdeA64 localUtil bgrep_e)
