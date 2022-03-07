add_executable(ki_to_vmlinux main.cpp)
# for the kernel image parsing
target_link_libraries(ki_to_vmlinux PRIVATE parseAndKern kernel_block kern_linux)
target_link_libraries(ki_to_vmlinux PRIVATE bgrep_e c_reginst hdeA64 localUtil)
# for now i've added these kernel dependencies, considering splitting them.
# target_link_libraries(ki_to_vmlinux PRIVATE krw_util targkrw drv_krw)
target_include_directories(ki_to_vmlinux PRIVATE
    ${ANDROID_TOOL_BASE}/common/c_reginst
    ${ANDROID_TOOL_BASE}/common/local_util
    ${ANDROID_TOOL_BASE}/common/bgrep_c
    ../parseAndKern
    ${ANDROID_TOOL_BASE}/linux/rwDrv/include
    ${ANDROID_TOOL_BASE}/common/kern_img/linux
    ${ANDROID_TOOL_BASE}/common/kern_img/common
    /usr/include/json-c)
